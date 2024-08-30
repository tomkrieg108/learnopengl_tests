////OpenGL Bible cha 2 - drawing a point!

#pragma once
#include "layer.h"

class Shader;

namespace sb7 
{
	class PointTest : public Layer
	{
	public:

		PointTest(const std::string& name = "Point Test");

		void Startup();
		void Shutdown();
		void OnUpdate(double now, double time_step) override;

	private:
		std::unique_ptr<Shader> m_shader;
		uint32_t VAO = 0;
	};
}


