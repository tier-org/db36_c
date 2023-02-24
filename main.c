#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>

#define one 1
#define pathDelim '/'
#define strTerm '\0'

#define DB36_C_OK 0
#define DB36_C_BLB_ENOENT 166

struct blob {
    //
    // parameters, set before init
    //
    char *path;
    char *dir;
    unsigned char keySize; // bytes
    unsigned char valueSize; // bytes
    unsigned char capacity; // 2 ** capacity records, 0 means full range mapping
    //
    // calc values are not set directly, just using a constructor
    //
    FILE *file;
    int fd;
    struct stat *filestat;
    unsigned short keyBitsSize; // number of bits of a key
    unsigned short recordSize; // size of one record: valueSize or keySize + valueSize
    unsigned long long recordsCount; // count of records
    unsigned long long capacitySize; // size of bytes allocated for the whole blob  
    unsigned short shift;
};

int init_params(struct blob *bl) {
    bl->keyBitsSize = bl->keySize;
    bl->keyBitsSize = bl->keyBitsSize << 3;
    bl->recordsCount = one; // init value to shift it
    if (bl->capacity) { // if capacity is not 0
        if (bl->keyBitsSize > bl->capacity) {
            bl->shift = bl->keyBitsSize - bl->capacity;
        }
        bl->recordsCount = bl->recordsCount << bl->capacity;
        bl->recordSize = bl->keySize + bl->valueSize;
    } else {
        bl->recordsCount = bl->recordsCount << bl->keyBitsSize;
        bl->recordSize = bl->valueSize;
    }
    bl->capacitySize = bl->recordSize * bl->recordsCount;
    return DB36_C_OK;
}

int init_file(struct blob *bl) {
    bl->dir = strdup(bl->path);
    bl->dir[strrchr(bl->path, pathDelim) - bl->path] = strTerm;

    bl->fd = open(bl->path, O_RDWR|O_CREAT, 0666);
    if (bl->fd < 0) {
        return DB36_C_BLB_ENOENT;
    }

    posix_fallocate(bl->fd, 0, bl->capacitySize);
    bl->file = fdopen(bl->fd, "w");

    return DB36_C_OK;
}


int main() {
    int s = DB36_C_OK;
    struct blob *b1 = calloc(one, sizeof *b1);

    b1->path = strdup("/db36_c/data/tests/test.bl");
    b1->keySize = 4;
    b1->valueSize = 4;
    b1->capacity = 16;

    if ((s = init_params(b1)) != DB36_C_OK) {
        return s;
    }
    if ((s = init_file(b1)) != DB36_C_OK) {
        return s;
    }

    printf("%s\n", b1->path);
    printf("%s\n", b1->dir);
    printf("%u\n", b1->keySize);
    printf("%u\n", b1->shift);
    printf("%lld\n", b1->recordsCount);
    printf("%d\n", b1->recordSize);

    return 0;
}
