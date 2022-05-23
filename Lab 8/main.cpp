// Lighting
   #define run 9

   #if run == 0                                        

    #include "main-0.cpp"         // 0 < t < ...
      //* The starting point here is different from other labs:
      // We have a utils.cpp (very similar to the one in the project) that has a load_mesh and load_texture function.
      // The load_mesh function loads a 3D model (monkey) called Suzanne
      // It's loaded with positions, colors, texture coordinates and as well "normals"
      // There is as well a load_texture function that reads an image as a texture (./assets/images/albedo)
      //* In the main:
      // Two functions create_mesh_from_data and create_texture_from_data externalize the logic we used to do 
      // in the past to pass vertices, colors, texture coordinates, normals to the GPU and
      // loading a texture there to be placed on the 3D model.
      // The output from the utility load_mesh and load_texture function is what gets input to these.
      // There are two functions that are not used: create_plane and create_single_color
      // Finally, drawMonkey renders the model at the origin and lets the camera rotate around it.
      //! Try to run to see the model in action.
      //! At the moment the model can't be part of a realistic scene since it only renders a fixed texture on it
      //! with no account of light.

    #elif run == 1                // ... < t < 5                  //Uses simple1 vertex and frag. shaders
      #include "main-1.cpp"
      // Main didn't change.
      // We didn't use the normals sent from the main yet.  
      //* 1 - Add a layout qualifier for normals in the vertex shader
      //* 2 - For modularity, output color, texture coordinates, normal in one struct.
      //* 3 - Receive and use in the fragment shader.
      // No difference should be noticed upon running

    #elif run == 2                // 5 < t < 7                  //Uses simple2 fragment shader (and simple1 vertex)
      #include "main-2.cpp"
      //  Will implement diffuse light with a directional light source
      //* 1 - Add a directional light source and implement diffuse lighting in the fragment shader.
      // "directional" and "diffuse" will make sense later (or if you've seen the lecture = can be worth it)
      // The results should align with your understanding of the logic we wrote in the fragment shader

    #elif run == 3              // 7 < t < 9                        
        #include "main-3.cpp"                                         
        // The output from main-2 looks good but that's not the case implementation wise.
        // What if we make the monkey itself rotate?
        //* 1 - Make the monkey rotate about y (besides of how the camera is rotating)
        // Yaw = 0, Pitch = angle (rotate about y) and roll is 0
        // Multiply it instead of the translate
        // It's as if the light is going around with monkey => we didn't apply the transformations on the normals.
        // (for each vertex it's using the local normal value of the monkey => without the transformation)


    #elif run == 4                                      

        #include "main-4.cpp"                     // 9 < t < 14             Uses simple2.vert (and simple2 frag)
        //! Let's Fix the issue in main 3
        // We just need to be transforming normals as well.
        // Recall that if M is what takes points/tangents from the local to world space
        // then Trn(Inv(M)) is what will take the normals from the local to world space
        // Thus,
        //* 1 - In the vertex shader receive uniforms for M, M^IT, VP
        //* 2 - Apply M on the positions and M^IT on the normals
        //* 3 - Further apply VP on the positions (for normals there job: lighting will be over in the world space)
        //* 4 - Now in the main, get rid of MVP and send VP, M, M^IT instead (there also a useless tex_sampler)
        
        // Issue solved! The normals keep changing from second to second and only points with normals
        // that are vertical get maximum illumination.
       

    #elif run == 5                                                // 14 < t < 33      
        #include "main-5.cpp"                                     // Uses simple3 frag and vert.              
       //*What we have just simulated is diffuse reflection where ligh upon hitting an object reflects equally in all directions.
       // i.e. regardless to viewing angle the perceived color (reflection is the same).
       // What we calculated (the max(...)) is called the lambert component. Lambertian Reflection = Ideal Diffuse Reflection (which we're assuming)

       //* There is also specular reflection that simulates reflection due to smooth surfaces.
       // It's component is called Phong and depends on the viewing angle (is it towards the perfectly reflected ray? => max intensity)
       // Otherwise, the intensity depends on the angle between the view vector (point to camera) and perfect reflected ray

       //* We as well need a way to simulate indirect light.
       // i.e. when we see objects due to light reflected from other objects (even if light from light source can't reach them directly)
       // expensive solutions exist but the Phong model (our model for lighting) rather adds an ambient component (constant light throughout the whole scene)
       // so that even if light can't reach an object, it emits some dim color (but not be entirely black = unnatural)
       // In essence, the Phong model computes the three components then adds them together to perceive the overall effect of light.
       // This is explained in detail in the lecture. Or https://en.wikipedia.org/wiki/Phong_reflection_model

       //! Now steps we'll take to implement the Phong Model:

       //* 1 - Remove texture from fragment shader.   (simple3)
       // We will give it our color through the Phong model.

       //* 2 - Define a directional light struct (uniform) with the three light components and its constant direction. 
       // This define the color that light uses to affect object

       //* 3 - Define a struct (uniform) as well for how the object responds to each 3 types of light.
       // k constants in the Phong model = By how much is the specific object at hand affected by each type of light
       // diffuse, specular, ambient, shineness/specular power

       //* 4 - Compute the ambient component of the Phong model

       //* 5 - Compute the Diffuse component (did that before) 
       // -> care for the -ve sign (we'll use the actual light direction)

       //* 6 - Compute the specular component 
       // Need view vector and reflection vector => compute them first and come back:

       //* 6-1- In the vertex shader send as well an eye location uniform
       // Will need it to compute the view vector that will be needed for specular

       //* 6-2- Compute the view vector there then send it fg shader as well.
       // We made use of the world's position

       //* 6-3- Receive the view vector in fragment shader and normalize it there
       // Normalizing view there in vertex shader

       //* 6-4- Compute the reflection vector for specular and then the specular component.

       //* 7 - Combine (add) the three components as the fragment's color = implemented the Phong Model

       //! Still have some uniforms to send in main:

       //* 8 - Send the uniform for eye position to vertex shader
       //=> sent as eye.x, eye,y, eye,z  # sent to vertex
       //* 9 - Send to the fragment shader the light and material structs and set their values:
       //! At this point we have over 8 parameters to toy with (can beyond realism = artistic freedom):
       // Light direction = Well defined (the needed direction of light)
       // Light diffuse and specular components should be equal (unless we want a white object to look red but shine green)
       // We sometimes discard specular (to put lighting with no reference from where its coming = fill light in games)
       // Ambient is a weaker version of them (simulate indirect light)
       // Now for the material itself:
       // Material Ambient and Diffuse are typically the same (both go in all direction) = They represent the object's color (for white light)
       // Material Specular = low with small shineness for rough  materials (don't react to specular light)
       // Material Specular = monochromatic(e.g. white) for dielectric materials that can shine like plastic (don't react to specular light)
       // Material Specular = high (representing the material's color) for metals and with low diffuse. [Don't totally agree with this, diffuse still rules the overall color]
       // Dielectric's shine with a monochromatic color meanwhile metals shine with their own color (e.g. gold)

       //* Now run to see the Phong Model in action.
       



    #elif run == 6                                //33 < t < 53    Uses simple4 frag
        #include "main-6.cpp"    
        //* We have been assuming directional light only (always has one direction and doesn't get attenuated). But there are other types.
        // Directional light is like sun: light always seems to be in the same direction.

        // There is also point light with one location and no particular direction.
        // Point light => as we get closer light becomes more intense and vice versa
        // The rule is 1/d^2
        // For more freedom we apply 1/(ad^2+bd+c) where a, b, c are constants we choose (e.g. b=c=0 for realism)
        // e.g. just constant => even if far, same intensity. or just liner change => slow decay
        // last type of light = spotlight is like torch.
        // has a cone with inner angle and outer angle.
        // inside inner angle: 100% light
        // outer angle: 0% 
        // Between both 100% to 0% gradually.
        // Let's implement the other two types of light as well!

        //* 1 - Add a type to the light struct in the fragment shader with 3 #defines that govern type
        //* 1.2- As well add light source position and attenuation (a, b, c) vector
        //* 1.3 - Add the spotlight cone's inner and outer angles.

        //* 2 - Implement Light Direction

        //* 3 - Implement Attenuation for Point Light

        //* 4 - Extend Implementation for Spotlight

        //* 5 - Multiply the computed attenuation by diffuse and specular.

        //* 6 - Send the needed light uniforms from the main (and let's turn off camera rotation)

        // Now when you run the whole monkey is black (except for the spot that received light from the spotlight)
        // <Perhaps> It might feel that the spotlight is following the monkey when it really isn't
        // It's just due to the monkey's geometry and where the light is in the world space.
        // Try using a larger spotlight or point source to confirm. </Perhaps>


    #elif run == 7                                //53 < t < 57           Uses simple5 frag
        #include "main-7.cpp"   
        // What if we want multiple light source.

        //* 1 - Define max number of lights (According to the GPU/OpenGL) 

        //* 2 - Receive a uniform array Lights (each of Light type) along with their count in the FG shader.

        //* 3 - Loop on the lights in the fragment shader and add the overall effect:

        //* 4 - In main send light_count and lights[0], lights[1] (define two light sources)

        // Now run to see the effect of both light sources.


    #elif run == 8                            // 57 < t < 70         uses simple6.frag
      #include "main-8.cpp"
      // We have been assuming no textures so far.
      // Have material diffuse, specular, ambient and shineness => can provide a texture for each (and even one more for emmisiveness)

      // So 5 textures overall:
      // 1 - Albedo -> Used to sample the material's diffuse
      // 2 - Specular -> to sample material's specular
      // 3 - AmbientOcclusion -> Ambient = AmbientOcclusion (accounts positions receiving little ambient) * Diffuse (previously diffuse = ambient)
      // 4 - Shineness ranges from 0 to infinity which is not supported by any texture: But we can use roughness (from 0 to 1) and convert it.
      // 5 -  emmisive. (Magma, TV things that output color)
      // We will get them from: assets/images

      //* 1 - Modify Material to include 5 Sampler2Ds
      //* 2 - Compute the 5 quantities by sampling the corresponding texture
      //* 2.1 - Compute Shininess from Roughness
      //* 3 - Get material Specular, Ambient, Roughness (float) -> shineness, Emissiveness Similarily.

      //* 3 - Use the computed quantities in Phong model (material. becomes material_ in the loop)

      //* 4 - Create textures for each in the main (5 textures)

      //* 5 - Send each of the textures in the render loop. (and comment the material uniforms)

      // Notice that they eyes are glowing because of the emmisive texture


      #elif run == 9                            // 70 < t < ...         uses simple7.frag
          #include "main-9.cpp"
          // We can improve our implementation of ambient by introducing "skylight"
          // That is ambient light that changes based which direction is the object facing (towards the sky or not)

          // Because it's not a light component (it's constant through the scene) we will remove it from the light struct
          // and add it only once before entering the loop
          // and make for it a seperate sky light struct with the color vectors top, middle, bottom
          // If the point is facing the sky the ambient it gets is a mixture between top and middle
          // otherwise its a mixture between middle and bottom.

          //* 1 - Remove the ambient component from the light struct and add it only before entering the loop (remove from last line)
          //* 2 - Make for it a sky light struct.
          //* 3 - Choose the light ambient based on the normal at the point.

          //* 4 - In the main, send a seperate sky uniform and don't sent ambinet as a light component.


          // sending material for multiple objects = Texture Switching.
          // Although shaders (program) should be sharing light but we will need to send uniforms for each


   #endif


