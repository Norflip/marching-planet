#ifndef __GL__
#define __GL__

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

// https://www.khronos.org/opengl/wiki/Common_Mistakes#glAreTexturesResident_and_Video_Memory
// http://nasutechtips.blogspot.com/2011/02/how-to-get-gpu-memory-size-and-usage-in.html

/* 
    INTEL
*/
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX 0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX 0x904B

static inline GLint gl_max_memory(void)
{
    GLint nTotalMemoryInKB = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &nTotalMemoryInKB);
    return nTotalMemoryInKB;
}

static inline GLint gl_available_memory(void)
{
    GLint nCurAvailMemoryInKB = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &nCurAvailMemoryInKB);
    return nCurAvailMemoryInKB;
}

#undef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
#undef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
#endif //__GL__