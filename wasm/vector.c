/**
 * This file contains
 * 
 */

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3_t;

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

float vec3_dot(const vec3_t *a, const vec3_t *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}
