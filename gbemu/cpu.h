#ifndef GB_CPU_H
#define GB_CPU_H

#define DEBUG

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <array>

#include "memdefs.h"
#include "input.h"
#include "types.h"
#include "cart.h"

#ifdef DEBUG
#include "toHex.h"
#endif

/**
Resources:
* http://clrhome.org/table/
* http://fms.komkon.org/GameBoy/Tech/Software.html
* http://gameboy.mongenel.com/dmg/asmmemmap.html
* http://tutorials.eeems.ca/ASMin28Days/lesson/day04.html
* http://www.zophar.net/fileuploads/2/10807fvllz/z80-1.txt
* http://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html
* http://www.z80.info/z80sflag.htm
* http://www.chrisantonellis.com/files/gameboy/gb-cribsheet.pdf
* http://chrisantonellis.com/files/gameboy/gb-programming-manual.pdf
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

#ifdef DEBUG
	void dumpCPU();
#endif // DEBUG

	std::vector<byte>* dumpMem() { return &internalmem; }

// CPU status getting/ setting functions
public:
	void wByte(addr16 addr, byte val);
	byte rByte(addr16 addr) const; // read byte
	byte* gByte(addr16 addr);
	inline void clrBit(byte& val, byte bit) { val &= ~bit; }

	void wWord(addr16 addr, word val);
	inline word rWord(addr16 addr) const { return ((rByte(addr + 1) << 8) | (rByte(addr) & 0xFF)); } // read word

	bool isHalted()	 const { return halted; }
	bool isStopped() const { return stopped; }
	void unHalt() { halted = false; }
	void unStop() { stopped = false; }

	void resetClock() { clockCycles = 0; }
	uint16_t getClockCycles() const { return clockCycles; }

	GBKeys keyInfo;

#ifdef DEBUG
	bool _test = false;
#endif

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

	unsigned char F;		// flag register
	
	// decode flag register bits
	inline const bool zero()		const { return F & 0x40; }
	inline const bool half_carry()	const { return F & 0x10; }
	inline const bool N()			const { return F & 0x2; } // add or subtract
	inline const bool carry()		const { return F & 0x1; }

	// 16 bit "registers"
	inline reg16 AF() { return ((A << 8) | (F & 0xFF)); }
	inline reg16 BC() { return ((B << 8) | (C & 0xFF)); }
	inline reg16 DE() { return ((D << 8) | (E & 0xFF)); }
	inline reg16 HL() { return ((H << 8) | (L & 0xFF)); }

	inline void AF(word val) { A = ((val >> 0x8) & 0xFF); F = val & 0xFF; } // For Hb: shift the value up and mask off lower bits
	inline void BC(word val) { B = ((val >> 0x8) & 0xFF); C = val & 0xFF; } // For Lb: mask upper bits
	inline void DE(word val) { D = ((val >> 0x8) & 0xFF); E = val & 0xFF; }
	inline void HL(word val) { H = ((val >> 0x8) & 0xFF); L = val & 0xFF; }

	addr16 PC;		// program counter register
	addr16 SP;		// stack pointer

	std::vector<byte> internalmem;

	bool IME = true;	// interrupt master enable

	bool halted = false;	// HALT(ed)?
	bool stopped = false;	// STOP(ed)?

	uint16_t clockCycles = 0;

	Cart cart;

// Flag helper functions
private:
	inline void updateZero(reg16 reg);
	inline void resetZero();
	inline void setZero();
	
	inline void updateHC(byte prevVal, byte newVal);
	inline void resetHC();
	inline void setHC();
	
	inline void updateN(bool add);
	inline void resetN();
	inline void setN();
	
	void updateCarry(uint16_t newVal);
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

	void dec(byte& b);
	void inc(byte& b);
	void add(byte val);
	void adc(byte val);
	void sub(byte val);
	void sbc(byte val);
	void andr(byte val);
	void xorr(byte val);
	void orr(byte val);
	// extended instructions
	void rlc(reg& val);
	void rrc(reg& val);
	void rl(reg& val);
	void rr(reg& val);
	void sla(reg& val);
	void sra(reg& val);
	void srl(reg& val);
	void bit(reg r, ubyte bit);
	void res(reg& r, ubyte bit);
	void set(reg& r, ubyte bit);
	inline void swap(reg& r);

	template<bool memread = false>
	inline void ld8(reg& dst, reg src);

	template<bool memread = false>
	inline void ld16(reg& hi, reg& lo, reg16 src);

	void emulateExtendedInstruction(byte opcode);

	void push(reg16 val);
	reg16 pop();

	inline const uint16_t getNextWord() { return rWord(PC + 1); }

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

#endif
