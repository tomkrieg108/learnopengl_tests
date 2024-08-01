#include "layer.h"
#include "model_animation.h"
#include "animator.h"

class Shader;
class Camera;
class Window;

//Screen-space ambient occlusion
class SkeletalAnimation : public Layer
{
public:
	SkeletalAnimation(Window& window, Camera& camera);
	virtual ~SkeletalAnimation() override;

	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;
	void ImGuiUpdate() override;

private:
	std::unique_ptr<Shader> ourShader = nullptr;
	//lgl::Model* backpack = nullptr;

	Model *ourModel;
	Animation* danceAnimation;
	Animator* animator;

	Camera& m_camera;
	Window& m_window;

	// lighting info
	// -------------
	glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
	glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

	//unsigned int loadTexture(char const* path);
	void renderCube();
	void renderQuad();
};
