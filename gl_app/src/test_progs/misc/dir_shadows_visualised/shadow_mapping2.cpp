#include "pch.h"
#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "camera.h"
#include "window.h"
#include "stb_image/stb_image.h"
#include "shader.h"

#include "shadow_mapping2.h"
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
  unsigned int ShadowMappingVisualised2::loadTexture(char const* path)
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

  ShadowMappingVisualised2::ShadowMappingVisualised2(Window& window, Camera& camera) :
    m_window{ window }, m_camera{ camera }
  {
    float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
    m_camera_vis = new PerspectiveCamera(aspect_ratio);
    
    m_camera_light = new OrthographicCamera();
    m_camera_light->SetPosition(lightPos);
  }

  void ShadowMappingVisualised2::Startup()
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

  void ShadowMappingVisualised2::Shutdown()
  {
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    delete m_camera_vis;
    delete m_camera_light;
  }

  void ShadowMappingVisualised2::OnUpdate(double now, double time_step)
  {
    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    /*glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;*/

    float near_plane = 1.0f, far_plane = 7.5f;
    m_camera_light->SetPosition(lightPos);
    m_camera_light->LookAt(glm::vec3{ 0, 0, 0 });
    m_camera_light->SetViewVolume(glm::vec4{ -10.0f, 10.0f, -10.0f, 10.0f }, near_plane, far_plane);
    glm::mat4 lightSpaceMatrix = m_camera_light->ProjMatrix() * m_camera_light->ViewMatrix();
    //glm::mat4 diff = lightSpaceMatrix - lightSpaceMatrix2;

    // render scene from light's point of view
    simpleDepthShader->Bind();
    simpleDepthShader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, woodTexture);
    renderScene(*simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. render scene as normal using the generated depth/shadow map  
     // --------------------------------------------------------------
    glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shader->Bind();
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    // set light uniforms
    m_shader->SetUniform3f("viewPos", m_camera.Position());
    m_shader->SetUniform3f("lightPos", lightPos);
    m_shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //glCullFace(GL_FRONT);
    renderScene(*m_shader);
    //glCullFace(GL_BACK);
    renderLamp(&m_camera);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    debugDepthQuad->Bind();
    //debugDepthQuad->SetUniform1f("near_plane", near_plane);
    //debugDepthQuad->SetUniform1f("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    //renderQuad();
  }

  void ShadowMappingVisualised2::ImGuiUpdate()
  {
    ImGui::Begin("Parameters");

    if (ImGui::CollapsingHeader("Directional Light"))
    {
      ImGui::SliderFloat3("Pos ", &(lightPos[0]), -10.0f, 10.0f);
      //ImGui::SliderFloat4("Extents", this->)
      /*ImGui::SliderFloat3("Ambient ", &(light_ambient[0]), 0.0f, 1.0f);
      ImGui::SliderFloat3("Diffuse ", &(light_diffuse[0]), 0.0f, 1.0f);
      ImGui::SliderFloat3("Specular ", &(light_specular[0]), 0.0f, 1.0f);*/
    }
    /*if (ImGui::CollapsingHeader("Box Properties"))
    {
      ImGui::SliderFloat3("Box Pos ", &(BoxPos[0]), -5.0f, 5.0f);
      ImGui::SliderFloat("Shininess ", &(shininess), 1.0f, 256.0f);
    }*/

    ImGui::End();
  }

  // renders the 3D scene
  // --------------------
  void ShadowMappingVisualised2::renderScene(Shader& shader)
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

  void ShadowMappingVisualised2::renderLamp(Camera* camera)
  {
    
    //glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, lightPos);
    //model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube

    //same as above
    //glm::mat4 translation = glm::translate(glm::mat4(1.0f), lightPos);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
    //glm::mat4 model = translation * scale;

    glm::mat4 light_model = m_camera_light->ModelMatrix();
    light_model = light_model * scale;

    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("model", light_model);
    m_light_cube_shader->SetUniformMat4f("view", camera->ViewMatrix());
    m_light_cube_shader->SetUniformMat4f("projection", camera->ProjMatrix());
    renderCube();
    m_light_cube_shader->Unbind();
    RenderCoords(&m_camera, light_model);
  }

  
  void  ShadowMappingVisualised2::RenderCoords(Camera* camera, glm::mat4& model)
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
    m_coords_shader->SetUniformMat4f("u_view", camera->ViewMatrix());
    m_coords_shader->SetUniformMat4f("u_proj", camera->ProjMatrix());

    glBindVertexArray(coordsVAO);
    //glLineWidth(2.0f); //cannot do in openGL 3.2 + gives a INVALID_VALUE error
    glDrawArrays(GL_LINES, 0, m_coord_verts.size());
    //glLineWidth(1);
    m_coords_shader->Unbind();
  }


  // renderCube() renders a 1x1 3D cube in NDC.
  // -------------------------------------------------
  void ShadowMappingVisualised2::renderCube()
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
  void ShadowMappingVisualised2::renderQuad()
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

}

