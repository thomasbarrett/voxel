#ifndef CLICKABLE_HPP
#define CLICKABLE_HPP

#include <voxel/physics_object.hpp>

class Clickable {
public:
    virtual aabb3_t* physicsObject() = 0;
    virtual void triggerAction(class Player *p) = 0;
};

#endif /* CLICKABLE_HPP */