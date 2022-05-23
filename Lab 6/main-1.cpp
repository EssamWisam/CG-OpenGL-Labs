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


struct Vertex {
    float x, y, z;
    uint8_t r, g, b, a;
};

// we can use this to make an image (array of colors)
struct Color {
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

    
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Triple Square", nullptr, nullptr);
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

    GLuint fs = loadShader("assets/shaders/screen-texture.frag", GL_FRAGMENT_SHADER);
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
    
    // {________________________________
    //* 1 - Make a texture (pretty much an image, we'll make a smiley face.)

    Color w = {255, 255, 255, 255};                      // white (background)
    Color y = {255, 255, 0, 255};                        // yellow (foreground)
    Color O = {0, 0, 0, 255};                            // black (eyes and mouth)

    const int TEX_W = 8;
    const int TEX_H = 8;

    Color pattern[TEX_W*TEX_H] = {               // represents an 8x8 smiley face using the three colors
        w, w, y, y, y, y, w, w, 
        w, y, y, O, O, y, y, w,
        y, y, O, y, y, O, y, y,
        y, y, y, y, y, y, y, y,
        y, y, O, y, y, O, y, y,
        y, y, O, y, y, O, y, y,
        w, y, y, y, y, y, y, w,
        w, w, y, y, y, y, w, w,
    };
    // It's so far on the CPU.
    // Notice that the smiley face is supplied upside down. OpenGL needs this because it
    // assumes that its reading the image row by row from the bottom.

    GLuint texture;
    glGenTextures(1, &texture);                 // generate an ID for 1 texture
    glActiveTexture(GL_TEXTURE0);               // Set 0th unit as the active texture unit (default anyway) 
    glBindTexture(GL_TEXTURE_2D, texture);      // Bind it so that subsequence texture calls apply on it.    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEX_W, TEX_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pattern);
    // (texture target: GL_TEXTURE_2D will refer to the currently bound texture object,
    //  mipmap level: 0 is the base level, data format to store the texture: RGBA 8 bits, texture width, texture height,
    // must be 0 for backward compatibility, format of source image, its datatype, actual image data )
    //! Now currently bound texture object now has the texture image attached to it (on the GPU).
    
    glGenerateMipmap(GL_TEXTURE_2D);
    // Must have different mipmaps for the texture (e.g. 8x8 -> 8x8, 4x4, 2x2, 1x1 by successive 2x2 averaging)
    // If the object is farther away OpenGL will automatically use a smaller mipmap 
    // (can't fit an 8x8 image in 2x2 pixels).
    // glGenerateMipMap starts with the base level (we passed above as 0: default 8x8) and uses it
    // to generate mipmaps that OpenGL will need.
    // Can't skip doing this step (black squares) unless generating the mipmaps manually.

    
    //* 2 - Use the texture in the fragment shader
    // Need a sampler2D uniform object (get location here, set value in render loop and declare in fragment shader)
     GLint samplerLoc = glGetUniformLocation(program, "tex_sampler");
    // The goal is to use the 2D texture image to color the squares instead of the colors from lab 2.
    // OpenGL allows us to use multiple textures by making us specify the texture unit after binding
    // We succeed in binding the sampler to the texture by providing the sampler with the no. of the texture unit
    // Hence, we will provide a value of 0 in the loop below.
    // The sampler is needed because using the texture image on an object (e.g. square/cube)
    // means we will need to sample colors from it. There are many sampling parameters involved
    // that it stores as well (more on this later.)

   
    //__________________________________}
   

    glEnable(GL_DEPTH_TEST);            
    glDepthFunc(GL_LESS);               

    GLint mvpLoc = glGetUniformLocation(program, "MVP");
   


    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);                   
        glClearDepth(1.0);                                      
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
        glBindVertexArray(VAO);
        glUseProgram(program);
        //{______________________________
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);      // activate and bind.

        glUniform1i(samplerLoc, 0);                 // This attaches the sampler to the 0th texture unit.
        //______________________________}
        drawTripleSquare(W, H, mvpLoc);                         

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glDeleteTextures(1, &texture);
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
       

        for (int i = 0 ; i <=2 ; i++)
        {
        mat4 VP = projection * view * model[i];      
        glUniformMatrix4fv(mvpLoc, 1, false, (float*)&VP);      
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
        }
}