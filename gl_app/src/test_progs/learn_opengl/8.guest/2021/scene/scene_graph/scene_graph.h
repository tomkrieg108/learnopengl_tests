#pragma once

#include "layer.h"

class Shader;
class Camera;
class Window;

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
	

	Camera& m_camera;
	Window& m_window;

	// meshes
	unsigned int m_planeVAO = 0;
	unsigned int m_planeVBO = 0;
	unsigned int m_cubeVAO = 0;
	unsigned int m_cubeVBO = 0;
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;
	unsigned int m_matricesUBO = 0;
	unsigned int m_wood_texture = 0;

	
	unsigned int LoadTexture(char const* path);

	void RenderScene(Shader* shader);
	void RenderCube();
	void RenderQuad();

	void OnKeyPressed(EventKeyPressed& e) override;
};
