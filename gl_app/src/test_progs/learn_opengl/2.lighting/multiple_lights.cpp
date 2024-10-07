#include "pch.h"
#include <GL\glew.h>

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include "glm/gtc/matrix_transform.hpp"
#include "stb_image/stb_image.h"

#include "camera.h"
#include "shader.h"

#include "multiple_lights.h"

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
// positions all containers
static glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
// positions of the point lights
static glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
};

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int MultipleLights::loadTexture(char const* path)
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

MultipleLights::MultipleLights(Camera& camera) :
	m_camera(camera)
{
}

void MultipleLights::Startup()
{
    // configure global opengl state
// -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // AOGL p20

    // build and compile our shader zprogram
    // ------------------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/2.lighting/");
    m_shader = shader_builder.Vert("6.multiple_lights.vs").Frag("6.multiple_lights.fs").Build("Multiple lights");
    m_light_cube_shader = shader_builder.Vert("2.1.light_cube.vs").Frag("2.1.light_cube.fs").Build("light source shader");

    // first, configure the cube's VAO (and VBO)
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

void MultipleLights::Shutdown()
{
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
}

void MultipleLights::OnUpdate(double now, double time_step)
{
    m_shader->Bind();

    m_shader->SetUniform3f("viewPos", m_camera.Position());
    m_shader->SetUniform1f("material.shininess", 32.0f);

    //Directional light properties
    m_shader->SetUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
    m_shader->SetUniform3f("dirLight.ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    m_shader->SetUniform3f("dirLight.specular", 1.0f, 1.0f, 1.0f);

    //point light 1
    m_shader->SetUniform3f("pointLights[0].position", pointLightPositions[0]);
    m_shader->SetUniform3f("pointLights[0].ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("pointLights[0].diffuse", 0.5f, 0.5f, 0.5f);
    m_shader->SetUniform3f("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("pointLights[0].constant", 1.0f);
    m_shader->SetUniform1f("pointLights[0].linear", 0.09f);
    m_shader->SetUniform1f("pointLights[0].quadratic", 0.032f);

    //point light 2
    m_shader->SetUniform3f("pointLights[1].position", pointLightPositions[1]);
    m_shader->SetUniform3f("pointLights[1].ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("pointLights[1].diffuse", 0.5f, 0.5f, 0.5f);
    m_shader->SetUniform3f("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("pointLights[1].constant", 1.0f);
    m_shader->SetUniform1f("pointLights[1].linear", 0.09f);
    m_shader->SetUniform1f("pointLights[1].quadratic", 0.032f);

    //point light 3
    m_shader->SetUniform3f("pointLights[2].position", pointLightPositions[2]);
    m_shader->SetUniform3f("pointLights[2].ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("pointLights[2].diffuse", 0.5f, 0.5f, 0.5f);
    m_shader->SetUniform3f("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("pointLights[2].constant", 1.0f);
    m_shader->SetUniform1f("pointLights[2].linear", 0.09f);
    m_shader->SetUniform1f("pointLights[2].quadratic", 0.032f);

    //point light 4
    m_shader->SetUniform3f("pointLights[3].position", pointLightPositions[3]);
    m_shader->SetUniform3f("pointLights[3].ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("pointLights[3].diffuse", 0.5f, 0.5f, 0.5f);
    m_shader->SetUniform3f("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("pointLights[3].constant", 1.0f);
    m_shader->SetUniform1f("pointLights[3].linear", 0.09f);
    m_shader->SetUniform1f("pointLights[3].quadratic", 0.032f);

    //spot light
    m_shader->SetUniform3f("spotLight.position", m_camera.Position());
    m_shader->SetUniform3f("spotLight.direction", m_camera.Front());
    m_shader->SetUniform3f("spotLight.ambient", 0.2f, 0.2f, 0.2f);
    m_shader->SetUniform3f("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
    m_shader->SetUniform3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
    m_shader->SetUniform1f("spotLight.constant", 1.0f);
    m_shader->SetUniform1f("spotLight.linear", 0.09f);
    m_shader->SetUniform1f("spotLight.quadratic", 0.032f);
    m_shader->SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    m_shader->SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    //transformations
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
    m_shader->SetUniformMat4f("model", glm::mat4(1.0f));

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // render containers
    glBindVertexArray(cubeVAO);
    for (unsigned int i = 0; i < 10; i++)
    {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m_shader->SetUniformMat4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

   // also draw the lamp object(s)
   m_light_cube_shader->Bind();
   m_light_cube_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
   m_light_cube_shader->SetUniformMat4f("view", m_camera.ViewMatrix());
   
    // we now draw as many light bulbs as we have point lights.
    glBindVertexArray(lightCubeVAO);
    for (unsigned int i = 0; i < 4; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        m_light_cube_shader->SetUniformMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void MultipleLights::ImGuiUpdate()
{
}
