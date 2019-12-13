#ifndef STDLIB_H
#define STDLIB_H

#include <libc/stdint.hpp>
#include <libc/heap.hpp>

extern uint8_t __heap_base;
 
extern "C" int memcpy(void *a, void *b, size_t n);
extern "C" int memset(void *a, char b, size_t n);

#define TRUE 1
#define FALSE 0
#define NULL 0

#endif /* STDLIB_H */