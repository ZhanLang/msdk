#include "StdAfx.h"
#include "CXMSCProxy.h"
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
CXMSCProxy::CXMSCProxy()
{
	m_dwRefCount = 0;
	m_bLogin     = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CXMSCProxy::~CXMSCProxy()
{
}

/////////////////////////////////////////////////////////////////////////////
DWORD CXMSCProxy::AddRef()
{
	return InterlockedIncrement((LPLONG)&m_dwRefCount);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CXMSCProxy::Release()
{
	LONG dwRefCount = InterlockedDecrement((LPLONG)&m_dwRefCount);
	if (0 >= dwRefCount)
	{
		delete this;
	}
	
	return dwRefCount;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxy::Login(DWORD dwSubsystemID, XMSCDispatcher* lpXMSCDispatcher, BOOL bAsync, DWORD dwTimeOut, LPCTSTR lpszName)
{
	if (m_bLogin)
	{
		return FALSE;
	}
	SYN_OBJ_EX(&m_xMutex);
	

	XMessage* lpXMessage = CreateLoginMessage(dwSubsystemID);
	if (NULL == lpXMessage)
	{
		return FALSE;
	}

	CProfile* lpProfile = CProfile::GetInstance();
	if (NULL == lpProfile)
	{
		return FALSE;
	}

	LPCTSTR lpszPipeName = (lpszName) ? lpszName : lpProfile->GetDefaultPipeName();

	if (!m_objXMSCProxyImpl.Initialize(lpszPipeName, lpXMSCDispatcher,NULL, bAsync))
	{
		FreeMessage(lpXMessage);
		return FALSE;
	}

	if (!m_objXMSCProxyImpl.Issue(lpXMessage))
	{
		FreeMessage(lpXMessage);
		m_objXMSCProxyImpl.Uninitialize();
		return FALSE;
	}

	DWORD dwResult = WaitForSingleObject(m_objXMSCProxyImpl.Event(), dwTimeOut);
	if (dwResult != WAIT_OBJECT_0)
	{
		m_objXMSCProxyImpl.Uninitialize();
		return FALSE;
	}
	
	m_bLogin = TRUE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
VOID CXMSCProxy::Logoff()
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin)
	{
		return;
	}

	m_objXMSCProxyImpl.Uninitialize();
	m_bLogin = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxy::Issue(XMessage* lpXMessage)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin)
	{
		return FALSE;
	}

	return m_objXMSCProxyImpl.Issue(lpXMessage);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CXMSCProxy::Subscribe(DWORD dwCount, RSMSGID* lpMessageID)
{
	SYN_OBJ_EX(&m_xMutex);
	if (!m_bLogin)
	{
		return FALSE;
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
BOOL CXMSCProxy::UnSubscribe(DWORD dwCount, RSMSGID* lpMessageID)
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
VOID CXMSCProxy::FreeMessage(XMessage* lpXMessage)
{
	if (lpXMessage)
	{
		delete[] ((LPBYTE)lpXMessage);
	}
}

/////////////////////////////////////////////////////////////////////////////
XMessage* CXMSCProxy::AllocMessage(DWORD dwDataLen)
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
XMessage* CXMSCProxy::CreateLoginMessage(DWORD dwSubSystemID)
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
XMessage* CXMSCProxy::CreateSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID)
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
XMessage* CXMSCProxy::CreateUnSubscribeMessage(DWORD dwCount, RSMSGID* lpMessageID)
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
VOID CXMSCProxy::GetPrivateKey(R_RSA_PRIVATE_KEY* privateKey)
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
VOID CXMSCProxy::ReverseKey(R_RSA_PRIVATE_KEY* privateKey)
{
	CConfusion::ReverseBuffer(privateKey->modulus, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->publicExponent, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->exponent, MAX_RSA_MODULUS_LEN);
	CConfusion::ReverseBuffer(privateKey->prime[0], MAX_RSA_PRIME_LEN);
	CConfusion::ReverseBuffer(privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
	CConfusion::ReverseBuffer(privateKey->coefficient, MAX_RSA_PRIME_LEN);
}

