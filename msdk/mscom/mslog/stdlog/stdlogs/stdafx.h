// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <mslog/syslay/dbg.h>

//#pragma comment(lib, "syslay.lib")
//现在只使用静态库的版本，不再依赖syslay.dll
// #ifdef	_DEBUG 
// #pragma comment(lib, "syslaysD.lib")
// #else
// #pragma comment(lib, "syslays.lib")
// #endif
// TODO: reference additional headers your program requires here
