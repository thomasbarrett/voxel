#ifndef LINALG_H
#define LINALG_H

/**
 * \file linalg.h
 * \brief
 * \author Thomas Barrett <tbarrett@caltech.edu>s
 * \date Nov 21, 2019
 */

/**
 * A 3x1 column vector.
 */
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3_t;

/**
 * A 4x4 square matrix.
 * Note that a mat4 is given in column major order rather than row major order
 * so as to be compatible with OpenGL, which also uses column major order.
 * 
 * The main reason behind using 4x4 matrices rather than 3x3 matrices is to
 * allow the representation of arbitrary 'affine transformation' which
 * have a translation component in addition to a linear transformation.
 */
typedef struct mat4 {
    float entries[4][4];
} mat4_t;

/**
 * Initializes the given vector with the given x, y, and z values.
 * \param self: The vector to initialize.
 * \param x: The x value.
 * \param y: The x value.
 * \param z: The x value.
 */
void vec3_init(vec3_t *self, float x, float y, float z);

/**
 * Adds two vectors and stores theb result in a third vector.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \param c: The output vector.
 */
void vec3_add(const vec3_t *a, const vec3_t *b, vec3_t *c);

/**
 * Subtracts two vectors and store the result in a third vector.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \param c: The output vector.
 */
void vec3_sub(const vec3_t *a, const vec3_t *b, vec3_t *c);

/**
 * Perform elementwise multiplication on two vectors and stores the result
 * in a third vector.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \param c: The output vector.
 */
void vec3_mult(const vec3_t *a, const vec3_t *b, vec3_t *c);

/**
 * Perform elementwise division on two vectors and stores the result
 * in a third vector.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \param c: The output vector.
 */
void vec3_div(const vec3_t *a, const vec3_t *b, vec3_t *c);

/**
 * Perform scalar multiplication on a vector and stores the result in a second vector.
 * \param a: The input vector.
 * \param b: The scalar.
 * \param c: The output vector.
 */
void vec3_scale(const vec3_t *a, float b, vec3_t *c);

/**
 * Returns the euclidian distance between two vectors.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \return The distance.
 */
float vec3_distance(const vec3_t *a, const vec3_t *b);

/**
 * Returns the dot product between two vectors.
 * \param a: The first input vector.
 * \param b: The second input vector.
 * \return The dot product.
 */
float vec3_dot(const vec3_t *a, const vec3_t *b);

/**
 * Returns the euclidian norm of a vector.
 * \param a: The first input vector.
 * \return The norm.
 */
float vec3_norm(const vec3_t *a);

/**
 * Normalizes a vector in place.
 * \param a: The vector.
 */
void vec3_normalize(vec3_t *a);

/**
 * Initializes a 4x4 matrix as a projection matrix with the given parameters
 * \param fov: field of view angle in radians
 * \param near: near z-distance
 * \param far: far z-distance
 * \param aspect: aspect ratio
 * \param c: the matrix to initialize
 */
void mat4_projection(float fov, float near, float far, float aspect, mat4_t *c);

/**
 * Initializes a 4x4 matrix as a x-rotation matrix with the given parameters
 * \param theta: rotation
 */
void mat4_rotate_x(float theta, mat4_t *c);
void mat4_rotate_y(float theta, mat4_t *c);
void mat4_translate(float x, float y, float, mat4_t *c);
void mat4_multiply(mat4_t *a, mat4_t *b, mat4_t *c);
void mat4_vec3_multiply(mat4_t *a, vec3_t *b, vec3_t *c);

#endif /* LINALG_H */
