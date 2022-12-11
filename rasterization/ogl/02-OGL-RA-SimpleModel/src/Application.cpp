#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec3.hpp>

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
	const std::string MODEL_PATH = "./models/african_head/african_head.obj";
	const std::string VERTEX_SHADER_PATH = "./shaders/triangle.vert";
	const std::string FRAGMENT_SHADER_PATH = "./shaders/triangle.frag";
	Mesh mesh;
	mesh.loadModel(MODEL_PATH);
	mesh.setupProgram(
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH
	);
	scene.push_back(mesh);
}

void Application::run() {
	window->render(renderer, scene);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}