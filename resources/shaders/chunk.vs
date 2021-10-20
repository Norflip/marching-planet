#version 430
layout (location = 0) in vec4 aVertex;
layout (location = 1) in vec2 aUV;
layout (location = 2) in int aLight;
//layout (location = 3) in float aAO;


layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view_proj;

out vec3 worldPos;
out vec2 UV;
out flat int light;

void main()
{
    vec4 m = model * aVertex;
    
    light = aLight;
    UV = aUV;
    worldPos = m.xyz;
    gl_Position = view_proj * m;
}