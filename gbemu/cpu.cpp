#include "cpu.h"

const int clockTimes[256] =
{
	4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,
	4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,
	8, 12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,
	8, 12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 9, 4,
	8, 12, 12, 12, 12, 16, 8, 16, 8, 4, 12, 12, 12, 12, 8, 16,
	8, 12, 12, 0, 12, 16, 8, 16, 8, 4, 12, 0, 12, 0, 8, 16,
	12, 12, 8, 0, 0, 16, 8, 16, 16, 4, 16, 0, 0, 0, 8, 16,
	12, 12, 8, 4, 0, 16, 8, 16, 12, 8, 16, 4, 0, 0, 8, 16,
};

CPU::CPU() 
	: internalmem(MEM_SIZE)
{
	keyInfo = { { 0x0F, 0x0F }, 0x0 };
	reset();
}

CPU::~CPU()
{
	
}

void CPU::reset()
{
	// initialize all mem to 0
	for (int i = 0; i < MEM_SIZE; i++)
	{
		internalmem[i] = 0;
	}
	// some known starting values of registers
	A = 0x01;
	F = 0xB0;
	BC(0x13);
	DE(0xD8);
	HL(0x14D);
	SP = 0xFFFE;
	PC = 0x100;
	// set memory registers to their known starting values
	internalmem[TIMA] = 0x00;
	internalmem[TMA] = 0x00;
	internalmem[TAC] = 0x00;
	internalmem[NR10] = 0x80;
	internalmem[NR11] = 0xBF;
	internalmem[NR12] = 0xF3;
	internalmem[NR14] = 0xBF;
	internalmem[NR21] = 0x3F;
	internalmem[NR22] = 0x00;
	internalmem[NR24] = 0xBF;
	internalmem[NR30] = 0x7F;
	internalmem[NR31] = 0xFF;
	internalmem[NR32] = 0x9F;
	internalmem[NR33] = 0xBF;
	internalmem[NR41] = 0xFF;
	internalmem[NR42] = 0x00;
	internalmem[NR43] = 0x00;
	internalmem[NR30] = 0xBF;
	internalmem[NR50] = 0x77;
	internalmem[NR51] = 0xF3;
	internalmem[NR52] = 0xF1;
	internalmem[LCDC] = 0x91;
	internalmem[SCY] = 0x00;
	internalmem[SCX] = 0x00;
	internalmem[LYC] = 0x00;
	internalmem[BGP] = 0xFC;
	internalmem[OBP0] = 0xFC;
	internalmem[OBP1] = 0xFF;
	internalmem[WY] = 0x00;
	internalmem[WX] = 0x00;
	internalmem[IE] = 0x00;
	internalmem[LY] = 0x94;
}

#pragma region FlagFuncs

void CPU::updateCarry(uint16_t newVal)
{
	if (newVal & 0x1000)
	{
		setCarry();
	}
	else
	{
		resetCarry();
	}
}

void CPU::resetCarry()
{
	F &= 0xFE;
}

void CPU::setCarry()
{
	F |= 0x1;
}

void CPU::updateHC(byte prevVal, byte newVal)
{
	if ((prevVal & b3) != (newVal & b4) && (newVal & b4))
	{
		setHC();
	}
	else
	{
		resetHC();
	}
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
	if (add == SUB) { F |= 0x2; }
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

void CPU::updateZero(reg16 reg)
{
	if (reg == 0)
	{
		setZero();
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

void CPU::rlc(reg& val)
{
	byte bit7 = ((val & b7) >> 7) & b0;
	val <<= 1;
	F |= bit7;
	val |= bit7;
	resetN();
	resetHC();
	updateZero(val);
}

void CPU::rrc(reg& val)
{
	byte bit0 = val & b0;
	val >>= 1;
	F |= bit0;
	val |= (bit0 << 7);
	updateZero(val);
	resetHC();
	resetN();
}

void CPU::rl(reg& val)
{
	byte bit7 = ((val & b7) >> 7) & b0;
	byte carryCpy = F & b0;
	val <<= 1;
	F |= bit7;
	val |= carryCpy;
	updateZero(val);
	resetHC();
	resetN();
}

void CPU::rr(reg& val)
{
	byte bit0 = val & b0;
	byte carryCpy = F & b0;
	val >>= 1;
	F |= bit0;
	val &= ~b7;
	val |= carryCpy;
	updateZero(val);
	resetHC();
	resetN();
}

void CPU::sla(reg& val)
{
	byte bit7 = ((val & b7) >> 7) & b0;
	val <<= 1;
	F |= bit7;
	updateZero(val);
	resetN();
	resetHC();
}

void CPU::sra(reg& val)
{
	byte bit0 = val & b0;
	val >>= 1;
	F |= bit0;
	updateZero(val);
	resetN();
	resetHC();
}

void CPU::srl(reg& val)
{
	byte bit0 = val & b0;
	val >>= 1;
	F |= bit0;
	val &= ~b7;
	updateZero(val);
	resetN();
	resetHC();
}

void CPU::bit(reg r, ubyte bit)
{
	if (r & bit)
	{
		resetZero();
	}
	else
	{
		setZero();
	}
	setHC();
	resetN();
}

void CPU::res(reg& r, ubyte bit)
{
	r &= ~bit;
}

void CPU::set(reg& r, ubyte bit)
{
	r |= bit;
}

void CPU::swap(reg& r)
{
	r = ((r & 0x0F) << 4 | (r & 0xF0) >> 4);
	updateZero(r);
	resetN();
	resetHC();
	resetCarry();
}

void CPU::emulateBitInstruction(ubyte opcode)
{
	reg* op1s[] = { &B, &C, &D, &E, &H, &L, gByte(static_cast<addr16>(HL())), &A, 
					&B, &C, &D, &E, &H, &L, gByte(static_cast<addr16>(HL())), &A };
	uint8_t op2;

	if (opcode >= 0x40) // instructions from 0x40-0xFF have non register,
	{					// constant operands
		// those operands go in order from b0-b7 with two bits per 16 byte
		// row in the instruction table
		// first bit is low byte 0x00-0x07
		uint8_t op2sLo[] = { b0, b2, b4, b6 };
		// second bit is high byte 0x08-0x0F
		uint8_t op2sHi[] = { b1, b3, b5, b7 };
		const int op2Index = ((opcode >> 0x4) & 0x0F) - 4;
		if ((opcode & 0x0F) < 0x8)
		{
			op2 = op2sLo[op2Index];
		}
		else
		{
			op2 = op2sHi[op2Index];
		}
	}

	reg& op1 = *op1s[opcode & 0x0F];

	switch (opcode & 0xF0)
	{
		case 0x00:
		{
			if ((opcode & 0x0F) < 0x8)
				rlc(op1);
			else
				rrc(op1);
			break;
		}
		case 0x10:
		{
			if ((opcode & 0x0F) < 0x8)
				rl(op1);
			else
				rr(op1);
			break;
		}
		case 0x20:
		{
			if ((opcode & 0x0F) < 0x8)
				sla(op1);
			else
				sra(op1);
			break;
		}
		case 0x30:
		{
			if ((opcode & 0x0F) < 0x8)
				swap(op1);
			else
				srl(op1);
			break;
		}
		case 0x40: case 0x50: case 0x60: case 0x70:
		{
			bit(op1, op2);
			break;
		}
		case 0x80: case 0x90: case 0xA0: case 0xB0:
		{
			res(op1, op2);
			break;
		}
		case 0xC0: case 0xD0: case 0xE0: case 0xF0:
		{
			set(op1, op2);
			break;
		}
	}
	PC += 2; // all 0xCB instructions are 2 bytes long
}

void CPU::cmp(const byte val)
{
	updateCarry(A - val);
	updateN(SUB);
	updateHC(A, A - val);
	updateZero(A - val);
	PC++;
}

void CPU::dec(byte& b)
{
	const byte before = b;
	b--;
	updateZero(b);
	updateN(SUB);
	updateHC(before, b);
	PC++;
}

void CPU::inc(byte& b)
{
	const byte before = b;
	b++;
	updateZero(b);
	updateN(ADD);
	updateHC(before, b);
	PC++;
}

void CPU::add(byte val)
{
	const byte before = A;
	updateCarry(A + val);
	A += val;
	updateN(ADD);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::adc(byte val)
{
	const byte before = A;
	updateCarry(A + val + carry());
	A += val + carry();
	updateN(ADD);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::sub(byte val)
{
	const byte before = A;
	updateCarry(A - val);
	A -= val;
	updateN(SUB);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::sbc(byte val)
{
	const byte before = A;
	updateCarry(A - (val + carry()));
	A -= (val + carry());
	updateN(SUB);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::andr(byte val)
{
	A &= val;
	resetCarry();
	resetN();
	setHC();
	updateZero(A);
	PC++;
}

void CPU::xorr(byte val)
{
	A ^= val;
	resetCarry();
	resetN();
	resetHC();
	updateZero(A);
	PC++;
}

void CPU::orr(byte val)
{
	A |= val;
	resetCarry();
	resetN();
	resetHC();
	updateZero(A);
	PC++;
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

bool interrupted = false;

void CPU::ret(bool cond)
{
	if (cond)
	{
		PC = pop(); // pop PC off the stack
		clockCycles += 12;
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
		wByte(SP, ((PC + 3) & 0xFF));
		SP--;
		wByte(SP, (((PC + 3) >> 8) & 0xFF));
		PC = getNextWord();
		clockCycles += 12;
	}
	else
	{
		PC += 3;
	}
}

void CPU::push(reg16 val)
{
	SP--;
	wByte(SP, val & 0xFF);
	SP--;
	wByte(SP, val >> 0x8);
}

reg16 CPU::pop()
{
	reg16 ret = 0;
	ret = (rByte(SP) & 0xFF) << 8;
	SP++;
	ret |= rByte(SP) & 0xFF;
	SP++;
	return ret;
}

inline void CPU::rst(const uint8_t mode)
{
	push(PC + 1);
	PC = mode;
}

// jrs PC to [to] if cond is true
// Else it increases PC by [opsize]
inline void CPU::jr(bool cond, int8_t to, uint8_t opsize)
{
	if (cond)
	{
		PC += to + opsize; // + opsize is to jump over the opsize in bytes
		clockCycles += 4; // 5 more cycles added (total of 12) for jump
	}
	else
	{
		PC += opsize;
	}
}

void CPU::jp(bool cond, addr16 to, uint8_t opsize)
{
	if (cond)
	{
		PC = to;
		clockCycles += 4;
	}
	else
	{
		PC += opsize;
	}
}

template<bool memread>
void CPU::ld8(reg& dst, reg src)
{
	dst = src;
	if (memread)
	{
		PC += 2;
		return;
	}
	PC++;
}

template<bool memread>
void CPU::ld16(reg& hi, reg& lo, reg16 src)
{
	hi = (src >> 0x8) & 0xFF;
	lo = src & 0xFF;
	if (memread)
	{
		PC += 3;
		return;
	}
	PC++;
}

#pragma endregion

#ifdef DEBUG

void printMem(CPU* cpu, int start, int end)
{
	for (int i = start; i <= end; i++)
	{
		std::cout << toHex(cpu->rByte(i)) << "\tat" << toHex(i) << "\n";
	}
	std::cout << "\n\n";
}

void CPU::dumpCPU()
{
	std::cout << "A: " << toHex((byte)A) << std::endl;
	std::cout << "B: " << toHex((byte)B) << std::endl;
	std::cout << "C: " << toHex((byte)C) << std::endl;
	std::cout << "D: " << toHex((byte)D) << std::endl;
	std::cout << "E: " << toHex((byte)E) << std::endl;
	std::cout << "F: " << toHex((byte)F) << std::endl;
	std::cout << "AF: " << toHex(AF()) << std::endl;
	std::cout << "BC: " << toHex(BC()) << std::endl;
	std::cout << "DE: " << toHex(DE()) << std::endl;
	std::cout << "HL: " << toHex(HL()) << std::endl;
	std::cout << "PC: " << toHex(PC) << std::endl;
	std::cout << "SP: " << toHex(SP) << std::endl;
}

#endif // DEBUG

void CPU::dma()
{
	const addr16 dmaStart = A << 0x8; // get the location that the DMA will be copying from
	for (int i = 0; i < 0x8C; i++) // copy the 0x8C bytes from dmaStart to the OAM
	{
		internalmem[OAM + i] = rByte(dmaStart + i);
	}
}

void CPU::interrupt(const byte loc)
{
	push(PC); // push the program counter onto the stack
	PC = loc; // jump to the interrupt location
	interrupted = true;
	IME = false; // disable interrupts
	wByte(IF, 0x0);
}

static bool x = false;
static bool u = false;

void CPU::handleInterrupts()
{
	const byte intEnable = rByte(IE);
	const byte intFlag = rByte(IF);
	if (IME) // are interrupts enabled?
	{
		if ((intEnable & b0) && (intFlag & b0)) // vblank DONE
		{
			const int vBlankInt = 0x40;
			interrupt(vBlankInt);
		}
		else if ((intEnable & b1) && (intFlag & b1)) // LCDC (STAT) interrupt TODO
		{
			const int LCDStatInt = 0x48;
			interrupt(LCDStatInt);
		}
		else if ((intEnable & b2) && (intFlag & b2)) // timer overflow TODO
		{
			const int timerOverflowInt = 0x50;
			interrupt(timerOverflowInt);
		}
		else if ((intEnable & b3) && (intFlag & b3)) // serial I/O transfer complete TODO
		{
			const int serialInt = 0x58;
			interrupt(serialInt);
		}
		else if ((intEnable & b4) && (intFlag & b4)) // Transition from High to Low of Pin number P10-P13 TODO
		{
			const int hiToLoP10_P13 = 0x60;
			interrupt(hiToLoP10_P13);
		}
	}
}

byte CPU::rByte(addr16 addr) const
{
	if (isInternalMem(addr)) 
	{
		return internalmem[addr];
	}
	else
	{ 
		return cart.rByte(addr);
	}
}

byte* CPU::gByte(addr16 addr)
{
	if (isInternalMem(addr))
	{
		return &internalmem[addr];
	}
	else
	{
		return cart.gByte(addr);
	}
}

void CPU::wByte(addr16 addr, byte val)
{
	if (isInternalMem(addr))
	{
		internalmem[addr] = val;
		if (addr >= 0xC000 && addr <= 0xDE00)
		{
			internalmem[addr + 0x2000] = val; // emulate mirroring of RAM
		}
		else if (addr >= 0xE000 && addr <= 0xFE00)
		{
			internalmem[addr - 0x2000] = val; // emulate mirroring of RAM
		}
	}
	else
	{
		std::cout << "wbyte PC = " << toHex(PC) << std::endl;
		cart.wByte(addr, val);
	}
}

void CPU::wWord(addr16 addr, word val)
{
	if (isInternalMem(addr))
	{
		internalmem[addr] = val & 0x00FF; // lower byte
		internalmem[addr + 1] = ((val & 0xFF00) >> 8) & 0xFF; // upper byte
	}
	else
	{
		cart.wWord(addr, val);
	}
}

#ifdef DEBUG
void CPU::test()
{

}

static int ii = 0;
static bool p = 0;

#endif // DEBUG

void CPU::emulateCycle()
{
	updateInterrupts();
	handleInterrupts();
	ubyte opcode = rByte(PC); // get next opcode
	clockCycles += clockTimes[opcode];
	emulateInstruction(opcode);
}

void CPU::emulateInstruction(ubyte opcode)
{

	//if (PC == 0x2075)
	//{
	//	p = true;
	//}
	//
	//if (p && ii < 100)
	//{
	//	std::cout << toHex(opcode) << "\tat " << toHex(PC) << "\n";
	//	ii++;
	//}
	//else if (p)
	//{
	//	//system("pause");
	//	ii = 0;
	//}

	if (_test)
	{
		std::cout << toHex(opcode) << "\tat " << toHex(PC) << "\n";
	}

	/// emulate opcodes 0x40-0xBF (not including 0x76)
	if ((opcode & 0xFF) >= 0x40 && (opcode & 0xFF) <= 0xBF && (opcode & 0xFF) != 0x76) // 0x76 is halt
	{
		reg srcs[] = { B, C, D, E, H, L, rByte(static_cast<addr16>(HL())), A,
					   B, C, D, E, H, L, rByte(static_cast<addr16>(HL())), A };
		reg src = srcs[opcode & 0x0F];
		switch (opcode & 0xF0)
		{
			case 0x40: case 0x50: case 0x60: case 0x70: // ld r8, o8
			{
				reg* dst;

				const int dstIndex = ((opcode >> 0x4) & 0x0F) - 4;

				// in a single "row" of 16 instructions on an z80 instruction table
				// there are 2 destinations, 1 in the first 8 and 1 in the second 8
				if ((opcode & 0x0F) < 0x8) // low "row" operands are B, D, H, (HL)
				{
					reg* dstsLo[] = { &B, &D, &H, gByte(static_cast<addr16>(HL())) };
					dst = dstsLo[dstIndex];
				}
				else // right "row" operands are C, E, L, A
				{
					reg* dstsHi[] = { &C, &E, &L, &A };
					dst = dstsHi[dstIndex];
				}

				ld8<0>(*dst, src);
				break;
			}
			case 0x80:
			{
				if ((opcode & 0x0F) < 0x08) // add a, o8
					add(src);
				else // adc a, o8
					adc(src);
				break;
			}
			case 0x90:
			{
				if ((opcode & 0x0F) < 0x08) // sub o8
					sub(src);
				else // sbc a, o8
					sub(src);
				break;
			}
			case 0xA0:
			{
				if ((opcode & 0x0F) < 0x08) // and o8
					andr(src);
				else // xor o8
					xorr(src);
				break;
			}
			case 0xB0:
			{
				if ((opcode & 0x0F) < 0x08) // or o8
					orr(src);
				else // cp o8
					cmp(src);
				break;
			}
		}
		return;
	}

	/// emulate all other opcodes
	switch (opcode & 0xFF)
	{
		case 0x00: // NOP
		{
			PC++;
			break;
		}
		case 0x01: // ld BC, **
		{
			ld16<1>(B, C, getNextWord());
			break;
		}
		case 0x02: // ld (BC), a
		{
			wByte(static_cast<addr16>(BC()), A);
			PC++;
			break;
		}
		case 0x03: // inc BC
		{
			const reg16 bc = BC();
			BC(bc + 1);
			PC++;
			break;
		}
		case 0x04: // inc b
		{
			inc(B);
			break;
		}
		case 0x05: // dec b
		{
			dec(B);
			break;
		}
		case 0x06: // ld b, *
		{
			B = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x07: // rlca
		{
			const byte before = A;
			updateCarry(A << 1);
			A <<= 1;
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x08: // ld (**), sp
		{
			std::cout << "asfsaffsdaf453" << std::endl;
			std::cout << toHex(PC) << std::endl;
			system("pause");
			wWord(getNextWord(), SP);
			PC += 3;
			break;
		}
		case 0x09: // add hl, bc
		{
			const reg16 hl = HL();
			HL(BC() + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(hl, HL());
			PC++;
			break;
		}
		case 0x0A: // ld a, (BC)
		{
			A = rByte(static_cast<addr16>(BC()));
			PC++;
			break;
		}
		case 0x0B: // dec BC
		{
			const reg16 bc = BC();
			BC(bc - 1);
			PC++;
			break;
		}
		case 0x0C: // inc C
		{
			inc(C);
			break;
		}
		case 0x0D: // dec C
		{
			dec(C);
			break;
		}
		case 0x0E: // ld c, *
		{
			C = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x0F: // rrca
		{
			const byte before = A;
			updateCarry(A >> 1);
			A >>= 1;
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x10: // STOP
		{
			stop();
			PC++;
			break;
		}
		case 0x11: // ld de, **
		{
			DE(getNextWord());
			PC += 3;
			break;
		}
		case 0x12: // ld (de), a
		{
			wByte(static_cast<addr16>(DE()), A);
			PC++;
			break;
		}
		case 0x13: // inc de
		{
			const reg16 de = DE();
			DE(de + 1);
			PC++;
			break;
		}
		case 0x14: // inc d
		{
			inc(D);
			break;
		}
		case 0x15: // dec d
		{
			dec(D);
			break;
		}
		case 0x16: // ld d, *
		{
			D = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x17: // rla
		{
			const byte before = A;
			updateCarry(A << 1);
			A <<= 1;
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x18: // jr *
		{
			jr(true, rByte(PC + 1), 2);
			clockCycles -= 5;
			break;
		}
		case 0x19: // add hl, de
		{
			const reg16 hl = HL();
			HL(hl + DE());
			updateCarry(HL());
			updateN(ADD);
			updateHC(hl, HL());
			PC++;
			break;
		}
		case 0x1A: // ld a, (de)
		{
			A = rByte(static_cast<addr16>(DE()));
			PC++;
			break;
		}
		case 0x1B: // dec de
		{
			const reg16 de = DE();
			DE(de - 1);
			PC++;
			break;
		}
		case 0x1C: // inc e
		{
			inc(E);
			break;
		}
		case 0x1D: // dec e
		{
			dec(E);
			break;
		}
		case 0x1E: // ld e, *
		{
			E = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x1F: // rra
		{
			const reg before = A;
			updateCarry(A >> 1);
			A >>= 1;
			resetN();
			resetHC();
			PC++;
			break;
		}
		case 0x20: // jr nz, *
		{
			jr(!zero(), rByte(PC + 1), 2);
			break;
		}
		case 0x21: // ld hl, **
		{
			HL(getNextWord());
			PC += 3;
			break;
		}
		case 0x22: // ldi (hl), a   or   ld (hl+), a
		{
			wByte(static_cast<addr16>(HL()), A);
			const reg16 hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x23: // inc hl
		{
			const reg16 hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x24: // inc h
		{
			inc(H);
			break;
		}
		case 0x25: // dec h
		{
			dec(H);
			break;
		}
		case 0x26: // ld h, *
		{
			H = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x27: // daa
		{
#ifdef DEBUG
			std::cout << "daa" << std::endl;
			std::cout << "Before A = " << toHex((uint16_t)A) << std::endl;
#endif // DEBUG
			// implementation from http://www.worldofspectrum.org/faq/reference/z80reference.htm
			const reg before = A;
			byte correction = 0x0;
			if (A > 0x99 || carry())
			{
				correction |= 0x60;
				setCarry();
			}
			else
			{
				correction = 0x0;
				resetCarry();
			}
			if (((A & 0x0F) > 0x9) || half_carry())
			{
				correction |= 0x6;
			}
			if (!N())
			{
				A += correction;
			}
			else
			{
				A -= correction;
			}
			updateZero(A);
			updateHC(before, A);
			PC++;
#ifdef DEBUG
			std::cout << "After A = " << toHex((uint16_t)A) << std::endl;
#endif // DEBUG
			break;
		}
		case 0x28: // jr z, *
		{
			jr(zero(), rByte(PC + 1), 2);
			break;
		}
		case 0x29: // add hl, hl
		{
			const reg16 hl = HL();
			HL(hl + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(hl, HL());
			PC++;
			break;
		}
		case 0x2A: // ldi a, (hl) 
		{
			A = rByte(static_cast<addr16>(HL()));
			const reg16 hl = HL();
			HL(hl + 1);
			PC++;
			break;
		}
		case 0x2B: // dec hl
		{
			const reg16 hl = HL();
			HL(hl - 1);
			PC++;
			break;
		}
		case 0x2C: // inc l
		{
			inc(L);
			break;
		}
		case 0x2D: // dec l
		{
			dec(L);
			break;
		}
		case 0x2E: // ld l, *
		{
			L = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x2f: // cpl
		{
			A = ~A;
			PC++;
			break;
		}
		case 0x30: // jr nc, *
		{
			jr(!carry(), rByte(PC + 1), 2);
			break;
		}
		case 0x31: // ld sp, **
		{
			SP = getNextWord();
			PC += 3;
			break;
		}
		case 0x32: // ldd (hl), a
		{
			wByte(static_cast<addr16>(HL()), A);
			const reg16 hl = HL();
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
		case 0x34: // inc (hl) 
		{
			byte val = rByte(static_cast<addr16>(HL()));
			inc(val);
			wByte(static_cast<addr16>(HL()), val);
			break;
		}
		case 0x35: // dec (hl)
		{
			byte val = rByte(static_cast<addr16>(HL()));
			dec(val);
			wByte(static_cast<addr16>(HL()), val);
			break;
		}
		case 0x36: // ld (hl), *
		{
			wByte(static_cast<addr16>(HL()), rByte(PC + 1));
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
			jr(carry(), rByte(PC + 1), 2);
			break;
		}
		case 0x39: // add hl, sp
		{
			const reg16 hl = HL();
			HL(hl + SP);
			updateCarry(HL());
			updateN(ADD);
			updateHC(hl, HL());
			PC++;
			break;
		}
		case 0x3A: // ldd a, (hl)
		{
			A = rByte(static_cast<addr16>(HL()));
			const reg16 hl = HL();
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
			inc(A);
			break;
		}
		case 0x3D: // dec a
		{
			dec(A);
			break;
		}
		case 0x3E: // ld a, *
		{
			A = rByte(PC + 1);
			PC += 2;
			break;
		}
		case 0x3F: // ccf inverts carry
		{
			if (carry())
			{
				resetCarry();
			}
			else
			{
				setCarry();
			}
			PC++;
			break;
		}
		case 0x76: // halt
		{
			halt();
			PC++;
			break;
		} 
		case 0xC0: // ret nz
		{
			ret(!zero());
			break;
		}
		case 0xC1: // pop bc
		{
			BC(pop());
			PC++;
			break;
		}
		case 0xC2: // jp nz, ** 
		{
			jp(!zero(), getNextWord(), 3);
			break;
		}
		case 0xC3: // jp **
		{
			jp(true, getNextWord(), 3);
			break;
		}
		case 0xC4: // call nz, ** 
		{
			call(!zero());
			break;
		}
		case 0xC5: // push bc
		{
			push(BC());
			PC++;
			break;
		}
		case 0xC6: // add a, *
		{
			add(rByte(PC + 1));
			PC++; // 2 byte opcode
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
			jp(zero(), getNextWord(), 3);
			break;
		}
		case 0xCB: // BIT INSTRUCTIONS
		{
			emulateBitInstruction(rByte(PC + 1));
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
			adc(rByte(PC + 1));
			PC++;
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
			DE(pop());
			PC++;
			break;
		}
		case 0xD2: // jp nc, **
		{
			jp(!carry(), getNextWord(), 3);
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
			push(DE());
			PC++;
			break;
		}
		case 0xD6: // sub *
		{
			sub(rByte(PC + 1));
			PC++;
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
			clockCycles -= 4;
			IME = true;
			break;
		}
		case 0xDA: // jp c, **
		{
			jp(carry(), getNextWord(), 3);
			break;
		}
		case 0xDB: // in a, (*) ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xDC: // call c, **
		{
			call(carry());
			break;
		}
		case 0xDD: // IX INSTRUCTIONS ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xDE: // sbc a, *
		{
			sbc(rByte(PC + 1));
			PC++;
			break;
		}
		case 0xDF: // rst 0x18
		{
			rst(0x18);
			break;
		}
		case 0xE0: //ld (0xFF00 + n), a
		{
			const ubyte low = static_cast<ubyte>(rByte(PC + 1)) & 0xFF;
			const addr16 addr = 0xFF00 + low;
			wByte(addr, A);

			if (low == 0x46) // dma
			{
				dma();
			}
			else if (low == 0x00) // joypad write
			{
				keyInfo.colID = A;
			}
			PC += 2;
			break;
		}
		case 0xE1: // pop hl
		{
			HL(pop());
			PC++;
			break;
		}
		case 0xE2: // ld (C), a
		{
			const addr16 addr = static_cast<ubyte>(C) + 0xFF00;
			wByte(addr, A);
			PC++;
			break;
		}
		case 0xE3: // NOP
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
#endif // DEBUG
			break;
		}
		case 0xE4: // call po, **
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
#endif // DEBUG
			break;
		}
		case 0xE5: // push hl
		{
			push(HL());
			PC++;
			break;
		}
		case 0xE6: // and *
		{
			andr(rByte(PC + 1));
			PC++;
			break;
		}
		case 0xE7: // rst 0x20
		{
			rst(0x20);
			break;
		}
		case 0xE8: // add sp, *
		{
			const addr16 before = SP;
			SP += rByte(PC + 1);
			resetZero();
			resetN();
			updateHC(before, SP);
			updateCarry(SP);
			PC += 2;
			break;
		}
		case 0xE9: // jp (hl)
		{
			PC = HL();
			break;
		}
		case 0xEA: // ld (**), a
		{
			wByte(getNextWord(), A);
			PC += 3;
			break;
		}
		case 0xEB: // ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xEC: // ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xED: // EXTENDED INSTRUCTIONS ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xEE: // xor *
		{
			xorr(rByte(PC + 1));
			PC++;
			break;
		}
		case 0xEF: // rst 0x28
		{
			rst(0x28);
			break;
		}
		case 0xF0 : //ld a, (0xFF00 + n) or ldh, (*)
		{
			const ubyte low = static_cast<ubyte>(rByte(PC + 1)); // low byte
			const addr16 addr = 0xFF00 + low; // high byte always 0xFF00

			A = rByte(addr);
			if (low == 0x00) // joypad read
			{
				if (keyInfo.colID == b4)
				{
					A = keyInfo.keys[p15] | keyInfo.colID | 0xC0; // set the upper (unused) bits with 0xC0
				}
				else if (keyInfo.colID == b5)
				{
					A = keyInfo.keys[p14] | keyInfo.colID | 0xC0;
				}
			}
			PC += 2;
			break;
		}
		case 0xF1: // pop af
		{
			AF(pop());
			PC++;
			break;
		}
		case 0xF2: // ld a, (C)
		{
			const addr16 addr = static_cast<ubyte>(C) + 0xFF00;
			A = rByte(addr);
			PC++;
			break;
		}
		case 0xF3: // di
		{
			IME = false;
			PC++;
			break;
		}
		case 0xF4: // ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xF5: // push af
		{
			push(AF());
			PC++;
			break;
		}
		case 0xF6: // or *
		{
			
			orr(rByte(PC + 1));
			PC++;
			break;
		}
		case 0xF7: // rst 0x30
		{
			rst(0x30);
			break;
		}
		case 0xF8: // ld hl, sp
		{
			HL(SP + rByte(PC + 1));
			PC += 2;
			break;
		}
		case 0xF9: // ld sp, hl
		{
			SP = HL();
			PC++;
			break;
		}
		case 0xFA: // ld a, (**)
		{
			A = rByte(getNextWord());
			PC += 3;
			break;
		}
		case 0xFB: // ei
		{
			IME = true;
			PC++;
			break;
		}
		case 0xFC: // ~!GB
		{
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xFD: // ~!GB
		{
			// da2f
#ifdef DEBUG
			std::cout << "Opcode not supported by Gameboy: " << toHex(opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
#endif // DEBUG
			break;
		}
		case 0xFE: // cp *
		{
			cmp(rByte(PC + 1));
			PC++;
			break;
		}
		case 0xFF: // rst 0x38
		{
			rst(0x38);
			break;
		}
	}
}

int CPU::loadROM(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);
	char* ROMstr;
	std::streampos size;
	if (file.is_open())
	{
		size = file.tellg();
		ROMstr = new char[size]; // get rom size
		std::cout << "Seek size: " << size << std::endl;
		file.seekg(0, std::ios::beg);
		file.read(ROMstr, size); // load the rom file into the file
		file.close();
	}
	else
	{
		return 1; // file load fail
	}
	if (ROMstr == nullptr)
	{
		delete[] ROMstr;
		return 3; // mem alloc failure
	}
	
	cart.init(ROMstr, size);

	delete[] ROMstr;
	return EXIT_SUCCESS; // ROM load completed succesfully
}