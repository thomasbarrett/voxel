#ifndef STDLIB_H
#define STDLIB_H

#include "stdint.h"
#include "heap.h"

extern uint8_t __heap_base;
 
int memcpy(void *a, void *b, size_t n);
int memset(void *a, char b, size_t n);

#define TRUE 1
#define FALSE 0
#define NULL 0

#endif /* STDLIB_H */