#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

class FrameBufferTest : public Layer
{
public:
	FrameBufferTest(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_screen_shader = nullptr;
	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path);

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int planeVAO = 0, planeVBO = 0;
	unsigned int quadVAO = 0, quadVBO = 0;

	unsigned int cubeTexture = 0;
	unsigned int floorTexture = 0;

	unsigned int framebuffer = 0;
	unsigned int textureColorbuffer = 0;
};
