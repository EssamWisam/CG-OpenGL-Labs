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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Recursive Happiness", nullptr, nullptr);
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

    Color w = {255, 255, 255 };                      
    Color y = {255, 255, 0 };                        
    Color O = {0, 0, 0 };                            

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TEX_W, TEX_H, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)pattern);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);       
    // Was changed from GL_NEAREST_MIPMAP_LINEAR (will see why later)

    // {________________________________________________________________
     //* 1 - Create and bind an empty texture object (we want to render something on this)
    const int RT_W = 200;
    const int RT_H = 200;

    GLuint renderTarget;                             
    glGenTextures(1, &renderTarget);     
    glBindTexture(GL_TEXTURE_2D, renderTarget);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, RT_W, RT_H);
    // takes texture type, no. of levels, internal format and texture dimensions
    // Unlike glTexImage2D it doesn't upload any specific pixel data to the texture
    // (it allocates storage for a empty one which is what we want -> will render on it later.)
    // Unlike glTexImage2D the allocated storage is immutable and allocates all mipmaps needed at once
    // this means that unlike glTexImage2D  we can't later come and change type or dimensions (but can change data.)

    //* 2 - Create and bind a frame buffer then attach it to the texture
    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb); 
    // Now all the next write operations (drawing) will affect this frame buffer (if we used GL_READ_FRAMEBUFFER -> read operations)
    // and if we used GL_FRAMEBUFFER then for both.
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTarget, 0);
    // This attaches a texture to the frame buffer object.
    // GL_COLOR_ATTACHMENT0 makes us choose a color attachment there's finite number of those
    // and they make us use the same frame buffer to render on multiple textures 
    //! As per the documentation, the last argument must be zero 

    // ______________________________________________________________}

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);



    while(!glfwWindowShouldClose(window)){
        
        // {________________________________________________________________
        //* 3.1) Off-screen rendering on the renderTarget texture (1st pass)
        // will draw twice, once on framebuffer and once on screen.
        // now all code before glBindFrameBuffer will be on this one.
        // (bnersem 3al sora bt3tna)

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);     
        // Binding the frame buffer object -> off-screen rendering on texture
        // That is, all draw calls will draw here (not on the screen) until we bind the default FB (2nd pass)
        glViewport(0, 0, RT_W, RT_H);
        // Set the view port according to the texture's size (we're rendering on it).
        //! Try RT_W/4, RT_H/4 then try W, H or equivalently removing it.
        // ______________________________________________________________}


        glClearColor(0.6f, 0.4f, 0.2f, 1.0f);           // set background color of render target to brown
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);      

        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);
        
        drawTripleSquare(RT_W, RT_H, mvpLoc);           
        // this will be drawn on "renderTarget" note that we used the smiley face texture.
        // so the output from main-0 is what gets rendered on the renderTarget
                                                        

        // {________________________________________
        //* 3.2) Use the renderTarget as a texture and render on the screen.
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);      
        // to go back to default framebuffer (by passing 0)
        glViewport(0, 0, W, H);
        // readjust the viewport to normal

        glClearColor(0.2f, 0.4f, 0.6f, 1.0f);                   // set background color of screen to blue
        glClearDepth(1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTarget);     // we rendered a whole scene on this in the 1st pass
        glBindSampler(0, sampler);
        glUniform1i(samplerLoc, 0);
        
        drawTripleSquare(W, H, mvpLoc);
        // Now each of the squares has what was rendered on renderTarget = main-0 output
        // _________________________________________}

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