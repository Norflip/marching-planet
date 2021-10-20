#ifndef __TEXTURE__
#define __TEXTURE__
#include "gl.h"

GLuint texture_load (const char* path);
GLuint texture_loadex (const char* path, GLint wrap_s, GLint wrap_t, GLint minFilter, GLint maxFilter);

GLuint texture_from_memory(const unsigned char* data, GLuint width, GLuint height, GLint format, GLint wrap, GLint filter);

void texture_bind (GLuint id, GLenum slot);
void texture_destroy(GLuint id);

#endif //__TEXTURE__