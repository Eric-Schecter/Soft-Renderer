#include "Window.h"

#include <iostream>

Window::Window(int x, int y, int _width, int _height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		throw "SDL_Init failed";
	}

	SDL_DisplayMode displayMode;
	const int border = 30;
	if (_width == 0 && _height == 0) {
		if (SDL_GetDesktopDisplayMode(0, &displayMode)) {
			SDL_Quit();
			throw "get display mode failed";
		}
		width = displayMode.w;
		height = displayMode.h - border;
	}

	window = SDL_CreateWindow("Renderer", x, y+ border, width, height, 0);
	if (!window) {
		SDL_Quit();
		throw "SDL_CreateWindow failed";
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		SDL_Quit();
		throw "SDL_CreateRenderer failed";
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!texture) {
		SDL_Quit();
		throw "SDL_CreateTexture failed";
	}
}

Window::~Window() {
	SDL_DestroyTexture(texture);
;	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::handleKeyEvent(const SDL_Event& event) {
	switch (event.key.keysym.sym) {
		default:
			break;
		case SDLK_ESCAPE:
			isRunning = false;
			break;
		}
}

void Window::handleEvent(const SDL_Event& event) {
	switch (event.type) {
		default:
			break;
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			handleKeyEvent(event);
			break;
	}
}

void Window::handleEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		handleEvent(event);
	}
}

bool Window::init() {
	if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) != 0) {
		std::cout<< "SDL_LockTexture failed" << std::endl;
		return false;
	}
	return true;
}

bool Window::render() {
	SDL_UnlockTexture(texture);
	
	if (SDL_RenderClear(renderer) != 0) {
		std::cerr << "SDL_RenderClear failed" << std::endl;
		return false;
	}

	if (SDL_RenderCopy(renderer, texture, nullptr, nullptr) != 0) {
		std::cerr << "SDL_RenderCopy failed" << std::endl;
		return false;
	}

	SDL_RenderPresent(renderer);
	return true;
}