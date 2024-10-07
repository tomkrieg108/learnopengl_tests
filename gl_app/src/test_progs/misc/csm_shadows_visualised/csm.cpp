#include "pch.h"
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"

#include "csm.h"

namespace me {

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


  CSMVisualised::CSMVisualised(Window& window, v2::Camera& camera) :
    m_window{ window }, m_camera{ camera },
    m_shadow_cascade_levels{ m_camera.ZFar() / 50.0f, m_camera.ZFar() / 25.0f, m_camera.ZFar() / 10.0f, m_camera.ZFar() / 2.0f }
  {
    float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();

    m_camera.SetPerspectiveParams({ aspect_ratio , 45.0f, 0.5f, 100.0f, true });
    m_camera.SetPosition(glm::vec3(-5.0f, 10.0f, 10.0f));
    m_camera.LookAt(glm::vec3{ 0,0,0 });

    m_camera_vis = new v2::Camera();
    m_camera_vis->SetProjectionType(v2::Camera::ProjectionType::Perspective);
    m_camera_vis->SetPerspectiveParams({ aspect_ratio , 45.0f, 0.1f, 1000.0f, true });
    m_camera_vis->SetPosition(glm::vec3(-20.0f, 15.0f, 20.0f));
    m_camera_vis->LookAt(glm::vec3{ 0,0,0 });

    m_camera_light = new v2::Camera();
    m_camera_light->SetProjectionType(v2::Camera::ProjectionType::Ortho);
    m_camera_light->SetOrthoParams({ -10.0f,10.0f,-10.0f,10.0f,1.0f,7.5f });
    m_camera_light->SetPosition(m_light_pos);

    m_controlled_camera = &m_camera;
  }


  // renders the 3D scene
  // --------------------
  void CSMVisualised::RenderScene(Shader* shader)
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
      shader->SetUniformMat4f("model", model);
      RenderCube();
    }
  }

  // renderCube() renders a 1x1 3D cube in NDC.
  void CSMVisualised::RenderCube()
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

  void CSMVisualised::RenderQuad()
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

  void CSMVisualised::RenderLamp(v2::Camera* camera)
  {
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    glm::mat4 model = m_camera_light->GetTransform();
    model = model * scale;

    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("model", model);
    m_light_cube_shader->SetUniformMat4f("view", camera->GetViewMatrix());
    m_light_cube_shader->SetUniformMat4f("projection", camera->GetProjMatrix());
    RenderCube();
    m_light_cube_shader->Unbind();
    RenderCoords(camera, model);
  }

  void CSMVisualised::RenderCamera(v2::Camera* viewing_camera, v2::Camera* rendered_camera)
  {
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
    glm::mat4 model = rendered_camera->GetTransform();
    model = model * scale;

    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("model", model);
    m_light_cube_shader->SetUniformMat4f("view", viewing_camera->GetViewMatrix());
    m_light_cube_shader->SetUniformMat4f("projection", viewing_camera->GetProjMatrix());
    RenderCube();
    m_light_cube_shader->Unbind();
    RenderCoords(viewing_camera, model);
  }

  void  CSMVisualised::RenderCoords(v2::Camera* camera, glm::mat4& model)
  {
    if (m_coordsVAO == 0)
    {
      float y_offset = 0.0f;
      float axis_length = 3.5f;
      VertexCoords origin_x = { {0,y_offset,0},{1,0,0,1} };	//x=>red
      VertexCoords origin_y = { {0,y_offset,0},{0,1,0,1} };	//y=>green
      VertexCoords origin_z = { {0,y_offset,0},{0,0,1,1} };	//z=>blue
      VertexCoords terminal_x = { {axis_length,y_offset,0},{1,0,0,1} };
      VertexCoords terminal_y = { {0,axis_length,0},{0,1,0,1} };
      VertexCoords terminal_z = { {0,y_offset,axis_length},{0,0,1,1} };
      m_coord_verts.push_back(origin_x); m_coord_verts.push_back(terminal_x);
      m_coord_verts.push_back(origin_y); m_coord_verts.push_back(terminal_y);
      m_coord_verts.push_back(origin_z); m_coord_verts.push_back(terminal_z);
      glGenVertexArrays(1, &m_coordsVAO);
      glBindVertexArray(m_coordsVAO);

      glGenBuffers(1, &m_coordsVBO);
      glBindBuffer(GL_ARRAY_BUFFER, m_coordsVBO);
      glBufferData(GL_ARRAY_BUFFER, m_coord_verts.size() * sizeof(VertexCoords), m_coord_verts.data(), GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCoords), (void*)offsetof(VertexCoords, pos));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexCoords), (void*)offsetof(VertexCoords, col));
    }

    m_coords_shader->Bind();
    m_coords_shader->SetUniformMat4f("u_model", model);
    m_coords_shader->SetUniformMat4f("u_view", camera->GetViewMatrix());
    m_coords_shader->SetUniformMat4f("u_proj", camera->GetProjMatrix());

    glBindVertexArray(m_coordsVAO);
    //glLineWidth(2.0f); //cannot do in openGL 3.2 + gives a INVALID_VALUE error
    glDrawArrays(GL_LINES, 0, m_coord_verts.size());
    //glLineWidth(1);
    m_coords_shader->Unbind();
  }

  void CSMVisualised::DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader)
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
  unsigned int CSMVisualised::LoadTexture(char const* path)
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

  std::vector<glm::vec4> CSMVisualised::GetFrustumCornersWorldSpace(const glm::mat4& projview)
  {
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

  std::vector<glm::vec4> CSMVisualised::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
  {
    //investigating !! ---------------------------------------------------
    //See one of my notepads my book on how this works!
    glm::vec4 coord = glm::vec4{ 2,3,-10,1 };
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

  glm::mat4 CSMVisualised::GetLightSpaceMatrix(const float nearPlane, const float farPlane)
  {
    const auto proj = glm::perspective(
      glm::radians(m_camera.FOV()), (float)m_window.BufferWidth() / (float)m_window.BufferHeight(), nearPlane,
      farPlane);

    const auto corners = GetFrustumCornersWorldSpace(proj, m_camera.GetViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
      center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + m_light_dir, center, glm::vec3(0.0f, 1.0f, 0.0f));
    //+ve z dir of camera = -Front()
    //const auto lightView = glm::lookAt(center + -m_camera_light->Front(), center, glm::vec3(0.0f, 1.0f, 0.0f));

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

    glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
  }

  std::vector<glm::mat4> CSMVisualised::GetLightSpaceMatrices()
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


  void CSMVisualised::Startup()
  {
    // configure global opengl state
      // -----------------------------
    glEnable(GL_DEPTH_TEST);


    ShaderBuilder shader_builder("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_shader = shader_builder.Vert("10.shadow_mapping.vs").Frag("10.shadow_mapping.fs").Build("css shader");

    ShaderBuilder shader_builder2("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_simple_depth_shader = shader_builder2.Vert("10.shadow_mapping_depth.vs").Frag("10.shadow_mapping_depth.fs").Geom("10.shadow_mapping_depth.gs").Build("Simple depth shader");

    ShaderBuilder shader_builder3("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_debug_Depth_quad = shader_builder3.Vert("10.debug_quad.vs").Frag("10.debug_quad_depth.fs").Build("Debug quad shader");

    ShaderBuilder shader_builder4("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_debug_cascade_shader = shader_builder4.Vert("10.debug_cascade.vs").Frag("10.debug_cascade.fs").Build("Debug cascad shader");

    ShaderBuilder shader_builder_light("src/test_progs/learn_opengl/2.lighting/");
    m_light_cube_shader = shader_builder_light.Vert("2.1.light_cube.vs").Frag("2.1.light_cube.fs").Build("light source shader");

    ShaderBuilder shader_builder_coords("src/test_progs/misc/coords/");
    m_coords_shader = shader_builder_coords.Vert("coords.vs").Frag("coords.fs").Build("Local Coord shader");

    ShaderBuilder shader_builder_frustum("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_frustum_shader = shader_builder_frustum.Vert("10.debug_cascade.vs").Frag("10.debug_cascade.fs").Build("Frustum shader");

    
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

    //Setup the framebuffer for visualiser camera
    glGenFramebuffers(1, &m_camera_vis_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_camera_vis_fbo);
    glGenTextures(1, &m_camera_vis_texture);
    glBindTexture(GL_TEXTURE_2D, m_camera_vis_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_window.BufferWidth(), m_window.BufferHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_camera_vis_texture, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    uint32_t rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_window.BufferWidth(), m_window.BufferHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: m_camera_vis_texture is not complete!\n";
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

  void CSMVisualised::OnUpdate(double now, double time_step)
  {
    m_camera_light->SetPosition(m_light_pos);
    m_camera_light->LookAt(glm::vec3{ 0, 0, 0 });

    // 0. UBO setup
    const auto lightMatrices = GetLightSpaceMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
      glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    m_simple_depth_shader->Bind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
    glViewport(0, 0, m_depth_map_resolution, m_depth_map_resolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // peter panning
    RenderScene(m_simple_depth_shader.get());
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. render scene as normal using the generated depth/shadow map  
    // --------------------------------------------------------------
    glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->Bind();

    const glm::mat4 projection = m_camera.GetProjMatrix();

    const glm::mat4 view = m_camera.GetViewMatrix();
    m_shader->SetUniformMat4f("projection", projection);
    m_shader->SetUniformMat4f("view", view);
    // set light uniforms
    m_shader->SetUniform3f("viewPos", m_camera.GetPosition());
    m_shader->SetUniform3f("lightDir", m_light_dir);
    //m_shader->SetUniform3f("lightDir", -m_camera_light->Front());
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
    RenderLamp(&m_camera);

    // Render from point pf view of m_camera_vis
    glBindFramebuffer(GL_FRAMEBUFFER, m_camera_vis_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->Bind(); //need to bind shader again because renderLamp binds a different shader
    m_shader->SetUniformMat4f("projection", m_camera_vis->GetProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera_vis->GetViewMatrix());
    m_shader->SetUniform3f("viewPos", m_camera_vis->GetPosition());
    RenderScene(m_shader.get());
    RenderLamp(m_camera_vis);
    RenderCamera(m_camera_vis, &m_camera);
    //RenderFrustum(m_camera_vis, &m_camera, glm::vec4{ 0.0f, 0.0f, 1.0f, 0.15f });
    //RenderFrustum(m_camera_vis, m_camera_light, glm::vec4{ 1.0f, 0.0f, 0.0f, 0.15f });
    RenderAllFrustrums(m_camera_vis, m_camera_light);
    RenderAllFrustrums(m_camera_vis, &m_camera);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void CSMVisualised::RenderFrustum(v2::Camera* viewing_camera, /*v2::Camera* rendered_camera,*/ const glm::mat4& projview,
   /* const float nearPlane, const float farPlane,*/ glm::vec4 color)
  {

    /*v2::Camera camera = *rendered_camera;
    camera.SetZNearFar(nearPlane, farPlane);
    
    std::vector<glm::vec4> frustum_corners_ws = GetFrustumCornersWorldSpace(camera.GetProjMatrix(), camera.GetViewMatrix());*/

    std::vector<glm::vec4> frustum_corners_ws = GetFrustumCornersWorldSpace(projview);

    std::vector<glm::vec3> corners_vec3;
    for (const auto& v : frustum_corners_ws)
      corners_vec3.push_back(glm::vec3(v));

    //if (frustumVAO == 0)
    //{
    uint32_t indices[] =
    {
      //front
      0,6,2,
      0,4,6,

      //back
      1,7,5,
      1,3,7,

      //left
      1,0,2,
      1,2,3,

      //right
      5,6,4,
      5,7,6,

      //top
      2,6,7,
      2,7,3,

      //bottom
      0,1,5,
      0,5,4
    };

    glGenVertexArrays(1, &frustumVAO);
    glBindVertexArray(frustumVAO);

    glGenBuffers(1, &frustumEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(uint32_t), indices, GL_STATIC_DRAW);


    //}

    //glBindVertexArray(frustumVAO);
    glGenBuffers(1, &frustumVBO);
    glBindBuffer(GL_ARRAY_BUFFER, frustumVBO);
    auto bytes = corners_vec3.size() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, bytes, corners_vec3.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    //glm::vec4 col = glm::vec4{ 0.0f, 1.0f, 1.0f, 0.35f };
    m_frustum_shader->Bind();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_frustum_shader->SetUniform4f("color", color);
    m_frustum_shader->SetUniformMat4f("view", viewing_camera->GetViewMatrix());
    m_frustum_shader->SetUniformMat4f("projection", viewing_camera->GetProjMatrix());
    glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, GLsizei(36), GL_UNSIGNED_INT, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_frustum_shader->Unbind();

    glBindVertexArray(0);
    glDeleteBuffers(1, &frustumVBO);
    glDeleteBuffers(1, &frustumEBO);
    glDeleteVertexArrays(1, &frustumVAO);
  }

  void CSMVisualised::RenderAllFrustrums(v2::Camera* viewing_camera, v2::Camera* rendered_camera)
  {
    const glm::vec4 colors[] = {
        {1.0, 1.0, 0.0, 0.15f},
        {0.0, 1.0, 0.0, 0.15f},
        {0.0, 0.0, 1.0, 0.15f},
    };

    const glm::vec4 colors_light[] = {
        {1.0, 0.0, 0.0, 0.15f},
        {1.0, 0.4, 0.0, 0.15f},
        {1.0, 0.0, 0.4, 0.15f},
    };

    if (rendered_camera == m_camera_light)
    {
      for (size_t i = 0; i < m_shadow_cascade_levels.size() + 1; ++i)
      {
        if (i == 0)
        {
          glm::mat4 light_space_matrix = GetLightSpaceMatrix(m_camera.ZNear(), m_shadow_cascade_levels[i]);
          RenderFrustum(viewing_camera, light_space_matrix,  colors_light[i % 3]);
        }
        else if (i < m_shadow_cascade_levels.size())
        {
          glm::mat4 light_space_matrix = GetLightSpaceMatrix(m_shadow_cascade_levels[i - 1], m_shadow_cascade_levels[i]);

          RenderFrustum(viewing_camera, light_space_matrix, colors_light[i % 3]);
        }
        else
        {
          glm::mat4 light_space_matrix = GetLightSpaceMatrix(m_shadow_cascade_levels[i - 1], m_camera.ZFar());
          RenderFrustum(viewing_camera, light_space_matrix, colors_light[i % 3]);
        }
      }
    }

    v2::Camera camera = *rendered_camera;
    for (size_t i = 0; i < m_shadow_cascade_levels.size() + 1; ++i)
    {
      if (i == 0)
      {
        camera.SetZNearFar(rendered_camera->ZNear(), m_shadow_cascade_levels[i]);
        glm::mat4 proj_view_matrix = camera.GetProjMatrix() * camera.GetViewMatrix();

        RenderFrustum(viewing_camera, proj_view_matrix, colors[i%3]);
      }
      else if (i < m_shadow_cascade_levels.size())
      {
        camera.SetZNearFar(m_shadow_cascade_levels[i - 1], m_shadow_cascade_levels[i]);
        glm::mat4 proj_view_matrix = camera.GetProjMatrix() * camera.GetViewMatrix();
        RenderFrustum(viewing_camera, proj_view_matrix, colors[i % 3]);
      }
      else
      {
        camera.SetZNearFar(m_shadow_cascade_levels[i - 1], rendered_camera->ZFar());
        glm::mat4 proj_view_matrix = camera.GetProjMatrix() * camera.GetViewMatrix();
        RenderFrustum(viewing_camera, proj_view_matrix, colors[i % 3]);
      }
    }
  }


  void CSMVisualised::Shutdown()
  {
    // optional: de-allocate all resources once they've outlived their purpose:
      // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);

    glfwTerminate();
  }

  void CSMVisualised::ImGuiUpdate()
  {
    float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
    float tex_height = 600.0f;
    float tex_width = tex_height * aspect_ratio;

    ImGui::Begin("Parameters");

    if (ImGui::CollapsingHeader("Directional Light"))
    {
      ImGui::SliderFloat3("Pos ", &(m_light_pos[0]), -50.0f, 50.0f);
      auto& light_ortho_params = m_camera_light->GetOrthoParameters();
      ImGui::SliderFloat("Near ", &light_ortho_params.z_near, 0.2f, 2.0f);  //ImGui::SameLine();
      ImGui::SliderFloat("Far ", &light_ortho_params.z_far, 3.0f, 25.0f);

      ImGui::SliderFloat("L ", &light_ortho_params.left, -25.0f, 0.0f); //ImGui::SameLine();
      ImGui::SliderFloat("R ", &light_ortho_params.right, 0.0f, 25.0f); //ImGui::SameLine();
      ImGui::SliderFloat("B ", &light_ortho_params.bottom, -25.0f, 0.0f); //ImGui::SameLine();
      ImGui::SliderFloat("T ", &light_ortho_params.top, 0.0f, 25.0f);
    }

    if (ImGui::CollapsingHeader("Depth Buffer"))
    {
      //ImTextureID tex_id = (void*)depthMap;
      //don't flip UV's
     // ImGui::Image(tex_id, ImVec2(tex_width, tex_height));
    }

    if (ImGui::CollapsingHeader("Camera 2"))
    {
      ImTextureID tex_id = (void*)m_camera_vis_texture;
      //flip uv's in this case WTF?
      ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 });
    }

    ImGui::End();
  }

  void CSMVisualised::CheckKeys(double delta_time)
  {
    const float move_speed = 5.0f;
    const float t = (float)(delta_time);
    bool* keys = m_window.GetKeys().m_key_code;

    if (keys[GLFW_KEY_W])
      m_controlled_camera->MoveForward(-move_speed * t); //note the negative value needed to move forward

    if (keys[GLFW_KEY_S])
      m_controlled_camera->MoveForward(move_speed * t); //note the positive value needed to move backward

    if (keys[GLFW_KEY_A])
      m_controlled_camera->MoveRight(-move_speed * t);

    if (keys[GLFW_KEY_D])
      m_controlled_camera->MoveRight(move_speed * t);
  }

  void CSMVisualised::OnEvent(Event& event)
  {
    if (event.Type() == Event::kMouseMove)
    {
      EventMouseMove& e = dynamic_cast<EventMouseMove&>(event);
      auto* window = m_window.GlfwWindow();
      bool* keys = m_window.GetKeys().m_key_code;
      auto state = glfwGetMouseButton(window, static_cast<int32_t>(GLFW_MOUSE_BUTTON_MIDDLE));
      if (state == GLFW_PRESS)
      {
      }
      if (keys[GLFW_KEY_LEFT_CONTROL])
        m_controlled_camera->RotateWorld(e.delta_x * 0.03f, e.delta_y * 0.03f);
      else if (keys[GLFW_KEY_LEFT_SHIFT])
        m_controlled_camera->MoveForward((e.delta_x + e.delta_y) * 0.01f);
      else
        m_controlled_camera->RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);
    }

    if (event.Type() == Event::kMouseScroll)
    {
      EventMouseScroll& e = dynamic_cast<EventMouseScroll&>(event);
      m_controlled_camera->Zoom(e.y_offset);
    }

    if (event.Type() == Event::kKeyPressed)
    {
      EventKeyPressed& e = dynamic_cast<EventKeyPressed&>(event);
      if (e.key == GLFW_KEY_T)
      {
        if (m_controlled_camera == &m_camera)
          m_controlled_camera = m_camera_vis;
        else
          m_controlled_camera = &m_camera;
      }
    }
  }
}