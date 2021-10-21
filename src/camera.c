#include "camera.h"

static Camera *main_cam = NULL;

void camera_initialize(Camera* camera, int width, int height)
{
    camera->fov = DEFAULT_FOV;
    camera->near = DEFAULT_NEAR_PLANE;
    camera->far = DEFAULT_FAR_PLANE;

    glm_vec3_copy((vec3){0, 0, 0}, camera->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->forward);
    glm_vec3_copy((vec3){0, 1, 0}, camera->up);
    
    glm_look(camera->position, camera->forward, camera->up, camera->view);
    camera_resize(camera, width, height);
}

void camera_resize(Camera* camera, int width, int height)
{
    const float aspect = (float)width / (float)height;
    glm_perspective(glm_rad(camera->fov), aspect, camera->near, camera->far, camera->projection);
    glm_mat4_inv(camera->projection, camera->inv_projection);
    glm_mat4_mul(camera->projection, camera->view, camera->projectionView);
}

void camera_update_fps(Camera *camera, GLFWwindow *window, const float dt)
{
    main_cam = camera;

    // get keyboard input
    const float MOVEMENT_SPEED = 8.5f;
    const float ROTATION_SPEED = 35.0f;

    float speed = MOVEMENT_SPEED;
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        speed *= 4;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
        speed *= 12;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
    {
        glm_vec3_muladds(camera->forward, dt * speed, camera->position);
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
    {
        glm_vec3_muladds(camera->forward, -dt * speed, camera->position);
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
    {
        vec3 dir;
        glm_cross(camera->forward, camera->up, dir);
        glm_normalize(dir);
        glm_vec3_muladds(dir, -dt * speed, camera->position);
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        vec3 dir;
        glm_cross(camera->forward, camera->up, dir);
        glm_normalize(dir);
        glm_vec3_muladds(dir, dt * speed, camera->position);
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_X))
    {
        glm_vec3_muladds(camera->up, dt * speed, camera->position);
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z))
    {
        glm_vec3_muladds(camera->up, -dt * speed, camera->position);
        camera->dirty = true;
    }

    float rotation_offset = 0.0f;


    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_E))
    {
        rotation_offset += 1.0f;
        camera->dirty = true;
    }

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q))
    {
        rotation_offset -= 1.0f;
        camera->dirty = true;
    }

    // rotate delta by camera direction
    if (camera->dirty)
    {
        camera->xRotation += rotation_offset * ROTATION_SPEED * dt;

        //printf("camera_pos: %f, %f, %f\n", camera->position[0], camera->position[1], camera->position[2]);
        const float co_p = cos(glm_rad(camera->yRotation));
        camera->forward[0] = cos(glm_rad(camera->xRotation)) * co_p;
        camera->forward[1] = sin(glm_rad(camera->yRotation));
        camera->forward[2] = sin(glm_rad(camera->xRotation)) * co_p;

        glm_look(camera->position, camera->forward, camera->up, camera->view);
        glm_mat4_mul(camera->projection, camera->view, camera->projectionView);

        glm_frustum_planes(camera->projectionView, camera->frustum_planes);

        /*
        for (size_t i = 0; i < 6; i++)
            printf("FPLANE [%i]: %f, %f, %f, %f\n", (int)i, camera->frustum_planes[i][0], camera->frustum_planes[i][1], camera->frustum_planes[i][2], camera->frustum_planes[i][3]);
        */
        
        camera->dirty = false;
    }

    // get mouse input
}

static bool firstMouse;
static double lastX;
static double lastY;
bool camera_allow_mouse_input = true;

void camera_handle_mouse(GLFWwindow *window, double xpos, double ypos)
{
    if (main_cam == NULL)
        return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;

#if FLIP_MOUSE_Y
    float yoffset = ypos - lastY;
#else
    float yoffset = lastY - ypos;
#endif

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (camera_allow_mouse_input && (xoffset != 0.0f || yoffset != 0.0f))
    {

        main_cam->xRotation += xoffset;
        main_cam->yRotation += yoffset;

        if (main_cam->yRotation > 89.0f)
            main_cam->yRotation = 89.0f;
        if (main_cam->yRotation < -89.0f)
            main_cam->yRotation = -89.0f;

        main_cam->dirty = true;
    }
}