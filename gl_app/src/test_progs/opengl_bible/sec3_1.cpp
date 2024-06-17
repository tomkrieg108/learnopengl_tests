//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage


#pragma once
#include "pch.h"
#include <GL\glew.h>
#include "sec3_1.h"
#include "shader.h"

LayerSec3_1::LayerSec3_1(const std::string& name) :
	Layer::Layer(name)
{
}

void LayerSec3_1::Startup()
{
	ShaderBuilder shader_builder("src/test_progs/opengl_bible/");
	m_shader = shader_builder.Vert("sec_3_1.vs").Frag("sec_3_1.fs").Build("Sec3_1_shader");
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	m_shader->OutputInfo();
}

void LayerSec3_1::Shutdown()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(m_shader->GetProgramID());
}

void LayerSec3_1::OnUpdate(double now, double time_step)
{
	glUseProgram(m_shader->GetProgramID());
	GLfloat attrib[] = { glm::sin(now) * 0.5f,glm::cos(now) * 0.6f, 0.0f, 0.0f };
	GLfloat color[] = { 0.0, 0.0, 1.0, 1.0 };
	// Update the value of input attribute 0
	glVertexAttrib4fv(0, attrib);
	glVertexAttrib4fv(1, color);
	// Draw one triangle
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
