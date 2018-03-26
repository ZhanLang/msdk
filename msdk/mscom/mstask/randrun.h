// RandRun.h: interface for the CRandRun class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDRUN_H__C6AA994D_4462_4CFA_8649_EA72616EB666__INCLUDED_)
#define AFX_RANDRUN_H__C6AA994D_4462_4CFA_8649_EA72616EB666__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CThread.h"
#include <list>
#include "StartTask.h"
#include "check.h"

class CRandRun  
{
public:
	CRandRun();
	virtual ~CRandRun();
	BOOL Init(CStartTask*  m_pStart);
	void UnInit();

	BOOL SetRandRun(LPCTSTR lpszCmd);
	BOOL SetSmartupCmd(LPCTSTR lpszCmd, BOOL bOpenUpdate);

	//提供给新的升级方式使用
	enum
	{
		UPDATE_NO	= 0,	//没有升级时间
		UPDATE_NOW,			//需要启动升级
		UPDATE_WAIT			//有升级时间但需要等待
	};
private:
	int ReadNetConfigUpdate();

	//运行定时升级的程序
	CThread  m_RunNewverThread;
	void DoRunNewverThread();
	HANDLE	m_hEventStop;
	TCHAR	m_szSmartupCmd[MAX_PATH];
	BOOL	m_bOpenUpdate;	//是否使用即时升级
	
	struct  tag_RandRunInfo
	{
		TCHAR	szCmd[MAX_PATH];
		int     nCount;
	};
	typedef tag_RandRunInfo RANDRUNINFO,*LPRANDRUNINFO;
	typedef std::list<RANDRUNINFO> RANDRUNINFOLIST;

	TCHAR m_szNetConfig [MAX_PATH];

	CStartTask*  m_pStart;
	CCheck m_check;
};

#endif // !defined(AFX_RANDRUN_H__C6AA994D_4462_4CFA_8649_EA72616EB666__INCLUDED_)
