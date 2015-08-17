#ifndef GB_CART_H
#define GB_CART_H

#include <iostream>
#include <vector>
#include <array>

#include "memdefs.h"
#include "types.h"
#include <string>

#include "toHex.h"

constexpr int KBitToByte(int kbit)
{
	return kbit * 128;
}

constexpr int MBitToByte(int mbit)
{
	return mbit * 131072;
}

const int banksize = 0x4000; // bytes

class Cart
{
public:
	void init(const char* romStr, int filesize);
	const byte rByte(addr16 addr) const;
	void wByte(const addr16 addr, byte val);

	void wWord(const addr16 addr, word val);

private:
	std::vector<byte> fixedROM;

	std::vector<std::array<byte, banksize>> bankedROM;
	int currentROMBank;
	byte upperROMBankBits;

	std::vector<std::vector<byte>> bankedRAM;
	int currentRAMBank;
	bool RAMBankEnabled = false;
	int memMode;

	// cart info
	int isGBC;
	int isSBG;
	int romsize;
	int ramsize;
	int type;

	void initROM(const char* ROMstr, int filesize);
	void initRAM();
	void setupMBC(const char* ROMstr);
};

int getROMSize(const char size);
int getRAMSize(const char size);

bool isCartRAM(const addr16 addr);
inline bool isInternalMem(const addr16 addr);
bool isCartROM(const addr16 addr);
bool isBankedROM(const addr16 addr);

enum memmodes
{
	ROM_BANK = 0,
	RAM_BANK,
};

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