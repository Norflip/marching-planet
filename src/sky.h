#ifndef __SKY__
#define __SKY__
#include <cglm/cglm.h>
#include "camera.h"
#include "mesh.h"

void sky_initialize ();
void sky_destroy ();
void skybox_draw (Camera* camera);

#endif //__SKY__