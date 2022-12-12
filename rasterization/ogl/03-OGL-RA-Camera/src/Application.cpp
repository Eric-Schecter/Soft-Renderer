#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

#include "ModelRenderer.h"
#include "Vertex.h"
#include "Mesh.h"

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
	renderer = std::make_shared<ModelRenderer>();
}

void Application::initScene() {
	const float fov = glm::radians(60.f);
	const float aspect = static_cast<float>(window->width) / window->height;
	const float near = 0.1f;
	const float far = 1000.f;
	camera = new Camera();
	camera->pos = glm::vec3(1.f, 1.f, 2.f);
	camera
		->setView(glm::vec3(0.f, 0.f, 0.f))
		->setProjection(fov, aspect, near, far)
		->setViewport(0, 0, window->width, window->height);

	const std::string MODEL_PATH = "./models/african_head/african_head.obj";
	const std::string VERTEX_SHADER_PATH = "./shaders/triangle.vert";
	const std::string FRAGMENT_SHADER_PATH = "./shaders/triangle.frag";
	Mesh mesh;
	mesh.setupModel(MODEL_PATH);
	mesh.setupProgram(
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH
	);
	mesh.setupUniforms();
	scene.push_back(mesh);
}

void Application::run() {
	window->render(renderer, scene, camera);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}