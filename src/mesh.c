#include "mesh.h"
#include "log.h"

Mesh* mesh_create_quad (float size, float z)
{
      float vertices[] = {
        // pos              // tex
        -size, -size, z,    0.f, 0.f, // BL
        size, -size, z,     1.f, 0.f, // BR
        size, size, z,      1.f, 1.f, // TR
        -size, size, z,     0.f, 1.f  // TL
    };

    unsigned short indices[] = {
        0, 1, 2, // first triangle
        0, 2, 3,  // second triangle
    };

    Mesh* mesh = malloc(sizeof(Mesh));
    mesh->name = "quad";
    mesh->index_count = 6;
    mesh->vertex_count = 4;

    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);

    // VBO VAO
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

void mesh_draw (Mesh* mesh)
{
    /*
    log_info("MESH: %s [%i]  indexes: %i  verts: %i", mesh->name, (int)mesh, (int)mesh->index_count, (int)mesh->vertex_count);
    log_info("VAO: %i", (int)mesh->vao);
    log_info("VBO: %i", (int)mesh->vbo);
    log_info("EBO: %i", (int)mesh->ebo);
  */

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
}

