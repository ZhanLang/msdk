#ifndef _STD_AFX_H_
#define _STD_AFX_H_

/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif 

#pragma warning(disable : 4996)
#define WIN32_LEAN_AND_MEAN

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#endif
#include <TCHAR.h>
#include <windows.h>
//#include <crtdbg.h>

#include "mscom\mscominc.h"
#include "xml\ixmltree3.h"

#ifndef CFG_FILE_NAME
#	define CFG_FILE_NAME _T("logtool.ini")
#endif

#ifndef LOG_FILE_NAME
#	define LOG_FILE_NAME _T("msc.log")
#endif


#include "mslog\logtool\logtool.h"

#define GroupName _T("msc")

using namespace msdk;
using namespace mscom;
#pragma warning(disable : 4786)
//{{AFX_INSERT_LOCATION}}

/////////////////////////////////////////////////////////////////////////////
#endif
