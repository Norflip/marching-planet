#include "texture.h"
#include "log.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define FLIP_VERTICALLY 1
#define WARNING_POW2 1

GLuint texture_load(const char *path)
{
    return texture_loadex(path, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
}

GLuint texture_loadex(const char *path, GLint wrap_s, GLint wrap_t, GLint minFilter, GLint maxFilter)
{
    stbi_set_flip_vertically_on_load(FLIP_VERTICALLY);

    int nrChannels, width, height;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (!data)
    {
        log_error("ERROR: could not load %s", path);
        return 0;
    }

#if WARNING_POW2
    if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0)
    {
        log_warn("texture %s is not power-of-2 dimensions", path);
        return 0;
    }
#endif

    // skip alpha?  GL_RGB
    GLuint texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

    stbi_image_free(data);
    return texture;
}

GLuint texture_from_memory(const unsigned char* data, GLuint width, GLuint height, GLint format, GLint wrap, GLint filter)
{
     GLuint texture;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);


    return texture;
}

void texture_bind(GLuint id, GLenum slot)
{
    glActiveTexture(slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void texture_destroy(GLuint id)
{
    glDeleteTextures(1, &id);
}
