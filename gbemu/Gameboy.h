#ifndef GB_GAMEBOY_H
#define GB_GAMEBOY_H

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "cpu.h"
#include "memdefs.h"
#include "input.h"
#include "types.h"

#ifdef DEBUG
#include "toHex.h"
#endif

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

#define SCR_BUFFER_HEIGHT 256
#define SCR_BUFFER_WIDTH 256

// These colors roughly mimick the green colors of the DMG Gameboy 
#define BLACK 8, 24, 32
#define DARK_GREY 52, 104, 86
#define LIGHT_GREY 136, 192, 112
#define WHITE 224, 248, 208

class Gameboy
{
public:
	Gameboy();
	~Gameboy();

	// Load the ROM into the CPU's memory and clear both screens
	// @param romName is the name/ file path of the ROM - ".gb" required at the end
	bool init(const std::string& romName);

	// Main program loop
	void run();

private:
	// Draws the full screen to screenBuffer and then copies it to screensurface through the scroll registers
	void renderFull();

	/// Draws a single scanline from the screenSurface to the screenFinal and increments the current scanline
	void drawScanline();

	// @Returns true if a key valid key on the Gameboy was pressed (this is used for breaking out of the STOP command)
	bool handleEvents(); 

	// Draws a single 8 pixel slice of a background
	// @param b1 is byte one of the slice 
	// @param b2 is byte two of the slice
	// @param x is the starting x coordinate to draw to
	// @param y is the starting y coordinate to draw to
	void drawBGSlice(const byte b1, const byte b2, unsigned& x, unsigned& y);

	// Draws a single 8 pixel slice of a sprite (only difference is color = (0x0, 0x0) is clear for sprites)
	// @param b1 is byte one of the slice
	// @param b2 is byte two of the slice
	// @param x is the starting x coordinate to draw to
	// @param y is the starting y coordinate to draw to
	void drawSpriteSlice(const byte b1, const byte b2, unsigned& x, unsigned& y);

	// Draws a single pixel of color <color> to the screenbuffer at (<x>, <y>)
	// @param dest is a pointer to the SDL_Surface to draw to
	// @param r is the red component of the RGB color
	// @param g is the green component of the RGB color
	// @param b is the blue component of the RGB color
	// @param x is the x coordinate to draw the pixel at
	// @param y is the y coordinate to draw the pixel at
	void drawPixel(SDL_Surface* dest, const char r, const char g, const char b, const unsigned x, const unsigned y);

	// Draws all 32x32 tiles of the background
	// @param mem is a full copy of the cpu's memory
	void drawBG(const std::vector<byte>& mem);

	// Draws all of the sprites defined in OAM
	// Does NOT restrict number of sprites per line
	// @param mem is a full copy of the cpu's memory
	void drawSprites(const std::vector<byte>& mem);

	// emulate CPU HALTing
	void halt();
	
	// emulate CPU STOPing
	void stop();

	bool __T = false;

private:
	CPU cpu; // the emulated z80-like cpu of the Gameboy
	ubyte scanline = 0; // current scanline to draw

	SDL_Window* window = nullptr;
	SDL_Surface* fullScreenSurface = nullptr; // Final image surface including scrolled background and sprites 
	SDL_Surface* backgroundSurface = nullptr; // Full 32x32 background, controlled by the scroll registers (SCX, SCY)
	SDL_Surface* windowSurface = nullptr;  // Surface that is actually rendered to the window

	// True while the program is running
	bool running = true;

	// these three are only here as an optimization so 3 new SDL_Rect objects arent made at a very high frequency
	SDL_Rect srcSurfaceRect; // An SDL_Rect that represents the area and scroll coords of the window to be copied from the backgroundSurface to the fullscreenSurface
	SDL_Rect pixel; // A pixel for rendering
	SDL_Rect scanLineRect; // An SDL_Rect that represents the area and coords of the current scanline to copy from the fullscreenSurface to the windowSurface
};

/// Clears an SDL_Surface to white
void clear(SDL_Surface*);

#endif // GB_GAMEBOY_H
