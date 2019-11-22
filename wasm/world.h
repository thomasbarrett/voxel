#include "stdint.h"
#include "include/physics_object.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

/*
 * Represents a 16x256x16 
 * 
 */
enum block_t { AIR = 0x0, GRASS, STONE, GOLD };

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
    aabb3_t *selection;
    float theta;
    float phi;
    float projection_matrix[4][4];
    dyn_aabb3_t player;
    struct chunk_t* chunks[CHUNK_CAPACITY];
};

/*
 * Constructor for voxel world.
 * Initializes an empty voxel world with the default chunk capacity.
 */
struct world_t* world_init() {
    struct world_t *self = (struct world_t*) malloc(sizeof(struct world_t));
    vec3_init(&self->player.base.position, 0, 220, 0.0);
    vec3_init(&self->player.base.size, 0.5, 2.0, 0.5);
    vec3_init(&self->player.velocity, 0, 0, 0);
    self->chunk_count = 0;
    self->selection = NULL;
    self->theta = 0;
    self->phi = 0;
    return self;
}

/*
 * Destructor for voxel world.
 * Free all memory allocated by constructor.
 */
void world_destroy(struct world_t *self) {
    free(self);
}

dyn_aabb3_t *world_get_player(struct world_t *self) {
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

int world_break_block(struct world_t *self, int x, int y, int z) {
    int chunkX = floor((float) x / CHUNK_SIZE);
    int chunkZ = floor((float) z / CHUNK_SIZE);
    struct chunk_t *chunk = world_get_chunk(self, chunkX, chunkZ);
    chunk_set(chunk, x - chunkX * CHUNK_SIZE, y, z - chunkZ * CHUNK_SIZE, AIR);

}

int world_get_chunk_count(struct world_t *self) {
    return self->chunk_count;
}

void world_set_theta(struct world_t *self, float theta) {
    self->theta = theta;
}

void world_set_phi(struct world_t *self, float phi) {
    self->phi = phi;
}

float world_get_theta(struct world_t *self) {
    return self->theta;
}

float world_get_phi(struct world_t *self) {
    return self->phi;
}

float* world_get_projection_matrix(struct world_t *self, float aspect) {
    float fov = 45 * 3.14159 / 180;   // in radians
    float near = 0.1;
    float far = 5000.0;
    float rotation_x_matrix[4][4];
    float rotation_y_matrix[4][4];
    float translate_matrix[4][4];
    mat4_projection(fov, near, far, aspect, (float *) &self->projection_matrix);
    mat4_rotate_x(self->phi, (float *) &rotation_x_matrix);
    mat4_rotate_y(self->theta, (float *) &rotation_y_matrix);
    mat4_translate(-self->player.base.position.x, -self->player.base.position.y, -self->player.base.position.z, (float *) &translate_matrix);

    mat4_multiply(rotation_x_matrix, self->projection_matrix, self->projection_matrix);
    mat4_multiply(rotation_y_matrix, self->projection_matrix, self->projection_matrix);
    mat4_multiply(translate_matrix, self->projection_matrix, self->projection_matrix);
    return (float*) &self->projection_matrix;
}

struct chunk_t* world_get_chunk_by_index(struct world_t *self, int i) {
    return self->chunks[i];
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

aabb3_t *world_ray_intersect(ray3_t *ray, struct world_t *self) {
    aabb3_t *min_block = NULL;
    float min_time = 1E6;
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        aabb3_t* blocks = chunk_get_physics_objects(c);
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            float time = ray_intersects(ray, block);
            if (time < min_time) {
                min_time = time;
                min_block = block;
            }
        }
    }
    
    if (min_time < 1E6) {
        self->selection = min_block;
    }
    return min_block;
}

int world_update(struct world_t *self, float dt, int f, int b, int l, int r, int u) {
    ray3_t ray;
    ray.position = self->player.base.position;
    vec3_init(&ray.direction, sin(3.14159 - self->theta) * cos(self->phi), -sin(self->phi), cos(3.14159 - self->theta) * cos(self->phi));   
    world_ray_intersect(&ray, self);
    
    int bottom = 0;
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        aabb3_t* blocks = chunk_get_physics_objects(c);
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            if (aabb3_intersects(block, (aabb3_t *) &self->player)) {
                bottom = bottom || aabb3_resolve_collision(block, &self->player);
            }
        }
    }

    vec3_t velocity;
    vec3_t velocity_left;
    vec3_init(&velocity, 0, 0, 8);
    vec3_rotate_y(&velocity, -self->theta, &velocity);
    vec3_rotate_y(&velocity, 1.57, &velocity_left);
    if (f) {
        self->player.base.position.x -= dt * velocity.x;
        self->player.base.position.z -= dt * velocity.z;
    }
    
    if (b) {
        self->player.base.position.x += dt * velocity.x;
        self->player.base.position.z += dt * velocity.z;
    }
    
    if (l) {
        self->player.base.position.x -= dt * velocity_left.x;
        self->player.base.position.z -= dt * velocity_left.z;
    }
    
    if (r) {
        self->player.base.position.x += dt * velocity_left.x;
        self->player.base.position.z += dt * velocity_left.z;
    }
    if (u && bottom) {
        self->player.velocity.y = 12;
    }

    if (!bottom) {
      self->player.base.position.y += dt * self->player.velocity.y;
      self->player.velocity.y -= dt * 20;
    } else if (self->player.velocity.y < 0) {
      self->player.velocity.y = 0.0;
    }

    int center_x = self->player.base.position.x / 2 / CHUNK_SIZE;
    int center_z = self->player.base.position.z / 2 / CHUNK_SIZE;

    for (int chunk_x = -VISIBLE_CHUNK_RADIUS; chunk_x < VISIBLE_CHUNK_RADIUS; chunk_x++) {
        for (int chunk_z = -VISIBLE_CHUNK_RADIUS; chunk_z < VISIBLE_CHUNK_RADIUS; chunk_z++) {
            struct chunk_t *chunk = world_get_chunk(self, chunk_x + center_x, chunk_z + center_z);
            if (chunk == NULL) {
                chunk = chunk_init(self, chunk_x + center_x, chunk_z + center_z, 0);
                chunk_update_buffers(chunk);
                self->chunks[self->chunk_count] = chunk;
                self->chunk_count++;
            }
        }
    }
    return bottom;
}

void world_click_handler(struct world_t *self) {
    if (self->selection != NULL) {
        int x = self->selection->position.x / 2;
        int y = self->selection->position.y / 2;
        int z = self->selection->position.z / 2;
        world_break_block(self, x, y, z);
    }
}

void world_move_handler(struct world_t *self, float dx, float dy) {
    self->theta += dx * 3.14159;
    self->phi += dy * 3.14159;
}