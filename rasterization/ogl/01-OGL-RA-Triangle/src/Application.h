#pragma once

#include "Renderer.h"
#include "Window.h"

#include <memory>

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
	std::shared_ptr<Renderer> renderer;
};

