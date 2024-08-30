#pragma once

#include "layer.h"

class Shader;
class Camera;
class Window;

class CSM : public Layer
{
public:
	CSM(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	std::unique_ptr<Shader> m_simple_depth_shader = nullptr;
	std::unique_ptr<Shader> m_debug_Depth_quad = nullptr;
	std::unique_ptr<Shader> m_debug_cascade_shader = nullptr;
	
	Camera& m_camera;
	Window& m_window;

	std::vector<float> m_shadow_cascade_levels;
	int m_debug_layer = 0;

	// meshes
	unsigned int m_planeVAO = 0;
	unsigned int m_planeVBO = 0;
	unsigned int m_cubeVAO = 0;
	unsigned int m_cubeVBO = 0;
	unsigned int m_quadVAO = 0;
	unsigned int m_quadVBO = 0;
	unsigned int m_matricesUBO = 0;
	unsigned int m_wood_texture = 0;

	// lighting info
	// -------------
	glm::vec3 m_light_dir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
	unsigned int m_lightFBO;
	unsigned int m_light_depth_maps;
	unsigned int m_depth_map_resolution = 4096;

	bool m_show_quad = false;
	bool m_show_visualizers = false;

	std::random_device m_device;
	std::mt19937 m_generator = std::mt19937(m_device());

	std::vector<glm::mat4> m_light_matrices_cache;

	std::vector<GLuint> m_visualizerVAOs;
	std::vector<GLuint> m_visualizerVBOs;
	std::vector<GLuint> m_visualizerEBOs;


	unsigned int LoadTexture(char const* path);
	
	void RenderScene(Shader* shader);
	void RenderCube();
	void RenderQuad();

	glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
	std::vector<glm::mat4> GetLightSpaceMatrices();

	std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projview);
	std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

	void DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

	void OnKeyPressed(EventKeyPressed& e) override;
};
