#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace glm;

GLuint loadShader(const std::string& filePath, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(filePath);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCodeCStr = sourceCode.c_str();

    glShaderSource(shader, 1, &sourceCodeCStr, nullptr);
    glCompileShader(shader);

    return shader;
}

struct Vertex {
    float x, y, z;
    uint8_t r, g, b, a;
};

int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Example 1", nullptr, nullptr);
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
    
    GLuint vs = loadShader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    glAttachShader(program, vs);
    glDeleteShader(vs);

    GLuint fs = loadShader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glDeleteShader(fs);

    glLinkProgram(program);

    GLint mvpLoc = glGetUniformLocation(program, "MVP");

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    Vertex vertices[] = {
        {-0.5f, -0.5f, 0.0f,   0, 255, 255, 255},
        { 0.5f, -0.5f, 0.0f, 255,   0, 255, 255},
        { 0.5f,  0.5f, 0.0f, 255, 255,   0, 255},
        {-0.5f,  0.5f, 0.0f, 255,   0,   0, 255}
    };

    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(Vertex), vertices, GL_STATIC_DRAW);

    GLint positionLoc = 0; //glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    GLint colorLoc = 1; //glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, r));

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    uint16_t elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(uint16_t), elements, GL_STATIC_DRAW);

    glEnable(GL_DEPTH_TEST);            
    glDepthFunc(GL_LESS);    
    // {_________________________________________________
    //! Start by reading the corresponding 4 sections in Notes.md
    //* 1 - Depth and Color Masks
    // glDepthMask(true);                      // enables/disables writing on the depth buffer (enabled here.)
    // glColorMask(true, true, true, true);    // enable/disable writing colors in the frame buffer. (e.g. if cause red to stay red in all frames)
    

    //* 2 - Culling
    // glEnable(GL_CULL_FACE);              // enable culling  (there is also glDisable)
    // glCullFace(GL_BACK);                 // of the front face
    // glFrontFace(GL_CCW);                 // where a front face occurs when vertices are ordered in clockwise fashion
    
    //* 3 - Scene Graphs
    // No code written.

    //* 4 - View Port Transformations (check I and II below):

    glEnable(GL_SCISSOR_TEST);              


    // _________________________________________________}

    while(!glfwWindowShouldClose(window)){

        glBindVertexArray(VAO);
        glUseProgram(program);

        // From the previous lab:
       
        mat4 model[3];
        for (int i = 0 ; i <=2 ; i++){
            model[i] = translate(                   
                mat4(1.0f),                         
                vec3(0, 0, i-1)                     
            );
        }

        float time = glfwGetTime();
        mat4 view = lookAt(
            vec3(2*sin(time), 1, 2*cos(time)),                            
            vec3(0, 0, 0),                                                 
            vec3(0, 1, 0)                                                  
        );
      

        mat4 projection = perspective(
            radians(90.0f),                                         
            W/(float)H,
            0.01f,
            100.0f
        );
      
        
    // {_________________________________________________

        //* I - Draw Call for the first section

        // I - Clear
        glViewport(0, 0, W/2, H);
        glScissor(0, 0, W/2, H);
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);                   
        glClearDepth(1.0);                                      
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     

        // II - Draw
        for (int i = 0 ; i <=2 ; i++){
         mat4 VP = projection * view * model[i];      

        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP);     

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }



        //* II - Draw Call for the second section

        // II - Clear
        glViewport(W/2, 0, W/2, H);     
        glScissor(W/2, 0, W/2, H);  
        glClearColor(0.6f, 0.4f, 0.2f, 1.0f);
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // II - Draw
        for (int i = 0 ; i <=2 ; i++){
         mat4 VP = projection * view * model[i];      

        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP);     


        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }
    // _________________________________________________}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
