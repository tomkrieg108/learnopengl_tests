#include "pch.h"
#include <GL/glew.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include "shader.h"
#include "camera.h"

#include "vector.h"
#include "comp_geom.h"

namespace jmk
{

	CompGeom::CompGeom(Window& window, Camera& camera) :
		m_window{ window }, m_camera{ camera }
	{
	}

	void CompGeom::Startup()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//comment out this for the backpack model
		stbi_set_flip_vertically_on_load(true);
	}

	void CompGeom::Shutdown()
	{
	}

	void CompGeom::OnUpdate(double now, double time_step)
	{

	}

	void CompGeom::ImGuiUpdate()
	{
	}

}