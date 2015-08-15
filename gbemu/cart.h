#ifndef GB_CART_H
#define GB_CART_H

#include <iostream>
#include <vector>
#include "memdefs.h"
#include "types.h"
#include <iomanip>
#include <string>
#include <sstream>

#include "toHex.h"

constexpr int KBitToByte(int kbit)
{
	return kbit * 128;
}

constexpr int MBitToByte(int mbit)
{
	return mbit * 131072;
}

class Cart
{
public:
	void init(const char* romStr, int filesize);

private:
	std::vector<byte> rom;
	std::vector<byte> ram;

	// cart info
	int isGBC;
	int isSBG;
	int romsize;
	int ramsize;
	int type;

	void loadROM(const char* ROMstr, int filesize);
	void initRAM();
	void setupMBC(const char* ROMstr);
};

int getROMSize(const char size);
int getRAMSize(const char size);

enum carttypes
{
	ROM_ONLY = 0x00,
	ROM_MBC1,
	ROM_MBC1_RAM,
	ROM_MBC1_RAM_BATT,
	ROM_MBC2 = 0x05,
	ROM_MBC2_BATT,
	ROM_RAM = 0x08,
	ROM_RAM_BATT,
	ROM_MM01 = 0x0B,
	ROM_MM01_SRAM,
	ROM_MM01_SRAM_BATT,
	ROM_MBC3_TIMER_BATT = 0x0F,
	ROM_MBC3_TIMER_RAM_BATT,
	ROM_MBC3,
	ROM_MBC3_RAM,
	ROM_MBC3_RAM_BATT,
	ROM_MBC5 = 0x19,
	ROM_MBC5_RAM,
	ROM_MBC5_RAM_BATT,
	ROM_MBC5_RUMBLE,
	ROM_MBC5_RUMBLE_SRAM,
	ROM_MBC5_RUMLE_SRAM_BATT,
	POCKET_CAMERA,
	BANDAI_TAMA5 = 0x1F,
	HUDSON_HUC_3 = 0xFE,
	HUDSON_HUC3_1
};

#endif // GB_CART_H