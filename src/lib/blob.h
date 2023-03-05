#include <stdio.h>
#include <inttypes.h>

#ifndef DB36_LIB_BLOB_H
#define DB36_LIB_BLOB_H

#define DB36_BLOB_DEFAULT_PERM 0666
#define DB36_BLOB_FD_OPEN_MODE "w"

struct db36_blob {
    //
    // parameters, set before init
    //
    char *path;
    char *dir;
    uint8_t keySize; // bytes
    uint8_t valueSize; // bytes
    uint8_t capacity; // 2 ** capacity records, 0 means full range mapping
    //
    // calc values are not set directly, just using a constructor
    //
    FILE *file;
    int fd;
    struct stat *filestat;
    uint16_t keyBitsSize; // number of bits of a key
    uint16_t recordSize; // size of one record: valueSize or keySize + valueSize
    uint64_t recordsCount; // count of records
    uint64_t capacitySize; // size of bytes allocated for the whole blob  
    uint16_t shift;
};

uint64_t db36_blob_calc_slot(struct db36_blob *bl, char *key);
     int db36_blob_init_params(struct db36_blob *bl);
     int db36_blob_init_file(struct db36_blob *bl);

#endif // DB36_LIB_BLOB_H