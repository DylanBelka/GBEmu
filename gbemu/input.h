#ifndef GB_INPUT_H
#define GB_INPUT_H

struct GBKeys
{
	char keys[2];
	char colID;
};

enum KeyGroups
{
	p14 = 0,
	p15
};

enum KeyCodes
{
	keyRight = 0x1,
	keyA = 0x1,
	keyLeft = 0x2,
	keyB = 0x2,
	keyUp = 0x4,
	keySelect = 0x4,
	keyDown = 0x8,
	keyStart = 0x8
};

#endif // GB_INPUT_H