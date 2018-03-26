/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,16th,2014

Module Name:

	dsCheckBox.h 

Abstract: checkbox ÊµÀýÀà


*/
/************************************************************************/


#ifndef __DSCHECKBOX_H__
#define __DSCHECKBOX_H__

class dsCheckBox:public dsUIBase, public IdsCheckBox
{
public:
	static dsUIBase* CALLBACK NewDsuiElesInst()
	{
		return new dsCheckBox();
	}
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci)
	{
		if(NULL == pci)
			return FALSE;

		pci->procNewUI = NewDsuiElesInst;
		pci->strClassName = L"dsCheckBox";

		return TRUE;
	}

	virtual BOOL ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BOOL bRedraw = FALSE;

		int nCheckState = GetCheckState();

		BOOL bSendMsg = FALSE;

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			if(DSUI_STATE_NORMAL & m_nState)
			{
				m_nState = DSUI_STATE_HOT;
				bRedraw = TRUE;
			}

			break;

		case WM_LBUTTONDOWN:
			dsGlobalStateMgr::getInstance()->SetCapture(this);
			m_nState = DSUI_STATE_PUSHED;
			bRedraw = TRUE;

			break;

		case WM_LBUTTONUP:
			{

				int nOldState = m_nState;

				dsGlobalStateMgr::getInstance()->ReleaseCapture();

				CPoint point(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

				if (m_rcUI.PtInRect(point))
				{

					m_nState = DSUI_STATE_HOT;

					if(DSUI_STATE_PUSHED & nOldState)
					{

						if(DSUI_STATE_CHECKED & nCheckState)
							nCheckState = DSUI_STATE_NULL;
						else if(DSUI_STATE_PARTCHECKED & nCheckState)
							nCheckState = DSUI_STATE_CHECKED;
						else
							nCheckState = DSUI_STATE_CHECKED;

						bSendMsg = TRUE;

						
					}


				}
				else
					m_nState = DSUI_STATE_NORMAL;


				bRedraw = TRUE;

			}
			break;

		case WM_MOUSELEAVE:

			m_nState = DSUI_STATE_NORMAL;
			bRedraw = TRUE;
			break;

		default:

			break;
		}

		m_nState |= nCheckState;

		if(bRedraw)
		{
			dsuiInvalidateRect(hwnd, m_rcUI, TRUE);
		}


		if(bSendMsg)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_wID, BN_CLICKED), (LPARAM)(LPVOID)(IdsUIBase*)this);

		return FALSE;
	}

public:

	//////////////////////////////////////////////////////////////////////////
	//override
	virtual int SetCheckState(int nCheckState)
	{
		int nCheckStateOld = GetCheckState();

		int nState = m_nState & (~nCheckStateOld);

		if(DSUI_STATE_CHECKED & nCheckState)
			m_nState = nState | DSUI_STATE_CHECKED;
		else if(DSUI_STATE_PARTCHECKED & m_nState)
			m_nState = nState | DSUI_STATE_PARTCHECKED;
		else
			m_nState = nState;

		if(nCheckStateOld != nCheckState)
			dsuiInvalidateRect(m_hwnd, m_rcUI, TRUE);

		return nCheckStateOld;
	}

	virtual int GetCheckState()
	{
		if(DSUI_STATE_CHECKED & m_nState)
			return DSUI_STATE_CHECKED;
		else if(DSUI_STATE_PARTCHECKED & m_nState)
			return DSUI_STATE_PARTCHECKED;
		else
			return DSUI_STATE_UNCHECKED;
	}

};


#endif /*__DSCHECKBOX_H__*/