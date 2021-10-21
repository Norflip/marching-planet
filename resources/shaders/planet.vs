#version 430
layout (location = 0) in vec4 aVertex;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view_proj;

out vec3 worldPos;

void main()
{
    vec4 m = model * aVertex;
    worldPos = m.xyz;
    gl_Position = view_proj * m;
}