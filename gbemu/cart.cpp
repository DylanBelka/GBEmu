#include "cart.h"

void Cart::init(const char* ROMstr, int filesize)
{
	setupMBC(ROMstr);
	initROM(ROMstr, filesize);
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

bool isBankedROM(const addr16 addr) 
{
	if (addr >= ROM_BANK_0 && addr <= ROM_BANK_0_END ||
		addr <= ROM_BANK_0)
	{
		return false;
	}
	return true;
}

bool isCartRAM(const addr16 addr)
{
	if (!isInternalMem(addr) && addr >= 0xA000 && addr <= 0xCFFF)
	{
		return true;
	}
	return false;
}

const byte Cart::rByte(addr16 addr) const
{
	if (isCartROM(addr))
	{
		if (isBankedROM(addr))
		{
			const addr16 cpy = addr;
			addr -= banksize; // calculate the actual address in the banked rom
			return bankedROM[currentrombank][addr];
		}
		else
		{
			return fixedrom[addr];
		}
	}
	else
	{
		std::cout << "reading from ram" << std::endl;
		std::cout << "addr = " << toHex(addr) << std::endl;
		system("pause");
		return ram[addr];
	}
}

void Cart::wByte(const addr16 addr, byte val)
{
	if (isCartROM(addr))
	{
		switch (type)
		{
			case ROM_MBC1:
			case ROM_MBC1_RAM:
			case ROM_MBC1_RAM_BATT:
			{
				if (addr >= 0x6000 && addr <= 0x7FFF)
				{
					// switch memory mode
					std::cout << "switching memory mode" << std::endl;
					if (val == 0x00)
					{
						memmode = memmodes::_16_8;
					}
					else
					{
						memmode = memmodes::_4_32;
					}
				}
				if (addr >= 0x2000 && addr <= 0x3FFF)
				{
					currentrombank = val & 0xFF;
					if ((val & 0xFF) == 0 || (val & 0xFF) == 1)
					{
						currentrombank = 1;
					}
					std::cout << "Switched to rom bank #" << currentrombank << std::endl;
				}
				if (memmode == memmodes::_4_32)
				{
					if (addr >= 0x4000 && addr <= 0x5FFF)
					{
						currentrambank = val;
					}
					if (addr >= 0x0000 && addr <= 0x1FFF)
					{
						if ((val & 0xA) == 0xA)
						{
							rambankenabled = true;
						}
						else
						{
							rambankenabled = false;
						}
					}
				}
				else if (memmode == memmodes::_16_8)
				{
					if (addr >= 0x4000 && addr <= 0x5FFF)
					{
						std::cout << "setting ROM address lines with: " << toHex(val) << std::endl;
						system("pause");
					}
				}
				break;
			}
		}

		//std::cout << "bank switching" << std::endl;
		//std::cout << "writing value: " << toHex(val) << "\tto " << toHex(addr) << std::endl;
		//system("pause");
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
	ramsize = ROMstr[CART_RAM_SIZE];

	std::cout << romsize << std::endl;
	std::cout << ramsize << std::endl;
}

void Cart::initROM(const char* ROMstr, int filesize)
{
	fixedrom.resize(banksize);
	currentrombank = 0;
	const char sizeinfo = ROMstr[CART_ROM_SIZE];
	int numBanks = 0;

	switch (sizeinfo)
	{
		case 0x00:
			numBanks = 1;
			break;
		case 0x01:
			numBanks = 3;
			break;
		case 0x02: 
			numBanks = 7;
			break;
		case 0x03:
			numBanks = 15;
			break;
		case 0x04:
			numBanks = 31;
			break;
		case 0x05:
			numBanks = 63;
			break;
		case 0x06:
			numBanks = 127;
			break;
		case 0x52:
			numBanks = 71;
			break;
		case 0x53:
			numBanks = 79;
			break;
		case 0x54:
			numBanks = 95;
			break;
		default:
			std::cout << "unknown bank #: " << toHex(sizeinfo) << std::endl;
			system("pause");
			break;
	}
	bankedROM.resize(numBanks);
	int rompos = 0;
	int filepos = 0;
	for (rompos = 0; rompos < banksize; rompos++, filepos++)
	{
		fixedrom[rompos] = ROMstr[filepos];
	}

	for (int i = 0; i < numBanks; i++) // 1 bank is fixed
	{
		for (rompos = 0; rompos < banksize; rompos++, filepos++)
		{
			bankedROM[i][rompos] = ROMstr[filepos];
			currentrombank = 0;
		}
	}
}

void Cart::initRAM()
{
	switch (ramsize)
	{
		case 0x00:
			bankedRAM.resize(0);
			break;
		case 0x01:
			bankedRAM.resize(1);
			bankedRAM[0].resize(KBitToByte(16));
			break;
		case 0x02:
			bankedRAM.resize(1);
			bankedRAM[0].resize(KBitToByte(64));
			break;
		case 0x03:
		{
			bankedRAM.resize(4);
			for (int i = 0; i < 4; i++)
			{
				bankedRAM[i].resize(KBitToByte(256 / 4));
			}
			break;
		}
		case 0x04:
		{
			bankedRAM.resize(16);
			for (int i = 0; i < 16; i++)
			{
				bankedRAM[i].resize(1024 / 16);
			}
			break;
		}
	}
}

int getROMSize(const char size)
{
	switch (size)
	{
		case 0x00: // 2 banks
			return KBitToByte(256);
			break;
		case 0x01: // 4 banks
			return KBitToByte(512);
			break;
		case 0x02: // 8 banks
			return MBitToByte(1);
			break;
		case 0x03: // 16 banks
			return MBitToByte(2);
			break;
		case 0x04: // 32 banks
			return MBitToByte(4);
			break;
		case 0x05: // 64 banks
			return MBitToByte(8);
			break;
		case 0x06: // 128 banks
			return MBitToByte(16);
			break;
		case 0x52: // 72 banks
			return MBitToByte(9);
			break;
		case 0x53: // 80 banks
			return MBitToByte(10);
			break;
		case 0x54: // 96 banks
			return MBitToByte(12);
			break;
		default:
			std::cout << "unknown ROM size: " << toHex(size) << std::endl;
			system("pause");
			break;
	}
}

int getRAMSize(const char size)
{
	switch (size)
	{
		case 0x00: // 0 banks
			return KBitToByte(0);
			break;
		case 0x01: // 1 bank
			return KBitToByte(16);
			break;
		case 0x02: // 1 bank
			return KBitToByte(64);
			break;
		case 0x03: // 4 banks
			return KBitToByte(256);
			break;
		case 0x04: // 16 banks
			return MBitToByte(1);
			break;
		default:
			std::cout << "unknown RAM size: " << toHex(size) << std::endl;
			system("pause");
			break;
	}
}