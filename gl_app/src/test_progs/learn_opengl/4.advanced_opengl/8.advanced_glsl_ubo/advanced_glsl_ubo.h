#pragma once
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

class AdvancedGLSL_UBO : public Layer
{
public:
	AdvancedGLSL_UBO(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> shaderRed = nullptr;
	std::unique_ptr<Shader> shaderGreen = nullptr;
	std::unique_ptr<Shader> shaderBlue = nullptr;
	std::unique_ptr<Shader> shaderYellow = nullptr;

	Camera& m_camera;
	
	unsigned int cubeVAO = 0, cubeVBO = 0;

	unsigned int uboMatrices = 0;
	unsigned int uniformBlockIndexRed = 0;
	unsigned int uniformBlockIndexGreen = 0;
	unsigned int uniformBlockIndexBlue = 0;
	unsigned int uniformBlockIndexYellow = 0;
};

