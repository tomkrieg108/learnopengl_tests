#pragma once

#include "camera.h"
#include "layer.h"
#include "lgl_mesh.h"
#include "lgl_model.h"


class Shader;
class Window;
class Entity;

class FrustumCulling : public Layer
{
public:
	FrustumCulling(Window& window, v2::Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;
	void CheckKeys(double delta_time) override;
	void OnEvent(Event& event) override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	lgl::Model* m_model = nullptr;
	Entity* m_entity = nullptr;

	v2::Camera& m_camera;
	v2::Camera* m_camera_vis = nullptr; //for visualisation 
	v2::Camera* m_controlled_camera = nullptr;
	Window& m_window;
};
