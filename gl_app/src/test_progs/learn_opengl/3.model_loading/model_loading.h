#pragma once

#include "layer.h"
#include "lgl_model.h"

class Shader;
class Camera;

class ModelLoadingTest : public Layer
{
public:
	ModelLoadingTest(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera;

	lgl::Model* ourModel = nullptr;
};