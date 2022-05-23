#include <iostream>
#include "glad/gl.h" // be sure to include GLAD before other header files that require OpenGL (like GLFW).
#include <GLFW/glfw3.h>

// Need BetterComments extention to see colorful comments.
// Neet Unotes extention to access Notes.md and Pipeline.md (check them first)

int main(int, char**) {
   //* Initalization
   glfwInit();
   //  Configure GLFW: (GLFW_Option, Value to Set) 
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
   // No backward compatibility features (a more limited package of functions)
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Allow forward compatibaility.


   //* Create a window object
   GLFWwindow* window = glfwCreateWindow(800, 600, "Window Name", NULL, NULL);  // 4th Argument -> Fullscren
   if (window == NULL)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);    
   // An OpenGL's context holds the state which contains information related to the specific window.    
   // https://computergraphics.stackexchange.com/questions/4562/what-does-makecontextcurrent-do-exactly
    

   //* Initialize GLAD 
    gladLoadGL(glfwGetProcAddress);

   
   //* Want to Draw a triangle (will only take the first step - more in the next lab)
   float vertices[] = {                // z is 0 so the triangle is 2D
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
   };  

   //* Want to store the vertices in the GPU's memory so that the vertex shader has
   //* instantenous access to them.
   //* Routine to get this done is to (1) Generate an ID for the buffer & store it, (2) bind the buffer to it and specify type
   //* Then put the data in the buffer:

   GLuint VBO;                                    // Where the buffer ID will be stored
   glGenBuffers(1, &VBO);                        // 1 Buffer -> generate 1 ID (else would need an array)
   glBindBuffer(GL_ARRAY_BUFFER, VBO);           // GL_ARRAY_BUFFER is the type of a vertex buffer object.
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  
   //  Copies the previously defined vertex data into the buffer's memory:
   // (buffer type, size of data to copy in bytes, data, does the data change? is it frequently used?)
   // Note that since whenever we binded the buffer: 
   #define Note "any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bound buffer which is VBO"

   

   //! In general callbacks should be registered after initializing the window and before the render loop 

   //* Render loop (keeps running until explictly closed)
   while(!glfwWindowShouldClose(window))     //? Was the window requested to close
   {
      //Process input:

      //Render stuff:
      glClearColor(0.2, 0.7, 0.3, 1.0);      // clear the screen with a color of your choice.
      glClear(GL_COLOR_BUFFER_BIT);          // we're interested in clearing the color (there are also other things)
      // Note that glClearColor is the state setting and glClear is the command.


      //Fixed:
      glfwSwapBuffers(window);               //? Implements double buffer (draw on the back buffer then swap)
      glfwPollEvents();                      //? to respond to events such as resizing, moving, ...
   }

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
   
}


// Configure once per project/adding files. (CMD + Shft + P)
// To build: cmake --build ./build          (./build may be different depending on where you are)
// To run the exe: ./bin/projectname       (depending on where the exe is and its name)

