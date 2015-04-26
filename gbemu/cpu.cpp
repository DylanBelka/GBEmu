#include "cpu.h"

// ^^^ = check this
// &&& = redundant opcode - 'optimized' ex: ld a, a

#define MAX_ROM_SIZE 0xBFFF
#define MEM_SIZE 0xFFFF + 0x1 // give one byte padding

#define ADD true
#define SUB false

CPU::CPU()
{
	mem = new char[MEM_SIZE];
	reset();
}

CPU::~CPU()
{
	delete[] mem;
}

void CPU::reset()
{
	// initialize all mem to 0
	for (int i = 0; i < MEM_SIZE; i++)
	{
		mem[i] = 0;
	}
	// some known starting values of registers
	F = 0xB0;
	BC(0x13);
	DE(0xD8);
	HL(0x14D);
	SP = 0xFFFE;
	R = 0;
	PC = 0x100;
	// set memory registers to their known starting values
	mem[TIMA] = 0x00;
	mem[TMA] = 0x00;
	mem[TAC] = 0x00;
	mem[NR10] = 0x80;
	mem[NR11] = 0xBF;
	mem[NR12] = 0xF3;
	mem[NR14] = 0xBF;
	mem[NR21] = 0x3F;
	mem[NR22] = 0x00;
	mem[NR24] = 0xBF;
	mem[NR30] = 0x7F;
	mem[NR31] = 0xFF;
	mem[NR32] = 0x9F;
	mem[NR33] = 0xBF;
	mem[NR41] = 0xFF;
	mem[NR42] = 0x00;
	mem[NR43] = 0x00;
	mem[NR30] = 0xBF;
	mem[NR50] = 0x77;
	mem[NR51] = 0xF3;
	mem[NR52] = 0xF1;
	mem[LCDC] = 0x91;
	mem[SCY] = 0x00;
	mem[SCX] = 0x00;
	mem[LYC] = 0x00;
	mem[BGP] = 0xFC;
	mem[OBP0] = 0xFC;
	mem[OBP1] = 0xFF;
	mem[WY] = 0x00;
	mem[WX] = 0x00;
	mem[IE] = 0x00;
	mem[LY] = 0x94;
}

#pragma region FlagFuncs

void CPU::updateCarry(short reg)
{
	F |= (reg > 0xFF || reg < 0x00) ? 0x1 : F;
}

void CPU::resetCarry()
{
	F &= 0xFE;
}

void CPU::setCarry()
{
	F |= 0x1;
}

void CPU::updateHC(short reg)
{
	F |= (reg > 0xF) ? 0x10 : F;
}

void CPU::resetHC()
{
	F &= 0xEF;
}

void CPU::setHC()
{
	F |= 0x10;
}

void CPU::updateN(bool add)
{
	if (add) { F |= 0x2; }
	else { F &= 0xFD; }
}

void CPU::resetN()
{
	F &= 0xFD;
}

void CPU::setN()
{
	F |= 0x2;
}

void CPU::updateOverflow(short reg)
{
	F |= (reg & 0x80) ? 0x4 : F;
}

void CPU::resetOverflow()
{	
	F &= 0xFB;
}

void CPU::setOverflow()
{
	F |= 0x4;
}

void CPU::updateParity(char reg)
{
	bool parity =
		(((reg * 0x0101010101010101ULL) & 0x8040201008040201ULL) % 0x1FF) & 1; // https://graphics.stanford.edu/~seander/bithacks.html#ParityNaive 
																				// (uses Compute parity of a byte using 64-bit multiply and modulus division)
	F |= (!parity) ? 0x4 : F;
}

void CPU::resetParity()
{
	F &= 0xFB;
}

void CPU::setParity()
{
	F |= 0x4;
}

void CPU::updateSign(short reg)
{
	F |= reg & 0x80;
}

void CPU::resetSign()
{
	F &= 0x7F;
}

void CPU::setSign()
{
	F |= 0x80;
}

void CPU::updateZero(short reg)
{
	if (reg == 0)
	{
		F |= 0x40;
	}
	else
	{
		resetZero();
	}
}

void CPU::resetZero()
{
	F &= 0xBF;
}

void CPU::setZero()
{
	F |= 0x40;
}

#pragma endregion

#pragma region OpFuncs

void CPU::decodeExtendedInstruction(char opcode)
{
	switch (opcode)
	{
		case 0x00: // rlc b
		{
			rlc(B);
			PC += 2;
			break;
		}
		case 0x01: // rlc c
		{
			rlc(C);
			PC += 2;
			break;
		}
		case 0x02: // rlc d
		{
			rlc(D);
			PC += 2;
			break;
		}
		case 0x03: // rlc e
		{
			rlc(E);
			PC += 2;
			break;
		}
		case 0x04: // rlc h
		{
			rlc(H);
			PC += 2;
			break;
		}
		case 0x05: // rlc l
		{
			rlc(L);
			PC += 2;
			break;
		}
		case 0x06: // rlc (hl)
		{
			char val = mem[(unsigned short)HL()];
			val <<= 1;
			resetCarry();
			F |= val & 0x80;
			val |= val & 0x80;
			resetN();
			updateParity(val);
			updateZero(val);
			updateSign(val);
			mem[(unsigned short)HL()] = val;
			PC += 2;
			break;
		}
		case 0x07: // rlc a
		{
			rlc(A);
			PC += 2;
			break;
		}
		case 0x08: // rrc b
		{
			rrc(B);
			PC += 2;
			break;
		}
		case 0x09: // rrc c
		{
			rrc(C);
			PC += 2;
			break;
		}
		case 0x0A: // rrc d
		{
			rrc(D);
			PC += 2;
			break;
		}
		case 0x0B: // rrc e
		{
			rrc(E);
			PC += 2;
			break;
		}
		case 0x0C: // rrc h
		{
			rrc(H);
			PC += 2;
			break;
		}
		case 0x0D: // rrc l
		{
			rrc(L);
			PC += 2;
			break;
		}
		case 0x0E: // rrc (hl)
		{
			char val = mem[(unsigned short)HL()];
			val >>= 1;
			resetCarry();
			F |= val & 0x1;
			val |= val & 0x1;
			resetN();
			updateParity(val);
			updateZero(val);
			updateSign(val);
			mem[(unsigned short)HL()] = val;
			PC += 2;
			break;
		}
		case 0x0F: // rrc a
		{
			rrc(A);
			PC += 2;
			break;
		}
		case 0x10: // rl b
		{
			rl(B);
			PC += 2;
			break;
		}
		case 0x11: // rl c
		{
			rl(C);
			PC += 2;
			break;
		}
		case 0x12: // rl d
		{
			rl(D);
			PC += 2;
			break;
		}
		case 0x13: // rl e
		{
			rl(E);
			PC += 2;
			break;
		}
		case 0x14: // rl h
		{
			rl(H);
			PC += 2;
			break;
		}
		case 0x15: // rl l
		{
			rl(L);
			PC += 2;
			break;
		}
		case 0x16: // rl (hl)
		{
			char val = mem[(unsigned short)HL()];
			val <<= 1;
			val |= overflow();
			F |= val & 0x80;
			updateCarry(val);
			resetN();
			updateParity(val);
			resetHC();
			updateZero(val);
			updateSign(val);
			mem[(unsigned short)HL()] = val;
			PC += 2;
			break;
		}
		case 0x17: // rl a
		{
			rl(A);
			PC += 2;
			break;
		}
		case 0x18: // rr b
		{
			rr(B);
			PC += 2;
			break;
		}
		case 0x19: // rr c
		{
			rr(C);
			PC += 2;
			break;
		}
		case 0x1A: // rr d
		{
			rr(D);
			PC += 2;
			break;
		}
		case 0x1B: // rr e
		{
			rr(E);
			PC += 2;
			break;
		}
		case 0x1C: // rr h
		{
			rr(H);
			PC += 2;
			break;
		}
		case 0x1D: // rr l
		{
			rr(L);
			PC += 2;
			break;
		}
		case 0x1E: // rr (hl)
		{
			char val = mem[(unsigned short)HL()];
			val >>= 1;
			val &= F & 0x80;
			F |= val & 0x1;
			updateCarry(val);
			resetN();
			updateParity(val);
			resetHC();
			updateZero(val);
			updateSign(val);
			mem[(unsigned short)HL()] = val;
			PC += 2;
			break;
		}
		case 0x1F: // rr a
		{
			rr(A);
			PC += 2;
			break;
		}
		case 0x20: // sla b
		{

		}
		case 0x37: // swap a
		{
			swapNibble(A);
			PC += 2;
			break;
		}
		case 0x47: // bit 0, a
		{
			bit(A, 0x1);
			PC += 2;
			break;
		}
		case 0x4F: // bit 1, a
		{
			bit(A, 0x2);
			PC += 2;
			break;
		}
		case 0x57: // bit 2, a
		{
			bit(A, 0x4);
			PC += 2;
			break;
		}
		case 0x5F: // bit 3, a
		{
			bit(A, 0x8);
			PC += 2;
			break;
		}
		case 0x67: // bit 4, a
		{
			bit(A, 0x10);
			PC += 2;
			break;
		}
		case 0x6F: // bit 5, a
		{
			bit(A, 0x20);
			PC += 2;
			break;
		}
		case 0x77: // bit 6, a
		{
			bit(A, 0x40);
			PC += 2;
			break;
		}
		case 0x7F: // bit 7, a
		{
			bit(A, 0x80);
			PC += 2;
			break;
		}
		default:
		{
			PC += 2;
			std::cout << "Unimplemented instruction: " << toHex(opcode) << std::endl;
			system("pause");
		}
	}
}

void CPU::swapNibble(signed char val)
{
	val = ((val & 0x0F) << 4 | (val & 0xF0) >> 4);
}

void CPU::cmp(const char val)
{
	updateCarry(A - val);
	updateN(SUB);
	updateOverflow(A - val);
	updateHC(A - val);
	updateZero(A - val);
	updateSign(A - val);
}

const unsigned short CPU::load16()
{
	return ((mem[PC + 2] << 8) | (mem[PC + 1] & 0xFF));
}

const unsigned short CPU::get16()
{
	return ((mem[PC + 2] << 8) | (mem[PC + 1] & 0xFF));
}

const unsigned short CPU::get16(const short where)
{
	return ((mem[where + 2] << 8) | (mem[where + 1] & 0xFF));
}

void CPU::rlc(signed char& reg)
{
	reg <<= 1;
	resetCarry();
	F |= reg & 0x80;
	reg |= reg & 0x80;
	updateCarry(reg);
	resetN();
	updateParity(reg);
	updateZero(reg);
	updateSign(reg);
	resetHC();
}

void CPU::rl(signed char& reg)
{
	reg <<= 1;
	reg |= F & 0x1;
	F |= reg & 0x80;
	updateCarry(reg);
	resetN();
	updateParity(reg);
	resetHC();
	updateZero(reg);
	updateSign(reg);
}

void CPU::rrc(signed char& reg)
{
	reg >>= 1;
	resetCarry();
	F |= reg & 0x1;
	reg |= reg & 0x1;
	resetN();
	updateParity(reg);
	updateZero(reg);
	updateSign(reg);
}

void CPU::rr(signed char& reg)
{
	reg >>= 1;
	reg &= F & 0x80;
	F |= reg & 0x1;
	updateCarry(reg);
	resetN();
	updateParity(reg);
	resetHC();
	updateZero(reg);
	updateSign(reg);
}

void CPU::bit(signed char reg, unsigned char bit)
{
	updateZero(B & bit);
	setHC();
	resetN();
}

// set halted flag
void CPU::halt()
{
	halted = true;
}

void CPU::stop()
{
	stopped = true;
}

void CPU::ret(bool cond)
{
	if (cond)
	{
		PC = mem[SP] << 8;
		SP++;
		PC |= mem[SP] & 0xFF;
		SP++;
	}
	else
	{
		PC++;
	}
}

void CPU::call(bool cond)
{
	if (cond)
	{
		SP--;
		mem[SP] = (PC + 3) & 0xFF; // + 3 is for jumping past the 3 bytes for the opcode and dest
		SP--;
		mem[SP] = (((PC + 3) >> 8));
		PC = get16();
	}
	else
	{
		PC += 3;
	}
}

inline void CPU::rst(unsigned char mode)
{
	mem[SP] = PC + 1;
	PC = mode;
}

// jrs PC to [to] if cond is true
// Else it increases PC by [opsize]
inline void CPU::jr(bool cond, signed char to, unsigned char opsize)
{
	PC += (cond) ? to + opsize : opsize; // the + 2 is to jump past the initial instruction
}

void CPU::jp(bool cond, signed short to, unsigned char opsize)
{
	if (cond)
	{
		PC = to;
	}
	else
	{
		PC += opsize;
	}
}

#pragma endregion

void CPU::dma()
{
	const unsigned short dmaStart = A << 0x8;
	for (int i = 0; i < 0x8C; i++)
	{
		mem[OAM + i] = mem[dmaStart + i];
	}
	//std::cout << "DMA start: " << toHex(dmaStart) << std::endl;
}

void CPU::interrupt(const char to)
{
	SP--;
	mem[SP] = (PC) & 0xFF;
	SP--;
	mem[SP] = (((PC) >> 8));
	PC = to;
	IME = false; // turn off interrupts
	mem[IE] = 0x0; // reset IE
}

void CPU::handleInterrupts()
{
	if (IME) // are interrupts enabled?
	{
		switch (mem[IE])
		{
			case 0x1: // vblank
			{
				const int vblank = 0x40;
				interrupt(vblank);
				break;
			}
		}
	}
}

void CPU::test()
{
	std::string testROM = "tetris.gb";
	if (!loadROM(testROM))
	{
		std::cout << "rom load failed, rom too big" << std::endl;
		return;
	}
	mem[LY] = 0x91;
	for (int i = 0; i < 0x4500; i++)
	{
		emulateCycle();
	}
	std::cout << std::endl;
	std::cout << "A: " << toHex(A) << std::endl;
	std::cout << "B: " << toHex(B) << std::endl;
	std::cout << "C: " << toHex(C) << std::endl;
	std::cout << "D: " << toHex(D) << std::endl;
	std::cout << "E: " << toHex(E) << std::endl;
	std::cout << "F: " << toHex(F) << std::endl;
	std::cout << "AF: " << toHex(AF()) << std::endl;
	std::cout << "BC: " << toHex(BC()) << std::endl;
	std::cout << "DE: " << toHex(DE()) << std::endl;
	std::cout << "HL: " << toHex(HL()) << std::endl;
}

void CPU::emulateCycle()
{
	handleInterrupts();
	unsigned char opcode = mem[PC]; // get next opcode
	R++; // I think this is what R does
	//std::cout << toHex((int)opcode) << "\tat " << toHex((int)PC) << std::endl;
	std::cout << toHex(mem[OAM + 0x00]) << std::endl;
	// emulate the opcode (compiles to a jump table)
	switch (opcode)
	{
		case 0x00: // NOP
		{
			PC++;
			break;
		}
		case 0x01: // ld BC, **
		{
			BC((unsigned short)get16());
			PC += 3;
			break;
		}
		case 0x02: // ld (BC), a
		{
			mem[(unsigned short)BC()] = A;
			PC++;
			break;
		}
		case 0x03: // inc BC
		{
			const short bc = BC();
			BC(bc + 1);
			PC++;
			break;
		}
		case 0x04: // inc b
		{
			B++;
			updateN(ADD);
			updateOverflow(B);
			updateHC(B);
			updateZero(B);
			updateSign(B);
			PC++;
			break;
		}
		case 0x05: // dec b
		{
			B--;
			updateN(SUB);
			updateOverflow(B);
			updateHC(B);
			updateZero(B);
			updateSign(B);
			PC++;
			break;
		}
		case 0x06: // ld b, *
		{
			B = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x07: // rlca
		{
			A <<= 1;
			updateCarry(A);
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x08: // ex af, af' (~!GB)
		{
			PC++;
			break;
		}
		case 0x09: // add hl, bc
		{
			const short hl = HL();
			HL(BC() + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x0A: // ld a, (BC)
		{
			A = mem[(unsigned short)BC()];
			PC++;
			break;
		}
		case 0x0B: // dec BC
		{
			const short bc = BC();
			BC(bc - 1);
			PC++;
			break;
		}
		case 0x0C: // inc C
		{
			C++;
			updateN(ADD);
			updateOverflow(C);
			updateHC(C);
			updateZero(C);
			updateSign(C);
			PC++;
			break;
		}
		case 0x0D: // dec C
		{
			C--;
			updateN(SUB);
			updateOverflow(C);
			updateHC(C);
			updateZero(C);
			updateSign(C);
			PC++;
			break;
		}
		case 0x0E: // ld c, *
		{
			C = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x0F: // rrca
		{
			A >>= 1;
			updateCarry(A);
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x10: // STOP
		{
			PC++;
			break;
		}
		case 0x11: // ld de, **
		{
			DE(load16());
			PC += 3;
			break;
		}
		case 0x12: // ld (de), a
		{
			mem[(unsigned short)DE()] = A;
			PC++;
			break;
		}
		case 0x13: // inc de
		{
			const short de = DE();
			DE(de + 1);
			PC++;
			break;
		}
		case 0x14: // inc d
		{
			D++;
			updateN(ADD);
			updateOverflow(D);
			updateHC(D);
			updateZero(D);
			updateSign(D);
			PC++;
			break;
		}
		case 0x15: // dec d
		{
			D--;
			updateN(SUB);
			updateOverflow(D);
			updateHC(D);
			updateZero(D);
			updateSign(D);
			PC++;
			break;
		}
		case 0x16: // ld d, *
		{
			D = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x17: // rla
		{
			A <<= 1;
			updateCarry(A);
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x18: // jr *
		{
			jr(true, mem[PC + 1], 2);
			break;
		}
		case 0x19: // add hl, de
		{
			const short hl = HL();
			HL(hl + DE());
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x1A: // ld a, (de)
		{
			A = mem[DE()];
			PC++;
			break;
		}
		case 0x1B: // dec de
		{
			const short de = DE();
			DE(de - 1);
			PC++;
			break;
		}
		case 0x1C: // inc e
		{
			E++;
			updateCarry(E);
			updateN(ADD);
			updateOverflow(E);
			updateHC(E);
			updateSign(E);
			PC++;
			break;
		}
		case 0x1D: // dec e
		{
			E--;
			updateCarry(E);
			updateN(SUB);
			updateOverflow(E);
			updateHC(E);
			updateSign(E);
			PC++;
			break;
		}
		case 0x1E: // ld e, *
		{
			E = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x1F: // rra
		{
			A >>= 1;
			updateCarry(A);
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x20: // jr nz, *
		{
			jr(!zero(), mem[PC + 1], 2);
			break;
		}
		case 0x21: // ld hl, **
		{
			HL(get16());
			PC += 3;
			break;
		}
		case 0x22: // ldi (hl), a   or   ld (hl+), a
		{
			mem[(unsigned short)HL()] = A;
			const short hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x23: // inc hl
		{
			const short hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x24: // inc h
		{
			H++;
			updateN(ADD);
			updateOverflow(H);
			updateHC(H);
			updateZero(H);
			updateZero(H);
			updateSign(H);
			PC++;
			break;
		}
		case 0x25: // dec h
		{
			H--;
			updateN(SUB);
			updateOverflow(H);
			updateHC(H);
			updateZero(H);
			updateZero(H);
			updateSign(H);
			PC++;
			break;
		}
		case 0x26: // ld h, *
		{
			H = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x27: // daa ^^^Probably doesnt work ^^^^
		{
			short uiResult = 0;
			while (A > 0) 
			{
				uiResult <<= 4;
				uiResult |= A % 10;
				A /= 10;
			}
			updateOverflow(A);
			updateCarry(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x28: // jr z, *
		{
			jr(zero(), mem[PC + 1], 2);
			break;
		}
		case 0x29: // add hl, hl
		{
			const short hl = HL();
			HL(hl + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x2A: // ldi a, (hl) 
		{
			A = mem[(unsigned short)HL()];
			const short hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x2B: // dec hl
		{
			const short hl = HL();
			HL(hl - 1);
			PC++;
			break;
		}
		case 0x2C: // inc l
		{
			L++;
			updateOverflow(L);
			updateHC(L);
			updateN(ADD);
			updateZero(L);
			updateSign(L);
			PC++;
			break;
		}
		case 0x2D: // dec l
		{
			L--;
			updateOverflow(L);
			updateHC(L);
			updateN(SUB);
			updateZero(L);
			updateSign(L);
			PC++;
			break;
		}
		case 0x2E: // ld l, *
		{
			L = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x2f: // cpl ^^^
		{
			A = ~A;
			PC++;
			break;
		}
		case 0x30: // jr nc, *
		{
			jr(!carry(), mem[PC + 1], 2);
			break;
		}
		case 0x31: // ld sp, **
		{
			SP = get16();
			PC += 3;
			break;
		}
		case 0x32: // ldd (hl), a
		{
			mem[(unsigned short)HL()] = A;
			const short hl = HL();
			HL(hl - 1);
			PC++;
			break;
		}
		case 0x33: // inc sp
		{
			SP++;
			PC++;
			break;
		}
		case 0x34: // inc (hl) ^^^
		{
			mem[HL()]++;
			updateOverflow(HL());
			updateN(ADD);
			updateZero(HL());
			updateHC(HL());
			updateSign(HL());
			PC++;
			break;
		}
		case 0x35: // dec (hl) ^^^
		{
			mem[HL()]--;
			updateOverflow(HL());
			updateN(SUB);
			updateZero(HL());
			updateHC(HL());
			updateSign(HL());
			PC++;
			break;
		}
		case 0x36: // ld (hl), *
		{
			mem[HL()] = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x37: // scf
		{
			setCarry();
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x38: // jr c, *
		{
			jr(carry(), mem[PC + 1], 2);
			break;
		}
		case 0x39: // add hl, sp
		{
			const short hl = HL();
			HL(hl + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			updateZero(HL());
			updateSign(HL());
			PC++;
			break;
		}
		case 0x3A: // ldd a, (hl)
		{
			A = mem[HL()];
			const short hl = HL();
			HL(hl - 1);
			PC++;
			break;
		}
		case 0x3B: // dec sp
		{
			SP--;
			PC++;
			break;
		}
		case 0x3C: // inc a
		{
			A++;
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x3D: // dec a
		{
			A--;
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x3E: // ld a, *
		{
			A = mem[PC + 1];
			PC += 2;
			break;
		}
		case 0x3F: // ccf
		{
			F ^= 0x1;
			PC++;
			break;
		}
		case 0x40: // ld b, b
		{
			PC++;
			break;
		}
		case 0x41: // ld b, c
		{
			B = C;
			PC++;
			break;
		}
		case 0x42: // ld b, d
		{
			B = D;
			PC++;
			break;
		}
		case 0x43: // ld b, e
		{
			B = E;
			PC++;
			break; 
		}
		case 0x44: // ld b, h
		{
			B = H;
			PC++;
			break;
		}
		case 0x45: // ld b, l
		{
			B = L;
			PC++;
			break;
		}
		case 0x46: // ld b, (hl)
		{
			B = mem[HL()];
			PC++;
			break;
		}
		case 0x47: // ld b, a
		{
			B = A;
			PC++;
			break;
		}
		case 0x48: // ld c, b
		{
			C = B;
			PC++;
			break;
		}
		case 0x49: // ld c, c
		{
			PC++;
			break;
		}
		case 0x4A: // ld c, d
		{
			C = D;
			PC++;
			break;
		}
		case 0x4B: // ld c, e
		{
			C = E;
			PC++;
			break;
		}
		case 0x4C: // ld c, h
		{
			C = H;
			PC++;
			break;
		}
		case 0x4D: // ld c, l
		{
			C = L;
			PC++;
			break;
		}
		case 0x4E: // ld c, (hl)
		{
			C = mem[HL()];
			PC++;
			break;
		}
		case 0x4F: // ld c, a
		{
			C = A;
			PC++;
			break;
		}
		case 0x50: // ld d, b
		{
			D = B;
			PC++;
			break;
		}
		case 0x51: // ld d, c
		{
			D = C;
			PC++;
			break;
		}
		case 0x52: // ld d, d
		{
			PC++;
			break;
		}
		case 0x53: // ld d, e
		{
			D = E;
			PC++;
			break;
		}
		case 0x54: // ld d, h
		{
			D = H;
			PC++;
			break;
		}
		case 0x55: // ld d, l
		{
			D = L;
			PC++;
			break;
		}
		case 0x56: // ld d, (hl)
		{
			D = mem[HL()];
			PC++;
			break;
		}
		case 0x57: // ld d, a
		{
			D = A;
			PC++;
			break;
		}
		case 0x58: // ld e, b
		{
			E = B;
			PC++;
			break;
		}
		case 0x59: // ld e, c
		{
			E = C;
			PC++;
			break;
		}
		case 0x5A: // ld e, d
		{
			E = D;
			PC++;
			break;
		}
		case 0x5B: // ld e, e
		{
			PC++;
			break;
		}
		case 0x5C: // ld e, h
		{
			E = H;
			PC++;
			break;
		}
		case 0x5D: // ld e, l
		{
			E = L;
			PC++;
			break;
		}
		case 0x5E: // ld e, (hl)
		{
			E = mem[HL()];
			PC++;
			break;
		}
		case 0x5F: // ld e, a
		{
			E = A;
			PC++;
			break;
		}
		case 0x60: // ld h, b
		{
			H = B;
			PC++;
			break;
		}
		case 0x61: // ld h, c
		{
			H = C;
			PC++;
			break;
		}
		case 0x62: // ld h, d
		{
			H = D;
			PC++;
			break;
		}
		case 0x63: // ld h, e
		{
			H = E;
			PC++;
			break;
		}
		case 0x64: // ld h, h &&&
		{
			PC++;
			break;
		}
		case 0x65: // ld h, l
		{
			H = L;
			PC++;
			break;
		}
		case 0x66: // ld h, (hl)
		{
			H = mem[(unsigned short)HL()];
			PC++;
			break;
		}
		case 0x67: // ld h, a
		{
			H = A;
			PC++;
			break;
		}
		case 0x68: // ld l, b
		{
			L = B;
			PC++;
			break;
		}
		case 0x69: // ld l, c
		{
			L = C;
			PC++;
			break;
		}
		case 0x6A: // ld l, d
		{
			L = D;
			PC++;
			break;
		}
		case 0x6B: // ld l, e
		{
			L = E;
			PC++;
			break;
		}
		case 0x6C: // ld l, h
		{
			L = H;
			PC++;
			break;
		}
		case 0x6D: // ld l, l &&&
		{
			PC++;
			break;
		}
		case 0x6E: // ld l, (hl)
		{
			L = mem[(unsigned short)HL()];
			PC++;
			break;
		}
		case 0x6F: // ld l, a
		{
			L = A;
			PC++;
			break;
		}
		case 0x70: // ld (hl), b
		{
			mem[(unsigned short)HL()] = B;
			PC++;
			break;
		}
		case 0x71: // ld (hl), c
		{
			mem[(unsigned short)HL()] = C;
			PC++;
			break;
		}
		case 0x72: // ld (hl), d
		{
			mem[(unsigned short)HL()] = D;
			PC++;
			break;
		}
		case 0x73: // ld (hl), e
		{
			mem[(unsigned short)HL()] = E;
			PC++;
			break;
		}
		case 0x74: // ld (hl), h
		{
			mem[(unsigned short)HL()] = H;
			PC++;
			break;
		}
		case 0x75: // ld (hl), l
		{
			mem[(unsigned short)HL()] = L;
			PC++;
			break;
		}
		case 0x76: // halt ^^^ TODO:Implement
		{
			halt();
			PC++;
			break;
		}
		case 0x77: // ld (hl), a
		{
			mem[(unsigned short)HL()] = A;
			PC++;
			break;
		}
		case 0x78: // ld a, b
		{
			A = B;
			PC++;
			break;
		}
		case 0x79: // ld a, c
		{
			A = C;
			PC++;
			break;
		}
		case 0x7A: // ld a, d
		{
			A = D;
			PC++;
			break;
		}
		case 0x7B: // ld a, e
		{
			A = E;
			PC++;
			break;
		}
		case 0x7C: // ld a, h
		{
			A = H;
			PC++;
			break;
		}
		case 0x7D: // ld a, l
		{
			A = L;
			PC++;
			break;
		}
		case 0x7E: // ld a, (hl)
		{
			A = mem[(unsigned short)HL()];
			PC++;
			break;
		}
		case 0x7F: // ld a, a &&&
		{
			PC++;
			break;
		}
		case 0x80: // add a, b
		{
			A += B;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x81: // add a, c
		{
			A += C;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x82: // add a,d 
		{
			A += D;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x83: // add a, e
		{
			A += E;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x84: // add a, h
		{
			A += H;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x85: // add a, l
		{
			A += L;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x86: // add a, (hl)
		{
			A += mem[(unsigned short)HL()];
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x87: // add a, a
		{
			A += A;
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x88: // adc a, b ^^^ check all adcs
		{
			A += B + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x89: // adc a, c
		{
			A += B + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8A: // adc a, d
		{
			A += D + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8B: // adc a, e
		{
			A += E + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8C: // adc a, h
		{
			A += H + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8D: // adc a, l
		{
			A += L + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8E: // adc a, (hl)
		{
			A += mem[(unsigned short)HL()] + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x8F: // adc a, a
		{
			A += A + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x90: // sub b
		{
			A -= B;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x91: // sub c
		{
			A -= C;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x92: // sub d
		{
			A -= D;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x93: // sub e
		{
			A -= E;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x94: // sub h
		{
			A -= H;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x95: // sub l
		{
			A -= L;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x96: // sub (hl)
		{
			A -= mem[(unsigned short)HL()];
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x97: // sub a 
		{
			A -= A;
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x98: // sbc a, b ^^^ (B - carry() or B + carry())
		{
			A -= B - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x99: // sbc a, c
		{
			A -= C - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9A: // sbc a, d
		{
			A -= D - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9B: // sbc a, e
		{
			A -= E - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9C: // sbc a, h
		{
			A -= H - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9D: // sbc a, l
		{
			A -= L - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9E: // sbc a, (hl)
		{
			A -= mem[(unsigned short)HL()] - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0x9F: // sbc a, a
		{
			A -= A - carry();
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA0: // and b
		{
			A &= B;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA1: // and c
		{
			A &= C;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA2: // and d
		{
			A &= D;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA3: // and e
		{
			A &= E;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA4: // and h
		{
			A &= H;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA5: // and l
		{
			A &= L;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA6: // and (hl)
		{
			A &= mem[(unsigned short)HL()];
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA7: // and a &&&
		{
			// A &= A;
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA8: // xor b
		{
			A ^= B;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xA9: // xor c
		{
			A ^= C;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAA: // xor d
		{
			A ^= D;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAB: // xor e
		{
			A ^= E;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAC: // xor h
		{
			A ^= H;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAD: // xor l
		{
			A ^= L;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAE: // xor (hl)
		{
			A ^= mem[(unsigned short)HL()];
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xAF: // xor a &&& A = 0
		{
			A = 0;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB0: // or b
		{
			A |= B;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB1: // or c
		{
			A |= C;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB2: // or d
		{
			A |= D;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB3: // or e
		{
			A |= E;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB4: // or h
		{
			A |= H;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB5: // or l
		{
			A |= L;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB6: // or (hl)
		{
			A |= mem[(unsigned short)HL()];
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB7: // or a &&&
		{
			// A |= A;
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC++;
			break;
		}
		case 0xB8: // cp b
		{
			cmp(B);
			PC++;
			break;
		}
		case 0xB9: // cp c
		{
			cmp(C);
			PC++;
			break;
		}
		case 0xBA: // cp d
		{
			cmp(D);
			PC++;
			break;
		}
		case 0xBB: // cp e
		{
			cmp(E);
			PC++;
			break;
		}
		case 0xBC: // cp h
		{
			cmp(H);
			PC++;
			break;
		}
		case 0xBD: // cp l
		{
			cmp(L);
			PC++;
			break;
		}
		case 0xBE: // cp (hl)
		{
			cmp(mem[(unsigned short)HL()]);
			PC++;
			break;
		}
		case 0xBF: // cp a ^^^ = &&& try to optimize this
		{
			cmp(A);
			PC++;
			break;
		}
		case 0xC0: // ret nz
		{
			ret(!carry());
			break;
		}
		case 0xC1: // pop bc
		{
			C = mem[SP];
			SP++;
			B = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xC2: // jp nz, ** ^^^ check get16
		{
			jp(!zero(), get16(), 3);
			break;
		}
		case 0xC3: // jp **
		{
			jp(true, get16(), 3);
			break;
		}
		case 0xC4: // call nz, ** ^^^
		{
			call(!zero());
			break;
		}
		case 0xC5: // push bc
		{
			SP--;
			mem[SP] = B;
			SP--;
			mem[SP] = C;
			PC++;
			break;
		}
		case 0xC6: // add a, *
		{
			A += mem[PC + 1];
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xC7: // rst 0x00
		{
			rst(0x00);
			break;
		}
		case 0xC8: // ret z
		{
			ret(zero());
			break;
		}
		case 0xC9: // ret
		{
			ret(true);
			break;
		}
		case 0xCA: // jp z, **
		{
			jp(zero(), get16(), 3);
			break;
		}
		case 0xCB: // EXTENDED INSTRUCTIONS
		{
			decodeExtendedInstruction(mem[PC + 1]);
			break;
		}
		case 0xCC: // call z, **
		{
			call(zero());
			break;
		}
		case 0xCD: // call **
		{
			call(true);
			break;
		}
		case 0xCE: // adc a, *
		{
			A += mem[PC + 1] + carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xCF: // rst 0x08
		{
			rst(0x08);
			break;
		}
		case 0xD0: // ret nc
		{
			ret(!carry());
			break;
		}
		case 0xD1: // pop de
		{
			E = mem[SP];
			SP++;
			D = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xD2: // jp nc, **
		{
			jp(!carry(), get16(), 3);
			break;
		}
		case 0xD3: // NOP
		{
			PC++;
			break;
		}
		case 0xD4: // call nc, **
		{
			call(!carry());
			break;
		}
		case 0xD5: // push de
		{
			SP--;
			mem[SP] = D;
			SP--;
			mem[SP] = E;
			PC++;
			break;
		}
		case 0xD6: // sub *
		{
			A -= mem[PC + 1];
			updateCarry(A);
			updateN(SUB);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xD7: // rst 0x10
		{
			rst(0x10);
			break;
		}
		case 0xD8: // ret c
		{
			ret(carry());
			break;
		}
		case 0xD9: // reti 
		{
			ret(true);
			IME = true;
			//PC++; //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			break;
		}
		case 0xDA: // jp c, **
		{
			jp(carry(), get16(), 3);
			break;
		
		}
		case 0xDB: // in a, (*) ~!GB
		{
			//A = ports[mem[PC + 1]];
			PC += 2;
			break;
		}
		case 0xDC: // call c, **
		{
			call(carry());
			break;
		}
		case 0xDD: // IX INSTRUCTIONS ~!GB
		{
			PC += 2;
			break;
		}
		case 0xDE: // sbc a, *
		{
			A -= mem[PC + 1] - carry();
			updateCarry(A);
			updateN(ADD);
			updateOverflow(A);
			updateHC(A);
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xDF: // rst 0x18
		{
			rst(0x18);
			break;
		}
		case 0xE0: //ld (0xFF00 + n), a
		{
			mem[0xFF00 + mem[PC + 1]] = A;
			if ((unsigned char )mem[PC + 1] == 0x46)
			{
				dma();
			}
			PC += 2;
			break;
		}
		case 0xE1: // pop hl
		{
			L = mem[SP];
			SP++;
			H = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xE2: // ld (0xFF00 + C), a
		{
			mem[0xFF00 + C] = A;
			PC++;
			break;
		}
		case 0xE3: // NOP
		{
			PC++;
			break;
		}
		case 0xE4: // call po, **
		{
			call(!overflow());
			break;
		}
		case 0xE5: // push hl
		{
			SP--;
			mem[SP] = H;
			SP--;
			mem[SP] = L;
			PC++;
			break;
		}
		case 0xE6: // and *
		{
			A &= mem[PC + 1];
			resetCarry();
			resetN();
			updateParity(A);
			setHC();
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xE7: // rst 0x20
		{
			rst(0x20);
			break;
		}
		case 0xE8: // add sp, *
		{
			SP += mem[PC + 1];
			PC += 2;
			resetZero();
			resetN();
			updateHC(PC);
			updateCarry(PC);
			break;
		}
		case 0xE9: // jp (hl)
		{
			jp(true, (unsigned short)HL(), 1);
			break;
		}
		case 0xEA: // ld (**), a
		{
			mem[get16()] = A;
			PC += 3;
			break;
		}
		case 0xEB: // ex de, hl ~!GB
		{
			const short de = DE();
			DE(HL()); // swap de = hl
			HL(de); // hl = de
			PC++;
			break;
		}
		case 0xEC: // call pe, **
		{
			call(overflow());
			break;
		}
		case 0xED: // EXTENDED INSTRUCTIONS ~!GB
		{
			PC += 2;
			break;
		}
		case 0xEE: // xor *
		{
			A ^= mem[PC + 1];
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xEF: // rst 0x28
		{
			rst(0x28);
			break;
		}
		case 0xF0: //ld a, (0xFF00 + n) or ldh, (*)
		{
			A = mem[(unsigned short)(0xFF00 + mem[PC + 1])];
			PC += 2;
			break;
		}
		case 0xF1: // pop af
		{
			F = mem[SP];
			SP++;
			A = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xF2: // ld a, (0xFF00 + c)
		{
			A = mem[0xFF00 + C];
			PC++;
			break;
		}
		case 0xF3: // di ^^^
		{
			IME = false;
			PC++;
			break;
		}
		case 0xF4: // call p, **
		{
			call(overflow());
			break;
		}
		case 0xF5: // push af
		{
			SP--;
			A = mem[SP];
			SP--;
			F = mem[SP];
			PC++;
			break;
		}
		case 0xF6: // or *
		{
			A |= mem[PC + 1];
			resetCarry();
			resetN();
			updateParity(A);
			resetHC();
			updateZero(A);
			updateSign(A);
			PC += 2;
			break;
		}
		case 0xF7: // rst 0x30
		{
			rst(0x30);
			break;
		}
		case 0xF8: // ld hl, sp + *
		{
			HL(SP + mem[PC + 1]);
			PC += 2;
			break;
		}
		case 0xF9: // ld sp, hl
		{
			SP = ((L >> 8) & 0xFF);
			SP |= (char)HL();
			PC++;
			break;
		}
		case 0xFA: // ld a, (**)
		{
			A = get16();
			PC += 3;
			break;
		}
		case 0xFB: // ei ^^^
		{
			IME = true;
			PC++;
			break;
		}
		case 0xFC: // call m, **
		{
			call(sign());
			break;
		}
		case 0xFD: // IY INSTRUCTIONS ~!GB
		{
			PC += 2;
			break;
		}
		case 0xFE: // cp *
		{
			cmp((unsigned short)mem[PC + 1]);
			PC += 2;
			break;
		}
		case 0xFF: // rst 0x38
		{
			rst(0x38);
			PC++;
			break;
		}
		default: // just in case the definition of a char changes
		{
			std::cout << "You should never ever see this" << std::endl;
			PC++;
			break;
		}
	}
}

const std::string toHex(const int val)
{
	std::stringstream stream;
	stream << std::hex << (int)val;
	std::string result(stream.str());
	return "0x" + result;
}

int CPU::loadROM(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	char* fileStr;
	std::streampos size;
	if (file.is_open())
	{
		size = file.tellg();
		fileStr = new char[size]; // get rom size
		std::cout << "Seek size: " << size << std::endl;
		file.seekg(0, std::ios::beg);
		file.read(fileStr, size); // load the rom file into the std::string file
		file.close();
	}
	else
	{
		return 1; // file load fail
	}
	// make sure the ROM fits within the memory
	if (size > MAX_ROM_SIZE)
	{
		delete[] fileStr;
		return 2; // ROM too big
	}
	if (fileStr == nullptr)
	{
		delete[] fileStr;
		return 3; // mem alloc failure
	}
	// load the rom into memory starting at 0x00
	for (unsigned i = 0; i < size; i++)
	{
		mem[i] = fileStr[i];
	}
	delete[] fileStr;
	return 0; // ROM load completed succesfully
}