#pragma once

#include "camera.h"
#include "events.h"

//class Camera;

class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer() = default;
	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void Startup() {}
	virtual void Shutdown() {}
	virtual void OnUpdate(double now, double delta_time) {}
	virtual void ImGuiUpdate() {}
	virtual void OnKeyPressed(EventKeyPressed& e) { std::cout << "Key code pressed - layer " << e.key << "\n"; }
	//virtual void OnImGuiRender() {}
	//virtual void OnEvent(Event& event) {}

	const std::string& GetName() const { return m_debug_name; }
protected:
	std::string m_debug_name;
};

class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void PopLayer(Layer* layer);
	void PopOverlay(Layer* overlay);

	std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
	std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

	std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
	std::vector<Layer*>::const_iterator end()	const { return m_Layers.end(); }
	std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
	std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }
private:
	std::vector<Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};
