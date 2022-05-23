#version 330


#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2
struct Light {
    int type;               // This will take space 4*int_size (always try to group things into vectors)
    vec3 direction;         // Nonsense incase of point light, cone axis in case of spotlight and the real direction in case of directional.
    vec3 diffuse;          
    vec3 specular;          
    vec3 ambient;           
    vec3 attenuation;               // Used by Point and Spotlight only.
    vec3 position;
    vec2 cone_angles;              // Used by Spotlight only
};
   //* 1 - Define max number of lights 
    #define MAX_LIGHTS 8
   // https://stackoverflow.com/questions/3583735/opengl-lights-limit
    //* 2 - Receive a uniform array Lights (each of Light type) along with their actual count
    uniform Light lights[MAX_LIGHTS];
    uniform int light_count;
   



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

    //* 3 - Loop on the lights in the fragment shader and add the overall effect (4 lines of code):
    frag_color = vec4(0,0,0, 1.0);                                         // will be updated with color
    int clamped_light_count = min(MAX_LIGHTS, light_count);               //! (1) Clamp if exceed max

    for(int i = 0; i < clamped_light_count; i++){                         //! (2) Loop the lights.
    Light light = lights[i];                                              //! (3) Set the current light

    vec3 normal_vector = normalize(fs_in.normal);     

    vec3 ambient = light.ambient * material.ambient;

    vec3 light_vector = (light.type==DIRECTIONAL) ? - normalize(light.direction) : normalize(light.position-fs_in.world_position);


    vec3 diffuse = light.diffuse * material.diffuse*max(0, dot(normal_vector, light_vector));

    vec3 view_vector = normalize(fs_in.view_vector);  

    vec3 reflected = reflect(-light_vector, normal_vector);     

    vec3 specular = light.specular * material.specular * pow(max(0, dot(view_vector, reflected)), material.shininess);

    float attenuation = 1;                          
    if(light.type != DIRECTIONAL)
    {
        float d = distance(light.position, fs_in.world_position);
        attenuation /= (dot(light.attenuation, vec3(d*d, d, 1)));          
     if (light.type == SPOT){
        float angle = acos(dot(-light_vector, light.direction));            

        attenuation *= smoothstep(light.cone_angles.y, light.cone_angles.x, angle);
       
    }
    }

    frag_color.rgb += ambient + attenuation * (diffuse + specular);             //! (4) Add the effects from different lights.
    frag_color.a = 1.0;
    }


}