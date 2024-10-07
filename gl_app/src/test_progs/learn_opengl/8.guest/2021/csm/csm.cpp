#include "pch.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"

#include "csm.h"

//vertex data
static float s_plane_vertices[] = {
	// positions            // normals         // texcoords
	 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	 25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};


CSM::CSM(Window& window, Camera& camera) :
	m_window{ window }, m_camera{ camera },
	m_shadow_cascade_levels{ m_camera.ZFar()/50.0f, m_camera.ZFar() / 25.0f, m_camera.ZFar() / 10.0f, m_camera.ZFar() / 2.0f }
{
  //auto a = m_shadow_cascade_levels;
}


// renders the 3D scene
// --------------------
void CSM::RenderScene(Shader* shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
  shader->SetUniformMat4f("model", model);
	glBindVertexArray(m_planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	static std::vector<glm::mat4> modelMatrices;
	if (modelMatrices.size() == 0)
	{
		for (int i = 0; i < 10; ++i)
		{
			static std::uniform_real_distribution<float> offsetDistribution = std::uniform_real_distribution<float>(-10, 10);
			static std::uniform_real_distribution<float> scaleDistribution = std::uniform_real_distribution<float>(1.0, 2.0);
			static std::uniform_real_distribution<float> rotationDistribution = std::uniform_real_distribution<float>(0, 180);

			auto model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(offsetDistribution(m_generator), offsetDistribution(m_generator) + 10.0f, offsetDistribution(m_generator)));
			model = glm::rotate(model, glm::radians(rotationDistribution(m_generator)), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(scaleDistribution(m_generator)));
			modelMatrices.push_back(model);
		}
	}

	for (const auto& model : modelMatrices)
	{
		//shader.setMat4("model", model);
    shader->SetUniformMat4f("model", model);
		RenderCube();
	}
}

// renderCube() renders a 1x1 3D cube in NDC.
void CSM::RenderCube()
{
  // initialize (if necessary)
  if (m_cubeVAO == 0)
  {
    float vertices[] = {
      // back face
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
       1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
       1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
      -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
      -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
      // front face
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
       1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
       1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
      -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
      -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
      // left face
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
      -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
      -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
      // right face
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
       1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
       1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
       1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
       1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
       // bottom face
       -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
       -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
       -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
       // top face
       -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
       -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
       -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };
    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(m_cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(m_cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void CSM::RenderQuad()
{
  if (m_quadVAO == 0)
  {
    float quadVertices[] = {
      // positions        // texture Coords
      -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(m_quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void CSM::DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
{
  m_visualizerVAOs.resize(8);
  m_visualizerEBOs.resize(8);
  m_visualizerVBOs.resize(8);

  const GLuint indices[] = {
      0, 2, 3,
      0, 3, 1,
      4, 6, 2,
      4, 2, 0,
      5, 7, 6,
      5, 6, 4,
      1, 3, 7,
      1, 7, 5,
      6, 7, 3,
      6, 3, 2,
      1, 5, 4,
      0, 1, 4
  };

  const glm::vec4 colors[] = {
      {1.0, 0.0, 0.0, 0.5f},
      {0.0, 1.0, 0.0, 0.5f},
      {0.0, 0.0, 1.0, 0.5f},
  };

  for (int i = 0; i < lightMatrices.size(); ++i)
  {
    const auto corners = GetFrustumCornersWorldSpace(lightMatrices[i]);
    std::vector<glm::vec3> vec3s;
    for (const auto& v : corners)
    {
      vec3s.push_back(glm::vec3(v));
    }

    glGenVertexArrays(1, &m_visualizerVAOs[i]);
    glGenBuffers(1, &m_visualizerVBOs[i]);
    glGenBuffers(1, &m_visualizerEBOs[i]);

    glBindVertexArray(m_visualizerVAOs[i]);

    glBindBuffer(GL_ARRAY_BUFFER, m_visualizerVBOs[i]);
    glBufferData(GL_ARRAY_BUFFER, vec3s.size() * sizeof(glm::vec3), &vec3s[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_visualizerEBOs[i]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(m_visualizerVAOs[i]);

    //shader->setVec4("color", colors[i % 3]);
    glm::vec4 vec = colors[i % 3];
    shader->SetUniform4f("color", vec);

    glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);

    glDeleteBuffers(1, &m_visualizerVBOs[i]);
    glDeleteBuffers(1, &m_visualizerEBOs[i]);
    glDeleteVertexArrays(1, &m_visualizerVAOs[i]);

    glBindVertexArray(0);
  }

  m_visualizerVAOs.clear();
  m_visualizerEBOs.clear();
  m_visualizerVBOs.clear();
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int CSM::LoadTexture(char const* path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

std::vector<glm::vec4> CSM::GetFrustumCornersWorldSpace(const glm::mat4& projview)
{
  //converts from NDC 
  const auto inv = glm::inverse(projview);

  std::vector<glm::vec4> frustumCorners;
  for (unsigned int x = 0; x < 2; ++x)
  {
    for (unsigned int y = 0; y < 2; ++y)
    {
      for (unsigned int z = 0; z < 2; ++z)
      {
        const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
        frustumCorners.push_back(pt / pt.w);
      }
    }
  }

  return frustumCorners;
}

std::vector<glm::vec4> CSM::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
  //investigating !! ---------------------------------------------------
  //See smaller brown notebook for the math (up the back)
  glm::vec4 coord = glm::vec4{2,3,-10,1};
  glm::vec4 coord_clip = proj * coord;
  glm::vec4 coord_ndc = coord_clip / coord_clip.w;

  auto inv_proj = glm::inverse(proj);
  glm::vec4 coord_clip_2 = inv_proj * coord_ndc;
  glm::vec4 coord_2 = coord_clip_2 / coord_clip_2.w; // is equal to coord
  glm::vec4 coord_3 = coord_clip_2 * coord_clip.w; // is equal to coord
  auto diff = coord_2 - coord;
  auto diff2 = coord_3 - coord;
  //-------------------------------------------------------------------

   
  return GetFrustumCornersWorldSpace(proj * view);
}

glm::mat4 CSM::GetLightSpaceMatrix(const float nearPlane, const float farPlane)
{
  PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(m_camera);
  const auto proj = glm::perspective(
    glm::radians(cam.FOV()), (float)m_window.BufferWidth() / (float)m_window.BufferHeight(), nearPlane,
    farPlane);

  const auto corners = GetFrustumCornersWorldSpace(proj, m_camera.ViewMatrix());

  glm::vec3 center = glm::vec3(0, 0, 0);
  for (const auto& v : corners)
  {
    center += glm::vec3(v);
  }
  center /= corners.size();

  const auto lightView = glm::lookAt(center + m_light_dir, center, glm::vec3(0.0f, 1.0f, 0.0f));

  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::lowest();
  float minZ = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::lowest();
  for (const auto& v : corners)
  {
    const auto trf = lightView * v;
    minX = std::min(minX, trf.x);
    maxX = std::max(maxX, trf.x);
    minY = std::min(minY, trf.y);
    maxY = std::max(maxY, trf.y);
    minZ = std::min(minZ, trf.z);
    maxZ = std::max(maxZ, trf.z);
  }

  // Tune this parameter according to the scene
  constexpr float zMult = 10.0f;
  if (minZ < 0)
  {
    minZ *= zMult;
  }
  else
  {
    minZ /= zMult;
  }
  if (maxZ < 0)
  {
    maxZ /= zMult;
  }
  else
  {
    maxZ *= zMult;
  }

  const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
  return lightProjection * lightView;
}

std::vector<glm::mat4> CSM::GetLightSpaceMatrices()
{
  std::vector<glm::mat4> ret;
  for (size_t i = 0; i < m_shadow_cascade_levels.size() + 1; ++i)
  {
    if (i == 0)
    {
      ret.push_back(GetLightSpaceMatrix(m_camera.ZNear(), m_shadow_cascade_levels[i]));
    }
    else if (i < m_shadow_cascade_levels.size())
    {
      ret.push_back(GetLightSpaceMatrix(m_shadow_cascade_levels[i - 1], m_shadow_cascade_levels[i]));
    }
    else
    {
      ret.push_back(GetLightSpaceMatrix(m_shadow_cascade_levels[i - 1], m_camera.ZFar()));
    }
  }
  return ret;
}


void CSM::Startup()
{
	// configure global opengl state
		// -----------------------------
	glEnable(GL_DEPTH_TEST);


	ShaderBuilder shader_builder1("src/test_progs/learn_opengl/8.guest/2021/csm/");
	m_shader = shader_builder1.Vert("10.shadow_mapping.vs").Frag("10.shadow_mapping.fs").Build("css shader");

	ShaderBuilder shader_builder2("src/test_progs/learn_opengl/8.guest/2021/csm/");
	m_simple_depth_shader = shader_builder2.Vert("10.shadow_mapping_depth.vs").Frag("10.shadow_mapping_depth.fs").Geom("10.shadow_mapping_depth.gs").Build("Simple depth shader");

	ShaderBuilder shader_builder3("src/test_progs/learn_opengl/8.guest/2021/csm/");
	m_debug_Depth_quad = shader_builder3.Vert("10.debug_quad.vs").Frag("10.debug_quad_depth.fs").Build("Debug quad shader");

	ShaderBuilder shader_builder4("src/test_progs/learn_opengl/8.guest/2021/csm/");
	m_debug_cascade_shader = shader_builder4.Vert("10.debug_cascade.vs").Frag("10.debug_cascade.fs").Build("Debug cascad shader");

  //plane VAO
  glGenVertexArrays(1, &m_planeVAO);
  glGenBuffers(1, &m_planeVBO);
  glBindVertexArray(m_planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(s_plane_vertices), s_plane_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glBindVertexArray(0);

	// load textures
		// -------------
	m_wood_texture = LoadTexture("assets/textures/wood.png");

  // configure light FBO
    // -----------------------
  glGenFramebuffers(1, &m_lightFBO);

  glGenTextures(1, &m_light_depth_maps);
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_light_depth_maps);
  glTexImage3D(
    GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, m_depth_map_resolution, m_depth_map_resolution, int(m_shadow_cascade_levels.size()) + 1,
    0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

  glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_light_depth_maps, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    throw 0;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // configure UBO
    // --------------------
  
  glGenBuffers(1, &m_matricesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matricesUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
	
	// shader configuration
		// --------------------
	m_shader->Bind();
	m_shader->SetUniform1i("diffuseTexture", 0);
	m_shader->SetUniform1i("shadowMap", 1);
	m_debug_Depth_quad->Bind();
	m_debug_Depth_quad->SetUniform1i("depthMap", 0);
}

void CSM::OnUpdate(double now, double time_step)
{
  // 0. UBO setup
  const auto lightMatrices = GetLightSpaceMatrices();
  glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
  for (size_t i = 0; i < lightMatrices.size(); ++i)
  {
    glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
  }
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // 1. render depth of scene to texture (from light's perspective)
       // --------------------------------------------------------------
       //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
       // render scene from light's point of view
  m_simple_depth_shader->Bind();

  glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
  glViewport(0, 0, m_depth_map_resolution, m_depth_map_resolution);
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);  // peter panning
  RenderScene(m_simple_depth_shader.get());
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // reset viewport
  glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 2. render scene as normal using the generated depth/shadow map  
        // --------------------------------------------------------------
  //glViewport(0, 0, fb_width, fb_height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_shader->Bind();

  /*PerspectiveCamera& cam = dynamic_cast<PerspectiveCamera&>(m_camera);
  const glm::mat4 projection1 = glm::perspective(glm::radians(cam.FOV()), (float)m_window.BufferWidth() / (float)m_window.BufferHeight(), cam.ZNear(), cam.ZFar());*/
  const glm::mat4 projection = m_camera.ProjMatrix();

  const glm::mat4 view = m_camera.ViewMatrix();
  m_shader->SetUniformMat4f("projection", projection);
  m_shader->SetUniformMat4f("view", view);
  // set light uniforms
  m_shader->SetUniform3f("viewPos", m_camera.Position());
  m_shader->SetUniform3f("lightDir", m_light_dir);
  m_shader->SetUniform1f("farPlane", m_camera.ZFar());
  m_shader->SetUniform1i("cascadeCount", m_shadow_cascade_levels.size());
  for (size_t i = 0; i < m_shadow_cascade_levels.size(); ++i)
  {
    m_shader->SetUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", m_shadow_cascade_levels[i]);
  }
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_wood_texture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_light_depth_maps);
  RenderScene(m_shader.get());

  if (m_light_matrices_cache.size() != 0)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_debug_cascade_shader->Bind();
    m_debug_cascade_shader->SetUniformMat4f("projection", projection);
    m_debug_cascade_shader->SetUniformMat4f("view", view);
    DrawCascadeVolumeVisualizers(m_light_matrices_cache, m_debug_cascade_shader.get());
    glDisable(GL_BLEND);
  }

  // render Depth map to quad for visual debugging
  // ---------------------------------------------
  m_debug_Depth_quad->Bind();
  m_debug_Depth_quad->SetUniform1i("layer", m_debug_layer);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, m_light_depth_maps);
  if (m_show_quad)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderQuad();
  }
}

#if 0
void CSM::OnKeyPressed(EventKeyPressed& e)
{
  std::cout << "Key code pressed - CSM " << e.key << "\n";

  if (e.key == GLFW_KEY_F)
    m_show_quad = !m_show_quad;

  if ((e.key == GLFW_KEY_N) && m_show_quad)
  {
    m_debug_layer++;
    if (m_debug_layer > m_shadow_cascade_levels.size())
      m_debug_layer = 0;
  }

  if ( e.key == GLFW_KEY_C)
  {
    m_show_visualizers = !m_show_visualizers;
    if (m_show_visualizers)
      m_light_matrices_cache = GetLightSpaceMatrices();
    else
      m_light_matrices_cache.clear();
  }
}
#endif

void CSM::OnEvent(Event& event)
{
  if (event.Type() != Event::kKeyPressed)
    return;

  EventKeyPressed& e = dynamic_cast<EventKeyPressed&>(event);
  if (e.key == GLFW_KEY_F)
    m_show_quad = !m_show_quad;
}

void CSM::Shutdown()
{
  // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &m_planeVAO);
  glDeleteBuffers(1, &m_planeVBO);

  glfwTerminate();
}

void CSM::ImGuiUpdate()
{
}
