#ifndef __CAMERA__
#define __CAMERA__
#include <stdbool.h>
#include <cglm/cglm.h>
#include "gl.h"

#define DEFAULT_NEAR_PLANE 0.001f
#define DEFAULT_FAR_PLANE 1000.0f
#define DEFAULT_FOV 60.0f

typedef struct Camera
{
    vec3 position;
    vec3 forward;
    vec3 up;

    float fov, near, far;
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    mat4 inv_projection;

    // frustum planes
    vec4 frustum_planes [6];

    float xRotation;
    float yRotation;
    bool dirty;

} Camera;

void camera_initialize(Camera* camera, int width, int height);

void camera_resize(Camera* camera, int width, int height);

void camera_update_fps (Camera* camera, GLFWwindow *window, const float dt);
void camera_handle_mouse(GLFWwindow *window, double xpos, double ypos);

/*
void camera_translate (Camera* camera, float x, float y);
void camera_update (Camera* camera);
bool camera_in_view(Camera* camera, Bounds bounds);
*/


#endif //__CAMERA__