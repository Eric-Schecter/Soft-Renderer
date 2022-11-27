#pragma once

#include "Renderer.h"
#include "Window.h"

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

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

