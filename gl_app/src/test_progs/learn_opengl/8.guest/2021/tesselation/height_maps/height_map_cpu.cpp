//https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

#include "pch.h"
#include <GL\glew.h>
#include "shader.h"
#include "camera.h"
#include "stb_image/stb_image.h"
#include "height_map_cpu.h"

LayerHeightMapCPU::LayerHeightMapCPU(Camera& camera) :
	Layer("Height-Map CPU"),
	m_camera{camera}
{
}

void LayerHeightMapCPU::Startup()
{
	glEnable(GL_DEPTH_TEST);

	//shader dir doesn't get assigned until after the constructor of LayerHeightMapCPU
	ShaderBuilder shader_builder("src/test_progs/learn_opengl/8.guest/2021/tesselation/height_maps/");
	m_shader = shader_builder.Vert("height_cpu.vs").Frag("height_cpu.fs").Build("height_cpu_shader");

	m_camera.SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));

	auto const TEXTURE_DIR = std::string{ "assets/textures/" };
	auto texture = TEXTURE_DIR + std::string{ "flinders_heightmap.png" };

	// load and create a texture
	// -------------------------
	// load image, create texture and generate mipmaps
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texture.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		std::cout << "Loaded heightmap of size (HxW) " << height << " x " << width << " Channels: " << nrChannels << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	std::vector<float> vertices;
	float yScale = 64.0f / 256.0f, yShift = 16.0f;
	int rez = 1;
	unsigned bytePerPixel = nrChannels;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
			unsigned char y = pixelOffset[0];

			// vertex
			vertices.push_back(-height / 2.0f + height * i / (float)height);   // vx
			vertices.push_back((int)y * yScale - yShift);   // vy
			vertices.push_back(-width / 2.0f + width * j / (float)width);   // vz
		}
	}
	std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
	stbi_image_free(data);

	std::vector<unsigned> indices;
	for (unsigned i = 0; i < height - 1; i += rez)
	{
		for (unsigned j = 0; j < width; j += rez)
		{
			for (unsigned k = 0; k < 2; k++)
			{
				indices.push_back(j + width * (i + k * rez));
			}
		}
	}
	std::cout << "Loaded " << indices.size() << " indices" << std::endl;

	numStrips = (height - 1) / rez;
	numTrisPerStrip = (width / rez) * 2 - 2;
	std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
	std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

	// first, configure the cube's VAO (and terrainVBO + terrainIBO)
	
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &terrainIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
}

void LayerHeightMapCPU::Shutdown()
{
	glDeleteVertexArrays(1, &terrainVAO);
	glDeleteBuffers(1, &terrainVBO);
	glDeleteBuffers(1, &terrainIBO);
}

void LayerHeightMapCPU::OnUpdate(double now, double time_step)
{
	// render
	// ------
	m_shader->Bind();
	m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
	m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
	m_shader->SetUniformMat4f("model", glm::mat4(1.0f));

	// render the terrain
	glBindVertexArray(terrainVAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (unsigned strip = 0; strip < numStrips; strip++)
	{
		glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
			numTrisPerStrip + 2,   // number of indices to render
			GL_UNSIGNED_INT,     // index data type
			(void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
	}
}