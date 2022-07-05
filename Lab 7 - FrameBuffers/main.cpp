   // Framebuffers
   

   #define run 3                    // change this variable to travel in time through the tutorial.

   #if run == 0                     // 0 < time < 12

      #include "main-0.cpp"       

      //* The starting point is just lab 6 except that the texture coordinate ranges are back to (0,0) (1,1)
      //* Check the discussion on the perspective transform & z-fighting in the draw function.


   #elif run == 1                   // 12 < time < 28

      #include "main-1.cpp"        
      //* A framebuffer is an area in memory that can be rendered to
      //* Great thing about them is that they allow us to render a scene directly on a texture
      //* we can then further use such texture in our main scene (rendered on screen)
      //* the result is "scenes within scenes". e.g. Game scene with a T.V. inside or a mirror.
      //* More broadly, they allow us to do flexible off-screen rendering which can be used for cool preprocessing effects.

      //* Steps we will take:
      //* 1 - Create and bind an empty texture (we want to render something on this)
      //* 2 - Create and bind a frame buffer then attach it to that texture
      // At this point, any rendering we do in the main loop will render to the bound framebuffer
      // which we can think of as a virtual screen. Since we've attached the FB to the texture all
      // that we render actually goes to that texture.
      //! 3 - Now we can render a scene within another by letting our render loop have two passes
      //* 3.1 - In the 1st pass we render while binding to the frame buffer object we created
      //* 3.2 - In the 2nd pass (2nd half of the loop ) we render using the default frame buffer 
      //* unlike our other frame buffer object what we render here goes directly to the screen.
      //! In the 1st pass we render a scene as normal = rendering on the texture
      //! In the 2nd pass we can use that texture which is the scene we've just rendered off-screen
      //* This results in a scene within another.

   #elif run == 2                   // 28 < time < 42

      #include "main-2.cpp"     
      //* If you look closely in the previous run's output you'll notice that depth test is not working
      //* within each of the render targets. The calls glClearDepth(1.0); and glClear(GL_DEPTH_BUFFER_BIT); (first two)
      //* have no effect on our render target. (but glClearColor and GL_COLOR_BUFFER_BIT do)
      //! Recall that when we corresponded the texture renderTarget to the frame buffer
      //* we specified that this is a "color attachment". It recorded the rendered colors as expected.
      //* for it to record the corresponding depths as well we need another texture with a "depth attachment" 
      //* of the same size. It will simply record the depth corresponding to each point in the color attachment.
      //* Problem solved with steps:
      //* 1 - Create and bind an empty texture to act our depth attachment.
      //* 2 - Create and bind a frame buffer then attach it to that texture.
      //* Now depth test, glClearDepth,... work as expected 
      //* (OpenGL disables depth test by default if no depth texture is provided (main-1))

      //! Note that the depth attachment is a monochrome image in itself (has z-values)
      //* hence is sometimes is made use of (rendered) to provide a "foggy effect"

      //*Although out of scope, you might be interested as well in reading about the stencil buffer
      //* (masked drawing). In this case GL_DEPTH_STENCIL_ATTACHMENT let's you use one texture for both

   #elif run == 3                   // 42 < time < 60

      #include "main-3.cpp"     
      //* Recall that in main-1 an unjustified change we did was make the minification filter parameter
      //* be GL_Nearest from GL_NEAREST_MIPMAP_LINEAR[about choosing mipmaps] <-  Note that in the render loop were' using same sampler for both textures
      //* We did this since later for renderTarget we specified only one mip-level but GL_NEAREST_MIPMAP_LINEAR
      //* chooses the two mipmaps that most closely match the textured pixel....
      //! So it's either that the API will handle it using just 1 mip-level (probably in an unexpected way)
      //* or that we'll get an error (it's the former)
 

      //* Solution is clearly hence is to store all possible miplevels -> for a 200x200 pictures
      //* we have log2(200) + 1 miplevels (can keep divind by half for log2(200) times + 1 for the base level)
      //* i.e. instead of passing 1 to glTexStorage2D pass it that number
      //! Note that we're not interested in drawing renderTargetDepth and hence needn't do the same for it.
      //* Recall as highlighted earlier that glTexImage2D generates only one mipmap and we follow with glGenerateMipmap
      //* to generate them all and have the appropriate one be used according to distance.
      //* meanwhile, glTexStorage2D generates them all within the function (we just pass how many)
      //* and if we want to regenerate them then we should call glGenerateMipmap (it generally acts on bound texture target)
      //! Since renderTarget changes every frame (unlike the fixed smiley) we'll need to regerate every render loop.

      //* This makes our two steps)
      //* 0 - Set minification again to GL_NEAREST_MIPMAP_LINEAR
      //* 1 - Compute the no. of miplevels and pass it to glTexStorage2D
      //* 2 - Regenerate them before each time renderTarget is bound in the loop

      //! Now what happens if we comment the step 2?
      //* vertices in renderTarget use black miplevels (they are initially set to 0 and we didn't regenerate.)

      //* Sidenote:
      //* As highlighted earlier, unlike glTexStorage2D creates immutable storage
      //* this means that we can't change the size or format of our base texture (but can still change data -> glTexSubImage2D.)
      //* meanwhile glTexImage2D allocates mutable storage (can change texture's size, format, data)
      //* Note that storage != storage contents -> it refers to the logical process of acquiring resources 
      //* for those contents (like malloc/realloc).
      //* Similarly glBufferStorage is immutable meanwhile glBufferData is mutable.

      //* Why do we say that framebuffers aren't actually buffers?
      //* Because what actually store the data are the textures.
      //* Similar to how a vertex array object encapsulates the state needed to know where the data is stored
      //* (vertex buffer object) but doesn't store the vertex data itself. The frame buffer object
      //* only holds the attachments to (color, depth,..) which actually store the data.
      //* In fact, a lot of API dislike the existence of frame buffers and provide ways to directly draw on texture

      //* Other applications to framebuffers:
      //* We considered the scene within another application.
      //* The broader one is post-processing effect.
      //* 1 - Do off-screen rendering of the same scene but with a different shader 
      //* (e.g. one that further applies a distortion effect on vertices)
      //* 2 - upon trigger of some even render that instead of default scene
      //* Another effect: If interested in a true foggy effect consider adding the color and depth textures.
      //* The last requirement in the project is about this.


   #endif
