#include "Application.h"

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(window);
	release(renderer);
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	renderer = new Renderer(window->window);
}

void Application::initScene() {
	const std::vector<Vertex> vertices = {
		{ {0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{ {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	 };
	Mesh mesh;
	mesh.vertices = vertices;
	scene.push_back(mesh);
}

void Application::run() {
	window->render(renderer,scene);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}