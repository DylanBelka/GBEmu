#include "cpu.h"

const unsigned int clockTimes[256] =
{
	4, 10, 7, 6, 4, 4, 7, 4, 4, 11, 7, 6, 4, 4, 7, 4,
	4, 10, 7, 6, 4, 4, 7, 4, 12, 11, 7, 6, 4, 4, 7, 4,
	7, 10, 16, 6, 4, 4, 7, 4, 7, 11, 16, 6, 4, 4, 7, 4,
	7, 10, 13, 6, 11, 11, 10, 4, 7, 11, 13, 6, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	5, 10, 10, 10, 10, 11, 7, 11, 5, 10, 10, 8, 10, 17, 7, 11,
	5, 10, 10, 11, 10, 11, 7, 11, 5, 4, 10, 11, 10, 4, 7, 11,
	5, 10, 10, 19, 10, 11, 7, 11, 5, 4, 10, 4, 10, 4, 7, 11,
	5, 10, 10, 4, 10, 11, 7, 11, 5, 6, 10, 4, 10, 4, 7, 11,
};

// todo: change opcodes that are gameboy specific
const std::string opcodeStr[] = 
{
	"inc bc", "inc b", "dec b", "add hl,bc", "dec bc", "inc c", "dec c", "inc de", "inc d",
	"dec d", "add hl,de", "dec de", "inc e", "dec e", "inc hl", "inc h", "dec h", "add hl,hl",
	"dec hl", "inc l", "dec l", "inc sp", "inc (hl)", "dec (hl)", "add hl,sp", "dec sp", "inc a",
	"dec a", "add a,b", "add a,c", "add a,d", "add a,e", "add a,h", "add a,l", "add a,(hl)", "add a,a",
	"adc a,b", "adc a,c", "adc a,d", "adc a,e", "adc a,h", "adc a,l", "adc a,(hl)", "adc a,a", "sub b",
	"sub c", "sub d", "sub e", "sub h", "sub l", "sub (hl)", "sub a", "sbc a,b", "sbc a,c",
	"sbc a,d", "sbc a,e", "sbc a,h", "sbc a,l", "sbc a,(hl)", "sbc a,a", "and b", "and c", "and d",
	"and e", "and h", "and l", "and (hl)", "and a", "xor b", "xor c", "xor d", "xor e",
	"xor h", "xor l", "xor (hl)", "xor a", "ret nz", "pop bc", "add a,*", "rst 00h", "ret z",
	"adc a,*", "rst 08h", "ret nc", "pop de", "out (*),a", "sub *", "rst 10h", "ret c", "sbc a,*",
	"rst 18h", "ret po", "pop hl", "and *", "rst 20h", "ret pe", "xor *", "rst 28h", "ret p",
	"pop af", "rst 30h", "ret m", "rst 38h", "out (c),b", "sbc hl,bc", "out (c),c", "adc hl,bc", "out (c),d",
	"sbc hl,de", "out (c),e", "adc hl,de", "out (c),h", "sbc hl,hl", "out (c),l", "adc hl,hl", "out (c),0", "sbc hl,sp",
	"out (c),a", "adc hl,sp", 
	// these might as well not be here
	"rlc b", "rlc c", "rlc d", "rlc e", "rlc h", "rlc l", "rlc (hl)",
	"rlc a", "rrc b", "rrc c", "rrc d", "rrc e", "rrc h", "rrc l", "rrc (hl)", "rrc a",
	"sla b", "sla c", "sla d", "sla e", "sla h", "sla l", "sla (hl)", "sla a", "sra b",
	"sra c", "sra d", "sra e", "sra h", "sra l", "sra (hl)", "sra a", "sll b", "sll c",
	"sll d", "sll e", "sll h", "sll l", "sll (hl)", "sll a", "srl b", "srl c", "srl d",
	"srl e", "srl h", "srl l", "srl (hl)", "srl a", "bit 0,b", "bit 0,c", "bit 0,d", "bit 0,e",
	"bit 0,h", "bit 0,l", "bit 0,(hl)", "bit 0,a", "bit 1,b", "bit 1,c", "bit 1,d", "bit 1,e", "bit 1,h",
	"bit 1,l", "bit 1,(hl)", "bit 1,a", "bit 2,b", "bit 2,c", "bit 2,d", "bit 2,e", "bit 2,h", "bit 2,l",
	"bit 2,(hl)", "bit 2,a", "bit 3,b", "bit 3,c", "bit 3,d", "bit 3,e", "bit 3,h", "bit 3,l", "bit 3,(hl)",
	"bit 3,a", "bit 4,b", "bit 4,c", "bit 4,d", "bit 4,e", "bit 4,h", "bit 4,l", "bit 4,(hl)", "bit 4,a",
	"bit 5,b", "bit 5,c", "bit 5,d", "bit 5,e", "bit 5,h", "bit 5,l", "bit 5,(hl)", "bit 5,a", "bit 6,b",
	"bit 6,c", "bit 6,d", "bit 6,e", "bit 6,h", "bit 6,l", "bit 6,(hl)", "bit 6,a", "bit 7,b", "bit 7,c",
	"bit 7,d", "bit 7,e", "bit 7,h", "bit 7,l", "bit 7,(hl)", "bit 7,a", "res 0,b", "res 0,c", "res 0,d",
	"res 0,e", "res 0,h", "res 0,l", "res 0,(hl)", "res 0,a", "res 1,b", "res 1,c", "res 1,d", "res 1,e",
	"res 1,h", "res 1,l", "res 1,(hl)", "res 1,a", "res 2,b", "res 2,c", "res 2,d", "res 2,e", "res 2,h",
	"res 2,l", "res 2,(hl)", "res 2,a", "res 3,b", "res 3,c", "res 3,d", "res 3,e", "res 3,h", "res 3,l",
	"res 3,(hl)", "res 3,a", "res 4,b", "res 4,c", "res 4,d", "res 4,e", "res 4,h", "res 4,l", "res 4,(hl)",
	"res 4,a", "res 5,b", "res 5,c", "res 5,d", "res 5,e", "res 5,h", "res 5,l", "res 5,(hl)", "res 5,a",
	"res 6,b", "res 6,c", "res 6,d", "res 6,e", "res 6,h", "res 6,l", "res 6,(hl)", "res 6,a", "res 7,b",
	"res 7,c", "res 7,d", "res 7,e", "res 7,h", "res 7,l", "res 7,(hl)", "res 7,a", "set 0,b", "set 0,c",
	"set 0,d", "set 0,e", "set 0,h", "set 0,l", "set 0,(hl)", "set 0,a", "set 1,b", "set 1,c", "set 1,d",
	"set 1,e", "set 1,h", "set 1,l", "set 1,(hl)", "set 1,a", "set 2,b", "set 2,c", "set 2,d", "set 2,e",
	"set 2,h", "set 2,l", "set 2,(hl)", "set 2,a", "set 3,b", "set 3,c", "set 3,d", "set 3,e", "set 3,h",
	"set 3,l", "set 3,(hl)", "set 3,a", "set 4,b", "set 4,c", "set 4,d", "set 4,e", "set 4,h", "set 4,l",
	"set 4,(hl)", "set 4,a", "set 5,b", "set 5,c", "set 5,d", "set 5,e", "set 5,h", "set 5,l", "set 5,(hl)",
	"set 5,a", "set 6,b", "set 6,c", "set 6,d", "set 6,e", "set 6,h", "set 6,l", "set 6,(hl)", "set 6,a",
	"set 7,b", "set 7,c", "set 7,d", "set 7,e", "set 7,h", "set 7,l", "set 7,(hl)", "set 7,a",
};

CPU::CPU()
{
	keyInfo = { { 0x0F, 0x0F }, 0x0 };
	mem = new byte[MEM_SIZE];
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
	R = 0x0;
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

void CPU::updateCarry(reg16 reg)
{
	F |= (reg > 127 || reg < -128) ? 0x1 : F;
}

void CPU::resetCarry()
{
	F &= 0xFE;
}

void CPU::setCarry()
{
	F |= 0x1;
}

void CPU::updateHC(reg16 reg)
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

void CPU::decodeExtendedInstruction(char opcode)
{
	switch (opcode)
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
		char val = mem[(addr16)HL()];
		val <<= 1;
		resetCarry();
		F |= val & b7;
		val |= val & b7;
		resetN();
		updateZero(val);
		mem[(addr16)HL()] = val;
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
		char val = mem[(addr16)HL()];
		val >>= 1;
		resetCarry();
		F |= val & b0;
		val |= val & b0;
		resetN();
		updateZero(val);
		mem[(addr16)HL()] = val;
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
		char val = mem[(addr16)HL()];
		val <<= 1;
		val |= carry();
		F |= val & b7;
		updateCarry(val);
		resetN();
		resetHC();
		updateZero(val);
		mem[(addr16)HL()] = val;
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
		char val = mem[(addr16)HL()];
		val >>= 1;
		val &= F & 0x80;
		F |= val & 0x1;
		updateCarry(val);
		resetN();
		resetHC();
		updateZero(val);
		mem[(addr16)HL()] = val;
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
		byte val = mem[(reg16)HL()];
		val <<= 1;
		F |= val & 0x80;
		val &= 0xFE;
		updateCarry(val);
		resetN();
		updateZero(val);
		mem[(reg16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val >>= 1;
		F |= val & 0x1;
		updateCarry(val);
		resetN();
		updateZero(val);
		mem[(addr16)HL()] = val;
		break;
	}
	case 0x2F: // sra a
	{
		sra(A);
		break;
	}
	/**
	No instructions 0x30 - 0x36
	These are here to allow the compiler to easily tell if there are misspelled cases and counting the number of breaks
	**/
	case 0x30:
		break;
	case 0x31:
		break;
	case 0x32:
		break;
	case 0x33:
		break;
	case 0x34:
		break;
	case 0x35:
		break;
	case 0x36:
		break;
	case 0x37: // swap a
	{
		swapNibble(A);
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
		byte val = mem[(addr16)HL()];
		val >>= 1;
		F |= val & 0x1;
		val &= 0xF7;
		updateCarry(val);
		resetN();
		resetHC();
		updateZero(val);
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
		bit(mem[(addr16)HL()], b0);
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
		bit(mem[(addr16)HL()], b1);
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
		bit(mem[(addr16)HL()], b2);
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
		bit(mem[(addr16)HL()], b3);
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
		bit(mem[(addr16)HL()], b3);
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
		bit(mem[(addr16)HL()], b5);
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
		bit(mem[(addr16)HL()], b6);
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
		bit(mem[(addr16)HL()], b7);
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
		byte val = mem[(addr16)HL()];
		val &= ~b0;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b1;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b2;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b3;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b4;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b5;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b6;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val &= ~b7;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b0;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b1;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b2;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b3;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b4;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b5;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b6;
		mem[(addr16)HL()] = val;
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
		byte val = mem[(addr16)HL()];
		val |= b7;
		mem[(addr16)HL()] = val;
		break;
	}
	case 0xFF: // set 7, a
	{
		set(A, b7);
		break;
	}
	default:
	{
		//std::cout << "Unimplemented CB instruction: " << toHex(opcode) << std::endl;
	//	std::cout << "At: " << toHex(PC + 1) << std::endl;
		break;
	}
	}
	PC += 2; // all 0xCB instructions are 2 bytes long
}

void CPU::swapNibble(reg& val)
{
	val = ((val & 0x0F) << 4 | (val & 0xF0) >> 4);
}

void CPU::cmp(const byte val)
{
	updateCarry(A - val);
	updateN(SUB);
	updateHC(A - val);
	updateZero(A - val);
}

const addr16 CPU::get16()
{
	return ((mem[PC + 2] << 8) | (mem[PC + 1] & 0xFF));
}

const addr16 CPU::get16(const addr16 where)
{
	return ((mem[where + 2] << 8) | (mem[where + 1] & 0xFF));
}

void CPU::rlc(reg& reg)
{
	reg <<= 1;
	resetCarry();
	F |= reg & b7;
	reg |= reg & b7;
	updateCarry(reg);
	resetN();
	updateZero(reg);
	resetHC();
}

void CPU::rl(reg& reg)
{
	reg <<= 1;
	reg |= F & b0;
	F |= reg & b7;
	updateCarry(reg);
	resetN();
	resetHC();
	updateZero(reg);
}

void CPU::rrc(reg& reg)
{
	reg >>= 1;
	resetCarry();
	F |= reg & b0;
	reg |= reg & b0;
	resetN();
	updateZero(reg);
}

void CPU::rr(reg& reg)
{
	reg >>= 1;
	reg &= F & b7;
	F |= reg & b0;
	updateCarry(reg);
	resetN();
	resetHC();
	updateZero(reg);
}

void CPU::sla(reg& reg)
{
	reg <<= 1;
	F |= reg & b7;
	reg &= ~b0;
	updateCarry(reg);
	resetN();
	updateZero(reg);
}

void CPU::sra(reg& reg)
{
	reg >>= 1;
	F |= reg & b0;
	updateCarry(reg);
	resetN();
	resetHC();
	updateZero(reg);
}

void CPU::srl(reg& reg)
{
	reg >>= 1;
	F |= reg & b0;
	reg &= ~b3;
	updateCarry(reg);
	resetN();
	resetHC();
	updateZero(reg);
}

void CPU::bit(reg reg, ubyte bit)
{
	updateZero(!(reg & bit));
	setHC();
	resetN();
}

void CPU::res(reg& reg, ubyte bit)
{
	reg &= ~bit;
}

void CPU::set(reg& reg, ubyte bit)
{
	reg |= bit;
}

// set halted flag
void CPU::halt()
{
	halted = true;
	std::cout << "Halted" << std::endl; // for debugging 
}

void CPU::stop()
{
	stopped = true;
	std::cout << "Stopped" << std::endl; // for debugging 
}

bool interrupted = false;

void CPU::ret(bool cond)
{
	if (cond)
	{
		PC = pop(); // pop PC off the stack
		clockCycles += 6;
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
		addr16 pccpy = PC;
		SP--;
		mem[SP] = (PC + 3) & 0xFF; // + 3 is for jumping past the 3 bytes for the opcode and dest
		SP--;
		mem[SP] = (((PC + 3) >> 8) & 0xFF);
		PC = get16();
		if (PC == 0x24ac)
		{
			std::cout << "callllll" << std::endl;
			std::cout << toHex(pccpy) << std::endl;
			system("pause");
		}
		clockCycles += 7;
	}
	else
	{
		PC += 3;
	}
}

void CPU::push(reg16 val)
{
	SP--;
	mem[SP] = val & 0xFF;
	SP--;
	mem[SP] = val >> 0x8;
}

reg16 CPU::pop()
{
	reg16 ret = 0;
	ret = mem[SP] << 8;
	SP++;
	ret |= mem[SP] & 0xFF;
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
		clockCycles += 5; // 5 more cycles added (total of 12) for jump
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
	}
	else
	{
		PC += opsize;
	}
}

#pragma endregion

void printMem(CPU* cpu, int start, int end)
{
	const byte* mem = cpu->dumpMem();
	for (int i = start; i <= end; i++)
	{
		std::cout << toHex((byte)mem[i]) << "\tat" << toHex(i) << "\n";
	}
	std::cout << "\n\n";
}

void CPU::dma()
{
	const addr16 dmaStart = A << 0x8; // get the location that the DMA will be copying from
	for (int i = 0; i < 0x8C; i++) // copy the 0x8C bytes from dmaStart to the OAM
	{
		mem[OAM + i] = mem[dmaStart + i];
	}
}

void CPU::interrupt(const char to)
{
	push(PC); // push the program counter onto the stack
	PC = to; // jump to the interrupt location
	interrupted = true;
	IME = false; // disable interrupts
	mem[IF] = 0x0;
}

void CPU::handleInterrupts()
{
	const byte intEnable = mem[IE];
	const byte intFlag = mem[IF];
	if (IME) // are interrupts enabled?
	{
		if ((intEnable & 0x1) && (intFlag & 0x1)) // vblank
		{
			const int vblank = 0x40;
			interrupt(vblank);
		}
	}
}

void CPU::test()
{
	loadROM("test.bin");
	PC = 0x0;
	for (int i = 0; i < 0x10; i++)
	{
		emulateCycle();
	}
	std::cout << std::endl;
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

bool x = false;
bool y = false;
int c = 0;

addr16 shadowPC;

unsigned ticks = SDL_GetTicks();

#define printOpcode() \
	if (opcode == 0xCB) { std::cout << opcodeStr[0xCB + mem[PC + 1]] << "\tat " << toHex(PC) << "\t"; } \
		else { std::cout << opcodeStr[opcode] << "\tat " << toHex(PC) << "\t"; } \
	std::cout << toHex((uint16_t)opcode) << "\n";

void CPU::emulateCycle()
{
	handleInterrupts();
	unsigned char opcode = mem[PC]; // get next opcode
	R = (R + 1) & 0x7F; // bit 7 never set
	clockCycles += clockTimes[opcode];
	// std::cout << toHex(opcode) << "\tat " << toHex(PC) << "\n";

	/// TODO: find bug in opus5
	/// might be same bug as in tetris
	/// either way its a bug fix

	if (_test)
	{
		//std::cout << toHex((byte)opcode) << "\tat " << toHex(PC) << "\n";
		std::cout << toHex((byte)opcode) << "\tat " << toHex(PC) << "\n";
	}
	// problems occur in tetris when byte 0 is written to
	// obviously the game is never meant to actually do that
	// there must be some kind of loop condition that is not being met
	// therefore the memory loading never ends, raps around and everything to 0x2F
	// 0x2F = cpl

	// problem at 0x24af
	// with 0x77 = ld (hl), a
	// a should not equal 0x2f
	// for some reason it does
	if (mem[0x00] == 0x2f)
	{
		std::cout << SDL_GetTicks() - ticks << std::endl;
		printOpcode();
		
		system("pause");
	}
	if (SDL_GetTicks() - ticks > 15260)
	{
		//printOpcode();
	}
	//if (PC == 0x24ac)
	//{
	//	std::cout << "A: " << toHex((byte)A) << std::endl;
	//	std::cout << "B: " << toHex((byte)B) << std::endl;
	//	std::cout << "C: " << toHex((byte)C) << std::endl;
	//	std::cout << "D: " << toHex((byte)D) << std::endl;
	//	std::cout << "E: " << toHex((byte)E) << std::endl;
	//	std::cout << "F: " << toHex((byte)F) << std::endl;
	//	std::cout << "AF: " << toHex(AF()) << std::endl;
	//	std::cout << "BC: " << toHex(BC()) << std::endl;
	//	std::cout << "DE: " << toHex(DE()) << std::endl;
	//	std::cout << "HL: " << toHex(HL()) << std::endl;
	//	std::cout << "mem[de]: " << toHex((uint16_t)mem[(addr16)DE()]) << std::endl;
	//	printMem(this, (addr16)DE(), (addr16)DE() + 100);
	//	system("pause");
	//}
	// 0x27a8
	// 0x1ff7 - 0x2004 important, here a lot
	// call to 0x24ac, there never should be
	// call comes from 0x22a9, never should happen
	// never a jump to 0x1a07
	// std::cout << toHex((byte)opcode) << "\tat " << toHex(PC) << "\n";

	if (PC == 0x1a07)
	{
		std::cout << "sdfa" << std::endl;
		x = true;
		system("pause");
	}
	if (x)
	{
		printOpcode();
	}

	if (mem[0xFFE3] != 0)
	{
		//std::cout << "ffe3" << std::endl;
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
		//std::cout << "mem[de]: " << toHex((uint16_t)mem[(addr16)DE()]) << std::endl;
		//printMem(this, (addr16)DE(), (addr16)DE() + 100);
		//system("pause");
	}
	/// emulate the opcode 
	switch (opcode)
	{
		case 0x00: // NOP
		{
			PC++;
			break;
		}
		case 0x01: // ld BC, **
		{
			BC(get16());
			PC += 3;
			break;
		}
		case 0x02: // ld (BC), a
		{
			mem[(addr16)BC()] = A;
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
			B++;
			updateN(ADD);
			updateHC(B);
			updateZero(B);
			PC++;
			break;
		}
		case 0x05: // dec b
		{
			B--;
			updateN(SUB);
			updateHC(B);
			updateZero(B);
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
		case 0x08: // ld (**), sp
		{
			mem[get16()] = SP;
			PC += 3;
			break;
		}
		case 0x09: // add hl, bc
		{
			const reg16 hl = HL();
			HL(BC() + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x0A: // ld a, (BC)
		{
			A = mem[(addr16)BC()];
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
			C++;
			updateN(ADD);
			updateHC(C);
			updateZero(C);
			PC++;
			break;
		}
		case 0x0D: // dec C
		{
			C--;
			updateN(SUB);
			updateHC(C);
			updateZero(C);
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
			stop();
			PC++;
			break;
		}
		case 0x11: // ld de, **
		{
			DE(get16());
			PC += 3;
			break;
		}
		case 0x12: // ld (de), a
		{
			mem[(addr16)DE()] = A;
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
			D++;
			updateN(ADD);
			updateHC(D);
			updateZero(D);
			PC++;
			break;
		}
		case 0x15: // dec d
		{
			D--;
			updateN(SUB);
			updateHC(D);
			updateZero(D);
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
			clockCycles -= 5;
			break;
		}
		case 0x19: // add hl, de
		{
			const reg16 hl = HL();
			HL(hl + DE());
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x1A: // ld a, (de)
		{
			A = mem[(addr16)DE()];
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
			E++;
			updateN(ADD);
			updateZero(E);
			updateHC(E);
			PC++;
			break;
		}
		case 0x1D: // dec e
		{
			E--;
			updateN(SUB);
			updateZero(E);
			updateHC(E);
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
			mem[(addr16)HL()] = A;
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
			H++;
			updateN(ADD);
			updateHC(H);
			updateZero(H);
			PC++;
			break;
		}
		case 0x25: // dec h
		{
			H--;
			updateN(SUB);
			updateHC(H);
			updateZero(H);
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
			std::cout << "daa" << std::endl;
			std::cout << "Before A = " << toHex((uint16_t)A) << std::endl;
			if (A > 9 || half_carry())
			{
				A += 0x6;
			}
			if (A & 0xF0 > 9 || carry())
			{
				A += 0x60;
			}
			updateCarry(A);
			updateHC(A);
			updateZero(A);
			PC++;
			std::cout << "After A = " << toHex((uint16_t)A) << std::endl;
			system("pause");
			break;
		}
		case 0x28: // jr z, *
		{
			jr(zero(), mem[PC + 1], 2);
			break;
		}
		case 0x29: // add hl, hl
		{
			const reg16 hl = HL();
			HL(hl + hl);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			PC++;
			break;
		}
		case 0x2A: // ldi a, (hl) 
		{
			A = mem[(addr16)HL()];
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
			L++;
			updateHC(L);
			updateN(ADD);
			updateZero(L);
			PC++;
			break;
		}
		case 0x2D: // dec l
		{
			L--;
			updateHC(L);
			updateN(SUB);
			updateZero(L);
			PC++;
			break;
		}
		case 0x2E: // ld l, *
		{
			L = mem[PC + 1];
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
			mem[(addr16)HL()] = A;
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
			mem[(addr16)HL()]++;
			const uint8_t val = mem[(addr16)HL()];
			updateN(ADD);
			updateZero(val);
			updateHC(val);
			PC++;
			break;
		}
		case 0x35: // dec (hl)
		{
			mem[(addr16)HL()]--;
			const uint8_t val = mem[(addr16)HL()];
			updateN(SUB);
			updateZero(val);
			updateHC(val);
			PC++;
			break;
		}
		case 0x36: // ld (hl), *
		{
			mem[(addr16)HL()] = mem[PC + 1];
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
			const reg16 hl = HL();
			HL(hl + SP);
			updateCarry(HL());
			updateN(ADD);
			updateHC(HL());
			updateZero(HL());
			PC++;
			break;
		}
		case 0x3A: // ldd a, (hl)
		{
			A = mem[(addr16)HL()];
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
			A++;
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x3D: // dec a
		{
			A--;
			updateN(SUB);
			updateHC(A);
			updateZero(A);
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
			setCarry();
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
			B = mem[(addr16)HL()];
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
			C = mem[(addr16)HL()];
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
			D = mem[(addr16)HL()];
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
			E = mem[(addr16)HL()];
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
			H = mem[(addr16)HL()];
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
			L = mem[(addr16)HL()];
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
			mem[(addr16)HL()] = B;
			PC++;
			break;
		}
		case 0x71: // ld (hl), c
		{
			mem[(addr16)HL()] = C;
			PC++;
			break;
		}
		case 0x72: // ld (hl), d
		{
			mem[(addr16)HL()] = D;
			PC++;
			break;
		}
		case 0x73: // ld (hl), e
		{
			mem[(addr16)HL()] = E;
			PC++;
			break;
		}
		case 0x74: // ld (hl), h
		{
			mem[(addr16)HL()] = H;
			PC++;
			break;
		}
		case 0x75: // ld (hl), l
		{
			mem[(addr16)HL()] = L;
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
			mem[(addr16)HL()] = A;
			if (A == 0x2F)
			{
				system("pause");
			}
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
			A = mem[(addr16)HL()];
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
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x81: // add a, c
		{
			A += C;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x82: // add a,d 
		{
			A += D;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x83: // add a, e
		{
			A += E;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x84: // add a, h
		{
			A += H;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x85: // add a, l
		{
			A += L;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x86: // add a, (hl)
		{
			A += mem[(addr16)HL()];
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x87: // add a, a
		{
			A += A;
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x88: // adc a, b
		{
			A += B + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x89: // adc a, c
		{
			A += B + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8A: // adc a, d
		{
			A += D + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8B: // adc a, e
		{
			A += E + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8C: // adc a, h
		{
			A += H + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8D: // adc a, l
		{
			A += L + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8E: // adc a, (hl)
		{
			A += mem[(addr16)HL()] + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x8F: // adc a, a
		{
			A += A + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x90: // sub b
		{
			A -= B;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x91: // sub c
		{
			A -= C;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x92: // sub d
		{
			A -= D;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x93: // sub e
		{
			A -= E;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x94: // sub h
		{
			A -= H;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x95: // sub l
		{
			A -= L;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x96: // sub (hl)
		{
			A -= mem[(addr16)HL()];
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x97: // sub a 
		{
			A -= A;
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x98: // sbc a, b
		{
			A -= B - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x99: // sbc a, c
		{
			A -= C - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9A: // sbc a, d
		{
			A -= D - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9B: // sbc a, e
		{
			A -= E - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9C: // sbc a, h
		{
			A -= H - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9D: // sbc a, l
		{
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9E: // sbc a, (hl)
		{
			A -= mem[(addr16)HL()] - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0x9F: // sbc a, a
		{
			A -= A - carry();
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
			updateZero(A);
			PC++;
			break;
		}
		case 0xA0: // and b
		{
			A &= B;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA1: // and c
		{
			A &= C;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA2: // and d
		{
			A &= D;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA3: // and e
		{
			A &= E;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA4: // and h
		{
			A &= H;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA5: // and l
		{
			A &= L;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA6: // and (hl)
		{
			A &= mem[(addr16)HL()];
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA7: // and a
		{
			A &= A;
			resetCarry();
			resetN();
			setHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA8: // xor b
		{
			A ^= B;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xA9: // xor c
		{
			A ^= C;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAA: // xor d
		{
			A ^= D;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAB: // xor e
		{
			A ^= E;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAC: // xor h
		{
			A ^= H;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAD: // xor l
		{
			A ^= L;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAE: // xor (hl)
		{
			A ^= mem[(addr16)HL()];
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xAF: // xor a
		{
			A ^= A;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB0: // or b
		{
			A |= B;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB1: // or c
		{
			A |= C;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB2: // or d
		{
			A |= D;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB3: // or e
		{
			A |= E;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB4: // or h
		{
			A |= H;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB5: // or l
		{
			A |= L;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB6: // or (hl)
		{
			A |= mem[(addr16)HL()];
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
			PC++;
			break;
		}
		case 0xB7: // or a
		{
			A |= A;
			resetCarry();
			resetN();
			resetHC();
			updateZero(A);
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
			cmp(mem[(addr16)HL()]);
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
			B = mem[SP];
			SP++;
			C = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xC2: // jp nz, ** 
		{
			jp(!zero(), get16(), 3);
			break;
		}
		case 0xC3: // jp **
		{
			jp(true, get16(), 3);
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
			mem[SP] = C;
			SP--;
			mem[SP] = B;
			PC++;
			break;
		}
		case 0xC6: // add a, *
		{
			A += mem[PC + 1];
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
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
			clockCycles -= 6;
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
			clockCycles -= 7; // adjust clock cycles down (always true - no cycles added for testing)
			break;
		}
		case 0xCE: // adc a, *
		{
			A += mem[PC + 1] + carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
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
			D = mem[SP];
			SP++;
			E = mem[SP];
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
			mem[SP] = E;
			SP--;
			mem[SP] = D;
			PC++;
			break;
		}
		case 0xD6: // sub *
		{
			A -= mem[PC + 1];
			updateCarry(A);
			updateN(SUB);
			updateHC(A);
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
			clockCycles -= 6;
			IME = true;
			break;
		}
		case 0xDA: // jp c, **
		{
			jp(carry(), get16(), 3);
			break;
		}
		case 0xDB: // in a, (*) ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xDC: // call c, **
		{
			call(carry());
			break;
		}
		case 0xDD: // IX INSTRUCTIONS ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xDE: // sbc a, *
		{
			A -= mem[PC + 1] - carry();
			updateCarry(A);
			updateN(ADD);
			updateHC(A);
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
			const ubyte low = mem[PC + 1];
			const addr16 addr = 0xFF00 + low;
			mem[addr] = A;

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
			H = mem[SP];
			SP++;
			L = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xE2: // ld (C), a
		{
			const addr16 addr = static_cast<ubyte>(C) + 0xFF00;
			mem[addr] = A;
			PC++;
			break;
		}
		case 0xE3: // NOP
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			break;
		}
		case 0xE4: // call po, **
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			break;
		}
		case 0xE5: // push hl
		{
			SP--;
			mem[SP] = L;
			SP--;
			mem[SP] = H;
			PC++;
			break;
		}
		case 0xE6: // and *
		{
			A &= mem[PC + 1];
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
			SP += mem[PC + 1];
			resetZero();
			resetN();
			updateHC(SP);
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
			mem[get16()] = A;
			PC += 3;
			break;
		}
		case 0xEB: // ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xEC: // ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xED: // EXTENDED INSTRUCTIONS ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xEE: // xor *
		{
			A ^= mem[PC + 1];
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
			const ubyte low = mem[PC + 1]; // low byte
			const addr16 addr = 0xFF00 + low; // high byte always 0xFF00
			A = mem[addr];
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
			A = mem[SP];
			SP++;
			F = mem[SP];
			SP++;
			PC++;
			break;
		}
		case 0xF2: // ld a, (C)
		{
			const addr16 addr = static_cast<ubyte>(C) + 0xFF00;
			A = mem[addr];
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
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xF5: // push af
		{
			SP--;
			mem[SP] = F;
			SP--;
			mem[SP] = A;
			PC++;
			break;
		}
		case 0xF6: // or *
		{
			A |= mem[PC + 1];
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
			HL(SP);
			PC++;
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
			A = mem[get16()];
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
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xFD: // ~!GB
		{
			std::cout << "Opcode not supported by Gameboy: " << toHex((int16_t)opcode) << " at " << toHex(PC) << std::endl;
			system("pause"); // this is for debugging only
			break;
		}
		case 0xFE: // cp *
		{
			cmp(mem[PC + 1]);
			PC += 2;
			break;
		}
		case 0xFF: // rst 0x38
		{
			rst(0x38);
			PC++;
			break;
		}
		default: // if the definition of a char changes 
		{
			std::cout << "You should never ever see this" << std::endl;
			std::cout << "But here is the opcode : " << toHex(opcode) << std::endl;
			std::cin.ignore(); // make the user see what they have done
			PC++;
			break;
		}
	}
}

template<typename T>
const std::string toHex(const T val)
{
	std::stringstream stream;
	stream << std::hex << val;
	std::string result(stream.str());
	return "0x" + result;
}

const std::string toHex(const char val)
{
	std::stringstream stream;
	stream << std::hex << (uint16_t)val; // cast to an unsigned short so it isnt treated as a character
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
		file.read(fileStr, size); // load the rom file into the file
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
	return EXIT_SUCCESS; // ROM load completed succesfully
}