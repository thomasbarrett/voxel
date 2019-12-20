#ifndef VOXEL_ITEM_HPP
#define VOXEL_ITEM_HPP

#include <util/Array.hpp>

#include <voxel/cube.hpp>
#include <voxel/Chunk.hpp>
#include <voxel/Browser.hpp>


extern voxel::OBJLoader *meshLoader;

class Item {
private:
    voxel::Mesh *mesh;
    Block block_;
public:
    dyn_aabb3_t physics_object;

    Item(Block block, voxel::Array<float, 3> position): block_{block} {
        if (Block::blocks[(int) block] != nullptr) {
            mesh = Block::blocks[(int) block];
        } else {
            Block::blocks[(int)  block] = &(meshLoader->mesh());
            mesh = Block::blocks[(int) block];
        }
        physics_object.position.x = position[0];
        physics_object.position.y = position[1];
        physics_object.position.z = position[2];

        physics_object.size.x = 0.3;
        physics_object.size.y = 0.3;
        physics_object.size.z = 0.3;

        physics_object.velocity.x = 0;
        physics_object.velocity.y = 0;
        physics_object.velocity.z = 0;

        
    
    }

    void draw(mat4_t *projection_matrix) {
        voxel::Matrix model_view_matrix = (voxel::Matrix::Scale({0.3, 0.3, 0.3}) * voxel::Matrix::Translate({
            physics_object.position.x,
            physics_object.position.y,
            physics_object.position.z
        })).tranpose(); 

        mesh->draw((mat4_t*) &model_view_matrix, projection_matrix);
    }

    Block block() {
        return block_;
    }
};

#endif /* VOXEL_ITEM_HPP */