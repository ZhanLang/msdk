#include "StdAfx.h"
#include "MBDevice.h"
#include <wbemcli.h>
#include <comutil.h>
#pragma comment(lib,"comsuppwd.lib")
#pragma comment(lib,"Wbemuuid.lib")
CMBDevice::CMBDevice(void)
{
}

CMBDevice::~CMBDevice(void)
{
}

std::wstring GetProperty(IWbemClassObject *pclsObj, LPCWSTR lpszProperty)
{ 

	VARIANT vtProperty;  
	VariantInit( &vtProperty ); 
	pclsObj->Get(lpszProperty, 0, &vtProperty, NULL, NULL );
	
	std::wstring t;
	if (vtProperty.vt != VT_NULL && vtProperty.vt != VT_EMPTY)
	{
		t = vtProperty.bstrVal ? vtProperty.bstrVal:L""; 
	}
	
	VariantClear( &vtProperty ); 

	return t;
}


BOOL CMBDevice::Scan(DeviceScanNotify* pNotify)
{
	RASSERT(pNotify, FALSE);

	HRESULT hres = S_OK;

	hres = CoInitializeSecurity(NULL,
		-1, 
		NULL, 
		NULL, 
		RPC_C_AUTHN_LEVEL_DEFAULT, 
		RPC_C_IMP_LEVEL_IMPERSONATE, 
		NULL, 
		EOAC_NONE, 
		NULL 
		);

	 IWbemLocator *pLoc = NULL;
	 hres = CoCreateInstance(CLSID_WbemLocator, 
		 0, 
		 CLSCTX_INPROC_SERVER, 
		 IID_IWbemLocator, 
		 (LPVOID *) &pLoc
		 );
	 if ( FAILED(hres) ) {
		 return FALSE; // Program has failed.
	 }

	 RASSERT(SUCCEEDED(hres), FALSE);

	 IWbemServices *pSvc = NULL;
	 hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), 
		 NULL, 
		 NULL, 
		 0, 
		 NULL, 
		 0, 
		 0, 
		 &pSvc 
		 );

	 hres = CoSetProxyBlanket(pSvc, 
		 RPC_C_AUTHN_WINNT, 
		 RPC_C_AUTHZ_NONE, 
		 NULL, 
		 RPC_C_AUTHN_LEVEL_CALL,
		 RPC_C_IMP_LEVEL_IMPERSONATE, 
		 NULL, 
		 EOAC_NONE 
		 );
	 if ( FAILED(hres) ) {
		 pSvc->Release();
		 pLoc->Release();
		 return FALSE; 
	 }

	 IEnumWbemClassObject *pEnumerator = NULL;  
	 hres = pSvc->ExecQuery(  
		 bstr_t("WQL"),   
		 bstr_t("SELECT * FROM Win32_BaseBoard"),  
		 WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		 NULL,  
		 &pEnumerator  
		 ); 

	 if( FAILED(hres) )  
	 {  
		 pSvc->Release();  
		 pLoc->Release();  
		 return FALSE;  
	 }  

	 while( pEnumerator )  
	 {  
		 IWbemClassObject *pclsObj = NULL;  
		 ULONG uReturn = 0;  
		 pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn );  
		 if( uReturn == 0 )  
		 {  
			 break;  
		 }  

		 
		 CMBDevice* pDevice = new CMBDevice();
		 pDevice->SetManufacturer(::GetProperty(pclsObj,L"Manufacturer"));
		 pDevice->SetHID(::GetProperty(pclsObj,L"SerialNumber"));
		 pDevice->SetDeviceModel(::GetProperty(pclsObj,L"Product"));
		
		 //如需获取BIOS，南北桥，在下面添加代码
		 //todo

		 pclsObj->Release(); 
		 pNotify->OnDeviceScanNotify(pDevice);
	 } 

	 // 释放资源  
	 pEnumerator->Release();  
	 pSvc->Release();  
	 pLoc->Release();      
	return TRUE;
}
