#ifndef VOXEL_ITEM_HPP
#define VOXEL_ITEM_HPP

#include <voxel/cube.hpp>
#include <voxel/Chunk.hpp>
#include <voxel/Array.hpp>
#include <voxel/Browser.hpp>

extern "C" void print_float(float f);

class Item {
private:
    voxel::Mesh mesh;
    Block block;
public:
    dyn_aabb3_t physics_object;

    Item(Block block, voxel::Array<float, 3> position) {
        physics_object.position.x = position[0];
        physics_object.position.y = position[1];
        physics_object.position.z = position[2];

        physics_object.size.x = 0.3;
        physics_object.size.y = 0.3;
        physics_object.size.z = 0.3;

        physics_object.velocity.x = 0;
        physics_object.velocity.y = 0;
        physics_object.velocity.z = 0;

        for (int v = 0; v < 24; v++) {
            mesh.appendVertex({
                single_positions[v][0],
                single_positions[v][1],
                single_positions[v][2],
            });
            mesh.appendTextureCoord({
                (single_texture_coords[v][0] + block_texture_index[(int) block][v / 4][0]) / 16,
                (single_texture_coords[v][1] + block_texture_index[(int) block][v / 4][1]) / 16
            });
            mesh.appendNormal({
                single_normals[v][0],
                single_normals[v][1],
                single_normals[v][2]
            });
        }
        for(int v = 0; v < 12; v++) {
             mesh.appendFace({
                single_indices[3 * v],
                single_indices[3 * v + 1],
                single_indices[3 * v + 2]
            });
        }

        mesh.update();
    
    }

    void draw(mat4_t *projection_matrix) {
        voxel::Matrix model_view_matrix = (voxel::Matrix::Scale({0.3, 0.3, 0.3}) * voxel::Matrix::Translate({
            physics_object.position.x,
            physics_object.position.y,
            physics_object.position.z
        })).tranpose(); 

        mesh.draw((mat4_t*) &model_view_matrix, projection_matrix);
    }
};

#endif /* VOXEL_ITEM_HPP */