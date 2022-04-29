#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <map>
void drawTripleSquare(int, int, GLint);

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


struct Color {
    uint8_t r, g, b, a;
};

struct Vertex {
    float x, y, z;
    uint8_t r, g, b, a;
    float s, t;
};

GLFWwindow* window;

int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(800, 600, "Recursive Happiness with Transperancy Fixed ", nullptr, nullptr);
    
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
    
    GLuint vs = loadShader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    glAttachShader(program, vs);
    glDeleteShader(vs);

    GLuint fs = loadShader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glDeleteShader(fs);

    glLinkProgram(program);

    GLint mvpLoc = glGetUniformLocation(program, "MVP");
    GLint samplerLoc = glGetUniformLocation(program, "tex_sampler");

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    
    Vertex vertices[] = {
        {-0.5f, -0.5f, 0.0f,   0, 255, 255, 255, 0, 0},
        { 0.5f, -0.5f, 0.0f, 255,   0, 255, 255, 1, 0},
        { 0.5f,  0.5f, 0.0f, 255, 255,   0, 255, 1, 1},
        {-0.5f,  0.5f, 0.0f, 255,   0,   0, 255, 0, 1}
    };

    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(Vertex), vertices, GL_STATIC_DRAW);

    GLint positionLoc = 0; 
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    GLint colorLoc = 1;
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, r));

    GLint texCoordLoc = 2; 
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, s));

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    uint16_t elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(uint16_t), elements, GL_STATIC_DRAW);

    Color w = {255, 255, 255, 128 };            
    Color y = {255, 255, 0, 255 };                        
    Color O = {0, 0, 0, 255 };                            

    const int TEX_W = 8;
    const int TEX_H = 8;

    Color pattern[TEX_W*TEX_H] = {               
        w, w, y, y, y, y, w, w,
        w, y, y, O, O, y, y, w,
        y, y, O, y, y, O, y, y,
        y, y, y, y, y, y, y, y,
        y, y, O, y, y, O, y, y,
        y, y, O, y, y, O, y, y,
        w, y, y, y, y, y, y, w,
        w, w, y, y, y, y, w, w,
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_W, TEX_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pattern);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    //* 0 - Have depth test be enabled. 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Blending on!
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);                       
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  



    while(!glfwWindowShouldClose(window)){

        glClearColor(0.1f, 0.4f, 0.6f, 1.0f);      
        glClearDepth(1.0);
        //* 1 - re-enable writing on depth buffer 
        glDepthMask(true);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //* 2 - Draw opaque objects -> there are none.
        //* 3 - disable writing on depth buffer 
        glDepthMask(false);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);

        //* 4+5 - Draw translucent objects (farther then nearer)
        drawTripleSquare(W, H, mvpLoc);
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void drawTripleSquare(int W, int H, GLint mvpLoc){
    using namespace glm;
   
    mat4 model[3];
    for (int i = 0 ; i <=2 ; i++)   model[i] = translate(mat4(1.0f), vec3(0, 0, i-1) );         


    float time = glfwGetTime();

    mat4 view = lookAt(vec3(2*sin(time), 1, 2*cos(time)), vec3(0, 0, 0), vec3(0, 1, 0));        
    mat4 projection = perspective( radians(90.0f), W/(float)H, 0.01f, 100.0f );                 

    //! Let's only compute the three transformation matrices for now then decide the right order of use
    mat4 VP[3];
    for (int i = 0 ; i <=2 ; i++){
     VP[i] = projection * view * model[i];
    }          

    //! The centers of each the 3 squares
    vec4 centers[3]= {
        vec4(0.0f, 0.0f, 0.0f, 1.0f),
        vec4( 0.0f, 0.0f, 0.25f, 1.0f),
        vec4( 0.0f, 0.0f, 0.5f, 1.0f),
    };


    //! Calculate the distance from each to the camera (which is at (0,0,0) -> magnitude) in the camera space
    //! Recall that maps in C++ are sorted by key by default.
    std::map<float, int> sorted;
    for (unsigned int i = 0; i <= 2 ; i++)
    {
        glm::vec3 currCenter = view * model[i] * centers[i];    
        float distance = length(currCenter);
        sorted[distance] = i;
    }

    //! Draw after sorting the distances
    for(std::map<float, int>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) 
    {
        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP[it->second]);      
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
    }  
}