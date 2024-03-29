#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>

#include <lib/utils.h>
#include <lib/blob.h>

namespace db36 {
namespace lib {

blob::blob(char *path,
           uint8_t capacity,
           uint16_t keySize,
           uint16_t valueSize) {
    this->path = path;
    this->keySize = keySize;
    this->valueSize = valueSize;
    this->capacity = capacity;
    init_params();
    init_file();
}

void blob::print_info() {
    printf("[*] Blob %s in directory %s is initialized for:\n"
           "\tKey size: %u bytes (%u bits)\n"
           "\tCapacity: 2^%u\n"
           "\tShift: %u bits\n"
           "\tRecords count: %ld\n"
           "\tRecord size: %u bytes\n",
           path, dir, keySize, keyBitsSize, capacity, shift, recordsCount, recordSize);
}

uint64_t blob::calc_slot(const char *key) {
    uint64_t key_i = 0;
    memcpy(&key_i, key, DB36_UINT64_BYTES_COUNT);
    key_i = key_i << shift >> shift;
    return key_i;
}

void blob::read_at(const uint64_t address, char *data) {
    if (pread(fd, data, recordSize, address * recordSize) - recordSize != 0){
        throw blobReadWrongBytesException();
    }
    // posix_fadvise(fd, address * recordSize, recordSize, POSIX_FADV_DONTNEED);
}

void blob::write_at(const uint64_t address, char *data) {
    if (pwrite(fd, data, recordSize, address * recordSize) - recordSize != 0){
        throw blobWriteWrongBytesException();
    }
    // posix_fadvise(fd, address * recordSize, recordSize, POSIX_FADV_DONTNEED);
}

std::pair<uint64_t, uint8_t> blob::get(const char *key, char *value) {
    uint64_t address = calc_slot(key);
    uint8_t iters = 0;

    if (capacity) {
        char *data = (char *)calloc(recordSize, sizeof *data);
        while (++iters <= capacity) {
            read_at(address, data);
            if (std::memcmp(data, key, keySize) == 0) {
                memcpy(value, &data[keySize], valueSize);
                break;
            }
            address++;
        }
        free(data);
        if (iters > capacity) {
            throw blobCollisionCapacityReadException();
        }
    } else {
        iters++;
        read_at(address, value);
    }
    return std::make_pair(address, iters);
}

std::pair<uint64_t, uint8_t> blob::set(const char *key, char *value) {
    uint64_t address = calc_slot(key);
    uint8_t iters = 0;

    if (capacity) {
        char *data = (char *)calloc(recordSize, sizeof *data);
        while (++iters <= capacity) {
            read_at(address, data);
            if (
                (memcmp(data, key, keySize) == 0) ||
                (memcmp(data, keyZero, keySize) == 0)
            ) {
                memcpy(data, key, keySize);
                memcpy(&data[keySize], value, valueSize);
                write_at(address, data);
                break;
            }

            address++;
        }
        free(data);
        if (iters > capacity) {
            throw blobCollisionCapacityWriteException();
        }
    } else {
        iters++;
        write_at(address, value);
    }
    return std::make_pair(address, iters);
}

void blob::init_params() {
    keyBitsSize = keySize << 3;
    recordsCount = DB36_ONE; // init value to shift it
    if (capacity) { // if capacity is not 0
        if (keyBitsSize > capacity) {
            shift = DB36_UINT64_BITS_COUNT - capacity;
        }
        recordsCount <<= capacity;
        recordSize = keySize + valueSize;
    } else {
        recordsCount <<= keyBitsSize;
        recordSize = valueSize;
    }
    capacitySize = recordSize * recordsCount;
    keyZero = (char *)calloc(keySize, sizeof *keyZero);
}

void blob::init_file() {
    dir = strdup(path);
    dir[strrchr(path, DB36_PATH_DELIM) - path] = DB36_STR_TERM;

    fd = open(path, O_RDWR|O_CREAT|O_SYNC, DB36_BLOB_DEFAULT_PERM);

    if (fd < 0)
        throw blobFileEnoentException();

    if (stat(path, &fstat) == -1)
        throw blobFileStatReadException();

    if (fstat.st_size == 0) {
        posix_fallocate(fd, 0, capacitySize);
        if (stat(path, &fstat) == -1)
            throw blobFileStatReadException();
    }

    if (capacitySize - fstat.st_size > 0)
        throw blobFileSizeException();

    posix_fadvise(fd, 0, capacitySize, POSIX_FADV_RANDOM);
}

}}