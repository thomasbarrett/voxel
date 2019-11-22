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

float* mat4_create();
void mat4_projection(float fov, float near, float far, float aspect, float c[16]);
void mat4_rotate_x(float theta, float c[16]);
void mat4_rotate_y(float theta, float c[16]);
void mat4_translate(float x, float y, float, float c[16]);
void mat4_multiply(float a[4][4], float b[4][4], float c[4][4]);

#endif /* VECTOR_H */
