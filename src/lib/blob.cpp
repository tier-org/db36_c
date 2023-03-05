#include <fcntl.h>
#include <string.h>

#include <lib/utils.h>
#include <lib/blob.h>

namespace db36 {
namespace lib {

uint64_t Blob::calc_slot(char *key) {
    uint64_t key_i = 0;
    memcpy(&key_i, key, DB36_UINT64_BYTES_COUNT);
    key_i = key_i << this->shift >> this->shift;
    return key_i;
}

int Blob::init_params() {
    this->keyBitsSize = this->keySize << 3;
    this->recordsCount = DB36_ONE; // init value to shift it
    if (this->capacity) { // if capacity is not 0
        if (this->keyBitsSize > this->capacity) {
            this->shift = DB36_UINT64_BITS_COUNT - this->capacity;
        }
        this->recordsCount <<= this->capacity;
        this->recordSize = this->keySize + this->valueSize;
    } else {
        this->recordsCount <<= this->keyBitsSize;
        this->recordSize = this->valueSize;
    }
    this->capacitySize = this->recordSize * this->recordsCount;
    return DB36_STATUS_OK;
}

int Blob::init_file() {
    this->dir = strdup(this->path);
    this->dir[strrchr(this->path, DB36_PATH_DELIM) - this->path] = DB36_STR_TERM;

    this->fd = open(this->path, O_RDWR|O_CREAT, DB36_BLOB_DEFAULT_PERM);
    if (this->fd < 0) {
        return DB36_STATUS_BLB_ENOENT;
    }

    posix_fallocate(this->fd, 0, this->capacitySize);
    this->file = fdopen(this->fd, DB36_BLOB_FD_OPEN_MODE);

    return DB36_STATUS_OK;
}

}}