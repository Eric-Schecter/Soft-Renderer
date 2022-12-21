#pragma once

#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"

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
	std::vector<Mesh> scene;
};
