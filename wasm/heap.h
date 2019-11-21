#ifndef HEAP_H
#define HEAP_H

#include "stdint.h"
#include "stdlib.h"

extern uint8_t __heap_base;

int putchar(int c);
int puts(char *s);

extern void *malloc (size_t size);
extern void free (void *ptr);
extern void *realloc(void *ptr, size_t size);
extern void *calloc (size_t nmemb, size_t size);
extern int mm_init(void);

#endif