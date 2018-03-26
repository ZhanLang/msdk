#include "stdafx.h"
#include "P2PClient.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
    }

    return TRUE;
}


//R_EXPORT1(IMSP2PClient, CMsP2PClient);
IID Margin_iid_map[] = { __uuidof(IMSP2PClient), }; 
IUnknown* Margin_interface_create(IUnknown *pUnkOuter, IID riid) 
{ 
	if(IsEqualGUID(riid, __uuidof(IMSP2PClient))) 
	{ 
		CMsP2PClient* pNew = new CMsP2PClient; 
		if(pNew) 
		{ 
			if(pUnkOuter) 
			{ 
				CUnknownImp_Inner* pInner = (CUnknownImp_Inner*)(pNew); 
				pInner->init_class_inner(pUnkOuter); 
			} 
			pNew->AddRef(); 
			return static_cast<IUnknown*>(static_cast<IMSP2PClient*>(pNew)); 
		} 
	} 
	return 0; 
} 
int Margin_iid_count() 
{ 
	return sizeof(Margin_iid_map)/sizeof(IID);
}; 

IID Margin_iid_getat(int nIndex) 
{ 
	return Margin_iid_map[nIndex]; 
};

// ------
BEGIN_CLIDMAP
	CLIDMAPENTRY_BEGIN
		CLIDMAPENTRY_PROGID(CLSID_MSP2PClient, CMsP2PClient, L"MsP2PClient")
	CLIDMAPENTRY_END
END_CLIDMAP

DEFINE_ALL_EXPORTFUN

