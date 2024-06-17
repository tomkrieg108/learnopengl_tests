#pragma once

#include "layer.h"

class Shader;
class Camera;

class CoordSys : public Layer
{
public:
	CoordSys(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:

	struct Vertex
	{
		std::array<float, 3> pos;
		std::array<float, 4> col;
	};

	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera;
	unsigned int va = 0, vb = 0;

	std::vector<Vertex> m_verticies;
};
