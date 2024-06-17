//OpenGL Bible sec 3 - tesselation

#pragma once

#include "layer.h"

class Shader;

class LayerSec3_2 : public Layer
{
public:
	LayerSec3_2(const std::string& name = "Layer");

	void Startup();
	void Shutdown();
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader;
	uint32_t VAO = 0;
};
