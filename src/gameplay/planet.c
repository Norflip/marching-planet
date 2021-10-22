#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>

#include "planet.h"
#include "../gl.h"
#include "../shader.h"
#include "../log.h"

#define CHUNK_AXIS_POINTS 16
#define CHUNK_AXIS_VOXELS (CHUNK_AXIS_POINTS - 1)
#define PLANET_TEXTURE_RESOLUTION 128

#define CHUNK_VOLUME (CHUNK_AXIS_POINTS * CHUNK_AXIS_POINTS * CHUNK_AXIS_POINTS)
#define PLANET_DENSITY_VOLUME (PLANET_TEXTURE_RESOLUTION * PLANET_TEXTURE_RESOLUTION * PLANET_TEXTURE_RESOLUTION)

enum
{
    CHUNK_FLAG_NONE = 0,
    CHUNK_FLAG_DIRTY = 1 << 0,
    CHUNK_FLAG_EMPTY = 1 << 1,
    CHUNK_FLAG_FULL = 1 << 2,
};

struct Chunk
{
    int x, y, z;
    uint8_t flag;

    // MESH DATA
    GLuint vao;
    GLuint vbo;
    GLuint counter;

    size_t vertex_count;

    vec3 bounds[2];
};

struct Planet
{
    float size;
    struct Chunk *chunks;
    uint16_t chunkCount;

    GLuint densityBuffer;
};

struct Vertex
{
    float x,y,z,w;
    float nx, ny, nz;
    float pad0;
};

static ComputeShader *meshCS = NULL;
static ComputeShader *densityCS = NULL;
static Shader *planetShader = NULL;

void planet_load_shared(void)
{
    meshCS = computeshader_load("resources/mesh.glsl");
    densityCS = computeshader_load("resources/density.glsl");
    planetShader = shader_load("resources/shaders/planet.vs", "resources/shaders/planet.fs");
}

void planet_release_shared(void)
{
    computeshader_release(meshCS);
    free(meshCS);
    meshCS = NULL;

    computeshader_release(densityCS);
    free(densityCS);
    densityCS = NULL;

    shader_destroy(planetShader);
    free(planetShader);
    planetShader = NULL;
}

Planet *planet_generate(void)
{
    uint8_t numChunks = PLANET_TEXTURE_RESOLUTION / CHUNK_AXIS_POINTS;

    Planet *planet = calloc(1, sizeof(Planet));
    planet->chunkCount = numChunks * numChunks * numChunks;
    planet->chunks = calloc(1, planet->chunkCount * sizeof(struct Chunk));
    planet->size = 0.5f;

    size_t index = 0;
    for (size_t y = 0; y < numChunks; y++)
    {
        for (size_t z = 0; z < numChunks; z++)
        {
            for (size_t x = 0; x < numChunks; x++)
            {
                Chunk *chunk = &planet->chunks[index];
                chunk->flag = CHUNK_FLAG_DIRTY;
                chunk->x = x;
                chunk->y = y;
                chunk->z = z;
                chunk->vertex_count = 0;

                // CHUNK BOUNDS
                float mx = x * CHUNK_AXIS_POINTS;
                float my = y * CHUNK_AXIS_POINTS;
                float mz = z * CHUNK_AXIS_POINTS;
                glm_vec3_copy((vec3){mx, my, mz}, chunk->bounds[0]);
                glm_vec3_copy((vec3){mx + CHUNK_AXIS_POINTS, my + CHUNK_AXIS_POINTS, mz + CHUNK_AXIS_POINTS}, chunk->bounds[1]);

                glGenBuffers(1, &chunk->counter);
                glGenBuffers(1, &chunk->vbo);
                glGenVertexArrays(1, &chunk->vao);

                // kanske måste flyttas till update om chunks ändras utanför init
                const int max_verts = CHUNK_VOLUME * 5 * 3;
                
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunk->vbo);
                glBufferData(GL_SHADER_STORAGE_BUFFER, max_verts * sizeof(struct Vertex), NULL, GL_DYNAMIC_DRAW);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

                index++;
            }
        }
    }

    // create a data int buffer for blocktypes
    const size_t chunk_voxel_data_size = (PLANET_DENSITY_VOLUME * sizeof(float));

    glGenBuffers(1, &planet->densityBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, planet->densityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, chunk_voxel_data_size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(densityCS->program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, planet->densityBuffer);

    /*
    location = 0 int textureSize;
    location = 1 float planetSize;
   */
    glUniform1i(0, PLANET_TEXTURE_RESOLUTION);
    glUniform1f(1, planet->size);

    uint32_t threadgroups = PLANET_TEXTURE_RESOLUTION / 8.0f;
    glDispatchCompute(threadgroups, threadgroups, threadgroups);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0); // UNBIND

    log_info("\nGenerated density\n");

    return planet;
}

void planet_update(Planet *planet)
{
    for (size_t i = 0; i < planet->chunkCount; i++)
    {
        Chunk *chunk = &planet->chunks[i];
        if ((chunk->flag & CHUNK_FLAG_DIRTY) != 0)
        {

            /*
                COUNTER BUFFER
            */
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, chunk->counter);
            glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

            /*
                MESH BUFFER
            */
            //const int max_verts = CHUNK_VOLUME * 5 * 3;
            //const int vertex_size = 4 * sizeof(float);


            /*
            layout (location = 0) uniform int textureSize;
            layout (location = 1) uniform float planetSize;
            layout (location = 2) uniform ivec3 chunkCoord;
            */
            //bind and dispatch shader
            glUseProgram(meshCS->program);
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, chunk->counter);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, chunk->vbo);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, planet->densityBuffer);

            glUniform1i(0, PLANET_TEXTURE_RESOLUTION);
            glUniform1f(1, planet->size);
            glUniform3i(2, chunk->x * CHUNK_AXIS_POINTS, chunk->y * CHUNK_AXIS_POINTS, chunk->z * CHUNK_AXIS_POINTS);

            int threadgroups = CHUNK_AXIS_POINTS / 8.0f;
            glDispatchCompute(threadgroups, threadgroups, threadgroups);

            char desc[512];
            GLenum err = glfwGetError(desc);
            if (err != GLFW_NO_ERROR)
            {
                log_error("Failed to run meshCS [%s]", desc);
            }

            glMemoryBarrier(GL_ALL_BARRIER_BITS);


            // detta kan skippas med indirect rendering, men värdet måste gångras med 3... vilket kanske inte är värt en glUseProgram
            GLuint triangle_count = *((GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT));
            chunk->vertex_count = triangle_count * 3;
            glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);




            glBindVertexArray(chunk->vao);
            glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);

            // position attribute
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)0);
            glEnableVertexAttribArray(0);

            // normal attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *)(sizeof(float) * 4));
            glEnableVertexAttribArray(1);
            

            glBindVertexArray(0);




            chunk->flag &= ~CHUNK_FLAG_DIRTY;
        }
    }
}

void planet_draw(Camera *camera, Planet *planet)
{
    mat4 id;
    glm_mat4_identity(id);

    // set shader
    shader_bind(planetShader);
    // vs
    glUniformMatrix4fv(0, 1, GL_FALSE, (float *)camera->projectionView);
    glUniformMatrix4fv(1, 1, GL_FALSE, (float *)id);
    glUniform3f(2, PLANET_TEXTURE_RESOLUTION * 0.5f, PLANET_TEXTURE_RESOLUTION * 0.5f, PLANET_TEXTURE_RESOLUTION * 0.5f);
    // fs
    glUniform3f(8, 1.0f, 1.0f, 1.0f);

    size_t in_frustum_counter = 0;

    for (size_t i = 0; i < planet->chunkCount; i++)
    {
        Chunk *chunk = &planet->chunks[i];

        if (chunk->vertex_count > 0 && glm_aabb_frustum(chunk->bounds, camera->frustum_planes))
        {
            // glDrawArraysIndirect
            glBindVertexArray(chunk->vao);
            glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);

            in_frustum_counter++;
        }
    }

    //printf("in frustum: %i / %i\n", (int)in_frustum_counter, (int)planet->chunkCount);
}

void planet_release(Planet *planet)
{
    free(planet->chunks);
}
