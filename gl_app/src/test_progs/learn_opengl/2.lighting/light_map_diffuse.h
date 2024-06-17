#pragma once

#include "layer.h"

class Shader;
class Camera;

class LightMapDiffuse : public Layer
{
public:
	LightMapDiffuse(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_light_cube_shader = nullptr;
	Camera& m_camera;

	unsigned int VBO = 0, cubeVAO = 0;
	unsigned int lightCubeVAO = 0;

	unsigned int diffuseMap = 0;

	unsigned int loadTexture(char const* path);
};
