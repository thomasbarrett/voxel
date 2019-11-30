#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

/**
 * \file physics_object.h
 * \brief Contains physics engine function declarations and data structures.
 * \author Thomas Barrett <tbarrett@caltech.edu>
 * \date Nov 21, 2019
 */

#include <voxel/linalg.h>
#include <math.h>

/**
 * An axis-aligned bounding box is a three dimensional box with both a position
 * and a width. An axis aligned bounding box is static: it does not have a
 * velocity and is considered to have infinite mass. Additionally, an aabb3_t
 * is not effected by gravity. The aabb3_t should be used to represent all
 * non-moving items such as world blocks.
 */
typedef struct aabb3 {
    vec3_t position;
    vec3_t size;
} aabb3_t;

/**
 * A dynamic axis-aligned bounding box is similar to an aabb3_t but it has a
 * velocity in addition to the position and size fields of aabb3_t. Thus, 
 * collision detection is applied to dyn_aabb3_t objects.
 * 
 * TODO: Add collision detection for ALL dynamic bounding boxes, not just the
 * player. In the case of colliding dyn_aabb3_t, it might be important to
 * consider the mass of a dyn_aabb3_t. Future updates might require a mass
 * field to be added.
 */
typedef struct dyn_aabb3 {
    vec3_t position;
    vec3_t size;
    vec3_t velocity;
} dyn_aabb3_t;

/**
 * A ray3 is a three dimension ray in space.
 * It has both a position and a direction. Both fields are represented as
 * three dimensional vectors. The main use of a ray3 is in 'raytracing'.
 * See the ray_intersects function for further information.
 */
typedef struct ray3 {
    vec3_t position;
    vec3_t direction;
} ray3_t;

/**
 * Returns 1 if the two aabb3_t objects are intersecting and 0 if not.
 * This function is used during collision detection. However, another
 * method `aabb3_resolve_collision` is actually used to correct the
 * intersection.
 * 
 * As a side note, this method works on both aabb3_t and dyn_aabb3_t
 * objects since they are guarenteed to be compatible. A dyn_aabb3_t
 * can simply be downcast to an aabb3_t before being passed to this
 * function.
 * 
 * \param a: The first physics object.
 * \param b: The second physics object. 
 * \returns 1 if a and b intersect and 0 otherwise.
 */
int aabb3_intersects(const aabb3_t *a, const aabb3_t *b);

/**
 * Returns 1 if the given physics object contains the given point.
 * To avoid floating point errors, a small epsilon value is used.
 * Thus, any point within `epsilon` distance from the outer boundary
 * will still be considered to be inside the object.
 * 
 * \param a: The physics object.
 * \param b: The point.
 * \returns 1 if b is contained by a and 0 otherwise.
 */
int aabb3_contains(const aabb3_t *a, const vec3_t *b);

/**
 * Corrects the position of two intersection physics objects so that they are
 * no longer intersecting.
 * 
 * The algorithm in use is naiive, and if the physics engine is not updated
 * frequently enough, it is possible for a dynamic object to 'phase' through
 * another one.
 * 
 * TODO: After collision resolution, it can be useful to know more information
 * about the collision such as the collision normal (direction of collision).
 * 
 * Currently, this function only works with a static and a dynamic physics
 * object. Two static physics objects don't need collision detection since
 * they do not move. However, in the case of two dynamic physics objects, more
 * complex physical characteristics such as mass should be considered.
 * 
 * \param a: The static physics object
 * \param b: The dynamic physics object
 * \returns 1 if 
 */
int aabb3_resolve_collision(const aabb3_t *a, dyn_aabb3_t *b);

/**
 * Calculates the intersection of a ray and a physics object and returns the
 * distance between the point of collision and the ray. 
 * 
 * This function is essential to the 'picking' algorithm that is used when
 * players click on and destroy blocks.
 * 
 * TODO: Modify the function the return additional information such as the
 * side (front, back, left, etc...) that the ray collided with. This is
 * needed in order for players to place blocks.
 * 
 * \param ray: The ray being traced.
 * \param obj: The physics object being tested.
 */
float ray_intersects(const ray3_t *ray, const aabb3_t *obj);

#endif /* PHYSICS_OBJECT_H */