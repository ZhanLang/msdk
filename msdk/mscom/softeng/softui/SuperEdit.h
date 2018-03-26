#pragma once
#include <map>
#include "EditTextDlg.h"

class CSuperEdit : public CWindowImpl<CSuperEdit, CEdit>
{
public:
	BEGIN_MSG_MAP(CSuperEdit)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDBClick)
	END_MSG_MAP()

	LRESULT OnLButtonDBClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CString strText;
		GetWindowText(strText);

		CEditTextDlg editTextDlg(strText);
		if(IDOK == editTextDlg.DoModal(GetParent()))
		{
			SetWindowText(editTextDlg.GetNewText());
		}
		return 0;
	}
};

class CSuperEditMgr
{
public:
	~CSuperEditMgr()
	{
		SuperEditList::iterator _find = m_editList.begin();
		for(; _find != m_editList.end(); _find++)
		{
			CSuperEdit* pEdit = _find->second;
			if(pEdit)
			{
				delete pEdit;
				pEdit = NULL;
			}
		}
		m_editList.clear();
	}

	void AddDlgEdit(UINT uId, HWND hCtrlWnd)
	{
		if(m_editList.find(uId) != m_editList.end())
			return;

		CSuperEdit* pEdit = new CSuperEdit();
		pEdit->SubclassWindow(hCtrlWnd);
		m_editList[uId] = pEdit;
	}

	void DelDlgEdit(UINT uId)
	{
		SuperEditList::iterator _find = m_editList.find(uId);
		if(_find == m_editList.end())
			return;

		CSuperEdit* pEdit = _find->second;
		if(pEdit)
		{
			pEdit->UnsubclassWindow(TRUE);
			delete pEdit;
			pEdit = NULL;
		}

		m_editList.erase(_find);
	}

	typedef std::map<UINT, CSuperEdit*> SuperEditList;
	SuperEditList m_editList;
};