#ifndef HEAP_H
#define HEAP_H

#include "stdint.h"
#include "stdlib.h"

extern uint8_t __heap_base;

extern "C" int mem_init(void);
extern "C" void *malloc (size_t size);
extern "C" void free (void *ptr);
extern "C" void *realloc(void *ptr, size_t size);
extern "C" void *calloc (size_t nmemb, size_t size);

#endif