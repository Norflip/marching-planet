#version 430

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

layout(binding = 0) uniform atomic_uint tris_counter;
layout(std140, binding = 1) writeonly buffer triangle_buffer { Triangle triangles[]; };
layout(std430, binding = 2) readonly buffer density_data { float density []; };

void add_triangle(Vertex a, Vertex b, Vertex c) 
{
    Triangle tri;
    tri.A = a;
    tri.B = b;
    tri.C = c;

    uint i = atomicCounterIncrement(tris_counter);
    triangles[i] = tri;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main ()
{
    ivec3 pos = ivec3(gl_GlobalInvocationID);

    

}