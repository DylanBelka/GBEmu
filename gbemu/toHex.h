#ifndef GB_TOHEX_H
#define GB_TOHEX_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "types.h"

template<typename T>
inline const std::string toHex(const T val)
{
	std::stringstream stream;
	stream << std::hex << val;
	std::string result(stream.str());
	return "0x" + result;
}

template<> 
inline const std::string toHex(sbyte val)
{
	return toHex(static_cast<unsigned short>(val));
}

template<> 
inline const std::string toHex(ubyte val)
{
	return toHex(static_cast<unsigned short>(val));
}

template<> 
inline const std::string toHex(char val)
{
	return toHex(static_cast<unsigned short>(val));
}

#endif // GB_TOHEX_H