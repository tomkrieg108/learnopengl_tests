//OpenGL Bible sec 2 - drawing a point

#include "pch.h"
#include <GL/glew.h>
#include "shader.h"
#include "ch2.1_point.h"

namespace sb7
{
	PointTest::PointTest(const std::string& name) :
		Layer::Layer(name)
	{
	}

	void PointTest::Startup()
	{
		ShaderBuilder shader_builder("src/test_progs/opengl_bible/");
		m_shader = shader_builder.Vert("2.1.point.vs").Frag("2.1.point.fs").Build("2.1.point_shader");
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		m_shader->OutputInfo();
	}

	void PointTest::Shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteProgram(m_shader->GetProgramID());
	}

	void PointTest::OnUpdate(double now, double time_step)
	{
		glUseProgram(m_shader->GetProgramID());
		glPointSize(20.0f); //sb7 loc 1969 value supplied can be at least 64.0
		glDrawArrays(GL_POINTS, 0, 1);
	}
}

