#version 430

layout (location = 0) uniform int textureSize;
layout (location = 1) uniform float planetSize;

layout(std430, binding = 0) writeonly buffer density_data { float density []; };

float CalculateDensity (uvec3 index)
{
    float hs = textureSize * 0.5;
    float max_dist = length(vec3(hs, hs, hs));
    vec3 worldpos = vec3(index) - vec3(hs, hs, hs);

    const float NUDGE = 1.0f;
    return length(worldpos) / (max_dist + NUDGE) - 0.5;
}

#define FLATTEN(v) ((v.z * textureSize * textureSize) + (v.y * textureSize) + v.x)

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main ()
{
    uvec3 cell = gl_GlobalInvocationID;
    float d = CalculateDensity(cell);

    uint index = FLATTEN(cell);
    density[index] = d;
}