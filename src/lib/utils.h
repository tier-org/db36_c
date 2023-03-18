#ifndef DB36_LIB_UTILS_H
#define DB36_LIB_UTILS_H

#define DB36_ONE 1
#define DB36_PATH_DELIM '/'
#define DB36_STR_TERM '\0'
#define DB36_UINT64_BYTES_COUNT 8
#define DB36_UINT64_BITS_COUNT 64

#define DB36_STATUS_OK 0
#define DB36_STATUS_BLB_ENOENT 166

#define DB36_LL_LOG2(X) ((unsigned)(8 * sizeof(unsigned long long) - __builtin_clzll((X)) - 1))

void db36_utils_btox(char *xp, char *bb, int n);
void db36_utils_btox_print(char *bb, int bytesLen);

#endif // DB36_LIB_UTILS_H