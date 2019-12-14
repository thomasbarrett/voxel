#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>
#include <voxel/physics_object.hpp>
#include <voxel/world.hpp>
#include <voxel/Browser.hpp>
#include <server/message.hpp>
#include <voxel/graphics.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Item.hpp>

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

/*
 * Constructor for voxel world.
 * Initializes an empty voxel world with the default chunk capacity.
 */
World* world_init() {
    return new World();
}

World::World() {
    vec3_init(&player.physics_object.position, 0, 220, 0.0);
    vec3_init(&player.physics_object.size, 0.5, 2.0, 0.5);
    vec3_init(&player.physics_object.velocity, 0, 0, 0);
    chunk_count = 0;
    player.selection = NULL;
    player.theta = 0;
    player.phi = 0;

    /*
    for (int i = 0; i < PLAYER_COUNT; i++) {
        vec3_init(&players[i].physics_object.position, 10 * i, 220, 10 * i);
        vec3_init(&players[i].physics_object.size, 0.5, 2.0, 0.5);
        vec3_init(&players[i].physics_object.velocity, 0, 0, 0);
    }
    */

    // Initialize MOB_COUNT pigs in a random location
    for (int i = 0; i < MOB_COUNT; i++) {
        vec3_init(&mobs[i].physics_object.position, (2 * random() - 1) * 50, 220, (2 * random() - 1) * 50);
        vec3_init(&mobs[i].physics_object.size,  0.5, 0.5, 1.0);
    }
}

/*
 * Destructor for voxel world.
 * Free all memory allocated by constructor.
 */
void world_destroy(World *self) {
    free(self);
}


/*
 * Retrieves the chunk at the given coordinates.
 * Currently, this function is implemented as a linear search.
 * A more optimized approach should be taken to improve time complexity,
 * especially as number of chunks increases. As a short term alternative,
 * we could make this a 'move-to-front' list which 'caches' recently accessed
 * chunks by storing them at the front of the list.
 */
Chunk* world_get_chunk(World *self, int x, int z) {
    for (int i = 0; i < self->chunk_count; i++) {
        Chunk *c = self->chunks[i];
        if (c->chunk_x == x && c->chunk_z == z) {
            return c;
        }
    }
    return NULL;
}

Block world_set_block(World *self, int x, int y, int z, Block b) {
    int chunkX = floor((float) x / CHUNK_SIZE);
    int chunkZ = floor((float) z / CHUNK_SIZE);
    Chunk *chunk = world_get_chunk(self, chunkX, chunkZ);
    int lx = x - chunkX * CHUNK_SIZE;
    int ly = y;
    int lz = z - chunkZ * CHUNK_SIZE;
    Block old = chunk->blocks[lx][ly][lz];
    chunk->setBlock(lx, ly, lz, Block::Air);
    return old;
}

void world_break_block(World *self, int x, int y, int z) {
    Block b = world_set_block(self, x, y, z, Block::Air);
    block_update_t data;
    data.message = BLOCK_UPDATE;
    data.pid = get_pid();
    data.x = x;
    data.y = y;
    data.z = z;
    data.block = Block::Air;
    self->items.append(new Item{b, {2 * x, 2 * y, 2 * z}});
    for (int i = 0; i < MOB_COUNT; i++) {
        self->mobs[i].triggerAction(&self->player);
    }
    send(&data, sizeof(data));
}

int world_get_chunk_count(World *self) {
    return self->chunk_count;
}

float* world_get_projection_matrix(World *self, float aspect) {
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

Chunk* world_get_chunk_by_index(World *self, int i) {
    return self->chunks[i];
}

/*
 * Inserts a chunk at the given coordinates.
 * If the maximum chunk capacity has been exceeded, the function returns the
 * error code -1. Otherwise, the chunk is inserted in the map and returns 0.
 */
int world_set_chunk(World *self, int x, int z, Chunk *chunk) {
    if (self->chunk_count < CHUNK_CAPACITY) {
        self->chunks[self->chunk_count] = chunk;
        self->chunk_count++;
        return 0;
    } else {
        return -1;
    }
}

aabb3_t *world_ray_intersect(ray3_t *ray, World *self) {
    aabb3_t *min_block = NULL;
    IntersectionResult min = {IntersectionResult::None, 1E10f, nullptr};
    for (int i = 0; i < self->chunk_count; i++) {
        Chunk *c = self->chunks[i];
        aabb3_t* blocks = c->physics_objects;
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            IntersectionResult res = ray_intersects(ray, block);
            if (res.time() < min.time()) {
                min = res;
                min_block = block;
            }
        }
    }
    
    if (min.time() < 10) {
        self->player.selection = min_block;
    } else {
        self->player.selection = NULL;
    }

    return min_block;
}

void world_physics_update(World *self, Player *p, dyn_aabb3_t *mob, float dt) {
  

    mob->position.x += dt * mob->velocity.x;
    mob->position.y += dt * mob->velocity.y;
    mob->position.z += dt * mob->velocity.z;


    int bottom = 0;
    for (int i = 0; i < self->chunk_count; i++) {
        Chunk *c = self->chunks[i];
        aabb3_t* blocks = c->physics_objects;
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            if (aabb3_intersects(block, (aabb3_t *) mob)) {
                bottom = bottom | aabb3_resolve_collision(block, mob);
            }
        }
    }

    if (!(bottom & 0x1)) {
      mob->velocity.y -= dt * 20;
    } else if (mob->velocity.y < 0) {
      mob->velocity.y = 0.0;
    }

    if ((bottom & ~0x1) && (bottom & 0x1)) {
        p->physics_object.velocity.y = 12;
    }

    if (p->angry) {
        p->target = {
            p->angry_target->physics_object.position.x,
            p->angry_target->physics_object.position.z,
        };
    } else {
        if (random() < 0.005) {
            p->target = {
                mob->position.x + random() * 20 - 10,
                mob->position.z + random() * 20 - 10,
            };
        }
    }
    if (abs(mob->position.x - p->target[0]) > 0.5) {
        mob->velocity.x = (p->target[0] - mob->position.x);
    } else {
        mob->velocity.x = 0;
    }

    if (abs(mob->position.z - p->target[1]) > 0.5) {
        mob->velocity.z = (p->target[1] - mob->position.z);
    } else {
        mob->velocity.z = 0;
    }

    float mag = sqrt(mob->velocity.x * mob->velocity.x + mob->velocity.z * mob->velocity.z);
    if (abs(mag) > 1) {
        mob->velocity.x /= mag;
        mob->velocity.z /= mag;
        mob->velocity.x *= p->velocity;
        mob->velocity.z *= p->velocity;
        p->theta = atan2(mob->velocity.x, mob->velocity.z);
    }
}

int world_update(World *self, float dt, int f, int b, int l, int r, int u) {
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
        Chunk *c = self->chunks[i];
        aabb3_t* blocks = c->physics_objects;
        for (int j = 0; j < c->visible_block_count; j++) {
            aabb3_t *block = &blocks[j];
            if (aabb3_intersects(block, (aabb3_t *) &self->player.physics_object)) {
                bottom = bottom | aabb3_resolve_collision(block, &self->player.physics_object);
            }
        }
    }

    if (u && (bottom & 1)) {
        self->player.physics_object.velocity.y = 12;
    }

    if (!(bottom & 1)) {
      self->player.physics_object.velocity.y -= dt * 20;
    } else if (self->player.physics_object.velocity.y < 0) {
      self->player.physics_object.velocity.y = 0.0;
    }

    for (int i = 0; i < MOB_COUNT; i++) {
        world_physics_update(self, &self->mobs[i], &self->mobs[i].physics_object, dt);
    }

    for (int p = 0; p < self->items.size(); p++) {
        if (self->items[p] == nullptr) {
            continue;
        }

        dyn_aabb3_t *item = &self->items[p]->physics_object;
        item->position.x += dt * item->velocity.x;
        item->position.y += dt * item->velocity.y;
        item->position.z += dt * item->velocity.z;

        if (aabb3_intersects((aabb3_t *) &self->items[p]->physics_object, (aabb3_t *) &self->player.physics_object)) {
            delete self->items[p];
            self->items[p] = nullptr;
        }

        int bottom = 0;
        for (int i = 0; i < self->chunk_count; i++) {
            Chunk *c = self->chunks[i];
            aabb3_t* blocks = c->physics_objects;
            for (int j = 0; j < c->visible_block_count; j++) {
                aabb3_t *block = &blocks[j];
                if (aabb3_intersects(block, (aabb3_t *) item)) {
                    bottom = bottom | aabb3_resolve_collision(block, item);
                }
            }
        }

        if (!(bottom&1)) {
            item->velocity.y -= dt * 20;
        } else if (item->velocity.y < 0) {
            item->velocity.y = 0.0;
        }

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
            Chunk *chunk = world_get_chunk(self, chunk_x + center_x, chunk_z + center_z);
            if (chunk == NULL) {
                chunk = new Chunk(self, chunk_x + center_x, chunk_z + center_z, 0);
                chunk->updateBuffers();
                self->chunks[self->chunk_count] = chunk;
                self->chunk_count++;
            }
        }
    }
    return bottom;
}

void world_click_handler(World *self) {
    if (self->player.selection != NULL) {
        int x = self->player.selection->position.x / 2;
        int y = self->player.selection->position.y / 2;
        int z = self->player.selection->position.z / 2;
        world_break_block(self, x, y, z);
  
    }
}

void world_move_handler(World *self, float dx, float dy) {
    self->player.theta += dx * 3.14159;
    self->player.phi += dy * 3.14159;
}

void world_message_handler(World *self, void *message) {
    bulk_update_t *bulk = (bulk_update_t*) message;
    positions_update_t *pos_update = &bulk->position_update;
    for (int p = 0; p < 5; p++) {
        if (get_pid() != p) {
            self->players[p].setPosition(pos_update->positions[p][0], pos_update->positions[p][1], pos_update->positions[p][2]);
        }
    }
    block_updates_t *block_update = &bulk->block_updates;
    for (int i = 0; i < block_update->length; i++) {
       // world_set_block(self, block_update->updates[i].x, block_update->updates[i].y, block_update->updates[i].z, block_update->updates[i].block);
    }
}

double theta = 0;


void on_animation_frame(World *world, float dt, float aspect) {
    
    world_get_projection_matrix(world, aspect);
    
    for (int c = 0; c < world->chunk_count; c++) {
        world->chunks[c]->updateBuffers();
        mat4_t model_view_matrix;
        mat4_translate(0, 0, 0, &model_view_matrix);
        world->chunks[c]->mesh.draw(&model_view_matrix, &world->projection_matrix);

    }
    
    /*
    for (int p = 0; p < PLAYER_COUNT; p++) {
        mat4_t model_view_matrix = world->players[p].getModelViewMatrix();
        world->players[p].mesh.draw(&model_view_matrix, &world->projection_matrix);
    }
    */

    for (int p = 0; p < MOB_COUNT; p++) {
        world->mobs[p].draw(&world->projection_matrix);
    }


    for (int p = 0; p < world->items.size(); p++) {
        world->items[p]->draw(&world->projection_matrix);
    }

}