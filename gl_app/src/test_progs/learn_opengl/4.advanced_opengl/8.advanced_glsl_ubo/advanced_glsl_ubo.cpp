#include "pch.h"
#include <GL\glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <imgui_docking\imgui.h>
//#include <imgui_docking\imgui_impl_glfw.h>
//#include <imgui_docking\imgui_impl_opengl3.h>

#include "camera.h"

#include "stb_image/stb_image.h"
#include "shader.h"

#include "advanced_glsl_ubo.h"

  // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
static float cubeVertices[] = {
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
    -0.5f,  0.5f, -0.5f,
};

AdvancedGLSL_UBO::AdvancedGLSL_UBO(Camera& camera) :
    m_camera(camera)
{
}

void AdvancedGLSL_UBO::Startup()
{
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	ShaderBuilder shader_builder("src/test_progs/learn_opengl/4.advanced_opengl/8.advanced_glsl_ubo/");
	shaderRed = shader_builder.Vert("8.advanced_glsl.vs").Frag("8.red.fs").Build("Red Shader");
	shaderGreen = shader_builder.Vert("8.advanced_glsl.vs").Frag("8.green.fs").Build("Green Shader");
	shaderBlue = shader_builder.Vert("8.advanced_glsl.vs").Frag("8.blue.fs").Build("Blue Shader");
	shaderYellow = shader_builder.Vert("8.advanced_glsl.vs").Frag("8.yellow.fs").Build("Yellow Shader");

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // configure a uniform buffer object
    // //More info on this in sb7 book, ch5 - straight after listing 5.19
  // ---------------------------------
  // first. We get the relevant block indices
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed->GetProgramID(), "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen->GetProgramID(), "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue->GetProgramID(), "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow->GetProgramID(), "Matrices");
    // then we link each shader's uniform block to this uniform binding point
    glUniformBlockBinding(shaderRed->GetProgramID(), uniformBlockIndexRed, 0);
    glUniformBlockBinding(shaderGreen->GetProgramID(), uniformBlockIndexGreen, 0);
    glUniformBlockBinding(shaderBlue->GetProgramID(), uniformBlockIndexBlue, 0);
    glUniformBlockBinding(shaderYellow->GetProgramID(), uniformBlockIndexYellow, 0);
    // Now actually create the buffer
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glm::mat4 projection = m_camera.ProjMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void AdvancedGLSL_UBO::Shutdown()
{
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
}

void AdvancedGLSL_UBO::OnUpdate(double now, double time_step)
{
    // set the view and projection matrix in the uniform block - we only have to do this once per loop iteration.
    glm::mat4 view = m_camera.ViewMatrix();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // draw 4 cubes 
    // RED
    glBindVertexArray(cubeVAO);
    shaderRed->Bind();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
    shaderRed->SetUniformMat4f("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // GREEN
    shaderGreen->Bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
    shaderGreen->SetUniformMat4f("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // YELLOW
    shaderYellow->Bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
    shaderYellow->SetUniformMat4f("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // BLUE
    shaderBlue->Bind();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
    shaderBlue->SetUniformMat4f("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

void AdvancedGLSL_UBO::ImGuiUpdate()
{
}
