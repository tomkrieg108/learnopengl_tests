#include "pch.h"

#include "camera.h"

Camera::Camera()
{
	Update();
}

Camera::Camera(glm::vec3 position, GLfloat yaw, GLfloat pitch) :
	m_position{ position }, m_yaw{ yaw }, m_pitch{ pitch }
{
	Update();
}

glm::mat4 Camera::ViewMatrix()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
	//return glm::lookAt(m_position, m_position + m_front, m_world_up); //up or world_up both work
}

void Camera::MoveForward(float amount)
{
	m_position += amount * m_front;
	Update();
}

void Camera::MoveRight(float amount)
{
	m_position += amount * m_right;
	Update();
}

void Camera::Turn(float amount_x, float amount_y)
{
	m_yaw += amount_x;
	m_pitch += amount_y;

	//std::cout << "Mouse move yaw pitch:" << amount_x/0.1f << "," << amount_y/0.1f << "\n";

	if (m_pitch > 89.0f)
	{
		m_pitch = 89.0f;
	}
	if (m_pitch < -89.0f)
	{
		m_pitch = -89.0f;
	}
	Update();
}

void Camera::MoveVertically(float amount)
{
	m_position.y += amount;
	Update();
}
void Camera::InvertPitch()
{
	m_pitch = -m_pitch;
	Update();
}

void Camera::Update()
{
	m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front.y = sin(glm::radians(m_pitch));
	m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(m_front);

	m_right = glm::normalize(glm::cross(m_front, this->m_world_up));
	m_up = glm::normalize(glm::cross(m_right, this->m_front));
}

void Camera::SetPosition(glm::vec3& position)
{
	m_position = position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PerspectiveCamera::PerspectiveCamera(GLfloat aspect_ratio) :
	m_aspect_ratio{aspect_ratio},
	Camera::Camera()
{
}

PerspectiveCamera::PerspectiveCamera(GLfloat aspect_ratio, glm::vec3 position, GLfloat yaw, GLfloat pitch) :
	m_aspect_ratio{ aspect_ratio },
	Camera::Camera(position,yaw,pitch)
{
}

glm::mat4 PerspectiveCamera::ProjMatrix()
{
	return glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near, m_far);
}

void PerspectiveCamera::Zoom(float amount)
{
	m_fov += amount;
	if (m_fov > 60.0f)
		m_fov = 60.0f;
	if (m_fov < 15.0f)
		m_fov = 15.0f;
}

void PerspectiveCamera::SetAspectRatio(float width, float height)
{
	m_aspect_ratio = width / height;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

OrthographicCamera::OrthographicCamera() :
	Camera::Camera()
{
}

OrthographicCamera::OrthographicCamera(glm::vec4 extent) :
	m_extent{extent},
	Camera::Camera()
{
}

OrthographicCamera::OrthographicCamera(glm::vec4 extent,glm::vec3 position, GLfloat yaw, GLfloat pitch) :
	m_extent{extent},
	Camera::Camera(position, yaw, pitch)
{
}

glm::mat4 OrthographicCamera::ProjMatrix()
{
	return glm::ortho(m_extent[0], m_extent[1], m_extent[2], m_extent[3], m_near, m_far);
}

void OrthographicCamera::Zoom(float amount)
{
}