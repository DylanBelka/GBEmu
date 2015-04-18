#ifndef GB_GB_H
#define GB_GB_H

#include <glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include <string>

#include "cpu.h"
#include "memdefs.h"
#include "binLut.h"

#define WIDTH 160
#define HEIGHT 144

#define BLACK 0x00
#define DARK_GREY 0x55
#define LIGHT_GREY 0xAA
#define WHITE 0xFF

#define GL_BLACK 0.0f
#define GL_DARK_GREY 0.2f
#define GL_LIGHT_GREY 0.7f
#define GL_WHITE 1.0f

#define DEBUG

/*
Fix so that rendering does not happen every cycle
Add signed background rendering
*/

class GB
{
public:
	GB();
	~GB();

	bool init(const std::string& romName);
	void run();


private:
	void draw();
	void handleEvents();
	void drawSlice(unsigned char b1, unsigned char b2, unsigned& x, unsigned& y);

	void drawPixel(const char color, const unsigned x, const unsigned y);
	inline void clear();

	bool running = true;


private:
	CPU cpu;
	SDL_Window* window = nullptr;
	SDL_GLContext glContext = NULL;

	SDL_Surface* screenSurface = nullptr; // surface that is drawn to from the virtSurface, controlled by the scroll registers
	SDL_Surface* screenBuffer = nullptr; // full 32x32 virtual gameboy screen
	SDL_Rect srcSurfaceRect;
	SDL_Rect pixel;
};

#endif