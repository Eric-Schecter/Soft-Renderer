#include "Window.h"

#include <iostream>

Window::Window(int x, int y, int _width, int _height) {
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

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

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		std::cout << glewGetErrorString(res) << std::endl;
		throw std::runtime_error("failed to init context");
	};
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::render(std::shared_ptr<Renderer> renderer, const std::vector<Mesh>& scene, Camera* camera) {
	while (!glfwWindowShouldClose(window)) {
		for (const auto& mesh : scene) {
			renderer->render(mesh,camera);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}