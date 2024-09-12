#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

/*
This is the same as ShadowMappingVisualised in shadow_mapping.h, cpp, but using the original camera class
 not v2::Camera
*/


namespace me
{

	class ShadowMappingVisualised2 : public Layer
	{
	public:
		ShadowMappingVisualised2(Window& window, Camera& camera);
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

		Camera& m_camera; //normal viewing
		PerspectiveCamera* m_camera_vis = nullptr; //for visualisation
		OrthographicCamera* m_camera_light = nullptr; //camera aligned with the directional light
		//Camera::OrthoParams m_light_ortho_params{ 1.0f,7.5f,-10.0f,10.0f,-10.0f,10.0f,true};

		Window& m_window;

		unsigned int loadTexture(char const* path);
		void renderCube();
		void renderLamp(Camera* camera);
		void RenderCoords(Camera* camera, glm::mat4& model);
		void renderQuad();
		void renderScene(Shader& shader);

		unsigned int cubeVAO = 0,  cubeVBO = 0;
		unsigned int planeVAO = 0, planeVBO = 0;
		unsigned int quadVAO = 0, quadVBO = 0;
		unsigned int coordsVAO = 0, coordsVBO = 0;
		unsigned int depthMapFBO = 0;
		unsigned int depthMap = 0;

		struct VertexCoords
		{
			std::array<float, 3> pos;
			std::array<float, 4> col;
		};
		std::vector<VertexCoords> m_coord_verts;

		uint32_t m_camera_vis_fbo = 0;
		uint32_t m_camera_vis_texture = 0;

		unsigned int cubeTexture = 0;
		unsigned int floorTexture = 0;
		unsigned int woodTexture = 0;

		unsigned int framebuffer = 0;
		unsigned int textureColorbuffer = 0;

		glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	};
}
