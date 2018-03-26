#include "stdafx.h"
#include "XmlUtilW.h"
#include <Windows.h>

void CXmlUtilW::Log(const wchar_t* fmt, ...)
{
	va_list va;
	wchar_t buf[WC_COUNT] = {0};

	va_start( va, fmt);

	_vsnwprintf_s( buf, WC_COUNT-1, fmt, va);

	buf [WC_COUNT - 1 ] = 0;

#ifdef USE_WINDOWS_DEBUG
	OutputDebugString(buf); OutputDebugString("\n");
#else
	std::locale loc("");
	std::wcout.imbue(loc);
	std::wcout << buf << std::endl; 
#endif  

}


void CXmlUtilW::Error(const wchar_t* fmt, ...)
{
	va_list va;
	wchar_t buf[WC_COUNT] = {0};

	va_start(va, fmt);

	_vsnwprintf_s( buf, WC_COUNT-1, fmt, va);

	buf [WC_COUNT - 1 ] = 0;

#ifdef USE_WINDOWS_DEBUG
	OutputDebugString(buf); 
	OutputDebugString("\n");
#else
	std::locale loc("");
	std::wcout.imbue(loc);
	std::wcout << buf << std::endl;
#endif  
}


// offset 返回点将是查找的下一个起始处
std::wstring CXmlUtilW::ParseTag(const wchar_t* tag, std::wstring const& xml, int* offset)
{
	if ( *offset >= int( xml.length() ) ) 
	{
		return std::wstring();
	}

	size_t istart = xml.find(tag, *offset);

	if (istart == std::wstring::npos)	// std::wstring::npos = -1
	{
		return std::wstring();
	}

	istart += wcslen(tag);

	std::wstring etag = L"</";

	etag += tag + 1;

	size_t iend = xml.find(etag, istart);

	if (iend == std::wstring::npos)
	{
		return std::wstring();
	}

	*offset = int(iend + etag.length());

	return xml.substr(istart, iend-istart);
}

// offset 返回找到Tag的内容所在处偏移
bool CXmlUtilW::FindTag(const wchar_t* tag, std::wstring const& xml, int* offset)
{
	if ( *offset >= int( xml.length() ) ) { return false; }

	size_t istart = xml.find(tag, *offset);

	if (istart == std::wstring::npos) { return false; }

	*offset = int(istart + wcslen(tag) );

	return true;
}

std::wstring CXmlUtilW::GetNextTag(std::wstring const& xml, int* offset)
{
	if ( *offset >= int( xml.length() ) )  { return std::wstring(); }

	size_t pos = *offset;

	const wchar_t* cp = xml.c_str() + pos;

	while ( *cp && iswspace(*cp)) 
	{
		++cp;
		++pos;
	}

	if (*cp != L'<') return std::wstring();

	std::wstring s;
	do 
	{
		s += *cp;
		++pos;
	} while (*cp++ != L'>' && *cp != 0);

	*offset = int(pos);
	return s;
}


bool CXmlUtilW::NextTagIs(const wchar_t * tag, std::wstring const& xml, int* offset)
{
	if ( *offset >= int(xml.length() ) ) return false;

	const wchar_t* cp = xml.c_str() + *offset;

	int nc = 0;

	while (*cp && isspace(*cp)) 
	{
		++cp;
		++nc;
	}

	int len = int( wcslen(tag) );

	if( *cp && ( wcsncmp(cp, tag, len) == 0 ) ) 
	{
		*offset += nc + len;
		return true;
	}
	return false;
}

std::wstring CXmlUtilW::XmlEncode(const std::wstring& raw)
{
	std::wstring::size_type iRep = raw.find_first_of(rawEntityW);

	if (iRep == std::wstring::npos)
		return raw;

	std::wstring encoded(raw, 0, iRep);
	std::wstring::size_type iSize = raw.size();

	while (iRep != iSize)
	{
		int iEntity;

		for (iEntity=0; rawEntityW[iEntity] != 0; ++iEntity)
			if (raw[iRep] == rawEntityW[iEntity])
			{
				encoded += AMPW;
				encoded += xmlEntityW[iEntity];
				break;
			}
			if (rawEntityW[iEntity] == 0)
				encoded += raw[iRep];
			++iRep;
	}
	return encoded;
}


std::wstring CXmlUtilW::XmlDecode(const std::wstring& encoded)
{
	std::wstring::size_type iAmp = encoded.find(AMPW);

	if (iAmp == std::wstring::npos)
		return encoded;

	std::wstring decoded(encoded, 0, iAmp);
	std::wstring::size_type iSize = encoded.size();

	decoded.reserve(iSize);

	const wchar_t* ens = encoded.c_str();

	while (iAmp != iSize) 
	{
		if (encoded[iAmp] == AMPW && iAmp+1 < iSize) 
		{
			int iEntity;

			for (iEntity=0; xmlEntityW[iEntity] != 0; ++iEntity)
				//if (encoded.compare(iAmp+1, xmlEntLen[iEntity], xmlEntity[iEntity]) == 0)
				if ( wcsncmp( ens+iAmp+1, xmlEntityW[iEntity], xmlEntLenW[iEntity] ) == 0)
				{
					decoded += rawEntityW[iEntity];
					iAmp += xmlEntLenW[iEntity]+1;
					break;
				}
				if (xmlEntityW[iEntity] == 0)    // unrecognized sequence
					decoded += encoded[iAmp++];

		} else {
			decoded += encoded[iAmp++];
		}
	}

	return decoded;
}


bool CXmlUtilW::GetStringInterTag(std::wstring const& valueXml, std::wstring const & strEndTag, int* offset, std::wstring & strReturn )
{
	std::wstring  strRet = L"";

	size_t valueEnd = valueXml.find(L'<', *offset);

	if (valueEnd == std::wstring::npos)
		return false;     // No end tag;

	strRet = valueXml.substr(*offset, valueEnd-*offset);

	*offset += strRet.length();
	*offset += strEndTag.length();

	strReturn = strRet;

	return true;
}