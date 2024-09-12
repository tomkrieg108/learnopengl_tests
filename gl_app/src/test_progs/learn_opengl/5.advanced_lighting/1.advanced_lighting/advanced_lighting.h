#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

class AdvancedLighting : public Layer
{
public:
	AdvancedLighting(Camera& camera, Window& window);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera;
	Window& m_window;

	unsigned int loadTexture(char const* path);

	unsigned int planeVAO = 0, planeVBO = 0; //used
	unsigned int floorTexture = 0; //used
	

	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
	bool blinn = false;
	bool blinnKeyPressed = false;
};

