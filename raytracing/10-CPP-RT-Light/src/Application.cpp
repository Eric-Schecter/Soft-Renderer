#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>
#include <cstdlib>
#include <ctime>

#include "Lambertian.h"
#include "Metal.h"
#include "Dielectrics.h"
#include "ColorTexture.h"
#include "CheckerTexture.h"
#include "NoiseTexture.h"
#include "ImageTexture.h"
#include "Light.h"

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
			renderer->render(scene,camera);
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
	auto sphere1 = std::make_shared<Sphere>(
		glm::vec3(-.5f, 0.25f, 0.f),
		0.25f, 
		std::make_shared<Dielectrics>(1.5f)
	);
	//auto sphere2 = std::make_shared<Sphere>(
	//	glm::vec3(0.f, 0.25f, 0.f), 
	//	0.25f, 
	//	std::make_shared<Lambertian>(std::make_shared<ImageTexture>("images/earthmap.jpg"))
	//);
	auto sphere2 = std::make_shared<Sphere>(
		glm::vec3(0.f, 0.25f, 0.f),
		0.25f,
		std::make_shared<Light>(std::make_shared<ImageTexture>("images/earthmap.jpg"))
		);
	auto sphere3 = std::make_shared<Sphere>(
		glm::vec3(0.5f, 0.25f, 0.f),
		0.25f, 
		std::make_shared<Metal>(std::make_shared<ColorTexture>(glm::vec3(1.f)),0.1f)
	);
	auto land = std::make_shared<Sphere>(
		glm::vec3(0.f,-100.f,0.f), 
		100.f, 
		std::make_shared<Lambertian>(std::make_shared<CheckerTexture>(
			std::make_shared<ColorTexture>(glm::vec3(1.f,1.f,0.f)),
			std::make_shared<ColorTexture>(glm::vec3(0.f,1.f,1.f)))
		)
	);

	scene.push_back(sphere1);
	scene.push_back(sphere2);
	scene.push_back(sphere3);
	scene.push_back(land);

	glm::vec3 pos(1.f, 1.f, 1.f);
	glm::vec3 lookat(0.f, 0.25f, 0.f);
	float aspect = static_cast<float>(window->width) / window->height;
	float fov = glm::radians(90.f);
	float aperture = 0.f;
	float focus_dist = glm::length(pos - lookat);
	camera = std::make_shared<Camera>(pos,lookat,aspect,fov, aperture, focus_dist);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}