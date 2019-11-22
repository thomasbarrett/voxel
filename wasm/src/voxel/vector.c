/**
 * This file contains
 * 
 */
#include <voxel/vector.h>
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

float* mat4_create() {
    return malloc(sizeof(float) * 16);
}

void mat4_rotate_x(float theta, float c[16]) {
    float ctheta = cos(theta);
    float stheta = sin(theta);

    c[0] = 1.0;
    c[4] = 0.0;
    c[8] = 0.0;
    c[12] = 0.0;
    
    c[1] = 0.0;
    c[5] = ctheta;
    c[9] = -stheta;
    c[13] = 0.0;

    c[2] = 0.0;
    c[6] = stheta;
    c[10] = ctheta;
    c[14] = 0.0;

    c[3] = 0.0;
    c[7] = 0.0;
    c[11] = 0.0;
    c[15] = 1.0;
}


void mat4_rotate_y(float theta, float c[16]) {
    float ctheta = cos(theta);
    float stheta = sin(theta);

    c[0] = ctheta;
    c[4] = 0.0;
    c[8] = stheta;
    c[12] = 0.0;
    
    c[1] = 0.0;
    c[5] = 1.0;
    c[9] = 0.0;
    c[13] = 0.0;

    c[2] = -stheta;
    c[6] = 0.0;
    c[10] = ctheta;
    c[14] = 0.0;

    c[3] = 0.0;
    c[7] = 0.0;
    c[11] = 0.0;
    c[15] = 1.0;
}


void mat4_multiply(float a[4][4], float b[4][4], float c[4][4]) {
    float tmp1[4][4];
    float tmp2[4][4];
    memcpy(&tmp1, a, 16 * sizeof(float));
    memcpy(&tmp2, b, 16 * sizeof(float));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            c[i][j] = 0; 
            for (int k = 0; k < 4; k++) {
                c[i][j] += tmp1[i][k] * tmp2[k][j]; 
            }
        }
    }
}

void mat4_projection(float fov, float near, float far, float aspect, float c[16]) {
    float f = 1.0 / tan(fov / 2);
    float nf = 1 / (near - far);
    
    c[0] = f / aspect;
    c[1] = 0.0;
    c[2] = 0.0;
    c[3] = 0.0;
    
    c[4] = 0.0;
    c[5] = f;
    c[6] = 0.0;
    c[7] = 0.0;

    c[8] = 0.0;
    c[9] = 0.0;
    c[10] = (far + near) * nf;
    c[11] = -1;

    c[12] = 0.0;
    c[13] = 0.0;
    c[14] = 2 * far * near * nf;
    c[15] = 0.0;
}

void mat4_translate(float x, float y, float z, float c[16]) {
    c[0] = 1.0;
    c[1] = 0.0;
    c[2] = 0.0;
    c[3] = 0.0;
    
    c[4] = 0.0;
    c[5] = 1.0;
    c[6] = 0.0;
    c[7] = 0.0;

    c[8] = 0.0;
    c[9] = 0.0;
    c[10] = 1.0;
    c[11] = 0.0;

    c[12] = x;
    c[13] = y;
    c[14] = z;
    c[15] = 1.0;
}