#version 330

//* 2 - Define a directional light struct (uniform) with the three light components and its constant direction. 
struct Light {
    vec3 direction;
    vec3 diffuse;           // Materials with high diffuse coefficient will be most affected by this light color.
    vec3 specular;          // Materials with high specular coefficient will be most affected by this light color.
    vec3 ambient;           // Light constant everywhere in the scene 
};
uniform Light light;


//* 3 - Define a struct (uniform) as well for how the object responds to each 3 types of light.
struct Material {
    vec3 diffuse;          // How does the material react to Diffuse light (percentage reflect along each direction)
    vec3 specular;         // How does the material react to Specular light (percentage reflect along each direction)
    vec3 ambient;         // How does the material react to Ambient light (percentage reflect along each direction)
    float shininess;      // How small is the Specular highlight in the material. AKA Specular Power.
};
uniform Material material;



in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;

    //* 6-3-  Receive the view vector in fragment shader and normalize it there
    vec3 view_vector;
    vec3 world_position;          // will be needed later.         
} fs_in;

out vec4 frag_color;


//* 1 - Remove texture from fragment shader.
// uniform sampler2D tex_sampler; Also deleted what used to be in the main that used texture.

void main(){


    vec3 normal_vector = normalize(fs_in.normal);     // We normalized in the vertex shader but interpolated normals may not be normalized (they shorten)

    //* 4 - Compute the ambient component of the Phong model
    vec3 ambient = light.ambient * material.ambient;

    //* 5 - Compute the Diffuse component (did that before) 

    vec3 diffuse = light.diffuse*material.diffuse*max(0, dot(normal_vector, -light.direction));
    // -ve sign because this time we'll put the actual light direction (not from object to light)

    //* 6 - Compute the specular component 

    //* 6-3- Receive the view vector in fragment shader and normalize it there.

    vec3 view_vector = normalize(fs_in.view_vector);  // Should be normalized here and not the vertex shader.

    //* 6-4- Compute the reflection vector for specular and then the specular component.

    vec3 reflected = reflect(light.direction, normal_vector);

    vec3 specular = light.specular * material.specular * pow(max(0, dot(view_vector, reflected)), material.shininess);

    //* 7 - Combine (add) the three components as the fragment's color = implemented the Phong Model

    frag_color.rgb = ambient + diffuse + specular;
    frag_color.a = 1.0;


}