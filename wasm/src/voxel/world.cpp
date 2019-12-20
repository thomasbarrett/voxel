#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>
#include <voxel/physics_object.hpp>
#include <voxel/world.hpp>
#include <voxel/Browser.hpp>
#include <server/message.hpp>
#include <voxel/graphics.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Item.hpp>
#include <util/Fetch.hpp>
#include <voxel/Perlin.hpp>

voxel::Mesh* Block::blocks[256];
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

voxel::OBJLoader *meshLoader;
voxel::OBJLoader *pigMeshLoader;

int World::sea_level() {
    return 80;
}

int World::elevation(int x, int z) {
    float noise = perlin2d(x , z, 0.05, 3);
    return 10 * noise + sea_level() - 3;
}


World* world_init() {
    meshLoader = new voxel::OBJLoader{"/models/cube.obj"};
    pigMeshLoader = new voxel::OBJLoader{"/models/pig.obj"};
    return new World();
}

World::World(): player{Player{this}} { 
    vec3_init(&player.physics_object.position, 0,  2 * World::elevation(0, 0), 0.0);
    vec3_init(&player.physics_object.size, 0.5, 2.0, 0.5);
    chunk_count = 0;

    // Initialize MOB_COUNT pigs in a random location
    for (int i = 0; i < MOB_COUNT; i++) {
        Player *mob = new Player{this};
        int x = (int)((2 * random() - 1) * 50);
        int z = (int)((2 * random() - 1) * 50);
        vec3_init(&mob->physics_object.position, x, 2 * World::elevation(x, z), z);
        vec3_init(&mob->physics_object.size, 0.75, 1, 1.5);
        mobs_.append(mob);
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
    for (auto &chunk: self->chunks_){
        if (chunk->x() == x && chunk->z() == z) {
            return chunk;
        }
    }
    return nullptr;
}

Block world_set_block(World *self, int x, int y, int z, Block b) {
    int chunkX = floor((float) x / CHUNK_SIZE);
    int chunkZ = floor((float) z / CHUNK_SIZE);
    Chunk *chunk = world_get_chunk(self, chunkX, chunkZ);
    int lx = x - chunkX * CHUNK_SIZE;
    int ly = y;
    int lz = z - chunkZ * CHUNK_SIZE;
    Block old = chunk->getBlock(lx, ly, lz);
    chunk->setBlock(lx, ly, lz, b);
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

    send(&data, sizeof(data));
}

int world_get_chunk_count(World *self) {
    return self->chunk_count;
}

void on_key_press(World *self, int key) {
    if (key == '1') {
        self->player.setCurrentBlock(Block::Stone);
    } else if (key == '2') {
        self->player.setCurrentBlock(Block::Dirt);
    }
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
    return self->chunks_[i];
}


int world_update(World *self, float dt) {

    vec3_t velocity;
    vec3_t velocity_left;
    vec3_init(&velocity, 0, 0, 8);
    mat4_t rotate_y;

    mat4_rotate_y(-self->player.theta, &rotate_y); 
    mat4_vec3_multiply(&rotate_y, &velocity, &velocity);
    mat4_rotate_y(3.14159 / 2, &rotate_y); 
    mat4_vec3_multiply(&rotate_y, &velocity, &velocity_left);

  
    if (is_key_pressed('w')) {
        self->player.physics_object.velocity.x = -velocity.x;
        self->player.physics_object.velocity.z = -velocity.z;
    }
    
    if (is_key_pressed('s')) {
        self->player.physics_object.velocity.x = velocity.x;
        self->player.physics_object.velocity.z = velocity.z;
    }
    
    if (is_key_pressed('a')) {
        self->player.physics_object.velocity.x = -velocity_left.x;
        self->player.physics_object.velocity.z = -velocity_left.z;
    }
    
    if (is_key_pressed('d')) {
        self->player.physics_object.velocity.x = velocity_left.x;
        self->player.physics_object.velocity.z = velocity_left.z;
    }
    
    self->player.physics_object.position.x += dt * self->player.physics_object.velocity.x;
    self->player.physics_object.position.y += dt * self->player.physics_object.velocity.y;
    self->player.physics_object.position.z += dt * self->player.physics_object.velocity.z;

    self->player.physics_object.velocity.y += dt * self->player.physics_object.velocity.y;

    auto pchunk = self->player.chunk();
    int bottom = Face::None;
    for (auto chunk: self->chunks_) {
        if (abs(chunk->x() - pchunk[0]) <= 1 
         || abs(chunk->z() - pchunk[1]) <= 1) {
            for (auto &block: chunk->physicsObjects()) {
                if (aabb3_intersects(&block, (aabb3_t *) &self->player.physics_object)) {
                    bottom = bottom | aabb3_resolve_collision(&block, &self->player.physics_object);
                }
            }
        }
    }


    if ((bottom & Face::Bottom)) {
        self->player.physics_object.velocity.x = 0;
        self->player.physics_object.velocity.z = 0;
    }

    if (is_key_pressed(' ') && (bottom & Face::Bottom)) {
        self->player.physics_object.velocity.y = 10;
    }

    if (!(bottom & Face::Bottom)) {
      self->player.physics_object.velocity.y -= dt * 20;
    } else if (self->player.physics_object.velocity.y < 0) {
      self->player.physics_object.velocity.y = 0.0;
    }

    for (int i = 0; i < self->mobs_.size(); i++) {
        for (int j = i + 1; j < self->mobs_.size(); j++) {
            auto mob1 = self->mobs_[i];
            auto mob2 = self->mobs_[j];
            auto aabb1 = &mob1->physics_object;
            auto aabb2 = &mob2->physics_object;
            auto mchunk1 = mob1->chunk();
            auto mchunk2 = mob2->chunk();
            if (abs(mchunk1[0] - mchunk2[0]) <= 1
            ||  abs(mchunk1[1] - mchunk2[1]) <= 1) {
                if (aabb3_intersects((aabb3_t *) aabb1, (aabb3_t *) aabb2)) {
                    aabb3_resolve_collision((aabb3_t *) aabb1, aabb2);
                }
            }
        }
    }

    for (auto mob: self->mobs_) {
        auto mchunk = mob->chunk();
        if (abs(pchunk[0] - mchunk[0]) <= 1
        ||  abs(pchunk[1] - mchunk[1]) <= 1) {
            if (aabb3_intersects((aabb3_t *) &mob->physics_object, (aabb3_t *) &self->player.physics_object)) {
                aabb3_resolve_collision((aabb3_t *) &mob->physics_object, &self->player.physics_object);
                self->player.physics_object.velocity.x = 2 * mob->physics_object.velocity.x;
                self->player.physics_object.velocity.y = 5;
                self->player.physics_object.velocity.z = 2 * mob->physics_object.velocity.z;
                
                mob->physics_object.velocity.x /= 2;
                mob->physics_object.velocity.z /= 2;

                self->player.health -= 1;
                update_health(self->player.health);
                if (self->player.health <= 0) {
                    game_over();
                }
            }
            mob->update(dt);
        }
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
            self->player.addBlock(self->items[p]->block());
            delete self->items[p];
            self->items[p] = nullptr;
        }

        int bottom = Face::None;
        for (auto &chunk: self->chunks_) {
            auto &blocks = chunk->physicsObjects();
            for (int j = 0; j < blocks.size(); j++) {
                aabb3_t *block = &blocks[j];
                if (aabb3_intersects(block, (aabb3_t *) item)) {
                    bottom = bottom | aabb3_resolve_collision(block, item);
                }
            }
        }

        if (!(bottom & Face::Bottom)) {
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
            if (chunk == nullptr) {
                self->chunks_.append(new Chunk({self, chunk_x + center_x, chunk_z + center_z, 0}));
            }
        }
    }
    return bottom;
}

/**
 * This function is called on mouse click events. Note that the game is run in
 * `pointer lock` mode wherein the mouse is hidden and fixed in the center of
 * the screen. Thus, we consider the center of the screen to be the mouse
 * location.
 * 
 * In this method, we perform selection in our scene by computing the
 * intersection of a ray projected from the center of the screen with the rest
 * of the scene and sort intersections by distance. Game logic is performed
 * based on the nearest intersection.
 */
void world_click_handler(World *self) {
    
    dyn_aabb3_t &player = self->player.physics_object;

    ray3_t ray;
    ray.position = self->player.physics_object.position;
    vec3_init(&ray.direction,
        sin(3.14159 - self->player.theta) * cos(self->player.phi),
        -sin(self->player.phi), cos(3.14159 - self->player.theta) * cos(self->player.phi)
    );   
 
    aabb3_t *min_block;
    Player *min_mob = nullptr;
    IntersectionResult min = {IntersectionResult::None, 1E10f, nullptr};
    
    // Perform ray-cube collision on all block within a radius of 10.
    // TODO: Further optimization is still possible... We perform intersection
    // even on chunks that are extremely far away. As the world grows
    // very large, this will drastically slow down performance.
    for (auto &chunk: self->chunks_) {
        for (auto &block: chunk->physicsObjects()) {
            if (vec3_distance(&player.position, &block.position) < 10) {
                IntersectionResult res = ray_intersects(&ray, &block);
                if (res.time() < min.time()) {
                    min = res;
                    min_block = &block;
                }
            }
        }
    }

    // Perform ray-cube collision on all mobs within a radius of 10.
    // TODO: Further optimization is still possible... We perform intersection
    // even on chunks that are extremely far away. As the world grows
    // very large, this will drastically slow down performance.
    for (auto mob: self->mobs_){
        dyn_aabb3_t &mob_aabb = mob->physics_object;
        if (vec3_distance(&player.position, &mob_aabb.position) < 10) {
            IntersectionResult res = ray_intersects(&ray, (aabb3_t*) &mob_aabb);
            if (res.time() < min.time()) {
                min = res;
                min_block = nullptr;
                min_mob = mob;
            }
        }
    }

    // Prevent the player from selecting any block 
    if (min.time() > 10) {
        return;
    }

    if (min_block) {
        // Compute the `world coordinates` from the physics objects
        int x = min_block->position.x / 2;
        int y = min_block->position.y / 2;
        int z = min_block->position.z / 2;

        // Either place a block or break a block depending on key modifiers...
        if (!is_key_pressed('z')) {
            world_break_block(self, x, y, z);
        } else {
            // The type of block the player is currently holding
            Block b = self->player.getCurrentBlock();

            // If the player has enough blocks of the currently selected type
            if (self->player.removeBlock(b)) {

                // Place a block of the selected type adjacent to the face that
                // was clicked by the player
                switch ((int) min) {
                    case IntersectionResult::Front: 
                        world_set_block(self, x, y, z - 1, b);
                        break;
                    case IntersectionResult::Back:
                        world_set_block(self, x, y, z + 1, b);
                        break;
                    case IntersectionResult::Left:
                        world_set_block(self, x - 1, y, z, b);
                        break;
                    case IntersectionResult::Right:
                        world_set_block(self, x + 1, y, z, b);
                        break;
                    case IntersectionResult::Top:
                        world_set_block(self, x, y + 1, z, b);
                        break;
                    case IntersectionResult::Bottom:
                        world_set_block(self, x, y - 1, z, b);
                        break;
                }  
            }
             
        }

    } else if (min_mob != nullptr) {
        if (!min_mob->angry) {
            for (auto mob: self->mobs_){
                mob->triggerAction(&self->player);
            }
        }
        min_mob->health -= 34;
        if (min_mob->health < 0) {
            min_mob->physics_object.position.x = self->player.physics_object.position.x + 40 * random() - 10;
            min_mob->physics_object.position.y = 220;
            min_mob->physics_object.position.z = self->player.physics_object.position.z + 40 * random() - 10;
        }
       
    }
}

/**
 * This function is called on mouse move events. Note that the game is run in
 * `pointer lock` mode wherein the mouse is hidden and fixed in the center of
 * the screen. Thus, the `screen` is infinite in size and there is never a 
 * worry about the user hitting the edge of the screen.
 * 
 * \param self: the world
 * \param dx: the distance the mouse was moved in the x direction
 * \param dy: the distance the mouse was moved in the y direction
 */
void world_move_handler(World *self, float dx, float dy) {
    self->player.theta += dx * 3.14159;
    self->player.phi += dy * 3.14159;
}

/**
 * This function is called on a message from the server. 
 * Currently, multiplayer functionality is not implemented, so this function
 * does not do anything useful.
 * 
 * TODO: Implement!
 */
void world_message_handler(World *self, void *message) {
   // Does nothing
}

/**
 * This function is called once per frame. 
 * The timing of this function is dependent on the default framerate of 
 * the user, but may be slower if their computer is not able to meet the 
 * computational demands of the game.
 * 
 * This function should perform drawing operations only!
 * Physics operations should all be performed in the `world_update` function.
 */
void on_animation_frame(World *world, float dt, float aspect) {
    
    world_get_projection_matrix(world, aspect);

    // Draw all chunks within a VISIBLE_CHUNK_RADIUS distance measured in
    // taxicab coordinates. 
    auto pchunk = world->player.chunk();
    for (auto &chunk: world->chunks_) {
        if (abs(chunk->x() - pchunk[0]) + abs(chunk->z() - pchunk[1]) <= VISIBLE_CHUNK_RADIUS) {
            chunk->update();
            chunk->draw(&world->projection_matrix);
        }
    }

    // Draw all mobs
    for (auto mob: world->mobs_) {
        mob->draw(&world->projection_matrix);
    }

    // Draw all items
    for (auto item: world->items){
        item->draw(&world->projection_matrix);
    }

}

/**
 * This function is a temporary measure used to the implement asynchronous
 * file loading API from C++. It should be rearchitected.
 */
void fetch_callback(Fetch *self, char *file, unsigned int length) {
    self->callback(file, length);
}
