
// testcomx64.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Ctestcomx64App:
// See testcomx64.cpp for the implementation of this class
//

class Ctestcomx64App : public CWinApp//CWinAppEx
{
public:
	Ctestcomx64App();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Ctestcomx64App theApp;