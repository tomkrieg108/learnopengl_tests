#include "pch.h"
#include <GL/glew.h>

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include "shader.h"
#include "camera.h"

#include "vector.h"
#include "line.h"
#include "plane.h"
#include "intersection.h"
#include "distance.h"
#include "polygon.h"
#include "test.h"
#include "comp_geom.h"

namespace geom
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


		glm::vec3 v1(1.0f, 1.0f, 1.0f);
		glm::vec3 v2(1.5f, 2.0f, -1.7f);

		glm::vec3 v3 = v1 + v2;
		glm::vec3 v4 = v1 - v2;

		float dp = glm::dot(v1, v2);
		glm::vec3 cp = glm::cross(v1, v2);

		auto constexpr mag = v1.length();
		glm::length_t constexpr mag2 = v1.length(); //glm::length_t is unsigned int

		glm::normalize(v1);

		auto stp = ScalarTripleProduct(v1, v2, v3);

		glm::vec3 point_on_line{ 1.0f, 2.0f, 3.0f };
		glm::vec3 line_direction{ 1.0f,  1.0f, 1.0f };

		geom::Line3d line_3d = Line3d::FromPointAndDirection(point_on_line, line_direction);

		//static assertiob triggered for int
		//geom::Line<int, 3> line3d_2{ point_on_line , line_direction };
		
		//Planef plane(Vector3f{ 2.3f, 1.8f, -4.6f }, Point3d{ 1.0f, 2.3f, -4.5f });

		geom::Point3d p1{ 1.0f, 2.0f, 3.0f };
		geom::Point3d p2{ 1.0f, 2.0f, 3.0f };
		geom::Point3d p3{ 1.0f, 2.0f, 3.0f };

		geom::Planef plane{ p1,p2,p3 };

		Vertex2d vert = Vertex2d(Point2d{ 1,0 });

		std::list points{ Point2d{1,0}, Point2d{1,1}, Point2d{0,1} };
		Polygon2d{ points };

		std::cout << "float max: " << std::numeric_limits<float>::max() << "\n";
		std::cout << "float min: " << std::numeric_limits<float>::min() << "\n";
		std::cout << "float lowest: " << std::numeric_limits<float>::lowest() << "\n";
		std::cout << "float epsilon: " << std::numeric_limits<float>::epsilon() << "\n";

		std::cout << "double max: " << std::numeric_limits<double>::max() << "\n";
		std::cout << "double min: " << std::numeric_limits<double>::min() << "\n";
		std::cout << "double lowest: " << std::numeric_limits<double>::lowest() << "\n";
		std::cout << "double epsilon: " << std::numeric_limits<double>::epsilon() << "\n";



		AngleLines2DTest1();
		AngleLines3DTest1();
		AngleLinePlaneTest1();
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