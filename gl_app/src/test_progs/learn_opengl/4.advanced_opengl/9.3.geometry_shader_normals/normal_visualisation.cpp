#include "pch.h"
#include <GL\glew.h>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"
#include "stb_image/stb_image.h"
#include "shader.h"
#include "normal_visualisation.h"

NormalVisualisation::NormalVisualisation(Camera& camera) :
    m_camera{camera}
{
}

void NormalVisualisation::Startup()
{
    // configure global opengl state
// -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
   // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/9.3.geometry_shader_normals/");
    m_shader = shader_builder.Vert("9.3.default.vs").
        Frag("9.3.default.fs").
        Build("Default shader");

    m_normal_shader = shader_builder.Vert("9.3.normal_visualization.vs").
        Geom("9.3.normal_visualization.gs").
        Frag("9.3.normal_visualization.fs").
        Build("Default shader");

    ourModel = new lgl::Model{ "assets/models/backpack/backpack.obj" };

    m_shader->OutputInfo();
    m_normal_shader->OutputInfo();
}

void NormalVisualisation::Shutdown()
{
}

void NormalVisualisation::OnUpdate(double now, double time_step)
{
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    glm::mat4 model = glm::mat4(1.0f);
    m_shader->SetUniformMat4f("model", model);
    ourModel->Draw(*m_shader);

    m_normal_shader->Bind();
    m_normal_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_normal_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    m_normal_shader->SetUniformMat4f("model", model);
    ourModel->Draw(*m_normal_shader);
}

void NormalVisualisation::ImGuiUpdate()
{
}
