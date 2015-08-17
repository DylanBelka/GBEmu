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

void CPU::updateCarry(byte prevVal, byte newVal)
{
	if ((prevVal & b7) != (newVal & b7))
	{
		setCarry();
	}
	else
	{
		resetCarry();
	}
}

void CPU::updateCarry16(word prevVal, word newVal)
{
	const word b15 = 0x8000;
	if ((prevVal & b15) != (newVal & b15))
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

void CPU::emulateExtendedInstruction(byte opcode)
{
	switch (opcode & 0xFF)
	{
	case 0x00: // rlc b
	{
		rlc(B);
		break;
	}
	case 0x01: // rlc c
	{
		rlc(C);
		break;
	}
	case 0x02: // rlc d
	{
		rlc(D);
		break;
	}
	case 0x03: // rlc e
	{
		rlc(E);
		break;
	}
	case 0x04: // rlc h
	{
		rlc(H);
		break;
	}
	case 0x05: // rlc l
	{
		rlc(L);
		break;
	}
	case 0x06: // rlc (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		rlc(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x07: // rlc a
	{
		rlc(A);
		break;
	}
	case 0x08: // rrc b
	{
		rrc(B);
		break;
	}
	case 0x09: // rrc c
	{
		rrc(C);
		break;
	}
	case 0x0A: // rrc d
	{
		rrc(D);
		break;
	}
	case 0x0B: // rrc e
	{
		rrc(E);
		break;
	}
	case 0x0C: // rrc h
	{
		rrc(H);
		break;
	}
	case 0x0D: // rrc l
	{
		rrc(L);
		break;
	}
	case 0x0E: // rrc (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		rrc(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x0F: // rrc a
	{
		rrc(A);
		break;
	}
	case 0x10: // rl b
	{
		rl(B);
		break;
	}
	case 0x11: // rl c
	{
		rl(C);
		break;
	}
	case 0x12: // rl d
	{
		rl(D);
		break;
	}
	case 0x13: // rl e
	{
		rl(E);
		break;
	}
	case 0x14: // rl h
	{
		rl(H);
		break;
	}
	case 0x15: // rl l
	{
		rl(L);
		break;
	}
	case 0x16: // rl (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		rl(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x17: // rl a
	{
		rl(A);
		break;
	}
	case 0x18: // rr b
	{
		rr(B);
		break;
	}
	case 0x19: // rr c
	{
		rr(C);
		break;
	}
	case 0x1A: // rr d
	{
		rr(D);
		break;
	}
	case 0x1B: // rr e
	{
		rr(E);
		break;
	}
	case 0x1C: // rr h
	{
		rr(H);
		break;
	}
	case 0x1D: // rr l
	{
		rr(L);
		break;
	}
	case 0x1E: // rr (hl)
	{
		byte val = static_cast<addr16>(HL());
		rr(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x1F: // rr a
	{
		rr(A);
		break;
	}
	case 0x20: // sla b
	{
		sla(B);
		break;
	}
	case 0x21: // sla c
	{
		sla(C);
		break;
	}
	case 0x22: // sla d
	{
		sla(D);
		break;
	}
	case 0x23: // sla e
	{
		sla(E);
		break;
	}
	case 0x24: // sla h
	{
		sla(H);
		break;
	}
	case 0x25: // sla l 
	{
		sla(L);
		break;
	}
	case 0x26: // sla (hl)
	{
		byte val = static_cast<addr16>(HL());
		sla(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x27: // sla a
	{
		sla(A);
		break;
	}
	case 0x28: // sra b
	{
		sra(B);
		break;
	}
	case 0x29: // sra c
	{
		sra(C);
		break;
	}
	case 0x2A: // sra d
	{
		sra(D);
		break;
	}
	case 0x2B: // sra e
	{
		sra(E);
		break;
	}
	case 0x2C: // sra h
	{
		sra(H);
		break;
	}
	case 0x2D: // sra l
	{
		sra(L);
		break;
	}
	case 0x2E: // sra (hl)
	{
		byte val = static_cast<addr16>(HL());
		sra(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x2F: // sra a
	{
		sra(A);
		break;
	}
	case 0x30: // swap b
	{
		swap(B);
		break;
	}
	case 0x31: // swap c
	{
		swap(C);
		break;
	}
	case 0x32: // swap d
	{
		swap(D);
		break;
	}
	case 0x33: // swap e
	{
		swap(E);
		break;
	}
	case 0x34: // swap h
	{
		swap(H);
		break;
	}
	case 0x35: // swap l
	{
		swap(L);
		break;
	}
	case 0x36: // swap (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		swap(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x37: // swap a
	{
		swap(A);
		break;
	}
	case 0x38: // srl b
	{
		srl(B);
		break;
	}
	case 0x39: // srl c
	{
		srl(C);
		break;
	}
	case 0x3A: // srl d
	{
		srl(D);
		break;
	}
	case 0x3B: // srl e
	{
		srl(E);
		break;
	}
	case 0x3C: // srl h
	{
		srl(H);
		break;
	}
	case 0x3D: // srl l
	{
		srl(L);
		break;
	}
	case 0x3E: // srl (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		srl(val);
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x3F: // srl a
	{
		srl(A);
		break;
	}
	case 0x40: // bit 0, b
	{
		bit(B, b0);
		break;
	}
	case 0x41: // bit 0, c
	{
		bit(C, b0);
		break;
	}
	case 0x42: // bit 0, d
	{
		bit(D, b0);
		break;
	}
	case 0x43: // bit 0, e
	{
		bit(E, b0);
		break;
	}
	case 0x44: // bit 0, h
	{
		bit(H, b0);
		break;
	}
	case 0x45: // bit 0, l
	{
		bit(L, b0);
		break;
	}
	case 0x46: // bit 0, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b0);
		clockCycles += 8;
		break;
	}
	case 0x47: // bit 0, a
	{
		bit(A, b0);
		break;
	}
	case 0x48: // bit 1, b
	{
		bit(B, b1);
		break;
	}
	case 0x49: // bit 1, c
	{
		bit(C, b1);
		break;
	}
	case 0x4A: // bit 1, d
	{
		bit(D, b1);
		break;
	}
	case 0x4B: // bit 1, e
	{
		bit(E, b1);
		break;
	}
	case 0x4C: // bit 1, h
	{
		bit(H, b1);
		break;
	}
	case 0x4D: // bit 1, l
	{
		bit(L, b1);
		break;
	}
	case 0x4E: // bit 1, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b1);
		clockCycles += 8;
		break;
	}
	case 0x4F: // bit 1, a
	{
		bit(A, b1);
		break;
	}
	case 0x50: // bit 2, b
	{
		bit(B, b2);
		break;
	}
	case 0x51: // bit 2, c
	{
		bit(C, b2);
		break;
	}
	case 0x52: // bit 2, d
	{
		bit(D, b2);
		break;
	}
	case 0x53: // bit 2, e
	{
		bit(E, b2);
		break;
	}
	case 0x54: // bit 2, h
	{
		bit(H, b2);
		break;
	}
	case 0x55: // bit 2, l
	{
		bit(L, b2);
		break;
	}
	case 0x56: // bit 2, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b2);
		clockCycles += 8;
		break;
	}
	case 0x57: // bit 2, a
	{
		bit(A, b2);
		break;
	}
	case 0x58: // bit 3, b
	{
		bit(B, b3);
		break;
	}
	case 0x59: // bit 3, c
	{
		bit(C, b3);
		break;
	}
	case 0x5A: // bit 3, d
	{
		bit(D, b3);
		break;
	}
	case 0x5B: // bit 3, e
	{
		bit(E, b3);
		break;
	}
	case 0x5C: // bit 3, h
	{
		bit(H, b3);
		break;
	}
	case 0x5D: // bit 3, l
	{
		bit(L, b3);
		break;
	}
	case 0x5E: // bit 3, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b3);
		clockCycles += 8;
		break;
	}
	case 0x5F: // bit 3, a
	{
		bit(A, b3);
		break;
	}
	case 0x60: // bit 4, b
	{
		bit(B, b4);
		break;
	}
	case 0x61: // bit 4, c
	{
		bit(C, b4);
		break;
	}
	case 0x62: // bit 4, d
	{
		bit(D, b4);
		break;
	}
	case 0x63: // bit 4, e
	{
		bit(E, b4);
		break;
	}
	case 0x64: // bit 4, h
	{
		bit(H, b4);
		break;
	}
	case 0x65: // bit 4, l
	{
		bit(L, b4);
		break;
	}
	case 0x66: // bit 4, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b3);
		clockCycles += 8;
		break;
	}
	case 0x67: // bit 4, a
	{
		bit(A, b4);
		break;
	}
	case 0x68: // bit 5, b
	{
		bit(B, b5);
		break;
	}
	case 0x69: // bit 5, c
	{
		bit(C, b5);
		break;
	}
	case 0x6A: // bit 5, d
	{
		bit(D, b5);
		break;
	}
	case 0x6B: // bit 5, e
	{
		bit(E, b5);
		break;
	}
	case 0x6C: // bit 5, h
	{
		bit(H, b5);
		break;
	}
	case 0x6D: // bit 5, l
	{
		bit(L, b5);
		break;
	}
	case 0x6E: // bit 5, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b5);
		clockCycles += 8;
		break;
	}
	case 0x6F: // bit 5, a
	{
		bit(A, b5);
		break;
	}
	case 0x70: // bit 6, b
	{
		bit(B, b6);
		break;
	}
	case 0x71: // bit 6, c
	{
		bit(C, b6);
		break;
	}
	case 0x72: // bit 6, d
	{
		bit(D, b6);
		break;
	}
	case 0x73: // bit 6, e
	{
		bit(E, b6);
		break;
	}
	case 0x74: // bit 6, h
	{
		bit(H, b6);
		break;
	}
	case 0x75: // bit 6, l
	{
		bit(L, b6);
		break;
	}
	case 0x76: // bit 6, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b6);
		clockCycles += 8;
		break;
	}
	case 0x77: // bit 6, a
	{
		bit(A, b6);
		break;
	}
	case 0x78: // bit 7, b
	{
		bit(B, b7);
		break;
	}
	case 0x79: // bit 7, c
	{
		bit(C, b7);
		break;
	}
	case 0x7A: // bit 7, d
	{
		bit(D, b7);
		break;
	}
	case 0x7B: // bit 7, e
	{
		bit(E, b7);
		break;
	}
	case 0x7C: // bit 7, h
	{
		bit(H, b7);
		break;
	}
	case 0x7D: // bit 7, l
	{
		bit(L, b7);
		break;
	}
	case 0x7E: // bit 7, (hl)
	{
		bit(rByte(static_cast<addr16>(HL())), b7);
		clockCycles += 8;
		break;
	}
	case 0x7F: // bit 7, a
	{
		bit(A, b7);
		break;
	}
	case 0x80: // res 0, b
	{
		res(B, b0);
		break;
	}
	case 0x81: // res 0, c
	{
		res(C, b0);
		break;
	}
	case 0x82: // res 0, d
	{
		res(D, b0);
		break;
	}
	case 0x83: // res 0, e
	{
		res(E, b0);
		break;
	}
	case 0x84: // res 0, h
	{
		res(H, b0);
		break;
	}
	case 0x85: // res 0, l
	{
		res(L, b0);
		break;
	}
	case 0x86: // res 0, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b0;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x87: // res 0, a
	{
		res(A, b0);
		break;
	}
	case 0x88: // res 1, b
	{
		res(B, b1);
		break;
	}
	case 0x89: // res 1, c
	{
		res(C, b1);
		break;
	}
	case 0x8A: // res 1, d
	{
		res(D, b1);
		break;
	}
	case 0x8B: // res 1, e
	{
		res(E, b1);
		break;
	}
	case 0x8C: // res 1, h
	{
		res(H, b1);
		break;
	}
	case 0x8D: // res 1, l
	{
		res(L, b1);
		break;
	}
	case 0x8E: // res 1, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b1;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x8F: // res 1, a
	{
		res(A, b1);
		break;
	}
	case 0x90: // res 2, b
	{
		res(B, b2);
		break;
	}
	case 0x91: // res 2, c
	{
		res(C, b2);
		break;
	}
	case 0x92: // res 2, d
	{
		res(D, b2);
		break;
	}
	case 0x93: // res 2, e
	{
		res(E, b2);
		break;
	}
	case 0x94: // res 2, h
	{
		res(H, b2);
		break;
	}
	case 0x95: // res 2, l
	{
		res(L, b2);
		break;
	}
	case 0x96: // res 2, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b2;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x97: // res 2, a
	{
		res(A, b2);
		break;
	}
	case 0x98: // res 3, b
	{
		res(B, b3);
		break;
	}
	case 0x99: // res 3, c
	{
		res(C, b3);
		break;
	}
	case 0x9A: // res 3, d
	{
		res(D, b3);
		break;
	}
	case 0x9B: // res 3, e
	{
		res(E, b3);
		break;
	}
	case 0x9C: // res 3, h
	{
		res(H, b3);
		break;
	}
	case 0x9D: // res 3, l
	{
		res(L, b3);
		break;
	}
	case 0x9E: // res 3, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b3;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0x9F: // res 3, a
	{
		res(A, b3);
		break;
	}
	case 0xA0: // res 4, b
	{
		res(B, b4);
		break;
	}
	case 0xA1: // res 4, c
	{
		res(C, b4);
		break;
	}
	case 0xA2: // res 4, d
	{
		res(D, b4);
		break;
	}
	case 0xA3: // res 4, e
	{
		res(E, b4);
		break;
	}
	case 0xA4: // res 4, h
	{
		res(H, b4);
		break;
	}
	case 0xA5: // res 4, l
	{
		res(L, b4);
		break;
	}
	case 0xA6: // res 4, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b4;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xA7: //res 4, a
	{
		res(A, b4);
		break;
	}
	case 0xA8: //res 5, b
	{
		res(B, b5);
		break;
	}
	case 0xA9: // res 5, c
	{
		res(C, b5);
		break;
	}
	case 0xAA: // res 5, d
	{
		res(D, b5);
		break;
	}
	case 0xAB: // res 5, e
	{
		res(E, b5);
		break;
	}
	case 0xAC: // res 5, h
	{
		res(H, b5);
		break;
	}
	case 0xAD: // res 5, l
	{
		res(L, b5);
		break;
	}
	case 0xAE: // res 5, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b5;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xAF: // res 5, a
	{
		res(A, b5);
		break;
	}
	case 0xB0: // res 6, b
	{
		res(B, b6);
		break;
	}
	case 0xB1: // res 6, c
	{
		res(C, b6);
		break;
	}
	case 0xB2: // res 6, d
	{
		res(D, b6);
		break;
	}
	case 0xB3: // res 6, e
	{
		res(E, b6);
		break;
	}
	case 0xB4: // res 6, h
	{
		res(H, b6);
		break;
	}
	case 0xB5: // res 6, l
	{
		res(L, b6);
		break;
	}
	case 0xB6: // res 6, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b6;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xB7: // res 6, a
	{
		res(A, b6);
		break;
	}
	case 0xB8: // res 7, b
	{
		res(B, b7);
		break;
	}
	case 0xB9: // res 7, c
	{
		res(C, b7);
		break;
	}
	case 0xBA: // res 7, d
	{
		res(D, b7);
		break;
	}
	case 0xBB: // res 7, e
	{
		res(E, b7);
		break;
	}
	case 0xBC: // res 7, h
	{
		res(H, b7);
		break;
	}
	case 0xBD: // res 7, l
	{
		res(L, b7);
		break;
	}
	case 0xBE: // res 7, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val &= ~b7;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xBF: // res 7, a
	{
		res(A, b7);
		break;
	}
	case 0xC0: // set 0, b
	{
		set(B, b0);
		break;
	}
	case 0xC1: // set 0, c
	{
		set(C, b0);
		break;
	}
	case 0xC2: // set 0, d
	{
		set(D, b0);
		break;
	}
	case 0xC3: // set 0, e
	{
		set(E, b0);
		break;
	}
	case 0xC4: // set 0, h
	{
		set(H, b0);
		break;
	}
	case 0xC5: // set 0, l
	{
		set(L, b0);
		break;
	}
	case 0xC6: // set 0, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b0;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xC7: // set 0, a
	{
		set(A, b0);
		break;
	}
	case 0xC8: // set 1, b
	{
		set(B, b1);
		break;
	}
	case 0xC9: // set 1, c
	{
		set(C, b1);
		break;
	}
	case 0xCA: // set 1, d
	{
		set(D, b1);
		break;
	}
	case 0xCB: // set 1, e
	{
		set(E, b1);
		break;
	}
	case 0xCC: // set 1, h
	{
		set(H, b1);
		break;
	}
	case 0xCD: // set 1, l
	{
		set(L, b1);
		break;
	}
	case 0xCE: // set 1, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b1;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xCF: // set 1, a
	{
		set(A, b1);
		break;
	}
	case 0xD0: // set 2, b
	{
		set(B, b2);
		break;
	}
	case 0xD1: // set 2, c
	{
		set(C, b2);
		break;
	}
	case 0xD2: // set 2, d
	{
		set(D, b2);
		break;
	}
	case 0xD3: // set 2, e
	{
		set(E, b2);
		break;
	}
	case 0xD4: // set 2, h
	{
		set(H, b2);
		break;
	}
	case 0xD5: // set 2, l
	{
		set(L, b2);
		break;
	}
	case 0xD6: // set 2, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b2;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xD7: // set 2, a
	{
		set(A, b2);
		break;
	}
	case 0xD8: // set 3, b
	{
		set(B, b3);
		break;
	}
	case 0xD9: // set 3, c
	{
		set(C, b3);
		break;
	}
	case 0xDA: // set 3, d
	{
		set(D, b3);
		break;
	}
	case 0xDB: // set 3, e
	{
		set(E, b3);
		break;
	}
	case 0xDC: // set 3, h
	{
		set(H, b3);
		break;
	}
	case 0xDD: // set 3, l
	{
		set(L, b3);
		break;
	}
	case 0xDE: // set 3, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b3;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xDF: // set 3, a
	{
		set(A, b3);
		break;
	}
	case 0xE0: // set 4, b
	{
		set(B, b4);
		break;
	}
	case 0xE1: // set 4, c
	{
		set(C, b4);
		break;
	}
	case 0xE2: // set 4, d
	{
		set(D, b4);
		break;
	}
	case 0xE3: // set 4, e
	{
		set(E, b4);
		break;
	}
	case 0xE4: // set 4, h
	{
		set(H, b4);
		break;
	}
	case 0xE5: // set 4, l
	{
		set(L, b4);
		break;
	}
	case 0xE6: // set 4, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b4;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xE7: // set 4, a
	{
		set(A, b4);
		break;
	}
	case 0xE8: // set 5, b
	{
		set(B, b5);
		break;
	}
	case 0xE9: // set 5, c
	{
		set(C, b5);
		break;
	}
	case 0xEA: // set 5, d
	{
		set(D, b5);
		break;
	}
	case 0xEB: // set 5, e
	{
		set(E, b5);
		break;
	}
	case 0xEC: // set 5, h
	{
		set(H, b5);
		break;
	}
	case 0xED: // set 5, l
	{
		set(L, b5);
		break;
	}
	case 0xEE: // set 5, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b5;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xEF: // set 5, a
	{
		set(A, b5);
		break;
	}
	case 0xF0: // set 6, b
	{
		set(B, b6);
		break;
	}
	case 0xF1: // set 6, c
	{
		set(C, b6);
		break;
	}
	case 0xF2: // set 6, d
	{
		set(D, b6);
		break;
	}
	case 0xF3: // set 6, e
	{
		set(E, b6);
		break;
	}
	case 0xF4: // set 6, h
	{
		set(H, b6);
		break;
	}
	case 0xF5: // set 6, l
	{
		set(L, b6);
		break;
	}
	case 0xF6: // set 6, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b6;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xF7: // set 6, a
	{
		set(A, b6);
		break;
	}
	case 0xF8: // set 7, b
	{
		set(B, b7);
		break;
	}
	case 0xF9: // set 7, c
	{
		set(C, b7);
		break;
	}
	case 0xFA: // set 7, d
	{
		set(D, b7);
		break;
	}
	case 0xFB: // set 7, e
	{
		set(E, b7);
		break;
	}
	case 0xFC: // set 7, h
	{
		set(H, b7);
		break;
	}
	case 0xFD: // set 7, l
	{
		set(L, b7);
		break;
	}
	case 0xFE: // set 7, (hl)
	{
		byte val = rByte(static_cast<addr16>(HL()));
		val |= b7;
		wByte(static_cast<addr16>(HL()), val);
		clockCycles += 8;
		break;
	}
	case 0xFF: // set 7, a
	{
		set(A, b7);
		break;
	}
	default:
	{
		std::cout << "Unimplemented CB instruction: " << toHex(opcode) << std::endl;
		std::cout << "At: " << toHex(PC + 1) << std::endl;
		break;
	}
	}
	PC += 2; // all 0xCB instructions are 2 bytes long
}

void CPU::cmp(const byte val)
{
	updateCarry(A, A - val);
	updateN(SUB);
	updateHC(A, A - val);
	updateZero(A - val);
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
	A += val;
	updateCarry(before, A);
	updateN(ADD);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::adc(byte val)
{
	const byte before = A;
	A += val + carry();
	updateCarry(before, A);
	updateN(ADD);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::sub(byte val)
{
	const byte before = A;
	A -= val;
	updateCarry(before, A);
	updateN(SUB);
	updateHC(before, A);
	updateZero(A);
	PC++;
}

void CPU::sbc(byte val)
{
	const byte before = A;
	A -= (val + carry());
	updateCarry(before, A);
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
	//std::cout << "Halted" << std::endl; // for debugging 
}

void CPU::stop()
{
	stopped = true;
	//std::cout << "Stopped" << std::endl; // for debugging 
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

#pragma endregion

#ifdef DEBUG
void printMem(CPU* cpu, int start, int end)
{
	const std::vector<byte> mem = *cpu->dumpMem();
	for (int i = start; i <= end; i++)
	{
		std::cout << toHex((byte)mem[i]) << "\tat" << toHex(i) << "\n";
	}
	std::cout << "\n\n";
}
#endif // DEBUG

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
}

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
		if ((intEnable & 0x1) && (intFlag & 0x1)) // vblank
		{
			const int vblank = 0x40;
			interrupt(vblank);
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


void CPU::wByte(addr16 addr, byte val)
{
	if (isInternalMem(addr))
	{
		internalmem[addr] = val;
		if (addr >= 0xC000 && addr <= 0xDE00 ||
			addr >= 0xE000 && addr <= 0xFE00) // echo RAM
		{
			internalmem[addr + 0x2000] = val; // emulate mirroring of RAM
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
	emulateExtendedInstruction(00);
	//A = 0x30;
	//F = 0x10;
	//std::cout << toHex(AF()) << std::endl;
	//int x = rByte(0x0000);
	//x *= 3;
	//
	//loadROM("test.bin");
	//PC = 0x0;
	//for (int i = 0; i < 0x10; i++)
	//{
	//	emulateCycle();
	//}
	//std::cout << std::endl;
	//std::cout << "A: " << toHex((byte)A) << std::endl;
	//std::cout << "B: " << toHex((byte)B) << std::endl;
	//std::cout << "C: " << toHex((byte)C) << std::endl;
	//std::cout << "D: " << toHex((byte)D) << std::endl;
	//std::cout << "E: " << toHex((byte)E) << std::endl;
	//std::cout << "F: " << toHex((byte)F) << std::endl;
	//std::cout << "AF: " << toHex(AF()) << std::endl;
	//std::cout << "BC: " << toHex(BC()) << std::endl;
	//std::cout << "DE: " << toHex(DE()) << std::endl;
	//std::cout << "HL: " << toHex(HL()) << std::endl;
}

static bool y = false;
static int c = 0;

addr16 shadowPC;

#endif // DEBUG

static bool ii = false;
static int k = 0;

void CPU::emulateCycle()
{
	handleInterrupts();
	unsigned char opcode = rByte(PC); // get next opcode
	clockCycles += clockTimes[opcode];

	/// emulate the opcode 
	switch (opcode & 0xFF)
	{
		case 0x00: // NOP
		{
			PC++;
			break;
		}
		case 0x01: // ld BC, **
		{
			BC(getNextWord());
			PC += 3;
			break;
		}
		case 0x02: // ld (BC), a
		{
			wByte(static_cast<addr16>(BC()), A);
			//mem[static_cast<addr16>(BC())] = A;
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
			A <<= 1;
			updateCarry(before, A);
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
			updateCarry(hl, HL());
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
			A >>= 1;
			updateCarry(before, A);
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
			//mem[] = A;
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
			A <<= 1;
			updateCarry(before, A);
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
			updateCarry(hl, HL());
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
			A >>= 1;
			updateCarry(before, A);
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
			//rByte(static_cast<addr16>(HL())) = A;
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
			updateCarry(hl, HL());
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
			updateCarry(hl, HL());
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
			B = rByte(static_cast<addr16>(HL()));
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
			C = rByte(static_cast<addr16>(HL()));
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
			D = rByte(static_cast<addr16>(HL()));
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
			E = rByte(static_cast<addr16>(HL()));
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
			H = rByte(static_cast<addr16>(HL()));
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
			L = rByte(static_cast<addr16>(HL()));
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
			wByte(static_cast<addr16>(HL()), B);
			PC++;
			break;
		}
		case 0x71: // ld (hl), c
		{
			wByte(static_cast<addr16>(HL()), C);
			PC++;
			break;
		}
		case 0x72: // ld (hl), d
		{
			wByte(static_cast<addr16>(HL()), D);
			PC++;
			break;
		}
		case 0x73: // ld (hl), e
		{
			wByte(static_cast<addr16>(HL()), E);
			PC++;
			break;
		}
		case 0x74: // ld (hl), h
		{
			wByte(static_cast<addr16>(HL()), H);
			PC++;
			break;
		}
		case 0x75: // ld (hl), l
		{
			wByte(static_cast<addr16>(HL()), L);
			PC++;
			break;
		}
		case 0x76: // halt
		{
			halt();
			PC++;
			break;
		}
		case 0x77: // ld (hl), a
		{
			wByte(static_cast<addr16>(HL()), A);
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
			A = rByte(static_cast<addr16>(HL()));
			PC++;
			break;
		}
		case 0x7F: // ld a, a
		{
			PC++;
			break;
		}
		case 0x80: // add a, b
		{
			add(B);
			break;
		}
		case 0x81: // add a, c
		{
			add(C);
			break;
		}
		case 0x82: // add a,d 
		{
			add(D);
			break;
		}
		case 0x83: // add a, e
		{
			add(E);
			break;
		}
		case 0x84: // add a, h
		{
			add(H);
			break;
		}
		case 0x85: // add a, l
		{
			add(L);
			break;
		}
		case 0x86: // add a, (hl)
		{
			add(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0x87: // add a, a
		{
			add(A);
			break;
		}
		case 0x88: // adc a, b
		{
			adc(B);
			break;
		}
		case 0x89: // adc a, c
		{
			adc(C);
			break;
		}
		case 0x8A: // adc a, d
		{
			adc(D);
			break;
		}
		case 0x8B: // adc a, e
		{
			adc(E);
			break;
		}
		case 0x8C: // adc a, h
		{
			adc(H);
			break;
		}
		case 0x8D: // adc a, l
		{
			adc(L);
			break;
		}
		case 0x8E: // adc a, (hl)
		{
			adc(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0x8F: // adc a, a
		{
			adc(A);
			break;
		}
		case 0x90: // sub b
		{
			sub(B);
			break;
		}
		case 0x91: // sub c
		{
			sub(C);
			break;
		}
		case 0x92: // sub d
		{
			sub(D);
			break;
		}
		case 0x93: // sub e
		{
			sub(E);
			break;
		}
		case 0x94: // sub h
		{
			sub(H);
			break;
		}
		case 0x95: // sub l
		{
			sub(L);
			break;
		}
		case 0x96: // sub (hl)
		{
			sub(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0x97: // sub a 
		{
			sub(A);
			break;
		}
		case 0x98: // sbc a, b
		{
			sbc(B);
			break;
		}
		case 0x99: // sbc a, c
		{
			sbc(C);
			break;
		}
		case 0x9A: // sbc a, d
		{
			sbc(D);
			break;
		}
		case 0x9B: // sbc a, e
		{
			sbc(E);
			break;
		}
		case 0x9C: // sbc a, h
		{
			sbc(H);
			break;
		}
		case 0x9D: // sbc a, l
		{
			sbc(L);
			break;
		}
		case 0x9E: // sbc a, (hl)
		{
			sbc(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0x9F: // sbc a, a
		{
			sbc(A);
			break;
		}
		case 0xA0: // and b
		{
			andr(B);
			break;
		}
		case 0xA1: // and c
		{
			andr(C);
			break;
		}
		case 0xA2: // and d
		{
			andr(D);
			break;
		}
		case 0xA3: // and e
		{
			andr(E);
			break;
		}
		case 0xA4: // and h
		{
			andr(H);
			break;
		}
		case 0xA5: // and l
		{
			andr(L);
			break;
		}
		case 0xA6: // and (hl)
		{
			andr(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0xA7: // and a
		{
			andr(A);
			break;
		}
		case 0xA8: // xor b
		{
			xorr(B);
			break;
		}
		case 0xA9: // xor c
		{
			xorr(C);
			break;
		}
		case 0xAA: // xor d
		{
			xorr(D);
			break;
		}
		case 0xAB: // xor e
		{
			xorr(E);
			break;
		}
		case 0xAC: // xor h
		{
			xorr(H);
			break;
		}
		case 0xAD: // xor l
		{
			xorr(L);
			break;
		}
		case 0xAE: // xor (hl)
		{
			xorr(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0xAF: // xor a
		{
			xorr(A);
			break;
		}
		case 0xB0: // or b
		{
			orr(B);
			break;
		}
		case 0xB1: // or c
		{
			orr(C);
			break;
		}
		case 0xB2: // or d
		{
			orr(D);
			break;
		}
		case 0xB3: // or e
		{
			orr(E);
			break;
		}
		case 0xB4: // or h
		{
			orr(H);
			break;
		}
		case 0xB5: // or l
		{
			orr(L);
			break;
		}
		case 0xB6: // or (hl)
		{
			orr(rByte(static_cast<addr16>(HL())));
			break;
		}
		case 0xB7: // or a
		{
			orr(A);
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
			cmp(rByte(static_cast<addr16>(HL())));
			PC++;
			break;
		}
		case 0xBF: // cp a 
		{
			cmp(A);
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
			B = rByte(SP);
			SP++;
			C = rByte(SP);
			SP++;
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
			SP--;
			wByte(SP, C);
			//internalmem[SP] = C;
			SP--;
			//internalmem[SP] = B;
			wByte(SP, B);
			PC++;
			break;
		}
		case 0xC6: // add a, *
		{
			const reg before = A;
			A += rByte(PC + 1);
			updateCarry(before, A);
			updateN(ADD);
			updateHC(before, A);
			updateZero(A);
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
			jp(zero(), getNextWord(), 3);
			break;
		}
		case 0xCB: // EXTENDED INSTRUCTIONS
		{
			emulateExtendedInstruction(rByte(PC + 1));
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
			const reg before = A;
			A += rByte(PC + 1) + carry();
			updateCarry(before, A);
			updateN(ADD);
			updateHC(before, A);
			updateZero(A);
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
			D = rByte(SP);
			SP++;
			E = rByte(SP);
			SP++;
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
			SP--;
			wByte(SP, E);
			//internalmem[SP] = E;
			SP--;
			//internalmem[SP] = D;
			wByte(SP, D);
			PC++;
			break;
		}
		case 0xD6: // sub *
		{
			const reg before = A;
			A -= rByte(PC + 1);
			updateCarry(before, A);
			updateN(SUB);
			updateHC(before, A);
			updateZero(A);
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
			const reg before = A;
			A -= (rByte(PC + 1) + carry());
			updateCarry(before, A);
			updateN(SUB);
			updateHC(before, A);
			updateZero(A);
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
			H = rByte(SP);
			SP++;
			L = rByte(SP);
			SP++;
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
			SP--;
			//internalmem[SP] = L;
			wByte(SP, L);
			SP--;
			//internalmem[SP] = H;
			wByte(SP, H);
			PC++;
			break;
		}
		case 0xE6: // and *
		{
			A &= rByte(PC + 1);
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
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
			const addr16 before = SP;
			SP += rByte(PC + 1);
			resetZero();
			resetN();
			updateHC(before, SP);
			updateCarry(before, SP);
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
			A ^= rByte(PC + 1);
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC += 2;
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
			A = rByte(SP);
			SP++;
			F = rByte(SP);
			SP++;
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
			SP--;
			wByte(SP, F);
			SP--;
			wByte(SP, A);
			PC++;
			break;
		}
		case 0xF6: // or *
		{
			A |= rByte(PC + 1);
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC += 2;
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
			PC += 2;
			break;
		}
		case 0xFF: // rst 0x38
		{
			rst(0x38);
			break;
		}
		default: // if the definition of a char changes 
		{
#ifdef DEBUG
			std::cout << "You should never ever see this" << std::endl;
			std::cout << "But here is the opcode : " << toHex(opcode) << std::endl;
			std::cin.ignore(); // make the user see what they have done
#endif // DEBUG
			PC++;
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

	//if (!isROMTooBig)
	//{
	//	// load the rom into memory starting at 0x00
	//	for (unsigned i = 0; i < size; i++)
	//	{
	//		mem[i] = ROMstr[i];
	//	}
	//}
	//else
	//{
	//	system("pause");
	//}

	delete[] ROMstr;
	return EXIT_SUCCESS; // ROM load completed succesfully
}