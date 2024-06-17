//OpenGL Bible sec 2 - drawing a triangle

#include "pch.h"
#include <GL\glew.h>
#include "sec2.h"
#include "shader.h"

void DrawPoint()
{
	glPointSize(5.0f);
	glDrawArrays(GL_POINTS, 0, 1);
}

void DrawTriangle()
{
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

LayerSec2::LayerSec2(const std::string& name):
	Layer::Layer(name)
{
}

void LayerSec2::Startup()
{
	ShaderBuilder shader_builder("src/test_progs/opengl_bible/");
	m_shader = shader_builder.Vert("sec_2.vs").Frag("sec_2.fs").Build("Sec2_shader");
	auto shader1 = shader_builder.Vert("entity.vs").Frag("entity.fs").Build("entity_shader");
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	m_shader->OutputInfo();
	shader1->OutputInfo();
}

void LayerSec2::Shutdown()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(m_shader->GetProgramID());
}

void LayerSec2::OnUpdate(double now, double time_step)
{
	glUseProgram(m_shader->GetProgramID());
	DrawPoint();
	//DrawTriangle();
}
