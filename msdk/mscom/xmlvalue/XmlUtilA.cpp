#include "stdafx.h"
#include "XmlUtilA.h"
#include <Windows.h>

# pragma warning(disable:4996)


void CXmlUtilA::Log(int level, const char* fmt, ...)
{
	
	va_list va;
	char buf[1024];
	va_start( va, fmt);
	vsnprintf(buf,sizeof(buf)-1,fmt,va);
	buf[sizeof(buf)-1] = 0;

#ifdef USE_WINDOWS_DEBUG
	OutputDebugString(buf); 
	OutputDebugString("\n"); 
#else
	std::cout << buf << std::endl; 
#endif  
}


void CXmlUtilA::Error(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char buf[1024];
	vsnprintf(buf,sizeof(buf)-1,fmt,va);
	buf[sizeof(buf)-1] = 0;
	
#ifdef USE_WINDOWS_DEBUG
	OutputDebugString(buf); 
	OutputDebugString("\n"); 
#else
	std::cout << buf << std::endl; 
#endif  

}



std::string  CXmlUtilA::ParseTag(const char* tag, std::string const& xml, int* offset)
{
	if (*offset >= int(xml.length())) return std::string();
	size_t istart = xml.find(tag, *offset);
	if (istart == std::string::npos) return std::string();
	istart += strlen(tag);
	std::string etag = "</";
	etag += tag + 1;
	size_t iend = xml.find(etag, istart);
	if (iend == std::string::npos) return std::string();

	*offset = int(iend + etag.length());
	return xml.substr(istart, iend-istart);
}


bool CXmlUtilA::FindTag(const char* tag, std::string const& xml, int* offset)
{
	if (*offset >= int(xml.length())) return false;
	size_t istart = xml.find(tag, *offset);
	if (istart == std::string::npos)
		return false;

	*offset = int(istart + strlen(tag));
	return true;
}

bool CXmlUtilA::NextTagIs(const char* tag, std::string const& xml, int* offset)
{
	if (*offset >= int(xml.length())) return false;
	const char* cp = xml.c_str() + *offset;
	int nc = 0;
	while (*cp && isspace(*cp)) {
		++cp;
		++nc;
	}

	int len = int(strlen(tag));
	if  (*cp && (strncmp(cp, tag, len) == 0)) {
		*offset += nc + len;
		return true;
	}
	return false;
}


std::string CXmlUtilA::GetNextTag(std::string const& xml, int* offset)
{
	if (*offset >= int(xml.length())) return std::string();

	size_t pos = *offset;
	const char* cp = xml.c_str() + pos;
	while (*cp && isspace(*cp)) {
		++cp;
		++pos;
	}

	if (*cp != '<') return std::string();

	std::string s;
	do {
		s += *cp;
		++pos;
	} while (*cp++ != '>' && *cp != 0);

	*offset = int(pos);
	return s;
}


// Replace xml-encoded entities with the raw text equivalents.

std::string CXmlUtilA::XmlDecode(const std::string& encoded)
{
	std::string::size_type iAmp = encoded.find(AMPA);
	if (iAmp == std::string::npos)
		return encoded;

	std::string decoded(encoded, 0, iAmp);
	std::string::size_type iSize = encoded.size();
	decoded.reserve(iSize);

	const char* ens = encoded.c_str();
	while (iAmp != iSize) {
		if (encoded[iAmp] == AMPA && iAmp+1 < iSize) {
			int iEntity;
			for (iEntity=0; xmlEntityA[iEntity] != 0; ++iEntity)
				//if (encoded.compare(iAmp+1, xmlEntLen[iEntity], xmlEntity[iEntity]) == 0)
				if (strncmp(ens+iAmp+1, xmlEntityA[iEntity], xmlEntLenA[iEntity]) == 0)
				{
					decoded += rawEntityA[iEntity];
					iAmp += xmlEntLenA[iEntity]+1;
					break;
				}
				if (xmlEntityA[iEntity] == 0)    // unrecognized sequence
					decoded += encoded[iAmp++];

		} else {
			decoded += encoded[iAmp++];
		}
	}

	return decoded;
}


// Replace raw text with xml-encoded entities.

std::string CXmlUtilA::XmlEncode(const std::string& raw)
{
	std::string::size_type iRep = raw.find_first_of(rawEntityA);
	if (iRep == std::string::npos)
		return raw;

	std::string encoded(raw, 0, iRep);
	std::string::size_type iSize = raw.size();

	while (iRep != iSize) {
		int iEntity;
		for (iEntity=0; rawEntityA[iEntity] != 0; ++iEntity)
			if (raw[iRep] == rawEntityA[iEntity])
			{
				encoded += AMPA;
				encoded += xmlEntityA[iEntity];
				break;
			}
			if (rawEntityA[iEntity] == 0)
				encoded += raw[iRep];
			++iRep;
	}
	return encoded;
}