#ifndef VECTOR_H
#define VECTOR_H

typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3_t;

void vec3_init(vec3_t *a, float x, float y, float z);
void vec3_add(const vec3_t *a, const vec3_t *b, vec3_t *c);
void vec3_sub(const vec3_t *a, const vec3_t *b, vec3_t *c);
void vec3_mult(const vec3_t *a, const vec3_t *b, vec3_t *c);
void vec3_div(const vec3_t *a, const vec3_t *b, vec3_t *c);
void vec3_scale(const vec3_t *a, float b, vec3_t *c);
float vec3_distance(const vec3_t *a, const vec3_t *b);
void vec3_rotate_y(const vec3_t *a, float theta, vec3_t *c);
float vec3_dot(const vec3_t *a, const vec3_t *b);
float vec3_norm(const vec3_t *a);
void vec3_normalize(vec3_t *a);

#endif /* VECTOR_H */
