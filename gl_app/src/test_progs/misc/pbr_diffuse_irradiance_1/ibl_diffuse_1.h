#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
class Window;

class IblDiffuse : public Layer
{
public:

	IblDiffuse(Window& window, v2::Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;
	void CheckKeys(double delta_time) override;
	void OnEvent(Event& event) override;

private:
	v2::Camera& m_camera;
	Window& m_window;
	std::unique_ptr<Shader> pbrShader = nullptr;
	std::unique_ptr<Shader> equirectangularToCubemapShader = nullptr;
	std::unique_ptr<Shader> irradianceShader = nullptr;
	std::unique_ptr<Shader> backgroundShader = nullptr;

	//render id's
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	unsigned int sphereVAO = 0;
	unsigned int captureFBO = 0;
	unsigned int captureRBO = 0;
	unsigned int envCubemap = 0;
	unsigned int irradianceMap = 0;
	unsigned int hdrTexture = 0;
	unsigned int indexCount = 0;

	glm::mat4 captureViews[6] = { glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),
																glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f) };

	glm::mat4 captureProjection = glm::mat4(1.0f);

	//Try this out instead of the above
	v2::Camera m_capture_camera[6];

	void renderCube();
	void renderSphere();
	unsigned int loadTexture(char const* path);
};