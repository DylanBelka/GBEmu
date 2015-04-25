#ifndef GB_CPU_H
#define GB_CPU_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "memdefs.h"

/*
Resources:
http://clrhome.org/table/
http://fms.komkon.org/GameBoy/Tech/Software.html
http://gameboy.mongenel.com/dmg/asmmemmap.html
http://tutorials.eeems.ca/ASMin28Days/lesson/day04.html
http://www.zophar.net/fileuploads/2/10807fvllz/z80-1.txt
*/

class CPU
{
public:
	CPU();
	~CPU();

	void emulateCycle();
	int loadROM(const std::string& fileName);
	void test();

	char* dumpMem() { return mem; }
	void cpyMem(char* mem) { this->mem = mem; }

// CPU status getting/ setting functions
public:
	void setByte(unsigned short addr, char val) { mem[addr] = val; }
	char getByte(unsigned short addr) { return mem[addr]; }

private:
	void reset();

// registers
private:
	// 8 bit registers
	signed char A;
	signed char B;
	signed char C;
	signed char D;
	signed char E;
	signed char H;
	signed char L;

	unsigned char F;		// flag register

	// decode flag register bits
	inline bool sign() { return F & 0x80; }
	inline bool zero() { return F & 0x40; }
	inline bool half_carry() { return F & 0x10; }
	inline bool parity() { return F & 0x4; }
#define overflow() parity()
	inline bool N() { return F & 0x2; } // add or subtract
	inline bool carry() { return F & 0x1; }

	// 16 bit "registers"
	inline short AF() { return ((A << 8) | (F & 0xFF)); }
	inline short BC() { return ((B << 8) | (C & 0xFF)); }
	inline short DE() { return ((D << 8) | (E & 0xFF)); }
	inline short HL() { return ((H << 8) | (L & 0xFF)); }

	inline void AF(signed short val) { A = ((val >> 8) & 0xFF); F = (char)val; } // For Hb: shift the value up and mask off lower bits
	inline void BC(signed short val) { B = ((val >> 8) & 0xFF); C = (char)val; } // For Lb: cast to char which automatically masks upper bits
	inline void DE(signed short val) { D = ((val >> 8) & 0xFF); E = (char)val; }
	inline void HL(signed short val) { H = ((val >> 8) & 0xFF); L = (char)val; }

	char I;		// interrupt page address register
	//short IX, IY;	// 16 bit index registers ~!GB
	unsigned short PC;		// program counter register
	char R;		// memory refresh register
	unsigned short SP;		// stack pointer

	char* mem;

	bool IME = false; // interrupt master enable

// Flag helper functions
private:
	inline void updateSign(short reg);
	inline void resetSign();
	inline void setSign();

	inline void updateZero(short reg);
	inline void resetZero();
	inline void setZero();

	inline void updateHC(short reg);
	inline void resetHC();
	inline void setHC();

	inline void updateParity(char reg);
	inline void resetParity();
	inline void setParity();

	inline void updateOverflow(short reg);
	inline void resetOverflow();
	inline void setOverflow();

	inline void updateN(bool add);
	inline void resetN();
	inline void setN();

	inline void updateCarry(short reg);
	inline void resetCarry();
	inline void setCarry();

// opcode functions
private:
	inline void jr(bool cond, signed char to, unsigned char opsize);
	void jp(bool cond, signed short to, unsigned char opsize);
	void cmp(const char val);
	void ret(bool cond);
	void call(bool cond);
	void rst(const unsigned char mode);

	inline const unsigned short load16();
	inline const unsigned short get16();
	inline const unsigned short get16(const short where);

	void decodeExtendedInstruction(char opcode);
	void swapNibble(signed char reg);
	inline void rlc(signed char& reg);
	inline void rrc(signed char& reg);
	inline void rl(signed char& reg);
	inline void rr(signed char& reg);
	inline void sla(signed char& reg);
	inline void bit(signed char reg, unsigned char bit);

	void halt();

	void dma();
	void interrupt(const char loc);
	void handleInterrupts();
};

const std::string toHex(int);

#endif