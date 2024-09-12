#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;

class LightCasterSpot : public Layer
{
public:
	LightCasterSpot(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_light_cube_shader = nullptr;

	Camera& m_camera;

	uint32_t VBO = 0, cubeVAO = 0;
	uint32_t lightCubeVAO = 0;

	uint32_t diffuseMap = 0, specularMap = 0;

	uint32_t loadTexture(char const* path);

};
