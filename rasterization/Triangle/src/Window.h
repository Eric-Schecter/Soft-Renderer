#pragma once

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class Window
{
public:
	Window(int x = 0, int y = 0, int _width = 0, int _height = 0);
	~Window();

	bool init();
	bool render();
	void handleEvents();

	bool isRunning = true;
	int width = 0;
	int height = 0;
	void* pixels = nullptr;
	int pitch = 0;

private:
	void handleEvent(const SDL_Event& event);
	void handleKeyEvent(const SDL_Event& event);

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Texture* texture = nullptr;
};

