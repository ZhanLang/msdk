
#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#include <windows.h>

#include "wininet.h"
#include "tchar.h"
#include <string>
#include <assert.h>
#include "Winsock2.h"



#define SAFE_DELETE_ARRAY(a) {if (a) {delete [] a; a = NULL;}}
#define fsnew(p, T, L){p=new T[L];assert(p);memset(p,0,L*sizeof(T));}
