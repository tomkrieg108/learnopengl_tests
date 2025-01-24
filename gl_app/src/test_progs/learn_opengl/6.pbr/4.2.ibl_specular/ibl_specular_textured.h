#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
class Window;

class IblSpecularTextured : public Layer
{
public:

	IblSpecularTextured(Window& window, v2::Camera& camera);
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
	std::unique_ptr<Shader> prefilterShader = nullptr;
	std::unique_ptr<Shader> brdfShader = nullptr;
	std::unique_ptr<Shader> backgroundShader = nullptr;

	//render id's
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;
	unsigned int sphereVAO = 0;
	unsigned int captureFBO = 0;
	unsigned int captureRBO = 0;
	unsigned int envCubemap = 0;
	unsigned int irradianceMap = 0;
	unsigned int prefilterMap = 0;
	unsigned int brdfLUTTexture = 0;
	unsigned int hdrTexture = 0;
	unsigned int indexCount = 0;

	uint32_t ironAlbedoMap = 0;
	uint32_t ironNormalMap = 0;
	uint32_t ironMetallicMap = 0;
	uint32_t ironRoughnessMap = 0;
	uint32_t ironAOMap = 0;

	uint32_t graniteAlbedoMap = 0;
	uint32_t graniteNormalMap = 0;
	uint32_t graniteMetallicMap = 0;
	uint32_t graniteRoughnessMap = 0;
	uint32_t graniteAOMap = 0;

	uint32_t goldAlbedoMap = 0;
	uint32_t goldNormalMap = 0;
	uint32_t goldMetallicMap = 0;
	uint32_t goldRoughnessMap = 0;
	uint32_t goldAOMap = 0;

	uint32_t stoneAlbedoMap = 0;
	uint32_t stoneNormalMap = 0;
	uint32_t stoneMetallicMap = 0;
	uint32_t stoneRoughnessMap = 0;
	uint32_t stoneAOMap = 0;

	uint32_t meadowAlbedoMap = 0;
	uint32_t meadowNormalMap = 0;
	uint32_t meadowMetallicMap = 0;
	uint32_t meadowRoughnessMap = 0;
	uint32_t meadowAOMap = 0;



	glm::mat4 captureViews[6] = { glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),
																glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f) };

	glm::mat4 captureProjection = glm::mat4(1.0f);

	//Try this out instead of the above
	v2::Camera m_capture_camera[6];

	void renderCube();
	void renderQuad();
	void renderSphere();
	unsigned int loadTexture(char const* path);
};