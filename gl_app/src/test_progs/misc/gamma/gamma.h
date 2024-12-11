#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
class Window;

class Gamma : public Layer
{
public:

	Gamma(const std::string& name = "Gamme Test");

	void Startup();
	void Shutdown();
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader;
	uint32_t VAO = 0;
};