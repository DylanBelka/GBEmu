#include "Gameboy.h"

Gameboy::Gameboy() :
cpu()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		exit(-2);
	}
	window = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
	if (window == nullptr)
	{
		std::cout << "SDL_Window could not be created. Error: " << SDL_GetError() << std::endl;
		return;
	}
	windowSurface = SDL_GetWindowSurface(window);
	if (windowSurface == nullptr || windowSurface == NULL)
	{
		std::cout << "SDL_Surface <windowSurface> could not be created. Error: " << SDL_GetError() << std::endl;
		return;
	}
	SDL_PixelFormat* fmt = windowSurface->format;
	backgroundSurface = SDL_CreateRGBSurface(NULL, SCR_BUFFER_WIDTH, SCR_BUFFER_HEIGHT, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (backgroundSurface == nullptr || backgroundSurface == NULL)
	{
		std::cout << "SDL_Surface <backgroundSurface> could not be created. Error: " << SDL_GetError() << std::endl;
		return;
	}
	fullScreenSurface = SDL_CreateRGBSurface(NULL, WINDOW_WIDTH, WINDOW_HEIGHT, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (fullScreenSurface == nullptr || fullScreenSurface == NULL)
	{
		std::cout << "SDL_Surface <fullScreenSurface> could not be created. Error: " << SDL_GetError() << std::endl;
		return;
	}

	// setup the surface copy rect to some constant values
	srcSurfaceRect.w = SCR_BUFFER_WIDTH;
	srcSurfaceRect.h = SCR_BUFFER_HEIGHT;
	srcSurfaceRect.x = 0; 
	srcSurfaceRect.y = 0;
	// set up the pixel rect
	pixel.h = 1;
	pixel.w = 1;
	// set up the scanline rect
	scanLineRect.h = 1; // 1 pixel 
	scanLineRect.w = WINDOW_WIDTH;
	scanLineRect.x = 0;
}

Gameboy::~Gameboy()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Gameboy::init(const std::string& romName)
{
	clear(windowSurface);
	clear(backgroundSurface);
	clear(fullScreenSurface);
	return cpu.loadROM(romName);
}

void Gameboy::run()
{
	// CPU timings come from: http://hitmen.c02.at/files/releases/gbc/gbc_cpu_timing.txt
	const int hblankLen = 456; // length in clock cycles of a single hblank
	const int vBlankLen = hblankLen * 10; // length in clock cycles of a vblank (vblank is 10 h-lines (hblanks))
	while (running)
	{
		renderFull();
		while (scanline != WINDOW_HEIGHT) // while still drawing the scanlines
		{
			drawScanline(); // draw the current scanline (hblank of course comes after this)
			while (cpu.getClockCycles() < hblankLen) // emulate hblank
			{
				handleEvents();
				cpu.emulateCycle(); // emulate the cycles through the hblank
			}
			cpu.resetClock(); // reset number of clock cycles
		}
		// full rendering of screen has completed (all scanlines drawn)
		// |-> emulate vblank
		cpu.wByte(IF, 0x1); // set vblank interrupt
		while (cpu.getClockCycles() < vBlankLen) // emulate vblank
		{
			scanline++; // keep incrementing the LY because many games check that for in the range of the vblank
			cpu.wByte(LY, scanline);
			handleEvents();
			cpu.emulateCycle();
		}
	}
}

void Gameboy::drawScanline()
{
	// blitsurface modifies the scanLineRect width and height so they need to be reset here
	scanLineRect.h = 1; // 1 pixel 
	scanLineRect.w = WINDOW_WIDTH;
	scanLineRect.y = scanline; 
	// copy the 1x160 px slice at coords(0, line)
	SDL_BlitSurface(fullScreenSurface, &scanLineRect, windowSurface, &scanLineRect);

	cpu.wByte(LY, scanline);
	if (cpu.rByte(LY) == cpu.rByte(LYC))
	{
		cpu.wByte(STAT, cpu.rByte(STAT) | b2);
	}
	else
	{
		cpu.wByte(STAT, cpu.rByte(STAT) & ~b2);
	}
	scanline++;
	if (scanline == WINDOW_HEIGHT)
	{
		// display
		SDL_UpdateWindowSurface(window);
	}
}

void Gameboy::drawPixel(SDL_Surface* dest, const char r, const char g, const char b, const unsigned x, const unsigned y)
{
	pixel.x = x;
	pixel.y = y;
	// draw the pixel to the screenBuffer
	SDL_FillRect(dest, &pixel, SDL_MapRGB(dest->format, r, g, b));
}

void clear(SDL_Surface* surf)
{
	// clear the surface to white
	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, WHITE));
}

void Gameboy::drawBGSlice(const byte b1, const byte b2, unsigned& x, unsigned& y)
{
	// the bits of the string are compared to create the color of each pixel
	// 1. A bit that is 0 in both bytes will be a WHITE pixel
	// 2. A bit that is 1 in the first byte and 0 in the second will be a GREY pixel
	// 3. A bit that is 0 in the first byte and 1 in the second will be a DARK GREY pixel
	// 4. A bit that is 1 in both bytes will be a BLACK pixel
	// https://slashbinbash.wordpress.com/2013/02/07/gameboy-tile-mapping-between-image-and-memory/
	for (int i = 0x80; i >= 1; i >>= 1)
	{
		int currBit0 = b1 & i; // get the value of the current bit
		int currBit1 = b2 & i;
		if (currBit0 && currBit1) // bit1 (on) and bit2 (on)
		{
			drawPixel(backgroundSurface, BLACK, x, y); // draw the pixel to the screenBuffer 
		}
		else if (!currBit0 && !currBit1) // bit1 (off) and bit2 (off)
		{
			drawPixel(backgroundSurface, WHITE, x, y); // draw the pixel to the screenBuffer 
		}
		else if (currBit0 && !currBit1) // bit1 (on) and bit2 (off)
		{
			drawPixel(backgroundSurface, LIGHT_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		else // bit1 (off) bit2 (on)
		{
			drawPixel(backgroundSurface, DARK_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		x++;
	}
	x -= 8;
	y++;
}

void Gameboy::drawSpriteSlice(const byte b1, const byte b2, unsigned& x, unsigned& y)
{
	// 1. A bit that is 0 in both bytes will be a WHITE pixel
	// 2. A bit that is 1 in the first byte and 0 in the second will be a GREY pixel
	// 3. A bit that is 0 in the first byte and 1 in the second will be a DARK GREY pixel
	// 4. A bit that is 1 in both bytes will be a BLACK pixel
	// https://slashbinbash.wordpress.com/2013/02/07/gameboy-tile-mapping-between-image-and-memory/
	for (int i = 0x80; i >= 1; i >>= 1)
	{
		int currBit0 = b1 & i; // get the value of the current bit
		int currBit1 = b2 & i;
		if (currBit0 && currBit1) // bit1 (on) and bit2 (on)
		{
			drawPixel(fullScreenSurface, BLACK, x, y); // draw the pixel to the screenBuffer 
		}
		else if (!currBit0 && !currBit1) // bit1 (off) and bit2 (off)
		{
			// b0 and b1 == 0 is clear for sprites
		}
		else if (currBit0 && !currBit1) // bit1 (on) and bit2 (off)
		{
			drawPixel(fullScreenSurface, LIGHT_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		else // bit1 (off) bit2 (on)
		{
			drawPixel(fullScreenSurface, DARK_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		x++;
	}
	x -= 8;
	y++;
}

void Gameboy::drawBG(const std::vector<byte>& mem)
{
	unsigned x = 0;
	unsigned y = 0;
	const int lcdc = cpu.rByte(LCDC);
	if (lcdc & 0x40 != 0x0) // 0 = bg0, 1 = bg1
	{
		// bg0, draw all of the 8x8 tiles
		for (int i = BG_MAP_0; i < BG_MAP_0_END; i++)
		{
			// draw the 8x8 tile
			// first get the location in memory of the tile
			addr16 chrLocStart;
			if (lcdc & 0x10 != 0x0) // unsigned characters
			{
				chrLocStart = static_cast<ubyte>(mem[i]) * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			}
			else // signed characters
			{
				chrLocStart = mem[i] * 0x10 + CHR_MAP_SIGNED; // no cast because they are signed
			}
			// draw the slice pixel by pixel
			for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2) // note the += 2, 2 bytes per slice
			{
				drawBGSlice(mem[j], mem[j + 1], x, y); // draw the pixel of the slice
			}
			x += 8;
			y -= 8;
			if (x == SCR_BUFFER_WIDTH) // "hblank" (kind of) - at the end of the horiziontal screen, 
			{						   // move the x back to 0 and move y down 8 
				y += 8;
				x = 0;
			}
		}
	}
	else // bg1 
	{
		for (int i = BG_MAP_1; i < BG_MAP_1_END; i++)
		{
			// draw the 8x8 tile
			addr16 chrLocStart;
			if (lcdc & 0x10 != 0x0) // unsigned characters
			{
				chrLocStart = static_cast<ubyte>(mem[i]) * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			}
			else // signed characters
			{
				chrLocStart = static_cast<ubyte>(mem[i]) * 0x10 + CHR_MAP_SIGNED; // get the location of the first tile slice in memory
			}
			for (int i = chrLocStart; i < chrLocStart + 0x10; i += 2) // note the += 2
			{
				drawBGSlice(mem[i], mem[i + 1], x, y); // draw the slice
			}
			x += 8;
			y -= 8;
			if (x == SCR_BUFFER_WIDTH)
			{
				y += 8;
				x = 0;
			}
		}
	}
}

void Gameboy::drawSprites(const std::vector<byte>& mem)
{
	const byte lcdc = cpu.rByte(LCDC);
	// sprite size: 1 = 8x16, 0 = 8x8
	if ((lcdc & b2) != 0x0)  // 8x16 wxh, 2 8x8 sprites stacked on top of each other
	{
		for (int i = OAM; i < OAM_END; i += 4)
		{
			/* sprite are offset on the Gameboy hardware by (-8, -16) 
			 * so a sprite at (0, 0) is offscreen and actually at (-8, -16)
			 */
			unsigned int y = (mem[i] & 0xFF) - 16; // emulate that offset here
			unsigned int x = (mem[i + 1] & 0xFF) - 8; // ... 
			// sprites are always unsigned
			// draw the upper 8x8 tile
			addr16 chrLocStartUp = static_cast<ubyte>(mem[i + 2]) * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			for (int j = chrLocStartUp; j < chrLocStartUp + 0x10; j += 2)
			{
				drawSpriteSlice(mem[j], mem[j + 1], x, y);
			}
			// draw the lower 8x8 tile
			addr16 chrLocStartLow = chrLocStartUp + 0x10;
			for (int j = chrLocStartLow; j < chrLocStartLow + 0x10; j += 2)
			{
				drawSpriteSlice(mem[j], mem[j + 1], x, y);
			}
		}

	}
	else // 8x8
	{
		for (int i = OAM; i < OAM_END; i += 4)
		{
			unsigned int y = (mem[i] & 0xFF) - 16;
			unsigned int x = (mem[i + 1] & 0xFF) - 8;
			// sprites are always unsigned
			addr16 chrLocStart = static_cast<ubyte>(mem[i + 2]) * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2)
			{
				drawSpriteSlice(mem[j], mem[j + 1], x, y);
			}
		}
	}
}

void Gameboy::renderFull()
{
	// clear everything
	clear(backgroundSurface);
	clear(fullScreenSurface);
	const byte lcdc = cpu.rByte(LCDC); // get the current state of the lcd status register
 	if ((lcdc & b7) != 0x0) // LCD is enabled, do drawing
	{
		// get a dump of the cpu's memory (gfx data is stored in this memory)
		const std::vector<byte> mem = *cpu.dumpMem();
		// draw background first
		if ((lcdc & 0x1) != 0x0) // draw background?
		{
			drawBG(mem);
		}
		// get the scroll x and y positions
		srcSurfaceRect.x = static_cast<ubyte>(cpu.rByte(SCX));
		srcSurfaceRect.y = static_cast<ubyte>(cpu.rByte(SCY));

		/* emulate background wrapping in the worst way possible
		   if the scroll reaches the end of the background, scroll it back to the beginning
		*/
		if (srcSurfaceRect.x >= SCR_BUFFER_WIDTH - WINDOW_WIDTH) // if the scroll reaches the end of the background
		{
			int mod = srcSurfaceRect.x / (SCR_BUFFER_WIDTH - WINDOW_WIDTH); // get the multiplication modifier (when SCX >= 192 the * 2 adjusts down)
			srcSurfaceRect.x =  srcSurfaceRect.x - (SCR_BUFFER_WIDTH - WINDOW_WIDTH) * mod; // scroll it back (x - 96) 96 is when the window reaches the end of the buffer horizontally
		}
		if (srcSurfaceRect.y >= SCR_BUFFER_HEIGHT - WINDOW_HEIGHT)
		{
			int mod = srcSurfaceRect.y / (SCR_BUFFER_HEIGHT - WINDOW_HEIGHT); // get the multiplication modifier (when SCX >= 192 the * 2 adjusts down)
			srcSurfaceRect.y = srcSurfaceRect.y - (SCR_BUFFER_HEIGHT - WINDOW_HEIGHT) * mod; // scroll it back (y - 112) 112 is when the window reaches the end of the buffer vertically
		}
		// copy the screen buffer (the background) to the actual screen
		SDL_BlitSurface(backgroundSurface, &srcSurfaceRect, fullScreenSurface, NULL);
		// draw sprites on top of background 
		if ((lcdc & b1) != 0x0) // draw sprites?
		{
			drawSprites(mem);
		}
		//SDL_BlitSurface(backgroundSurface, &srcSurfaceRect, fullScreenSurface, NULL);
		// reset the LY and current scanline
		scanline = 0;
		cpu.wByte(LY, scanline);
	}
}

void Gameboy::halt()
{
	while (!cpu.rByte(IE)) // wait for interrupt
	{
		// run everything except for emulation of cpu cycles
		renderFull(); // basically just render so the vblank interrupt can be set
		handleEvents();
	}
	cpu.unHalt();
}

void Gameboy::stop()
{
	while (!handleEvents())
	{
#ifdef DEBUG
		std::cout << "Stopped" << std::endl; // for debugging 
#endif // DEBUG
	}
}

bool Gameboy::handleEvents()
{
	SDL_Event e;
	int eventR;
	bool validKeyPressed = false; // is a valid (Gameboy) key pressed?
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			running = false;
		}
		if (e.type == SDL_KEYDOWN)
		{
			SDL_Keycode key = e.key.keysym.sym;
#ifdef DEBUG
			if (key == SDLK_9)
			{
				__T = true;
			}
			if (key == SDLK_ESCAPE)
			{
				running = false;
			}
			if (key == SDLK_1)
			{
				cpu._test = true;
			}
			if (key == SDLK_2)
			{
				cpu._test = false;
			}
			if (key == SDLK_g)
			{
				static int numChecks = 0;
				std::cout << "still running: " << numChecks << std::endl;
				numChecks++;
			}
#endif
			if (key == SDLK_UP) // up
			{
				cpu.keyInfo.keys[p14] &= ~keyUp;
				validKeyPressed = true;
			}
			if (key == SDLK_DOWN) // down
			{
				cpu.keyInfo.keys[p14] &= ~keyDown;
				validKeyPressed = true ;
			}
			if (key == SDLK_LEFT) // left
			{
				cpu.keyInfo.keys[p14] &= ~keyLeft;
				validKeyPressed = true;
			}
			if (key == SDLK_RIGHT) // right
			{
				cpu.keyInfo.keys[p14] &= ~keyRight;
				validKeyPressed = true;
			}
			if (key == SDLK_z) // a
			{
				cpu.keyInfo.keys[p15] &= ~keyA;
				validKeyPressed = true;
			}
			if (key == SDLK_x) // b
			{
				cpu.keyInfo.keys[p15] &= ~keyB;
				validKeyPressed = true;
			}
			if (key == SDLK_RETURN) // start
			{
				cpu.keyInfo.keys[p15] &= ~keyStart;
				validKeyPressed = true;
			}
			if (key == SDLK_BACKSPACE) // select
			{
				cpu.keyInfo.keys[p15] &= ~keySelect;
				validKeyPressed = true;
			}
		}
		else if (e.type == SDL_KEYUP)
		{
			SDL_Keycode key = e.key.keysym.sym;
			if (key == SDLK_UP) // up
			{
				cpu.keyInfo.keys[p14] |= keyUp;
			}
			if (key == SDLK_DOWN) // down
			{
				cpu.keyInfo.keys[p14] |= keyDown;
			}
			if (key == SDLK_LEFT) // left
			{
				cpu.keyInfo.keys[p14] |= keyLeft;
			}
			if (key == SDLK_RIGHT) // right
			{
				cpu.keyInfo.keys[p14] |= keyRight;
			}
			if (key == SDLK_z) // a
			{
				cpu.keyInfo.keys[p15] |= keyA;
			}
			if (key == SDLK_x) // b
			{
				cpu.keyInfo.keys[p15] |= keyB;
			}
			if (key == SDLK_RETURN) // start
			{
				cpu.keyInfo.keys[p15] |= keyStart;
			}
			if (key == SDLK_BACKSPACE) // select
			{
				cpu.keyInfo.keys[p15] |= keySelect;
			}
		}
	}
	return validKeyPressed;
}
