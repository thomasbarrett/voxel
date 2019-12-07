#ifndef CHUNK_UPDATE_H
#define CHUNK_UPDATE_H

#include <stdint.h>

typedef struct chunk_entry {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t block;
} chunk_entry_t;

/**
 * A chunk_update contains a highly compressed representation of a modified
 * chunk. Rather than storing an entire chunk, this data structure contains a
 * list of all blocks which have been modified from the deterministically
 * generated chunk. This should be sufficiently compact to send
 * over a network for the case of multiplayer games.
 */
typedef struct chunk_update {
    uint16_t size;
    chunk_entry_t data[];
} chunk_update_t;

#endif /* CHUNK_UPDATE_H */