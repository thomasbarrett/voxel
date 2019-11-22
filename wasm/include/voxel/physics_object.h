#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <voxel/vector.h>
#include <math.h>

/*
 * Represents a solid block in space.
 */
typedef struct aabb3 {
    vec3_t position;
    vec3_t size;
} aabb3_t;

typedef struct dyn_aabb3 {
    aabb3_t base;
    vec3_t velocity;
} dyn_aabb3_t;

typedef struct ray3 {
    vec3_t position;
    vec3_t direction;
} ray3_t;

int aabb3_intersects(const aabb3_t *a, const aabb3_t *b);
int aabb3_contains(const aabb3_t *a, const vec3_t *b);
int aabb3_resolve_collision(const aabb3_t *block, dyn_aabb3_t *player);
float ray_intersects(const ray3_t *ray, const aabb3_t *obj);

#endif /* PHYSICS_OBJECT_H */