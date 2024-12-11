
#include "pch.h"
#include <GL/glew.h>
#include "shader.h"
#include "gamma.h"

	Gamma::Gamma(const std::string& name) :
		Layer::Layer(name)
	{
	}

	void Gamma::Startup()
	{
		ShaderBuilder shader_builder("src/test_progs/misc/gamma/");
		m_shader = shader_builder.Vert("gamma.vs").Frag("gamma.fs").Build("Gamma Test Shader");
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		m_shader->OutputInfo();

		//glEnable(GL_FRAMEBUFFER_SRGB); //makes the whole screen brighter
	}

	void Gamma::Shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteProgram(m_shader->GetProgramID());
	}

	void Gamma::OnUpdate(double now, double time_step)
	{
		glUseProgram(m_shader->GetProgramID());
		glPointSize(40.0f); //sb7 loc 1969 value supplied can be at least 64.0
		glDrawArrays(GL_POINTS, 0, 32*3);
	}

	/*
		


	*/


