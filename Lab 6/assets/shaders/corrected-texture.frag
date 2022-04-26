#version 330

uniform sampler2D tex_sampler;      
in vec4 vertex_color;
in vec2 vertex_tex_coord;               // receive texture coordinate of pixel as well.
out vec4 frag_color;

void main(){
    // frag_color = vertex_color;
    frag_color = texture(tex_sampler, vertex_tex_coord);
    // instead of screen location (unlike texelFetch) it takes the texture coordinates.
    // recall that in main we corresponded each of the four vertices of the square to the four vertices
    // of the smiley face by having them be at the order/row.
    // !These are used to interpolate the corresponding texture coordinates to all fragments

    
}

