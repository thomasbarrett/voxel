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
    float *vertex_buffer;
    unsigned short *index_buffer;
    float *normal_buffer;
    float *texture_buffer;
    int update;
} player_t;

void player_set_position(player_t *player, float x, float y, float z);
void player_init(player_t *player);
float* player_get_vertex_buffer(player_t *self);
unsigned short * player_get_index_buffer(player_t *self);
float* player_get_normal_buffer(player_t *self);
float* player_get_texture_buffer(player_t *self);
void player_update_buffers(player_t *self);

#endif /* PLAYER_H */