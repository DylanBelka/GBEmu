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
	void renderFull();

	/// Draws a single scanline from the screenSurface to the screenFinal and increments the current scanline
	void drawScanline();

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
	CPU cpu; // the emulated z80 cpu of the Gameboy
	ubyte scanline = 0; // current scanline to draw

	SDL_Window* window = nullptr;
	SDL_Surface* fullScreenSurface = nullptr; /// Final image surface including scrolled background and sprites 
	SDL_Surface* backgroundSurface = nullptr; /// Full 32x32 background, controlled by the scroll registers (SCX, SCY)
	SDL_Surface* windowSurface = nullptr;  /// Surface that is actually rendered to the window

	// these three are only here as an optimization so 3 new SDL_Rect objects arent made with very high frequency
	SDL_Rect srcSurfaceRect; /// An SDL_Rect that represents the area and scroll coords of the window to be copied from the backgroundSurface to the fullscreenSurface
	SDL_Rect pixel; /// A pixel for rendering
	SDL_Rect scanLineRect; /// An SDL_Rect that represents the area and coords of the current scanline to copy from the fullscreenSurface to the windowSurface

	/// Delay the v-blank from everyframe to every 30 frames
	/// This is done because the original hardware does not v-blank every frame
	unsigned int framesSinceLastVBlank = 0;
	const int framesBetweenVBlank = 30;
};

/// Clears an SDL_Surface to white
void clear(SDL_Surface*);

template<typename T>
const std::string toHex(const T val)
{
	std::stringstream stream;
	stream << std::hex << val;
	std::string result(stream.str());
	return "0x" + result;
}

#endif