#pragma once

#include "layer.h"

class Shader;

class GeometryShaderHouses : public Layer
{
public:
	GeometryShaderHouses();
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	unsigned int VAO = 0, VBO = 0;
};

