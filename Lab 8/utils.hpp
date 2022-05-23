#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>

typedef glm::vec<4, glm::uint8, glm::defaultp> Color;

class Vertex {
public:

    glm::vec3 position;
    Color color;
    glm::vec2 tex_coord;
    glm::vec3 normal;                               // needed for lighting.

    bool operator==(const Vertex& other) const {
        return position == other.position &&
                color == other.color &&
                tex_coord == other.tex_coord &&
                normal == other.normal;
    }
};

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> elements;
};



MeshData load_mesh(const std::string& file_path);

struct TextureData {
    glm::ivec2 size;
    std::vector<Color> pixels;
};

TextureData load_texture(const std::string& file_path);


//////////////////////////////////////////////////////////////
// Hash Function
//////////////////////////////////////////////////////////////

namespace std {
    inline size_t hash_combine(size_t h1, size_t h2){ return h1 ^ (h2 << 1); }

    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            size_t combined = hash<glm::vec3>()(vertex.position);
            combined = hash_combine(combined, hash<Color>()(vertex.color));
            combined = hash_combine(combined, hash<glm::vec2>()(vertex.tex_coord));
            combined = hash_combine(combined, hash<glm::vec3>()(vertex.normal));
            return combined;
        }
    };
}