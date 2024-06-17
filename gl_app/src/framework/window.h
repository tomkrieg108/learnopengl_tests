#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <assimp/Importer.hpp>
#include "input.h"

struct GLFWwindow;

class Window
{
public:
	
	Window();
	~Window();
	
	void SwapBuffers();
	void ClearScreeen();
	void PollEvents();
	int ShouldClose();

	int Height();
	int Width();
	int BufferHeight();
	int BufferWidth();

	void DisplayParams();

	GLFWwindow* GlfwWindow();
	Keys& GetKeys() { return m_keys; }
	void ShutDown() { glfwSetWindowShouldClose(m_glfw_window, GL_TRUE); }

private:
	int Initialise();
	
	GLFWwindow* m_glfw_window = nullptr;
	//window dim
	int m_width, m_height;
	int m_buffer_width, m_buffer_height;

	Mouse m_mouse;
	Keys m_keys;

	//Callbacks
	static void HandleWindowResize(GLFWwindow* glfw_window, int width, int height);
	static void HandleMouseMove(GLFWwindow* glfw_window, double xpos, double ypos);
	static void HandleMouseButton(GLFWwindow* glfw_window, int button, int action, int mods);
	static void HandleMousesScroll(GLFWwindow* glfw_window, double xoffset, double yoffset);
	static void HandleKeys(GLFWwindow* glfw_window, int key, int code, int action, int mode);
};

