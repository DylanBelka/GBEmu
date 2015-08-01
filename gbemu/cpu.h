#ifndef GB_CPU_H
#define GB_CPU_H

#define DEBUG

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <functional>

#include "memdefs.h"
#include "input.h"
#include "types.h"

#ifdef DEBUG
#include <SDL.h>
#endif // DEBUG

/**
Resources:
* http://clrhome.org/table/
* http://fms.komkon.org/GameBoy/Tech/Software.html
* http://gameboy.mongenel.com/dmg/asmmemmap.html
* http://tutorials.eeems.ca/ASMin28Days/lesson/day04.html
* http://www.zophar.net/fileuploads/2/10807fvllz/z80-1.txt
* And many more
**/

#define MAX_ROM_SIZE 0xBFFF
#define MEM_SIZE 0xFFFF + 0x1 // addresses up to and including 0xFFFF

#define ADD true
#define SUB false

class CPU
{
public:
	CPU();
	~CPU();

	void emulateCycle();
	int loadROM(const std::string& fileName);
	void test();

	std::vector<byte>* dumpMem() { return &mem; }

// CPU status getting/ setting functions
public:
	void setByte(addr16 addr, byte val) { mem[addr] = val; }
	byte getByte(addr16 addr) { return mem[addr]; }

	bool isHalted()	 const { return halted; }
	bool isStopped() const { return stopped; }
	void unHalt() { halted = false; }
	void unStop() { stopped = false; }

	void resetClock() { clockCycles = 0; }
	uint16_t getClockCycles() const { return clockCycles; }

	GBKeys& getKeyInfo() { return keyInfo; }

	bool _test = false;
	
private:
	void reset();

// registers
private:
	// 8 bit registers
	reg A;
	reg B;
	reg C;
	reg D;
	reg E;
	reg H;
	reg L;

	ubyte F;		// flag register

	// decode flag register bits
	inline const bool zero()		const { return F & 0x40; }
	inline const bool half_carry()	const { return F & 0x10; }
	inline const bool N()			const { return F & 0x2; } // add or subtract
	inline const bool carry()		const { return F & 0x1; }

	// 16 bit "registers"
	inline reg16 AF() { return ((A << 8) | (F & 0xFF)); } // can remove the masking
	inline reg16 BC() { return ((B << 8) | (C & 0xFF)); }
	inline reg16 DE() { return ((D << 8) | (E & 0xFF)); }
	inline reg16 HL() { return ((H << 8) | (L & 0xFF)); }

	inline void AF(int16_t val) { A = ((val >> 0x8) & 0xFF); F = val & 0xFF; } // For Hb: shift the value up and mask off lower bits
	inline void BC(int16_t val) { B = ((val >> 0x8) & 0xFF); C = val & 0xFF; } // For Lb: mask upper bits
	inline void DE(int16_t val) { D = ((val >> 0x8) & 0xFF); E = val & 0xFF; }
	inline void HL(int16_t val) { H = ((val >> 0x8) & 0xFF); L = val & 0xFF; }

	addr16 PC;				// program counter register
	byte R;					// memory refresh register
	addr16 SP;				// stack pointer

	std::vector<byte> mem;	// cpu memory (0x10000 bytes in size)

	bool IME = true;		// interrupt master enable

	bool halted = false;	// HALT(ed)?
	bool stopped = false;	// STOP(ed)?

	uint16_t clockCycles = 0;

	// initialize keyInfo to default values
	GBKeys keyInfo;

// Flag helper functions
private:
	inline void updateZero(reg16 reg);
	inline void resetZero();
	inline void setZero();

	inline void updateHC(reg16 reg);
	inline void resetHC();
	inline void setHC();

	inline void updateN(bool add);
	inline void resetN();
	inline void setN();

	inline void updateCarry(reg16 reg);
	inline void resetCarry();
	inline void setCarry();

// opcode functions
private:
	inline void jr(bool cond, int8_t to, uint8_t opsize);
	void jp(bool cond, addr16 to, uint8_t opsize);
	void cmp(const byte val);
	void ret(bool cond);
	void call(bool cond);
	void rst(const uint8_t mode);

	void push(reg16 val);
	reg16 pop();

	inline const unsigned short get16();
	inline const unsigned short get16(const addr16 where);

	void decodeExtendedInstruction(byte opcode);

	void halt();
	void stop();

	void dma();
	void interrupt(const byte loc);
	void handleInterrupts();
};

enum Bits
{
	b0 = 0x1,
	b1 = 0x2,
	b2 = 0x4,
	b3 = 0x8,
	b4 = 0x10,
	b5 = 0x20,
	b6 = 0x40,
	b7 = 0x80,
};
#ifdef DEBUG
template<typename T> const std::string toHex(T);
template<> const std::string toHex(sbyte);
template<> const std::string toHex(ubyte);
template<> const std::string toHex(char);
#endif // debug

#endif
