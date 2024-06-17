#pragma once

#include "layer.h"

class Shader;
class Camera;

class BlendingDiscardTest : public Layer
{
public:
	BlendingDiscardTest(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera;

	unsigned int loadTexture(char const* path);

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int planeVAO = 0, planeVBO = 0;
	unsigned int transparentVAO = 0, transparentVBO = 0;

	unsigned int cubeTexture = 0;
	unsigned int floorTexture = 0;
	unsigned int transparentTexture = 0;
};
