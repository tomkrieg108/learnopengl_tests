#include "pch.h"
#include <GLFW\glfw3.h>
#include "events.h"
#include "input.h"


Keys::Keys()
{
	for (int i = 0; i < 1024; i++)
	{
		m_key_code[i] = false;
	}
}

void Keys::Action(int key, int code, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			m_key_code[key] = true;
			EventKeyPressed e{ key };
			EventManager::Dispatch(e);
		}
		else if (action == GLFW_RELEASE)
		{
			m_key_code[key] = false;
			EventKeyReleased e{ key };
			EventManager::Dispatch(e);
		}
		else if (action == GLFW_REPEAT)
		{

		}
	}
}

void Mouse::MouseMove(double xpos, double ypos)
{
	if (!m_mouse_first_moved)
	{
		m_mouse_first_moved = true;
		m_last_x = (float)xpos;
		m_last_y = (float)ypos;
	}

	m_delta_x = (float)xpos - m_last_x;
	m_delta_y = m_last_y - (float)ypos;

	m_last_x = (float)xpos;
	m_last_y = (float)ypos;

	EventMouseMove e{ m_last_x, m_last_y, m_delta_x, m_delta_y };
	EventManager::Dispatch(e);
}

void Mouse::MouseButtonPress(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		EventMouseLDown e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		EventMouseLUp e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		EventMouseRDown e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		EventMouseRUp e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		EventMouseMDown e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		EventMouseMUp e{ m_last_x, m_last_y };
		EventManager::Dispatch(e);
	}
}

void Mouse::MouseScroll(double xoffset, double yoffset)
{
	EventMouseScroll e{ float(xoffset), float(yoffset) };
	EventManager::Dispatch(e);
}

auto Mouse::x_pos()
{
	return m_last_x;
}

auto Mouse::y_pos()
{
	return m_last_y;
}

auto Mouse::delta_x()
{
	auto delta = m_delta_x;
	m_delta_x = 0.0f;
	return delta;
}

auto Mouse::delta_y()
{
	auto delta = this->m_delta_y;
	m_delta_y = 0.0f;
	return delta;
}

