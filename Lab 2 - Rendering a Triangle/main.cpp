#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// New content is surrounded by {__}

//{ ____________________________
//* For debugging
// OpenGL generates errors behind the scenes. Can be queried by calling the e() function (to print the error value)
// The moment an error flag is set, no other error flags will be reported
void e(){
        std::cout << glGetError() << std::endl;
}
// Call it over the codebase (after calling any other function) to see what might be causing the window
// to not render as expected.
//_____________________________ }


//{_________________________________________________________________
//* To load any of the two shaders we defined in the assets folder.

// All openGL objects are unsigned ints (some id) - e.g. GLuint here
GLuint loadShader(const std::string& filePath, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);                         // create an empty shader

    // load file then convert it to string (then to char* as that's what openGL understands):
    std::ifstream file(filePath);
    std::string source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = source.c_str();

    glShaderSource(shader, 1, &sourceCStr, nullptr);   // Replaces the source code in a shader object (we'll pass an empty one)
    // pass empty shader, will take 1 string, pass it, it knows where the string ends (no need to specify size)

    glCompileShader(shader);                            // Compile the shader object.

    return shader;
}
//_________________________________________________________________}


int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // No backward compatibility features.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(500, 500, "Crazy Triangle", nullptr, nullptr);
    if(!window){
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

//{_________________________________________________________________

    //* Attach the two shaders here (By attaching each to the program)
    GLuint program = glCreateProgram();     // create an empty program object and return a reference to it.             

    // 1 - Vertex Shader
    GLuint vs = loadShader("assets/shaders/simple.vert", GL_VERTEX_SHADER);
    glAttachShader(program, vs);
    glDeleteShader(vs);                     // Flag the shader to be deleted once no longer attached.

    // 2 - Fragment Shader
    GLuint fs = loadShader("assets/shaders/simple.frag", GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glDeleteShader(fs);


    glLinkProgram(program);                 // Create -> Attach-> Flag -> Link (shaders will now run on execution)

    //* Generate an ID for a vertex array object corresponding to vertex data (more on this next lab) 
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    //* Uniforms are global shader variables (can be set in main and used by any shader)
    GLint timeLoc = glGetUniformLocation(program, "time");      
    GLint animatedLoc = glGetUniformLocation(program, "isAnimated");      
    // takes uniform variable's name (that was defined in the shaders) and returns a pointer to it.
//_________________________________________________________________}

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2, 0.4, 0.6, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
//{_________________________________________________________________
        glUseProgram(program);
        glBindVertexArray(VAO);                     
        //In this lab it will always be the same program and VAO each render (can take them above the loop)
        

        // assigning a value to the uniform variable (seen everywhere)
        glUniform1f(timeLoc, (float)glfwGetTime());
        glUniform1i(animatedLoc, (int)1);


        glDrawArrays(GL_TRIANGLES, 0, 3);       
        // 0 is the starting index (we don't want to skip any vertices in the array)
        // Here it will use every 3 vertices to draw a triangle
        // Can draw just points, lines/line loop or triangles.
        // 9 points -> 3 triangles, 5 points -> 1 triangle
//_________________________________________________________________}
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
