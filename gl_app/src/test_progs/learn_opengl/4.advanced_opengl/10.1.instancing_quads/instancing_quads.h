#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;

class InstancingQuads : public Layer
{
public:
	InstancingQuads(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	Camera& m_camera;
	std::unique_ptr<Shader> m_shader = nullptr;
	
	unsigned int instanceVBO = 0;
	unsigned int quadVAO = 0, quadVBO = 0;
};
