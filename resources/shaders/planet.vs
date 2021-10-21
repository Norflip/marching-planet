#version 430
layout (location = 0) in vec4 aVertex;

layout (location = 0) uniform mat4 projection_view;
layout (location = 1) uniform mat4 model;

out vec3 worldPos;

void main()
{
    vec4 m = model * aVertex;
    vec4 p = projection_view * m;

    worldPos = m.xyz;
    gl_Position = p;
}