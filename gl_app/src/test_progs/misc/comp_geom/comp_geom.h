#pragma once
#include <glm/glm.hpp>
#include "camera.h"
#include "layer.h"

class Shader;
//class Camera;
class Window;

namespace geom
{
	class CompGeom : public Layer
	{
	public:
		CompGeom(Window& window, Camera& camera);
		void Startup() override;
		void Shutdown() override;
		void OnUpdate(double now, double time_step) override;
		void ImGuiUpdate() override;

	private:
		Camera& m_camera;
		Window& m_window;
	};
}
