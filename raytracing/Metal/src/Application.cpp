#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>
#include <cstdlib>
#include <ctime>

#include "Lambertian.h"
#include "Metal.h"

Application::Application() {
	std::srand(time(NULL));
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(renderer);
	release(window);
}

void Application::run() {
	bool mark = false;
	while (window->isRunning) {
		window->handleEvents();
		if (!window->init()) {
			break;
		}

		if (!mark) {
			renderer->setFramebuffer(static_cast<uint32_t*>(window->pixels), window->width, window->height);
			renderer->clear();
			renderer->render(scene);
		}

		if (!window->render()) {
			break;
		}
		
		mark = true; // render once for now until Monte Carlo Sampling
	}
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	renderer = new Renderer();
}

void Application::initScene() {
	auto sphere1 = std::make_shared<Sphere>(glm::vec3(-.5f,0.25f,0.f),0.25f, std::make_shared<Metal>(glm::vec3(1.f),0.3f));
	auto sphere2 = std::make_shared<Sphere>(glm::vec3(0.f, 0.25f, 0.f), 0.25f, std::make_shared<Lambertian>(glm::vec3(.7f, .3f, .3f)));
	auto sphere3 = std::make_shared<Sphere>(glm::vec3(0.5f, 0.25f, 0.f), 0.25f, std::make_shared<Metal>(glm::vec3(1.f),1.f));
	auto land = std::make_shared<Sphere>(glm::vec3(0.f,-10.f,0.f), 10.f, std::make_shared<Lambertian>(glm::vec3(1.f,1.f,0.f)));
	scene.push_back(sphere1);
	scene.push_back(sphere2);
	scene.push_back(sphere3);
	scene.push_back(land);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}