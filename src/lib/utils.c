#include <stdio.h>
#include <lib/utils.h>

void db36_utils_btox(char *xp, char *bb, int n) {
    const char xx[]= "0123456789ABCDEF";
    while (--n >= 0) xp[n] = xx[(bb[n>>1] >> ((1 - (n&1)) << 2)) & 0xF];
}

void db36_utils_btox_print(char *bb, int bytesLen) {
    int n = bytesLen << 1;
    char hexstr[n + 1];
    db36_utils_btox(hexstr, bb, n);
    hexstr[n] = 0;
    printf("%s\n", hexstr);
}

