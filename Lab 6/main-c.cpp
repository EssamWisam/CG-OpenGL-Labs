#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
void drawTripleSquare(int, int, GLint);

GLuint loadShader(const std::string& filePath, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(filePath);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCodeCStr = sourceCode.c_str();

    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);
    //* For Shader debugging (skip)
    // {______________________________________________________
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
    // _________________________________________________________

    return shader;
}

// {_________________________           // here
struct Color {
    uint8_t r, g, b;
};
// _______________________}

struct Vertex {
    float x, y, z;
    uint8_t r, g, b, a;
    float s, t;                         // added texture coordinates (0,0) lower bottom to (1,1) top right 
                                        // independent of texture.
};
// means we will need an extra attribute for texture coordinates.


int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const int W = 800, H = 600;
    GLFWwindow* window = glfwCreateWindow(W, H, "Happy Squares", nullptr, nullptr);
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
        { 0.5f, -0.5f, 0.0f, 255,   0, 255, 255, 20, 0},
        { 0.5f,  0.5f, 0.0f, 255, 255,   0, 255, 20, 20},
        {-0.5f,  0.5f, 0.0f, 255,   0,   0, 255, 0, 20}
    };

    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(Vertex), vertices, GL_STATIC_DRAW);

    GLint positionLoc = 0; //glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    GLint colorLoc = 1; //glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, r));

    GLint texCoordLoc = 2; //glGetAttribLocation(program, "tex_coord");
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

    // {________________________________________________

    //* 1 - Create a texture
    Color w = {255, 255, 255};                   // White
    Color Y = {255, 255, 0};                    // Yellow
    Color b = {0, 0, 0};                        // Black

    const int TEX_W = 9;
    const int TEX_H = 8;

    Color pixels[TEX_W*TEX_H] = {               // represents an 9x8 smiley face using the three colors
        w, w, Y, Y, Y, Y, w, w, w, 
        w, Y, Y, b, b, Y, Y, w, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        Y, Y, Y, Y, Y, Y, Y, Y, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        w, Y, Y, Y, Y, Y, Y, w, w,
        w, w, Y, Y, Y, Y, w, w, w,
    };
    // It's so far on the CPU.

    GLuint texture;
    glGenTextures(1, &texture);     
    glBindTexture(GL_TEXTURE_2D, texture);      // need to bind any shader. hmm (attribute)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixels);
    // sending image data to texture. level = 0 (y3ny 8x8), internal format (when stored on GPU)= 8 bit for each channel.
    // order = 0 (must be 0 backward comp.), the input format and type. Then finally a pointer to pixels.
    // Now data is on the GPU
    // Running at this point won't change anything. (need to stuff in shader first)
    //glDeleteTextures(1, &texture);

    glGenerateMipmap(GL_TEXTURE_2D);        // need it 3lshan el denya teshta8a;
    // 8x8 -> 4x4 -> 2x2 -> 1x1 (Mimmap averages every 2 pixels). Each of these is a level.
    // it will automatically calculate them all.
    // sampler expects mipmap

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

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);
        
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
        for (int i = 0 ; i <=2 ; i++)   model[i] = translate(mat4(1.0f), vec3(0, 0, i-1) );         // Modelling Transformation

        float time = glfwGetTime();
        mat4 view = lookAt(vec3(2*sin(time), 1, 2*cos(time)), vec3(0, 0, 0), vec3(0, 1, 0));        // Camera Transformation
       
        mat4 projection = perspective( radians(90.0f), W/(float)H, 0.01f, 100.0f );                 // Projection Transformation
       

        for (int i = 0 ; i <=2 ; i++)
        {
        mat4 VP = projection * view * model[i];      
        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP);      
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }
}