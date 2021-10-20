#ifndef __PLANET__
#define __PLANET__


typedef struct Chunk Chunk;
typedef struct Planet Planet;

void planet_load_shared (void);
void planet_release_shared (void);

Planet* planet_generate(void);
void planet_update (Planet* planet);
void planet_draw (Planet* planet);
void planet_release(Planet* planet);

void chunk_create (Chunk* chunk);

#endif //__PLANET__