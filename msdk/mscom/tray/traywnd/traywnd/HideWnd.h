#pragma once

interface IWinProc
{
	virtual LRESULT OnWinMessage(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandle) = 0;
};

#define	HIDEWNDCLASSNAME	TEXT("Margin HideWnd Class")
#define	HIDEWNDNAME			TEXT("Margin HideWnd")

class CHideWnd
{
public:
	CHideWnd(void);
	~CHideWnd(void);

	HWND Create(HWND hOwnerWnd, IWinProc* pWndProc = NULL);
	VOID Destroy();

	// 隐藏窗体的消息处理函数
	static LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

	HWND GetHwnd(){return m_hWnd;}
private:
	HWND		m_hWnd;

	HWND		m_hOwnerWnd;
	IWinProc*	m_pWndProc;
};
