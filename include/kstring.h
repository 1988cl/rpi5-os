#ifndef KSTRING
#define KSTRING
#include "ktype.h"
uint64_t k_strlen(const char *str);
uint8_t k_strcmp(const char *cmp1, const char *cmp2);
uint8_t k_str_startwith(const char *str, const char *prefix);
#endif
