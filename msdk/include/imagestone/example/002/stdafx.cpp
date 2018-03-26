// stdafx.cpp : source file that includes just the standard includes
// 002.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

//-----------------------------------------------------------------------------------
void DDX_Text_No_Tip (CDataExchange* pDX, int nIDC, int& t, int tDefault)
{
    if (pDX->m_bSaveAndValidate)
    {
        CString   s ;
        pDX->m_pDlgWnd->GetDlgItemText(nIDC, s) ;
        t = (s.GetLength() ? StrToInt(s) : tDefault) ;
    }
    else
    {
        DDX_Text(pDX, nIDC, t) ;
    }
}
