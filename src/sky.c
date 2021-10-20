#include "sky.h"
#include "shader.h"

static Shader* skybox_shader;
static Mesh* skybox_mesh;

static GLint viewLocation;
static GLint invProjLocation;
static GLint positionLocation;
static GLint timeLocation;

void sky_initialize ()
{
    skybox_shader = shader_load("resources/shaders/skybox.vs","resources/shaders/skybox.fs");
    skybox_mesh = mesh_create_quad(1.0f, 0.0f);

    viewLocation = glGetUniformLocation(skybox_shader->program, "view");
    invProjLocation = glGetUniformLocation(skybox_shader->program, "invProj");
    positionLocation = glGetUniformLocation(skybox_shader->program, "camera_position");
    //timeLocation = glGetUniformLocation(skybox_shader->program, "time");
}

void sky_destroy ()
{
    shader_destroy(skybox_shader);
    // destroy meshes aswell
}


void skybox_draw (Camera* camera)
{
    shader_bind(skybox_shader);
  
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float *)camera->view);
    glUniformMatrix4fv(invProjLocation, 1, GL_FALSE, (float *)camera->inv_projection);
    //glUniform3f(positionLocation, camera->position[0], camera->position[1], camera->position[2]);
    //glUniform1f(timeLocation, (float)glfwGetTime());

    glDepthMask(GL_FALSE);
    mesh_draw(skybox_mesh);
    glDepthMask(GL_TRUE);
    
}

