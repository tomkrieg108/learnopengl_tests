#pragma once

#include "layer.h"
#include "lgl_model.h"

class Shader;
class Camera;

class AsteroidsInstanced : public Layer
{
public:
	AsteroidsInstanced(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	Camera& m_camera;
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_planet_shader = nullptr;

	//unsigned int instanceVBO = 0;
	//unsigned int quadVAO = 0, quadVBO = 0;

	unsigned int buffer;

	lgl::Model* rock = nullptr;
	lgl::Model* planet = nullptr;

	glm::mat4* modelMatrices = nullptr;
	unsigned int amount = 100000; //num rocks
};
