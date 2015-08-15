#include "cart.h"

void Cart::init(const char* ROMstr, int filesize)
{
	setupMBC(ROMstr);
	loadROM(ROMstr, filesize);
	initRAM();
}

void Cart::setupMBC(const char* ROMstr)
{
	const char* romTitle = &ROMstr[TITLE];
	std::cout << "ROM <" << romTitle << "> loaded succesfuly" << std::endl;
	std::cout << "Cart type: " << toHex(ROMstr[CART_TYPE]) << std::endl;
	std::cout << "Cart ROM size: " << toHex(ROMstr[CART_ROM_SIZE]) << std::endl;
	std::cout << "Cart RAM size: " << toHex(ROMstr[CART_RAM_SIZE]) << std::endl;

	type = ROMstr[CART_TYPE];
	isGBC = ROMstr[SGB_COMPAT];
	isGBC = ROMstr[GBC_COMPAT];
	
	romsize = getROMSize(ROMstr[CART_ROM_SIZE]);
	ramsize = getRAMSize(ROMstr[CART_RAM_SIZE]);

	std::cout << romsize << std::endl;
	std::cout << ramsize << std::endl;
}

void Cart::loadROM(const char* ROMstr, int filesize)
{
	switch (type)
	{
		case ROM_ONLY:
		{
			rom.resize(romsize);
			for (unsigned i = 0; i < filesize; i++)
			{
				rom[i] = ROMstr[i];
			}
			break;
		}
	}
}

void Cart::initRAM()
{

}

int getROMSize(const char size)
{
	switch (size)
	{
		case 0x00: // 2 banks
		{
			return KBitToByte(256);
			break;
		}
		case 0x01: // 4 banks
		{
			return KBitToByte(512);
			break;
		}
		case 0x02: // 8 banks
		{
			return MBitToByte(1);
			break;
		}
		case 0x03: // 16 banks
		{
			return MBitToByte(2);
			break;
		}
		case 0x04: // 32 banks
		{
			return MBitToByte(4);
			break;
		}
		case 0x05: // 64 banks
		{
			return MBitToByte(8);
			break;
		}
		case 0x06: // 128 banks
		{
			return MBitToByte(16);
			break;
		}
		case 0x52: // 72 banks
		{
			return MBitToByte(9);
			break;
		}
		case 0x53: // 80 banks
		{
			return MBitToByte(10);
			break;
		}
		case 0x54: // 96 banks
		{
			return MBitToByte(12);
			break;
		}
	}
}

int getRAMSize(const char size)
{
	switch (size)
	{
		case 0x00: // 0 banks
		{
			return KBitToByte(0);
			break;
		}
		case 0x01: // 1 bank
		{
			return KBitToByte(16);
			break;
		}
		case 0x02: // 1 bank
		{
			return KBitToByte(64);
			break;
		}
		case 0x03: // 4 banks
		{
			return KBitToByte(256);
			break;
		}
		case 0x04: // 16 banks
		{
			return MBitToByte(1);
			break;
		}
	}
}