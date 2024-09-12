////OpenGL Bible cha 2 - drawing a point!

#pragma once
#include "layer.h"

class Shader;

namespace sb7 
{
	class TriangleTest : public Layer
	{
	public:

		TriangleTest(const std::string& name = "Triangle Test");

		void Startup();
		void Shutdown();
		void OnUpdate(double now, double time_step) override;

	private:
		std::unique_ptr<Shader> m_shader;
		uint32_t VAO = 0;
	};
}


