#pragma once

#include "layer.h"

class Shader;
class Camera;

class CubeMapSkyBox : public Layer
{
public:
	CubeMapSkyBox(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_skybox_shader = nullptr;
	Camera& m_camera;

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int skyboxVAO = 0, skyboxVBO = 0;
	
	unsigned int cubeTexture = 0; 
	unsigned int cubemapTexture = 0;

	unsigned int loadTexture(char const* path);
	unsigned int loadCubemap(std::vector<std::string> faces);
};