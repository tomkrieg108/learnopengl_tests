#include "pch.h"
#include "coord_sys.h"
#include "camera.h"
#include "shader.h"


CoordSys::CoordSys(Camera* camera, v2::Camera* camera2) :
	m_camera(camera), m_camera2(camera2)
{
}

void CoordSys::Startup()
{
	glEnable(GL_LINE_SMOOTH);
	/*GLfloat lineWidthRange[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
	std::cout << "Min windith " << lineWidthRange[0] << "\n";
	std::cout << "Max windith " << lineWidthRange[1] << "\n";*/

	ShaderBuilder shader_builder("src/test_progs/misc/coords/");
	m_shader = shader_builder.Vert("coords.vs").Frag("coords.fs").Build("World Coord System");

	//grid
	m_verticies.clear();

	//build grid (setup m_verticies)
	Vertex vert;
	float x, y, z;
	const float extent = 20.0f;
	const float step = 1.0f;
	y = -0.01f;
	std::array<float, 4> grid_colour{ 0.5f,0.5f,0.5f,1.0f };

#if 1
	z = -extent;
	while (z < extent + 0.1f)
	{
		x = -extent;
		vert = { {x,y,z}, grid_colour };
		m_verticies.push_back(vert);

		x = +extent;
		vert = { {x,y,z}, grid_colour };
		m_verticies.push_back(vert);
		z += step;
	}

	x = -extent;
	while (x < extent + +0.1f)
	{
		z = -extent;
		vert = { {x,y,z}, grid_colour };
		m_verticies.push_back(vert);

		z = +extent;
		vert = { {x,y,z}, grid_colour };
		m_verticies.push_back(vert);

		x += step;
	}
#endif

	//world coord sys
	float y_offset = 0.01f;
	float axis_length = extent;
	Vertex origin_x = { {0,y_offset,0},{1,0,0,1} };	//x=>red
	Vertex origin_y = { {0,y_offset,0},{0,1,0,1} };	//y=>green
	Vertex origin_z = { {0,y_offset,0},{0,0,1,1} };	//z=>blue
	Vertex terminal_x = { {axis_length,y_offset,0},{1,0,0,1} };
	Vertex terminal_y = { {0,axis_length,0},{0,1,0,1} };
	Vertex terminal_z = { {0,y_offset,axis_length},{0,0,1,1} };
	m_verticies.push_back(origin_x); m_verticies.push_back(terminal_x);
	m_verticies.push_back(origin_y); m_verticies.push_back(terminal_y);
	m_verticies.push_back(origin_z); m_verticies.push_back(terminal_z);

	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, m_verticies.size() * sizeof(Vertex), m_verticies.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
}

void CoordSys::Shutdown()
{
	glDeleteVertexArrays(1, &va);
	glDeleteBuffers(1, &vb);
}

void CoordSys::OnUpdate(double now, double time_step)
{
	m_shader->Bind();
	m_shader->SetUniformMat4f("u_model", glm::mat4(1.0f));

	if (m_camera != nullptr)
	{
		m_shader->SetUniformMat4f("u_view", m_camera->ViewMatrix());
		m_shader->SetUniformMat4f("u_proj", m_camera->ProjMatrix());
	}
	else
	{
		m_shader->SetUniformMat4f("u_view", m_camera2->GetViewMatrix());
		m_shader->SetUniformMat4f("u_proj", m_camera2->GetProjMatrix());		
	}

	glBindVertexArray(va);
	//glLineWidth(2.0f); //cannot do in openGL 3.2 + gives a INVALID_VALUE error
	glDrawArrays(GL_LINES, 0, m_verticies.size());
	//glLineWidth(1);
}

void CoordSys::ImGuiUpdate()
{
}
