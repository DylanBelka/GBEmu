#include <iostream>
#include <SDL.h>

#include "Gameboy.h"
#include "cpu.h"

//#define DEBUG_CPU

#define DEBUG_GFX

#define ROM_LOAD_FAIL 1
#define BAD_ARGS 2
#define ROM_TOO_BIG 3
#define MALLOC_FAIL 4
#define DEFAULT_ERROR -1


int main(int argc, char **argv)
{
#ifdef DEBUG_CPU
	CPU cpu;
	cpu.test();
	std::cin.ignore();
#endif
	Gameboy gb;
	if (argc == 2)
	{
		const int gbLoadStatus = gb.init(argv[1]);
		if (gbLoadStatus != EXIT_SUCCESS) // something went wrong find out what
		{
			if (gbLoadStatus == 1) // ROM load failure
			{
				std::cout << "ROM <" << argv[1] << "> not failed to load" << std::endl;
				return ROM_LOAD_FAIL;
			}
			else if (gbLoadStatus == 2) // ROM too big
			{
				std::cout << "ROM <" << argv[1] << "> too large - memory mappers not supported by this emulator" << std::endl;
				return ROM_TOO_BIG;
			}
			else if (gbLoadStatus == 3) // malloc failure
			{
				std::cout << "Failed to allocate memory for ROM file (new/malloc returned nullptr) - nothing you can really do about this one" << std::endl;
				return MALLOC_FAIL;
			}
			else
			{
				std::cout << "Some unknown error happened" << std::endl;
				return DEFAULT_ERROR;
			}
		}
	}
	else 
	{
#ifdef DEBUG_GFX
		if (gb.init("apocnow.gb") != EXIT_SUCCESS)
		{
			std::cout << "rom not found" << std::endl;
			system("pause");
			return ROM_LOAD_FAIL;
		}
#else
		std::cout << "Usage: gbemu <rom file>" << std::endl;
		return BAD_ARGS;
#endif
	}
	gb.run();
	return 0;
}