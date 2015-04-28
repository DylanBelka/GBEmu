#include "GB.h"

GB::GB() :
cpu()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "SDL Error: " << SDL_GetError() << std::endl;
		exit(-2);
	}
	window = SDL_CreateWindow("gbemu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, NULL);
	if (window == nullptr)
	{
		std::cout << "SDL window could not be created. Error: " << SDL_GetError() << std::endl;
	}
	screenSurface = SDL_GetWindowSurface(window);
	SDL_PixelFormat* fmt = screenSurface->format;
	screenBuffer = SDL_CreateRGBSurface(NULL, 256 * MODIFIER, 256 * MODIFIER, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	srcSurfaceRect.h = HEIGHT * MODIFIER;
	srcSurfaceRect.w = WIDTH * MODIFIER;
	pixel.h = 1 * MODIFIER;
	pixel.w = 1 * MODIFIER;
}

GB::~GB()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool GB::init(const std::string& romName)
{
	return cpu.loadROM(romName);
	clear();
}

void GB::run()
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

void GB::drawPixel(const char color, const unsigned x, const unsigned y)
{
	pixel.x = x;
	pixel.y = y;
	// draw the pixel to the screenBuffer
	SDL_FillRect(screenBuffer, &pixel, SDL_MapRGB(screenBuffer->format, color, color, color));
}

void GB::clear()
{
	// clear the screen to white
	SDL_FillRect(screenBuffer, NULL, SDL_MapRGB(screenBuffer->format, 0xFF, 0xFF, 0xFF));
}

// get the binary strings of both bytes
void GB::drawSlice(const byte b1, const byte b2, unsigned& x, unsigned& y)
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
		unsigned char color = DARK_GREY;
		if (bin1[i] == '1' && bin2[i] == '1')
		{
			color = BLACK;
		}
		else if (bin1[i] == '0' && bin2[i] == '0')
		{
			color = WHITE;
		}
		else if (bin1[i] == '1' && bin2[i] == '0')
		{
			color = LIGHT_GREY;
		}
		//// else color = dark grey
		drawPixel(color, x, y); // draw the pixel to the screenBuffer 
		x++;
	}
	x -= 8;
	y++;
}

void GB::drawBG(const char* mem)
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
				drawSlice(mem[j], mem[j + 1], x, y); // draw the pixel of the slice
			}
			x += 8;
			y -= 8;
			if (x == WIDTH) // "hblank" (kind of) - at the end of the horiziontal screen, 
			{				// move the x back to 0 and move y down 8 (y -= 8 after each slice so y += 16 === y += 8)
				y += 16;
				x = 0;
			}
		}
	}
	else // bg1
	{
		// bg1 ^^^ make sure to fix this when done with bg0
		for (int i = BG_MAP_1; i < BG_MAP_1_END; i++)
		{
			// draw the 8x8 tile
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
				drawSlice(mem[i], mem[i + 1], x, y); // draw the slice
			}
			x += 8;
			y -= 8;
			if (x == WIDTH)
			{
				y += 16;
				x = 0;
			}
		}
	}
}

void GB::drawSprites(const byte* mem)
{
	const byte lcdc = cpu.getByte(LCDC);
	// sprite size: 1 = 8x16, 0 = 8x8
	if (lcdc & 0x4)  // 8x16 wxh ie 2 8x8 sprites stacked on top of each other
	{
		for (int i = OAM; i < OAM_END; i += 4)
		{
			unsigned y = mem[i] - 16; // sprite are offset on the GB hardware by (-8, -16) so a sprite at (0, 0) is offscreen and actually at (-8, -16)
			unsigned x = mem[i + 1] - 8; // emulate that offset here
			// sprites are always unsigned
			// draw the upper 8x8 tile
			addr16 chrLocStartUp = (ubyte)mem[i + 2] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
			for (int j = chrLocStartUp; j < chrLocStartUp + 0x10; j += 2)
			{
				drawSlice(mem[j], mem[j + 1], x, y);
			}
			// draw the lower 8x8 tile
			addr16 chrLocStartLow = chrLocStartUp + 0x10;
			for (int j = chrLocStartLow; j < chrLocStartLow + 0x10; j += 2)
			{
				drawSlice(mem[j], mem[j + 1], x, y);
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
				drawSlice(mem[j], mem[j + 1], x, y);
			}
		}
	}

}

void GB::draw()
{
	clear();
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
		// draw sprites
		if (lcdc & 0x2) // draw sprites?
		{
			drawSprites(mem); 
		}
		// copy the screen buffer to the screensurface from scroll positions (x, y) to display screen
		srcSurfaceRect.x = mem[SCX];
		srcSurfaceRect.y = mem[SCY];
		SDL_BlitSurface(screenBuffer, &srcSurfaceRect, screenSurface, NULL);
		SDL_UpdateWindowSurface(window);

		if (framesSinceLastVBlank >= framesBetweenVBlank)
		{
			framesSinceLastVBlank = 0;
			cpu.setByte(IF, 0x1); // set v-blank interrupt
		}
		framesSinceLastVBlank++;
		//// emulate through the v-blank interrupt (vblank interrupt starts at 0x40 and ends at 0x48)
		//for (int i = 0x40; i < 0x48; i++)
		//{
		//	cpu.emulateCycle();
		//}
		// emulate through v-blank: many games wait for a specific value of LY during v-blank before continuing execution
		for (int i = 0x90; i < 0x99; i++)
		{
			cpu.setByte(LY, i);
			cpu.emulateCycle();
		}
	}
}

void GB::handleEvents()
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
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				running = false;
			}
			if (e.key.keysym.sym == SDLK_1)
			{
				std::cout << e.key.keysym.sym << std::endl;
			}
		}
	}
}

void GB::halt()
{
	while (!cpu.getByte(IE)) // wait for interrupt
	{
		// run everything except for emulation of cpu cycles
		draw();
		handleEvents();
	}
	cpu.stopHalt();
}

void GB::stop()
{
	
}