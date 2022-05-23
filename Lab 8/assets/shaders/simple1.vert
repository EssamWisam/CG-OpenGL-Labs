#version 330

uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 tex_coord;
//* 1 - Add a layout qualifier for normals in the vertex shader
layout(location=3) in vec3 normal;

//* 2 - For modularity, output color, texture coordinates, normal in one struct.
out Varyings {                          //! <------ Notice that this name should be the same across both shaders.
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} vs_out;                               //! <------ This one can be different (it's fs_in there)



void main(){
    gl_Position = MVP * vec4(position, 1.0);
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = normal;

}