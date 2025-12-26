#pragma once

#include <vector>


template <typename T>
class vec3 {
    public:
        T x, y, z;
        vec3() : x(0), y(0), z(0) {}
        vec3(T x, T y, T z) : x(x), y(y), z(z) {}
        T operator[](int i) const {
            return i == 0 ? x : (i == 1 ? y : z);
        }
        T& operator[](int i) {
            return i == 0 ? x : (i == 1 ? y : z);
        }
};

template <typename T>
class vec2 {
    public:
        T u, v;
        vec2() : u(0), v(0) {}
        vec2(T u, T v) : u(u), v(v) {}
        T operator[](int i) const {
            return i == 0 ? u : v;
        }
        T& operator[](int i) {
            return i == 0 ? u : v;
        }
};

class Vertex{
    public:
        vec3<float> position;
        vec2<float> texcoord;
        vec3<float> normal;
};

class Model {
    private:
        std::vector<vec3<float>> vertices;
        std::vector<vec3<float>> texcoords;
        std::vector<vec3<float>> normals;
        std::vector<vec3<vec3<int>>> faces; 
        // vec3<vec3<int>> to store 
        // {{vertex_ind, texcoord_ind, normal_ind}, 
        //  {vertex_ind, texcoord_ind, normal_ind},
        //  {vertex_ind, texcoord_ind, normal_ind}}

    public:
        Model(const char* filename); 
        const int get_num_faces()    const  {return static_cast<int>(faces.size());};
        const int get_num_vertices() const  {return static_cast<int>(vertices.size());};
        const int get_num_texcoords()const  {return static_cast<int>(texcoords.size());};
        const int get_num_normals()  const  {return static_cast<int>(normals.size());};

        vec3<float> vert(int i);
        vec3<float> vert(int face_index, int index);
        vec3<float> texcoord(int i);
        vec3<float> texcoord(int face_index, int index);
        vec3<float> normal(int i);
        vec3<float> normal(int face_index, int index);
        vec3<vec3<int>> face(int i);
};