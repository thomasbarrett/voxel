#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file player.h
 * \brief This file contains declarations pertaining to a player.
 * \author Thomas Barrett <tbarrett@caltech.edu>
 * \date Nov 21, 2019
 */

#include <util/Array.hpp>

#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Matrix.hpp>
#include <voxel/Browser.hpp>
#include <voxel/Chunk.hpp>

extern voxel::OBJLoader *pigMeshLoader;

/**
 * A data structure containing all information about a player.
 * A players position and size is given by the physics_object field.
 * On keyboard input, the physic_object's velocity field is updated.
 * During collision detection, the players position is corrected.
 * 
 * Additionally, a player has a currently selected block as well as
 * a theta and phi field representing the players orientation in the
 * world. 
 */
class Player {
public:
    class World *world_;
    dyn_aabb3_t physics_object;
    int health = 100;
    float theta;
    float phi;
    float velocity = 1;
    voxel::Mesh *mesh;
    voxel::Array<float, 2> target;
    bool angry = false;
    class Player * angry_target = nullptr;
    int blocks[256];
    Block current_block_;
    bool update_;
public:
    Player(World *world);
    void setPosition(float x, float y, float z);
    void draw(mat4_t *projection_matrix) {
        voxel::Matrix model_view_matrix = getModelViewMatrix();
        voxel::Matrix matrix = (voxel::Matrix::Translate({0, 0, 0}) * model_view_matrix).tranpose();
        mesh->draw((mat4_t*) &matrix, projection_matrix);
    }
    voxel::Matrix getModelViewMatrix();
   
    voxel::Array<float, 2> chunk() {
        int chunkX = floor((float) physics_object.position.x / 2 / 16);
        int chunkZ = floor((float) physics_object.position.z / 2 / 16);
        return {chunkX, chunkZ};
    }
    void update(float dt);
    void addBlock(Block b) {
        blocks[(int) b]++;
    }
    bool removeBlock(Block b) {
        if (blocks[(int) b] > 0) {
            blocks[(int) b]--;
            return true;
        } else {
            return false;
        }
    }
    Block getCurrentBlock() {
        return current_block_;
    }
    void setCurrentBlock(Block b) {
        current_block_ = b;
    }
    bool hasBlock(Block b) {
        return blocks[(int) b] > 0;
    }
    void triggerAction(Player *p) {
        velocity = 4.0 + 2 * random() - 1;
        angry_target = p;
        angry = true;
    }
};

#endif /* PLAYER_H */