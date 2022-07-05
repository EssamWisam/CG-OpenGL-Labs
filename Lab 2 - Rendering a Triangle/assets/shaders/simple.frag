#version 330

uniform float time;          // to be seen here as well, need to be declared here as well (consistent type).
uniform int isAnimated;


in vec4 vColor;         // linkage into shader from previous stage (vertex shader)
out vec4 fColor;        // linkage out of a shader to next stage

void main(){
    // Need a vector function to change the color vector (R, G, B, A)
    vec4 tint;
    tint = (isAnimated==0)?  vec4(0.5, 0.5, 0.5, 1.0) : vec4(sin(time), sin(2*time), sin(3*time), 1.0) * 0.5 + 0.5 ;
    // note that the *0.5 and +0.5 apply to 4 elements of the vector.    
    
    // modifying the color by tint.
    fColor = vColor * tint;

    // Now it would make sense if we rather use gl_FragColor to decide the color of each fragment
    // similar to what we did with gl_position in the vertex shader
    // but it's deprecated. (can only be used in compatibility mode, we're on core)

}