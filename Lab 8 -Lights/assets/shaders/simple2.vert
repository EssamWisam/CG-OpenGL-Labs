#version 330

//* 1 - In the vertex shader receive uniforms for M, M^IT, VP instead of just MVP
uniform mat4 VP;
uniform mat4 M;
uniform mat4 M_IT;

layout(location=0) in vec3 position;
layout(location=1) in vec4 color;
layout(location=2) in vec2 tex_coord;
layout(location=3) in vec3 normal;

out Varyings {                          
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} vs_out;                              



void main(){
    //* 2 - Apply M on the positions 
    vec3 world_position = (M * vec4(position, 1.0)).xyz;

    vs_out.color = color;
    vs_out.tex_coord = tex_coord;

    //* 2 - and M^IT on the normals 
    // Normalize because we'll assume that later
    // Notice that we casted to vec4 to multiply by the matrix and then casted back to vec3 (by taking .xyz)
    vs_out.normal = normalize((M_IT * vec4(normal, 0.0)).xyz);

    //* 3 - Further apply VP on the positions (for normals their job (lighting) will be over in the world space)
    gl_Position = VP * vec4(world_position, 1.0);

}