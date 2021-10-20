#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>

#include "planet.h"
#include "../gl.h"
#include "../shader.h"
#include "../log.h"

#define CHUNK_AXIS_POINTS 16
#define CHUNK_AXIS_VOXELS 15
#define PLANET_DENSITY_RESOLUTION 64
#define PLANET_DENSITY_VOLUME (PLANET_DENSITY_RESOLUTION * PLANET_DENSITY_RESOLUTION * PLANET_DENSITY_RESOLUTION)

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

static ComputeShader* meshCS = NULL;
static ComputeShader* densityCS = NULL;

void planet_load_shared (void)
{
    meshCS = computeshader_load("resources/mesh.glsl");
    densityCS = computeshader_load("resources/density.glsl");
}

void planet_release_shared (void)
{
    computeshader_release(meshCS);
    free(meshCS);
    meshCS = NULL;

    computeshader_release(densityCS);
    free(densityCS);
    densityCS = NULL;
}

Planet *planet_generate(void)
{
    uint8_t numChunks = PLANET_DENSITY_RESOLUTION / CHUNK_AXIS_POINTS;
    
    Planet *planet = calloc(1, sizeof(Planet));
    planet->chunkCount = numChunks * numChunks * numChunks;
    planet->chunks = calloc(1, numChunks * sizeof(struct Chunk));
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


   // glGenBuffers(1, &c.opaque_mesh_buffer);
    //glGenVertexArrays(1, &c.vao);


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
    glUniform1i(0, PLANET_DENSITY_RESOLUTION);
    glUniform1f(1, planet->size);

    uint32_t threadgroups = PLANET_DENSITY_RESOLUTION / 8.0f;
    glDispatchCompute(threadgroups, threadgroups, threadgroups);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0); // UNBIND
    
    log_info("\nGenerated density\n");

    return planet;
}


void planet_update (Planet* planet)
{
    printf("A\n");

    for (size_t i = 0; i < planet->chunkCount; i++)
    {
        Chunk *chunk = &planet->chunks[i];
        if((chunk->flag & CHUNK_FLAG_DIRTY) != 0)
        {
            // update the mesh


            chunk->flag &= ~CHUNK_FLAG_DIRTY;
        }
    }
    printf("B\n");
    
}

void planet_draw(Planet *planet)
{
        printf("AAA\n");

    for (size_t i = 0; i < planet->chunkCount; i++)
    {
        //Chunk *chunk = &planet->chunks[i];
    }

    
        printf("BBB\n");

}

void planet_release(Planet *planet)
{

}
