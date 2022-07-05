#version 330


#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2
struct Light {
    int type;               
    vec3 direction;         
    vec3 diffuse;          
    vec3 specular;          
    vec3 ambient;           
    vec3 attenuation;               
    vec3 position;
    vec2 cone_angles;              
};
    #define MAX_LIGHTS 8
    uniform Light lights[MAX_LIGHTS];
    uniform int light_count;
   





//* 1 - Modify Material to include 5 Sampler2Ds ()

struct Material {
    sampler2D albedo;                   // Diffuse
    sampler2D specular;
    sampler2D ambient_occlusion;       
    sampler2D roughness;                // Will get shineness from this
    sampler2D emissive;                 // For objects that emit light (e.g. Magma, TV,..)
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


    //* 2 - Compute the 5 quantities by sampling the corresponding texture

    vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;      
    // only the red component because the texture is greyscale
    float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    //* 2.1 - Compute Shininess from Roughness
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;
    // When roughness is 0 shineness is infinity and when its 1 shineness is 0.
    // Hence, a roughness texture from [0,1] can map to any possible shineness 
    // We will still clamp the value to stay away from 0 and infinite shineness (esp. infinite)

    vec3 material_emissive = texture(material.emissive, fs_in.tex_coord).rgb;



//* 3 - Use the computed quantities in Phong model (material_ becomes material_ in the loop)

    frag_color = vec4(material_emissive, 1.0);                  // emmisive is independent of any light source.                                 
    int clamped_light_count = min(MAX_LIGHTS, light_count);               

    for(int i = 0; i < clamped_light_count; i++){                         
    Light light = lights[i];                                              

    vec3 normal_vector = normalize(fs_in.normal);     

    vec3 ambient = light.ambient * material_ambient;

    vec3 light_vector = (light.type==DIRECTIONAL) ? - normalize(light.direction) : normalize(light.position-fs_in.world_position);


    vec3 diffuse = light.diffuse * material_diffuse*max(0, dot(normal_vector, light_vector));

    vec3 view_vector = normalize(fs_in.view_vector);  

    vec3 reflected = reflect(-light_vector, normal_vector);     

    vec3 specular = light.specular * material_specular * pow(max(0, dot(view_vector, reflected)), material_shininess);

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

    frag_color.rgb += ambient + attenuation * (diffuse + specular);             
    frag_color.a = 1.0;
    }


}