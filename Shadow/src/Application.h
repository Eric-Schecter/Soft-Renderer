#pragma once

#include "Pipeline.h"
#include "Window.h"
#include "Camera.h"
#include "ColorTexture.h"
#include "Mesh.h"
#include "Light.h"
#include "PhongShader.h"
#include "DepthShader.h"

#include <vector>
#include <string>

class Application
{
public:
	Application();
	~Application();

	void run();
	inline void setViewport(const glm::vec4& viewport) { m_viewport = viewport; };

private:
	void initWindow();
	void initRenderer();
	virtual void initScene();
	void release(void* pointer);
	Mesh* loadModel(const std::string& objPath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices,Material* material);
	void update();

	Window* window = nullptr;
	Pipeline* pipeline = nullptr;
	Camera* camera = nullptr;
	Mesh* mesh = nullptr;
	Light* lights = new Light();
	PhongShader* phongShader = new PhongShader();
	DepthShader* depthShader = new DepthShader();
	glm::vec4 m_viewport = glm::vec4(1.f);
};

