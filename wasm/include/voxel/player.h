#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file player.h
 * \brief This file contains declarations pertaining to a player.
 * \author Thomas Barrett <tbarrett@caltech.edu>
 * \date Nov 21, 2019
 */

#include <voxel/physics_object.h>

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
typedef struct player {
    dyn_aabb3_t physics_object;
    aabb3_t *selection;
    float theta;
    float phi;
} player_t;

#endif /* PLAYER_H */