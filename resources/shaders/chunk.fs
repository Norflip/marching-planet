#version 430
in vec3 worldPos;
in vec2 UV;
in flat int light;

out vec4 frag_color;

//layout(binding = 0) 
uniform sampler2D atlas;

#define AO_STRENGTH 0.8
#define BASE_LIGHTNING 0.2

void main(void)
{    

    frag_color = vec4(UV, 0, 1);
    //return;

    float light_level = clamp(float(light) / 15.0, 0, 1);
    float llvl = BASE_LIGHTNING + (1.0 - BASE_LIGHTNING) * light_level;

    
//    vec3 norm = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
    
    /*
    frag_color = vec4((norm + 1) * 0.5, 1.0);
    frag_color *= llvl;
    frag_color.a = 1.0;
    return;
    */
    
    /*

    vec3 sun = normalize(vec3(-1, 1 , -1));
    float d = max(dot(sun, norm), 0);
    d = (d + 1.0) * 0.5;  //remap
    */
    

/// TEXTURUURURUR
    vec4 texture_color = texture(atlas, UV);

    frag_color = texture_color * llvl;
    frag_color.a = 1.0;










    /*
    vec3 sun = normalize(vec3(-1, 1 , -1));

    float d = max(dot(sun, normal_o), 0);
    d = (d + 1.0f) * 0.5f;  //remap

    vec4 ao = vec4(1.0 - color_o.a * AO_STRENGTH);
    ao.a = 1.0;

    vec4 color = (texture(image, uv_o) * vec4(color_o.rgb, 1.0)) * ao;
    frag_color = (color) * (d);
    */

}  
