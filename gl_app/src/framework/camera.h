#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

 inline namespace v1
{
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
		void LookAt(glm::vec3& look_pos);
		void InvertPitch();

		virtual void Zoom(float amount) = 0;

		glm::mat4 ViewMatrix();
		glm::mat4 ModelMatrix();
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
		GLfloat m_yaw = -90.0f;			//left and right (rotation about y) -90 => fromt is negative z dir
		//GLfloat m_yaw = 0.0f;
		GLfloat m_pitch = 0.0f;			//up and down (rotation about x)
		//GLfloat m_roll;				//not used tilt left and right (rotation about z)

		GLfloat m_near = 0.1f;
		GLfloat m_far = 500.0f;

		glm::vec3 m_front = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 m_right;
		glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);

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

		void SetViewVolume(glm::vec4 extent, float near, float far);

		glm::mat4 ProjMatrix() override;
		void Zoom(float amount) override;

	private:
		glm::vec4 m_extent = glm::vec4(0.0f, 10.0f, 0.0f, 10.0f);	//left, right, top, bottom
	};

}


 namespace v2
 {
	 class Camera
	 {
	 public:

		 enum class ProjectionType { Perspective = 0, Ortho = 1, };

		 struct PerspectiveParams
		 {
			 float aspect_ratio = 1.0f;			// width / height
			 float fov = 45.0f;							// Vert Field of view in degrees
			 float z_near = 0.1f;
			 float z_far = 100.0f;
			 bool modified = true;
		 };

		 struct OrthoParams
		 {
			 float left = -20.0f;
			 float right = 20.0f;
			 float bottom = -20.0f;
			 float top = 20.0f;
			 float z_near = 0.1f;
			 float z_far = 100.0f;
			 bool modified = true;
		 };

		 // Controller
		 enum class InputMode
		 {
			 Select, RotateWorld, RotateLocal, TranslateXY, TranslateZ
		 };

	 public:
		 Camera();
		 ~Camera() = default;

		 glm::mat4 GetProjMatrix() const;
		 glm::mat4 GetInverseProjMatrix() const;
		 glm::mat4 GetViewMatrix() const;
		 void SetAspectRatio(float width, float height);
		 void Zoom(float amount);

		 void SetProjectionType(ProjectionType projection_type) { m_projection_type = projection_type; }
		 ProjectionType GetProjectionType() const { return m_projection_type; }

		 void SetPerspectiveParams(const PerspectiveParams& perspective_params);
		 void SetOrthoParams(const OrthoParams& ortho_params);

		 auto& GetPerspectiveParameters() { return m_perspective_params; }
		 auto& GetOrthoParameters() { return m_ortho_params; }

		 glm::vec3 GetPosition();
		 glm::mat4& GetTransform() { return m_transform; }

		 // Controller
		 void SetPosition(glm::vec3& position);
		 void LookAt(glm::vec3& look_pos);
		 void MoveForward(float amount);
		 void MoveRight(float amount);
		 void MoveVertically(float amount);
		 void Turn(float delta_yaw, float delta_pitch);
		 void RotateLocal(float delta_yaw, float delta_pitch);
		 void RotateWorld(float amount_x, float amount_y);
		
	 private:
			ProjectionType m_projection_type = ProjectionType::Perspective;
			PerspectiveParams m_perspective_params;
			OrthoParams m_ortho_params;
			glm::mat4 m_transform = glm::mat4(1.0f);

			//Controller
			InputMode m_input_mode = InputMode::Select;
			//TODO - this stuff should be somewhere else!
			//angle in degrees
			glm::mat4 GetRotationMatX(float angle) const;
			glm::mat4 GetRotationMatY(float angle) const;
			glm::mat4 GetRotationMatZ(float angle) const;
	 };

 }