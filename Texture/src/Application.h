#pragma once

#include "Renderer.h"
#include "Window.h"
#include "Camera.h"
#include "Image.h"

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

	Window* window;
	Renderer* renderer;
	Camera* camera;
	Image* diffuse;
	Image* normal;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

