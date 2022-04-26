#version 330

uniform sampler2D tex_sampler;      // will need to define this in main.

// won't find the option.

in vec4 vertex_color;
in vec2 vertex_tex_coord;

out vec4 frag_color;

void main(){
     frag_color = vertex_color;
    //frag_color = texture(tex_sampler, vertex_tex_coord);
    // instead of location (unlike texelFetch) it takes the texture coordinates.
    // draws three upside down smiley faces.
    // upside down because openGL considers the last row as first and so on.


    // texelFetch(tex_sampler, ivec2(vec2(gl_FragCoord.xy) / 100), 1);      // choosing 
    // takes extra variable lod (level of deviates = 0 )
    // has to do with the other level
    // for each pixel (in the square) decide a texture pixel. vertex -> screen coordinate -> draw
    // why divide by 100:
    // the texture is small
    // now for pixel at (300, 400) it will use texture at (3, 4)
    // i vector because 350/100 = 3.5
    // level 1 here as well???


    // Not good it will choose a texture pixel based on location in the screen. (gl_FargCoord)


}