#version 330                 // always start with the version

uniform float time;         // Sent from C++ and is constant to the shaders.
uniform int isAnimated;
out vec4 vColor;      // linkage out of a shader to next stage (fragment shader)

void main(){

    // we will stop hard-coding these next lab.
    const vec3 positions[3] = vec3[3](
        vec3(-0.9, -0.9, 0.0),                  // Third dimension has to do with objects covering one another (z: depth)
        vec3( 0.9, -0.9, 0.0),
        vec3( 0.0,  0.9, 0.0)                   // One more comma here yields no error and nothing is drawn.
    );

    const vec4 colors[3] = vec4[3](             // We specified a color for each vertex
        vec4(1, 0, 0, 1),                       // Rasterizer will interpolate colors based on distance for all other vertices of the triangle
        vec4(0, 1, 0, 1),
        vec4(0, 0, 1, 1)
    );

    // gl_Position is a special variable to decide position of each vertex
    // line below is executed for each vertex (gl_VertexID=0,1,2)
    gl_Position = (isAnimated==0)? vec4(positions[gl_VertexID], 1.0) : vec4(positions[gl_VertexID] * sin(time) * exp(0.1*time) , 1.0);

    // Last one is the homogenous coordinate, check lecture for why we need it.
    
    // Assigning a color to each vertex
    vColor = colors[gl_VertexID]; 
}

