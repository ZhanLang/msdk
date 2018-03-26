/************************************************************************/
/* 
Author:

lourking. .All rights reserved.

Create Time:

	4,22th,2014

Module Name:

	dsLocalPeroidProcess.h 

Abstract: 短周期过程，避免大量if和else 做同样的事情


*/
/************************************************************************/

#ifndef __DSLOCALPEROIDPROCESS_H__
#define __DSLOCALPEROIDPROCESS_H__

typedef void (CALLBACK *PROC_BEGINLPP) (LPVOID /*lpParam*/);
typedef void (CALLBACK *PROC_ENDLPP) (LPVOID /*lpParam*/);

class dsLocalPeroidProcess
{
private:
	
	LPVOID m_lpParam;
	PROC_BEGINLPP m_procBegin;
	PROC_ENDLPP m_procEnd;

public:
	dsLocalPeroidProcess(LPVOID lpParam, PROC_BEGINLPP procBegin, PROC_ENDLPP procEnd):m_lpParam(lpParam), m_procBegin(procBegin), m_procEnd(procEnd)
	{
		if(NULL != m_procBegin)
			m_procBegin(m_lpParam);
	}

	~dsLocalPeroidProcess()
	{
		if(NULL != m_procEnd)
			m_procEnd(m_lpParam);
	}

private:

	dsLocalPeroidProcess(){}

};
#endif /*__DSLOCALPEROIDPROCESS_H__*/