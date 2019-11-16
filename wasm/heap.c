/*
 * mm-seglist.c
 *
 * This implementation of malloc uses an explicit-segregated-list. We perform
 * a number of optimizations including storing our free-list pointers as
 * uint32_t offsets from mem_heap_lo(). In addition, we replace our header
 * with a single uint32_t integer which represents the size of the block.
 * 2^32 bytes should be more than enough for any allocation needs. We utilize
 * the lowest byte - which will always be zero for a 4 or 8 byte aligned
 * address - as the 'free' marker. We maintain alignment by adding a 32 bit
 * offset to the front of the heap and to ensure that two headers will.
 */

#include "stdint.h"
#include "heap.h"

#define SEGREGATED_SLOT_COUNT 16
#define ALIGNMENT 4

struct list_node* first[SEGREGATED_SLOT_COUNT];


void* mem_sbrk(uintptr_t inc) {
    void *brk = (void *) (__builtin_wasm_memory_size(0) * PAGE_SIZE);
    __builtin_wasm_memory_grow(0, (inc / PAGE_SIZE) + 1);
    return brk;
}

void* mem_heap_lo(void) {
    return &__heap_base;
}

void* mem_heap_hi(void) {
    return (void*)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
}

int mem_heap_doctor(void) {
    return 1;
}

int mm_init(void) {
    return 0;
}

void* malloc(size_t size) {
    return mem_sbrk(size);
}

void* realloc(void *p, size_t size) {
    void *n = malloc(size);
    memcpy(n, p, size);
    return n;
}
void free(void *p) {

}