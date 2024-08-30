#include "pch.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <imgui\imgui.h>
//#include <imgui\imgui_impl_glfw.h>
//#include <imgui\imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"

#include "scene_graph.h"

SceneGraph::SceneGraph(Window& window, Camera& camera) :
	m_window{ window }, m_camera {camera }
{
}

void SceneGraph::Startup()
{
}

void SceneGraph::Shutdown()
{
}

void SceneGraph::OnUpdate(double now, double time_step)
{
}

void SceneGraph::ImGuiUpdate()
{
}

void SceneGraph::RenderScene(Shader* shader)
{
}

void SceneGraph::RenderCube()
{
}

void SceneGraph::RenderQuad()
{
}

void SceneGraph::OnKeyPressed(EventKeyPressed& e)
{
}
