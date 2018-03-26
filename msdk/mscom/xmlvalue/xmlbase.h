
#pragma once

#include <map>
#include <iostream>
#include <vector>
#include <time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>


using namespace std;


#define	 MIN_COUNT		256
#define  MIN_BYTES		1024

#define  WC_COUNT		1024
#define  WC_BYTES		2048


static const wchar_t  AMPW = L'&';
static const wchar_t  rawEntityW[] = { L'<',   L'>',   L'&',    L'\'',    L'\"',    0 };
static const wchar_t* xmlEntityW[] = { L"lt;", L"gt;", L"amp;", L"apos;", L"quot;", 0 };
static const int	  xmlEntLenW[] = { 3,     3,     4,      5,       5 };


static const char  AMPA = '&';
static const char  rawEntityA[] = { '<',   '>',   '&',    '\'',    '\"',    0 };
static const char* xmlEntityA[] = { "lt;", "gt;", "amp;", "apos;", "quot;", 0 };
static const int   xmlEntLenA[] = { 3,     3,     4,      5,       5 };


//----------------------------------------------------------------------------------

static const wchar_t VALUE_TAGW[]     = L"<value>";
static const wchar_t VALUE_ETAGW[]    = L"</value>";

static const wchar_t BOOLEAN_TAGW[]   = L"<boolean>";
static const wchar_t BOOLEAN_ETAGW[]  = L"</boolean>";
static const wchar_t DOUBLE_TAGW[]    = L"<double>";
static const wchar_t DOUBLE_ETAGW[]   = L"</double>";
static const wchar_t INT_TAGW[]       = L"<int>";
static const wchar_t I4_TAGW[]        = L"<i4>";
static const wchar_t I4_ETAGW[]       = L"</i4>";
static const wchar_t STRING_TAGW[]    = L"<string>";
static const wchar_t DATETIME_TAGW[]  = L"<dateTime.iso8601>";
static const wchar_t DATETIME_ETAGW[] = L"</dateTime.iso8601>";
static const wchar_t BASE64_TAGW[]    = L"<base64>";
static const wchar_t BASE64_ETAGW[]   = L"</base64>";

static const wchar_t ARRAY_TAGW[]     = L"<array>";
static const wchar_t DATA_TAGW[]      = L"<data>";
static const wchar_t DATA_ETAGW[]     = L"</data>";
static const wchar_t ARRAY_ETAGW[]    = L"</array>";

static const wchar_t STRUCT_TAGW[]    = L"<struct>";
static const wchar_t MEMBER_TAGW[]    = L"<member>";
static const wchar_t NAME_TAGW[]      = L"<name>";
static const wchar_t NAME_ETAGW[]     = L"</name>";
static const wchar_t MEMBER_ETAGW[]   = L"</member>";
static const wchar_t STRUCT_ETAGW[]   = L"</struct>";


//---------------------------------------------------------------------------------

static const char VALUE_TAGA[]		= "<value>";
static const char VALUE_ETAGA[]		= "</value>";

static const char BOOLEAN_TAGA[]	= "<boolean>";
static const char BOOLEAN_ETAGA[]	= "</boolean>";
static const char DOUBLE_TAGA[]		= "<double>";
static const char DOUBLE_ETAGA[]	= "</double>";
static const char INT_TAGA[]		= "<int>";
static const char I4_TAGA[]			= "<i4>";
static const char I4_ETAGA[]		= "</i4>";
static const char STRING_TAGA[]		= "<string>";
static const char DATETIME_TAGA[]	= "<dateTime.iso8601>";
static const char DATETIME_ETAGA[]	= "</dateTime.iso8601>";
static const char BASE64_TAGA[]		= "<base64>";
static const char BASE64_ETAGA[]	= "</base64>";

static const char ARRAY_TAGA[]		= "<array>";
static const char DATA_TAGA[]		= "<data>";
static const char DATA_ETAGA[]		= "</data>";
static const char ARRAY_ETAGA[]		= "</array>";

static const char STRUCT_TAGA[]		= "<struct>";
static const char MEMBER_TAGA[]		= "<member>";
static const char NAME_TAGA[]		= "<name>";
static const char NAME_ETAGA[]		= "</name>";
static const char MEMBER_ETAGA[]	= "</member>";
static const char STRUCT_ETAGA[]	= "</struct>";

//------------------------------------------------------------------------------------------
const wchar_t METHODCALL_BEGIN[]		= L"<methodCall>";
const wchar_t METHODNAME_TAG[]			= L"<methodName>";


const wchar_t REQUEST_BEGIN[]			= L"<methodCall><methodName>";
const wchar_t REQUEST_END_METHODNAME[]	= L"</methodName>\r\n";
const wchar_t PARAMS_TAG[]				= L"<params>";
const wchar_t PARAMS_ETAG[]				= L"</params>";
const wchar_t PARAM_TAG[]				= L"<param>";
const wchar_t PARAM_ETAG[]				= L"</param>";
const wchar_t REQUEST_END[]				= L"</methodCall>\r\n";
const wchar_t METHODRESPONSE_TAG[]		= L"<methodResponse>";
const wchar_t FAULT_TAG[]				= L"<fault>";


//------------------------------------------------------------------------------------------

const wchar_t XML_TAG[]					= L"<?xml version=\"1.0\" encoding=\"utf-8\"?>";
const wchar_t MESSAGE_BEGIN[]			= L"<message>";
const wchar_t MESSAGE_END[]				= L"</message>";
const wchar_t HEADER_BEIGIN[]			= L"<header>";
const wchar_t HEADER_END[]				= L"</header>";
const wchar_t BODY_BEGIN[]				= L"<body>";
const wchar_t BODY_END[]				= L"</body>";
const wchar_t ID_BEIGN[]				= L"<id>";
const wchar_t ID_END[]					= L"</id>";
const wchar_t FROM_BEGIN[]				= L"<from>";
const wchar_t FROM_END[]				= L"</from>";
const wchar_t TO_BEGIN[]				= L"<to>";
const wchar_t TO_END[]					= L"</to>";
const wchar_t TIME_BEGIN[]				= L"<time>";
const wchar_t TIME_END[]				= L"</time>";


//------------------------------------------------------------------------------------------

inline std::wstring  SPECIAL_NORMAL_REPONSE_BEGIN( const std::wstring & plug )
{
	std::wstring  result;
	if( plug.empty() ) { return L"<unknown-methodResponse><params><param>\r\n\t"; }

	result += L"<";
	result += plug.c_str();
	result += L"-methodResponse><params><param>\r\n\t";

	return result;
}

inline std::wstring SPECIAL_NORMAL_REPONSE_END( const std::wstring & plug )
{
	std::wstring result;
	if( plug.empty() ) { return L"\r\n</param></params></unknown-methodResponse>\r\n"; }

	result += L"\r\n</param></params></";
	result += plug.c_str();
	result += L"-methodResponse>\r\n";

	return result;
};

inline std::wstring SPECIAL_FAULT_REPONSE_BEGIN( const std::wstring & plug )
{
	std::wstring result;
	if( plug.empty() ) { return L"<unknown-methodResponse><fault>\r\n\t"; }

	result += L"<";
	result += plug.c_str();
	result += L"-methodResponse><fault>\r\n\t";

	return result;
};


inline std::wstring SPECIAL_FAULT_REPONSE_END( const std::wstring & plug )
{
	std::wstring result;
	if( plug.empty() ) { return L"\r\n</fault></unknown-methodResponse>\r\n"; }

	result += L"\r\n</fault></";
	result += plug.c_str();
	result += L"-methodResponse>\r\n";

	return result;
};
						

const wchar_t RESPONSE_NORMAL_BEGIN[]				= L"<methodResponse><params><param>\r\n\t";
const wchar_t RESPONSE_NORMAL_END[]					= L"\r\n</param></params></methodResponse>\r\n";
const wchar_t RESPONSE_FAULT_BEGIN[]				= L"<methodResponse><fault>\r\n\t";
const wchar_t RESPONSE_FAULT_END[]					= L"\r\n</fault></methodResponse>\r\n";


const std::wstring FAULTCODE   = L"faultCode";
const std::wstring FAULTSTRING = L"faultString";


class CXmlValueW;
class CXmlValueA;






