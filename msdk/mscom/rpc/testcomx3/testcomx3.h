// testcomx3.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号


// Ctestcomx3App:
// 有关此类的实现，请参阅 testcomx3.cpp
//

class Ctestcomx3App : public CWinApp
{
public:
	Ctestcomx3App();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern Ctestcomx3App theApp;
