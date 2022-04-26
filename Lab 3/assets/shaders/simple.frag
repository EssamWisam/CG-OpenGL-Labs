#version 330

uniform float time;

in vec4 vColor;
out vec4 fColor;

void main(){
    fColor = vColor;
}