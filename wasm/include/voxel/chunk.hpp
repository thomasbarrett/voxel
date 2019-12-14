#ifndef VOXEL_CHUNK_HPP
#define VOXEL_CHUNK_HPP

#include <libc/stdint.hpp>
#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

float max(float a, float b);
extern int block_texture_index[][6][2];

class Block {
public:
    enum Value: uint8_t { 
        Air, Stone, Grass, Dirt, CobbleStone, WoodenPlanks,
        Gold, Iron, Coal, Wood, Leaves
    };

    Block() = default;
    constexpr Block(Value v): value_{v} {};
    constexpr bool operator==(Block b) const { return value_ == b.value_; }
    constexpr bool operator!=(Block b) const { return value_ != b.value_; }\
    explicit operator int() { return value_; }
private:
    Value value_;
};

struct Chunk {
public:
    struct World *world;
    Block blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    int32_t chunk_x;
    int32_t chunk_z;
    aabb3_t *physics_objects;
    voxel::Mesh mesh;
    bool update;
    size_t visible_block_count;
public:
    Chunk(struct World *w, int x, int z, uint32_t seed);
    void updateBuffers();
    Block setBlock(int x, int y, int z, Block b);
    bool isBlockVisible(int x, int y, int z);
    int visibleBlockCount();
};

#endif /* VOXEL_CHUNK_HPP */
