#include "pch.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"

#include "skeletal_animation.h"

SkeletalAnimation::SkeletalAnimation(Window& window, Camera& camera) :
	m_window{ window }, m_camera{ camera }
{
}

SkeletalAnimation::~SkeletalAnimation()
{
}

void SkeletalAnimation::Startup()
{
	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	ShaderBuilder shader_builder("src/test_progs/learn_opengl/8.guest/2020/skeletal_animation/");
	ourShader = shader_builder.Vert("anim_model.vs").Frag("anim_model.fs").Build("ourShader");

	// load models
	// -----------
	ourModel = new Model("assets/models/Hip Hop Dancing/Hip Hop Dancing.dae");
	danceAnimation = new Animation("assets/models/Hip Hop Dancing/Hip Hop Dancing.dae", ourModel);
	animator = new Animator(danceAnimation);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void SkeletalAnimation::Shutdown()
{
}

void SkeletalAnimation::OnUpdate(double now, double time_step)
{
	animator->UpdateAnimation(time_step);

	ourShader->Bind();

	// view/projection transformations
	glm::mat4 projection = m_camera.ProjMatrix();
	glm::mat4 view = m_camera.ViewMatrix();
	ourShader->SetUniformMat4f("projection", projection);
	ourShader->SetUniformMat4f("view", view);

	auto transforms = animator->GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
		ourShader->SetUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

	// render the loaded model
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));	// it's a bit too big for our scene, so scale it down
	//model = glm::scale(model, glm::vec3(.05f, .05f, .05f));	// it's a bit too big for our scene, so scale it down
	ourShader->SetUniformMat4f("model", model);
	ourModel->Draw(*ourShader);
}

void SkeletalAnimation::ImGuiUpdate()
{
}
