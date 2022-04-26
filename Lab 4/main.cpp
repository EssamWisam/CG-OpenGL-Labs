#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// Include the header-only library OpenGL Mathematics. Will need for transformations and more.
// header-only means we only need to include the hpp files below and no linking or compiling are needed.
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace glm;
// too much glm:: can hurt your eyes

// In this lab we will apply transformation matrices on the square from last lab.
// Start from the line just before the render loop.


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

    const int W = 800, H = 600;
    // If you try this in the previous lab we get a rectange although we thought we defined a square.
    // since it directly relied on normalized device coordinates. Here the perspective transform will solve this issue.
    GLFWwindow* window = glfwCreateWindow(W, H, "Geometry Squad", nullptr, nullptr);
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

    GLint positionLoc = 0; 
    glEnableVertexAttribArray(positionLoc);
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);

    GLint colorLoc = 1; 
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
   

    //* 5 - Solving the depth problem (comment the 3 lines that have to do with depth to see it first)
    glEnable(GL_DEPTH_TEST);            // Now fragments passing the depth test will be stored in the depth buffer.
    glDepthFunc(GL_LESS);               // closer fragments to camera (smaller z) are those passing the depth test.

    GLint mvpLoc = glGetUniformLocation(program, "MVP");

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);                   // set color clear value
        glClearDepth(1.0);                                      // set depth clear value (background = far plane). 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // execute the clear
        // Recall nonlinearity of the transformed z values and its implications on setting z = 0.5 (side note)

        glBindVertexArray(VAO);
        glUseProgram(program);

        //* 1 - Modelling Transformation:
        // we will generate 3 models of it and apply all 3 on the square. 
        // They essentially are just different shifts of it.
        mat4 model[3];
        for (int i = 0 ; i <=2 ; i++){
            model[i] = translate(                   // just a translation matrix
                mat4(1.0f),                         // apply the nothing transform (identity matrix)
                vec3(0, 0, i-1)                     // translate square by (0, 0, z) for z=-1,0,1
            );
        }


        //* 2 - Camera Transformation:
        float time = glfwGetTime();
        mat4 view = lookAt(
            vec3(2*sin(time), 1, 2*cos(time)),                             // camera position
            vec3(0, 0, 0),                                                 // position camera is looking at
            vec3(0, 1, 0)                                                  // turn up vector
        );
        // It will use the vectors to make a 4x4 M_cam matrix (as in the lecture)
        // The gaze direction will be subtraction of the first two positions.
        // Notice that the camera's position is always a point on the circle x^2+z^2 = 4 (on the horizontal plane)
        // and that it changes with time. 
        // the camera is always looking towards the origin (so gaze direction is also dynamic)
        // turn up vector is along the vertical axis (y-axis)

        //* 3 - Projection Transformation
        mat4 projection = perspective(
            radians(90.0f),                                         
            // The vertical field of view angle (smaller means more zoom in)
            W/(float)H,
            //aspect ratio (width ÷ height of the window)
            0.01f,
            //near plane relative to camera
            100.0f
            // far plane relative to camera
        );
        // these uniquely define our frustum (view volume) as in the lecture
        // to yield a 4x4 perspective projection matrix

        //* 4 - Applying the composite transformation
        for (int i = 0 ; i <=2 ; i++){
         mat4 VP = projection * view * model[i];      

        // passing the matrix to shader after each render loop:
        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP);      // location, count, transpose, (float*)&matrix

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }
        // this means we get three squares (3 draw calls each render each with a different value of the uniform)

        //* Note that OpenGL handles the remaining transformation to screen coordinates on its own.

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
