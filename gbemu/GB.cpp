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

void GB::run()
{
	while (running)
	{
		draw();
		handleEvents();
		cpu.emulateCycle();
	}
}

void GB::drawPixel(const char color, const unsigned x, const unsigned y)
{
	SDL_Rect pix; // create a 1x1 rect with coords (x, y)
	pix.h = 1;
	pix.w = 1;
	pix.x = x;
	pix.y = y;
	// draw the pixel to the screensurface
	SDL_FillRect(screenBuffer, &pix, SDL_MapRGB(screenBuffer->format, color, color, color));
}

void GB::clear()
{
	SDL_FillRect(screenBuffer, NULL, SDL_MapRGB(screenBuffer->format, 0xFF, 0xFF, 0xFF));
}

const std::string toBin(char val)
{
	std::string bin = std::bitset<8>(val).to_string(); // string conversion
	//if (bin != "00000000")
	//{
	//	std::cout << bin << std::endl;
	//}
	return bin;
}

static unsigned x = 0;
static unsigned y = 0;

void GB::drawBGSlice(unsigned char b1, unsigned char b2)
{
	std::string bin1 = toBin(b1);
	std::string bin2 = toBin(b2);
	for (int i = 0; i < bin1.size(); i++)
	{
		unsigned char color = WHITE;
		if (bin1[i] == '1' && bin2[i] == '0')
		{
			color = LIGHT_GREY;
		}
		else if (bin1[i] == '0' && bin2[i] == '1')
		{
			color = DARK_GREY;
		}
		else if (bin1[i] == '1' && bin2[i] == '1')
		{
			color = BLACK;
		}
		// else color = white
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
	const char lcdc = cpu.getLCDC();
	if (lcdc & 0x80) // LCD is enabled, do drawing
	{
		char* mem = cpu.dumpMem();
		// draw background first
		if (!(lcdc & 0x40)) // 0 = bg0, 1 = bg1
		{
			// bg0, draw all of the 8x8 tiles
			for (int i = BG_MAP_0; i < BG_MAP_0_END; i++)
			{
				// draw the 8x8 tile
				unsigned short chrLocStart = (unsigned char)mem[i] * 0x10 + CHR_MAP; // get the location of the first tile slice in memory
				for (int i = chrLocStart; i < chrLocStart + 0x10; i += 2) // note the += 2
				{
					drawBGSlice(mem[i], mem[i + 1]); // draw the slice
				}
				//std::cout << std::endl;
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
					drawBGSlice(mem[i], mem[i + 1]); // draw the slice
				}
				//std::cout << std::endl;
				x += 8;
				y -= 8;
				if (x == WIDTH)
				{
					y += 16;
					x = 50;
				}
			}
		}
		mem[LY] = 0x91;
		cpu.cpyMem(mem);
		x = 0;
		y = 0;
		// draw rest of display here

		// copy the screen buffer from scroll positions x, y to display screen
		srcSurfaceRect.x = mem[SCX]; 
		srcSurfaceRect.y = mem[SCY];
		srcSurfaceRect.h = HEIGHT;
		srcSurfaceRect.w = WIDTH;
		if (SDL_BlitSurface(screenBuffer, &srcSurfaceRect, screenSurface, NULL) != 0)
		{
			std::cout << "blit failed " << SDL_GetError() << std::endl;
		}
	}
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