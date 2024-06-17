#include "pch.h"
#include <GL\glew.h>

//#include <imgui\imgui.h>
//#include <imgui\imgui_impl_glfw.h>
//#include <imgui\imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "lgl_mesh.h"
#include "lgl_model.h"

#include "shader.h"
#include "camera.h"

#include "model_loading.h"


ModelLoadingTest::ModelLoadingTest(Camera& camera) :
	m_camera{camera}
{
}

void ModelLoadingTest::Startup()
{
    // configure global opengl state
// -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // AOGL p20

    // build and compile our shader zprogram
    // ------------------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/3.model_loading/");
    m_shader = shader_builder.Vert("1.model_loading.vs").Frag("1.model_loading.fs").Build("Model loading Test");

   // ourModel = new lgl::Model{ "assets/models/backpack/backpack.obj" };
    ourModel = new lgl::Model{ "assets/models/nanosuit/nanosuit.obj" };
    //ourModel = new lgl::Model{ "assets/models/rock/rock.obj" };
    //ourModel = new lgl::Model{ "assets/models/tropical_shrubs/01/trop_shrub_01.obj" };
    //ourModel = new lgl::Model{ "assets/models/Tree1TS/Tree1/Tree1.blend" };
    //ourModel = new lgl::Model{ "assets/models/birch_tree/birch_tree.blend" };
    //ourModel = new lgl::Model{ "assets/models/maple_tree/MapleTree.obj" };
    //ourModel = new lgl::Model{ "assets/models/tropical_shrubs/05/trop_shrub_05.obj" };
    //ourModel = new lgl::Model{ "assets/models/tree2/tree.glb" };

    //m_shader->OutputInfo();
}

void ModelLoadingTest::Shutdown()
{
    m_shader->OutputInfo();
}

void ModelLoadingTest::OnUpdate(double now, double time_step)
{
    m_shader->Bind();

    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down

    m_shader->SetUniformMat4f("model", model);

    ourModel->Draw(*m_shader);
}

void ModelLoadingTest::ImGuiUpdate()
{
}
