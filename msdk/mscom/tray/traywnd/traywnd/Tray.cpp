#include "StdAfx.h"
#include "Tray.h"
#include "mslog\logtool\logtool.h"


CTray::CTray(void)
: m_dwRunThreadId(0)
, m_bStart(false)
{
}

CTray::~CTray(void)
{
}

STDMETHODIMP CTray::init_class(IMSBase* prot, IMSBase* punkOuter)
{
	m_pRot = prot;	//把rot缓存下来
	return S_OK;
}

STDMETHODIMP CTray::Init(void*)
{
	HWND hWnd = m_hideWnd.Create(NULL, this);
	RASSERT(hWnd, E_FAIL);

	AddConnectionPoint(__uuidof(IWndMessageConnectPoint), m_WndMsgConnectPoint);

	return S_OK;
}

STDMETHODIMP CTray::Uninit()
{
	m_hideWnd.Destroy();

	DelConnectionPoint(__uuidof(IWndMessageConnectPoint), m_WndMsgConnectPoint); //再从连接容器里删除
	m_WndMsgConnectPoint.EmptyConnection();
	if(m_pRot)
		m_pRot = INULL;

	return S_OK;
}

STDMETHODIMP CTray::Start()
{
	
	m_bStart = true;
	
	m_dwRunThreadId = GetCurrentThreadId();
	int nRc = Run();
	m_bStart = false;

	return S_OK;
}

STDMETHODIMP CTray::Stop()
{
	return S_OK;
}

STDMETHODIMP_(HWND) CTray::GetWndHwnd()
{
	return m_hideWnd.GetHwnd();
}

STDMETHODIMP CTray::NotifyExit(bool* bExit)
{
	if(bExit)
		*bExit = true;

	if(m_bStart && m_dwRunThreadId)
	{
		::PostThreadMessage(m_dwRunThreadId, WM_QUIT, NULL, NULL);
	}

	return S_OK;
}

LRESULT CTray::OnWinMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle)
{
	UTIL::com_ptr<IMsEnumConnections> pEnum;
	RASSERT(SUCCEEDED(m_WndMsgConnectPoint.EnumConnections((IMsEnumConnections**)&pEnum)), 0);
	RASSERT(pEnum, 0);

	while(1)
	{
		MSCONNECTDATA data[1];
		RASSERT(S_OK == pEnum->Next(1, data, NULL), 0); //一个一个遍历的，S_FALSE不行
		
		UTIL::com_ptr<IWndMessageConnectPoint> p = data[0].pUnk;
		if(p)
		{
			LRESULT rc = p->OnWndMessage(msg, wParam, lParam, bHandle);
			RTEST(bHandle, rc);
		}
	}
	
	return 0;
}

