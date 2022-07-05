#version 330

uniform mat4 MVP;

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 tex_coord;       // third vertex attribute (texture coordinate)

out vec4 vertex_color;
out vec2 vertex_tex_coord;

void main(){
    gl_Position = MVP * vec4(position, 1.0);
    vertex_color = color;
    vertex_tex_coord = tex_coord;           // send as well to fragment shader.
}