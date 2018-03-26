// inetbasedome.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "mscom/mscominc.h"
#include "inetbase/inetbase_i.h"
#include "combase/MsBufferManangerImp.h"
#include "util/filestream.h"
#include "util/mapfilestream.h"
#include <assert.h>
using namespace msinet;
int _tmain(int argc, _TCHAR* argv[])
{
	
	CoInitializeEx(NULL , COINIT_APARTMENTTHREADED);

	UTIL::com_ptr<IINetBaseMgr> pINetBaseMgr;
	RFAILED(CoCreateInstance(CLSID_INetBaseMgr , NULL , CLSCTX_INPROC_SERVER , __uuidof(IINetBaseMgr),(void**)&pINetBaseMgr));
	RASSERT(pINetBaseMgr , E_FAIL);
	
	UTIL::com_ptr<fsInternetSession_i> pfsInternetSession;
	RFAILED(pINetBaseMgr->CreateInternetSession(&pfsInternetSession.m_p));
	RASSERT(pINetBaseMgr , E_FAIL);

	fsInternetResult fr = pfsInternetSession->Create(NULL , IAT_NOPROXY , NULL);
	RASSERT(fr == IR_SUCCESS , E_FAIL);

	UTIL::com_ptr<fsHttpConnection_i> pfsHttpConnection;
	RFAILED(pINetBaseMgr->CreateHttpConnection(&pfsHttpConnection.m_p));
	RASSERT(pfsHttpConnection ,E_FAIL);

	UTIL::com_ptr<fsInternetServerConnection_i> pfsInternetServerConnection = pfsHttpConnection;
	RASSERT(pfsInternetServerConnection ,E_FAIL);
	RASSERT(pfsInternetServerConnection->Initialize(pfsInternetSession) == IR_SUCCESS , E_FAIL);

	//http://dx1.qqtn.com//qq2/LoveVPN.zip
	RASSERT(pfsHttpConnection->Connect("dx1.qqtn.com" , NULL , NULL) == IR_SUCCESS , E_FAIL);

	UTIL::com_ptr<fsHttpFile_i> pfsHttpFile;
	RFAILED(pINetBaseMgr->CreateHttpFile(&pfsHttpFile.m_p));

	UTIL::com_ptr<fsInternetFile_i> pfsInternetFile = pfsHttpFile;
	RASSERT(pfsInternetFile->Initialize(pfsInternetServerConnection)  == IR_SUCCESS , E_FAIL);

	
	fr = pfsInternetFile->QuerySize("/qq2/LoveVPN.zip");
	UINT64 uSize = pfsInternetFile->GetFileSize();

	fr = pfsInternetFile->Open("/qq2/LoveVPN.zip" , 0);
	LPCSTR pFileName =pfsInternetFile->GetSuggestedFileName();
	UTIL::com_ptr<IMsBuffer> pBuf;
	CMsBufferManangerImp mgr;
	mgr.CreateMsBuf(&pBuf.m_p , MAX_PATH);
	BYTE pData[MAX_PATH] = {0};
	DWORD dwReadLen = 0;
	bool bFaild = false;

	{
		safe_mapfilestream ms((DWORD)uSize);
		BOOL bRet = ms.Open(_T("c:\\LoveVPN.zip"));
		do 
		{

			if (pfsInternetFile->Read(pData , MAX_PATH ,&dwReadLen) == IR_SUCCESS)
			{
				if (dwReadLen)
				{
					ms.Append(pData , dwReadLen , dwReadLen);
				}
			}
			else
			{
				bFaild = true;
				break;
			}

		} while (dwReadLen>0);
	}
	
	
	//assert(bFaild);
	
	CoUninitialize();
	return 0;
}

