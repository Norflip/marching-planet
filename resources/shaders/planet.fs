#version 430
in vec3 worldPos;

out vec4 frag_color;

void main(void)
{    
    vec3 norm = normalize(cross(dFdx(worldPos), dFdy(worldPos)));
    frag_color = vec4((norm + 1) * 0.5, 1.0);
    //frag_color = vec4(1,0,0,1);

}  
