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

}

void Application::run() {
	window->render(renderer);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}