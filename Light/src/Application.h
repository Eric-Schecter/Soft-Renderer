#pragma once

#include "Renderer.h"
#include "Window.h"
#include "Camera.h"
#include "Image.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <vector>

class Application
{
public:
	Application();
	~Application();

	void run();
private:
	void initWindow();
	void initRenderer();
	virtual void initScene();
	void release(void* pointer);

	Window* window = nullptr;
	Renderer* renderer = nullptr;
	Camera* camera = nullptr;
	Material* material = nullptr;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	AmbientLight* ambientLight = nullptr;
	DirectionalLight* directionalLight = nullptr;
	PointLight* pointLight = nullptr;
	SpotLight* spotLight = nullptr;
};

