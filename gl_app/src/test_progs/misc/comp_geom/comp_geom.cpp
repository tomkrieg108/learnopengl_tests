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
#include "point.h"
#include "plane.h"
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


		Vector3f v1(1.0f, 1.0f, 1.0f);
		Vector3f v2(1.5f, 2.0f, -1.7f);

		Vector3f v3 = v1 + v2;
		Vector3f v4 = v1 - v2;

		float dp = dotProduct(v1, v2);
		Vector3f cp = crossProduct3d(v1, v2);

		auto mag = v1.magnitude();
		v1.normalize();

		auto stp = scalarTripleProduct(v1, v2, v3);

		Planef plane(Vector3f{ 2.3f, 1.8f, -4.6f }, Point3d{ 1.0f, 2.3f, -4.5f });

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