#include <stdio.h>

//#include <GLAD/glad.h>
//#include <GLFW/glfw3.h>
#include "gl.h"

#include <cglm/cglm.h>
#include <stdint.h>

#include "sky.h"
#include "camera.h"
#include "log.h"

#include "gameplay/planet.h"

#define EXIT_FULLSCREEN_ON_ESQ 1

static double lastFrame = 0.0f;
static GLFWwindow *window;
static GLFWmonitor *monitor;

static int initialize_gl(int width, int height, const char *title);
static void error_callback(int e, const char *d);
static void resize_callback(GLFWwindow *window, int width, int height);
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void toggle_wireframemode();
static void toggle_fullscreen();

static int screenWidth = 1280;
static int screenHeight = 720;

#define SCREEN_ASPECT ((float)screenWidth / (float)screenHeight)
static bool dirtyViewport;
static bool fullscreen = false;

static int screen_x, screen_y, screen_width, screen_height;
static Camera camera = {0};

int main()
{
    log_add_fp(fopen("logs/debug.log", "wb"), 0);

    initialize_gl(screenWidth, screenHeight, "Planet");

    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera.forward);
    glm_vec3_copy((vec3){0, 1, 0}, camera.up);
    glm_look(camera.position, camera.forward, camera.up, camera.view);

    glm_perspective(glm_rad(FOV), SCREEN_ASPECT, NEAR_PLANE, FAR_PLANE, camera.projection);
    glm_mat4_inv(camera.projection, camera.inv_projection);
    glm_mat4_mul(camera.projection, camera.view, camera.projectionView);

    sky_initialize();

    planet_load_shared();
    Planet *planet = planet_generate();

    dirtyViewport = true;
    log_info("Starting loop");

    while (!glfwWindowShouldClose(window))
    {

        if (dirtyViewport)
        {
            glViewport(0, 0, screenWidth, screenHeight);
            // add as camera_resize(screenWidth, screenHeight);
            glm_perspective(glm_rad(FOV), SCREEN_ASPECT, NEAR_PLANE, FAR_PLANE, camera.projection);
            glm_mat4_inv(camera.projection, camera.inv_projection);
            glm_mat4_mul(camera.projection, camera.view, camera.projectionView);
            dirtyViewport = false;
        }

        const double currentFrame = glfwGetTime();
        const double deltatime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // UPDATE
        camera_update(&camera, window, deltatime);
        //planet_update(planet);

        // RENDER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox_draw(&camera);
        // planet_draw(planet);

        log_info("AA");

        glfwSwapBuffers(window);

        log_info("BB");

        glfwPollEvents();

        log_info("CC");
    }

    log_info("exiting");

    sky_destroy();
    // planet_release(planet);
    // planet_release_shared();

    glfwTerminate();
    return 0;
}

static int initialize_gl(int width, int height, const char *title)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        log_fatal("failed to initialize GLFW");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for mac?
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    monitor = glfwGetPrimaryMonitor();
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        log_fatal("failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    /*
    const char* ICO_PATH = "";
    GLFWimage images[1]; 
    images[0].pixels = stbi_load(ICO_PATH, &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images); 
    stbi_image_free(images[0].pixels);
    */

    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, camera_handle_mouse);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        log_fatal("Failed to initialize GLAD");
        return -1;
    }

    glfwSwapInterval(1);

    glEnable(GL_MULTISAMPLE);      // ANTIALIASING
    glEnable(GL_FRAMEBUFFER_SRGB); // GAMAM CORRECTION
    glDepthFunc(GL_LEQUAL);        // depth-testing interprets a smaller value as "closer"
    glEnable(GL_DEPTH_TEST);       // enable depth-testing
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte *version = glGetString(GL_VERSION);   // version as a string
    log_info("Renderer: %s", renderer);
    log_info("OpenGL version supported %s", version);

    dirtyViewport = true;
    return 0;
}

static void error_callback(int e, const char *d)
{
    log_error("%d: %s", e, d);
}

static void resize_callback(GLFWwindow *window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    dirtyViewport = true;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        // toggle fullscreen
        if (key == GLFW_KEY_F12)
            toggle_fullscreen();

        if (key == GLFW_KEY_F1)
            toggle_wireframemode();

        /*
        //toggle mouse
        if (key == GLFW_KEY_F)
        {
            camera_allow_mouse_input = !camera_allow_mouse_input;
            int value = (camera_allow_mouse_input) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
            glfwSetInputMode(window, GLFW_CURSOR, value);
        }
        */

        if (key == GLFW_KEY_ESCAPE)
        {
#if EXIT_FULLSCREEN_ON_ESQ
            if (fullscreen)
                toggle_fullscreen();
            else
#endif
                glfwSetWindowShouldClose(window, 1);
        }
    }
}

static void toggle_wireframemode()
{
    static bool wiremode = false;
    wiremode = !wiremode;

    if (wiremode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

static void toggle_fullscreen()
{
    fullscreen = !fullscreen;

    if (fullscreen)
    {
        glfwGetWindowPos(window, &screen_x, &screen_y);
        glfwGetWindowSize(window, &screen_width, &screen_height);

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    }
    else
    {
        glfwSetWindowMonitor(window, NULL, screen_x, screen_y, screen_width, screen_height, GLFW_DONT_CARE);
    }
}