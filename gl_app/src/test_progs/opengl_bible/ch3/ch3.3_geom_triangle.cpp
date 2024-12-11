//OpenGL Bible sec 3 - geometry shader


#include "pch.h"
#include <GL/glew.h>
#include "shader.h"
#include "ch3.3_geom_triangle.h"


namespace sb7
{
	GeomTriangle::GeomTriangle(const std::string& name) :
		Layer::Layer(name)
	{
	}

	void GeomTriangle::Startup()
	{
		ShaderBuilder shader_builder("src/test_progs/opengl_bible/ch3/");
		m_shader = shader_builder.Vert("3.3.geom_triangle.vs")
			.TessCtrl("3.2.tesselated_triangle.tcs.glsl")
			.TessEval("3.2.tesselated_triangle.tes.glsl")
			.Geom("3.3.geom_triangle.gs")
			.Frag("3.2.tesselated_triangle.fs")
			.Build("Sec3_3_shader");
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3); //number of patch points (3 for a triangle!, which is default).  Guaranteed max at least 32
		glPointSize(5);
		m_shader->OutputInfo();
	}

	void GeomTriangle::Shutdown()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteProgram(m_shader->GetProgramID());
	}

	void GeomTriangle::OnUpdate(double now, double time_step)
	{
		glUseProgram(m_shader->GetProgramID());
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_PATCHES, 0, 3);
	}
}

