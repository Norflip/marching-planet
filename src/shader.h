#ifndef __SHADER__
#define __SHADER__
#include <stdbool.h>
#include "gl.h"

typedef struct Shader
{
    GLuint program;
    const char* vs;
    const char* fs;
    GLenum cullMode;
    bool transparancy;
} Shader;

typedef struct ComputeShader
{
    GLuint program;
    const char* path;
} ComputeShader;

// add includes

Shader* shader_load (const char* vs, const char* fs);
void shader_reload (Shader* shader);
void shader_bind (Shader* shader);
void shader_destroy(Shader* shader);

ComputeShader* computeshader_load (const char* path);
void computeshader_dispatch (ComputeShader* shader);
void computeshader_release (ComputeShader* shader);

#endif //__SHADER__