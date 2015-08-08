#ifndef GB_COMMON_H
#define GB_COMMON_H

#include <cstdint>

typedef uint16_t addr16; // 16-bit address
typedef uint8_t addr8; // 8-bit address

typedef int16_t reg16; // 16-bit register
typedef int8_t reg; // register

typedef int8_t byte; // 8-bit for when sign is unspecified
typedef uint8_t ubyte;
typedef int8_t sbyte;

typedef int16_t word;
typedef uint16_t uword;
typedef int16_t sword;

#endif // GB_COMMON_H