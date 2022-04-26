// Textures
// Textures allow us to create complex shapes without using too many vertices with different colors
// A texture is an array of pixels (1 or 2 or 3D). We will be interesed in 2D (which are just images)
   
   
   
   #define run 8                    // change this variable to travel in time through the tutorial.

   #if run == 0                     // time = 0

      #include "main-0.cpp"        // Uses simple.vert and simple.frag

      //* The starting point is just lab 4 except that the drawing logic is now in a seperate function
      //* and there is an extra piece of code in compile shader for debugging purposes.


   #elif run == 1                   // 0 < time < 34

      #include "main-1.cpp"        // Uses simple.vert and screen-texture.frag

      //* We created a texture here and used it in the fragment shader.

   #elif run == 2                   // 34 < time < 39
      
      #include "main-2.cpp"          // uses texture.vert and corrected-texture.frag
      //* Solved the problem of aligning the texture to the object rather than the screen by introducing
      //* texture coordinates and using texture() instead of texelFetch()

   #elif run == 3                   // 39 < time < 55

      #include "main-3.cpp"          // uses texture.vert and corrected-texture.frag
      //* Removed the alpha channel which wasn't used to demo the significance of glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      //* Try to uncomment the line and see what happens.

   #elif run == 4                   // uses texture.vert and corrected-texture.frag

      #include "main-4.cpp"         // 55 < time < 62
      //* If you notice, prior to this run the smiley face was feeling a little bit blurry.
      //* All we did here is use glTexParameteri to change the magnification filter parameter

   #elif run == 5

      #include "main-5.cpp"            // 62 < time < 66
         //* We can do better by not relying on glTexParameteri and rather having a seperate sampler object
         //* Generate sampler -> set parameters -> bind to active texture.
         // output is like last stage as expected.

   #elif run == 6

      #include "main-6.cpp"            // 66 < time < c
      //* We explored texture wrapping.
      //* Occurs whenever the texture coordinates associated with vertices exceed the (0,0) (1,1) range.
      //* We first tried making them go from (-1,-1) to (2,2) (check vertices object)
      //* What it does in this case is just ignore the fractional part of each coordinate
      //* Thus (1.5,1.5) becomes (0.5, 0.5) -> the section of the square from ]1,1[ to ]2,2[
      //* is mapped to ]0,0[ to ]1,1[: i.e. a whole texture image is formed in this section.
      //* since we made the range from (-1,-1) to (2,2) we expect 3 repetitions along the 2 square axes.
      //! We can as well change the default behaviour, uncomment the line to learn more.

#elif run == 7

      #include "main-7.cpp"            // c < time < 75
      //* This time we let the range be from (0,0) to (100,100) -> texture gets repeated 100*100 times in the square
      //* and it then feels just like a noisy square
      //* Changing the minification filter parameter is the solution.
      //! try uncommenting the line to fix the noise
      
#elif run == 8                         // last run

      #include "main-8.cpp"            // 75 < time < 87
      //* This time we let the range be from (0,0) to (20,20) -> texture gets repeated 400 times in the square
      //* It can be observed at this point that its using different mipmaps for points in the object that are farther away?
      //! But how?                                                            # decrypting in progress.
      // Fragment shader considers 4 fragments at a time (what the GPU does) 
      // frag_color = texture(tex_sampler, vertex_tex_coord); exploits the fact by computing 
      // discrete partial derivatives (difference) along the x and y texture coordinates (consider vertex above and to the right)
      // if the difference is large (e.g. (3,5) and (3, 15)) -> need a larger level of detail
      // so we need a larger mipmap

      // Note that this implies that its essential that texture() is called for all fragments.
      // i.e. do not put it in an if condition (compute then decide whether or not you'll use the result)
     

      
   #endif

 // The rest of the tutorial was recap of things covered earlier that were mentioned again in slides.
      // demonstrating using textures to make 3D stuff
      // GL_LINEAR_MIPMAP_LINEAR -> blurred floor when away. (most realistic and computationally intensive)
      // How many pixels are needed in linear filtering? 4 then wt. average
      // How many pixels in linear_mipmap_linear filtering? 8 (4 from each mipmap then wt avg then wt avg results )

      // More anisotropy would cause less blurring for the floor when far away.
      // can be set in glTextParameterf

      // Texture as 2D array can represent more than just color.
      // e.g. set it to a value that decides if vertices should be shifted up (3D mountain)

      // In the mountain example it uses 2 different textures (rock and grass) -> interpolates between both
      // based on height of vertex. (can also use a step function for a less smooth effect)
      // In the code, we drew the image by ourselves can also just load it
      