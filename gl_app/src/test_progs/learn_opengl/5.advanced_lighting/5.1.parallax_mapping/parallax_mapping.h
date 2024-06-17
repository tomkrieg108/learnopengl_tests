#pragma once

#include "layer.h"

class Shader;
class Camera;
class Window;

class ParallaxMapping : public Layer
{
public:
	ParallaxMapping(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;

	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path);
	void renderQuad();

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	unsigned int diffuseMap = 0;
	unsigned int normalMap = 0;
	unsigned int heightMap = 0;

	glm::vec3 lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
};

