#ifndef GB_CPU_H
#define GB_CPU_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "memdefs.h"

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
#define MEM_SIZE 0xFFFF + 0x1 // give one byte padding

#define ADD true
#define SUB false

typedef unsigned short addr16; // 16-bit address
typedef unsigned char addr8; // 8-bit address

typedef signed char reg; // register
typedef signed short reg16; // 16-bit register

typedef unsigned char u8;
typedef signed char s8;
typedef char byte; // 8-bit for when sign is unspecified
typedef unsigned char ubyte;
typedef signed char sbyte;

typedef unsigned short u16;
typedef signed short s16;

class CPU
{
public:
	CPU();
	~CPU();

	void emulateCycle();
	int loadROM(const std::string& fileName);
	void test();

	byte* dumpMem() { return mem; }
	void cpyMem(byte* mem) { this->mem = mem; }

// CPU status getting/ setting functions
public:
	void setByte(addr16 addr, byte val) { mem[addr] = val; }
	byte getByte(addr16 addr) { return mem[addr]; }

	bool isHalted() { return halted; }
	bool isStopped() { return stopped; }
	void stopHalt() { halted = false; }
	void stopStop() { stopped = false; }

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
	inline bool sign() { return F & 0x80; }
	inline bool zero() { return F & 0x40; }
	inline bool half_carry() { return F & 0x10; }
	inline bool parity() { return F & 0x4; }
	inline bool overflow() { return F & 0x4; }
	inline bool N() { return F & 0x2; } // add or subtract
	inline bool carry() { return F & 0x1; }

	// 16 bit "registers"
	inline reg16 AF() { return ((A << 8) | (F & 0xFF)); }
	inline reg16 BC() { return ((B << 8) | (C & 0xFF)); }
	inline reg16 DE() { return ((D << 8) | (E & 0xFF)); }
	inline reg16 HL() { return ((H << 8) | (L & 0xFF)); }

	inline void AF(s16 val) { A = ((val >> 8) & 0xFF); F = (byte)val; } // For Hb: shift the value up and mask off lower bits
	inline void BC(s16 val) { B = ((val >> 8) & 0xFF); C = (byte)val; } // For Lb: cast to char which automatically masks upper bits
	inline void DE(s16 val) { D = ((val >> 8) & 0xFF); E = (byte)val; }
	inline void HL(s16 val) { H = ((val >> 8) & 0xFF); L = (byte)val; }

	byte I;					// interrupt page address register
	unsigned short PC;		// program counter register
	byte R;					// memory refresh register
	unsigned short SP;		// stack pointer

	byte* mem;				// cpu memory (0xFFFF in size)

	bool IME = true;		// interrupt master enable

	bool halted = false;	// HALT(ed)?
	bool stopped = false;	// STOP(ed)?

// Flag helper functions
private:
	inline void updateSign(reg16 reg);
	inline void resetSign();
	inline void setSign();

	inline void updateZero(reg16 reg);
	inline void resetZero();
	inline void setZero();

	inline void updateHC(reg16 reg);
	inline void resetHC();
	inline void setHC();

	inline void updateParity(reg16 reg);
	inline void resetParity();
	inline void setParity();

	inline void updateOverflow(reg16 reg);
	inline void resetOverflow();
	inline void setOverflow();

	inline void updateN(bool add);
	inline void resetN();
	inline void setN();

	inline void updateCarry(reg16 reg);
	inline void resetCarry();
	inline void setCarry();

// opcode functions
private:
	inline void jr(bool cond, s8 to, u8 opsize);
	void jp(bool cond, s16 to, u8 opsize);
	void cmp(const byte val);
	void ret(bool cond);
	void call(bool cond);
	void rst(const u8 mode);

	void push(reg16 val);
	reg16 pop();

	inline const unsigned short load16();
	inline const unsigned short get16();
	inline const unsigned short get16(const addr16 where);

	void decodeExtendedInstruction(byte opcode);
	void swapNibble(reg& reg);
	inline void rlc(reg& reg);
	inline void rrc(reg& reg);
	inline void rl(reg& reg);
	inline void rr(reg& reg);
	inline void sla(reg& reg);
	inline void sra(reg& reg);
	inline void srl(reg& reg);

	inline void bit(reg reg, ubyte bit);
	inline void res(reg& reg, ubyte bit);
	inline void set(reg& reg, ubyte bit);

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

template<typename T> const std::string toHex(T);
const std::string toHex(const char);

#endif