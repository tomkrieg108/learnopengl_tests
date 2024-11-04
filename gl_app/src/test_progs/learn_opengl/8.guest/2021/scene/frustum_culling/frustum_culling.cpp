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

#include "../scene_graph/lgl_entity.h"
#include "frustum_culling.h"

#ifndef ENTITY_H
#define ENTITY_H

class Entity : public Model
{
public:
	list<unique_ptr<Entity>> children;
	Entity* parent;
};
#endif

FrustumCulling::FrustumCulling(Window& window, v2::Camera& camera):
	m_window{ window }, m_camera{ camera }
{
	float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();

	m_camera.SetPosition(glm::vec3(-5.0f, 10.0f, 10.0f));
	m_camera.LookAt(glm::vec3{ 0,0,0 });

	m_camera_vis = new v2::Camera();
	m_camera_vis->SetProjectionType(v2::Camera::ProjectionType::Perspective);
	m_camera_vis->SetPerspectiveParams({ aspect_ratio , 45.0f, 0.1f, 1000.0f, true });
	m_camera_vis->SetPosition(glm::vec3(-20.0f, 15.0f, 20.0f));
	m_camera_vis->LookAt(glm::vec3{ 0,0,0 });

	m_controlled_camera = &m_camera;
}

void FrustumCulling::Startup()
{
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // AOGL p20

	ShaderBuilder shader_builder("src/test_progs/learn_opengl/3.model_loading/");
	m_shader = shader_builder.Vert("1.model_loading.vs").Frag("1.model_loading.fs").Build("Model loading");

	m_model = new lgl::Model{ "assets/models/planet/planet.obj" };
	m_entity = new Entity(*m_model);

	const float scale = 0.75;
	m_entity->transform.setLocalScale({ scale, scale, scale });
	m_entity->transform.setLocalPosition({ 0,0,0 });

	//make 400 of them
	{
		Entity* lastEntity = m_entity;

		for (unsigned int x = 0; x < 20; ++x)
		{
			for (unsigned int z = 0; z < 20; ++z)
			{
				m_entity->addChild(*m_model);
				lastEntity = m_entity->children.back().get();

				//Set transform values
				lastEntity->transform.setLocalPosition({ x * 10.f - 100.f,  0.f, z * 10.f - 100.f });
			}
		}
	}
	m_entity->updateSelfAndChild();
}

void FrustumCulling::Shutdown()
{

}

void FrustumCulling::OnUpdate(double now, double time_step)
{
	m_shader->Bind();

	glm::mat4 projection = m_camera.GetProjMatrix();
	glm::mat4 view = m_camera.GetViewMatrix();

	float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
	const Frustum camFrustum = createFrustumFromCamera(m_camera, aspect_ratio, glm::radians(m_camera.FOV()), 0.1f, 100.0f);

	m_shader->SetUniformMat4f("projection", projection);
	m_shader->SetUniformMat4f("view", view);

	// draw our scene graph
	unsigned int total = 0, display = 0;
	m_entity->drawSelfAndChild(camFrustum, *m_shader, display, total);
	std::cout << "Total process in CPU : " << total << " / Total send to GPU : " << display << std::endl;

	//ourEntity.transform.setLocalRotation({ 0.f, ourEntity.transform.getLocalRotation().y + 20 * deltaTime, 0.f });
	m_entity->updateSelfAndChild();
}

void FrustumCulling::ImGuiUpdate()
{

}

void FrustumCulling::CheckKeys(double delta_time)
{
	const float move_speed = 5.0f;
	const float t = (float)(delta_time);
	bool* keys = m_window.GetKeys().m_key_code;

	if (keys[GLFW_KEY_W])
		m_controlled_camera->MoveForward(-move_speed * t); //note the negative value needed to move forward

	if (keys[GLFW_KEY_S])
		m_controlled_camera->MoveForward(move_speed * t); //note the positive value needed to move backward

	if (keys[GLFW_KEY_A])
		m_controlled_camera->MoveRight(-move_speed * t);

	if (keys[GLFW_KEY_D])
		m_controlled_camera->MoveRight(move_speed * t);
}

void FrustumCulling::OnEvent(Event& event)
{
	if (event.Type() == Event::kMouseMove)
	{
		EventMouseMove& e = dynamic_cast<EventMouseMove&>(event);
		auto* window = m_window.GlfwWindow();
		bool* keys = m_window.GetKeys().m_key_code;
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(GLFW_MOUSE_BUTTON_MIDDLE));
		if (state == GLFW_PRESS)
		{
		}
		if (keys[GLFW_KEY_LEFT_CONTROL])
			m_controlled_camera->RotateWorld(e.delta_x * 0.03f, e.delta_y * 0.03f);
		else if (keys[GLFW_KEY_LEFT_SHIFT])
			m_controlled_camera->MoveForward((e.delta_x + e.delta_y) * 0.01f);
		else
			m_controlled_camera->RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);
	}

	if (event.Type() == Event::kMouseScroll)
	{
		EventMouseScroll& e = dynamic_cast<EventMouseScroll&>(event);
		m_controlled_camera->Zoom(e.y_offset);
	}

	if (event.Type() == Event::kKeyPressed)
	{
		EventKeyPressed& e = dynamic_cast<EventKeyPressed&>(event);
		if (e.key == GLFW_KEY_T)
		{
			if (m_controlled_camera == &m_camera)
				m_controlled_camera = m_camera_vis;
			else
				m_controlled_camera = &m_camera;
		}
	}
}




