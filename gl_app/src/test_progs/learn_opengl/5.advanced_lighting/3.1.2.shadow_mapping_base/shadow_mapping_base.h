#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

class ShadowMappingBase : public Layer
{
public:
	ShadowMappingBase(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> simpleDepthShader = nullptr;
	std::unique_ptr<Shader> debugDepthQuad = nullptr;
	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path);

	void renderCube();
	void renderQuad();
	void renderScene(Shader& shader);

	unsigned int cubeVAO = 0, cubeVBO = 0;
	unsigned int planeVAO = 0, planeVBO = 0;
	unsigned int quadVAO = 0, quadVBO = 0;
	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;

	unsigned int cubeTexture = 0;
	unsigned int floorTexture = 0;
	unsigned int woodTexture = 0;

	unsigned int framebuffer = 0;
	unsigned int textureColorbuffer = 0;

	glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

	//NOTE:  size of depth map inpacts frame rate wuite a bit (bigger = slower)!
	//const unsigned int SHADOW_WIDTH = 16000, SHADOW_HEIGHT = 16000;
	//const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	//const unsigned int SHADOW_WIDTH = 256, SHADOW_HEIGHT = 256;
	//const unsigned int SHADOW_WIDTH = 64, SHADOW_HEIGHT = 64;
};

