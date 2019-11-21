/**
 * This file contains
 * 
 */
#include <vector.h>
#include <math.h>

void vec3_init(vec3_t *a, float x, float y, float z) {
    a->x = x;
    a->y = y;
    a->z = z;
}

void vec3_add(const vec3_t *a, const vec3_t *b, vec3_t *c) {
    c->x = a->x + b->x;
    c->y = a->y + b->y;
    c->z = a->z + b->z;
}

void vec3_sub(const vec3_t *a, const vec3_t *b, vec3_t *c) {
    c->x = a->x - b->x;
    c->y = a->y - b->y;
    c->z = a->z - b->z;
}

void vec3_mult(const vec3_t *a, const vec3_t *b, vec3_t *c) {
    c->x = a->x * b->x;
    c->y = a->y * b->y;
    c->z = a->z * b->z;
}

void vec3_div(const vec3_t *a, const vec3_t *b, vec3_t *c) {
    c->x = a->x / b->x;
    c->y = a->y / b->y;
    c->z = a->z / b->z;
}

void vec3_scale(const vec3_t *a, float b, vec3_t *c) {
    c->x = b * a->x;
    c->y = b * a->y;
    c->z = b * a->z;
}

void vec3_rotate_y(const vec3_t *a, float theta, vec3_t *c) {
    vec3_t temp = *a;
    float ctheta = cos(theta);
    float stheta = sin(theta);
    c->x = ctheta * temp.x + stheta * temp.z;
    c->y = temp.y;
    c->z = -stheta * temp.x + ctheta * temp.z;
}

float vec3_distance(const vec3_t *a, const vec3_t *b) {
    float u = a->x - b->x;
    float v = a->y - b->y;
    float w = a->z - b->z;
    return sqrt(u * u + v * v + w * w);
}

float vec3_dot(const vec3_t *a, const vec3_t *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float vec3_norm(const vec3_t *a) {
    return sqrt(vec3_dot(a, a));
}

void vec3_normalize(vec3_t *a) {
    float norm = vec3_norm(a);
    vec3_scale(a, 1.0 / norm, a);
}
