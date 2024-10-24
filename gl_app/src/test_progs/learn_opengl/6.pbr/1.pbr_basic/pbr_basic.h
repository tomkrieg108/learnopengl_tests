#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
class Window;

class PbrBasic : public Layer
{
public:

	PbrBasic(Window& window, v2::Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;
	void CheckKeys(double delta_time) override;
	void OnEvent(Event& event) override;

private:
	v2::Camera& m_camera; 
	Window& m_window;
	std::unique_ptr<Shader> m_shader = nullptr;

	unsigned int sphereVAO = 0;
	unsigned int indexCount = 0;

	void renderSphere();
	unsigned int loadTexture(char const* path);
};