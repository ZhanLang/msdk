// rsxml3.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "rapidtree.h"

#define TREE_IMPL	CRapidTree

USE_DEFAULT_DLL_MAIN
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
	#ifdef _UNICODE
		CLIDMAPENTRY(CLSID_IXMLTreeW,TREE_IMPL)
	#else
		CLIDMAPENTRY(CLSID_IXMLTreeA,TREE_IMPL)
	#endif
	CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN

__declspec(dllexport) LPVOID GetDLLObject(DWORD dwType)
{
	IXMLTree3* pTree3 = new TREE_IMPL;
	pTree3->AddRef();

	return pTree3;
}
