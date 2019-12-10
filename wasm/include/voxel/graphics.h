#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "linalg.h"

extern "C" void on_animation_frame(struct world_t *world, float dt, float aspect);
extern "C" void draw(float *vertices, int n_vertices, unsigned short *faces, int n_faces, mat4_t *model);

#endif