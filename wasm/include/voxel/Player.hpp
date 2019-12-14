#ifndef PLAYER_H
#define PLAYER_H

/**
 * \file player.h
 * \brief This file contains declarations pertaining to a player.
 * \author Thomas Barrett <tbarrett@caltech.edu>
 * \date Nov 21, 2019
 */

#include <voxel/physics_object.hpp>
#include <voxel/Mesh.hpp>
#include <voxel/Array.hpp>
#include <voxel/Matrix.hpp>
#include <voxel/Clickable.hpp>
#include <voxel/Browser.hpp>

/**
 * A data structure containing all information about a player.
 * A players position and size is given by the physics_object field.
 * On keyboard input, the physic_object's velocity field is updated.
 * During collision detection, the players position is corrected.
 * 
 * Additionally, a player has a currently selected block as well as
 * a theta and phi field representing the players orientation in the
 * world. 
 */
class Player: public Clickable {
public:
    dyn_aabb3_t physics_object;
    aabb3_t *selection;
    float theta;
    float phi;
    float velocity = 1;
    voxel::Mesh mesh;
    voxel::Array<float, 2> target;
    bool angry = false;
    class Player * angry_target = nullptr;
    bool update;
public:
    Player();
    void setPosition(float x, float y, float z);
    void draw(mat4_t *projection_matrix) {
        voxel::Matrix model_view_matrix = getModelViewMatrix();
        voxel::Matrix matrix = (voxel::Matrix::Translate({0, 0.5, 0}) * model_view_matrix).tranpose();
        mesh.draw((mat4_t*) &matrix, projection_matrix);
        voxel::Matrix matrix2 = (voxel::Matrix::Scale({1, 0.8, 0.4}) * voxel::Matrix::Translate({-1, 1, 0}) * voxel::Matrix::RotateY(1.570795) * model_view_matrix).tranpose();
        mesh.draw((mat4_t*) &matrix2, projection_matrix);
        voxel::Matrix matrix3 = (voxel::Matrix::Scale({0.4, 1, 0.2}) * voxel::Matrix::Translate({0.3, 0, 0.75}) * model_view_matrix).tranpose();;
        mesh.draw((mat4_t*) &matrix3, projection_matrix);
        voxel::Matrix matrix4 = (voxel::Matrix::Scale({0.4, 1, 0.2}) * voxel::Matrix::Translate({-0.3, 0, 0.75}) * model_view_matrix).tranpose();;
        mesh.draw((mat4_t*) &matrix4, projection_matrix);
        voxel::Matrix matrix5 = (voxel::Matrix::Scale({0.4, 1, 0.2}) * voxel::Matrix::Translate({0.3, 0, -0.75}) * model_view_matrix).tranpose();;
        mesh.draw((mat4_t*) &matrix5, projection_matrix);
        voxel::Matrix matrix6 = (voxel::Matrix::Scale({0.4, 1, 0.2}) * voxel::Matrix::Translate({-0.3, 0, -0.75}) * model_view_matrix).tranpose();;
        mesh.draw((mat4_t*) &matrix6, projection_matrix);
    }
    voxel::Matrix getModelViewMatrix();
    aabb3_t* physicsObject() {
        return (aabb3_t*) &physics_object;
    }
    void triggerAction(Player *p) {
        velocity = 8.0 + 2 * random() - 1;
        angry_target = p;
        angry = true;
    }
};

#endif /* PLAYER_H */