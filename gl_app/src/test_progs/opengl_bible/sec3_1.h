//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage

#pragma once
#include "layer.h"

class Shader;

class LayerSec3_1 : public Layer
{
public:
	LayerSec3_1(const std::string& name = "Layer");

	void Startup();
	void Shutdown();
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader;
	uint32_t VAO = 0;
};