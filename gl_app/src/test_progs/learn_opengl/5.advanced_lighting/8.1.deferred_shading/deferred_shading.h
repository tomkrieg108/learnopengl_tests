#pragma once

#include "layer.h"
#include "lgl_model.h"

class Shader;
class Camera;
class Window;

class DeferredShading : public Layer
{
public:
	DeferredShading(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> shaderGeometryPass = nullptr;
	std::unique_ptr<Shader> shaderLightingPass = nullptr;
	std::unique_ptr<Shader> shaderLightBox = nullptr;

	lgl::Model* ourModel = nullptr;
	unsigned int gBuffer = 0;
	unsigned int gPosition = 0, gNormal = 0, gAlbedoSpec = 0;
	unsigned int rboDepth = 0;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	
	Camera& m_camera;
	Window& m_window;

	void renderCube();
	void renderQuad();

	std::vector<glm::vec3> objectPositions;
};

