#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(renderer);
	release(window);
}

void Application::run() {
	while (window->isRunning) {
		window->handleEvents();
		if (!window->init()) {
			break;
		}

		renderer->setFramebuffer(static_cast<uint32_t*>(window->pixels), window->width, window->height);
		renderer->clear();
		renderer->render(scene);

		if (!window->render()) {
			break;
		}
	}
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	renderer = new Renderer();
}

void Application::initScene() {
	auto sphere = std::make_shared<Sphere>(glm::vec3(0.f),0.25f,glm::vec3(1.f));
	scene.push_back(sphere);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}