#pragma once

#include "layer.h"

class Shader;
class Camera;
class Window;

class PointShadows : public Layer
{
public:
	PointShadows(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> simpleDepthShader = nullptr;
	std::unique_ptr<Shader> debugDepthQuad = nullptr;
	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path);

	void renderCube();
	void renderQuad();
	void renderScene(Shader& shader);

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int quadVAO = 0, quadVBO = 0;
	unsigned int depthMapFBO = 0;
	unsigned int depthCubemap = 0;
	bool shadows = true;

	unsigned int woodTexture = 0;

	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
};

