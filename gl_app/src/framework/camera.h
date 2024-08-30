#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera();
	Camera(glm::vec3 position, GLfloat yaw, GLfloat pitch);
	virtual ~Camera() = default;

	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveVertically(float amount);
	void Turn(float amount_x, float amount_y);
	void InvertPitch();

	virtual void Zoom(float amount) = 0;

	glm::mat4 ViewMatrix();
	virtual glm::mat4 ProjMatrix() = 0;

	void SetPosition(glm::vec3& position);

	glm::vec3& Position() { return m_position; }
	glm::vec3& Front() { return m_front; }
	glm::vec3& Right() { return m_right; }
	glm::vec3& Up() { return m_up; }
	GLfloat Yaw() { return m_yaw; }
	GLfloat Pitch() { return m_pitch; }
	GLfloat ZNear() { return m_near; }
	GLfloat ZFar() { return m_far; }

protected:

	//glm::vec3 m_position = glm::vec3(3.0f, 3.0f, 3.0f);
	glm::vec3 m_position = glm::vec3(-5.0f, 20.0f, 20.0f);
	glm::vec3 m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	GLfloat m_yaw = -90.0f;			//left and right (rotation about y) -90 => forward is negative z dir
	GLfloat m_pitch = 0.0f;			//up and down (rotation about x)
	//GLfloat m_roll;				//not used tilt left and right (rotation about z)

	GLfloat m_near = 0.1f;
	GLfloat m_far = 500.0f;

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;

protected:
	void Update();
};




class PerspectiveCamera : public Camera
{
public:

	friend class ImGuilayer;

	PerspectiveCamera() = delete;
	PerspectiveCamera(GLfloat aspect_ratio);
	PerspectiveCamera(GLfloat aspect_ratio, glm::vec3 position, GLfloat yaw, GLfloat pitch);
	~PerspectiveCamera() = default;

	glm::mat4 ProjMatrix() override;
	void Zoom(float amount) override;
	void SetAspectRatio(float width, float height);
	GLfloat FOV() const { return m_fov; }

private:
	GLfloat m_aspect_ratio; // width/height
	GLfloat m_fov = 45.0f; // Field of view - In degrees
};

class OrthographicCamera : public Camera
{
public:
	friend class ImGuilayer;

	OrthographicCamera();
	OrthographicCamera(glm::vec4 extent);
	OrthographicCamera(glm::vec4 extent, glm::vec3 position, GLfloat yaw, GLfloat pitch);
	~OrthographicCamera() = default;

	glm::mat4 ProjMatrix() override;
	void Zoom(float amount) override;

private:
	glm::vec4 m_extent = glm::vec4(0.0f, 10.0f, 0.0f, 10.0f);	//left, right, top, bottom
};