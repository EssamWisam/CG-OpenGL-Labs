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

void drawMonkey(int, int, GLuint, Mesh);


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
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));      

    glGenBuffers(1, &mesh.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.elements.size() * sizeof(uint16_t), data.elements.data(), GL_STATIC_DRAW);

    mesh.element_type = GL_UNSIGNED_SHORT;
    mesh.element_count = static_cast<GLsizei>(data.elements.size());

    return mesh;
}


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
    glfwGetFramebufferSize(window, &W, &H);         
    if(!window){
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    GLuint program = glCreateProgram();
    
    GLuint vs = loadShader("assets/shaders/simple3.vert", GL_VERTEX_SHADER);
    glAttachShader(program, vs);
    glDeleteShader(vs);

    GLuint fs = loadShader("assets/shaders/simple7.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glDeleteShader(fs);

    glLinkProgram(program);

    

    Mesh mesh = create_mesh_from_data(load_mesh("assets/models/Suzanne.obj"));
    GLuint albedo = create_texture_from_data(load_texture("assets/images/albedo.jpg"));
    GLuint specular = create_texture_from_data(load_texture("assets/images/specular.jpg"));
    GLuint ambient_occlusion = create_texture_from_data(load_texture("assets/images/ambient_occlusion.jpg"));
    GLuint roughness = create_texture_from_data(load_texture("assets/images/roughness.jpg"));
    GLuint emissive = create_texture_from_data(load_texture("assets/images/emissive.jpg"));
    

    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    
 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);


    while(!glfwWindowShouldClose(window)){

        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(mesh.VAO);
        glUseProgram(program);

      
      

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glBindSampler(0, sampler);
        glUniform1i(glGetUniformLocation(program, "material.albedo"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular);
        glBindSampler(1, sampler);
        glUniform1i(glGetUniformLocation(program, "material.specular"), 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ambient_occlusion);
        glBindSampler(2, sampler);
        glUniform1i(glGetUniformLocation(program, "material.ambient_occlusion"), 2);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, roughness);
        glBindSampler(3, sampler);
        glUniform1i(glGetUniformLocation(program, "material.roughness"), 3);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, emissive);
        glBindSampler(4, sampler);
        glUniform1i(glGetUniformLocation(program, "material.emissive"), 4);

        drawMonkey(W, H, program, mesh);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    destroy_mesh(mesh);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



void drawMonkey(int W, int H, GLuint program, Mesh mesh){
        using namespace glm;

        float angle = (float)glfwGetTime();

        vec3 eye = glm::vec3(2*sin(angle), 1, 2*cos(angle)); 

        mat4 view = glm::lookAt( eye, vec3(0, 0, 0), vec3(0, 1, 0));

        mat4 projection = perspective(radians(90.0f), W/(float)H, 0.01f,100.0f);


        mat4 VP = projection * view;

        glm::mat4 M = yawPitchRoll(0.0f, angle, 0.0f);
        glm::mat4 M_I = inverse(M);

       
        glUniform3f(glGetUniformLocation(program, "eye"), eye.x, eye.y, eye.z);

        glUniformMatrix4fv(glGetUniformLocation(program, "VP"), 1, false, (float*)&VP);
        glUniformMatrix4fv(glGetUniformLocation(program, "M"), 1, false, (float*)&M);
        glUniformMatrix4fv(glGetUniformLocation(program, "M_IT"), 1, true, (float*)&M_I);
        

        

        
        
        
        glUniform1i(glGetUniformLocation(program, "light_count"), 2);                

        glUniform1i(glGetUniformLocation(program, "lights[0].type"), 2);              
        glUniform3f(glGetUniformLocation(program, "lights[0].position"), 2, 0, 0);    
        glUniform3f(glGetUniformLocation(program, "lights[0].direction"), -1, 0, 0);   
        glUniform3f(glGetUniformLocation(program, "lights[0].attenuation"), 1, 0, 0);   
        glUniform2f(glGetUniformLocation(program, "lights[0].cone_angles"), glm::radians(10.0f), glm::radians(11.0f));   

        glUniform3f(glGetUniformLocation(program, "lights[0].diffuse"), 1, 0.9, 0.7);
        glUniform3f(glGetUniformLocation(program, "lights[0].specular"), 1, 0.9, 0.7);     

        //* 4 - In the main, send a seperate sky uniform and don't sent ambinet as a light component.
        //glUniform3f(glGetUniformLocation(program, "lights[0].ambient"), 0.2, 0.18, 0.14);   
        glUniform3f(glGetUniformLocation(program, "sky.top"), 0.1, 0.6, 1.0);         //Bluish
        glUniform3f(glGetUniformLocation(program, "sky.middle"), 0.3, 0.3, 0.3);      // Increase to see real effect.
        glUniform3f(glGetUniformLocation(program, "sky.bottom"), 0.1, 0.1, 0.0);      // Even more greyish


        

        glUniform1i(glGetUniformLocation(program, "lights[1].type"), 1);                        
        glUniform3f(glGetUniformLocation(program, "lights[1].position"), 0, 1.5f, 0);            
        
        glUniform3f(glGetUniformLocation(program, "lights[1].attenuation"), 1, 0, 0);   
        

        glUniform3f(glGetUniformLocation(program, "lights[1].diffuse"), 1, 0.2, 0.1);           
        glUniform3f(glGetUniformLocation(program, "lights[1].specular"), 1, 0.2, 0.1);          
        glUniform3f(glGetUniformLocation(program, "lights[1].ambient"), 0, 0, 0);               


        glDrawElements(GL_TRIANGLES, mesh.element_count, mesh.element_type, (void*)0);
 
}