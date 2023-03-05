#include <fcntl.h>
#include <string.h>

#include <lib/utils.h>
#include <lib/blob.h>

uint64_t db36_blob_calc_slot(struct db36_blob *bl, char *key) {
    uint64_t key_i = 0;
    memcpy(&key_i, key, DB36_UINT64_BYTES_COUNT);
    key_i = key_i << bl->shift >> bl->shift;
    return key_i;
}

int db36_blob_init_params(struct db36_blob *bl) {
    bl->keyBitsSize = bl->keySize << 3;
    bl->recordsCount = DB36_ONE; // init value to shift it
    if (bl->capacity) { // if capacity is not 0
        if (bl->keyBitsSize > bl->capacity) {
            bl->shift = DB36_UINT64_BITS_COUNT - bl->capacity;
        }
        bl->recordsCount <<= bl->capacity;
        bl->recordSize = bl->keySize + bl->valueSize;
    } else {
        bl->recordsCount <<= bl->keyBitsSize;
        bl->recordSize = bl->valueSize;
    }
    bl->capacitySize = bl->recordSize * bl->recordsCount;
    return DB36_STATUS_OK;
}

int db36_blob_init_file(struct db36_blob *bl) {
    bl->dir = strdup(bl->path);
    bl->dir[strrchr(bl->path, DB36_PATH_DELIM) - bl->path] = DB36_STR_TERM;

    bl->fd = open(bl->path, O_RDWR|O_CREAT, DB36_BLOB_DEFAULT_PERM);
    if (bl->fd < 0) {
        return DB36_STATUS_BLB_ENOENT;
    }

    posix_fallocate(bl->fd, 0, bl->capacitySize);
    bl->file = fdopen(bl->fd, DB36_BLOB_FD_OPEN_MODE);

    return DB36_STATUS_OK;
}
