#pragma once

#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Mesh.h"

class Window
{
public:
	Window(int x = 0, int y = 0, int _width = 0, int _height = 0);
	~Window();

	void render(Renderer* renderer,const std::vector<Mesh>& scene);

	int width = 0;
	int height = 0;
	GLFWwindow* window = nullptr;
};

