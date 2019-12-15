#ifndef CLICKABLE_HPP
#define CLICKABLE_HPP

#include <voxel/physics_object.hpp>

class Clickable {
public:
    virtual aabb3_t* physicsObject() = 0;
    virtual void triggerAction(class Player *p) = 0;
};

class BlockClickable: public Clickable {
private:
    aabb3_t* physicsObject_;
public:
    BlockClickable(aabb3_t* physics_object): physicsObject_{physics_object} {}

    aabb3_t* physicsObject() {
        return physicsObject_;
    }

    void triggerAction(class Player *p) {

    }
};

#endif /* CLICKABLE_HPP */