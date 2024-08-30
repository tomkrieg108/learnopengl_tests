//#define GLEW_STATIC 
//Can define here (ahead of #include <GL/glew.h>) or in preprocessor settings
//Need to define this if using the static glew lib glew32s.lib (linker input settings)
#include "pch.h"
#include "window.h"
#include "base.h"
#include "gl_error.h"
#include "events.h"
//#include "camera.h"
//#include "application.h"

//Docs on GLFW:
//https://www.glfw.org/docs/latest

Window::Window() :
	m_width(1200),
	m_height(1000)
{
	Window::Initialise();
}

Window::~Window()
{
	glfwDestroyWindow(m_glfw_window);
	glfwTerminate();
}

int Window::Initialise()
{
	//Initialise GLFW
	if (!glfwInit())
	{
		std::cout << "GLFW initalisation failed\n";
		glfwTerminate();
		return 1;
	}
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
	m_width = video_mode->width - 100;
	m_height = video_mode->height - 100;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLU_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, GLU_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLU_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 4);	//for MSAA multi-sample anti-aliasing

#ifdef DEBUG
	//Allows an error callback function to be used 
	//https://learnopengl.com/In-Practice/Debugging
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	m_glfw_window = glfwCreateWindow(m_width, m_height, "OpenGL App", NULL, NULL);
	if (!m_glfw_window)
	{
		std::cout << "GLFW window creation failed\n";
		glfwTerminate();
		return 1;
	}

	//Get buffer size info (dimensions of win excluding borders etc)
	glfwGetFramebufferSize(m_glfw_window, &m_buffer_width, &m_buffer_height);

	//Set the context for glew to use -tell glfw to use this window (could be multiple windows)
	glfwMakeContextCurrent(m_glfw_window);

	//glewExperimental = GL_TRUE;

	//set callbacks
	glfwSetMouseButtonCallback(m_glfw_window, Window::HandleMouseButton);
	glfwSetScrollCallback(m_glfw_window, Window::HandleMousesScroll);
	glfwSetCursorPosCallback(m_glfw_window, Window::HandleMouseMove);
	glfwSetWindowSizeCallback(m_glfw_window, Window::HandleWindowResize);
	glfwSetKeyCallback(m_glfw_window, Window::HandleKeys);

	//Initialise glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed\n";
		glfwDestroyWindow(m_glfw_window);
		glfwTerminate();
		return 1;
	}

#ifdef DEBUG
	//Setup error callback 
	//This needs to come after glewInit()
	//https://learnopengl.com/In-Practice/Debugging
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
#endif // DEBUG

	//setup OpenGL viewport size (use entire window - use pixels rather than screen coords)
	glViewport(0, 0, m_buffer_width, m_buffer_height);

	//can also call glDepthRange() which sets the z coord in 'viewport space'
	//refer sb7 ch3, section "Viewport Transformation"

	//call glFrontFace() to specify the cinding order (CW or CCW) for front faces.  Default is CCW => front facing

	//Used for setup of event handlers
	glfwSetWindowUserPointer(m_glfw_window, this);

	//glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	return 0;

	
}

void Window::ClearScreeen()
{
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(m_glfw_window);
}

void Window::PollEvents()
{
	glfwPollEvents();
}

int Window::ShouldClose()
{
	return glfwWindowShouldClose(m_glfw_window);
}

GLFWwindow* Window::GlfwWindow()
{
	return m_glfw_window;
}

int Window::BufferWidth()
{
	return m_buffer_width;
}

int Window::BufferHeight()
{
	return m_buffer_height;
}

int Window::Width()
{
	return m_width;
}

int Window::Height()
{
	return m_height;
}

void Window::DisplayParams()
{
	std::cout << "Width: " << m_width << "\n";
	std::cout << "Height: " << m_height << "\n";
	std::cout << "Buf Width: " << m_buffer_width << "\n";
	std::cout << "Buf Height: " << m_buffer_height << "\n";
	std::cout << "-----------------\n\n";
}

void Window::HandleWindowResize(GLFWwindow* window, int width, int height)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	win->m_width = width;
	win->m_height = height;
	glfwGetFramebufferSize(win->GlfwWindow(), &(win->m_buffer_width), &(win->m_buffer_height));
	glViewport(0, 0, win->m_buffer_width, win->m_buffer_height);
	EventWinResize e{ win->m_buffer_width,win->m_buffer_height };
	EventManager::Dispatch(e);
	//win->DisplayParams();
}

void Window::HandleMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	win->m_mouse.MouseMove(xpos, ypos);
}

void Window::HandleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	win->m_mouse.MouseButtonPress(button, action, mods);
}

void Window::HandleMousesScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	win->m_mouse.MouseScroll(xoffset, yoffset);
}

void Window::HandleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	win->m_keys.Action(key, code, action, mode);
}