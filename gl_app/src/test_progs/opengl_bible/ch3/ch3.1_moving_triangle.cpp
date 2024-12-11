//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage


#pragma once
#include "pch.h"
#include <GL\glew.h>
#include "shader.h"
#include "ch3.1_moving_triangle.h"

namespace sb7
{

	MovingTriangle::MovingTriangle(const std::string& name) :
		Layer::Layer(name)
	{
	}

	void MovingTriangle::Startup()
	{
		ShaderBuilder shader_builder("src/test_progs/opengl_bible/ch3/");
		m_shader = shader_builder.Vert("3.1.moving_triangle.vs").Frag("3.1.moving_triangle.fs").Build("3.1.moving_triangle_shader");
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		m_shader->OutputInfo();
	}

	void MovingTriangle::Shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteProgram(m_shader->GetProgramID());
	}

	void MovingTriangle::OnUpdate(double now, double time_step)
	{
		glUseProgram(m_shader->GetProgramID());
		GLfloat attrib[] = { glm::sin(now) * 0.5f,glm::cos(now) * 0.6f, 0.0f, 0.0f };
		GLfloat color[] = { 1.0, 0.0, 0.0, 1.0 };
		// Update the value of input attribute 0

		//With this call, the data sent is avaiable to each vertex shader run
		glVertexAttrib4fv(0, attrib); 
		glVertexAttrib4fv(1, color);
		// Draw one triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

}
