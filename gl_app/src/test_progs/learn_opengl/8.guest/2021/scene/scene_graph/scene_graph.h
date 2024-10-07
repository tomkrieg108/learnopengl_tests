#pragma once

#include "camera.h"
#include "layer.h"
#include "lgl_mesh.h"
#include "lgl_model.h"


class Shader;
//class Camera;
class Window;
class Entity;

class SceneGraph : public Layer
{
public:
	SceneGraph(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	lgl::Model* m_model = nullptr;
	Entity* m_entity = nullptr;
	
	Camera& m_camera;
	Window& m_window;
};
