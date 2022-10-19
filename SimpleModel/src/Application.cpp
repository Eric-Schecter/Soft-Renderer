#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>

#include "Vertex.h"

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

		Vertex v1(window->width / 2, window->height / 4);
		Vertex v2(window->width / 4 , window->height * 3/ 4);
		Vertex v3(window->width * 3 / 4, window->height * 3 /4);
			
		renderer->render(v1,v2,v3, glm::vec4(1.f));

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

}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}