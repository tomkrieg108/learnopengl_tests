#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

namespace me
{

	class ShadowMappingVisualised : public Layer
	{
	public:
		ShadowMappingVisualised(Window& window, v2::Camera& camera);
		void Startup() override;
		void Shutdown() override;
		void OnUpdate(double now, double time_step) override;
		void ImGuiUpdate() override;

	private:
		std::unique_ptr<Shader> m_shader = nullptr;
		std::unique_ptr<Shader> simpleDepthShader = nullptr;
		std::unique_ptr<Shader> debugDepthQuad = nullptr;
		std::unique_ptr<Shader> m_light_cube_shader = nullptr;
		std::unique_ptr<Shader> m_coords_shader = nullptr;
		std::unique_ptr<Shader> m_frustum_shader = nullptr;
		
		v2::Camera& m_camera; //normal viewing
		v2::Camera* m_camera_vis = nullptr; //for visualisation 
		v2::Camera* m_camera_light = nullptr; //camera aligned with the directional light
		
		Window& m_window;

		unsigned int loadTexture(char const* path);
		void renderCube();
		void renderLamp(v2::Camera* camera);
		void renderCamera(v2::Camera* viewing_camera, v2::Camera* rendered_camera);
		void RenderCoords(v2::Camera* camera, glm::mat4& model);
		void renderQuad();
		void renderScene(Shader& shader);

		std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		void RenderFrustum(v2::Camera* viewing_camera, v2::Camera* rendered_camera, glm::vec4 color);

		//Lots of render ID's
		uint32_t cubeVAO = 0,  cubeVBO = 0;
		uint32_t planeVAO = 0, planeVBO = 0;
		uint32_t quadVAO = 0, quadVBO = 0;
		uint32_t coordsVAO = 0, coordsVBO = 0;
		uint32_t frustumVAO = 0, frustumVBO = 0, frustumEBO = 0;
		uint32_t depthMapFBO = 0;
		uint32_t depthMap = 0;
		uint32_t m_camera_vis_fbo = 0;
		uint32_t m_camera_vis_texture = 0;
		uint32_t cubeTexture = 0;
		uint32_t floorTexture = 0;
		uint32_t woodTexture = 0;

		struct VertexCoords
		{
			std::array<float, 3> pos;
			std::array<float, 4> col;
		};
		std::vector<VertexCoords> m_coord_verts;

	
		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	};
}
