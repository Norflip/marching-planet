#include <stdbool.h>

#include "window.h"
#include "gl.h"
#include "log.h"

struct Window
{
    int width;
    int height;
    const char* title;
    void* windowptr;
    bool _dirtyViewport;

    struct Input 
    {
        int mouse_x;
        int mouse_y;
    };
};

// CALLBACKS
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
static void resize_callback(GLFWwindow *window, int width, int height);

static void error_callback(int e, const char *d);

void window_initialize(Window* window, int width, int height, const char* title)
{
    log_add_fp(fopen("logs/debug.log", "wb"), 0);
    glfwSetErrorCallback(error_callback);
    
    window->width = width;
    window->height = height;
    window->title = title;

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

    GLFWwindow* glfwWindow;
    glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
    if (glfwWindow == NULL)
    {
        log_fatal("failed to create GLFW window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSetWindowUserPointer(glfwWindow, window);
    window->windowptr = glfwWindow;

    
    /*
    const char* ICO_PATH = "";
    GLFWimage images[1]; 
    images[0].pixels = stbi_load(ICO_PATH, &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images); 
    stbi_image_free(images[0].pixels);
    */

    glfwSetFramebufferSizeCallback(glfwWindow, resize_callback);
    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);


    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    window->_dirtyViewport = true;
}

void window_run (Window* window, void(*update_fn)(void), void(*render_fn)(void))
{
    while (!glfwWindowShouldClose(window))
    {
        if (window->_dirtyViewport)
        {
            glViewport(0, 0, window->width, window->height);
            
            //camera_resize(&camera, screenWidth, screenHeight);
            
            window->_dirtyViewport = false;
        }

        _update_fps_counter(window);

        const double currentFrame = glfwGetTime();
        const double deltatime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // UPDATE
        
        // RENDER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    log_info("exiting");
    glfwTerminate();
    return 0;
}

void window_close (Window* window)
{
    glfwSetWindowShouldClose((GLFWwindow*)window->windowptr, 1);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Window* w = glfwGetWindowUserPointer(window);
    if(w != NULL)
    {
        
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* w = glfwGetWindowUserPointer(window);
    if(w != NULL)
    {
    
    }
}


static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    Window* w = glfwGetWindowUserPointer(window);
    if(w != NULL)
    {
    
    }
}


static void resize_callback(GLFWwindow *window, int width, int height)
{
    Window* w = glfwGetWindowUserPointer(window);
    if(w != NULL)
    {
        w->width = width;
        w->height = height;
        w->_dirtyViewport = true;
    }
}


static void error_callback(int e, const char *d)
{
    log_error("[%i] %s", e, d);
}
