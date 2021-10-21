#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>

#include "planet.h"
#include "../gl.h"
#include "../shader.h"
#include "../log.h"

#define CHUNK_AXIS_POINTS 16
#define CHUNK_AXIS_VOXELS 15
#define PLANET_TEXTURE_RESOLUTION 32

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
    size_t vertex_count;
};

struct Planet
{
    float size;
    struct Chunk *chunks;
    uint16_t chunkCount;

    GLuint densityBuffer;
};

static ComputeShader *meshCS = NULL;
static ComputeShader *densityCS = NULL;
static Shader* planetShader = NULL;

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

                glGenBuffers(1, &chunk->vbo);
                glGenVertexArrays(1, &chunk->vao);
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
            GLuint counter_buffer;
            glGenBuffers(1, &counter_buffer);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counter_buffer);
            glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

            /*
        MESH BUFFER
    */
            const int max_tris = CHUNK_VOLUME * 5;
            const int max_verts = max_tris * 3;
            const int vertex_size = 4 * sizeof(float);

            // borde kunna flyttas till init?
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunk->vbo);
            glBufferData(GL_SHADER_STORAGE_BUFFER, max_verts * vertex_size, NULL, GL_DYNAMIC_DRAW);
            
            
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


            //bind and dispatch shader
            glUseProgram(meshCS->program);
            glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, counter_buffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, chunk->vbo);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, planet->densityBuffer);

            /*
            layout (location = 0) uniform int textureSize;
            layout (location = 1) uniform float planetSize;
            layout (location = 2) uniform ivec3 chunkCoord;
            */

            glUniform1i(0, PLANET_TEXTURE_RESOLUTION);
            glUniform1f(1, planet->size);
            glUniform3i(2, chunk->x * CHUNK_AXIS_POINTS, chunk->y * CHUNK_AXIS_POINTS, chunk->z * CHUNK_AXIS_POINTS,);


            
            int threadgroups = CHUNK_AXIS_POINTS / 8.0f;
            glDispatchCompute(threadgroups, threadgroups, threadgroups);

            char desc[512];
            GLenum err = glfwGetError(desc);
            if (err != GLFW_NO_ERROR)
            {
                log_error("Failed to run meshCS [%s]", desc);
            }



            glMemoryBarrier(GL_ALL_BARRIER_BITS);



           //glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counter_buffer);
            
            
            
            GLuint triangle_count = *((GLuint *)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT));
            chunk->vertex_count = triangle_count * 3;
            glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);





            
            printf("VERTEX_COUNT: %i\n", chunk->vertex_count);

            //generate vertex array
            glBindVertexArray(chunk->vao);
            // binds to the vertex array
            glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);

            // position attribute
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertex_size, (void *)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
                
            //chunk->flag &= ~CHUNK_FLAG_DIRTY;
        }
    }
}

void planet_draw(Camera* camera, Planet *planet)
{
    mat4 id;
    glm_mat4_identity(id);

    // set shader
    shader_bind(planetShader);
    glUniformMatrix4fv(0, 1, GL_FALSE, (float *)camera->projectionView);
    glUniformMatrix4fv(1, 1, GL_FALSE, (float *)id);

    for (size_t i = 0; i < planet->chunkCount; i++)
    {
        Chunk *chunk = &planet->chunks[i];

        if(chunk->vertex_count > 0)
        {
            glBindVertexArray(chunk->vao);
            glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);   
        }
    }
}

void planet_release(Planet *planet)
{

}
