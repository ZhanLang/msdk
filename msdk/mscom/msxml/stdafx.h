// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//#include "RsDebug.h"
     
#define RSXML_DETAIL	RSXML|RSLOG_DETAIL
#define RSXML_ACTION	RSXML|RSLOG_ACTION
#define RSXML_WAINNING	RSXML|RSLOG_WAINNING
#define RSXML_ALERT		RSXML|RSLOG_ALERT
#define RSXML_FATAL		RSXML|RSLOG_FATAL

#define TIXML_USE_STL			// Uses stl in TinyXML



#ifndef tstring
#ifdef _UNICODE
#define tstring std::wstring
#else	
#define tstring std::string
#endif
#endif

#ifndef re_uuidof
#define re_uuidof(iface)	__uuidof(iface)
#endif

#include <mscom/mscominc.h>


