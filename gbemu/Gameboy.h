#ifndef GB_Gameboy_H
#define GB_Gameboy_H

#include <SDL.h>
#include <iostream>
#include <string>

#include "cpu.h"
#include "memdefs.h"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

#define SCR_BUFFER_HEIGHT 256
#define SCR_BUFFER_WIDTH 256

#define BLACK 0x00
#define DARK_GREY 0x55
#define LIGHT_GREY 0xAA
#define WHITE 0xFF

#define vblankclk 4560 // vblank length in clock ticks

class Gameboy
{
public:
	Gameboy();
	~Gameboy();

	/// Load the ROM into the CPU's memory and clear both screens
	bool init(const std::string& romName);

	/// Main program loop
	void run();

private:
	/// Draws the full screen to screenBuffer and then copies it to screensurface through the scroll registers
	void draw();

	/// Returns true if a key was pressed
	bool handleEvents(); 

	/// Draws a single 8 pixel slice of a background
	/// b1 is byte one of the slice
	/// b2 is byte two of the slice
	/// x is the starting x coordinate to draw to
	/// y is the starting y coordinate to draw to
	void drawBGSlice(const byte b1, const byte b2, unsigned& x, unsigned& y);

	/// Draws a single 8 pixel slice of a sprite (only difference is color = (0x0, 0x0) is clear for sprites)
	/// b1 is byte one of the slice
	/// b2 is byte two of the slice
	/// x is the starting x coordinate to draw to
	/// y is the starting y coordinate to draw to
	void drawSpriteSlice(const byte b1, const byte b2, unsigned& x, unsigned& y);


	/// Draws a single pixel of color <color> to the screenbuffer at (<x>, <y>)
	void drawPixel(SDL_Surface* dest, const char color, const unsigned x, const unsigned y);

	/// Draws all 32x32 tiles of the background
	/// mem is a full copy of the cpu's memory
	void drawBG(const byte* mem);

	/// Draws all of the sprites defined in OAM
	/// Does NOT restrict number of sprites per line
	/// mem is a full copy of the cpu's memory
	void drawSprites(const byte* mem);

	/// True while the program is running
	bool running = true;

	/// CPU HALT
	void halt();
	
	/// CPU STOP
	void stop();

private:
	CPU cpu;
	SDL_Window* window = nullptr;
	SDL_Surface* screenSurface = nullptr; // surface that is drawn to from the screenBuffer
	SDL_Surface* screenBuffer = nullptr; // full 32x32 background, controlled by the scroll registers (SCX, SCY)
	SDL_Rect srcSurfaceRect;
	SDL_Rect pixel;

	/// Delay the v-blank from everyframe to every 30 frames
	/// This is done because the original hardware does not v-blank every frame
	unsigned int framesSinceLastVBlank = 0;
	const int framesBetweenVBlank = 30;
};

/// Clears an SDL_Surface to white
void clear(SDL_Surface*);

#endif