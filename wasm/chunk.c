#include "stdlib.h"
#include "world.h"
#include "cube.h"

#define TRUE 1
#define FALSE 0

enum block_t { AIR = 0x0, GRASS, STONE, GOLD };

/*
 * Initializes a chunk with a simple flat world. This function quite simply fills
 * the bottom 99 blocks with stone, the 100th block with glass, and the rest with 
 * air.
 */
struct chunk_t* chunk_init(struct world_t *w, int x, int z, uint32_t seed) {
    struct chunk_t *self = (struct chunk_t*) malloc(sizeof(struct chunk_t));

    /*
     * 
     */
    self->world = w;
    self->chunk_x = x;
    self->chunk_z = z;
    self->vertex_buffer = NULL;
    self->index_buffer = NULL;
    self->normal_buffer = NULL;
    self->texture_buffer = NULL;
    self->update = TRUE;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_HEIGHT; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                if (j < 99) {
                    self->blocks[i][j][k] = STONE;
                } else if (j < 100) {
                    self->blocks[i][j][k] = GRASS;
                } else {
                    self->blocks[i][j][k] = AIR;
                }
            }
        }
    }
    return self;
}

/*
 * Returns the block at position (x, y, z) in the chunk
 */
uint8_t chunk_get(struct chunk_t *self, int x, int y, int z) {
    return self->blocks[x][y][z];
}

/*
 * Sets the block at position (x, y, z) in the chunk to b. 
 * This has the additional side effect of marking 'update'
 * as true.
 */
uint8_t chunk_set(struct chunk_t *self, int x, int y, int z, enum block_t b) {
    self->update = TRUE;
    if (x == 0) {
        struct chunk_t *l = world_get_chunk(self->world, self->chunk_x - 1, self->chunk_z);
        l->update = TRUE;

    } else if (x == CHUNK_SIZE - 1) {
        struct chunk_t *l = world_get_chunk(self->world, self->chunk_x + 1, self->chunk_z);
        l->update = TRUE;
    }

    if (z == 0) {
        struct chunk_t *l = world_get_chunk(self->world, self->chunk_x, self->chunk_z - 1);
        l->update = TRUE;
    } else if (z == CHUNK_SIZE - 1) {
        struct chunk_t *l = world_get_chunk(self->world, self->chunk_x, self->chunk_z + 1);
        l->update = TRUE;
    }
    return self->blocks[x][y][z] = b;
}


/*
 * Returns TRUE if the block in question is possibly visible to the player and false
 * otherwise. There is room for optimization in this method, as it shouldn't be necessary
 * to compute the visibility of every single block in a chunk after an update is made.
 * 
 * The current implementation is correct and complete, but still relatively innefficient.
 */
uint8_t chunk_is_block_visible(struct chunk_t *self, int x, int y, int z) {

    /*
     * Air blocks are never visible, so we return FALSE in all cases.
     */
    if (self->blocks[x][y][z] == AIR) return FALSE;

    /*
     * If any of the adjacent blocks in the blocks own chunk are air, then it
     * is possible for the block to be visible.
     */
    if ((y != 0 && self->blocks[x][y - 1][z] == AIR)
    ||  (y != CHUNK_HEIGHT - 1 && self->blocks[x][y + 1][z] == AIR)
    ||  (x != 0 && self->blocks[x - 1][y][z] == AIR)
    ||  (x != CHUNK_SIZE - 1 && self->blocks[x + 1][y][z] == AIR)
    ||  (z != 0 && self->blocks[x][y][z - 1] == AIR)
    ||  (z != CHUNK_SIZE - 1 && self->blocks[x][y][z + 1] == AIR)) {
        return TRUE;
    }
    
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
        struct chunk_t *l = world_get_chunk(self->world, self->chunk_x - 1, self->chunk_z);
        if (l != NULL && l->blocks[CHUNK_SIZE - 1][y][z] == AIR) {
            return TRUE;
        }
    } else if (x == CHUNK_SIZE - 1) {
        struct chunk_t *r = world_get_chunk(self->world, self->chunk_x + 1, self->chunk_z);
        if (r != NULL && r->blocks[0][y][z] == AIR) {
            return TRUE;
        }
    }

    if (z == 0) {
        struct chunk_t *t = world_get_chunk(self->world, self->chunk_x, self->chunk_z - 1);
        if (t != NULL && t->blocks[x][y][CHUNK_SIZE - 1] == AIR) {
            return TRUE;
        }
    } else if (z == CHUNK_SIZE - 1) {
        struct chunk_t *b = world_get_chunk(self->world, self->chunk_x, self->chunk_z + 1);
        if (b != NULL && b->blocks[x][y][0] == AIR) {
            return TRUE;
        }
    }
    return FALSE; 
}

/*
 * Returns the number of visible blocks in the chunk. This is a convenience
 * method that can be used to allocate memory for vertex buffers ahead of time.
 */
int chunk_visible_block_count(struct chunk_t *self) {
    size_t i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t visible = chunk_is_block_visible(self, x, y, z);
                if (visible) i += 1;
            }
        }
    }
    return i;
}


float* chunk_compute_vertex_buffer(struct chunk_t *self, size_t visible_vertex_count) {
    float *vertices = (float *) realloc(self->vertex_buffer, 3 * sizeof(float) * 24 * visible_vertex_count);
    size_t i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t visible = chunk_is_block_visible(self, x, y, z);
                if (visible) {
                    float block_x = x + self->chunk_x * CHUNK_SIZE;
                    float block_y = y;
                    float block_z = z + self->chunk_z * CHUNK_SIZE;
                    for (int v = 0; v < 24; v++) {
                        vertices[i + 3 * v + 0] = single_positions[v][0] + 2 * block_x;
                        vertices[i + 3 * v + 1] = single_positions[v][1] + 2 * block_y;
                        vertices[i + 3 * v + 2] = single_positions[v][2] + 2 * block_z;
                    }
                    i += 3 * 24;
                }
            }
        }
    }
    return vertices;
}

float* chunk_get_vertex_buffer(struct chunk_t *self) {
    return self->vertex_buffer;
}

float* chunk_compute_index_buffer(struct chunk_t *self) {
    if (self->update) {

    } else {
        return self->index_buffer;
    }
}

float* chunk_get_index_buffer(struct chunk_t *self) {
    return self->index_buffer;
}

/**
 * Computes the vertex normals for the chunk. 
 * \param visible_block_count the number of visible blocks in chunk
 * \return a pointer to the normal buffer
 */
float* chunk_compute_normal_buffer(struct chunk_t *self, size_t visible_block_count) {
    /* allocate an array of exactly the right size */
    float *normals = (float *) realloc(self->normal_buffer, 3 * sizeof(float) * 24 * visible_block_count);
    size_t normals_length = 0;

    /* iterate through all blocks in chunk */
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {

                /* check whether or not the block is visible */
                if (chunk_is_block_visible(self, x, y, z)) {
                    
                    /* add another copy of normals to array */
                    for (int v = 0; v < 24; v++) {
                        for (int w = 0; w < 3; w++) {
                            normals[normals_length + 3 * v + w] = single_normals[v][w];
                        }
                    }

                    normals_length += 3 * 24;
                }
            }
        }
    }
    return normals;
}

float* chunk_get_normal_buffer(struct chunk_t *self) {
    return self->normal_buffer;
}

float* chunk_compute_texture_buffer(struct chunk_t *self) {
    if (self->update) {

    } else {
        return self->texture_buffer;
    }
}

/*
 * Return the 
 */
float* chunk_get_texture_buffer(struct chunk_t *self) {
    return self->texture_buffer;
}



aabb3_t* chunk_compute_physics_objects(struct chunk_t *self, size_t visible_vertex_count) {
    aabb3_t *objects = (aabb3_t *) realloc(self->physics_objects, sizeof(aabb3_t) * visible_vertex_count);
    size_t i = 0;
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t visible = chunk_is_block_visible(self, x, y, z);
                if (visible) {
                    float block_x = x + self->chunk_x * CHUNK_SIZE;
                    float block_y = y;
                    float block_z = z + self->chunk_z * CHUNK_SIZE;
                    vec3_init(&objects[i].position, block_x * 2, block_y * 2, block_z * 2);
                    vec3_init(&objects[i].size, 1, 1, 1);
                    i += 1;
                }
            }
        }
    }
    return objects;
}

aabb3_t* chunk_get_physics_objects(struct chunk_t *self) {
    return self->physics_objects;
}

/*
 * Update the buffer fields in the given chunk.
 * This will be called at every frame, so the buffers should only actually be
 * recomputed if there was a block update affecting the chunk. This can be
 * determined from the state of the update flag. This method will set the
 * update flag to false.
 */
void chunk_update_buffers(struct chunk_t *self) {
    if (self->update) {
        size_t visible_block_count = chunk_visible_block_count(self);
        self->visible_block_count = visible_block_count;
        self->vertex_buffer = chunk_compute_vertex_buffer(self, visible_block_count);
        self->normal_buffer = chunk_compute_normal_buffer(self, visible_block_count);
        self->physics_objects = chunk_compute_physics_objects(self, visible_block_count);
    }
    self->update = FALSE;
}