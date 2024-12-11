#include "pch.h"
#include <GL\glew.h>

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

#include "gamma_correction.h"

// set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
static float planeVertices[] = {
    // positions            // normals         // texcoords
     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

     10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
    -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
     10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

// lighting info
    // -------------
static glm::vec3 lightPositions[] = {
    glm::vec3(-3.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(3.0f, 0.0f, 0.0f)
};

static glm::vec3 lightColors[] = {
    glm::vec3(0.25),
    glm::vec3(0.50),
    glm::vec3(0.75),
    glm::vec3(1.00)
};

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int GammaCorrection::loadTexture(char const* path, bool gamma_correction)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum dataFormat=0;
        GLenum internalFormat=0;
        if (nrComponents == 1)
        {
          dataFormat = GL_RED;
          internalFormat = GL_RED;
        }
         
        else if (nrComponents == 3)
        {
          dataFormat = GL_RGB;
          internalFormat = gamma_correction ? GL_SRGB : GL_RGB;
        }
            
        else if (nrComponents == 4)
        {
          dataFormat = GL_RGBA;
          internalFormat = gamma_correction ? GL_SRGB_ALPHA : GL_RGBA;
        }
            
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
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

GammaCorrection::GammaCorrection(Camera& camera, Window& window) :
	m_camera{camera}, m_window{window}
{
}

void GammaCorrection::Startup()
{
    // configure global opengl state
 // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    // -------------------------
    ShaderBuilder shader_builder("src/test_progs/learn_opengl/5.advanced_lighting/2.gamma_correction/");
    m_shader = shader_builder.Vert("2.gamma_correction.vs").Frag("2.gamma_correction.fs").Build("Gamma Correction shader");

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
    floorTexture = loadTexture("assets/textures/wood.png", false);
    floorTextureGammaCorrected = loadTexture("assets/textures/wood.png", true);

    // shader configuration
    // --------------------
    m_shader->Bind();
    m_shader->SetUniform1i("texture1", 0);
    m_shader->OutputInfo();
}

void GammaCorrection::Shutdown()
{
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
}

void GammaCorrection::OnUpdate(double now, double time_step)
{
    bool* keys = m_window.GetKeys().m_key_code;
    gammaEnabled = keys[GLFW_KEY_G];

    m_shader->Bind();

    //transformations
    m_shader->SetUniformMat4f("projection", m_camera.ProjMatrix());
    m_shader->SetUniformMat4f("view", m_camera.ViewMatrix());


    // set light uniforms
    glUniform3fv(glGetUniformLocation(m_shader->GetProgramID(), "lightPositions"), 4, &lightPositions[0][0]);
    glUniform3fv(glGetUniformLocation(m_shader->GetProgramID(), "lightColors"), 4, &lightColors[0][0]);
    m_shader->SetUniform3f("viewPos", m_camera.Position());
    m_shader->SetUniform1i("gamma", gammaEnabled);

    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gammaEnabled ? floorTextureGammaCorrected : floorTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    std::cout << (gammaEnabled ? "Gamma enabled" : "Gamma disabled") << std::endl;
}

void GammaCorrection::ImGuiUpdate()
{
}
