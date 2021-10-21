#version 430
#pragma include "resources/marchtable.glsl"

struct Vertex
{
    vec4 position;
};

struct Triangle
{
    Vertex A;
    Vertex B;
    Vertex C;
};

layout (location = 0) uniform int textureSize;
layout (location = 1) uniform float planetSize;
layout (location = 2) uniform ivec3 chunkCoord;

layout(binding = 0) uniform atomic_uint tris_counter;
layout(std140, binding = 1) writeonly buffer triangle_buffer { Triangle triangles[]; };
layout(std430, binding = 2) readonly buffer density_data { float density []; };


#define FLATTEN(v) ((v.z * textureSize * textureSize) + (v.y * textureSize) + v.x)
#define ISO_LEVEL 0.0

vec3 CellToWorld (ivec3 coord)
{
    return vec3(coord);
    //return (coord * (1.0 / (textureSize - 1.0) - 0.5)) * planetSize;
}

float SampleDensity (ivec3 coord)
{
    coord = clamp(coord, 0, textureSize);
    return density[FLATTEN(coord)];
}

Vertex CreateVertex (ivec3 coordA, ivec3 coordB)
{
    vec3 posA = CellToWorld(coordA);
    vec3 posB = CellToWorld(coordB);
    
    float densityA = SampleDensity(coordA);
    float densityB = SampleDensity(coordB);
    
    float t = (ISO_LEVEL - densityA) / (densityB - densityA);
    vec3 pos = posA + (posB - posA) * t;

    Vertex v;
    v.position = vec4(pos, 1.0);
    v.position = vec4((coordA + coordB) * 0.5, 0.0);
    return v;
}


layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main ()
{
    ivec3 pos = ivec3(gl_GlobalInvocationID);
    ivec3 coord = pos + chunkCoord;

    ivec3 cornerCoords[8];
	cornerCoords[0] = coord + ivec3(0, 0, 0);
	cornerCoords[1] = coord + ivec3(1, 0, 0);
	cornerCoords[2] = coord + ivec3(1, 0, 1);
	cornerCoords[3] = coord + ivec3(0, 0, 1);
	cornerCoords[4] = coord + ivec3(0, 1, 0);
	cornerCoords[5] = coord + ivec3(1, 1, 0);
	cornerCoords[6] = coord + ivec3(1, 1, 1);
	cornerCoords[7] = coord + ivec3(0, 1, 1);

    int cubeConfiguration = 0;
    for(int j = 0; j < 8; j++)
    {
        if(SampleDensity(cornerCoords[j]) < ISO_LEVEL)
        {
            cubeConfiguration |= (1 << j);
        }
    }

   // cubeConfiguration = 32;

    int edgeIndicies[] = triangulation[cubeConfiguration];

    for(int i = 0; i < 16; i += 3)
    {
        if(edgeIndicies[i] == -1)
            break;
    
        Triangle tri;
        
        int edgeIndexA = edgeIndicies[i];
        int a0 = cornerIndexAFromEdge[edgeIndexA];
        int a1 = cornerIndexBFromEdge[edgeIndexA];
        tri.A = CreateVertex(cornerCoords[a0], cornerCoords[a1]);

        int edgeIndexB = edgeIndicies[i + 1];
        int b0 = cornerIndexAFromEdge[edgeIndexB];
        int b1 = cornerIndexBFromEdge[edgeIndexB];
        tri.B = CreateVertex(cornerCoords[b0], cornerCoords[b1]);

        int edgeIndexC = edgeIndicies[i + 2];
        int c0 = cornerIndexAFromEdge[edgeIndexC];
        int c1 = cornerIndexBFromEdge[edgeIndexC];
        tri.C = CreateVertex(cornerCoords[c0], cornerCoords[c1]);
        
        uint tri_index = atomicCounterIncrement(tris_counter);
        triangles[tri_index] = tri;
    }
}