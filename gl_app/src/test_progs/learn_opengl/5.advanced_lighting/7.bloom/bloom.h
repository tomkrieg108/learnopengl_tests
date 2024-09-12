#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

class Bloom : public Layer
{
public:
	Bloom(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_shader_light = nullptr;
	std::unique_ptr<Shader> m_shader_blur = nullptr;
	std::unique_ptr<Shader> m_shader_bloom_final = nullptr;

	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path, bool gammaCorrection);

	void renderCube();
	void renderQuad();

	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;

	unsigned int quadVAO = 0;
	unsigned int quadVBO = 0;

	unsigned int woodTexture = 0;
	unsigned int containerTexture = 0;

	unsigned int hdrFBO = 0;
	unsigned int rboDepth = 0;
	unsigned int pingpongFBO[2] = { 0,0 };
	unsigned int pingpongColorbuffers[2] = { 0,0 };
	unsigned int colorBuffers[2] = { 0,0 };

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	
	//unsigned int diffuseMap = 0;
	//unsigned int normalMap = 0;

	bool bloom = true;
	float exposure = 1.0f;

	glm::vec3 lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
};

