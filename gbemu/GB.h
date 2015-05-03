#ifndef GB_GB_H
#define GB_GB_H

#include <SDL.h>
#include <iostream>
#include <string>

#include "cpu.h"
#include "memdefs.h"
#include "binLut.h"

#define MODIFIER 1

#define WIDTH 256 * MODIFIER
#define HEIGHT 256 * MODIFIER

#define BLACK 0x00
#define DARK_GREY 0x55
#define LIGHT_GREY 0xAA
#define WHITE 0xFF

class GB
{
public:
	GB();
	~GB();

	bool init(const std::string& romName);
	void run();

private:
	void draw();
	bool handleEvents(); // returns true if a key was pressed
	void drawSlice(const byte b1, const byte b2, unsigned& x, unsigned& y);

	void drawPixel(const char color, const unsigned x, const unsigned y);
	void drawBG(const byte* mem);
	void drawSprites(const byte* mem);

	bool running = true;

	void halt();
	void stop();

private:
	CPU cpu;
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr; // surface that is drawn to from the virtSurface, controlled by the scroll registers
	SDL_Surface* screenBuffer = nullptr; // full 32x32 virtual gameboy screen
	SDL_Rect srcSurfaceRect;
	SDL_Rect pixel;

	unsigned int framesSinceLastVBlank = 0;
	const int framesBetweenVBlank = 30;
};

void clear(SDL_Surface*);

#endif