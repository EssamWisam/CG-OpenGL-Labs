#version 330

uniform sampler2D tex_sampler;              // discussed in main.
in vec4 vertex_color;
out vec4 frag_color;

//* 3 - Fragment Shader Action

void main(){
   frag_color = texelFetch(tex_sampler, ivec2(vec2(gl_FragCoord.xy) / 100), 0) * vertex_color;
   // Takes the sampler bound to the texture that we will draw colors from (sample)

   // then it takes a vector in the texture's coordinates. we had an 8x8 texture so its expecting
   // (i,j) where each is an integer from 0 to 7
   // !But what are we passing?
   // For each fragment (pixel) corresponding to the object being rendered (square team)
   // we sent the pixel's (x,y) coordinates -> these can take large values: upper-right: (799.5, 599.5)
   // Thus we divide by 100 then take the nearest integer -> at most (7,5) so we always get a position
   // in terms of texture coordinates.
   
   // Last parameter is level of detail in the mipmap 0 means the baselevel.

   //! Observe by running that indeed, the color drawn depends on 
   // 1 - whether the pixel in the screen has the object or not (fragment shader only called on the object)
   // 2 - if it has an object then where is it in SCREEN COORDINATES
   // That's why it feels that the texture is more of a background on the cubes.
    
    //! Also note that we're not doing anything with the colors we set in the main in lab 2.
    // can multiply texelFetch by it for a nice effect.


}
