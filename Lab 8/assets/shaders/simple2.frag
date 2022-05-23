#version 330

uniform sampler2D tex_sampler;


in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} fs_in;

out vec4 frag_color;

//* 1 - Add a directional light source and implement diffuse lighting.

void main(){
  float lighting = max(0, dot(fs_in.normal, vec3(0,1,0)));
  frag_color = texture(tex_sampler, fs_in.tex_coord) * lighting;
  //* Before we color any fragment we see what's the angle between its normal
  //* and the vertical axis
  //* The smaller the angle = larger dot product (surface facing the light => intense color (*lightning) )
  //* else the larger the angle (smaller dot product) the less intense it is. = Surface parallel to light source
  //* When the angle is greater than 90 (normal facing opposite direction to light => should not get colored)
  //* Hence, we're maxing between the dot product and 0.

}