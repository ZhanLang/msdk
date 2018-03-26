#ifndef IdsDialog_h__
#define IdsDialog_h__

#include "IdsBuilder.h"

template<DWORD IDD_DIALOG, class T> 
class DSUI_API CIdsDialog : public CDialogImpl<T>
{
public:
	enum { IDD = IDD_DIALOG	 };
	BEGIN_MSG_MAP(CIdsDialog)
		MSG_WM_SIZE(OnSize)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

public:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnSize(UINT nType, CSize size);
	UINT OnNcHitTest(CPoint point);
	BOOL OnEraseBkgnd(CDCHandle dc);

public:
	void AssignXMLPath(LPCTSTR szFilePath);
	void SetBorderPadding(int l, int t, int r, int b, int c);
	IdsUIBase* GetDsuiElement(LPARAM lParam, LPVOID pUserInfo);
	IdsUIBase* GetDsuiElement(WORD wUIID);

private:
	CIdsBuilder m_idsBuilder;
};

template<DWORD IDD_DIALOG, class T> 
void CIdsDialog<IDD_DIALOG, T>::AssignXMLPath(LPCTSTR szFilePath)
{
	m_idsBuilder.AssignXMLPath(szFilePath);
}

template<DWORD IDD_DIALOG, class T> 
void CIdsDialog<IDD_DIALOG, T>::SetBorderPadding(int l, int t, int r, int b, int c)
{
	m_idsBuilder.SetBorderPadding(l, t, r, b, c);
}

template<DWORD IDD_DIALOG, class T> 
IdsUIBase* CIdsDialog<IDD_DIALOG, T>::GetDsuiElement(LPARAM lParam, LPVOID pUserInfo)
{
	return m_idsBuilder.GetDsuiElement(lParam, pUserInfo);
}

template<DWORD IDD_DIALOG, class T> 
IdsUIBase* CIdsDialog<IDD_DIALOG, T>::GetDsuiElement(WORD wUIID)
{
	return m_idsBuilder.GetDsuiElement(wUIID);
}

template<DWORD IDD_DIALOG, class T> 
LRESULT CIdsDialog<IDD_DIALOG, T>::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	ModifyStyle(WS_CAPTION, WS_POPUP);

	m_idsBuilder.Attach(m_hWnd);
	m_idsBuilder.builder();

	OnSize(0, CSize(0,0));
	return 0;
}

template<DWORD IDD_DIALOG, class T> 
void CIdsDialog<IDD_DIALOG, T>::OnSize(UINT nType, CSize size)
{	
	m_idsBuilder.OnSize(nType, size);
}

template<DWORD IDD_DIALOG, class T> 
UINT CIdsDialog<IDD_DIALOG, T>::OnNcHitTest(CPoint point)
{
	return m_idsBuilder.OnNcHitTest(point);
}

template<DWORD IDD_DIALOG, class T> 
BOOL CIdsDialog<IDD_DIALOG, T>::OnEraseBkgnd(CDCHandle dc)
{
	return m_idsBuilder.OnEraseBkgnd(dc);
}

#endif // IdsDialog_h__