#include <fcntl.h>
#include <iostream>
#include <cstring>

#include <lib/utils.h>
#include <lib/blob.h>

namespace db36 {
namespace lib {

blob::blob(char *path,
           uint8_t capacity,
           uint8_t keySize,
           uint8_t valueSize) {
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
           "\tRecord size: %d bytes (2^%d)\n",
           path, dir, keySize, keyBitsSize, capacity, shift, recordsCount, recordSize, recordSizeLn);
}

uint64_t blob::calc_slot(const char *key) {
    uint64_t key_i = 0;
    memcpy(&key_i, key, DB36_UINT64_BYTES_COUNT);
    // key_i = key_i << shift >> shift;
    key_i = key_i >> shift;
    return key_i;
}

void blob::read_at(const uint64_t address, char *data) {
    if (pread(fd, data, recordSize, address * recordSize) != recordSize){
        throw blobReadWrongBytesException();
    }
}

void blob::write_at(const uint64_t address, char *data) {
    if (pwrite(fd, data, recordSize, address * recordSize) != recordSize){
        throw blobWriteWrongBytesException();
    }
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
    recordSizeLn = DB36_LL_LOG2(recordSize);
    capacitySize = recordSize * recordsCount;
    keyZero = (char *)calloc(keySize, sizeof *keyZero);
}

void blob::init_file() {
    dir = strdup(path);
    dir[strrchr(path, DB36_PATH_DELIM) - path] = DB36_STR_TERM;

    fd = open(path, O_RDWR|O_CREAT|O_DIRECT, DB36_BLOB_DEFAULT_PERM);
    if (fd < 0) {
        throw blobFileEnoentException(path);
    }

    posix_fallocate(fd, 0, capacitySize);
    file = fdopen(fd, DB36_BLOB_FD_OPEN_MODE);
}

}}