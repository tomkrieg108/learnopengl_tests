#include "pch.h"
#include <GL\glew.h>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"
#include "stb_image/stb_image.h"
#include "shader.h"
#include "geometry_shader_exploding.h"

GeometryShaderExploding::GeometryShaderExploding(Camera& camera) :
    m_camera{camera}
{
}

void GeometryShaderExploding::Startup()
{
    // configure global opengl state
 // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
   // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/9.2.geometry_shader_exploding/");
    m_shader = shader_builder.Vert("9.2.geometry_shader.vs").
        Frag("9.2.geometry_shader.fs").
        Geom("9.2.geometry_shader.gs").
        Build("Geometry Shader Exploding");

    ourModel = new lgl::Model{ "assets/models/nanosuit/nanosuit.obj" };

    m_shader->OutputInfo();
}

void GeometryShaderExploding::Shutdown()
{
}

void GeometryShaderExploding::OnUpdate(double now, double time_step)
{
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    glm::mat4 model = glm::mat4(1.0f);
    m_shader->SetUniformMat4f("model", model);

    // add time component to geometry shader in the form of a uniform
    m_shader->SetUniform1f("time", (float)(now));
    ourModel->Draw(*m_shader);
}

void GeometryShaderExploding::ImGuiUpdate()
{
}
