#pragma once

#include "layer.h"
#include "lgl_model.h"

class Shader;
class Camera;
class Window;

//Screen-space ambient occlusion
class SSAO : public Layer
{
public:
	SSAO(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> shaderGeometryPass = nullptr;
	std::unique_ptr<Shader> shaderLightingPass = nullptr;
	std::unique_ptr<Shader> shaderSSAO = nullptr;
	std::unique_ptr<Shader> shaderSSAOBlur = nullptr;
	std::unique_ptr<Shader> shaderDebug = nullptr;

	lgl::Model* backpack = nullptr;

	unsigned int gBuffer = 0;
	unsigned int gPosition = 0, gNormal = 0, gAlbedo = 0;
	unsigned int rboDepth = 0;

	Camera& m_camera;
	Window& m_window;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int noiseTexture;
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

	std::vector<glm::vec3> ssaoKernel;

	// lighting info
	// -------------
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	//unsigned int loadTexture(char const* path);
	void renderCube();
	void renderQuad();
};

