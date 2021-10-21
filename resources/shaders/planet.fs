#version 430
in vec3 worldPos;
in vec3 normal;
layout (location = 8) uniform vec3 color;

out vec4 frag_color;

void main(void)
{    
   // vec3 norm = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
   
    frag_color = vec4(color * ((normal + 1) * 0.5), 1.0);
}  
