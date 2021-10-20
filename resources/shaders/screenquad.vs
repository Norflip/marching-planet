#version 430
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uv;

out vec2 TexCoords;

void main () 
{
    gl_Position = vec4(vertex, 1.0);
    TexCoords = uv;
}