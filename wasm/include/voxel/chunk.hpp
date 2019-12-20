#ifndef VOXEL_CHUNK_HPP
#define VOXEL_CHUNK_HPP

#include <libc/stdint.hpp>
#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Matrix.hpp>

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

float max(float a, float b);
extern int block_texture_index[][6][2];

class Block {
public:
    static voxel::Mesh* blocks[256];

    enum Value: uint8_t { 
        Air, Stone, Grass, Dirt, CobbleStone, WoodenPlanks,
        Gold, Iron, Coal, Wood, Leaves, Water = 252
    };

    Block() = default;
    constexpr Block(Value v): value_{v} {};
    constexpr bool operator==(Block b) const { return value_ == b.value_; }
    constexpr bool operator!=(Block b) const { return value_ != b.value_; }
    voxel::Array<float, 2> textureIndex() {
        switch (value_) {
            case Value::Stone:
                return {0, 0};
            case Value::Grass:
                return {1, 0};
            case Value::Dirt:
                return {3, 0};
            case Value::CobbleStone:
                return {4, 0};
            case Value::Water:
                return {12, 15};
            case Value::Wood:
                return {4, 1};
            case Value::Leaves:
                return {8, 1};
            default:
                return {((int) value_) % 16, ((int) value_) / 16};
        }
    };  
    explicit operator int() { return value_; }
private:
    Value value_;
};

struct Chunk {
private:
    World *world;
    Block blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    voxel::ArrayList<aabb3_t> physics_objects_;
    voxel::Mesh mesh;
    bool update_;

private:
    void computeMesh();
    void computePhysicsObjects();

public:

    /**
     * The chunk coordinates.
     * TODO: make private and add getter methods.
     */
    int chunk_x;
    int chunk_z;

    /**
     * Generate a chunk belonging to the given world at the gievn chunk
     * coordinate. Note that one chunk coordinate corresponds to CHUNK_SIZE
     * world coordinates. 
     */
    Chunk(struct World *w, int x, int z, uint32_t seed);

    Chunk(Chunk &&chunk) {
        world = chunk.world;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    blocks[x][y][z] = chunk.blocks[x][y][z];
                }
            }  
        }
        physics_objects_ = (voxel::ArrayList<aabb3_t>&&) chunk.physics_objects_;
        mesh = (voxel::Mesh &&) chunk.mesh;
        update_ = chunk.update_;
    }

    Chunk& operator=(Chunk &&m) = default;

    /**
     * Recompute mesh and list of active physics objects if any block changes
     * have occured since the last update.
     */
    void update();

    int x() {
        return chunk_x;
    }

    int z() {
        return chunk_z;
    }
    
    /**
     * Returns the block at the given local `chunk` coordinates. A valid chunk
     * coordinate is an integer between 0 and CHUNK_SIZE in the x and z
     * dimensions and 0 and CHUNK_HEIGHT in the y dimension.
     */
    Block getBlock(int x, int y, int z) {
        return blocks[x][y][z];
    }

    /**
     * Sets the given block at the given localcoordinates and notifies all
     * chunks which may need to update their mesh to account for new
     * 'visible' blocks.
     */
    Block setBlock(int x, int y, int z, Block b);

    /*
    * Returns true if the block in question is possibly visible to the player
    * and false otherwise. There is room for optimization in this method, as it
    * shouldn't be necessary to compute the visibility of every single block in
    * a chunk after an update is made.
    * 
    * The current implementation is correct and complete, but still relatively
    * innefficient.
    */
    int isBlockVisible(int x, int y, int z) ;

    /**
     * Return a reference to the list of active physics objects. This list does
     * not contain a physics object for every block... it only contains physics
     * objects for visible blocks - block which touch at least one transparent
     * block such as air or water.
     */
    voxel::ArrayList<aabb3_t>& physicsObjects() {
        return physics_objects_;
    }

    /**
     * Draws the chunk from the perspective of the given projection matrix.
     * Since chunks do not move, the model-view matrix is simply an identity
     * matrix.
     */
    void draw(mat4_t *projection) {
        voxel::Matrix identity = voxel::Matrix::identity();
        mesh.draw((mat4_t *) &identity, projection);
    }
};

#endif /* VOXEL_CHUNK_HPP */
