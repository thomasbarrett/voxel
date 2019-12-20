#ifndef MESH_HPP
#define MESH_HPP


#include <util/ArrayList.hpp>
#include <util/Array.hpp>
#include <util/Fetch.hpp>
#include <voxel/graphics.hpp>
#include <voxel/Browser.hpp>

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
        if (buffer != -1) {
            delete_buffer(buffer);
        }
    }
};

class OBJLoader: public File {
private:
    Mesh mesh_;
public:
    OBJLoader(const char *url): File{url} {}

    Mesh& mesh() {
        return mesh_;
    } 

    void callback(char *file, unsigned int length) override {
        File::callback(file, length);
        while (hasNext()) {
            while(File::next(" ") || File::next("\n")) {}
            if (File::next("vt")) {
                float u = File::nextFloat();
                float v = File::nextFloat();
                mesh_.appendTextureCoord({u, v});
            } else if (File::next("vn")) {
                float x = File::nextFloat();
                float y = File::nextFloat();
                float z = File::nextFloat();
                mesh_.appendNormal({x, y, z});
            } else if (File::next("f")) {
                int v1 = File::nextInt();
                int v2 = File::nextInt();
                int v3 = File::nextInt();
                mesh_.appendFace({v1, v2, v3});
            } else if (File::next("v")) {
                float x = File::nextFloat();
                float y = File::nextFloat();
                float z = File::nextFloat();
                mesh_.appendVertex({x, y, z});
            } else {
                print_char(File::next());
            }
            mesh_.update();
        }
    }
};

};

#endif /* MESH_HPP */