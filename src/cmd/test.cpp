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

int main() {
    //
    // init helpers to calc time and use random
    //
    struct timespec start, end;
    uint64_t delta_us;
    srand(time(NULL));

    //
    // blob and experiment params
    //
    uint64_t n = 1000;
    uint8_t capacity = 24;
    uint8_t keySize = 16;
    uint8_t valueSize = 4;
    char* fp = strdup("/db36_c/data/tests/test.bl");

    //
    // init and print data
    //
    db36::lib::blob blob = db36::lib::blob(fp, capacity, keySize, valueSize);
    blob.print_info();

    //
    // prepare data
    //
    uint64_t keysSize = keySize * n;
    printf("[*] preparing keys (%lu bytes)...\n", keysSize);
    char* keys = (char *)calloc(keysSize, sizeof *keys);

    uint64_t valuesSize = valueSize * n;
    printf("[*] preparing values (%lu bytes)...\n", valuesSize);
    char* values = (char *)calloc(valuesSize, sizeof *values);

    printf("[*] generating %lu keys...\n", n);
    for (uint64_t i = 0; i < keysSize; i++){
        keys[i] = rand() % 255;
    }

    printf("[*] generating %lu values...\n", n);
    for (uint64_t i = 0; i < valuesSize; i++){
        values[i] = rand() % 255;
    }


    //
    // tmp helpers
    //
    char* key = (char *)calloc(keySize, sizeof *key);
    char* value = (char *)calloc(valueSize, sizeof *value);

    //
    // run write iterations
    //
    printf("[*] run %lu write iterations\n", n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //
    uint64_t offsetKey = 0;
    uint64_t offsetValue = 0;
    uint64_t iterations = 0;
    for (uint64_t i = 0; i < n; i++){
        memcpy(key, &keys[offsetKey], keySize);
        memcpy(value, &values[offsetValue], valueSize);
        auto res = blob.set(key, value);
        iterations += res.second;
        offsetKey += keySize;
        offsetValue += valueSize;
    }
    //
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("[*] took %lu us in %lu iterations\n", delta_us, iterations);

    //
    // run read iterations
    //
    printf("[*] run %lu read iterations\n", n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //
    offsetKey = 0;
    iterations = 0;
    for (uint64_t i = 0; i < n; i++){
        memcpy(key, &keys[offsetKey], keySize);
        auto res = blob.get(key, value);
        iterations += res.second;
        offsetKey += keySize;
    }
    //
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("[*] took %lu us in %lu iterations\n", delta_us, iterations);

    return 0;
}
