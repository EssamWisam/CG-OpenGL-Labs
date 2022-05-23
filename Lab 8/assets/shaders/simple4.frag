#version 330


//* 1 - Add a type to the light struct in the fragment shader with 3 #defines that govern type
#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2
struct Light {
    int type;               // This will take space 4*int_size (always try to group things into vectors)
    vec3 direction;         // Nonsense incase of point light, cone axis in case of spotlight and the real direction in case of directional.
    vec3 diffuse;          
    vec3 specular;          
    vec3 ambient;           
    //* 1.2- As well add light source position and attenuation (a, b, c) vector
    vec3 attenuation;               // x*d^2 + y*d + z                      // Used by Point and Spotlight only.
    vec3 position;
    //* 1.3 - Add the spotlight cone's inner and outer angles.
    vec2 cone_angles;               // x: inner_angle, y: outer_angle       // Used by Spotlight only
};
uniform Light light;


struct Material {
    vec3 diffuse;          
    vec3 specular;       
    vec3 ambient;         
    float shininess;      
};
uniform Material material;



in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view_vector;
    vec3 world_position;                
} fs_in;

out vec4 frag_color;



void main(){

    vec3 normal_vector = normalize(fs_in.normal);     // We normalized in the vertex shader but interpolated normals may not be normalized.

    vec3 ambient = light.ambient * material.ambient;

    //* 2 - Implement Light Direction:
    vec3 light_vector = (light.type==DIRECTIONAL) ? - normalize(light.direction) : normalize(light.position-fs_in.world_position);
        // -light direction only works for directional light (since it's constant and doesn't depend on object location like the sun)
        // If it's not directional we need to compute the light vector (from point to light source)
        // then use the result in the diffuse and reflect formulae.



    vec3 diffuse = light.diffuse * material.diffuse*max(0, dot(normal_vector, light_vector));

    vec3 view_vector = normalize(fs_in.view_vector);  

    vec3 reflected = reflect(-light_vector, normal_vector);     // -ve light vector is from light to object (as needed here)

    vec3 specular = light.specular * material.specular * pow(max(0, dot(view_vector, reflected)), material.shininess);

    float attenuation = 1;                          
    //* 3 - Implement Attenuation for Point Light
    if(light.type != DIRECTIONAL)
    {
        float d = distance(light.position, fs_in.world_position);
        attenuation /= (dot(light.attenuation, vec3(d*d, d, 1)));          // Implements the quadratic. (1/(ad^2+bd+c))
     
    //* 4 - Extend the implementation for Spotlight
     if (light.type == SPOT){
        float angle = acos(dot(-light_vector, light.direction));            
        // The angle between -light vector (light to obj) and cone axis. (recall dot product of unit vectors = cosine)
        // The wides this is the more attenuation is applied (besides of the attenuation above.)
        
        attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
        // takes (a, b, c)
        // If a<c<b then gradual increase from 0 to 100, smaller than a -> 0, larger than b -> 1
        // Here we have a>b Docs say that the results should be undefined but the TA said it will apply:
        // b<c<a gradual decrease from 100 to 0, less than b -> 1 and more than a-> 0 (this seems to hold.)
    }
    }

    //* 5 - Multiply the computed attenuation by diffuse and specular.
    frag_color.rgb = ambient + attenuation * (diffuse + specular);
    frag_color.a = 1.0;


}