#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include <voxel/physics_object.h>
#include <voxel/player.h>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

/*
 * Represents a 16x256x16 
 * 
 */
enum block_t { 
    AIR = 0x0, 
    STONE, GRASS, DIRT, COBBLE_STONE, WOODEN_PLANKS,
    GOLD, IRON, COAL, WOOD, LEAVES
};

extern int block_texture_index[][6][2];

struct chunk_t {
    struct world_t *world;
    uint8_t blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    int32_t chunk_x;
    int32_t chunk_z;
    float *vertex_buffer;
    unsigned short *index_buffer;
    float *normal_buffer;
    float *texture_buffer;
    aabb3_t *physics_objects;
    uint8_t update;
    size_t visible_block_count;
};

aabb3_t* chunk_get_physics_objects(struct chunk_t *self);
void chunk_update_buffers(struct chunk_t *self);
uint8_t chunk_set(struct chunk_t *self, int x, int y, int z, enum block_t b);
struct chunk_t* chunk_init(struct world_t *w, int x, int z, uint32_t seed);
#define CHUNK_CAPACITY 1024
#define VISIBLE_CHUNK_RADIUS 2

/*
 * Represents an infinite voxel world composed of chunks.
 * Initially a world has no chunks, but up to CHUNK_CAPACITY chunks can be
 * dynamically added to the world.
 */
struct world_t {
    int chunk_count;
    mat4_t projection_matrix;
    player_t player;
    struct chunk_t* chunks[CHUNK_CAPACITY];
};

/**
 * Constructs a new voxel world. 
 * The world is allocated on the heap. The world should be destroyed with
 * world_destroy after use.
 */
struct world_t* world_init();

/**
 * Released resources allocated by the world.
 * After this function is called, the world will no longer be valid. This
 * should be called once the world is no longer in use.
 */
void world_destroy(struct world_t *self);


player_t *world_get_player(struct world_t *self);
struct chunk_t* world_get_chunk(struct world_t *self, int x, int z);
void world_break_block(struct world_t *self, int x, int y, int z);
int world_get_chunk_count(struct world_t *self);
float* world_get_projection_matrix(struct world_t *self, float aspect);
struct chunk_t* world_get_chunk_by_index(struct world_t *self, int i);
int world_set_chunk(struct world_t *self, int x, int z, struct chunk_t *chunk);
aabb3_t *world_ray_intersect(ray3_t *ray, struct world_t *self);
int world_update(struct world_t *self, float dt, int f, int b, int l, int r, int u);
void world_click_handler(struct world_t *self);
void world_move_handler(struct world_t *self, float dx, float dy);

#endif /* WORLD_H */