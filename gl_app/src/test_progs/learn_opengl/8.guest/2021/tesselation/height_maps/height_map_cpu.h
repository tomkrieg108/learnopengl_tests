//https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

#pragma once
#include "layer.h"

class Shader;
class Camera;

class LayerHeightMapCPU : public Layer
{
public:
	LayerHeightMapCPU(Camera& camera);
	void Startup() override;
	void Shutdown() override;
	void OnUpdate(double now, double time_step) override;

private:
	std::unique_ptr<Shader> m_shader = nullptr;
	Camera& m_camera ;
	
	unsigned int terrainVAO=0, terrainVBO=0, terrainIBO=0;
	int numStrips=0;
	int numTrisPerStrip=0;
};