#include <stdlib.h>
#include <string.h>

#include "shader.h"
#include "log.h"
#include "utils.h"

#define MAX_SHADERS 4
#define MAX_ENTRY_LENGTH 256

// todo(Filip): allocate the memory on the heap instead of stack
static GLuint bound_program = -1;


static inline char *shader_ReadTextFile(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *data = malloc(fsize + 1);
        if (data != NULL)
        {
            size_t bytesRead = fread(data, 1, fsize, f);
            if (ferror(f))
            {
                log_error("Failed reading file %s", path);
                fclose(f);
                return NULL;
            }

            log_info("Read %i bytes [%s]", (int)bytesRead, path);

            fclose(f);
            data[fsize] = '\0';
            return data;
        }
        else
        {
            log_error("Failed to allocate memory", "");
            return NULL;
        }
    }
    else
    {
        log_error("No file found at %s", path);
        return NULL;
    }
}

static void process_file (char** file)
{
    char * result = NULL;
    char line[256];

    const char *tag = "#pragma include";
    const size_t tag_length = strlen(tag);
    const size_t file_length = strlen(*file);

    char path_buffer [256];

    char *curLine = *file;
    while (curLine)
    {
        char *nextLine = strchr(curLine, '\n');
        if (strncmp(curLine, tag, tag_length) == 0)
        {
            size_t index = 0;
            size_t startIndex = -1;

            char* c = nextLine;
            while (c != '\n' && c != '\0' && index < 256)
            {
                if(startIndex != -1)
                    path_buffer[index++] = *c;

                if(*c == '"')
                {
                    if(startIndex == -1)
                        startIndex = c - nextLine;
                    else
                        break;
                }

                c++;
            }

            path_buffer[index-1] = '\0';

            printf("path: '%s'\n", path_buffer);
            char* included_file = shader_ReadTextFile(path_buffer);

            const size_t include_length = strlen(included_file);
            const size_t pos = (curLine - *file);

            size_t totlength = file_length + include_length + 1; 
            char* result = malloc(totlength);

            strncpy(result, *file, pos);
            result[pos] = '\0';
            strcat(result, included_file);
            strcat(result, nextLine);

            free(*file);
            *file = result;
            return;
        }

        if (nextLine)
            *nextLine = '\n'; // then restore newline-char, just to be tidy
        curLine = nextLine ? (nextLine + 1) : NULL;
    }
}

static GLuint load_shader_part(const char *path, GLenum type)
{
    char *file = shader_ReadTextFile(path);
    process_file(&file);


    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &file, NULL);
    glCompileShader(shader);
    free(file);

    int params = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE)
    {
        log_error("ERROR: GL shader %s, index %i did not compile", path, shader);
        log_error("source ------------\n%s\nsource ------------", file);

        static char program_log_buffer[MAX_ENTRY_LENGTH];
        int actual_length = 0;
        glGetShaderInfoLog(shader, MAX_ENTRY_LENGTH, &actual_length, program_log_buffer);
        log_info("shader info log for GL index %u:\n%s\n", shader, program_log_buffer);
        return 0;
    }

    return shader;
}


Shader* shader_load(const char *vs, const char *fs)
{
    Shader* shader = calloc(sizeof(Shader), 1);
    shader->cullMode = GL_BACK;
    shader->fs = fs;
    shader->vs = vs;

    GLuint vshader = load_shader_part(vs, GL_VERTEX_SHADER);
    GLuint fshader = load_shader_part(fs, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    /*
    GLint maxUniformNameLen, noOfUniforms;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLen );
    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &noOfUniforms );

    GLint maxAttribNameLen, noOfAttributes;
    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLen );
    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTES, &noOfAttributes );

    log_info("program has %i uniforms [%i] and %i attributes [%i]", noOfUniforms, maxUniformNameLen, noOfAttributes, maxAttribNameLen);

    GLint i;
    GLint count;
    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)
    const GLsizei bufSize = 16; // maximum name length
    GLchar name[256]; // variable name in GLSL
    GLsizei length; // name length
    
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
    printf("Active Uniforms: %d\n", count);

    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);
        printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
    }
    */

    int params = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        static char program_log_buffer[MAX_ENTRY_LENGTH];
        int actual_length = 0;
        glGetProgramInfoLog(program, MAX_ENTRY_LENGTH, &actual_length, program_log_buffer);
        log_error("could not link shader programme GL index %u", program);
        log_info("program info log for GL index %u:\n%s", program, program_log_buffer);
        
        free(shader);
        return NULL;
    }

    shader->program = program;
    return shader;
}

void shader_reload(Shader *shader)
{
    log_warn("Shader reload is bork");
    //glDeleteProgram(shader->program);
    //shader_load(shader, shader->vs, shader->fs);
}

void shader_bind(Shader *shader)
{
    //if (bound_program != shader->program)
    {
        //TODO: renderdoc detected an invalid enum in CullFace
        //glCullFace(shader->cullMode);

        /*
        if (shader->transparancy)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        */

        glUseProgram(shader->program);
        bound_program = shader->program;
    }
}

void shader_destroy(Shader *shader)
{
    glDeleteProgram(shader->program);
    free(shader);
}


ComputeShader* computeshader_load (const char* path)
{
    ComputeShader* cs = malloc(sizeof(ComputeShader));
    cs->path = path;
    
    // insert shader part here instead
    // load_shader_part
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    char* file = shader_ReadTextFile(path);
    process_file(&file);
    
    glShaderSource(shader, 1, &file, NULL);
    glCompileShader(shader);
    
    // flytta till utils
    int params = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &params);

    if (GL_TRUE != params)
    {
        
        FILE* f = fopen("compiled_shader_result.txt", "w");
        fprintf(f, file);
        fclose(f);
        free(file);

        log_error("GL shader %s, index %i did not compile\noutput to log/shader_output.txt\n", path, shader);

        static char program_log_buffer[MAX_ENTRY_LENGTH];
        int actual_length = 0;
        glGetShaderInfoLog(shader, MAX_ENTRY_LENGTH, &actual_length, program_log_buffer);
        log_error("shader info log for GL index %u:\n%s\n", shader, program_log_buffer);
        return 0;
    }

    free(file);


    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    
    cs->program = program;
    return cs;
}

void computeshader_dispatch (ComputeShader* shader)
{

}

void computeshader_release (ComputeShader* shader)
{

}

