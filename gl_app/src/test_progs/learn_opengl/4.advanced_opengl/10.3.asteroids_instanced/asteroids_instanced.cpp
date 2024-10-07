#include "pch.h"
#include <GL\glew.h>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
//#include "stb_image/stb_image.h"
#include "shader.h"
#include "asteroids_instanced.h"

AsteroidsInstanced::AsteroidsInstanced(Camera& camera) :
	m_camera{camera}
{
}

void AsteroidsInstanced::Startup()
{
    // configure global opengl state
// -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // AOGL p20

    // build and compile shaders
   // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/10.3.asteroids_instanced/");
    m_shader = shader_builder.Vert("10.3.asteroids.vs").Frag("10.3.asteroids.fs").Build("Asteroids instanced shader");
    m_planet_shader = shader_builder.Vert("10.3.planet.vs").Frag("10.3.planet.fs").Build("Planet shader");

    rock = new lgl::Model{ "assets/models/rock/rock.obj" };
    planet = new lgl::Model{ "assets/models/planet/planet.obj" };

    // generate a large list of semi-random model transformation matrices
       // ------------------------------------------------------------------
    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed
    float radius = 150.0f;
    float offset = 25.0f;
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

    // configure instanced array
   // -------------------------
    
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

    // set transformation matrices as an instance vertex attribute (with divisor 1)
   // note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
   // normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
   // -----------------------------------------------------------------------------------------------------------------------------------
    for (unsigned int i = 0; i < rock->m_meshes.size(); i++)
    {
        unsigned int VAO = rock->m_meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
   
    m_shader->OutputInfo();
    m_planet_shader->OutputInfo();
}

void AsteroidsInstanced::Shutdown()
{
}

void AsteroidsInstanced::OnUpdate(double now, double time_step)
{
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());

    m_planet_shader->Bind();
    m_planet_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_planet_shader->SetUniformMat4f("view", m_camera.ViewMatrix());

    // draw planet
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    m_planet_shader->SetUniformMat4f("model", model);
    planet->Draw(*m_planet_shader);

    m_shader->Bind();
    m_shader->SetUniform1i("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rock->m_textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.

    for (unsigned int i = 0; i < rock->m_meshes.size(); i++)
    {
        glBindVertexArray(rock->m_meshes[i].VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rock->m_meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
    }
}

void AsteroidsInstanced::ImGuiUpdate()
{
}
