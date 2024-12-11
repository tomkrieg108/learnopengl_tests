#include "pch.h"
#include "endless_grid.h"
#include "window.h"
#include "camera.h"
#include "shader.h"

EndlessGrid::EndlessGrid(Window& window, v2::Camera& camera):
	m_window(window), m_camera(camera)
{
  float aspect_ratio = (float)m_window.BufferWidth() / (float)m_window.BufferHeight();
  m_camera.SetPerspectiveParams({ aspect_ratio , 45.0f, 0.05f, 1000.0f, true });
  m_camera.SetPosition(glm::vec3(0.0f, 0.1f, 1.0f));
  m_camera.LookAt(glm::vec3{ 0,0,0 });
}

void EndlessGrid::Startup()
{
	//glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderBuilder shader_builder("src/test_progs/misc/endless_grid/");
	m_shader = shader_builder.Vert("endless_grid.vs").Frag("endless_grid.fs").Build("Endless Grid");

  glCreateVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
}

void EndlessGrid::Shutdown()
{
  glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &vb);
}

void EndlessGrid::OnUpdate(double now, double time_step)
{
  glm::mat4 view_proj_mat = m_camera.GetProjMatrix() * m_camera.GetViewMatrix();
  m_shader->Bind();
  m_shader->SetUniformMat4f("u_VP", view_proj_mat);
  m_shader->SetUniform3f("u_camera_world_pos", m_camera.GetPosition());
  
  //glDrawArrays(GL_TRIANGLES, 0, 3);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  m_shader->Unbind();
}

void EndlessGrid::ImGuiUpdate()
{

}

void EndlessGrid::CheckKeys(double delta_time)
{
  const float move_speed = 5.0f;
  const float t = (float)(delta_time);
  bool* keys = m_window.GetKeys().m_key_code;

  if (keys[GLFW_KEY_W])
    m_camera.MoveForward(-move_speed * t); //note the negative value needed to move forward

  if (keys[GLFW_KEY_S])
    m_camera.MoveForward(move_speed * t); //note the positive value needed to move backward

  if (keys[GLFW_KEY_A])
    m_camera.MoveRight(-move_speed * t);

  if (keys[GLFW_KEY_D])
    m_camera.MoveRight(move_speed * t);
}

void EndlessGrid::OnEvent(Event& event)
{
  if (event.Type() == Event::kMouseMove)
  {
    EventMouseMove& e = dynamic_cast<EventMouseMove&>(event);
    auto* window = m_window.GlfwWindow();
    bool* keys = m_window.GetKeys().m_key_code;
    auto state = glfwGetMouseButton(window, static_cast<int32_t>(GLFW_MOUSE_BUTTON_MIDDLE));
    if (state == GLFW_PRESS)
    {
    }
    if (keys[GLFW_KEY_LEFT_CONTROL])
      m_camera.RotateWorld(e.delta_x * 0.03f, e.delta_y * 0.03f);
    else if (keys[GLFW_KEY_LEFT_SHIFT])
      m_camera.MoveForward((e.delta_x + e.delta_y) * 0.01f);
    else
      m_camera.RotateLocal(e.delta_x * 0.001f, e.delta_y * 0.05f);

    //std::cout << " mouse move\n";
  }

  if (event.Type() == Event::kMouseScroll)
  {
    EventMouseScroll& e = dynamic_cast<EventMouseScroll&>(event);
    m_camera.Zoom(e.y_offset);

    //std::cout << " mouse scroll\n";
  }

  if (event.Type() == Event::kKeyPressed)
  {
    /*EventKeyPressed& e = dynamic_cast<EventKeyPressed&>(event);
    if (e.key == GLFW_KEY_T)
    {
      if (m_controlled_camera == &m_camera)
        m_controlled_camera = m_camera_vis;
      else
        m_controlled_camera = &m_camera;
    }*/
  }
}
