//https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
#pragma once
#include "layer.h"

class Shader;
class Camera;
class Window;

class Tesselation : public Layer
{
public:
	Tesselation(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera;
	Window& m_window;

	const unsigned int NUM_PATCH_PTS = 4;
	unsigned int rez = 20;
	unsigned int terrainVAO, terrainVBO;
}; 