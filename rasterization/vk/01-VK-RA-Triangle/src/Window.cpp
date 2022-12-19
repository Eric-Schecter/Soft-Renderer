#include "Window.h"

#include <iostream>

Window::Window(int x, int y, int _width, int _height) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	if (_width == 0 && _height == 0) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		if (monitor == nullptr) {
			std::cout << "failed to get primary monitor!" << std::endl;
		}
		const GLFWvidmode* screen = glfwGetVideoMode(monitor);
		width = screen->width;
		height = screen->height;
	}

	window = glfwCreateWindow(width, height, "Renderer", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::render(Renderer* renderer) {
	while (!glfwWindowShouldClose(window)) {
		renderer->render();
		glfwPollEvents();
	}

	renderer->waitIdle();
}