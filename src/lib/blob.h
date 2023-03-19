#include <stdio.h>
#include <cstdint>
#include <unistd.h>

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
    uint_fast16_t keySize; // bytes
    uint_fast16_t valueSize; // bytes
    uint_fast8_t capacity; // 2 ** capacity records in total, 0 means full range mapping
    //
    // calc values are not set directly, just using a constructor
    //
    FILE *file;
    int fd;
    struct stat *filestat;
    uint_fast32_t keyBitsSize; // number of bits of a key
    uint_fast32_t recordSize; // size of one record: valueSize or keySize + valueSize

    uint_fast64_t recordsCount; // count of records
    uint_fast64_t capacitySize; // size of bytes allocated for the whole blob
    uint_fast16_t shift;
    char *keyZero;

    blob(char *path,
         uint_fast8_t capacity,
         uint_fast16_t keySize,
         uint_fast16_t valueSize);
    blob() = default;
    ~blob() = default;

    uint_fast64_t calc_slot(const char *key);
        void print_info();
        void init_params();
        void init_file();
        void read_at(const uint_fast64_t address, char *data);
        void write_at(const uint_fast64_t address, char *data);
        std::pair<uint_fast64_t, uint_fast8_t> get(const char *key, char *value);
        std::pair<uint_fast64_t, uint_fast8_t> set(const char *key, char *value);
};

class blobFileEnoentException: public std::exception {
private:
    char *message;
public:
    blobFileEnoentException(char *msg) : message(msg) {}
    char* what() {
        return message;
    }
};

class blobCollisionCapacityWriteException: public std::exception {
public:
    char* what() {
        return (char*)(std::string("Error during write: couldn't find an address to write data to!").c_str());
    }
};

class blobCollisionCapacityReadException: public std::exception {
public:
    char* what() {
        return (char*)(std::string("Error during read: couldn't find an address to read data from!").c_str());
    }
};

class blobWriteWrongBytesException: public std::exception {
public:
    char* what() {
        return (char*)(std::string("Wrong bytes written!").c_str());
    }
};

class blobReadWrongBytesException: public std::exception {
private:
    // uint_fast64_t *expected;
    // uint_fast64_t *actual;
public:
    // blobReadWrongBytesException(uint_fast64_t *expected,
    //                         uint_fast64_t *actual) :
    //                         expected(expected),
    //                         actual(actual)
    //                         {}
    char* what() {
        return (char*)(std::string("Wrong bytes read!").c_str());
    }
};

}}

#endif // DB36_LIB_BLOB_H