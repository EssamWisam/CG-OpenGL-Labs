
   // Alpha Blending & Testing

   #define run 4

   #if run == 0                                                 // 60 < t < 63

      #include "main-0.cpp"                                     // uses simple.frag
        //* The starting point is just lab 6 except that 
        //* 1 - the texture coordinate ranges are back to (0,0) (1,1) (full smiley on square)
        //* 2 - We brought back the alpha channel in the texture (excluded in main-3 of lab 6)
        // (i.e. change in color object, the 3 declared colors ,255} and glTexImage2D call as well RGB->RGBA)
        // 3 - background color was changed to pink as well (for fun.)
        //! Try changing the alpha channel of white in the smiley texture to 0 or 128
        //! Notice that this doesn't change anything.


    #elif run == 1                                             // 63 < t < 65 + 4 + 4

      #include "main-1.cpp"                                     // uses simple.frag
        //* The reason for the problem above is that by default alpha blending is disabled
        //* After enabling we need to specify to openGL how to blend as well.
        //* The method we covered in the lecture was
        //! color = α * (c_src) + (1 - α) * (c_dst) where 
        //c_src = object's RGB (fg shader output) and c_dst = RGB in the color buffer (e.g. background) to be overwritten.
        //* OpenGL generalizes this such that the two operands are not necessarily added (can -, min, max as well.)
        //* but the default is set to addition. It also generalizes α and (1 - α) to F_src and F_dst
        //! color = F_src * (c_src) + F_dst * (c_dst)      
        //* and it sets F_src to 1 and F_dst to 0 by default = no blending <-- need to set as α and 1-α (also many other options)
        //* Thus, our steps are:
        //* 0 - Set the α of white in the smiley face to 0 (transparent) or 128 (50% opacity)
        //* 1 - Enable blending
        //* 2 - Set the operation (as +) and the two F components(as α,(1 - α)) to achieve color = α * (c_src) + (1 - α) * (c_dst)
        //! Now you should see transparency, but why is it only working for one ordering of the squares only?
        // In the render loop we clear (Q) then draw the three squares at a specfic order (Q -> a -> b -> c)
        // In case in the frame being displayed a is closer than b and b is closer than c
        // then due to the depth test only a will be be rendered (b and c) will fail the depth test.
        // so when a applies color = α * (c_src) + (1 - α) * (c_dst) only what has made it to color buffer already 
        // (background only) will be considered for c_dst which is why it doesn't work for this ordering.
        //! Meanwhile, if in the frame being displayed c is closer than b and b is closer than a
        // then when its turn to render c its true that Q then  a then b were rendered already
        // c wins so the color of whatever was being it will be discarded (replaced by that of c) but just before that happens
        // c will apply α * (c_src) + (1 - α) * (c_dst) so its color will be based on the objects behind it already
        // and thus we get transparency.
        //! Conclusions:
        // There's a conflift between depth test and blending 
        //(as depth test may discard stuff may be needed for blending before blending makes use of it)
        // Should be clear that whether or not this happens is dependent on the order of rendering of objects.
        


    #elif run == 2                                          // 73 < t < 76

        #include "main-2.cpp"                                     // uses simple.frag        
        //! What is the solution then?
        // Naive solution is to just always disable depth test-> obviously nonsense as we destroy depth 
        // Another naive solution is to rather disable the smiley faces from writing to the depth buffer. 
        // i.e. have it only enabled for clearing (so smiley faces are drawn on top of it after test)
        //* 1 - re-enable writing on depth buffer
        //* 2 - clearColor and depth 
        //* 3 - disable writing on depth buffer
        // Now smiley faces will be compared to background (drawn in front of it) but not to each other
        // For either solution transparency issue is clearly solved but depth no longer works properly
        // i.e. whatever smiley face is initially drawn in the front stays like that forever (last one drawn in loop)

    #elif run == 3                                      // 76 < t < 97

        #include "main-3.cpp"                                   // uses simple.frag   
        //! By understanding the problem, the real solution is clearly to always draw farther objects first
        // Since as we've seen as long as the object behind is present in the color buffer before being discarded
        // then we'll be able to use it to compute the color of the translucent object in front of it.
        // This yields the following algorithm:
        //* 1 - Draw all opaque objects (α = 255 for all vertices within) 
        //* 2 - For all translucent objects compute the distance to camera/gaze projections and sort them descendingly based on that
        //* 3 - Draw them in that order
        
        // This almost solves it, One problem however is that we're looping on objects
        // If the object is a cube and each face involves some transparency then we will run into the same problem
        // within the cube itself (we have no control over which face gets rendered first.) If the closest one
        // is rendered first then we be able to see the one behind through it even though it should be translucent.
        // An exhaustive solution is to sort triangle by triangle for all triangles forming the cube (too much effort).

        //! An imperfect remedy for this is to have depth writing be disabled while drawing those translucent objects.
        // It solves it because this means that no faces of the cube will be ever hidden in favour 
        // of another one being in front of it (so translucence will apply).
        // It's imperfect because the order of rendering still decides what vertices apply color = α * (c_src) + (1 - α) * (c_dst) on 
        // others first. (e.g. front face is shining through the back face rather than vice versa)
        //!This solution is deemed as acceptable whatsoever. It can also seen to have better properties when it comes
        //! to intersecting objects (compared to having depth write enabled). Intersections are unfavoured regardless.

        //The updated algorithm:
        //* 0 - Have depth test be enabled. All next steps are now in the render loop
        //* 1 - Re-enable writing depth    (i.e glDepthMask(true); )
        //* 2 - Draw all opaque objects    (α = 255 for all vertices within).  
        //* 3 - Disable writing depth      (i.e glDepthMask(false); )
        //* 4 - For all translucent objects compute the distance to camera/gaze projections and sort them descendingly based on that
        //* 5 - Draw them in that order
        //! Note that we didn't implement this in the tutorial (but check main-3 for that.)
        

    #elif run == 4                                              // t > 97

        #include "main-4.cpp"                                               // uses alpha-test.frag   
        //* Suppose we're only interested in true transparency rather than translucency
        //* i.e. it's always either that α=255 or α=0 (e.g. using a cube to draw grass)
        //! then in this case a different approach to blending that completely sidesteps the depth problem is possible
        //! known as alpha-testing.
        //* In this approach we ask the fragment shader to discard pixels that have α=0 (or generally set a threshold)
        // if(frag_color.a < 0.5) discard;
        //* so all such pixels will be discarded and won't make it to the color buffer
        // whatever was in the corresponding positions in the color buffer will just stay there (simulating complete transparency)

        // Steps to accomplish this:
        //* 1 - Set α as 0 for the white texture pixels of the smiley face.
        //* 2 - Use the if condition "if(frag_color.a < 0.5) discard;" in the fragment shader
        // Notice that this requires no blending or anything (we'll do these in main-0) and call it main-4


        //? What if we have objects that are opaque, completely transparent, translucent
        //* If the completely transparent ones are handled with alpha-testing (i.e. if(frag_color.a == 0) discard;)
        //* then we can draw them with the opaque objects (their issue is handled.)
        //* Sorting from far to near would be then done only on translucent objects.


        // Sidenotes:
        // We can achieve multiplication of colors by setting F_src = C_dst, F_dst = 0 (in this case depth order doesn't matter)
        // We're ready to finish phase 2 at this point and missing one lighting lab for phase 3.

   #endif


