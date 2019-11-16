#include "stdint.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

/*
 * Represents a 16x256x16 
 * 
 */
struct chunk_t {
    struct world_t *world;
    uint8_t blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    int32_t chunk_x;
    int32_t chunk_z;
    float *vertex_buffer;
    float *index_buffer;
    float *normal_buffer;
    float *texture_buffer;
    uint8_t update;
};

#define CHUNK_CAPACITY 1024

/*
 * Represents an infinite voxel world composed of chunks.
 * Initially a world has no chunks, but up to CHUNK_CAPACITY chunks can be
 * dynamically added to the world.
 */
struct world_t {
    int chunk_count;
    struct chunk_t* chunks[CHUNK_CAPACITY];
};

/*
 * Constructor for voxel world.
 * Initializes an empty voxel world with the default chunk capacity.
 */
struct world_t* world_init() {
    struct world_t *self = (struct world_t*) malloc(sizeof(struct world_t));
    self->chunk_count = 0;
    return self;
}

/*
 * Destructor for voxel world.
 * Free all memory allocated by constructor.
 */
void world_destroy(struct world_t *self) {
    free(self);
}

/*
 * Retrieves the chunk at the given coordinates.
 * Currently, this function is implemented as a linear search.
 * A more optimized approach should be taken to improve time complexity,
 * especially as number of chunks increases. As a short term alternative,
 * we could make this a 'move-to-front' list which 'caches' recently accessed
 * chunks by storing them at the front of the list.
 */
struct chunk_t* world_get_chunk(struct world_t *self, int x, int z) {
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        if (c->chunk_x == x && c->chunk_z == z) {
            return c;
        }
    }
    return NULL;
}

/*
 * Inserts a chunk at the given coordinates.
 * If the maximum chunk capacity has been exceeded, the function returns the
 * error code -1. Otherwise, the chunk is inserted in the map and returns 0.
 */
int world_set_chunk(struct world_t *self, int x, int z, struct chunk_t *chunk) {
    if (self->chunk_count < CHUNK_CAPACITY) {
        self->chunks[self->chunk_count] = chunk;
        self->chunk_count++;
        return 0;
    } else {
        return -1;
    }
}