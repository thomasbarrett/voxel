#ifndef MESH_HPP
#define MESH_HPP


#include <voxel/ArrayList.hpp>
#include <voxel/Array.hpp>
#include <voxel/graphics.hpp>

namespace voxel {

/**
 * Represents a three dimensional mesh.
 */
class Mesh {
private:
    int buffer;
    int modified;
    ArrayList<Array<float, 3>> vertices;
    ArrayList<Array<float, 3>> normals;
    ArrayList<Array<float, 2>> texture_coords;
    ArrayList<Array<unsigned short, 3>> faces;   
public:
    Mesh() {
        buffer = create_buffer();
        modified = true;
    }

    Mesh(Mesh &&m) {
        buffer = m.buffer;
        modified = m.modified;
        vertices = m.vertices;
        normals = m.normals;
        texture_coords = m.texture_coords;
        faces = m.faces;
        m.buffer = -1;
    }

    Mesh& operator=(Mesh &&m) = default;
    Mesh& operator=(const Mesh &m) = delete;

    void clear() {
        vertices.clear();
        normals.clear();
        texture_coords.clear();
        faces.clear();
    }
    
    void appendVertex(const Array<float, 3> &v) {
        vertices.append(v);
        modified = true;
    }

    void appendNormal(const Array<float, 3> &v) {
        normals.append(v);
        modified = true;
    }

    void setTexture(int i) {
        update_texture(buffer, i);
    }
    
    void appendTextureCoord(const Array<float, 2> &v) {
        texture_coords.append(v);
        modified = true;
    }

    void appendFace(const Array<unsigned short, 3> &f) {
        faces.append(f);
        modified = true;
    }

    void update() {
        if (modified) {
            update_vertex_buffer(buffer, (float *) vertices.buffer(), vertices.size());
            update_normal_buffer(buffer, (float *) normals.buffer(), normals.size());
            update_texture_buffer(buffer, (float *) texture_coords.buffer(), texture_coords.size());
            update_index_buffer(buffer, (unsigned short *) faces.buffer()/*(unsigned short *) faces.buffer()*/, faces.size());
        }
        modified = false;
    }

    void draw(mat4_t *model_view_matrix, mat4_t *projection_matrix) {
        draw_buffer(buffer, model_view_matrix, projection_matrix);
    }
    
    ~Mesh() {
        delete_buffer(buffer);
    }
};

};

#endif /* MESH_HPP */