////OpenGL Bible sec 2 - drawing a triangle

#pragma once
#include "layer.h"

class Shader;

class LayerSec2 : public Layer
{
public:

	LayerSec2(const std::string& name = "Layer");

	void Startup();
	void Shutdown();
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader;
	uint32_t VAO = 0;
};
