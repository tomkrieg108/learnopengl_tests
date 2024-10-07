#pragma once

#include "camera.h"
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

namespace me
{

	class CSMVisualised : public Layer
	{
	public:
		CSMVisualised(Window& window, v2::Camera& camera);
		void Startup() override;
		void Shutdown() override;
		void OnUpdate(double now, double time_step) override;
		void ImGuiUpdate() override;
		void CheckKeys(double delta_time) override;
		void OnEvent(Event& event) override;

	private:

		uint32_t LoadTexture(char const* path);

		void RenderScene(Shader* shader);
		void RenderCube();
		void RenderQuad();
		void RenderLamp(v2::Camera* camera);
		void RenderCamera(v2::Camera* viewing_camera, v2::Camera* rendered_camera);
		void RenderCoords(v2::Camera* camera, glm::mat4& model);
		glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane);
		std::vector<glm::mat4> GetLightSpaceMatrices();
		std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& projview);
		std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		void DrawCascadeVolumeVisualizers(const std::vector<glm::mat4>& lightMatrices, Shader* shader);

		void RenderFrustum(v2::Camera* viewing_camera, const glm::mat4& projview, glm::vec4 color);
		void RenderAllFrustrums(v2::Camera* viewing_camera, v2::Camera* rendered_camera);
		
		std::unique_ptr<Shader> m_shader = nullptr;
		std::unique_ptr<Shader> m_simple_depth_shader = nullptr;
		std::unique_ptr<Shader> m_debug_Depth_quad = nullptr;
		std::unique_ptr<Shader> m_debug_cascade_shader = nullptr;
		std::unique_ptr<Shader> m_coords_shader = nullptr;
		std::unique_ptr<Shader> m_frustum_shader = nullptr;
		std::unique_ptr<Shader> m_light_cube_shader = nullptr;
		
		v2::Camera& m_camera; //normal viewing
		v2::Camera* m_camera_vis = nullptr; //for visualisation 
		v2::Camera* m_camera_light = nullptr; //camera aligned with the directional light
		v2::Camera* m_controlled_camera = nullptr;

		Window& m_window;

		std::vector<float> m_shadow_cascade_levels;
		int m_debug_layer = 0;

		// OGL render ID's
		uint32_t m_planeVAO = 0;
		uint32_t m_planeVBO = 0;
		uint32_t m_cubeVAO = 0;
		uint32_t m_cubeVBO = 0;
		uint32_t m_quadVAO = 0;
		uint32_t m_quadVBO = 0;
		uint32_t m_matricesUBO = 0;
		uint32_t m_wood_texture = 0;
		uint32_t m_camera_vis_fbo = 0;
		uint32_t m_camera_vis_texture = 0;
		uint32_t m_coordsVAO = 0, m_coordsVBO = 0;
		uint32_t frustumVAO = 0, frustumVBO = 0, frustumEBO = 0;

		// lighting info
		// -------------
		glm::vec3 m_light_pos = glm::vec3(-2.0f, 4.0f, -1.0f);
		glm::vec3 m_light_dir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
		uint32_t m_lightFBO;
		uint32_t m_light_depth_maps;
		uint32_t m_depth_map_resolution = 4096;

		bool m_show_quad = false;
		bool m_show_visualizers = false;

		std::random_device m_device;
		std::mt19937 m_generator = std::mt19937(m_device());

		std::vector<glm::mat4> m_light_matrices_cache;

		std::vector<GLuint> m_visualizerVAOs;
		std::vector<GLuint> m_visualizerVBOs;
		std::vector<GLuint> m_visualizerEBOs;

		struct VertexCoords
		{
			std::array<float, 3> pos;
			std::array<float, 4> col;
		};
		std::vector<VertexCoords> m_coord_verts;
	};

}
