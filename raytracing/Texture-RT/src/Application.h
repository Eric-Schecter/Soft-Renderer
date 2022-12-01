#pragma once

#include "Renderer.h"
#include "Window.h"
#include "Primitive.h"
#include "Sphere.h"

#include <vector>
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
	Renderer* renderer;
	std::vector<std::shared_ptr<Primitive>> scene;
	std::shared_ptr<Camera> camera;
};

