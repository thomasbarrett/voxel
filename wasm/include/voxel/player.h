#ifndef PLAYER_H
#define PLAYER_H

#include <voxel/physics_object.h>

typedef struct player {
    dyn_aabb3_t physics_object;
    aabb3_t *selection;
    float theta;
    float phi;
} player_t;

#endif /* PLAYER_H */