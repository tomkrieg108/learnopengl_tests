#pragma once
#include "camera.h"
#include "layer.h"


class Shader;
//class Camera;
class Window;

class TextRendering : public Layer
{
public:
	TextRendering(Window& window, Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		unsigned int TextureID; // ID handle of the glyph texture
		glm::ivec2   Size;      // Size of glyph
		glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
		unsigned int Advance;   // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;
	unsigned int VAO = 0, VBO = 0;
	std::unique_ptr<Shader> m_shader = nullptr;
	
	Camera& m_camera;
	Window& m_window;

	void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
};
