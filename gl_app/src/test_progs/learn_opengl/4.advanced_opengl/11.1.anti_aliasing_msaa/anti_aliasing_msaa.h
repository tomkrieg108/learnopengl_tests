#pragma once
#include "camera.h"
#include "layer.h"
#include "lgl_model.h"

class Shader;
//class Camera;

class AntiAliasingMSAA : public Layer
{
public:
	AntiAliasingMSAA(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	Camera& m_camera;
	std::unique_ptr<Shader> m_shader = nullptr;

	unsigned int cubeVAO = 0, cubeVBO = 0;

	lgl::Model* rock = nullptr;
	lgl::Model* planet = nullptr;

	glm::mat4* modelMatrices = nullptr;
	unsigned int amount = 1000; //num rocks
};