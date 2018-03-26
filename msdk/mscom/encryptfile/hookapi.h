#pragma once


#include "stdio.h"
#include <tchar.h>
#include <stdlib.h>
#include <map>
#include <stack>
#include <string>
#include "atlstr.h"


#include "CommunicationManager.h"

//标准宏定义
#ifndef RASSERT
#	define RASSERT(x, _h_r_) { if(!(x)) return _h_r_; }
#endif

#ifndef RFAILED
#	define RFAILED(x) { HRESULT _h_r_ = (x); if(FAILED(_h_r_)) return _h_r_; }
#endif

#ifndef NMs_ASSERT
#define NMs_ASSERT(x)\
	do \
{\
	if (!(x))\
	{\
	char buffer[256] = { 0 }; \
	sprintf_s(buffer, 256,"File=(%s) Line=(%d) <%s>", __FILE__, __LINE__, #x); \
	/*OutputDebugStringA(buffer); */\
	}\
} while (0);
#endif

#define NMs_ASSERT_RETURN(x, hr) NMs_ASSERT(x)  RASSERT(x, hr)
#define NMs_RFAILED_RETURN(x) NMs_ASSERT(SUCCEEDED(x))  RFAILED(x)



//HOOK入口点
typedef struct _HOOK_ENTRY_
{
	int			nTag;
	CHAR		strfModuleName[MAX_PATH];
	CHAR		strFuncName[MAX_PATH];
	FARPROC		pRealFunctionAddr;
	FARPROC		pNewFunctionAddr;
	LPVOID		pUserData;
	BOOL		bHook;
} HOOK_ENTRY,*PHOOK_ENTRY;


//////////////////////////////////////////////////////////////////////////
//定义HOOK类型










