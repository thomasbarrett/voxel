/**
 * This file contains
 * 
 */
#include <voxel/linalg.h>
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

void mat4_rotate_x(float theta, mat4_t *mat) {
    float ctheta = cos(theta);
    float stheta = sin(theta);
    mat->entries[0][0] = 1.0;
    mat->entries[1][0] = 0.0;
    mat->entries[2][0] = 0.0;
    mat->entries[3][0] = 0.0;
    
    mat->entries[0][1] = 0.0;
    mat->entries[1][1] = ctheta;
    mat->entries[2][1] = -stheta;
    mat->entries[3][1] = 0.0;

    mat->entries[0][2] = 0.0;
    mat->entries[1][2] = stheta;
    mat->entries[2][2] = ctheta;
    mat->entries[3][2] = 0.0;

    mat->entries[0][3] = 0.0;
    mat->entries[1][3] = 0.0;
    mat->entries[2][3] = 0.0;
    mat->entries[3][3] = 1.0;
}


void mat4_rotate_y(float theta, mat4_t *mat) {
    float ctheta = cos(theta);
    float stheta = sin(theta);

    mat->entries[0][0] = ctheta;
    mat->entries[1][0] = 0.0;
    mat->entries[2][0] = stheta;
    mat->entries[3][0] = 0.0;
    
    mat->entries[0][1] = 0.0;
    mat->entries[1][1] = 1.0;
    mat->entries[2][1] = 0.0;
    mat->entries[3][1] = 0.0;

    mat->entries[0][2] = -stheta;
    mat->entries[1][2] = 0.0;
    mat->entries[2][2] = ctheta;
    mat->entries[3][2] = 0.0;

    mat->entries[0][3] = 0.0;
    mat->entries[1][3] = 0.0;
    mat->entries[2][3] = 0.0;
    mat->entries[3][3] = 1.0;
}


void mat4_multiply(mat4_t *a, mat4_t *b, mat4_t *c) {
    float tmp1[4][4];
    float tmp2[4][4];
    memcpy(&tmp1, &a->entries, 16 * sizeof(float));
    memcpy(&tmp2, &b->entries, 16 * sizeof(float));
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            c->entries[i][j] = 0; 
            for (int k = 0; k < 4; k++) {
                c->entries[i][j] += tmp1[i][k] * tmp2[k][j]; 
            }
        }
    }
}

void mat4_vec3_multiply(mat4_t *a, vec3_t *b, vec3_t *c) {
    float tmp1[4];
    float tmp2[4];
    tmp1[0] = b->x;
    tmp1[1] = b->y;
    tmp1[2] = b->z;
    tmp1[3] = 1;
    for (int i = 0; i < 4; i++) {
        tmp2[i] = 0; 
        for (int j = 0; j < 4; j++) {
            tmp2[i] += a->entries[j][i] * tmp1[j];
        }
    }
    c->x = tmp2[0];
    c->y = tmp2[1];
    c->z = tmp2[2];
}

void mat4_projection(float fov, float near, float far, float aspect, mat4_t *mat) {
    float f = 1.0 / tan(fov / 2);
    float nf = 1 / (near - far);
    float *c = (float*) &mat->entries;
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

void mat4_translate(float x, float y, float z, mat4_t *mat) {
    float *c = (float*) &mat->entries;

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