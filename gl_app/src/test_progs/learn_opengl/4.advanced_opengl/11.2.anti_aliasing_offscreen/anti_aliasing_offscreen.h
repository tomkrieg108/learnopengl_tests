#pragma once
#include "camera.h"
#include "layer.h"
#include "lgl_model.h"

class Shader;
//class Camera;
class Window;

class AntiAliasingOffscreen : public Layer
{
public:
	AntiAliasingOffscreen(Camera& camera, Window& window);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	Camera& m_camera;
	Window& m_window;
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_screen_shader = nullptr;

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int quadVAO = 0, quadVBO = 0;

	unsigned int framebuffer = 0;
	unsigned int rbo = 0;
	unsigned int intermediateFBO = 0;
	unsigned int screenTexture = 0;

	lgl::Model* rock = nullptr;
	lgl::Model* planet = nullptr;

	glm::mat4* modelMatrices = nullptr;
	unsigned int amount = 1000; //num rocks
};