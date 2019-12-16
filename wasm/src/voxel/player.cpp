#include "voxel/Player.hpp"
#include "voxel/cube.hpp"
#include "voxel/world.hpp"
#include "voxel/Mesh.hpp"

voxel::Mesh* Player::mesh = nullptr;

Player::Player() {
    theta = 0;
    phi = 0;
    update = true;

    
    physics_object.velocity.z = 1.0;

    if (mesh == nullptr) {
        mesh = new voxel::Mesh{};
        mesh->setTexture(1);
        for (int v = 0; v < 24; v++) {
            mesh->appendVertex({
                0.5 * single_positions[v][0] + 2 * physics_object.position.x,
                0.5 * single_positions[v][1] + 2 * physics_object.position.y,
                1.0 * single_positions[v][2] + 2 * physics_object.position.z
            });
            mesh->appendNormal({
                single_normals[v][0],
                single_normals[v][1],
                single_normals[v][2]
            });
        }

        mesh->appendTextureCoord({36/64.0,  16/64.0});
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        mesh->appendTextureCoord({46/64.0,  8/64.0});
        mesh->appendTextureCoord({36/64.0, 8/64.0});

        // Back
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        mesh->appendTextureCoord({46/64.0,  8/64.0});
        mesh->appendTextureCoord({ 56/64.0,  8/64.0});
        mesh->appendTextureCoord({56/64.0,  16/64.0});

        // Top
        mesh->appendTextureCoord({46/64.0,  32/64.0});
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  32/64.0});

        // Bottom
        mesh->appendTextureCoord({46/64.0,  32/64.0});
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  32/64.0});
        
        // Right
        mesh->appendTextureCoord({46/64.0,  32/64.0});
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  16/64.0});
        mesh->appendTextureCoord({56/64.0,  32/64.0});
        
        // Left
        mesh->appendTextureCoord({46/64.0,  32/64.0});
        mesh->appendTextureCoord({56/64.0,  32/64.0});
        mesh->appendTextureCoord({56/64.0,  16/64.0});
        mesh->appendTextureCoord({46/64.0,  16/64.0});
        
        for (int v = 0; v < 12; v++) {
            mesh->appendFace({
                single_indices[3 * v],
                single_indices[3 * v + 1],
                single_indices[3 * v + 2]
            });
        }

        mesh->update();

    }
   
    
}

void Player::setPosition(float x, float y, float z) {
    physics_object.position.x = x;
    physics_object.position.y = y;
    physics_object.position.z = z;
    update = true;
}

voxel::Matrix Player::getModelViewMatrix() {
    voxel::Matrix matrix = voxel::Matrix::RotateY(theta) * voxel::Matrix::Translate({
        physics_object.position.x,
        physics_object.position.y,
        physics_object.position.z
    }); 
    return matrix;
}