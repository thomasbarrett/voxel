#include <libc/stdint.h>
#include <libc/stdlib.h>
#include <voxel/physics_object.h>
#include <voxel/world.h>
#include <voxel/browser.h>
#include <server/message.h>
#include <voxel/graphics.h>
#include "voxel/Mesh.hpp"

/*
 * The dim(6) is block face, in the order defined in cube.
 * top 
 */
int block_texture_index[][6][2] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 2, 0, 1, 0, 3, 0, 2, 0, 2, 0,
    3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
    4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0,
    5, 0, 5, 0, 5, 0, 5, 0, 5, 0, 5, 0,
    0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 1, 2, 1, 2, 1,  2, 1, 2, 1, 2, 1,
    4, 1, 4, 1, 3, 1, 3, 1, 4, 1, 4, 1,
    8, 1, 8, 1, 8, 1, 8, 1, 8, 1, 8, 1
};

aabb3_t* chunk_get_physics_objects(struct chunk_t *self);
void chunk_update_buffers(struct chunk_t *self);
uint8_t chunk_set(struct chunk_t *self, int x, int y, int z, enum block_t b);
struct chunk_t* chunk_init(struct world_t *w, int x, int z, uint32_t seed);

float world_get_x(struct world_t *self) {
    return self->player.physics_object.position.x;
}
float world_get_y(struct world_t *self) {
    return self->player.physics_object.position.y;
}

float world_get_z(struct world_t *self) {
    return self->player.physics_object.position.z;
}

/*
 * Constructor for voxel world.
 * Initializes an empty voxel world with the default chunk capacity.
 */
struct world_t* world_init() {
    struct world_t *self = (struct world_t*) malloc(sizeof(struct world_t));
    vec3_init(&self->player.physics_object.position, 0, 220, 0.0);
    vec3_init(&self->player.physics_object.size, 0.5, 2.0, 0.5);
    vec3_init(&self->player.physics_object.velocity, 0, 0, 0);
    self->chunk_count = 0;
    self->player.selection = NULL;
    self->player.theta = 0;
    self->player.phi = 0;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        vec3_init(&self->players[i].physics_object.position, 10 * i, 110, 10 * i);
        vec3_init(&self->players[i].physics_object.size, 0.5, 2.0, 0.5);
        vec3_init(&self->players[i].physics_object.velocity, 0, 0, 0);
        player_init(&self->players[i]);
    }
    return self;
}

/*
 * Destructor for voxel world.
 * Free all memory allocated by constructor.
 */
void world_destroy(struct world_t *self) {
    free(self);
}

player_t *world_get_player(struct world_t *self, int i) {
    return &self->players[i];
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

void world_set_block(struct world_t *self, int x, int y, int z, int b) {
    int chunkX = floor((float) x / CHUNK_SIZE);
    int chunkZ = floor((float) z / CHUNK_SIZE);
    struct chunk_t *chunk = world_get_chunk(self, chunkX, chunkZ);
    int lx = x - chunkX * CHUNK_SIZE;
    int ly = y;
    int lz = z - chunkZ * CHUNK_SIZE;
    chunk_set(chunk, lx, ly, lz, AIR);
}

void world_break_block(struct world_t *self, int x, int y, int z) {
    world_set_block(self, x, y, z, AIR);
    block_update_t data;
    data.message = BLOCK_UPDATE;
    data.pid = get_pid();
    data.x = x;
    data.y = y;
    data.z = z;
    data.block = AIR;
    send(&data, sizeof(data));
}

int world_get_chunk_count(struct world_t *self) {
    return self->chunk_count;
}

float* world_get_projection_matrix(struct world_t *self, float aspect) {
    float fov = 45 * 3.14159 / 180;   // in radians
    float near = 0.1;
    float far = 5000.0;
    mat4_t rotation_x_matrix;
    mat4_t rotation_y_matrix;
    mat4_t translate_matrix;
    mat4_projection(fov, near, far, aspect, &self->projection_matrix);
    mat4_rotate_x(self->player.phi, &rotation_x_matrix);
    mat4_rotate_y(self->player.theta, &rotation_y_matrix);
    mat4_translate(-self->player.physics_object.position.x, -self->player.physics_object.position.y, -self->player.physics_object.position.z, &translate_matrix);

    mat4_multiply(&rotation_x_matrix, &self->projection_matrix, &self->projection_matrix);
    mat4_multiply(&rotation_y_matrix, &self->projection_matrix, &self->projection_matrix);
    mat4_multiply(&translate_matrix, &self->projection_matrix, &self->projection_matrix);
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
    
    if (min_time < 10) {
        self->player.selection = min_block;
    } else {
        self->player.selection = NULL;
    }

    return min_block;
}

int world_update(struct world_t *self, float dt, int f, int b, int l, int r, int u) {
    ray3_t ray;
    ray.position = self->player.physics_object.position;
    vec3_init(&ray.direction, sin(3.14159 - self->player.theta) * cos(self->player.phi), -sin(self->player.phi), cos(3.14159 - self->player.theta) * cos(self->player.phi));   
    world_ray_intersect(&ray, self);
    
    vec3_t velocity;
    vec3_t velocity_left;
    vec3_init(&velocity, 0, 0, 8);
    mat4_t rotate_y;

    mat4_rotate_y(-self->player.theta, &rotate_y); 
    mat4_vec3_multiply(&rotate_y, &velocity, &velocity);
    mat4_rotate_y(3.14159 / 2, &rotate_y); 
    mat4_vec3_multiply(&rotate_y, &velocity, &velocity_left);


    if (f) {
        self->player.physics_object.position.x -= dt * velocity.x;
        self->player.physics_object.position.z -= dt * velocity.z;
    }
    
    if (b) {
        self->player.physics_object.position.x += dt * velocity.x;
        self->player.physics_object.position.z += dt * velocity.z;
    }
    
    if (l) {
        self->player.physics_object.position.x -= dt * velocity_left.x;
        self->player.physics_object.position.z -= dt * velocity_left.z;
    }
    
    if (r) {
        self->player.physics_object.position.x += dt * velocity_left.x;
        self->player.physics_object.position.z += dt * velocity_left.z;
    }
    
    self->player.physics_object.position.y += dt * self->player.physics_object.velocity.y;

    int bottom = 0;
    for (int i = 0; i < self->chunk_count; i++) {
        struct chunk_t *c = self->chunks[i];
        aabb3_t* blocks = chunk_get_physics_objects(c);
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            if (aabb3_intersects(block, (aabb3_t *) &self->player)) {
                bottom = bottom || aabb3_resolve_collision(block, &self->player.physics_object);
            }
        }
    }

    if (u && bottom) {
        self->player.physics_object.velocity.y = 12;
    }

    if (!bottom) {
      self->player.physics_object.velocity.y -= dt * 20;
    } else if (self->player.physics_object.velocity.y < 0) {
      self->player.physics_object.velocity.y = 0.0;
    }

    position_update_t data;
    data.message = POSITION_UPDATE;
    data.pid = get_pid();
    data.x = self->player.physics_object.position.x / 2;
    data.y = self->player.physics_object.position.y / 2;
    data.z = self->player.physics_object.position.z / 2;
    send(&data, sizeof(data));

    int center_x = self->player.physics_object.position.x / 2 / CHUNK_SIZE;
    int center_z = self->player.physics_object.position.z / 2 / CHUNK_SIZE;

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
    if (self->player.selection != NULL) {
        int x = self->player.selection->position.x / 2;
        int y = self->player.selection->position.y / 2;
        int z = self->player.selection->position.z / 2;
        world_break_block(self, x, y, z);
    }
}

void world_move_handler(struct world_t *self, float dx, float dy) {
    self->player.theta += dx * 3.14159;
    self->player.phi += dy * 3.14159;
}

void world_message_handler(struct world_t *self, void *message) {
    bulk_update_t *bulk = (bulk_update_t*) message;
    positions_update_t *pos_update = &bulk->position_update;
    for (int p = 0; p < 5; p++) {
        if (get_pid() != p) {
            player_set_position(&self->players[p], pos_update->positions[p][0], pos_update->positions[p][1], pos_update->positions[p][2]);
        }
    }
    block_updates_t *block_update = &bulk->block_updates;
    for (int i = 0; i < block_update->length; i++) {
        world_set_block(self, block_update->updates[i].x, block_update->updates[i].y, block_update->updates[i].z, block_update->updates[i].block);
    }
}

double theta = 0;


void on_animation_frame(struct world_t *world, float dt, float aspect) {
    
    world_get_projection_matrix(world, aspect);
    
    for (int c = 0; c < world->chunk_count; c++) {
        chunk_update_buffers(world->chunks[c]);
        draw_buffer(world->chunks[c]->buffer, &world->projection_matrix);
    }
    
    for (int p = 0; p < 5; p++) {
        player_update_buffers(&world->players[p]);
        draw_buffer(world->players[p].buffer, &world->projection_matrix);
    }

}