#include "stdint.h"
#include "include/physics_object.h"

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
    aabb3_t *physics_objects;
    uint8_t update;
    size_t visible_block_count;
};

aabb3_t* chunk_get_physics_objects(struct chunk_t *self);

#define CHUNK_CAPACITY 1024

/*
 * Represents an infinite voxel world composed of chunks.
 * Initially a world has no chunks, but up to CHUNK_CAPACITY chunks can be
 * dynamically added to the world.
 */
struct world_t {
    int chunk_count;
    aabb3_t player;
    struct chunk_t* chunks[CHUNK_CAPACITY];
};

/*
 * Constructor for voxel world.
 * Initializes an empty voxel world with the default chunk capacity.
 */
struct world_t* world_init() {
    struct world_t *self = (struct world_t*) malloc(sizeof(struct world_t));
    vec3_init(&self->player.position, 0, 220, 0.0);
    vec3_init(&self->player.size, 0.5, 2.0, 0.5);
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

aabb3_t *world_get_player(struct world_t *self) {
    return &self->player;
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

aabb3_t *world_ray_intersect(float rx, float ry, float rz, float rdx, float rdy, float rdz, struct world_t *self) {
    ray3_t ray;
    ray.position.x = rx;
    ray.position.y = ry;
    ray.position.z = rz;
    ray.direction.x = rdx;
    ray.direction.y = rdy;
    ray.direction.z = rdz;

    aabb3_t *min_block = NULL;
    float min_time = 1E6;
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        aabb3_t* blocks = chunk_get_physics_objects(c);
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            float time = ray_intersects(&ray, block);
            if (time < min_time) {
                min_time = time;
                min_block = block;
            }
        }
    }
    return min_block;
}

int world_update(struct world_t *self) {
    int bottom = 0;
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        aabb3_t* blocks = chunk_get_physics_objects(c);
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            if (aabb3_intersects(block, &self->player)) {
                bottom = bottom || aabb3_resolve_collision(block, &self->player);
            }
        }
    }
    return bottom;
}