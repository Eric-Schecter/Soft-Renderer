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
#include "Box.h"
#include "Sphere.h"

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
	auto left = std::make_shared<Box>(
		glm::vec3(-0.51f, 0.f, -0.5f),
		glm::vec3(-0.5f, 1.f, 0.5f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(0.f,1.f,0.f)))
	);
	auto right = std::make_shared<Box>(
		glm::vec3(0.5f, 0.f, -0.5f),
		glm::vec3(0.51f, 1.f, 0.5f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(1.f,0.f,0.f)))
	);
	auto bottom = std::make_shared<Box>(
		glm::vec3(-0.5f, 0.f, -0.5f),
		glm::vec3(0.5f, .01f, 0.5f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(.9f)))
	);
	auto top = std::make_shared<Box>(
		glm::vec3(-0.5f, 1.f, -0.5f),
		glm::vec3(0.5f, 1.01f, 0.5f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(.9f)))
	);
	auto back = std::make_shared<Box>(
		glm::vec3(-0.5f, 0.f, -0.51f),
		glm::vec3(0.5f, 1.f, -0.5f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(.9f)))
	);
	auto light = std::make_shared<Box>(
		glm::vec3(-0.3f, 0.85f, 0.f),
		glm::vec3(0.3f, 0.86f, 0.4f),
		std::make_shared<Light>(std::make_shared<ColorTexture>(glm::vec3(1.f)))
	);
	auto sphere = std::make_shared<Sphere>(
		glm::vec3(0.125f, 0.125f, 0.25f),
		0.125f, 
		std::make_shared<Metal>(std::make_shared<ColorTexture>(glm::vec3(1.f)))
	);
	auto box = std::make_shared<Box>(
		glm::vec3(-0.25f, 0.f, -0.25f),
		glm::vec3(0.f, 0.5f, 0.f),
		std::make_shared<Lambertian>(std::make_shared<ColorTexture>(glm::vec3(0.8f)))
	);

	scene.push_back(light);
	scene.push_back(left);
	scene.push_back(right);
	scene.push_back(bottom);
	scene.push_back(top);
	scene.push_back(back);
	scene.push_back(box);
	scene.push_back(sphere);

	glm::vec3 pos(0.f, .5f, 1.25f);
	glm::vec3 lookat(0.f, 0.5f, 0.f);
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