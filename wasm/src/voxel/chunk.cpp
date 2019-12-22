#include <libc/stdlib.hpp>
#include <voxel/world.hpp>
#include <voxel/Chunk.hpp>
#include <voxel/cube.hpp>

#define TRUE 1
#define FALSE 0

float max(float a, float b) {
    return a > b ? a: b;
}

/*
 * Initializes a chunk with a simple flat world. This function quite simply fills
 * the bottom 99 blocks with stone, the 100th block with glass, and the rest with 
 * air.
 */
Chunk::Chunk(struct World *w, int x, int z, uint32_t seed) {

    world = w;
    chunk_x = x;
    chunk_z = z;
    update_ = TRUE;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                float block_x = i + x * CHUNK_SIZE;
                float block_y = j;
                float block_z = k + z * CHUNK_SIZE;
                int top = World::elevation(block_x, block_z);
                if (j < top - 10) {
                    blocks[i][j][k] = Block::Stone;
                } else if (j < top - 1) {
                    blocks[i][j][k] = Block::Dirt;
                } else if (j < top) {
                    blocks[i][j][k] = Block::Grass;
                } else if (j < World::sea_level()) {
                    blocks[i][j][k] = Block::Water;
                } else {
                    blocks[i][j][k] = Block::Air;
                }
            }
        }
    }

    for (int t = 0; t < 2; t++) {
        int tx = (int)(14 * random() + 2);
        int tz = (int)(14 * random() + 2);
        float tree_x = tx + x * CHUNK_SIZE;
        float tree_z = tz + z * CHUNK_SIZE;
        int tree_y = World::elevation(tree_x, tree_z);
        if (tree_y <= World::sea_level()) {
            continue;
        }
        for (int i = -2; i <= 2; i++) {
            for (int j = 0; j < 10; j++) {
                for (int k = -2; k <= 2; k++) {
                    if (i == 0 && k == 0  && j < 6) {
                        blocks[tx + i][tree_y + j][tz + k] = Block::Wood;
                    } else if (i * i + k * k + (j-6) * (j-6) < 8) {
                        blocks[tx + i][tree_y + j][tz + k] = Block::Leaves;
                    }
                }
            }
        }
    }


}

/*
 * Sets the block at position (x, y, z) in the chunk to b. 
 * This has the additional side effect of marking 'update'
 * as true.
 */
Block Chunk::setBlock(int x, int y, int z, Block b) {
    update_ = TRUE;
    if (x == 0) {
        Chunk *l = world_get_chunk(world, chunk_x - 1, chunk_z);
        l->update_ = TRUE;

    } else if (x == CHUNK_SIZE - 1) {
        Chunk *l = world_get_chunk(world, chunk_x + 1, chunk_z);
        l->update_ = TRUE;
    }

    if (z == 0) {
        Chunk *l = world_get_chunk(world, chunk_x, chunk_z - 1);
        l->update_ = TRUE;
    } else if (z == CHUNK_SIZE - 1) {
        Chunk *l = world_get_chunk(world, chunk_x, chunk_z + 1);
        l->update_ = TRUE;
    }
    return blocks[x][y][z] = b;
}

int is_block_transparent(Block block, Block b) {
    if (b == Block::Block::Water) {
       return block == Block::Air
            || block == Block::Block::Leaves;
    } else {
        return block == Block::Air
            || block == Block::Block::Leaves
            || block == Block::Block::Water;
    }
  
}

int Chunk::isBlockVisible(int x, int y, int z) {
    
    /*
     * Air blocks are never visible, so we return FALSE in all cases.
     */
    if (blocks[x][y][z] == Block::Air) return Face::None;

    int face = Face::None;

    /*
     * If any of the adjacent blocks in the blocks own chunk are air, then it
     * is possible for the block to be visible.
     */
    if (y != 0 && is_block_transparent(blocks[x][y - 1][z], blocks[x][y][z])) face |= Face::Bottom;
    if (y != CHUNK_HEIGHT - 1 && is_block_transparent(blocks[x][y + 1][z], blocks[x][y][z])) face |= Face::Top;
    if (x != 0 && is_block_transparent(blocks[x - 1][y][z], blocks[x][y][z])) face |= Face::Left;
    if (x != CHUNK_SIZE - 1 && is_block_transparent(blocks[x + 1][y][z], blocks[x][y][z])) face |= Face::Right;
    if (z != 0 && is_block_transparent(blocks[x][y][z - 1], blocks[x][y][z])) face |= Face::Back;
    if (z != CHUNK_SIZE - 1 && is_block_transparent(blocks[x][y][z + 1], blocks[x][y][z])) face |= Face::Front;

    /*
     * If there is an air block block on another chunk adjacent to our block,
     * then our block is visible. This only occurs for blocks on the outer perimeter of the
     * chunk. For corner blocks, both chunks in the x and z directions must be considered.
     * However, a block on the left perimeter will never need to worry about a chunk on the
     * right hand side.
     * 
     * Note that it is possible for an adjacent chunk to not exist as it has not yet been
     * explored by the player. In this case, it is too far away to be seen by the player
     * and will default to hidden.
     */

    if (x == 0) {
        Chunk *l = world_get_chunk(world, chunk_x - 1, chunk_z);
        if (l != NULL && is_block_transparent(l->blocks[CHUNK_SIZE - 1][y][z], blocks[x][y][z])) {
            face |= Face::Left;
        }
    } else if (x == CHUNK_SIZE - 1) {
        Chunk *r = world_get_chunk(world, chunk_x + 1, chunk_z);
        if (r != NULL && is_block_transparent(r->blocks[0][y][z], blocks[x][y][z])) {
            face |= Face::Right;
        }
    }

    if (z == 0) {
        Chunk *t = world_get_chunk(world, chunk_x, chunk_z - 1);
        if (t != NULL && is_block_transparent(t->blocks[x][y][CHUNK_SIZE - 1], blocks[x][y][z])) {
            face |= Face::Back;
        }
    } else if (z == CHUNK_SIZE - 1) {
        Chunk *b = world_get_chunk(world, chunk_x, chunk_z + 1);
        if (b != NULL && is_block_transparent(b->blocks[x][y][0], blocks[x][y][z])) {
            face |= Face::Front;
        }
    }
    return face; 
}

void Chunk::computeMesh() {
    int block_i = 0;

    opaque_mesh.clear();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (is_block_transparent(blocks[x][y][z], Block::Air)) {
                    continue;
                }
                uint8_t visible = isBlockVisible(x, y, z);
                if (visible) {

                    float block_x = x + chunk_x * CHUNK_SIZE;
                    float block_y = y;
                    float block_z = z + chunk_z * CHUNK_SIZE;

                    Block block = blocks[x][y][z];

                    for (int v = 0; v < 24; v++) {
                        Face face = (Face) (1 << (v / 4));
                        opaque_mesh.appendVertex({
                            single_positions[v][0] + 2 * block_x,
                            single_positions[v][1] + 2 * block_y,
                            single_positions[v][2] + 2 * block_z
                        });
                        opaque_mesh.appendTextureCoord({
                            (single_texture_coords[v][0] + block.textureIndex(face)[0]) / 16.0,
                            (single_texture_coords[v][1] + block.textureIndex(face)[1]) / 16.0
                        });
                        opaque_mesh.appendNormal({
                            single_normals[v][0],
                            single_normals[v][1],
                            single_normals[v][2]
                        });
                    }
                    for (int i = 0; i < 6; i++) {
                        int face = 1 << i;
                        if (visible & face) {
                            opaque_mesh.appendFace({
                                single_indices[6 * i] + block_i * 24,
                                single_indices[6 * i + 1] + block_i * 24,
                                single_indices[6 * i + 2] + block_i * 24,
                            });
                            opaque_mesh.appendFace({
                                single_indices[6 * i + 3] + block_i * 24,
                                single_indices[6 * i + 4] + block_i * 24,
                                single_indices[6 * i + 5] + block_i * 24,
                            });
                        }
                    }
                    block_i += 1;
                }
            }
        }
    }
    
    block_i = 0;
    opaque_mesh.update();
    transparent_mesh.clear();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (!is_block_transparent(blocks[x][y][z], Block::Air)) {
                    continue;
                }
                uint8_t visible = isBlockVisible(x, y, z);
                if (visible) {

                    float block_x = x + chunk_x * CHUNK_SIZE;
                    float block_y = y;
                    float block_z = z + chunk_z * CHUNK_SIZE;

                    Block block = blocks[x][y][z];

                    for (int v = 0; v < 24; v++) {
                        Face face = (Face) (1 << (v / 4));

                        transparent_mesh.appendVertex({
                            single_positions[v][0] + 2 * block_x,
                            single_positions[v][1] + 2 * block_y,
                            single_positions[v][2] + 2 * block_z
                        });
                        transparent_mesh.appendTextureCoord({
                            (single_texture_coords[v][0] + block.textureIndex(face)[0]) / 16.0,
                            (single_texture_coords[v][1] + block.textureIndex(face)[1]) / 16.0
                        });
                        transparent_mesh.appendNormal({
                            single_normals[v][0],
                            single_normals[v][1],
                            single_normals[v][2]
                        });
                    }
                    for (int i = 0; i < 6; i++) {
                        int face = 1 << i;
                        if (visible & face) {
                            transparent_mesh.appendFace({
                                single_indices[6 * i] + block_i * 24,
                                single_indices[6 * i + 1] + block_i * 24,
                                single_indices[6 * i + 2] + block_i * 24,
                            });
                            transparent_mesh.appendFace({
                                single_indices[6 * i + 3] + block_i * 24,
                                single_indices[6 * i + 4] + block_i * 24,
                                single_indices[6 * i + 5] + block_i * 24,
                            });
                        }
                    }
                    block_i += 1;
                }
            }
        }
    }
    transparent_mesh.update();

}

void Chunk::computePhysicsObjects() {
    physics_objects_.clear();
    size_t i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t visible = isBlockVisible(x, y, z);
                if (visible && blocks[x][y][z] != Block::Water) {
                    aabb3_t obj;
                    float block_x = x + chunk_x * CHUNK_SIZE;
                    float block_y = y;
                    float block_z = z + chunk_z * CHUNK_SIZE;
                    vec3_init(&obj.position, block_x * 2, block_y * 2, block_z * 2);
                    vec3_init(&obj.size, 1, 1, 1);
                    physics_objects_.append(obj);
                    i += 1;
                }
            }
        }
    }
}

/*
 * Update the buffer fields in the given chunk.
 * This will be called at every frame, so the buffers should only actually be
 * recomputed if there was a block update affecting the chunk. This can be
 * determined from the state of the update flag. This method will set the
 * update flag to false.
 */
void Chunk::update() {

    if (update_) {
        computePhysicsObjects();
        computeMesh();
    }
    update_ = FALSE;
}