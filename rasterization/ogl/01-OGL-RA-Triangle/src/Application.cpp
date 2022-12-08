#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec3.hpp>
#include <gl/glew.h>

#include "./loadShaders.h"
#include "TriangleRenderer.h"
#include "TriangleRendererDSA.h"

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(window);
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	//renderer = std::make_shared<TriangleRenderer>();
	renderer = std::make_shared<TriangleRendererDSA>();
}

void Application::initScene() {
	
}

void Application::run() {
	window->render(renderer);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}