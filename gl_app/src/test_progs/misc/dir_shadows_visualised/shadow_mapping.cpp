#include "pch.h"
#include <GL\glew.h>

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
#include "shadow_mapping.h"

namespace me {


  // set up vertex data (and buffer(s)) and configure vertex attributes
      // ------------------------------------------------------------------
  static float planeVertices[] = {
    // positions            // normals         // texcoords
     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

     25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
     25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
  };

  // utility function for loading a 2D texture from file
  // ---------------------------------------------------
  unsigned int ShadowMappingVisualised::loadTexture(char const* path)
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

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
      // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
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

  ShadowMappingVisualised::ShadowMappingVisualised(Window& window, v2::Camera& camera) :
    m_window{ window }, m_camera{ camera }
  {
    float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();

    m_camera.SetPerspectiveParams({ aspect_ratio , 45.0f, 1.0f, 50.0f, true });
    m_camera.SetPosition(glm::vec3(-5.0f, 10.0f, 10.0f));
    m_camera.LookAt(glm::vec3{ 0,0,0 });

    m_camera_vis = new v2::Camera();
    m_camera_vis->SetProjectionType(v2::Camera::ProjectionType::Perspective);
    m_camera_vis->SetPerspectiveParams({ aspect_ratio , 45.0f, 0.1f, 1000.0f, true });
    m_camera_vis->SetPosition(glm::vec3(-20.0f, 15.0f, 20.0f));
    m_camera_vis->LookAt(glm::vec3{ 0,0,0 });

    m_camera_light = new v2::Camera();
    m_camera_light->SetProjectionType(v2::Camera::ProjectionType::Ortho);
    m_camera_light->SetOrthoParams({-10.0f,10.0f,-10.0f,10.0f,1.0f,7.5f });
    m_camera_light->SetPosition(lightPos);

    m_controlled_camera = &m_camera;
  }

  void ShadowMappingVisualised::Startup()
  {
    // configure global opengl state
 // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    // build and compile shaders
    // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/5.advanced_lighting/3.1.3.shadow_mapping/");
    m_shader = shader_builder.Vert("3.1.3.shadow_mapping.vs").Frag("3.1.3.shadow_mapping.fs").Build("Shader mapping shader");
    simpleDepthShader = shader_builder.Vert("3.1.3.shadow_mapping_depth.vs").Frag("3.1.3.shadow_mapping_depth.fs").Build("Simple depth shader");
    debugDepthQuad = shader_builder.Vert("3.1.3.debug_quad.vs").Frag("3.1.3.debug_quad_depth.fs").Build("Debug depth quad");

    ShaderBuilder shader_builder_light("src/test_progs/learn_opengl/2.lighting/");
    m_light_cube_shader = shader_builder_light.Vert("2.1.light_cube.vs").Frag("2.1.light_cube.fs").Build("light source shader");

    ShaderBuilder shader_builder_coords("src/test_progs/misc/coords/");
    m_coords_shader = shader_builder_coords.Vert("coords.vs").Frag("coords.fs").Build("Local Coord shader");

    ShaderBuilder shader_builder_frustum("src/test_progs/learn_opengl/8.guest/2021/csm/");
    m_frustum_shader = shader_builder_frustum.Vert("10.debug_cascade.vs").Frag("10.debug_cascade.fs").Build("Frustum shader");

    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

 
    // load textures
    // -------------
    woodTexture = loadTexture("assets/textures/wood.png");

    // configure depth map FBO
    // -----------------------
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
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
    //-----------------------------------------------------------------------------------

    // shader configuration
    // --------------------
    debugDepthQuad->Bind();
    debugDepthQuad->SetUniform1i("depthMap", 0);

    m_shader->Bind();
    m_shader->SetUniform1i("diffuseTexture", 0);
    m_shader->SetUniform1i("shadowMap", 1);
  }

  void ShadowMappingVisualised::Shutdown()
  {
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    delete m_camera_vis;
    delete m_camera_light;
  }

  void ShadowMappingVisualised::OnUpdate(double now, double time_step)
  {
    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    m_camera_light->SetPosition(lightPos);
    m_camera_light->LookAt(glm::vec3{ 0, 0, 0 });
    glm::mat4 lightSpaceMatrix = m_camera_light->GetProjMatrix() * m_camera_light->GetViewMatrix();
    
    // render scene from light's point of view (generate depth/shadow map)
    simpleDepthShader->Bind();
    simpleDepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    renderScene(*simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. render scene as normal using the generated depth/shadow map  
     // --------------------------------------------------------------
    glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.GetProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.GetViewMatrix());
    // set light uniforms
    m_shader->SetUniform3f("viewPos", m_camera.GetPosition());
    m_shader->SetUniform3f("lightPos", lightPos);
    m_shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //glCullFace(GL_FRONT);
    renderScene(*m_shader);
   // glCullFace(GL_BACK);
    renderLamp(&m_camera);

    // Render from point of view of m_camera_vis
    glBindFramebuffer(GL_FRAMEBUFFER, m_camera_vis_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->Bind(); //need to bind shader again because renderLamp binds a different shader
    m_shader->SetUniformMat4f("projection", m_camera_vis->GetProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera_vis->GetViewMatrix());
    m_shader->SetUniform3f("viewPos", m_camera_vis->GetPosition());
    renderScene(*m_shader);
    renderLamp(m_camera_vis);
    renderCamera(m_camera_vis, &m_camera);
    RenderFrustum(m_camera_vis, &m_camera, glm::vec4{0.0f, 0.0f, 1.0f, 0.15f});
    RenderFrustum(m_camera_vis, m_camera_light, glm::vec4{ 1.0f, 0.0f, 0.0f, 0.15f });
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void ShadowMappingVisualised::ImGuiUpdate()
  {
    float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
    float tex_height = 600.0f;
    float tex_width = tex_height * aspect_ratio;

    ImGui::Begin("Parameters");

    if (ImGui::CollapsingHeader("Directional Light"))
    {
      ImGui::SliderFloat3("Pos ", &(lightPos[0]), -10.0f, 10.0f);
      auto& light_ortho_params = m_camera_light->GetOrthoParameters();
      ImGui::SliderFloat("Near ", &light_ortho_params.z_near, 0.2f,2.0f);  //ImGui::SameLine();
      ImGui::SliderFloat("Far ", &light_ortho_params.z_far, 3.0f, 25.0f);

      ImGui::SliderFloat("L ", &light_ortho_params.left, -25.0f, 0.0f); //ImGui::SameLine();
      ImGui::SliderFloat("R ", &light_ortho_params.right, 0.0f, 25.0f); //ImGui::SameLine();
      ImGui::SliderFloat("B ", &light_ortho_params.bottom, -25.0f, 0.0f); //ImGui::SameLine();
      ImGui::SliderFloat("T ", &light_ortho_params.top, 0.0f, 25.0f);
    }
   
    if (ImGui::CollapsingHeader("Depth Buffer"))
    {
      ImTextureID tex_id = (void*)depthMap;
      //don't flip UV's
      ImGui::Image(tex_id, ImVec2(tex_width, tex_height));
    }

    if (ImGui::CollapsingHeader("Camera 2"))
    {
      ImTextureID tex_id = (void*)m_camera_vis_texture;
      //flip uv's in this case WTF?
      ImGui::Image(tex_id, ImVec2(tex_width, tex_height), ImVec2{ 0,1 }, ImVec2{ 1,0 }); 
    }

    ImGui::End();
  }

  //from CSM
  std::vector<glm::vec4> ShadowMappingVisualised::GetFrustumCornersWorldSpace(const glm::mat4& proj, 
    const glm::mat4& view)
  {
    //converts from NDC to world space
    const auto inv = glm::inverse(proj * view);
    std::vector<glm::vec4> frustumCorners;
    std::vector<glm::vec4> frustumCorners_ndc;
    for (unsigned int x = 0; x < 2; ++x)
    {
      for (unsigned int y = 0; y < 2; ++y)
      {
        for (unsigned int z = 0; z < 2; ++z)
        {
          const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
          frustumCorners.push_back(pt / pt.w);

          const glm::vec4 pt_ndc = glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
          frustumCorners_ndc.push_back(pt_ndc);
        }
      }
    }
     return frustumCorners;
  }

  void ShadowMappingVisualised::RenderFrustum(v2::Camera* viewing_camera, v2::Camera* rendered_camera, glm::vec4 color)
  {
    std::vector<glm::vec4> frustum_corners_ws = GetFrustumCornersWorldSpace(rendered_camera->GetProjMatrix(), rendered_camera->GetViewMatrix());

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

  // renders the 3D scene
  // --------------------
  void ShadowMappingVisualised::renderScene(Shader& shader)
  {
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetUniformMat4f("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMat4f("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetUniformMat4f("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.SetUniformMat4f("model", model);
    renderCube();
  }

  void ShadowMappingVisualised::renderLamp(v2::Camera* camera)
  {
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    glm::mat4 model = m_camera_light->GetTransform();
    model = model * scale;

    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("model", model);
    m_light_cube_shader->SetUniformMat4f("view", camera->GetViewMatrix());
    m_light_cube_shader->SetUniformMat4f("projection", camera->GetProjMatrix());
    renderCube();
    m_light_cube_shader->Unbind();
    RenderCoords(camera, model);
  }

  void ShadowMappingVisualised::renderCamera(v2::Camera* viewing_camera, v2::Camera* rendered_camera)
  {
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
    glm::mat4 model = rendered_camera->GetTransform();
    model = model * scale;

    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("model", model);
    m_light_cube_shader->SetUniformMat4f("view", viewing_camera->GetViewMatrix());
    m_light_cube_shader->SetUniformMat4f("projection", viewing_camera->GetProjMatrix());
    renderCube();
    m_light_cube_shader->Unbind();
    RenderCoords(viewing_camera, model);
  }

  void  ShadowMappingVisualised::RenderCoords(v2::Camera* camera, glm::mat4& model)
  {
    if (coordsVAO == 0)
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
      glGenVertexArrays(1, &coordsVAO);
      glBindVertexArray(coordsVAO);

      glGenBuffers(1, &coordsVBO);
      glBindBuffer(GL_ARRAY_BUFFER, coordsVBO);
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

    glBindVertexArray(coordsVAO);
    //glLineWidth(2.0f); //cannot do in openGL 3.2 + gives a INVALID_VALUE error
    glDrawArrays(GL_LINES, 0, m_coord_verts.size());
    //glLineWidth(1);
    m_coords_shader->Unbind();
  }


  // renderCube() renders a 1x1 3D cube in NDC.
  // -------------------------------------------------
  void ShadowMappingVisualised::renderCube()
  {
    // initialize (if necessary)
    if (cubeVAO == 0)
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
      glGenVertexArrays(1, &cubeVAO);
      glGenBuffers(1, &cubeVBO);
      // fill buffer
      glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      // link vertex attributes
      glBindVertexArray(cubeVAO);
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
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
  }

  // renderQuad() renders a 1x1 XY quad in NDC
  // -----------------------------------------
  void ShadowMappingVisualised::renderQuad()
  {
    if (quadVAO == 0)
    {
      float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      };
      // setup plane VAO
      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }

  void ShadowMappingVisualised::CheckKeys(double delta_time)
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

  void ShadowMappingVisualised::OnEvent(Event& event)
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
      else if(keys[GLFW_KEY_LEFT_SHIFT])
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

