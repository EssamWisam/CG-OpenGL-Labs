#version 330


#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2

//* 1 - Remove the ambient component from the light struct and add it only before entering the loop (remove from loop)
struct Light {
    int type;               
    vec3 direction;         
    vec3 diffuse;          
    vec3 specular;          
    vec3 attenuation;               
    vec3 position;
    vec2 cone_angles;              
};
    #define MAX_LIGHTS 8
    uniform Light lights[MAX_LIGHTS];
    uniform int light_count;
   

//* 2 - Make for it a sky light struct (to be received form main)
struct Sky {
    vec3 top, middle, bottom;
};

uniform Sky sky;


struct Material {
    sampler2D albedo;                   
    sampler2D specular;
    sampler2D ambient_occlusion;       
    sampler2D roughness;                
    sampler2D emissive;                
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

    vec3 normal_vector = normalize(fs_in.normal);     


    vec3 material_diffuse = texture(material.albedo, fs_in.tex_coord).rgb;
    vec3 material_specular = texture(material.specular, fs_in.tex_coord).rgb;
    vec3 material_ambient = material_diffuse * texture(material.ambient_occlusion, fs_in.tex_coord).r;      
    float material_roughness = texture(material.roughness, fs_in.tex_coord).r;
    float material_shininess = 2.0 / pow(clamp(material_roughness, 0.001, 0.999), 4.0) - 2.0;
    vec3 material_emissive = texture(material.emissive, fs_in.tex_coord).rgb;

    
    //* 3 - Choose the light ambient based on the normal at the point.

     vec3 sky_light = (normal_vector.y > 0) ?
        mix(sky.middle, sky.top, normal_vector.y * normal_vector.y) :     // implements middle*(1-n^2) + top(n^2)
        mix(sky.middle, sky.bottom, normal_vector.y * normal_vector.y);   // implements middle*(1-n^2) + bottom(n^2)




    frag_color = vec4(material_emissive + material_ambient * sky_light, 1.0);
    int clamped_light_count = min(MAX_LIGHTS, light_count);               

    for(int i = 0; i < clamped_light_count; i++){                         
    Light light = lights[i];                                              

    //* 1 - Remove the ambient component 
    // vec3 ambient = light.ambient * material_ambient;

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

//* 1 - Remove the ambient component 
    frag_color.rgb +=  attenuation * (diffuse + specular);             
    frag_color.a = 1.0;
    }


}