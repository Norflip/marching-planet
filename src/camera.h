#ifndef __CAMERA__
#define __CAMERA__
#include <stdbool.h>
#include <cglm/cglm.h>
#include "gl.h"

#define NEAR_PLANE 0.001f
#define FAR_PLANE 1000.0f
#define FOV 60.0f

typedef struct Camera
{
    vec3 position;
    vec3 forward;
    vec3 up;

    int width, height;
    float fov;

    mat4 view;
    mat4 projection;
    mat4 projectionView;
    mat4 inv_projection;

    // frustum planes

    float xRotation;
    float yRotation;

    bool dirty;

} Camera;


void camera_update (Camera* camera, GLFWwindow *window, const float dt);
void camera_handle_mouse(GLFWwindow *window, double xpos, double ypos);

/*
void camera_translate (Camera* camera, float x, float y);
void camera_update (Camera* camera);
bool camera_in_view(Camera* camera, Bounds bounds);
*/


#endif //__CAMERA__