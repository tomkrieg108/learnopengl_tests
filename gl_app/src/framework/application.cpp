#include "pch.h"

#include <imgui_docking/imgui.h>
#include <imgui_docking/backends/imgui_impl_glfw.h>
#include <imgui_docking/backends/imgui_impl_opengl3.h>

#include "base.h"
#include "layer.h"
#include "window.h"
#include "shader.h"
#include "camera.h"
#include "gl_utils.h"
#include "application.h"

App App::s_instance;

App::App()
{
	m_window = new Window();

	float aspect_ratio = (float)m_window->BufferWidth() / (float)m_window->BufferHeight();
	m_camera = new PerspectiveCamera(aspect_ratio);
	m_camera2 = new v2::Camera();
	m_camera2->SetAspectRatio((float)m_window->BufferWidth(), (float)m_window->BufferHeight());
	
	TestAppMgr test_app_mgr(*m_window, *m_camera, *m_camera2);

	m_coords = test_app_mgr.GetLayer(TestAppMgr::COORD_SYS_CAM1);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_TEXTURE_TEST);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_TEXTURES_COMBINED_TEST);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BASIC_LIGHTING_COLOURS);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BASIC_LIGHTING_DIFFUSE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BASIC_LIGHTING_SPECULAR);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_MATERIALS_TEST);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_MAP_DIFFUSE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_MAP_SPECULAR);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_MAP_EMISSION);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_CASTER_DIRECTIONAL);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_CASTER_POINT);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_CASTER_SPOT);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_LIGHT_CASTER_SPOT_SOFT);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_MULTIPLE_LIGHTS);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_MODEL_LOADING_TEST);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_DEPTH_TESTING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_DEPTH_TESTING_VIEW);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_STENCIL_TESTING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BLEND_DISCARD_TESTING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BLEND_SORT_TESTING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_FACE_CULLING); 
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_FRAME_BUFFER_TEST);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SKYBOX);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ENV_MAPPING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ADVANCED_GLSL_UBO);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_GEOMETRY_SHADER_HOUSES);
  //m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_GEOMETRY_SHADER_EXPLODING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_NORMAL_VISUALISATION);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_INSTANCING_QUADS);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ASTEROIDS);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ASTEROIDS_INSTANCED);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ANTI_ALIASING_MSAA);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ANTI_ALIASING_OFFSCREEN);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_ADVANCED_LIGHTING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_GAMMA_CORRECTION);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SHADOW_MAPPING_DEPTH);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SHADOW_MAPPING_BASE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SHADOW_MAPPING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_POINT_SHADOW);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_POINT_SHADOW_SOFT);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_NORMAL_MAPPING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_PARALLAX_MAPPING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_PARALLAX_MAPPING_MULTI_SAMPLE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_PARALLAX_OCCLUSION_MAPPING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_HDR);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_BLOOM);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_DEFERRED_SHADING);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SCREEN_SPACE_AMBIENT_OCCLUSION);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_TEXT_RENDERING);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_HEIGHT_MAP_CPU);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_HEIGHT_MAP_TESSELATION);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_CSM);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SCENE_GRAPH);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_SKELETAL_ANIMATION);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_POINT);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_TRIANGLE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_MOVING_TRIANGLE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_TESSELATED_TRIANGLE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_GEOM_TRIANGLE);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::SB7_CH5_VERTEX_ARRAYS);

	//m_layer = test_app_mgr.GetLayer(TestAppMgr::MISC_ANIMATED_MODEL_BASIC);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::MISC_COMP_GEOM);
	
	

	/*---------------------------------------------------------------------
	Everything following this uses m_camera2 instead of m_camera => uncomment next line if using
	-----------------------------------------------------------------------*/
	if (m_coords)
	{
		delete m_coords;
		m_coords = test_app_mgr.GetLayer(TestAppMgr::COORD_SYS_CAM2);
	}
	
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::MISC_DIR_SHADOW_VISUALISED);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::MISC_CSM_VISUALISED);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_PBR_BASIC);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_PBR_TEXTURED);
	//m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_IBR_DIFFUSE_IRRADIANCE_1);
	m_layer = test_app_mgr.GetLayer(TestAppMgr::LGL_IBR_DIFFUSE_IRRADIANCE_2);

	EventManager::SetCallback(this, &App::OnMouseMove);
	EventManager::SetCallback(this, &App::OnMouseLDown);
	EventManager::SetCallback(this, &App::OnMouseLUp);
	EventManager::SetCallback(this, &App::OnMouseRDown);
	EventManager::SetCallback(this, &App::OnMouseRUp);
	EventManager::SetCallback(this, &App::OnMouseMDown);
	EventManager::SetCallback(this, &App::OnMouseMUp);
	EventManager::SetCallback(this, &App::OnMouseScroll);
	EventManager::SetCallback(this, &App::OnKeyPressed);
	EventManager::SetCallback(this, &App::OnKeyReleased);
	EventManager::SetCallback(this, &App::OnWinResize);
	//DisplayGLVersion();
	//DisplayGLParams();
	//DisplayVideoModes();
}


App::~App()
{
	delete m_layer;
	if (m_coords)
		delete m_coords;
}

void App::Startup()
{
	if(m_coords)
		m_coords->Startup();
	if(m_layer)
		m_layer->Startup();
	App::ImGuiInit();
}

void App::Shutdown()
{
	if (m_coords)
		m_coords->Shutdown();
	if (m_layer)
		m_layer->Shutdown();
	App::ImGuiShutdown();
}

void App::Render(double now, double time_step)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//sb7 calls glClearBufferfv() - see location 1271
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!
	
	if (m_layer)
		m_layer->OnUpdate(now, time_step);
	if (m_coords)
		m_coords->OnUpdate(now, time_step);
}

void App::Run()
{
	App::Startup();

	auto delta_time = 0.0;
	auto last_time = glfwGetTime(); //seconds since GLFW was initialised
	while (!m_window->ShouldClose())
	{
		auto now = glfwGetTime(); //in seconds
		delta_time = now - last_time;
		last_time = now;

		CheckKeys(delta_time);

		App::Render(now, delta_time);
		App::ImGuiRender();
		
		m_window->SwapBuffers();
		m_window->PollEvents();
	}

	App::Shutdown();
}

void App::ImGuiInit()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	//Slows the frame rate when IMGui window is outside main window.  More windows outside, slower the frame rate
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	float fontSize = 22.0f;
	//TODO - can't find these files in debug build - craches (ok in realease) WTF?
	//io.Fonts->AddFontFromFileTTF("asset/fonts/opensans/OpenSans-Bold.ttf", fontSize);
	//io.FontDefault = io.Fonts->AddFontFromFileTTF("asset/fonts/opensans/OpenSans-Regular.ttf", fontSize);
	//ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window->GlfwWindow(), true);
	const char* glsl_version = "#version 410";
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void App::ImGuiUpdate()
{
	auto win_width = m_window->Width();
	auto buf_width = m_window->BufferWidth();
	//ImGuiWindowFlags window_flags = 0;
	ImGui::Begin("App");

	if (ImGui::CollapsingHeader("Window"))
	{
		//if (ImGui::TreeNode("Frame Rate"))
		{
			
			ImGui::Text("Width,Height %d %d : ", m_window->Width(), m_window->Height());
			ImGui::Text(" Buf Width, Buf Height %d %d : ", m_window->BufferWidth(), m_window->BufferHeight());
			ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			//ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat3("Position ", &(m_camera->Position()[0]), -100.0f, 100.0f);
		ImGui::SliderFloat3("Front ", &(m_camera->Front()[0]), -1.01f, 1.01f);
		ImGui::SliderFloat3("Up    ", &(m_camera->Up()[0]), -1.01f, 1.01f);
		ImGui::SliderFloat3("Right ", &(m_camera->Right()[0]), -1.01f, 1.01f);
		
		auto yaw = m_camera->Yaw();
		auto pitch = m_camera->Pitch();
		auto near = m_camera->ZNear();
		auto far = m_camera->ZFar();

		ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::SliderFloat("Pitch", &pitch, -180.0f, 180.0f);
		//ImGui::SliderFloat("FOV", &(m_camera->F), 15.0f, 75.0f);
		ImGui::SliderFloat("Z Near", &near, 0.01f, 2.0f);
		ImGui::SliderFloat("Z Far", &far, 80.0f, 200.0f);
	}

	ImGui::End();

	//ImGui::ShowDemoWindow();
}

void App::ImGuiShutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void App::ImGuiRender()
{
	App::ImGuiNewFrame();

	App::ImGuiUpdate();
	if(m_layer)
		m_layer->ImGuiUpdate();

	//Need to do the following when ImGuiConfigFlags_ViewportsEnable is set:
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)m_window->Width(), (float)m_window->Height()); //This has no effect?

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void App::ImGuiNewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void App::OnMouseMove(EventMouseMove& e)
{
	// don't pass mouse and keyboard presses further if an ImGui widget is active
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	
	bool* keys = m_window->GetKeys().m_key_code;
	if (keys[GLFW_KEY_LEFT_ALT])
		return;

	const float sensitivity = 0.1f;
 	m_camera->Turn(e.delta_x * sensitivity, e.delta_y * sensitivity);

	m_layer->OnEvent(e);
}

void App::OnMouseLDown(EventMouseLDown& e)
{
	m_layer->OnEvent(e);
}
void App::OnMouseLUp(EventMouseLUp& e)
{
	m_layer->OnEvent(e);
}
void App::OnMouseRDown(EventMouseRDown& e)
{
	m_layer->OnEvent(e);
}
void App::OnMouseRUp(EventMouseRUp& e)
{
	m_layer->OnEvent(e);
}
void App::OnMouseMDown(EventMouseMDown& e)
{
	m_layer->OnEvent(e);
}
void App::OnMouseMUp(EventMouseMUp& e)
{
	m_layer->OnEvent(e);
}

void App::OnMouseScroll(EventMouseScroll& e)
{
	m_camera->Zoom(e.y_offset);
	m_layer->OnEvent(e);
}

void App::OnKeyPressed(EventKeyPressed& e)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
		return;
	m_layer->OnEvent(e);
}

void App::OnKeyReleased(EventKeyReleased& e)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
		return;
	m_layer->OnEvent(e);
}


void App::OnWinResize(EventWinResize& e)
{
	m_camera->SetAspectRatio((float)e.buffer_width, (float)e.buffer_height);
	m_camera2->SetAspectRatio((float)e.buffer_width, (float)e.buffer_height);

	m_layer->OnEvent(e);
}

void App::CheckKeys(double delta_time)
{
	const float move_speed = 5.0f;
	const float t = (float)(delta_time);
	bool* keys = m_window->GetKeys().m_key_code;

	if (keys[GLFW_KEY_W])
		m_camera->MoveForward(move_speed * t);

	if (keys[GLFW_KEY_S])
		m_camera->MoveForward(-move_speed * t);

	if (keys[GLFW_KEY_A])
		m_camera->MoveRight(-move_speed * t);

	if (keys[GLFW_KEY_D])
		m_camera->MoveRight(move_speed * t);
	
	m_layer->CheckKeys(delta_time);

	if (keys[GLFW_KEY_ESCAPE])
		m_window->ShutDown();
}


