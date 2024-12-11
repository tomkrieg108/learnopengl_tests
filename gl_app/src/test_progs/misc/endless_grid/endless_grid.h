#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
class Window;
//class Camera;

class EndlessGrid : public Layer
{
public:
	EndlessGrid(Window& window, v2::Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;
	void CheckKeys(double delta_time) override;
	void OnEvent(Event& event) override;

private:

	struct Vertex
	{
		std::array<float, 3> pos;
		std::array<float, 4> col;
	};

	Window& m_window;
	v2::Camera& m_camera;
	std::unique_ptr<Shader> m_shader = nullptr;
	uint32_t VAO = 0, VBO = 0;
	std::vector<Vertex> m_verticies;
};