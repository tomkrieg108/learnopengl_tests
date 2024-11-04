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

#include "lgl_entity.h"
#include "scene_graph.h"

#ifndef ENTITY_H
#define ENTITY_H

class Entity : public Model
{
public:
	list<unique_ptr<Entity>> children;
	Entity* parent;
};
#endif

SceneGraph::SceneGraph(Window& window, Camera& camera) :
	m_window{ window }, m_camera {camera }
{
}

void SceneGraph::Startup()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // AOGL p20

	stbi_set_flip_vertically_on_load(true);

	ShaderBuilder shader_builder("src/test_progs/learn_opengl/3.model_loading/");
	m_shader = shader_builder.Vert("1.model_loading.vs").Frag("1.model_loading.fs").Build("Model loading");

	m_model = new lgl::Model{ "assets/models/planet/planet.obj" };
	m_entity = new Entity(*m_model);
	const float scale = 0.75;
	m_entity->transform.setLocalScale({ scale, scale, scale });
	m_entity->transform.setLocalPosition({ 10,0,0 });

	Entity* lastEntity = m_entity;
	for (unsigned int i = 0; i < 10; ++i)
	{
		lastEntity->addChild(*m_model);
		lastEntity = lastEntity->children.back().get();

		//Set transform values
		lastEntity->transform.setLocalPosition({ 10, 0, 0 });
		lastEntity->transform.setLocalScale({ scale, scale, scale });
	}
	m_entity->updateSelfAndChild();

}

void SceneGraph::Shutdown()
{
	glfwTerminate();
}

void SceneGraph::OnUpdate(double now, double time_step)
{
	// draw our scene graph
	Entity* lastEntity = m_entity;
	
	m_shader->Bind();
	m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
	m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
	
	while (lastEntity->children.size())
	{
		const auto& transform = lastEntity->transform.getModelMatrix();
		m_shader->SetUniformMat4f("model", lastEntity->transform.getModelMatrix());
		lastEntity->pModel->Draw(*m_shader);
		lastEntity = lastEntity->children.back().get();
	}

	m_entity->transform.setLocalRotation({ 0.f, m_entity->transform.getLocalRotation().y + 20 * time_step, 0.f });
	m_entity->updateSelfAndChild();
}

void SceneGraph::ImGuiUpdate()
{
}

