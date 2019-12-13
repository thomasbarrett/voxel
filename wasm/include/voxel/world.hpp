#ifndef WORLD_H
#define WORLD_H

#include <voxel/physics_object.hpp>
#include <voxel/Player.hpp>
#include <voxel/Chunk.hpp>

#define CHUNK_CAPACITY 1024
#define VISIBLE_CHUNK_RADIUS 2
#define PLAYER_COUNT 5
#define MOB_COUNT 10

/*
 * Represents an infinite voxel world composed of chunks.
 * Initially a world has no chunks, but up to CHUNK_CAPACITY chunks can be
 * dynamically added to the world.
 */
struct World {
public:
    int chunk_count;
    mat4_t projection_matrix;
    Player player;
    Chunk* chunks[CHUNK_CAPACITY];
    Player players[PLAYER_COUNT];
    Player mobs[MOB_COUNT];
public:
    World();
};

/**
 * Constructs a new voxel world. 
 * The world is allocated on the heap. The world should be destroyed with
 * world_destroy after use.
 */
extern "C" struct World* world_init();

/**
 * Released resources allocated by the world.
 * After this function is called, the world will no longer be valid. This
 * should be called once the world is no longer in use.
 */
void world_destroy(struct World *self);

Chunk* world_get_chunk(struct World *self, int x, int z);
void world_break_block(struct World *self, int x, int y, int z);
int world_get_chunk_count(struct World *self);
float* world_get_projection_matrix(struct World *self, float aspect);
Chunk* world_get_chunk_by_index(struct World *self, int i);
int world_set_chunk(struct World *self, int x, int z, Chunk *chunk);
aabb3_t *world_ray_intersect(ray3_t *ray, struct World *self);

extern "C" int world_update(struct World *self, float dt, int f, int b, int l, int r, int u);
extern "C" void world_click_handler(struct World *self);
extern "C" void world_move_handler(struct World *self, float dx, float dy);

#endif /* WORLD_H */