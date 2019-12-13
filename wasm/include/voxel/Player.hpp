#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file player.h
 * \brief This file contains declarations pertaining to a player.
 * \author Thomas Barrett <tbarrett@caltech.edu>
 * \date Nov 21, 2019
 */

#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Array.hpp>

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
    dyn_aabb3_t physics_object;
    aabb3_t *selection;
    float theta;
    float phi;
    voxel::Mesh mesh;
    voxel::Array<float, 2> target;
    bool update;
public:
    Player();
    void setPosition(float x, float y, float z);
    mat4_t getModelViewMatrix();
};

#endif /* PLAYER_H */