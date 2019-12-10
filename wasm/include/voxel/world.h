#ifndef WORLD_H
#define WORLD_H

#include <voxel/physics_object.h>
#include <voxel/player.h>
#include <voxel/chunk_update.h>
#include <voxel/chunk.hpp>

#define CHUNK_CAPACITY 1024
#define VISIBLE_CHUNK_RADIUS 2
#define PLAYER_COUNT 5

/*
 * Represents an infinite voxel world composed of chunks.
 * Initially a world has no chunks, but up to CHUNK_CAPACITY chunks can be
 * dynamically added to the world.
 */
struct world_t {
    int chunk_count;
    mat4_t projection_matrix;
    player_t player;
    player_t players[PLAYER_COUNT];
    struct chunk_t* chunks[CHUNK_CAPACITY];
};

/**
 * Constructs a new voxel world. 
 * The world is allocated on the heap. The world should be destroyed with
 * world_destroy after use.
 */
extern "C" struct world_t* world_init();

/**
 * Released resources allocated by the world.
 * After this function is called, the world will no longer be valid. This
 * should be called once the world is no longer in use.
 */
extern "C" void world_destroy(struct world_t *self);

extern "C" float world_get_x(struct world_t *self);
extern "C" float world_get_y(struct world_t *self);
extern "C" float world_get_z(struct world_t *self);

extern "C" player_t *world_get_player(struct world_t *self, int index);
extern "C" struct chunk_t* world_get_chunk(struct world_t *self, int x, int z);
extern "C" void world_break_block(struct world_t *self, int x, int y, int z);
extern "C" int world_get_chunk_count(struct world_t *self);
extern "C" float* world_get_projection_matrix(struct world_t *self, float aspect);
extern "C"  struct chunk_t* world_get_chunk_by_index(struct world_t *self, int i);
extern "C" int world_set_chunk(struct world_t *self, int x, int z, struct chunk_t *chunk);
extern "C" aabb3_t *world_ray_intersect(ray3_t *ray, struct world_t *self);
extern "C" int world_update(struct world_t *self, float dt, int f, int b, int l, int r, int u);
extern "C" void world_click_handler(struct world_t *self);
extern "C" void world_move_handler(struct world_t *self, float dx, float dy);

#endif /* WORLD_H */