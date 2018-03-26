#pragma once

#include "xmlbase.h"

class CXmlUtilW
{

public:

	static std::wstring ParseTag(const wchar_t* tag, std::wstring const& xml, int* offset);

	static bool FindTag(const wchar_t* tag, std::wstring const& xml, int* offset);

	static std::wstring GetNextTag(std::wstring const& xml, int* offset);

	static bool NextTagIs(const wchar_t * tag, std::wstring const& xml, int* offset);

	static std::wstring XmlEncode(const std::wstring& raw);

	static std::wstring XmlDecode(const std::wstring& encoded);

	static void Log(const wchar_t* fmt, ...);

	static void Error(const wchar_t* fmt, ...);

	// 获取tag之间的字符串
	static bool GetStringInterTag(std::wstring const& valueXml, std::wstring const & strEndTag, int* offset, std::wstring & strReturn );
};

