#version 330

uniform float time;

layout(location=0) in vec3 position;      // comes from VAO and binded through GLint positionLoc = 0
layout(location=1) in vec4 color;         // comes from VAO and binded though GLint colorLoc = 1;   

out vec4 vColor;                    // Goes to Rasterizer (then to fragment shader)

void main(){
    gl_Position = vec4(position, 1.0);
    vColor = color;
}