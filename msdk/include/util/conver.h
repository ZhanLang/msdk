
#pragma once

#include <string>
#include <sstream> 

namespace msdk{;
namespace UTIL{;
	
template<typename CharType , typename T>
static std::basic_string<CharType, std::char_traits<CharType>, std::allocator<CharType>> toString(const T& val)
{
	std::basic_stringstream<CharType> out;
	out << val;
	return out.str();
}



};};