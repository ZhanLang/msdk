#pragma once

#include "xmlbase.h"

class CXmlUtilA
{
public:

	static std::string ParseTag(const char* tag, std::string const& xml, int* offset);

	static bool FindTag(const char* tag, std::string const& xml, int* offset);

	static std::string GetNextTag(std::string const& xml, int* offset);

	static bool NextTagIs(const char* tag, std::string const& xml, int* offset);

	static std::string XmlEncode(const std::string& raw);

	static std::string XmlDecode(const std::string& encoded);

	static void Log(int level, const char* fmt, ...);

	static void Error(const char* fmt, ...);
};

