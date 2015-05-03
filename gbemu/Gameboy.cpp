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
	screenSurface = SDL_GetWindowSurface(window);
	if (screenSurface == nullptr)
	{
		std::cout << "SDL_Surface <screenSurface> could not be created. Error: " << SDL_GetError() << std::endl;
		return;
	}
	SDL_PixelFormat* fmt = screenSurface->format;
	screenBuffer = SDL_CreateRGBSurface(NULL, SCR_BUFFER_WIDTH, SCR_BUFFER_HEIGHT, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (screenBuffer == nullptr)
	{
		std::cout << "SDL_Surface <screenBuffer> could not be created. Error: " << SDL_GetError() << std::endl;
	}
	srcSurfaceRect.w = SCR_BUFFER_WIDTH;
	srcSurfaceRect.h = SCR_BUFFER_HEIGHT;
	srcSurfaceRect.x = 0; 
	srcSurfaceRect.y = 0;
	pixel.h = 1;
	pixel.w = 1;
}

Gameboy::~Gameboy()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Gameboy::init(const std::string& romName)
{
	return cpu.loadROM(romName);
	clear(screenSurface);
	clear(screenBuffer);
}

void Gameboy::run()
{
	while (running)
	{
		draw();
		handleEvents();
		int ticks = SDL_GetTicks();
		while ((int)SDL_GetTicks() - ticks < 30)
		{
			cpu.emulateCycle();
		}
		if (cpu.isHalted())
		{
			halt();
		}
		if (cpu.isStopped())
		{
			stop();
		}
	}
}

void Gameboy::drawPixel(SDL_Surface* dest, const char color, const unsigned x, const unsigned y)
{
	pixel.x = x;
	pixel.y = y;
	// draw the pixel to the screenBuffer
	SDL_FillRect(dest, &pixel, SDL_MapRGB(dest->format, color, color, color));
}

void clear(SDL_Surface* surf)
{
	// clear the screen to white
	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
}

void Gameboy::drawBGSlice(const byte b1, const byte b2, unsigned& x, unsigned& y)
{
	// the bits of the string are compared to create the color of each pixel
	// 1. A bit that is 0 in both bytes will be a WHITE pixel
	// 2. A bit that is 1 in the first byte and 0 in the second will be a GREY pixel
	// 3. A bit that is 0 in the first byte and 1 in the second will be a DARK GREY pixel
	// 4. A bit that is 1 in both bytes will be a BLACK pixel
	// https://slashbinbash.wordpress.com/2013/02/07/gameboy-tile-mapping-between-image-and-memory/
	const char* bin1 = binLut[(ubyte)b1];
	const char* bin2 = binLut[(ubyte)b2];
	for (int i = 0; i < 8; i++)
	{
		if (bin1[i] == '1' && bin2[i] == '1')
		{
			drawPixel(screenBuffer, BLACK, x, y); // draw the pixel to the screenBuffer 
		}
		else if (bin1[i] == '0' && bin2[i] == '0')
		{
			drawPixel(screenBuffer, WHITE, x, y); // draw the pixel to the screenBuffer 
		}
		else if (bin1[i] == '1' && bin2[i] == '0')
		{
			drawPixel(screenBuffer, LIGHT_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		else
		{
			drawPixel(screenBuffer, DARK_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		x++;
	}
	x -= 8;
	y++;
}

void Gameboy::drawSpriteSlice(const byte b1, const byte b2, unsigned& x, unsigned& y)
{
	// the bits of the string are compared to create the color of each pixel
	// 1. A bit that is 0 in both bytes will be a WHITE pixel
	// 2. A bit that is 1 in the first byte and 0 in the second will be a GREY pixel
	// 3. A bit that is 0 in the first byte and 1 in the second will be a DARK GREY pixel
	// 4. A bit that is 1 in both bytes will be a BLACK pixel
	// https://slashbinbash.wordpress.com/2013/02/07/gameboy-tile-mapping-between-image-and-memory/
	const char* bin1 = binLut[(ubyte)b1];
	const char* bin2 = binLut[(ubyte)b2];
	for (int i = 0; i < 8; i++)
	{
		if (bin1[i] == '1' && bin2[i] == '1')
		{
			drawPixel(screenSurface, BLACK, x, y); // draw the pixel to the screenBuffer 
		}
		else if (bin1[i] == '0' && bin2[i] == '0')
		{
			// dont draw white
		}
		else if (bin1[i] == '1' && bin2[i] == '0')
		{
			drawPixel(screenSurface, LIGHT_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		else
		{
			drawPixel(screenSurface, DARK_GREY, x, y); // draw the pixel to the screenBuffer 
		}
		x++;
	}
	x -= 8;
	y++;
}

void Gameboy::drawBG(const char* mem)
{
	unsigned x = 0;
	unsigned y = 0;
	const byte lcdc = cpu.getByte(LCDC);
	if (!(lcdc & 0x40)) // 0 = bg0, 1 = bg1
	{
		// bg0, draw all of the 8x8 tiles
		for (int i = BG_MAP_0; i < BG_MAP_0_END; i++)
		{
			// draw the 8x8 tile
			// first get the location in memory of the tile
			addr16 chrLocStart;
			if (lcdc & 0x10) // unsigned characters
			{
				chrLocStart = (ubyte)mem[i] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			}
			else // signed characters
			{
				chrLocStart = (ubyte)mem[i] * 0x10 + CHR_MAP_SIGNED; // get the location of the first tile slice in memory
			}
			// draw the slice pixel by pixel
			for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2) // note the += 2, 2 bytes per slice
			{
				drawBGSlice(mem[j], mem[j + 1], x, y); // draw the pixel of the slice
			}
			x += 8;
			y -= 8;
			if (x == SCR_BUFFER_WIDTH) // "hblank" (kind of) - at the end of the horiziontal screen, 
			{						// move the x back to 0 and move y down 8 (y -= 8 after each slice so y += 16 === y += 8)
				y += 8;
				x = 0;
			}
		}
	}
	else // bg1 ^^^ fix this
	{
		for (int i = BG_MAP_1; i < BG_MAP_1_END; i++)
		{
			// draw the 8x8 tile
			addr16 chrLocStart;
			if (lcdc & 0x10) // unsigned characters
			{
				chrLocStart = (ubyte)mem[i] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			}
			else // signed characters
			{
				chrLocStart = (ubyte)mem[i] * 0x10 + CHR_MAP_SIGNED; // get the location of the first tile slice in memory
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

void Gameboy::drawSprites(const byte* mem)
{
	const byte lcdc = cpu.getByte(LCDC);
	// sprite size: 1 = 8x16, 0 = 8x8
	if (lcdc & 0x4)  // 8x16 wxh ie 2 8x8 sprites stacked on top of each other
	{
		for (int i = OAM; i < OAM_END; i += 4)
		{
			unsigned y = mem[i] - 16; // sprite are offset on the Gameboy hardware by (-8, -16) so a sprite at (0, 0) is offscreen and actually at (-8, -16)
			unsigned x = mem[i + 1] - 8; // emulate that offset here
			// sprites are always unsigned
			// draw the upper 8x8 tile
			addr16 chrLocStartUp = (ubyte)mem[i + 2] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
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
			unsigned y = mem[i] - 16;
			unsigned x = mem[i + 1] - 8;
			// sprites are always unsigned
			addr16 chrLocStart = (ubyte)mem[i + 2] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2)
			{
				drawSpriteSlice(mem[j], mem[j + 1], x, y);
			}
		}
	}

}

void Gameboy::draw()
{
	clear(screenSurface);
	clear(screenBuffer);
	const byte lcdc = cpu.getByte(LCDC);
	if (lcdc & 0x80) // LCD is enabled, do drawing
	{
		// get a dump of the cpu's memory (gfx data is stored in this memory)
		const byte* mem = cpu.dumpMem();
		// draw background first
		if (lcdc & 0x1) // draw background?
		{
			drawBG(mem);
		}
		// copy the screen buffer to the screensurface from scroll positions (SCX, SCYs)
		srcSurfaceRect.x = mem[SCX];
		srcSurfaceRect.y = mem[SCY];
		SDL_BlitSurface(screenBuffer, NULL, screenSurface, &srcSurfaceRect);
		// draw sprites
		if (lcdc & 0x2) // draw sprites?
		{
			drawSprites(mem); 
		}
		// display
		SDL_UpdateWindowSurface(window);

		// vblank every framesBetweenVBlank (30)
		if (framesSinceLastVBlank >= framesBetweenVBlank)
		{
			framesSinceLastVBlank = 0;
			cpu.setByte(IF, 0x1); // set v-blank interrupt
		}
		framesSinceLastVBlank++;

		// emulate through v-blank: many games wait for a specific value of LY during v-blank before continuing execution
		for (int i = 0x90; i < 0x99; i++)
		{
			cpu.setByte(LY, i);
			cpu.emulateCycle();
		}
	}
}

bool Gameboy::handleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			running = false;
		}
		if (e.type == SDL_KEYDOWN)
		{
			SDL_Keycode key = e.key.keysym.sym;
			const byte keyPort = cpu.getByte(JOYPAD); // get the current key group
			if (key == SDLK_ESCAPE)
			{
				running = false;
			}
			else if (keyPort & 0x10) // group P14
			{
				if (key == SDLK_UP) // up
				{
					cpu.setByte(JOYPAD, keyPort | b2);
					return true;
				}
				if (key == SDLK_DOWN) // down
				{
					cpu.setByte(JOYPAD, keyPort | b1);
					return true;
				}
				if (key == SDLK_LEFT) // left
				{
					cpu.setByte(JOYPAD, keyPort | b1);
					return true;
				}
				if (key == SDLK_RIGHT) // right
				{
					cpu.setByte(JOYPAD, keyPort | b0);
					return true;
				}

			}
			else if (keyPort & 0x20) // group P15
			{
				if (key == SDLK_a) // a
				{
					cpu.setByte(JOYPAD, keyPort | b0);
					return true;
				}
				if (key == SDLK_s) // b
				{
					cpu.setByte(JOYPAD, keyPort | b1);
					return true;
				}
				if (key == SDLK_x) // start
				{
					cpu.setByte(JOYPAD, keyPort | b1);
					return true;
				}
				if (key == SDLK_z) // select
				{
					cpu.setByte(JOYPAD, keyPort | b2);
					return true;
				}
			}
			else
			{
				cpu.setByte(JOYPAD, 0x0);
			}
		}
	}
	return false;
}

void Gameboy::halt()
{
	while (!cpu.getByte(IE)) // wait for interrupt
	{
		// run everything except for emulation of cpu cycles
		draw();
		handleEvents();
	}
	cpu.stopHalt();
}

void Gameboy::stop()
{
	while (!handleEvents()) { draw(); }
}