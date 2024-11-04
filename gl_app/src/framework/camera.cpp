#include "pch.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>

inline namespace v1
{
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
		glm::mat4 view = glm::lookAt(m_position, m_position + m_front, m_world_up);
		glm::mat4 model = glm::inverse(view);
		//Note: m_front = -z 
		return glm::lookAt(m_position, m_position + m_front, m_world_up); //up or world_up both work
	}

	glm::mat4 Camera::ModelMatrix()
	{
		//glm::mat4 view_mat = glm::lookAt(m_position, m_position + m_front, m_world_up);
		//glm::mat4 = glm::mat4({ m_front, m_right, m_up, m_position });
		//return glm::inverse(view_mat);

		glm::mat4 model = glm::mat4(glm::vec4(m_right, 0), glm::vec4(m_up, 0),
			glm::vec4(m_front, 0), glm::vec4(m_position, 1));

		return model;
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

	void Camera::LookAt(glm::vec3& look_pos)
	{
		m_front = glm::normalize(look_pos - m_position);
		m_right = glm::normalize(glm::cross(m_front, this->m_world_up));
		m_up = glm::normalize(glm::cross(m_right, this->m_front));
		//todo - yaw and pitch not updated - see elysiun camera_controller on how to do it
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

		/*
			Note: m_front = -z,
			This is why right is set to 'front X world_up' and not 'world_up X front' 
		*/
	}

	void Camera::SetPosition(glm::vec3& position)
	{
		m_position = position;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	PerspectiveCamera::PerspectiveCamera(GLfloat aspect_ratio) :
		m_aspect_ratio{ aspect_ratio },
		Camera::Camera()
	{
	}

	PerspectiveCamera::PerspectiveCamera(GLfloat aspect_ratio, glm::vec3 position, GLfloat yaw, GLfloat pitch) :
		m_aspect_ratio{ aspect_ratio },
		Camera::Camera(position, yaw, pitch)
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
		m_extent{ extent },
		Camera::Camera()
	{
	}

	OrthographicCamera::OrthographicCamera(glm::vec4 extent, glm::vec3 position, GLfloat yaw, GLfloat pitch) :
		m_extent{ extent },
		Camera::Camera(position, yaw, pitch)
	{
	}

	void OrthographicCamera::SetViewVolume(glm::vec4 extent, float near, float far)
	{
		m_extent = extent;
		m_near = near;
		m_far = far;
	}

	glm::mat4 OrthographicCamera::ProjMatrix()
	{
		return glm::ortho(m_extent[0], m_extent[1], m_extent[2], m_extent[3], m_near, m_far);
	}


	void OrthographicCamera::Zoom(float amount)
	{
	}

}

namespace v2
{
	Camera::Camera()
	{
	}

	glm::mat4 Camera::GetProjMatrix() const
	{
		if (m_projection_type == ProjectionType::Perspective)
		{
			return glm::perspective(glm::radians(m_perspective_params.fov), m_perspective_params.aspect_ratio,
				m_perspective_params.z_near, m_perspective_params.z_far);
		}
		else
		{
			return glm::ortho(m_ortho_params.left, m_ortho_params.right, m_ortho_params.bottom, m_ortho_params.top,
				m_ortho_params.z_near, m_ortho_params.z_far);
		}
	}

	glm::mat4 Camera::GetInverseProjMatrix() const
	{
		return glm::inverse(GetProjMatrix());
	}

	/*
	TODO
	Calculate faster by splitting the matrix into product of rotation component and position component.
	Inverse of rot component is it's transpose.  Inverse of pos component will be negated vals (in col 4)
	*/
	glm::mat4 Camera::GetViewMatrix() const
	{
		return glm::inverse(m_transform);
	}

	void Camera::SetAspectRatio(float width, float height)
	{
		//TODO assert height not zero
		m_perspective_params.aspect_ratio = width / height;
		m_perspective_params.modified = true;
	}

	void Camera::Zoom(float amount)
	{
		if (m_projection_type == ProjectionType::Perspective)
		{
			float new_fov = m_perspective_params.fov + amount;
			new_fov > 75.0f ? m_perspective_params.fov = 75.0f : m_perspective_params.fov = new_fov;
			new_fov < 1.0f ? m_perspective_params.fov = 1.0f : m_perspective_params.fov = new_fov;
		}
	}

	void Camera::SetPerspectiveParams(const PerspectiveParams& perspective_params)
	{
		m_perspective_params = perspective_params;
		m_perspective_params.modified = true;
	}

	void Camera::SetOrthoParams(const OrthoParams& ortho_params)
	{
		m_ortho_params = ortho_params;
		m_ortho_params.modified = true;
	}

	// Controller stuff

	void Camera::SetPosition(glm::vec3& position)
	{
		m_transform[3] = glm::vec4{ position, 1.0f };
	}

	glm::vec3 Camera::GetPosition()
	{
		glm::vec3 pos = (glm::vec3)m_transform[3];
		return pos;
	}
	glm::vec3 Camera::Position()
	{
		return GetPosition();
	}

	glm::vec3 Camera::Front()
	{
		glm::vec3 z = (glm::vec3)m_transform[2];
		return -z;  //camera looks in -ve z dir
	}

	glm::vec3 Camera::Right()
	{
		return (glm::vec3)m_transform[0]; //local x
	}

	glm::vec3 Camera::Up()
	{
		return (glm::vec3)m_transform[1]; //local y
	}
	

	void Camera::LookAt(glm::vec3& look_pos)
	{
		glm::vec3 pos = (glm::vec3)m_transform[3];
		glm::vec3 z = -glm::normalize(look_pos - pos); // negative front
		glm::vec3 x = glm::normalize(glm::cross(glm::vec3(0,1,0),z));
		glm::vec3 y = glm::normalize(glm::cross(z, x));

		m_transform[0] = glm::vec4{ x,0.0f };
		m_transform[1] = glm::vec4{ y,0.0f };
		m_transform[2] = glm::vec4{ z,0.0f };
	}

	void Camera::MoveForward(float amount)
	{
		m_transform = glm::translate(m_transform, glm::vec3(0, 0, amount));
	}

	void Camera::MoveRight(float amount)
	{
		m_transform = glm::translate(m_transform, glm::vec3(amount, 0, 0));
	}

	void Camera::MoveVertically(float amount)
	{
		glm::vec3 up = glm::vec3{ glm::inverse(m_transform) * glm::vec4{0,1,0,0} }; //global up in camera space
		m_transform = glm::translate(m_transform, amount * up);
	}

	void Camera::Turn(float delta_yaw, float delta_pitch)
	{
		glm::vec3 camera_front = glm::vec3{ m_transform[2] };
		glm::vec3 world_up = glm::vec3(0, 1, 0);

		float pitch_angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		pitch_angle = glm::degrees(pitch_angle);
		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp

		glm::mat4 rot_y = GetRotationMatY(-delta_yaw);
		m_transform = rot_y * m_transform;  //rotate about Y world axis

		if (pitch_angle > 175 && delta_pitch > 0)
			return;
		if (pitch_angle < 5 && delta_pitch < 0)
			return;

		glm::mat4 rot_x = GetRotationMatX(delta_pitch);
		m_transform = m_transform * rot_x;  //rotate about X local axis (lool up & down)
	}

	void Camera::RotateLocal(float delta_yaw, float delta_pitch)
	{
		//glm::vec3 camera_front = camera.GetFront(camera_transform);
		glm::vec3 camera_front = glm::vec3{ m_transform[2] };
		glm::vec3 world_up = glm::vec3{ 0, 1, 0 };
		glm::vec4 world_up_4 = glm::vec4{ 0, 1, 0, 0 };

		glm::vec3 rot_axis = glm::vec3{ glm::inverse(m_transform) * world_up_4 }; //global Y Axis in camera space
		m_transform = glm::rotate(m_transform, -delta_yaw, rot_axis);

		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp
		float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		angle = glm::degrees(angle);
		if (angle > 175 && delta_pitch > 0) return;
		if (angle < 5 && delta_pitch < 0) return;

		glm::mat4 rot_x = GetRotationMatX(delta_pitch);
		m_transform = m_transform * rot_x;  // X Axis in camera space
	}

	void Camera::RotateWorld(float amount_x, float amount_y)
	{
		glm::vec3 camera_front = glm::vec3{ m_transform[2] };
		glm::vec3 world_up = glm::vec3(0, 1, 0);

		float angle = glm::orientedAngle(camera_front, world_up, glm::cross(camera_front, world_up));
		angle = glm::degrees(angle);
		//angle reduces as you look down, positive z (coming out of screen) goes up.  Increases as look up, pos z goes down.  Probably the opposite if reverse order the of vectors in cross(), but haven't tried
		//note that both mouse up and right give pos values - see input.cpp

		glm::mat4 rot_y = GetRotationMatY(-amount_x);
		m_transform = rot_y * m_transform;  //orbit about Y global axis

		if (angle > 175 && amount_y > 0)
			return;
		if (angle < 5 && amount_y < 0)
			return;

		//glm::mat4 rot_x = GetRotationMatX(amount_y);
		//camera_transform = camera_transform * rot_x;  //rotate about X local axis
	}

	glm::mat4 Camera::GetRotationMatX(float angle) const
	{
		float angle_rads = glm::radians(angle);
		float c = std::cos(angle_rads);
		float s = std::sin(angle_rads);

		glm::mat4 m;
		m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
		m[0][1] = 0; m[1][1] = c; m[2][1] = -s; m[3][1] = 0;
		m[0][2] = 0; m[1][2] = s; m[2][2] = c; m[3][2] = 0;
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		return m;
	}

	glm::mat4 Camera::GetRotationMatY(float angle) const
	{
		float angle_rads = glm::radians(angle);
		float c = std::cos(angle_rads);
		float s = std::sin(angle_rads);

		glm::mat4 m;
		m[0][0] = c; m[1][0] = 0; m[2][0] = s; m[3][0] = 0;
		m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
		m[0][2] = -s; m[1][2] = 0; m[2][2] = c; m[3][2] = 0;
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		return m;
	}

	glm::mat4 Camera::GetRotationMatZ(float angle) const
	{
		float angle_rads = glm::radians(angle);
		float c = std::cos(angle_rads);
		float s = std::sin(angle_rads);

		glm::mat4 m;
		m[0][0] = c; m[1][0] = -s; m[2][0] = 0; m[3][0] = 0;
		m[0][1] = s; m[1][1] = c; m[2][1] = 0; m[3][1] = 0;
		m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
		m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
		return m;
	}

}