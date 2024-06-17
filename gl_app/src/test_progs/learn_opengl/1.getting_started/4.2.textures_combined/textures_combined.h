#pragma once

#include "layer.h"

class Shader;

class TexturesCombinedTest : public Layer
{
public:
	TexturesCombinedTest();
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;

	unsigned int VBO = 0, VAO = 0, EBO = 0;
	unsigned int texture1 = 0, texture2 = 0;
};
