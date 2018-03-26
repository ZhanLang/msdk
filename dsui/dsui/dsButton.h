/************************************************************************/
/* 
Author:

lourking. (languang).All rights reserved.

Create Time:

	1,7th,2014

Module Name:

	dsButton.h

Abstract: dsui button


*/
/************************************************************************/

#ifndef __DSBUTTON_H__
#define __DSBUTTON_H__

#include "dsUIBase.h"

class dsButton:public dsUIBase
{
public:
	//////////////////////////////////////////////////////////////////////////
	//register

	static dsUIBase* CALLBACK NewDsuiElesInst(){
		return new dsButton();
	}
	static BOOL GetClassInfo(__out PDSUICLASSINFO pci){
		if(NULL == pci)
			return FALSE;

		pci->procNewUI = NewDsuiElesInst;
		pci->strClassName = L"dsButton";

		return TRUE;
	}

	virtual BOOL ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		BOOL bRedraw = FALSE;

		BOOL bSendMsg = FALSE;

		if(DSUI_STATE_NULL == m_nState)
			m_nState = DSUI_STATE_NORMAL;

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			if(m_nState & DSUI_STATE_NORMAL)
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

		if(bRedraw)
		{
			dsuiInvalidateRect(hwnd, m_rcUI, TRUE);

			ATLTRACE("drawfffffff --- %d\n",rand());
		}

		if(bSendMsg)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(m_wID, BN_CLICKED), (LPARAM)(LPVOID)(IdsUIBase*)this);

		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	//override



private:

};



#endif