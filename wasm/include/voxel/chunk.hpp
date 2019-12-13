#ifndef VOXEL_CHUNK_HPP
#define VOXEL_CHUNK_HPP

#include <libc/stdint.hpp>
#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

enum block_t { 
    AIR = 0x0, 
    STONE, GRASS, DIRT, COBBLE_STONE, WOODEN_PLANKS,
    GOLD, IRON, COAL, WOOD, LEAVES
};

extern int block_texture_index[][6][2];

struct Chunk {
public:
    struct World *world;
    uint8_t blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    int32_t chunk_x;
    int32_t chunk_z;
    aabb3_t *physics_objects;
    voxel::Mesh mesh;
    bool update;
    size_t visible_block_count;
public:
    Chunk(struct World *w, int x, int z, uint32_t seed);
    void updateBuffers();
    uint8_t setBlock(int x, int y, int z, enum block_t b);
    bool isBlockVisible(int x, int y, int z);
    int visibleBlockCount();
};

#endif /* VOXEL_CHUNK_HPP */
