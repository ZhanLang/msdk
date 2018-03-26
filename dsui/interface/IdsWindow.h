#ifndef IdsWindow_h__
#define IdsWindow_h__

#include "IdsBuilder.h"

template<class T> 
class DSUI_API CIdsFrameWindow : public CFrameWindowImpl<T>,
	public CMessageFilter, public CIdleHandler
{
public:
	typedef map<DWORD,CRect> MAP_MARKEDRECT;
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_MSG_MAP(CIdsFrameWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_NCACTIVATE(OnNcActivate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_NCCALCSIZE, OnNcCalcSize)
		CHAIN_MSG_MAP(CFrameWindowImpl<T>)
	END_MSG_MAP()

public:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnSize(UINT nType, CSize size);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UINT OnNcHitTest(CPoint point);
	BOOL OnNcActivate(BOOL bActive);
	LRESULT OnNcCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	BOOL OnEraseBkgnd(CDCHandle dc);

public:
	void AssignXMLPath(LPCTSTR szFilePath);
	void SetBorderPadding(int l, int t, int r, int b, int c);
	IdsUIBase* GetDsuiElement(LPARAM lParam, LPVOID pUserInfo);
	IdsUIBase* GetDsuiElement(WORD wUIID);

private:
	CIdsBuilder m_idsBuilder;
};

template<class T> 
BOOL CIdsFrameWindow<T>::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<T>::PreTranslateMessage(pMsg))
		return TRUE;
	return FALSE;
}

template<class T>
BOOL CIdsFrameWindow<T>::OnIdle()
{
	return FALSE;
}

template<class T>
void CIdsFrameWindow<T>::AssignXMLPath(LPCTSTR szFilePath)
{
	m_idsBuilder.AssignXMLPath(szFilePath);
}

template<class T>
void CIdsFrameWindow<T>::SetBorderPadding(int l, int t, int r, int b, int c)
{
	m_idsBuilder.SetBorderPadding(l, t, r, b, c);
}

template<class T>
IdsUIBase* CIdsFrameWindow<T>::GetDsuiElement(LPARAM lParam, LPVOID pUserInfo)
{
	return m_idsBuilder.GetDsuiElement(lParam, pUserInfo);
}

template<class T>
IdsUIBase* CIdsFrameWindow<T>::GetDsuiElement(WORD wUIID)
{
	return m_idsBuilder.GetDsuiElement(wUIID);
}

template<class T>
int CIdsFrameWindow<T>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	ModifyStyle(WS_CAPTION, 0);

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	lpCreateStruct->hMenu = NULL;

	m_idsBuilder.Attach(m_hWnd);
	m_idsBuilder.builder();
	return 0;
}

template<class T>
void CIdsFrameWindow<T>::OnSize(UINT nType, CSize size)
{	
	m_idsBuilder.OnSize(nType, size);
}

template<class T>
LRESULT CIdsFrameWindow<T>::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
	bHandled = FALSE;
	return 1;
}

template<class T>
UINT CIdsFrameWindow<T>::OnNcHitTest(CPoint point)
{
	return m_idsBuilder.OnNcHitTest(point);
}

template<class T>
BOOL CIdsFrameWindow<T>::OnNcActivate(BOOL bActive)
{
	return m_idsBuilder.OnNcActivate(bActive);
}

template<class T>
LRESULT CIdsFrameWindow<T>::OnNcCalcSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return m_idsBuilder.OnNcCalcSize(uMsg, wParam, lParam, bHandled);
}

template<class T>
BOOL CIdsFrameWindow<T>::OnEraseBkgnd(CDCHandle dc)
{
	return m_idsBuilder.OnEraseBkgnd(dc);
}

#endif // IdsWindow_h__