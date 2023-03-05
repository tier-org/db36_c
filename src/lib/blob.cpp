#include <fcntl.h>
#include <iostream>
#include <cstring>

#include <lib/utils.h>
#include <lib/blob.h>

namespace db36 {
namespace lib {

blob::blob(std::string path, uint8_t capacity, uint8_t keySize, uint8_t valueSize) {

}

uint64_t blob::calc_slot(char *key) {
    uint64_t key_i = 0;
    memcpy(&key_i, key, DB36_UINT64_BYTES_COUNT);
    key_i = key_i << shift >> shift;
    return key_i;
}

int blob::init_params() {
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
    return DB36_STATUS_OK;
}

int blob::init_file() {
    dir = strdup(path);
    dir[strrchr(path, DB36_PATH_DELIM) - path] = DB36_STR_TERM;

    fd = open(path, O_RDWR|O_CREAT, DB36_BLOB_DEFAULT_PERM);
    if (fd < 0) {
        return DB36_STATUS_BLB_ENOENT;
    }

    posix_fallocate(fd, 0, capacitySize);
    file = fdopen(fd, DB36_BLOB_FD_OPEN_MODE);

    return DB36_STATUS_OK;
}

}}