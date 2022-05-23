#include "utils.hpp"
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

MeshData load_mesh(const std::string& file_path){

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str(), nullptr)) {
        std::cerr << "Failed to load obj file \"" << file_path << "\" due to error: " << err << std::endl;
        exit(-1);
    }
    
    if (!warn.empty()) {
        std::cout << "WARN while loading obj file \"" << file_path << "\": " << warn << std::endl;
    }

    MeshData data;
    std::unordered_map<Vertex, uint16_t> vertex_map;
    
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};

            // Read the data for a vertex from the "attrib" object
            vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
            };

            vertex.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
            };


            vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0] * 255,
                    attrib.colors[3 * index.vertex_index + 1] * 255,
                    attrib.colors[3 * index.vertex_index + 2] * 255,
                    255
            };

            // See if we already stored a similar vertex
            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end()) {
                auto new_vertex_index = static_cast<uint16_t>(data.vertices.size());
                vertex_map[vertex] = new_vertex_index;
                data.elements.push_back(new_vertex_index);
                data.vertices.push_back(vertex);
            } else {
                data.elements.push_back(it->second);
            }
        }
    }

    return data;
}

TextureData load_texture(const std::string& file_path){
    TextureData data;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixels = stbi_load(file_path.c_str(), &data.size.x, &data.size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << file_path << std::endl;
        exit(-1);
    }

    data.pixels.resize(data.size.x * data.size.y);
    memcpy(data.pixels.data(), pixels, data.pixels.size() * sizeof(Color));
    stbi_image_free(pixels);

    return data;
}