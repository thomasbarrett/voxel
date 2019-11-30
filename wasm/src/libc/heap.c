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

#include <stdint.h>
#include "heap.h"

#define ALIGNMENT 4

/**
 * A block header stores the size of a memory block in bytes.
 * The uppermost bit stores a flag indicating whether or not the
 * block is free. 
 */
typedef size_t header_t;

int header_get_free(header_t *h) {
    return *h >> 31;
}

int header_get_size(header_t *h) {
    return *h & (((unsigned int) -1) >> 1);
}

void header_set_free(header_t *h, unsigned int free) {
    *h = (free << 31) | header_get_size(h);
}

void header_set_size(header_t *h, unsigned int size) {
    *h = (header_get_free(h) << 31) | size;
}

void header_set(header_t *h, unsigned int free, unsigned int size) {
    *h = (free << 31) | size;
}

size_t brk_;

void* mem_heap_lo(void) {
    return &__heap_base;
}

void* mem_heap_hi(void) {
    return (void *) brk_;
}

void* mem_sbrk(size_t size) {
    size_t max = (size_t)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
    if (max - brk_ <= size) {
        __builtin_wasm_memory_grow(0, (max / PAGE_SIZE) + 1);
    }
    void *brk = (void *) brk_;
    brk_ += size;
    return brk;
}

int mem_init(void) {
    brk_ = (size_t)(__builtin_wasm_memory_size(0) * PAGE_SIZE);
    header_t *header = mem_heap_lo();
    header_t *footer = ((header_t *) mem_heap_hi()) - 1;
    size_t block_size = (size_t) footer - (size_t) header + sizeof(header_t);
    header_set(header, 1, block_size);
    header_set(footer, 1, block_size);
    return 1;
}


void split(header_t *header, size_t size) {
    size_t capacity = header_get_size(header);
    
    if (capacity - size > 2 * sizeof(header_t)) {
        header_t *footer = (header_t*)((size_t) header + size) - 1;
        header_t *next_header = footer + 1;
        header_t *next_footer = (header_t*)((size_t) header + capacity) - 1;
        header_set(header, 0, size);
        header_set(footer, 0, size);
        header_set(next_header, 1, capacity - size);
        header_set(next_footer, 1, capacity - size);
    } else {
        header_t *footer = (header_t*)((size_t) header + capacity) - 1;
        header_set_free(header, 0);
        header_set_free(footer, 0);
    }
}

void* malloc(size_t size) {

    header_t *current_header = mem_heap_lo();
    while ((size_t) current_header < (size_t) mem_heap_hi()) {
        int current_free = header_get_free(current_header);
        int current_size = header_get_size(current_header);
        header_t *current_footer = (header_t*)((size_t) current_header + current_size) - 1;
        if (current_free && current_size > size + 2 * sizeof(header_t)) {
            split(current_header, size + 2 * sizeof(header_t));
            return current_header + 1;     
        }
        current_header = current_footer + 1;
    }
    
    header_t *header = mem_sbrk(2 * sizeof(header_t) + size);
    header_t *footer = (header_t *) mem_heap_hi() - 1;
    size_t block_size = (size_t) footer - (size_t) header + sizeof(header_t);
    header_set(header, 0, block_size);
    header_set(footer, 0, block_size);
    return header + 1;
}

void* realloc(void *p, size_t size) {
    if (p != NULL) {
        header_t *header = (header_t*) p - 1;
        size_t existing_block_size = header_get_size(header);
        
        size_t requested_block_size = size + 2 * sizeof(header_t);
        if (requested_block_size <= existing_block_size) {
            return p;
        } else {
            void *n = malloc(size);
            memcpy(n, p, existing_block_size - 2 * sizeof(header_t));
            free(p);
            return n;    
        }
    } else {
        void *ret = malloc(size);
        return ret;
    }
}

void coalesce(header_t *header) {
    size_t size = header_get_size(header);
    header_t *footer = (header_t*)((size_t) header + size) - 1;

    header_t *prev_footer = header - 1;
    size_t prev_size = header_get_size(prev_footer);
    int prev_free = header_get_free(prev_footer);
    header_t *prev_header = (header_t*)((size_t) header - prev_size);

    if (prev_free) {
        size += prev_size;
        header = prev_header;
        header_set_size(header, size);
        header_set_size(footer, size);
    }

    header_t *next_header = footer + 1;
    size_t next_size = header_get_size(next_header);
    int next_free = header_get_free(next_header);
    header_t *next_footer = (header_t*)((size_t) next_header + next_size) - 1;

    if (next_free) {
        size += next_size;
        footer = next_footer;
        header_set_size(header, size);
        header_set_size(footer, size);
    }

    header_set_free(header, 1);
    header_set_free(footer, 1);
}

void free(void *p) {
    if (p != NULL) {
        header_t *header = (header_t*) p - 1;
        coalesce(header);
    }
}