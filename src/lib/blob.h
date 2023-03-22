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
    uint16_t keySize = 0; // bytes
    uint16_t valueSize = 0; // bytes
    uint8_t capacity = 0; // 2 ** capacity records in total, 0 means full range mapping
    //
    // calc values are not set directly, just using a constructor
    //
    int fd;
    struct stat *filestat;
    uint32_t keyBitsSize = 0; // number of bits of a key
    uint32_t recordSize = 0; // size of one record: valueSize or keySize + valueSize

    uint64_t recordsCount = 0; // count of records
    uint64_t capacitySize = 0; // size of bytes allocated for the whole blob
    uint16_t shift = 0;
    char *keyZero;

    blob(char *path,
         uint8_t capacity,
         uint16_t keySize,
         uint16_t valueSize);
    blob() = default;
    ~blob() = default;

    uint64_t calc_slot(const char *key);
        void print_info();
        void init_params();
        void init_file();
        void read_at(const uint64_t address, char *data);
        void write_at(const uint64_t address, char *data);
        std::pair<uint64_t, uint8_t> get(const char *key, char *value);
        std::pair<uint64_t, uint8_t> set(const char *key, char *value);
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
    // uint64_t *expected;
    // uint64_t *actual;
public:
    // blobReadWrongBytesException(uint64_t *expected,
    //                         uint64_t *actual) :
    //                         expected(expected),
    //                         actual(actual)
    //                         {}
    char* what() {
        return (char*)(std::string("Wrong bytes read!").c_str());
    }
};

}}

#endif // DB36_LIB_BLOB_H