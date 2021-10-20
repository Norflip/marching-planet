#ifndef __MESH__
#define __MESH__
#include <stdlib.h>

#include "gl.h"

typedef struct Mesh
{
    const char* name;
    size_t vertex_count;
    size_t index_count;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} Mesh;

Mesh* mesh_create_quad (float size, float z);

void create_mesh_from_memory(Mesh* mesh, float* vertices, size_t vertex_count, size_t vertex_stride, unsigned short* indices, size_t index_count);

void mesh_draw (Mesh* mesh);

#endif //__MESH__