#version 330

uniform sampler2D tex_sampler;


//* 3 - Receive and use in the fragment shader.
in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} fs_in;

out vec4 frag_color;


void main(){
  
  frag_color = texture(tex_sampler, fs_in.tex_coord);

}