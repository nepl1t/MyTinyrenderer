#include "model.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <stdexcept>

#define MAX_LINE_LENGTH 256

Model::Model(const char* filename){
    FILE *file = fopen(filename, "r");
    if (file == nullptr) {
        printf("Failed to open file: %s\n", filename);
        throw std::runtime_error("File not found");
    }

    char linebuf[MAX_LINE_LENGTH];

    int line_number = 0;

    while(fgets(linebuf, sizeof(linebuf), file)){
        if(strncmp(linebuf, "v ", 2) == 0){
            vec3<float> vertex;
            sscanf(linebuf + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        else if(strncmp(linebuf, "vt  ", 4) == 0){
            vec3<float> texcoord;
            sscanf(linebuf + 4, "%f %f %f", &texcoord.x, &texcoord.y, &texcoord.z);
            texcoords.push_back(texcoord);
        }
        else if(strncmp(linebuf, "vn  ", 4) == 0){
            vec3<float> normal;
            sscanf(linebuf + 4, "%f %f %f", &normal.x, &normal.y, &normal.z);
            normals.push_back(normal);   
        }
        else if(strncmp(linebuf, "f ", 2) == 0){
            vec3<int> v0, v1, v2;
            int matches = sscanf(linebuf + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                                &v0.x, &v0.y, &v0.z,
                                &v1.x, &v1.y, &v1.z,
                                &v2.x, &v2.y, &v2.z);
            if (matches == 9) {
                faces.push_back(vec3<vec3<int>>({v0.x - 1, v0.y - 1, v0.z - 1}, 
                                                {v1.x - 1, v1.y - 1, v1.z - 1}, 
                                                {v2.x - 1, v2.y - 1, v2.z - 1})); 
                // face format: vertex_index/texcoord_index/normal_index
                // How to extract vertex, texcoord, normal:
                // vec3<float> vertex = model.vert(face[i][0]);
                // vec3<float> texcoord = model.texcoord(face[i][1]);
                // vec3<float> normal = model.normal(face[i][2]);
                // which i is 0, 1, or 2 for the three vertices of the face
            }
            else {
                printf("Error parsing face at line %d: %s\n", line_number, linebuf);
            }
        }
        else if(strncmp(linebuf, "#", 1) == 0){
            // Comment line, ignore
        }
        else{
            // Ignore other lines
        }
        line_number++;
    }
    fclose(file);
    printf("Loaded model from %s: %zu vertices, %zu texcoords, %zu normals, %zu faces\n",
           filename, vertices.size(), texcoords.size(), normals.size(), faces.size());
}

vec3<float> Model::vert(int i){
    return vertices[i];
}

vec3<float> Model::vert(int face_index, int index){
    int vert_index = faces[face_index][index][0];
    return vertices[vert_index];
}

vec3<float> Model::texcoord(int i){
    return texcoords[i];
}

vec3<float> Model::texcoord(int face_index, int index){
    int tex_index = faces[face_index][index][1];
    return texcoords[tex_index];
}

vec3<float> Model::normal(int i){
    return normals[i];
}

vec3<float> Model::normal(int face_index, int index){
    int norm_index = faces[face_index][index][2];
    return normals[norm_index];
}

vec3<vec3<int>> Model::face(int i){
    return faces[i];
}