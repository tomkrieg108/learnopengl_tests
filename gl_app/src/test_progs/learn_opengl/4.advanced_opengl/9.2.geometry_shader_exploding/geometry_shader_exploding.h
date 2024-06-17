#pragma once

#include "layer.h"
#include "lgl_model.h"

class Shader;
class Camera;

class GeometryShaderExploding : public Layer
{
public:
	GeometryShaderExploding(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	Camera& m_camera;
	std::unique_ptr<Shader> m_shader = nullptr;
	unsigned int VAO = 0, VBO = 0;
	lgl::Model* ourModel = nullptr;
};