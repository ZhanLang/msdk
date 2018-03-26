#include "StdAfx.h"
#include "MsXMSCProxy.h"
#include "XSynchronization.h"
#include "XCredit.h"
#include "XMMSC.h"
#include "CXMessage.h"
#include "CXCredit.h"
#include "Confusion.h"
#include "Profile.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

static TCHAR XMSCLOGIN[] = _T("XMSC: Hello world!.");
/////////////////////////////////////////////////////////////////////////////
CMsXMSCProxy::CMsXMSCProxy()
{
	m_dwRefCount = 0;
	m_bLogin     = FALSE;
	m_pRsBufferManager =(IUnknown*)0;
	m_pRuningTable = (IUnknown*)0;
}
CMsXMSCProxy::CMsXMSCProxy(IUnknown * pBufferManager)
{
	m_dwRefCount = 0;
	m_bLogin     = FALSE;
	m_pRsBufferManager = pBufferManager;
}
/////////////////////////////////////////////////////////////////////////////
CMsXMSCProxy::~CMsXMSCProxy()
{
}

HRESULT CMsXMSCProxy::init_class(IUnknown* prot, IUnknown* punkOuter)
{
	RASSERT(prot, E_INVALIDARG);

	//取出根对象
	m_pRuningTable = prot;
	RASSERT(m_pRuningTable,E_UNEXPECTED);
	RFAILED(m_pRuningTable->GetObject(CLSID_MsBufferMgr,__uuidof(IMsBufferMgr), (void**)&m_pRsBufferManager));
	RASSERT(m_pRsBufferManager,E_UNEXPECTED);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsXMSCProxy::Login(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync, DWORD dwTimeOut, LPCTSTR lpszName)
{
	SYN_OBJ_EX(&m_xMutex);
	if (m_bLogin)
	{
		//ZM1_GrpDbg(GroupName,_T("[MSC]已经登录过了\n"));
		return FALSE;
	}

	XMessage* lpXMessage = CreateLoginMessage(dwSubsystemID);
	if (NULL == lpXMessage)
	{
		//ZM1_GrpDbg(GroupName,_T("[MSC]CreateLoginMessage 失败\n"));
		return FALSE;
	}

	CProfile* lpProfile = CProfile::GetInstance();
	if (NULL == lpProfile)
	{
		return FALSE;
	}

	LPCTSTR lpszPipeName = (lpszName) ? lpszName : lpProfile->GetDefaultPipeName();

	if (!m_objXMSCProxyImpl.Initialize(lpszPipeName, lpXMSCDispatcher, m_pRsBufferManager,bAsync))
	{
		FreeMessage(lpXMessage);
		//ZM1_GrpDbg(GroupName,_T("[MSC]m_objXMSCProxyImpl.Initialize 失败\n"));
		return FALSE;
	}

	if (!m_objXMSCProxyImpl.Issue(lpXMessage))
	{
		FreeMessage(lpXMessage);
		m_objXMSCProxyImpl.Uninitialize();
		//ZM1_GrpDbg(GroupName,_T("[MSC]发登录消息 失败\n"));
		return FALSE;
	}

	DWORD dwResult = WaitForSingleObject(m_objXMSCProxyImpl.Event(), dwTimeOut);
	if (dwResult != WAIT_OBJECT_0)
	{
		m_objXMSCProxyImpl.Uninitialize();
		//ZM1_GrpDbg(GroupName,_T("[MSC]WaitForSingleObject 失败\n"));
		return FALSE;
	}
	
	m_bLogin = TRUE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMsXMSCProxy::Logoff()
{
	{
		SYN_OBJ_EX(&m_xMutex);
		if (!m_bLogin)
		{
			return;
		}
		m_bLogin = FALSE;
	}

	m_objXMSCProxyImpl.Uninitialize();
	//m_bLogin = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsXMSCProxy::Issue(RSMSGID dwMessageID,IMsBuffer* lpXMessage)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin )
	{
		return FALSE;
	}
	//ZM1_GrpDbg(GroupName,_T("[MSC]发出消息ID=0x%016I64X\n"),dwMessageID);

	BYTE * pDataBuf =NULL;
	DWORD dwDataLen=0;
	UTIL::com_ptr<IMsBuffer> pRsBuffer = lpXMessage;
	if(pRsBuffer && pRsBuffer->GetRawBuffer(&pDataBuf,&dwDataLen) == S_OK)
	{
		//ZM1_GrpDbg(GroupName,_T("[MSC]得到消息体长度=%d\n"),dwDataLen);		
	}

	XMessage * lpNewXMessage = CreateIssueMessage(dwMessageID,dwDataLen);
	if(pDataBuf && dwDataLen>0)
		memcpy((void*)((BYTE*)lpNewXMessage+sizeof(XMessage)),pDataBuf,dwDataLen);
	//ZM1_GrpDbg(GroupName,_T("[MSC]客户端发消息内存=0x%x,len=%d\n"),lpNewXMessage,dwDataLen);

	if(! m_objXMSCProxyImpl.Issue(lpNewXMessage))
	{	
		FreeMessage(lpNewXMessage);
		return FALSE;
	}
	return TRUE;
}

BOOL CMsXMSCProxy::Send(RSMSGID dwMessageID,IMsBuffer* lpXMessage)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin )
	{
		return FALSE;
	}
	//ZM1_GrpDbg(GroupName,_T("[MSC]发出消息ID=0x%016I64X\n"),dwMessageID);

	BYTE * pDataBuf =NULL;
	DWORD dwDataLen=0;
	UTIL::com_ptr<IMsBuffer> pRsBuffer = lpXMessage;
	if(pRsBuffer && pRsBuffer->GetRawBuffer(&pDataBuf,&dwDataLen) == S_OK)
	{
		//ZM1_GrpDbg(GroupName,_T("[MSC]得到消息体长度=%d\n"),dwDataLen);		
	}

	XMessage * lpNewXMessage = CreateIssueMessage(dwMessageID,dwDataLen);
	if(pDataBuf && dwDataLen>0)
		memcpy((void*)((BYTE*)lpNewXMessage+sizeof(XMessage)),pDataBuf,dwDataLen);
	//ZM1_GrpDbg(GroupName,_T("[MSC]客户端发消息内存=0x%x,len=%d\n"),lpNewXMessage,dwDataLen);

	BOOL bRet = m_objXMSCProxyImpl.Send(lpNewXMessage);
	FreeMessage(lpNewXMessage);
    if(bRet)
	    return TRUE;
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsXMSCProxy::Subscribe(DWORD dwCount, RSMSGID* lpMessageID)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin)
	{
		return FALSE;
	}
	for(DWORD i=0;i<dwCount;i++)
	{
		//ZM1_GrpDbg(GroupName,_T("[MSC]订阅的消息ID=0x%016I64X\n"),lpMessageID[i]);
	}

	XMessage* lpXMessage = CreateSubscribeMessage(dwCount, lpMessageID);
	if (NULL == lpXMessage)
	{
		return FALSE;
	}

	if (!m_objXMSCProxyImpl.Issue(lpXMessage))
	{
		FreeMessage(lpXMessage);
		return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsXMSCProxy::UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin)
	{
		return FALSE;
	}

	XMessage* lpXMessage = CreateUnSubscribeMessage(dwCount, lpMessageID);
	if (NULL == lpXMessage)
	{
		return FALSE;
	}

	if (!m_objXMSCProxyImpl.Issue(lpXMessage))
	{
		FreeMessage(lpXMessage);
		return FALSE;
	}
	return TRUE;	
}

/////////////////////////////////////////////////////////////////////////////
VOID CMsXMSCProxy::FreeMessage(XMessage* lpXMessage)
{
	if (lpXMessage)
	{
		delete[] ((LPBYTE)lpXMessage);
	}
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CMsXMSCProxy::AllocMessage(DWORD dwDataLen)
{
	LPBYTE lpBuffer = new BYTE[dwDataLen + sizeof(XMessage)];
	if (NULL != lpBuffer)
	{
		CXMessage* lpXMessage = (CXMessage*)lpBuffer;
		lpXMessage->Initialize(0xFFFFFFFF, dwDataLen);
	}

	return (XMessage*)lpBuffer;
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CMsXMSCProxy::CreateIssueMessage(RSMSGID dwMessageID,DWORD dwDataLen)
{
	XMessage* lpXMessage = AllocMessage(dwDataLen);
	if (NULL == lpXMessage)
	{
		return NULL;
	}
	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	lpCXMessage->Initialize(dwMessageID, dwDataLen, 0);

	return lpXMessage;

}

XMessage* CMsXMSCProxy::CreateLoginMessage(DWORD dwSubSystemID)
{
	DWORD dwDataLen = sizeof(XCredit) + (_tcslen(XMSCLOGIN) + 1) * sizeof(TCHAR);
	XMessage* lpXMessage = AllocMessage(dwDataLen);
	if (NULL == lpXMessage)
	{
		return NULL;
	}

	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	lpCXMessage->Initialize(XMMSC_LOGIN_REQUEST, dwDataLen, 0);

	CXCredit* lpXCredit = (CXCredit*)lpCXMessage->Data();
	lpXCredit->Initlize(GetCurrentProcessId(), GetCurrentThreadId(), dwSubSystemID, dwDataLen);

	CopyMemory(lpXCredit->Data(), XMSCLOGIN, (_tcslen(XMSCLOGIN) + 1) * sizeof(TCHAR));

	R_RSA_PRIVATE_KEY PRIVATE_KEY;
	GetPrivateKey(&PRIVATE_KEY);
	ReverseKey(&PRIVATE_KEY);
	lpXCredit->MakeSignature(&PRIVATE_KEY);
	ZeroMemory(&PRIVATE_KEY, sizeof(R_RSA_PRIVATE_KEY));

	return lpXMessage;
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CMsXMSCProxy::CreateSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID)
{
	DWORD dwDataLen = sizeof(DWORD) + sizeof(RSMSGID) * dwCount ;
	XMessage* lpXMessage = AllocMessage(dwDataLen);
	if (NULL == lpXMessage)
	{
		return NULL;
	}

	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	lpCXMessage->Initialize(XMMSC_SUBSCRIBE, dwDataLen, 1);

	LPBYTE lpData = lpCXMessage->Data();
	*((DWORD*)lpData) = dwCount;

	if (0 != dwCount)
	{
		lpData += sizeof(DWORD);
		CopyMemory(lpData, (LPBYTE)lpMessageID, dwCount * sizeof(RSMSGID));
	}
	
	return lpXMessage;
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CMsXMSCProxy::CreateUnSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID)
{
	DWORD dwDataLen = sizeof(dwCount) + sizeof(RSMSGID) *dwCount;
	XMessage* lpXMessage = AllocMessage(dwDataLen);
	if (NULL == lpXMessage)
	{
		return NULL;
	}
	
	CXMessage* lpCXMessage = (CXMessage*)lpXMessage;
	lpCXMessage->Initialize(XMMSC_UNSUBSCRIBE, dwDataLen, 2);
	
	LPBYTE lpData = lpCXMessage->Data();
	*((DWORD*)lpData) = dwCount;
	
	if (0 != dwCount)
	{
		lpData += sizeof(DWORD);
		CopyMemory(lpData, (LPBYTE)lpMessageID, dwCount * sizeof(RSMSGID));
	}
	
	return lpXMessage;
}

/////////////////////////////////////////////////////////////////////////////
VOID CMsXMSCProxy::GetPrivateKey(R_RSA_PRIVATE_KEY* privateKey)
{
	privateKey->bits = 512;
	CConfusion::FillModulus(privateKey->modulus);
	CConfusion::ReverseBuffer(privateKey->modulus, MAX_RSA_MODULUS_LEN);
	CConfusion::FillPublicExponent(privateKey->publicExponent);
	CConfusion::ReverseBuffer(privateKey->publicExponent, MAX_RSA_MODULUS_LEN);
	CConfusion::FillExponent(privateKey->exponent);
	CConfusion::ReverseBuffer(privateKey->exponent, MAX_RSA_MODULUS_LEN);
	CConfusion::FillPrime1(privateKey->prime[0]);
	CConfusion::ReverseBuffer(privateKey->prime[0], MAX_RSA_PRIME_LEN);
	CConfusion::FillPrime2(privateKey->prime[1]);
	CConfusion::FillPrimeExponent1(privateKey->primeExponent[0]);
	CConfusion::ReverseBuffer(privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
	CConfusion::FillPrimeExponent2(privateKey->primeExponent[1]);
	CConfusion::FillCoefficient(privateKey->coefficient);
	CConfusion::ReverseBuffer(privateKey->coefficient, MAX_RSA_PRIME_LEN);
}

/////////////////////////////////////////////////////////////////////////////
VOID CMsXMSCProxy::ReverseKey(R_RSA_PRIVATE_KEY* privateKey)
{
	CConfusion::ReverseBuffer(privateKey->modulus, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->publicExponent, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->exponent, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->prime[0], MAX_RSA_PRIME_LEN);
	CConfusion::ReverseBuffer(privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
	CConfusion::ReverseBuffer(privateKey->coefficient, MAX_RSA_PRIME_LEN);
}

