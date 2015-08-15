#include "cart.h"

void Cart::init(const char* ROMstr, int filesize)
{
	setupMBC(ROMstr);
	loadROM(ROMstr, filesize);
	initRAM();
}

inline bool isInternalMem(const addr16 addr)
{
	return (addr >= INTERNAL_MEM);
}

bool isCartROM(const addr16 addr)
{
	if (!isInternalMem(addr) && addr >= ROM_BANK_N && addr <= ROM_BANK_N_END ||
		addr >= ROM_BANK_0 && addr <= ROM_BANK_0_END ||
		addr <= ROM_BANK_0)
	{
		return true;
	}
	return false;
}

bool isCartRAM(const addr16 addr)
{
	if (!isInternalMem(addr) && addr >= 0xA000 && addr <= 0xCFFF)
	{
		return true;
	}
	return false;
}

const byte Cart::rByte(const addr16 addr) const
{
	//std::cout << "rByte addr = " << toHex(addr) << std::endl;
	if (isCartROM(addr))
	{
		return rom[addr];
	}
	else
	{
		return ram[addr];
	}
}

void Cart::wByte(const addr16 addr, byte val)
{
	if (isCartROM(addr))
	{
		// TODO: bank switching done here
		std::cout << "bank switching" << std::endl;
		system("pause");
	}
	else
	{
		ram[addr] = val;
	}
}

void Cart::wWord(const addr16 addr, word val)
{
	if (isCartRAM(addr))
	{
		ram[addr] = val & 0x00FF; // lower byte
		ram[addr + 1] = ((val & 0xFF00) >> 8) & 0xFF; // upper byte
	}
	else
	{
		std::cout << "attempting to write word to cart ROM" << std::endl;
		std::cout << "addr = " << toHex(addr) << "\tval = " << toHex(val) << std::endl;
		system("pause");
	}
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
	rom.resize(romsize);
	for (unsigned i = 0; i < filesize; i++)
	{
		rom[i] = ROMstr[i];
	}
}

void Cart::initRAM()
{
	ram.resize(ramsize);
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