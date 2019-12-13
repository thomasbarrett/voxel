#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <voxel/linalg.hpp>

extern "C" void on_animation_frame(struct World *world, float dt, float aspect);
extern "C" void draw(float *vertices, int n_vertices, unsigned short *faces, int n_faces, mat4_t *model);

extern "C" int create_buffer();
extern "C" void update_vertex_buffer(int, float*, int);
extern "C" void update_normal_buffer(int, float*, int);
extern "C" void update_index_buffer(int, unsigned short*, int);
extern "C" void update_texture_buffer(int, float*, int);
extern "C" void update_texture(int, int);
extern "C" void delete_buffer(int);
extern "C" void draw_buffer(int, mat4_t*, mat4_t*);

#endif