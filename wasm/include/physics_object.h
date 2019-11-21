#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include <vector.h>
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

/**
 * Return whether or not the two axis-aligned bounding boxes intersect.
 * \param a: the first aabb.
 * \param b: the second aabb.
 * \return 1 if a and b intersection 0 otherwise.
 */
int aabb3_intersects(const aabb3_t *a, const aabb3_t *b) {
    return abs(a->position.x - b->position.x)  < a->size.x + b->size.x
        && abs(a->position.y - b->position.y)  < a->size.y + b->size.y
        && abs(a->position.z - b->position.z)  < a->size.z + b->size.z;
}

float aabb3_position_x(const aabb3_t *self) {
    return self->position.x;
}

float aabb3_position_y(const aabb3_t *self) {
    return self->position.y;
}

float aabb3_position_z(const aabb3_t *self) {
    return self->position.z;
}

float dyn_aabb3_velocity_x(const dyn_aabb3_t *self) {
    return self->velocity.x;
}

float dyn_aabb3_velocity_y(const dyn_aabb3_t *self) {
    return self->velocity.y;
}

float dyn_aabb3_velocity_z(const dyn_aabb3_t *self) {
    return self->velocity.z;
}

void aabb3_set_position_x(aabb3_t *self, float x) {
    self->position.x = x;
}

void aabb3_set_position_y(aabb3_t *self, float y) {
    self->position.y = y;
}

void aabb3_set_position_z(aabb3_t *self, float z) {
    self->position.z = z;
}

void dyn_aabb3_set_velocity_x(dyn_aabb3_t *self, float x) {
    self->velocity.x = x;
}

void dyn_aabb3_set_velocity_y(dyn_aabb3_t *self, float y) {
    self->velocity.y = y;
}

void dyn_aabb3_set_velocity_z(dyn_aabb3_t *self, float z) {
    self->velocity.z = z;
}

int aabb3_contains(const aabb3_t *a, const vec3_t *b) {
    float epsilon = 1E-6;
    return abs(a->position.x - b->x) < a->size.x + epsilon
        && abs(a->position.y - b->y) < a->size.y + epsilon
        && abs(a->position.z - b->z) < a->size.z + epsilon;
}

int aabb3_resolve_collision(const aabb3_t *block, dyn_aabb3_t *player) {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    
    float this_x = block->position.x;
    float this_y = block->position.y;
    float this_z = block->position.z;
    float this_a = block->size.x;
    float this_b = block->size.y;
    float this_c = block->size.z;
    float player_x = player->base.position.x;
    float player_y = player->base.position.y;
    float player_z = player->base.position.z;
    float player_a = player->base.size.x;
    float player_b = player->base.size.y;
    float player_c = player->base.size.z;

    if (this_x > player_x && this_x - player_x < this_a + player_a) {
        x = (this_x - player_x) - (this_a + player_a);
    } else if (player_x - this_x < this_a + player_a) {
        x = (this_a + player_a) - (player_x - this_x);
    }

    if (this_y > player_y && this_y - player_y < this_b + player_b) {
        y = (this_y - player_y) - (this_b + player_b);
    } else if (player_y - this_y < this_b + player_b) {
        y = (this_b + player_b) - (player_y - this_y);
    }

    if (this_z > player_z && this_z - player_z < this_c + player_c) {
        z = (this_z - player_z) - (this_c + player_c) ;
    } else if (player_z - this_z < this_c + player_c) {
        z = (this_c + player_c) - (player_z - this_z);
    }

    if (abs(x) < abs(y) && abs(x) < abs(z)) {
        player->base.position.x += x;
    }
    if (abs(y) < abs(x) && abs(y) < abs(z)) {
        player->base.position.y += y;
        if (y > 0) return 1;
    }
    if (abs(z) < abs(y) && abs(z) < abs(x)) {
        player->base.position.z += z;
    }
    return 0;
}

/**
 * Return 
 */
float ray_intersects(const ray3_t *ray, const aabb3_t *obj) {
    aabb3_t rel = *obj;
    vec3_sub(&obj->position, &ray->position, &rel.position);
   
    vec3_t sides_pos;
    vec3_t sides_neg;
    vec3_add(&rel.position, &rel.size, &sides_pos);
    vec3_sub(&rel.position, &rel.size, &sides_neg);

    vec3_t time_pos;
    vec3_t time_neg;
    vec3_div(&sides_pos, &ray->direction, &time_pos);
    vec3_div(&sides_neg, &ray->direction, &time_neg);

    float time_min = 1E10f;

    vec3_t right;
    vec3_scale(&ray->direction, time_pos.x, &right);
    if (aabb3_contains(&rel, &right) && time_pos.x < time_min) {
        time_min = time_pos.x;
    }

    vec3_t top;
    vec3_scale(&ray->direction, time_pos.y, &top);
    if (aabb3_contains(&rel, &top) && time_pos.y < time_min) {
        time_min = time_pos.y;
    }

    vec3_t back;
    vec3_scale(&ray->direction, time_pos.z, &back);
    if (aabb3_contains(&rel, &back) && time_pos.z < time_min) {
        time_min = time_pos.z;
    }

    vec3_t left;
    vec3_scale(&ray->direction, time_neg.x, &left);
    if (aabb3_contains(&rel, &left) && time_neg.x < time_min) {
        time_min = time_neg.x;
    }

    vec3_t bottom;
    vec3_scale(&ray->direction, time_neg.y, &bottom);
    if (aabb3_contains(&rel, &bottom) && time_neg.y < time_min) {
        time_min = time_neg.y;
    }

    vec3_t front;
    vec3_scale(&ray->direction, time_neg.z, &front);
    if (aabb3_contains(&rel, &front) && time_neg.z < time_min) {
        time_min = time_neg.z;
    }

    return time_min;
}

float ray_intersects_wrapper(float rx, float ry, float rz, float rdx, float rdy, float rdz, float ox, float oy, float oz, float owx, float owy, float owz) {
    ray3_t ray;
    ray.position.x = rx;
    ray.position.y = ry;
    ray.position.z = rz;
    ray.direction.x = rdx;
    ray.direction.y = rdy;
    ray.direction.z = rdz;
    aabb3_t obj;
    obj.position.x = ox;
    obj.position.y = oy;
    obj.position.z = oz;
    obj.size.x = owx;
    obj.size.y = owy;
    obj.size.z = owz;
    return ray_intersects(&ray, &obj);
}

#endif /* PHYSICS_OBJECT_H */