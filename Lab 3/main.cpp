#include <iostream>
#include <fstream>
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>


#define e() std::cout << __LINE__ << ": " << glGetError() << std::endl;

GLuint loadShader(const std::string& filePath, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);

    std::ifstream file(filePath);
    std::string source = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = source.c_str();

    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    return shader;
}

// {_________________________
struct Vertex {                     // skip for now
    float x, y, z;
    uint8_t r, g, b, a;
};
// ________________________}

int main(int, char**) {
    
    if(!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(500, 500, "Shiny Cube", nullptr, nullptr);
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
    

// {_____________________________________________

//* Primary objective of this lab is render without hard-coding like we did in last lab.
Vertex vertices[] = {                           
        {-0.5f, -0.5f, 0.0f,   0, 255, 255, 255},
        { 0.5f, -0.5f, 0.0f, 255,   0, 255, 255},
        { 0.5f,  0.5f, 0.0f, 255, 255,   0, 255},
        {-0.5f,  0.5f, 0.0f, 255,   0, 255, 255},   // Notice that we used the struct defined earlier
    };
    
    // It's on RAM need to take it to VRAM (without hardcoding data at vertex/fragment shader like last lab.)
    
//* Supplying Input (vertices and colors) to Vertex Shader
   // VAO stores all the state used to describe where a draw call (glDraw..) gets its vertex attributes. By vertex attributes
   // we mean vertex positions, colors, normal vectors,... and other attributes .
   // The data representing the values of an attribute are stored in a vertex buffer object.
   // The VAO is only about knowing for each attribute if it's enabled, where is the corresponding buffer,
   // at which offset does it start,... 
   // The 3 steps are: 1)Generate & bind VAO 2)Generate & bind VBO then put attribute data 3) Specify state of vertex attribute
    // Step 1)
    GLuint VAO;
    glGenVertexArrays(1, &VAO);         // Generating an ID and putting it in VAO.
    glBindVertexArray(VAO);             // The step where the vertex buffer is actually created.             
    // Once binding with VAO is done, GL_ARRAY_BUFFER relates to this VAO.
    // VBOs are where the attribute data (e.g. positions/colors) are stored
    // Step 2)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);         // Attach to the active array (VAO <-> GL_ARRAY_BUFFER)
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    

    // Step 3) Definining two vertex attributes for position and color.
    GLint positionLoc = 0;                      // the index we give to the attribute will be used in the vertex shader to receive input.
    glEnableVertexAttribArray(positionLoc);     // enable the attribute first (all attributes are disabled by default)
    glVertexAttribPointer(positionLoc, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0);   
    // It automatically considers the binded VBO which corresponds to the vertices.
    // (index of attribute, no. of components in each of its values, values type, are they normalized, ..., ... )
    // Now the VAO knows all it needs about the position attribute.
    

    GLint colorLoc = 1;                 // can also do glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, r));
    // Last two parameters are stride and offset. Stride is distance jumped to get next color (attribute value) and offset where to start in the buffer (r -> red).
    // to use 0->255 for the colors need to set normalize to true.
    

    //* Indexed Drawing.
    // allows us to use the same vertex for different shapes.
    // this is the general case (vertices are shared as in triangles forming a mesh)
    uint16_t elements[] = {
        0, 1, 2,                // Points used for 1st triangle
        2, 3, 0                 // Points used for 2nd triangle
    };


    //An EBO is a buffer, just like a VBO, that stores indices that OpenGL uses to decide what vertices to draw.
    # define Note "GL_ELEMENT_ARRAY_BUFFER is used to indicate the buffer you're presenting contains the indices of each element in the other (GL_ARRAY_BUFFER) buffer"
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(uint16_t), elements, GL_STATIC_DRAW);

    glBindVertexArray(0);       //unbinding the vertex array; it finished its job.
    


// ___________________________________________}

    GLint timeLoc = glGetUniformLocation(program, "time");
    

    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glUniform1f(timeLoc, (float)glfwGetTime());

        //glDrawArrays(GL_TRIANGLES, 0, 6);     // this would ignore EBO and draw 1 triangle only (as if element buffer is {0, 1, 2, 3})
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);   
        // This will implictly use the VBO according to the indecies EBO (glDrawArrays would ignore EBO)
        // Last argument is the index to start at.

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    

// {________________________________________
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(program);
// ________________________________________}
    glfwDestroyWindow(window);
    glfwTerminate();                // release resources allocated by GLFW.
        

    return 0;
}
