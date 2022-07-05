#version 330

uniform mat4 VP;
uniform mat4 M;
uniform mat4 M_IT;
//* 6-1- In the vertex shader send as well an eye location uniform
uniform vec3 eye;


layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 tex_coord;
layout(location=3) in vec3 normal;

out Varyings {                          
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    //* 6-2- Compute the view vector there then send it fg shader as well.
    vec3 view_vector;
    vec3 world_position;                // fragment shader will need this later.
} vs_out;                              



void main(){
    vec3 world_position = (M * vec4(position, 1.0)).xyz;

    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);

    gl_Position = VP * vec4(world_position, 1.0);

    //* 6-2- Compute the view vector there then send it fg shader as well.
    vs_out.view_vector = eye - world_position;
    vs_out.world_position = world_position;

}