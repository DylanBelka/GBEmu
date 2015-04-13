#include <iostream>
#include <SDL.h>


#include "GB.h"
#include "cpu.h"

//#define DEBUG_CPU

#define DEBUG_GFX

#define ROM_LOAD_FAIL 1
#define BAD_ARGS 2

int main(int argc, char **argv)
{
#ifdef DEBUG_CPU
	CPU cpu;
	cpu.test();
	std::cin.ignore();
#endif
	GB gb;
	if (argc == 2)
	{
		if (!gb.init(argv[1]))
		{
			return ROM_LOAD_FAIL;
		}
	}
	else
	{
#ifdef DEBUG_GFX
		gb.init("hello.gb");
#else
		std::cout << "Usage: gbemu <rom file>" << std::endl;
		return BAD_ARGS;
#endif
	}
	gb.run();
	return 0;
}
