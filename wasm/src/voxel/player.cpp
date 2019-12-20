#include "voxel/Player.hpp"
#include "voxel/cube.hpp"
#include "voxel/world.hpp"
#include "voxel/Mesh.hpp"


Player::Player(World *world) {
    world_ = world;
    theta = 0;
    phi = 0;
    update_ = true;

    
    physics_object.velocity.z = 1.0;

    if (mesh == nullptr) {
        mesh = &(pigMeshLoader->mesh());
        mesh->setTexture(1);
    }
}

void Player::setPosition(float x, float y, float z) {
    physics_object.position.x = x;
    physics_object.position.y = y;
    physics_object.position.z = z;
    update_ = true;
}

voxel::Matrix Player::getModelViewMatrix() {
    voxel::Matrix matrix = voxel::Matrix::RotateY(theta) * voxel::Matrix::Translate({
        physics_object.position.x,
        physics_object.position.y,
        physics_object.position.z
    }); 
    return matrix;
}

void Player::update(float dt) {
        physics_object.position.x += dt * physics_object.velocity.x;
        physics_object.position.y += dt * physics_object.velocity.y;
        physics_object.position.z += dt * physics_object.velocity.z;

        auto pchunk = chunk();
        int bottom = Face::None;
        for (auto chunk: this->world_->chunks_) {
            if (abs(chunk->x() - pchunk[0]) <= 1 
            || abs(chunk->z() - pchunk[1]) <= 1) {
                for (auto &block: chunk->physicsObjects()) {
                    if (aabb3_intersects(&block, (aabb3_t *) &physics_object)) {
                        bottom = bottom | aabb3_resolve_collision(&block, &physics_object);
                    }
                }
            }
        }

        if (!(bottom & Face::Bottom)) {
        physics_object.velocity.y -= dt * 20;
        } else if (physics_object.velocity.y < 0) {
        physics_object.velocity.y = 0.0;
        }

        if ((bottom & ~Face::Bottom) && (bottom & Face::Bottom)) {
            this->physics_object.velocity.y = 10;
        }

        if (this->angry) {
            this->target = {
                this->angry_target->physics_object.position.x,
                this->angry_target->physics_object.position.z,
            };
        } else {
            if (random() < 0.005) {
                this->target = {
                    physics_object.position.x + random() * 20 - 10,
                    physics_object.position.z + random() * 20 - 10,
                };
            }
        }
        if (abs(physics_object.position.x - this->target[0]) > 1) {
            physics_object.velocity.x = (this->target[0] - physics_object.position.x);
        } 

        if (abs(physics_object.position.z - this->target[1]) > 1) {
            physics_object.velocity.z = (this->target[1] - physics_object.position.z);
        } 

        float mag = sqrt(
            physics_object.velocity.x * physics_object.velocity.x 
          + physics_object.velocity.z * physics_object.velocity.z);

        if (abs(mag) > 1) {
            physics_object.velocity.x /= mag;
            physics_object.velocity.z /= mag;
            physics_object.velocity.x *= this->velocity;
            physics_object.velocity.z *= this->velocity;
            this->theta = atan2(physics_object.velocity.x, physics_object.velocity.z);
        }
    }