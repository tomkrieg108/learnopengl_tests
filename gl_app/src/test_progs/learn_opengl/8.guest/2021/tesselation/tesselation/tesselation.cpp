//https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation

#include "pch.h"
#include <GL/glew.h>
#include "shader.h"
#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "tesselation.h"

Tesselation::Tesselation(Window& window, Camera& camera) :
	 m_window{window}, m_camera{ camera }
{
}

void Tesselation::Startup()
{
	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	std::cout << "Max available tess level: " << maxTessLevel << std::endl;

	glEnable(GL_DEPTH_TEST);

	ShaderBuilder shader_builder("src/test_progs/learn_opengl/8.guest/2021/tesselation/tesselation/");
	m_shader = shader_builder.Vert("8.3.gpuheight.vs").TessCtrl("8.3.gpuheight.tcs").TessEval("8.3.gpuheight.tes").Frag("8.3.gpuheight.fs").Build("height_gpu_shader");

	m_camera.SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));

	// load and create a texture
		// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto const HEIGHT_MAP_DIR = std::string{ "assets/textures/" };
	auto height_map_file = HEIGHT_MAP_DIR + std::string{ "flinders_heightmap.png" };

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(height_map_file.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		
		std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	 // ------------------------------------------------------------------
	std::vector<float> vertices;

	
	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
		}
	}

	std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

}

void Tesselation::Shutdown()
{
	glDeleteVertexArrays(1, &terrainVAO);
	glDeleteBuffers(1, &terrainVBO);
}

void Tesselation::OnUpdate(double now, double time_step)
{

	// render
	// ------
	m_shader->Bind();
	m_shader->SetUniform1i("heightMap", 0);
	m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
	m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
	m_shader->SetUniformMat4f("model", glm::mat4(1.0f));

	glBindVertexArray(terrainVAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindVertexArray(terrainVAO);
	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}
