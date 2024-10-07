#include "pch.h"
#include <GL\glew.h>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "stb_image/stb_image.h"
#include "shader.h"
#include "geometry_shader_houses.h"

// set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
static float points[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
};

GeometryShaderHouses::GeometryShaderHouses()
{
}

void GeometryShaderHouses::Startup()
{
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
   // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/9.1.geometry_shader_houses/");
    m_shader = shader_builder.Vert("9.1.geometry_shader.vs").
                              Frag("9.1.geometry_shader.fs").
                              Geom("9.1.geometry_shader.gs").
                              Build("Geometry Shader Houses");
    
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    m_shader->OutputInfo();
}

void GeometryShaderHouses::Shutdown()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GeometryShaderHouses::OnUpdate(double now, double time_step)
{
    // draw points
    m_shader->Bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, 4);
}

void GeometryShaderHouses::ImGuiUpdate()
{
}
