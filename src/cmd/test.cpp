#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <time.h>
#include <lib/utils.h>
#include <lib/blob.h>
// #include <iterator>

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    int s = DB36_STATUS_OK;

    db36::lib::blob b1 = db36::lib::blob("/db36_c/data/tests/test.bl", 24, 16, 1);

    // struct db36_blob *b1 = (db36_blob *)calloc(DB36_ONE, sizeof *b1);

    b1.path = strdup("/db36_c/data/tests/test.bl");
    b1.keySize = 16; // bytes
    b1.valueSize = 1;
    b1.capacity = 24;

    if ((s = b1.init_params()) != DB36_STATUS_OK) {
        return s;
    }
    if ((s = b1.init_file()) != DB36_STATUS_OK) {
        return s;
    }


    printf("%s\n", b1.path);
    printf("%s\n", b1.dir);
    printf("%u\n", b1.keySize);
    printf("keybits size: %u\n", b1.keyBitsSize);
    printf("capacity: %u\n", b1.capacity);
    printf("shift: %u\n", b1.shift);
    printf("%ld\n", b1.recordsCount);
    printf("%d\n", b1.recordSize);


    int bytesLen = 16;
    char *key = (char *)calloc(bytesLen, sizeof *key);

    key[0] = 0xFF;
    key[1] = 0xEE;
    key[2] = 0xDD;
    key[3] = 0xCC;
    key[4] = 0xBB;
    key[5] = 0xAA;
    key[6] = 0x99;
    key[7] = 0x88;
    key[8] = 0x77;
    key[9] = 0x66;
    key[10] = 0x55;
    key[11] = 0x44;
    key[12] = 0x33;
    key[13] = 0x22;
    key[14] = 0x11;
    key[15] = 0x00;

    srand(time(NULL));

    for (int i = 0; i < bytesLen; i++){
        key[i] = rand() % 255;
    }

    for(int i=0; i<bytesLen; ++i)
        printf("%x", key[i]);

    // std::cout << std::format("key is {:x};\n", key);

    printf("\n");

    db36_utils_btox_print(key, bytesLen);

    uint64_t key_i = b1.calc_slot(key);


    for(int i = 0; i < 100000; i++){
        for (int i = 0; i < 15; i++){
            key[i] = rand() % 255;
        }
        key_i = b1.calc_slot(key);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("took %lu us\n", delta_us);

    printf("key_i: %lu\n", key_i);
    printf("key_i: %lX\n", key_i);

    return 0;
}


