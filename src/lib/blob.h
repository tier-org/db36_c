#include <stdio.h>
#include <inttypes.h>

#ifndef DB36_LIB_BLOB_H
#define DB36_LIB_BLOB_H

#define DB36_BLOB_DEFAULT_PERM 0666
#define DB36_BLOB_FD_OPEN_MODE "w"

namespace db36 {
namespace lib {

class blob {
public:
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

    blob(std::string path, uint8_t capacity, uint8_t keySize, uint8_t valueSize);

    uint64_t calc_slot(char *key);
         int init_params();
         int init_file();
};

}}

#endif // DB36_LIB_BLOB_H