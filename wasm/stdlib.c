#include "stdlib.h"

int memcpy(void *a, void *b, size_t n) {
    uint8_t *ptr1 = (uint8_t*) a;
    uint8_t *ptr2 = (uint8_t*) b;
    for (size_t i = 0; i < n; i++) {
        *(uint8_t*)(ptr1) = *(uint8_t*)(ptr2);
        ptr1++;
        ptr2++;
    }
}

int memset(void *a, char b, size_t n) {
    uint8_t *ptr1 = (uint8_t*) a;
    for (size_t i = 0; i < n; i++) {
        *(ptr1) = b;
        ptr1++;
    }
}
