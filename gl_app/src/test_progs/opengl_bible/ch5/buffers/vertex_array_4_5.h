#pragma once

#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;

//This comes from "light_map_specular" in 2.lighting of learn openGL.
//using OpenGL 4.5 functions to setup VBO and VAO

namespace sb7
{
	class LightMapSpecular : public Layer
	{
	public:
		LightMapSpecular(Camera& camera);
		void Startup() override;
		void Shutdown() override;
		void OnUpdate(double now, double time_step) override;
		void ImGuiUpdate() override;

	private:
		std::unique_ptr<Shader> m_shader = nullptr;
		std::unique_ptr<Shader> m_light_cube_shader = nullptr;
		Camera& m_camera;

		unsigned int VBO = 0, cubeVAO = 0;
		unsigned int lightCubeVAO = 0;

		unsigned int diffuseMap = 0, specularMap = 0;

		unsigned int loadTexture(char const* path);

		glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
		glm::vec3 BoxPos = glm::vec3(-2.0f, 2.0f, 3.0f);

		glm::vec3 light_ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		glm::vec3 light_diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);

		float shininess = 64.0f;
	};
}