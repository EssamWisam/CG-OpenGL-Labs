#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "utils.hpp"

struct Mesh {
    GLuint VAO, VBO, EBO;
    GLenum element_type;
    GLsizei element_count;
};

void drawMonkey(int, int, GLint, Mesh);


GLuint loadShader(const std::string& filePath, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(filePath);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCodeCStr = sourceCode.c_str();

    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::cerr << "ERROR IN " << filePath << std::endl;
        std::cerr << logStr << std::endl;
        delete[] logStr;
        glDeleteShader(shader);
        return false;
    }

    return shader;
}




// New
Mesh create_mesh_from_data(const MeshData& data){
    Mesh mesh;
    glGenVertexArrays(2, &mesh.VAO);
    glBindVertexArray(mesh.VAO);

    glGenBuffers(1, &mesh.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);

    GLint positionLoc = 0; 
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    GLint colorLoc = 1; 
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, color));

    GLint texCoordLoc = 2; 
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    GLint normalLoc = 3; 
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));      // Also send normals.

    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.elements.size() * sizeof(uint16_t), data.elements.data(), GL_STATIC_DRAW);

    mesh.element_type = GL_UNSIGNED_SHORT;
    mesh.element_count = static_cast<GLsizei>(data.elements.size());

    return mesh;
}

// New
void destroy_mesh(const Mesh& mesh){
    glDeleteVertexArrays(1, &mesh.VAO);
    glDeleteBuffers(1, &mesh.VBO);
    glDeleteBuffers(1, &mesh.EBO);
}

GLuint create_texture_from_data(const TextureData& data){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.size.x, data.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}


// Unused ------------->

MeshData create_plane(){
    MeshData data;
    data.vertices = std::vector<Vertex>({
        {{-1, 0, 1}, {255, 255, 255, 255}, {0, 0}, {0, 1, 0}},
        {{ 1, 0, 1}, {255, 255, 255, 255}, {1, 0}, {0, 1, 0}},
        {{ 1, 0,-1}, {255, 255, 255, 255}, {1, 1}, {0, 1, 0}},
        {{-1, 0,-1}, {255, 255, 255, 255}, {0, 1}, {0, 1, 0}},
    });
    data.elements = std::vector<uint16_t>({0, 1, 2, 2, 3, 0});
    return data;
}



TextureData create_single_color(const Color& color){
    return TextureData{
        {1, 1},
        std::vector<Color>({color})
    };
}

// unused <------------------

int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Monkey Boi is Darrrrk", nullptr, nullptr);
    int W, H;
    glfwGetFramebufferSize(window, &W, &H);         // For compatibility with Retina.
    if(!window){
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    GLuint program = glCreateProgram();
    
    GLuint vs = loadShader("assets/shaders/simple1.vert", GL_VERTEX_SHADER);
    glAttachShader(program, vs);
    glDeleteShader(vs);

    GLuint fs = loadShader("assets/shaders/simple2.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glDeleteShader(fs);

    glLinkProgram(program);

    GLint mvpLoc = glGetUniformLocation(program, "MVP");
    GLint samplerLoc = glGetUniformLocation(program, "tex-sampler");

    Mesh mesh = create_mesh_from_data(load_mesh("assets/models/Suzanne.obj"));
    GLuint texture = create_texture_from_data(load_texture("assets/images/albedo.jpg"));

    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    while(!glfwWindowShouldClose(window)){

        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mesh.VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);

        drawMonkey(W, H, mvpLoc, mesh);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    destroy_mesh(mesh);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



void drawMonkey(int W, int H, GLint mvpLoc, Mesh mesh){
        using namespace glm;

        float angle = (float)glfwGetTime();

        vec3 eye = glm::vec3(2*sin(angle), 1, 2*cos(angle)); 

        mat4 view = glm::lookAt( eye, vec3(0, 0, 0), vec3(0, 1, 0));

        mat4 projection = perspective(
            radians(90.0f), W/(float)H, 0.01f,100.0f);

        mat4 MVP = projection * view * translate( mat4(1.0f), vec3(0,0,0) );
        
        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&MVP);
        glDrawElements(GL_TRIANGLES, mesh.element_count, mesh.element_type, (void*)0);
 
}