#ifndef VOXEL_CHUNK_HPP
#define VOXEL_CHUNK_HPP

#include <stdint.h>
#include <voxel/physics_object.h>
#include <voxel/Mesh.hpp>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

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
    int buffer;
    size_t visible_block_count;
};


void chunk_update_buffers(struct chunk_t *self);
uint8_t chunk_set(struct chunk_t *self, int x, int y, int z, enum block_t b);
struct chunk_t* chunk_init(struct world_t *w, int x, int z, uint32_t seed);
int chunk_visible_block_count(struct chunk_t *self);
/**
 * Updates a chunk by applying a series of block replacements from the 
 * chunk_update parameter. Note that in the case that a chunk_update_t contains multiple updates at the
 * same location, the last update is the only one is applied.
 */
void chunk_apply_update(struct chunk_t *self, chunk_update_t *update);

#endif /* VOXEL_CHUNK_HPP */
