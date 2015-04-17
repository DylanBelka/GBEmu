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
	screenBuffer = SDL_CreateRGBSurface(NULL, 256, 256, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	srcSurfaceRect.h = HEIGHT;
	srcSurfaceRect.w = WIDTH;
	pixel.h = 1;
	pixel.w = 1;
}

GB::~GB()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool GB::init(const std::string& romName)
{
	return cpu.loadROM(romName);
}

// .0271 draws/ millisecond
void GB::run()
{
	while (running)
	{
		draw();
		handleEvents();
		cpu.emulateCycle();
		//std::cout << "Ticks: " << SDL_GetTicks() << std::endl; // 2251 - 2280
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
	SDL_FillRect(screenBuffer, NULL, SDL_MapRGB(screenBuffer->format, 0xFF, 0xFF, 0xFF));
}

inline const std::string toBin(char val)
{
	return binLut[(unsigned char)val];
}

void GB::drawSlice(unsigned char b1, unsigned char b2, unsigned& x, unsigned& y)
{
	const char* bin1 = binLut[(unsigned char)b1];
	const char* bin2 = binLut[(unsigned char)b2];
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
		drawPixel(color, x, y); // draw the actual pixel to the surface 
		x++;
	}
	x -= 8;
	y++;
}

// 1. A bit that is 0 in both bytes will be a WHITE pixel
// 2. A bit that is 1 in the first byte and 0 in the second will be a GREY pixel
// 3. A bit that is 0 in the first byte and 1 in the second will be a DARK GREY pixel
// 4. A bit that is 1 in both bytes will be a BLACK pixel
// https://slashbinbash.wordpress.com/2013/02/07/gameboy-tile-mapping-between-image-and-memory/

void GB::draw()
{
	clear();
	const char lcdc = cpu.getByte(LCDC);
	if (lcdc & 0x80) // LCD is enabled, do drawing
	{
		const char* mem = cpu.dumpMem();
		// draw background first
		if (lcdc & 0x1) // draw background?
		{
			unsigned x = 0;
			unsigned y = 0;
			if (!(lcdc & 0x40)) // 0 = bg0, 1 = bg1
			{
				// bg0, draw all of the 8x8 tiles
				for (int i = BG_MAP_0; i < BG_MAP_0_END; i++)
				{
					// draw the 8x8 tile
					unsigned short chrLocStart;
					if (lcdc & 0x10) // unsigned characters
					{
						chrLocStart = (unsigned char)mem[i] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
					}
					else // signed characters
					{
						chrLocStart = (unsigned char)mem[i] * 0x10 + CHR_MAP_SIGNED; // get the location of the first tile slice in memory
					}
					for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2) // note the += 2, 2 bytes per slice
					{
						drawSlice(mem[j], mem[j + 1], x, y); // draw the slice
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
			else // bg1
			{
				// bg1 ^^^ make sure to fix this when done with bg0
				for (int i = BG_MAP_1; i < BG_MAP_1_END; i++)
				{
					// draw the 8x8 tile
					unsigned short chrLocStart = (unsigned char)mem[i] * 0x10 + CHR_MAP; // get the location of the first tile slice in memory
					for (int i = chrLocStart; i < chrLocStart + 0x10; i += 2) // note the += 2
					{
						drawSlice(mem[i], mem[i + 1], x, y); // draw the slice
					}
					x += 8;
					y -= 8;
					if (x == WIDTH)
					{
						y += 16;
						x = 50;
					}
				}
			}
		}
		// draw sprites
		if (lcdc & 0x2) // draw sprites?
		{
			// sprite size: 1 = 8x16, 0 = 8x8
			if (lcdc & 0x4)  // 8x16 wxh
			{
				for (int i = OAM; i < OAM_END; i += 4)
				{
					unsigned y = mem[i] - 16;
					unsigned x = mem[i + 1] - 8;
					// draw the upper 8x8 tile
					unsigned short chrLocStartUp = (unsigned char)mem[i + 2] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
					for (int j = chrLocStartUp; j < chrLocStartUp + 0x10; j += 2)
					{
						drawSlice(mem[j], mem[j + 1], x, y);
					}
					// draw the lower 8x8 tile
					unsigned short chrLocStartLow = chrLocStartUp + 0x10;
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
					unsigned short chrLocStart = (unsigned char)mem[i + 2] * 0x10 + CHR_MAP_UNSIGNED; // get the location of the first tile slice in memory
					for (int j = chrLocStart; j < chrLocStart + 0x10; j += 2)
					{
						drawSlice(mem[j], mem[j + 1], x, y);
					}
				}
			}
		}
		cpu.setByte(LY, 0x91);
		// copy the screen buffer from scroll positions x, y to display screen
		srcSurfaceRect.x = mem[SCX];
		srcSurfaceRect.y = mem[SCY];
		SDL_BlitSurface(screenBuffer, &srcSurfaceRect, screenSurface, NULL);
	}
	cpu.setByte(IE, 0x1);
	//cpu.setByte(LY, 0x94);
	SDL_UpdateWindowSurface(window);
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
			if (e.key.keysym.sym == SDLK_1)
			{
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
			}
			if (e.key.keysym.sym == SDLK_2)
			{
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x55, 0x55, 0x55));
			}
			if (e.key.keysym.sym == SDLK_3)
			{
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xAA, 0xAA, 0xAA));
			}
			if (e.key.keysym.sym == SDLK_4)
			{
				SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0x00, 0x00));
			}
		}
		if (e.type == SDL_KEYUP)
		{
			//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x00, 0xFF, 0xFF));
		}
	}
}