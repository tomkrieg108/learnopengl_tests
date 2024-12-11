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

#include "ibl_diffuse_irradiance_1.h"

// lights
// ------
static glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3(10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
static glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};
static int nrRows = 7;
static int nrColumns = 7;
static float spacing = 2.5;

IblDiffuseIrradiance1::IblDiffuseIrradiance1(Window& window, v2::Camera& camera) :
  m_window{ window }, m_camera{ camera }
{
  float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
  m_camera.SetPerspectiveParams({ aspect_ratio , 45.0f, 1.0f, 50.0f, true });
  m_camera.SetPosition(glm::vec3(-5.0f, 10.0f, 10.0f));
  m_camera.LookAt(glm::vec3{ 0,0,0 });
}


void IblDiffuseIrradiance1::Startup()
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

  ShaderBuilder shader_builder("src/test_progs/learn_opengl/6.pbr/3.1.ibl_diffuse_irradiance/");
  pbrShader = shader_builder.Vert("2.1.2.pbr.vs").Frag("2.1.2.pbr.fs").Build("PBR shader");
  equirectangularToCubemapShader = shader_builder.Vert("2.1.2.cubemap.vs")
    .Frag("2.1.2.equirectangular_to_cubemap.fs").Build("equirectangular To Cubemap Shader");
  //irradianceShader = shader_builder.Vert("2.1.2.pbr.vs").Frag("2.1.2.irradiance_convolution.fs").Build("irradiance Shader");
  backgroundShader = shader_builder.Vert("2.1.2.background.vs").Frag("2.1.2.background.fs").Build("Background shader");

  //ShaderBuilder shader_builder2("src/test_progs/learn_opengl/6.pbr/1.pbr_basic/");
  //pbrShader = shader_builder2.Vert("1.1.pbr.vs").Frag("1.1.pbr.fs").Build("Basic PBR shader");

  pbrShader->Bind();
  //pbrShader->SetUniform1i("irradianceMap", 0);
  pbrShader->SetUniform3f("albedo", 0.5f, 0.0f, 0.0f);
  pbrShader->SetUniform1f("ao", 1.0f);

  backgroundShader->Bind();
  backgroundShader->SetUniform1i("environmentMap", 0);

  // pbr: setup framebuffer
  // ----------------------
  glGenFramebuffers(1, &captureFBO);
  glGenRenderbuffers(1, &captureRBO);

  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

  // pbr: load the HDR environment map
   // ---------------------------------
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  float* data = stbi_loadf("assets/polyhaven/hdri/christmas_photo_studio_01_2k.hdr", &width, &height, &nrComponents, 0);
  if (data)
  {
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Failed to load HDR image." << std::endl;
  }

  // pbr: setup cubemap to render to and attach to framebuffer
  // ---------------------------------------------------------
  glGenTextures(1, &envCubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  for (unsigned int i = 0; i < 6; ++i)
  {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
  // ----------------------------------------------------------------------------------------------
  captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

  // pbr: convert HDR equirectangular environment map to cubemap equivalent
   // ----------------------------------------------------------------------
  equirectangularToCubemapShader->Bind();
  equirectangularToCubemapShader->SetUniform1i("equirectangularMap", 0);
  equirectangularToCubemapShader->SetUniformMat4f("projection", captureProjection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);
  glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
  glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
  for (unsigned int i = 0; i < 6; ++i)
  {
    equirectangularToCubemapShader->SetUniformMat4f("view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderCube();
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  // initialize static shader uniforms before rendering
  // --------------------------------------------------
  //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
  pbrShader->Bind();
  pbrShader->SetUniformMat4f("projection", m_camera.GetProjMatrix());
  backgroundShader->Bind();
  backgroundShader->SetUniformMat4f("projection", m_camera.GetProjMatrix());

  // then before rendering, configure the viewport to the original framebuffer's screen dimensions
  //int scrWidth, scrHeight;
  //glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
  glViewport(0, 0, m_window.BufferWidth(), m_window.BufferHeight());
}

void IblDiffuseIrradiance1::Shutdown()
{
  glfwTerminate();
}

void IblDiffuseIrradiance1::OnUpdate(double now, double time_step)
{
  // pbr: convert HDR equirectangular environment map to cubemap equivalent
   // ----------------------------------------------------------------------
  /*equirectangularToCubemapShader->Bind();
  equirectangularToCubemapShader->SetUniform1i("equirectangularMap", 0);
  equirectangularToCubemapShader->SetUniformMat4f("view", m_camera.GetViewMatrix());
  equirectangularToCubemapShader->SetUniformMat4f("projection", m_camera.GetProjMatrix());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdrTexture);
  renderCube();*/

  // render scene, supplying the convoluted irradiance map to the final shader.
  // ------------------------------------------------------------------------------------------
  pbrShader->Bind();
  pbrShader->SetUniformMat4f("view", m_camera.GetViewMatrix());
  pbrShader->SetUniform3f("camPos", m_camera.GetPosition());

  // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
  glm::mat4 model = glm::mat4(1.0f);
  for (int row = 0; row < nrRows; ++row)
  {
    pbrShader->SetUniform1f("metallic", (float)row / (float)nrRows);
    for (int col = 0; col < nrColumns; ++col)
    {
      // we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
      // on direct lighting.
      pbrShader->SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(
        (float)(col - (nrColumns / 2)) * spacing,
        (float)(row - (nrRows / 2)) * spacing,
        -2.0f
      ));
      pbrShader->SetUniformMat4f("model", model);
      pbrShader->SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
      renderSphere();
    }
  }

  // render light source (simply re-render sphere at light positions)
  // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
  // keeps the codeprint small.
  for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
  {
    glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
    newPos = lightPositions[i];
    pbrShader->SetUniform3f("lightPositions[" + std::to_string(i) + "]", newPos);
    pbrShader->SetUniform3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

    model = glm::mat4(1.0f);
    model = glm::translate(model, newPos);
    model = glm::scale(model, glm::vec3(0.5f));
    pbrShader->SetUniformMat4f("model", model);
    pbrShader->SetUniformMat3f("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
    renderSphere();
  }

  // render skybox (render as last to prevent overdraw)
  backgroundShader->Bind();
  backgroundShader->SetUniformMat4f("view", m_camera.GetViewMatrix());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
  renderCube();
}

void IblDiffuseIrradiance1::ImGuiUpdate()
{
}

void IblDiffuseIrradiance1::CheckKeys(double delta_time)
{
  const float move_speed = 5.0f;
  const float t = (float)(delta_time);
  bool* keys = m_window.GetKeys().m_key_code;

  if (keys[GLFW_KEY_W])
    m_camera.MoveForward(-move_speed * t); //note the negative value needed to move forward

  if (keys[GLFW_KEY_S])
    m_camera.MoveForward(move_speed * t); //note the positive value needed to move backward

  if (keys[GLFW_KEY_A])
    m_camera.MoveRight(-move_speed * t);

  if (keys[GLFW_KEY_D])
    m_camera.MoveRight(move_speed * t);
}

void IblDiffuseIrradiance1::OnEvent(Event& event)
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
      m_camera.RotateWorld(e.delta_x * 0.03f, e.delta_y * 0.03f);
    else if (keys[GLFW_KEY_LEFT_SHIFT])
      m_camera.MoveForward((e.delta_x + e.delta_y) * 0.01f);
    else
      m_camera.RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);
  }

  if (event.Type() == Event::kMouseScroll)
  {
    EventMouseScroll& e = dynamic_cast<EventMouseScroll&>(event);
    m_camera.Zoom(e.y_offset);
  }

  if (event.Type() == Event::kKeyPressed)
  {
    /*EventKeyPressed& e = dynamic_cast<EventKeyPressed&>(event);
    if (e.key == GLFW_KEY_T)
    {
      if (m_controlled_camera == &m_camera)
        m_controlled_camera = m_camera_vis;
      else
        m_controlled_camera = &m_camera;
    }*/
  }
}

void IblDiffuseIrradiance1::renderSphere()
{
  if (sphereVAO == 0)
  {
    glGenVertexArrays(1, &sphereVAO);

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
      {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
      if (!oddRow) // even rows: y == 0, y == 2; and so on
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      }
      else
      {
        for (int x = X_SEGMENTS; x >= 0; --x)
        {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0)
      {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0)
      {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
  }

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void IblDiffuseIrradiance1::renderCube()
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

unsigned int IblDiffuseIrradiance1::loadTexture(char const* path)
{
	return 0;
}
