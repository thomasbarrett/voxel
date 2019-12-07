#include <stdlib.h>
#include "voxel/player.h"
#include "voxel/cube.h"
#include "voxel/world.h"

void player_compute_vertex_buffer(player_t *self) {
    for (int v = 0; v < 24; v++) {
        self->vertex_buffer[3 * v + 0] = 0.5 * single_positions[v][0] + 2 * self->physics_object.position.x;
        self->vertex_buffer[3 * v + 1] = 2.0 * single_positions[v][1] + 2 * self->physics_object.position.y ;
        self->vertex_buffer[3 * v + 2] = 0.5 * single_positions[v][2] + 2 * self->physics_object.position.z;
    }
}

void player_compute_index_buffer(player_t *self) {
    for (int v = 0; v < 36; v++) {
        self->index_buffer[v] = single_indices[v];
    }
}

void player_compute_normal_buffer(player_t *self) {
    for (int v = 0; v < 24; v++) {
        for (int w = 0; w < 3; w++) {
            self->normal_buffer[3 * v + w] = single_normals[v][w];
        }
    }
}

void player_compute_texture_buffer(player_t *self) {
    for (int v = 0; v < 24; v++) {
        self->texture_buffer[2 * v + 0] = (single_texture_coords[v][0] + block_texture_index[GOLD][v / 4][0]) / 16;
        self->texture_buffer[2 * v + 1] = (single_texture_coords[v][1] + block_texture_index[GOLD][v / 4][1]) / 16;
    }
}

void player_set_position(player_t *player, float x, float y, float z) {
    player->physics_object.position.x = x;
    player->physics_object.position.y = y;
    player->physics_object.position.z = z;
    player->update = 1;
}

void player_init(player_t *player) {
    player->theta = 0;
    player->phi = 0;
    player->selection = NULL;
    player->vertex_buffer = malloc(sizeof(float) * 3 * 24);
    player->index_buffer = malloc(sizeof(unsigned short) * 36);
    player->normal_buffer = malloc(sizeof(float) * 3 * 24);
    player->texture_buffer = malloc(sizeof(float) * 2 * 24);
    player->update = 1;
    player_compute_vertex_buffer(player);
    player_compute_index_buffer(player);
    player_compute_normal_buffer(player);
    player_compute_texture_buffer(player);
}


float* player_get_vertex_buffer(player_t *self) {
    return self->vertex_buffer;
}

unsigned short * player_get_index_buffer(player_t *self) {
    return self->index_buffer;
}

float* player_get_normal_buffer(player_t *self) {
    return self->normal_buffer;
}

float* player_get_texture_buffer(player_t *self) {
    return self->texture_buffer;
}

void player_update_buffers(player_t *self) {
    if (self != NULL) {
        if (self->update) {
            player_compute_vertex_buffer(self);
        }
        self->update = 0;
    }
    
}