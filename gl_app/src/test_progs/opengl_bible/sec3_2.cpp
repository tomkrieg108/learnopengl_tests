//OpenGL Bible sec 3 - tesselation


#include "pch.h"
#include <GL\glew.h>
#include "sec3_2.h"
#include "shader.h"

LayerSec3_2::LayerSec3_2(const std::string& name) :
	Layer::Layer(name)
{
}

void LayerSec3_2::Startup()
{
	ShaderBuilder shader_builder("src/test_progs/opengl_bible/");
	m_shader = shader_builder.Vert("sec_3_2.vs").TessCtrl("sec_3_2_tcs.glsl").
		TessEval("sec_3_2_tes.glsl").Frag("Sec_2.fs").Build("Sec3_1_shader");
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	m_shader->OutputInfo();
}

void LayerSec3_2::Shutdown()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteProgram(m_shader->GetProgramID());
}

void LayerSec3_2::OnUpdate(double now, double time_step)
{
	glUseProgram(m_shader->GetProgramID());
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_PATCHES, 0, 3);
}