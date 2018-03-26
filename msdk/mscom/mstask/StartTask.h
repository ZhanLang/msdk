#pragma once
#include "mstask\imstask.h"
//#include "rscom_interface/irslanguage2.h"
#include "Check.h"

class CStartTask
{
public:
	CStartTask(void);
	~CStartTask(void);

	BOOL StartProgram(VOID* lpClass,ST_RSTASK * lpTask,UINT uType=0,DWORD dwSessionID=0);

	DWORD TaskProcess(LPVOID lpParameter);
	DWORD TaskProcessXP(LPVOID lpParameter);

	BOOL StartDllProgram(TCHAR* aszAppName,DWORD dwSpecific);
	BOOL StartProcess(TCHAR* lpCmdLine, TCHAR* lpPath);

private:
	BOOL ConvertCommandline(IN  TCHAR* szOldCmd,OUT  TCHAR* szCmdLine,OUT  TCHAR* szPath);
	//UTIL::com_ptr<IRsLanguage2> m_iLang;
	//CRSComLoader m_cldr;
	CCheck m_check;

	TCHAR m_szPath[MAX_PATH];
	BOOL m_bXpAbove;

};
