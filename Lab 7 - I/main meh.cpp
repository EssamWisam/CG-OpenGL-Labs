#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

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
    uint8_t r, g, b;
};

struct Vertex {
    float x, y, z;
    uint8_t r, g, b, a;
    float s, t;
};

int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    const int W = 800, H = 600;
    GLFWwindow* window = glfwCreateWindow(W, H, "Example 1", nullptr, nullptr);
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

    Color w = {255, 255, 255}; // White
    Color Y = {255, 255, 0}; // Yellow
    Color b = {0, 0, 0}; // Black

    const int TEX_W = 9;
    const int TEX_H = 8;

    Color pixels[TEX_W*TEX_H] = {
        w, w, Y, Y, Y, Y, w, w, w, 
        w, Y, Y, b, b, Y, Y, w, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        Y, Y, Y, Y, Y, Y, Y, Y, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        Y, Y, b, Y, Y, b, Y, Y, w,
        w, Y, Y, Y, Y, Y, Y, w, w,
        w, w, Y, Y, Y, Y, w, w, w,
    };

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT

    const int RT_W = 200;
    const int RT_H = 200;

    GLuint colorRT;
    glGenTextures(1, &colorRT);
    glBindTexture(GL_TEXTURE_2D, colorRT);
    int levels = (int)glm::floor(glm::log2((float)glm::max(RT_W, RT_H))) + 1;
    glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, RT_W, RT_H);

    GLuint depthRT;
    glGenTextures(1, &depthRT);
    glBindTexture(GL_TEXTURE_2D, depthRT);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32, RT_W, RT_H);

    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRT, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRT, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glDepthMask(true);
    glColorMask(true, true, true, true);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);

    // glEnable(GL_SCISSOR_TEST);

    while(!glfwWindowShouldClose(window)){

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);

        glViewport(0, 0, RT_W, RT_H);

        glClearColor(0.6f, 0.4f, 0.2f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);
        
        float angle = (float)glfwGetTime();

        glm::vec3 eye = glm::vec3(2*glm::sin(angle), 1, 2*glm::cos(angle)); //glm::vec3(0, 0, 2); //

        glm::mat4 view = glm::lookAt(
            eye,
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );

        glm::mat4 projection = glm::perspective(
            glm::pi<float>()/2,
            RT_W/float(RT_H),
            0.01f,
            100.0f
        );

        for(int z = -1; z <= 1; z++){
            glm::mat4 MVP = projection * view * glm::translate(
                glm::mat4(1.0f),
                glm::vec3(0, 0, z)
            );
            glUniformMatrix4fv(mvpLoc, 1, false, (float*)&MVP);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glViewport(0, 0, W, H);

        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorRT);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);
        
        angle = (float)glfwGetTime();

        eye = glm::vec3(2*glm::sin(angle), 1, 2*glm::cos(angle)); //glm::vec3(0, 0, 2); //

        view = glm::lookAt(
            eye,
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
        );

        projection = glm::perspective(
            glm::pi<float>()/2,
            W/float(H),
            0.01f,
            100.0f
        );

        for(int z = -1; z <= 1; z++){
            glm::mat4 MVP = projection * view * glm::translate(
                glm::mat4(1.0f),
                glm::vec3(0, 0, z)
            );
            glUniformMatrix4fv(mvpLoc, 1, false, (float*)&MVP);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
