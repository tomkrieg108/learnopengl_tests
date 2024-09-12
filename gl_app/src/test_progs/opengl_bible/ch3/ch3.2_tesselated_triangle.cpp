//OpenGL Bible sec 3 - tesselation


#include "pch.h"
#include <GL/glew.h>
#include "shader.h"
#include "ch3.2_tesselated_triangle.h"


namespace sb7
{
	TesselatedTriangle::TesselatedTriangle(const std::string& name) :
		Layer::Layer(name)
	{
	}

	void TesselatedTriangle::Startup()
	{
		ShaderBuilder shader_builder("src/test_progs/opengl_bible/");
		m_shader = shader_builder.Vert("3.2.tesselated_triangle.vs").TessCtrl("3.2.tesselated_triangle.tcs.glsl").
			TessEval("3.2.tesselated_triangle.tes.glsl").Frag("3.2.tesselated_triangle.fs").Build("Sec3_1_shader");
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3); //number of patch points (3 for a triangle!, which is default).  Guaranteed max at least 32
		m_shader->OutputInfo();
	}

	void TesselatedTriangle::Shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteProgram(m_shader->GetProgramID());
	}

	void TesselatedTriangle::OnUpdate(double now, double time_step)
	{
		glUseProgram(m_shader->GetProgramID());
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, 3);
	}
}

