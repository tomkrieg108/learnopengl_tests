#include "pch.h"
#include <GL\glew.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "glm/gtc/matrix_transform.hpp"
#include "stb_image/stb_image.h"
#include "camera.h"
#include "shader.h"
#include "vertex_array_4_5.h"

//This comes from "light_map_specular" in 2.lighting of learn openGL.
//using OpenGL 4.5 functions to setup VBO and VAO

namespace sb7
{

  static float positions[] =
  {
    // positions        
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
  };

  static float normals[] =
  {
    // normals         
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,

    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,

    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,

    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,

    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
  };

  static float tex_coords[]
  {
    // texture co
    0.0f,  0.0f,
    1.0f,  0.0f,
    1.0f,  1.0f,
    1.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,

    0.0f,  0.0f,
    1.0f,  0.0f,
    1.0f,  1.0f,
    1.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,

    1.0f,  0.0f,
    1.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  0.0f,

    1.0f,  0.0f,
    1.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  1.0f,
    0.0f,  0.0f,
    1.0f,  0.0f,

    0.0f,  1.0f,
    1.0f,  1.0f,
    1.0f,  0.0f,
    1.0f,  0.0f,
    0.0f,  0.0f,
    0.0f,  1.0f,

    0.0f,  1.0f,
    1.0f,  1.0f,
    1.0f,  0.0f,
    1.0f,  0.0f,
    0.0f,  0.0f,
    0.0f,  1.0f
  };


  // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
  static float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  };

  // utility function for loading a 2D texture from file
  // ---------------------------------------------------
  unsigned int LightMapSpecular::loadTexture(char const* path)
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

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

  LightMapSpecular::LightMapSpecular(Camera& camera) :
    m_camera(camera)
  {
  }

  void LightMapSpecular::Startup()
  {
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // AOGL p20

    // build and compile our shader zprogram
    // ------------------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/2.lighting/");
    m_shader = shader_builder.Vert("4.2.lighting_maps.vs").Frag("4.2.lighting_maps.fs").Build("Light Map Specular");
    m_light_cube_shader = shader_builder.Vert("2.1.light_cube.vs").Frag("2.1.light_cube.fs").Build("light shouce shader");

    //https://stackoverflow.com/questions/32739297/direct-state-access-with-vertex-buffers
    //sb7 ch5 - buffers - GL v4.5+

#if 0
    //--------------------------------------------------------------------------------------
    // SAO - using multiple buffers.  this works
    //--------------------------------------------------------------------------------------

    GLuint buffer[3];
    glCreateBuffers(3, buffer);

    glCreateVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO); //Need to do this for call to  glEnableVertexAttribArray()

    //Load buffer with data. Last param is flags - none set here
    glNamedBufferStorage(buffer[0], sizeof(positions), positions, 0);  
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //Make association of the buffer with the VAO
    glVertexArrayVertexBuffer(cubeVAO, 0, buffer[0], 0, sizeof(glm::vec3));
    //param 2 (bindingindex) - matches param in glVertexArrayAttribBinding
    //param 4 (offset) - offset into the buffer 
    //param 5 (stride) - how far apart each vertex is in bytes (only 1 attribute in this case - position)
    
    //Specify the format of the attribute
    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    // param 2  (attribindex) is the attribute location in the vertex shader - matches param in glVertexArrayAttribBinding
    // param 3  (size) of the attribute in elements
    // param 5 (normalized) - applies to ints only - ignored for floats.  if true, int value will be scaled to [0,1] as a float.  False - unchanged, but still converted to a float
    // param 6 (relative offset) - offset in bytes to this attribute from start of vertex.  If left at 0, opengl calculates for you
    
    //Associate the binding index with the vertex shader attribute
    //Set the binding that a shader vertex attribute uses to reference a buffer
    glVertexArrayAttribBinding(cubeVAO, 0, 0);
    //param 2 (attribindex) is the attribute location in the vertex shader
    //param 3 (bindingindex)
    
    //Feed data to the vertex shader
    //tell opengGL to use data in buffer rather than data supplied from glVertexAttrib*() 
    glEnableVertexAttribArray(0);
    //param 1 is the vertex attribute location specified in shader - corresponds to attribindex
    
    //With this, can now supply data using glVertexAttrib*() again
    //glDisableVertexAttribArray(0); 

    //repeat for normals
    glNamedBufferStorage(buffer[1], sizeof(normals), normals, 0);
    glVertexArrayVertexBuffer(cubeVAO, 1, buffer[1], 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 1, 1);
    glEnableVertexAttribArray(1);

    //repeat for tex coords
    glNamedBufferStorage(buffer[2], sizeof(tex_coords), tex_coords, 0);
    glVertexArrayVertexBuffer(cubeVAO, 2, buffer[2], 0, sizeof(glm::vec2));
    glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 2, 2);
    glEnableVertexAttribArray(2);

    
    // second, configure the light's VAO
    glCreateVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO); //Need to do this
    glVertexArrayVertexBuffer(lightCubeVAO, 0, buffer[0], 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(lightCubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(lightCubeVAO, 0, 0);
    glEnableVertexAttribArray(0);
#endif

#if 0
    //--------------------------------------------------------------------------------------
    // SAO - effectively the same as above, but using a single buffer - this works
    //--------------------------------------------------------------------------------------
    GLuint buffer;
    glCreateBuffers(1, &buffer);
    auto buff_size = sizeof(positions) + sizeof(normals) + sizeof(tex_coords);
    //Need GL_DYNAMIC_STORAGE_BIT to use glNamedBufferSubData
    //glNamedBufferStorage(buffer, offset, size, data)
    glNamedBufferStorage(buffer, buff_size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(buffer, 0, sizeof(positions), positions);
    glNamedBufferSubData(buffer, sizeof(positions), sizeof(normals), normals);
    glNamedBufferSubData(buffer, sizeof(positions) + sizeof(normals), sizeof(tex_coords), tex_coords);

    //can also use glMapNamedBufferRange() to get a pointer to the buffer, and use memcpy()

    glCreateVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO); //Need to do this

    //positions
    glVertexArrayVertexBuffer(cubeVAO, 0, buffer, 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 0, 0);
    glEnableVertexAttribArray(0);

    //normals
    glVertexArrayVertexBuffer(cubeVAO, 1, buffer, sizeof(positions), sizeof(glm::vec3));
    glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 1, 1);
    glEnableVertexAttribArray(1);

    //tex coords
    glVertexArrayVertexBuffer(cubeVAO, 2, buffer, sizeof(positions) + sizeof(normals), sizeof(glm::vec2));
    glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 2, 2);
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO
    glCreateVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO); //Need to do this
    glVertexArrayVertexBuffer(lightCubeVAO, 0, buffer, 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(lightCubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(lightCubeVAO, 0, 0);
    glEnableVertexAttribArray(0);

#endif

#if 1
    //--------------------------------------------------------------------------------------
   // AOS - interleaved attribute data - this works
   //--------------------------------------------------------------------------------------
    GLuint buffer;
    glCreateBuffers(1, &buffer);
    glNamedBufferStorage(buffer, sizeof(vertices), vertices, 0);

    glCreateVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO); //Need to do this

    const auto stride = sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2);

    //position
    glVertexArrayVertexBuffer(cubeVAO, 0, buffer, 0, stride);
    glVertexArrayAttribFormat(cubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(cubeVAO, 0, 0);
    glEnableVertexAttribArray(0);

    //normal
    glVertexArrayVertexBuffer(cubeVAO, 1, buffer, 0, stride);
    glVertexArrayAttribFormat(cubeVAO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));
    glVertexArrayAttribBinding(cubeVAO, 1, 1);
    glEnableVertexAttribArray(1);

    //tex coords
    glVertexArrayVertexBuffer(cubeVAO, 2, buffer, 0, stride);
    glVertexArrayAttribFormat(cubeVAO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)+ sizeof(glm::vec3));
    glVertexArrayAttribBinding(cubeVAO, 2, 2);
    glEnableVertexAttribArray(2);


    // second, configure the light's VAO
    glCreateVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO); //Need to do this
    glVertexArrayVertexBuffer(lightCubeVAO, 0, buffer, 0, stride);
    glVertexArrayAttribFormat(lightCubeVAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(lightCubeVAO, 0, 0);
    glEnableVertexAttribArray(0);

#endif

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    diffuseMap = loadTexture("assets/textures/container2.png");
    specularMap = loadTexture("assets/textures/container2_specular.png");

    // shader configuration
    // --------------------
    m_shader->Bind();
    m_shader->SetUniform1i("material.diffuse", 0);
    m_shader->SetUniform1i("material.specular", 1);

    m_shader->OutputInfo();
    m_light_cube_shader->OutputInfo();
  }

  void LightMapSpecular::Shutdown()
  {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
  }

  void LightMapSpecular::OnUpdate(double now, double time_step)
  {
    m_shader->Bind();

    m_shader->SetUniform3f("light.position", lightPos);
    m_shader->SetUniform3f("viewPos", m_camera.Position());

    //light properties
    m_shader->SetUniform3f("light.ambient", light_ambient);
    m_shader->SetUniform3f("light.diffuse", light_diffuse);
    m_shader->SetUniform3f("light.specular", light_specular);

    //material properties
    m_shader->SetUniform1f("material.shininess", shininess);

    //transformations
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, BoxPos);
    m_shader->SetUniformMat4f("model", model);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // also draw the lamp object
    m_light_cube_shader->Bind();
    m_light_cube_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_light_cube_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    m_light_cube_shader->SetUniformMat4f("model", model);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  void LightMapSpecular::ImGuiUpdate()
  {
    ImGui::Begin("Light map specular");

    if (ImGui::CollapsingHeader("Light Properties"))
    {
      ImGui::SliderFloat3("Light Pos ", &(lightPos[0]), -5.0f, 5.0f);
      ImGui::SliderFloat3("Ambient ", &(light_ambient[0]), 0.0f, 1.0f);
      ImGui::SliderFloat3("Diffuse ", &(light_diffuse[0]), 0.0f, 1.0f);
      ImGui::SliderFloat3("Specular ", &(light_specular[0]), 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Box Properties"))
    {
      ImGui::SliderFloat3("Box Pos ", &(BoxPos[0]), -5.0f, 5.0f);
      ImGui::SliderFloat("Shininess ", &(shininess), 1.0f, 256.0f);
    }

    ImGui::End();

  }

}