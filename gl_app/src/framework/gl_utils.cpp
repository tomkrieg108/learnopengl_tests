#include "pch.h"
#include <GL\glew.h>
#include <GLFW\glfw3.h>

//OpenGL parameters - 'Anton's openGL tutorials'
GLenum params[] = { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, GL_MAX_CUBE_MAP_TEXTURE_SIZE, GL_MAX_DRAW_BUFFERS, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_SIZE, GL_MAX_VARYING_FLOATS, GL_MAX_VERTEX_ATTRIBS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_UNIFORM_COMPONENTS, GL_MAX_VIEWPORT_DIMS, GL_STEREO, };

const char* param_name[] = { "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", "GL_MAX_CUBE_MAP_TEXTURE_SIZE", "GL_MAX_DRAW_BUFFERS", "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
"GL_MAX_TEXTURE_IMAGE_UNITS", "GL_MAX_TEXTURE_SIZE", "GL_MAX_VARYING_FLOATS", "GL_MAX_VERTEX_ATTRIBS", "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", "GL_MAX_VERTEX_UNIFORM_COMPONENTS", "GL_MAX_VIEWPORT_DIMS", "GL_STEREO" };

void DisplayGLParams()
{
	std::cout << "Open GL parameters: \n\n";
	for (int i = 0; i <= 10; i++)
	{
		static int param_val;
		glGetIntegerv(params[i], &param_val);
		std::cout << param_name[i] << "\t" << param_val << "\n";
	}
	static GLboolean bool_val;
	glGetBooleanv(GL_STEREO, &bool_val);
	std::cout << param_name[11] << "\t" << bool_val << "\n\n";
	std::cout << "-----------------------------\n\n";
}

void DisplayVideoModes()
{
	//video modes 'Anton's openGL tutorials'
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int count;
	const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
	std::cout << "Video modes supported: " << count << "\n\n";
	for (int i = 0; i < count; i++)
	{
		std::cout << i << "\n";
		std::cout << "Height\t" << modes[i].height << "\n";
		std::cout << "Width\t" << modes[i].width << "\n";
		std::cout << "Red Bits\t" << modes[i].redBits << "\n";
		std::cout << "Blue Bits\t" << modes[i].blueBits << "\n";
		std::cout << "Green Bits\t" << modes[i].greenBits << "\n";
		std::cout << "Refresh Rate\t" << modes[i].refreshRate << "\n\n";
	}
	std::cout << "-----------------------------\n\n";
	std::cout.flush();
}

void DisplayGLVersion()
{
	//This needs to come after glewInit()
	std::cout << glGetString(GL_RENDERER) << "\n"; //AOPG - p20
	std::cout << glGetString(GL_VERSION) << "\n";
	std::cout << "----------------------------------------------------\n\n";
}
