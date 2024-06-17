#include "pch.h"
#include <GL\glew.h>

//#include <imgui\imgui.h>
//#include <imgui\imgui_impl_glfw.h>
//#include <imgui\imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"
#include "asteroids.h"

Asteroids::Asteroids(Camera& camera) :
	m_camera{camera}
{
}

void Asteroids::Startup()
{
    // configure global opengl state
// -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // AOGL p20

    // build and compile shaders
   // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/10.2.asteroids/");
    m_shader = shader_builder.Vert("10.2.instancing.vs").Frag("10.2.instancing.fs").Build("Asteroids shader");

    rock = new lgl::Model{ "assets/models/rock/rock.obj" };
    planet = new lgl::Model{ "assets/models/planet/planet.obj" };

    // generate a large list of semi-random model transformation matrices
  // ------------------------------------------------------------------
   
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    m_shader->OutputInfo();
}

void Asteroids::Shutdown()
{
}

void Asteroids::OnUpdate(double now, double time_step)
{
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());

    // draw planet
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    m_shader->SetUniformMat4f("model", model);
    planet->Draw(*m_shader);

    // draw meteorites
    for (unsigned int i = 0; i < amount; i++)
    {
        m_shader->SetUniformMat4f("model", modelMatrices[i]);
        rock->Draw(*m_shader);
    }
}

void Asteroids::ImGuiUpdate()
{
}
