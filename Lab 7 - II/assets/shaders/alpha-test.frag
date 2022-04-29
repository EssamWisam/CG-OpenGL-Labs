#version 330

uniform sampler2D tex_sampler;

in vec4 vertex_color;
in vec2 vertex_tex_coord;

out vec4 frag_color;

void main(){
    frag_color = texture(tex_sampler, vertex_tex_coord);
    //* 2 - Use the if condition to discard transparent pixels.
    if(frag_color.a < 0.5) discard;
}