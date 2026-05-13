#include "kstring.h"

uint64_t k_strlen(const char *str) {
    uint64_t len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

uint8_t k_strcmp(const char *cmp1, const char *cmp2) {
    while (*cmp1 && *cmp2 && *cmp1 == *cmp2) {
        cmp1++;
        cmp2++;
    }
    return *cmp1 - *cmp2;
}

uint8_t k_str_startwith(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) {
            return FALSE;
        }
    }
    return TRUE;
}
