// RsTask.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "RsTask.h"
#include "mstask\imstask.h"
#include "gtasks.h"
#include "taskcontrol.h"
#include "tasksvrwrap.h"

BEGIN_CLIDMAP
CLIDMAPENTRY_BEGIN
CLIDMAPENTRY_PROGID(CLSID_RSTask, CTaskControl, _T("IRSTask"))
CLIDMAPENTRY_PROGID(CLSID_RSTaskSrv, CGTasks, _T("IRSTaskSrv"))
CLIDMAPENTRY_PROGID(CLSID_RSTaskSvrWrap, CTaskSvrWrap, _T("ITaskSvrWrap"))
CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN


HMODULE  g_hinstance = NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hinstance = (HMODULE)hModule;
    return TRUE;
}



void RSLOG(DWORD dwLevel,const TCHAR *format, ... )
{
	va_list ap;
	TCHAR szTmp[1024];
	va_start(ap, format);
	_vstprintf_s(szTmp,1024, format, ap);
	va_end(ap);
	OutputDebugString(szTmp);
}

BOOL GetProductType(TCHAR *szProduct,DWORD dwMax){
    //取产品类型
    TCHAR szNetConfig[MAX_PATH] = {0};
    TCHAR szPath[MAX_PATH] = {0};
    // 获取安装目录
    ::GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pszP = _tcsrchr(szPath, TEXT('\\'));
    if (NULL == pszP )	
        return FALSE;
    *pszP = 0;

    _tcscpy(szNetConfig, szPath);
    _tcscat(szNetConfig, TEXT("\\netconfig.ini"));
     GetPrivateProfileString(TEXT("SETTING"), TEXT("INI"), TEXT("ris.ini"), szProduct, dwMax, szNetConfig);
    pszP = _tcsrchr(szProduct, TEXT('.'));
    if ( NULL == pszP )	
        return FALSE;
    *pszP = 0;
    return TRUE;
}

HANDLE CreateCfgEvent()
{
    TCHAR szEventName[MAX_PATH] = { 0 };
    SECURITY_ATTRIBUTES* lpSA = NULL;
    SECURITY_ATTRIBUTES sa;
    SECURITY_DESCRIPTOR     sDesc = {0};

    if(GetVersion() < 0x80000000 )
    {
        lstrcpy(szEventName, _T("Global\\"));
        lstrcat(szEventName, RSTASK_EVENT);

        if (!InitializeSecurityDescriptor(&sDesc, SECURITY_DESCRIPTOR_REVISION))
        {
            RSLOG(RSLOG_ALERT, _T("InitializeSecurityDescriptor FAILED, Err(%d)"), GetLastError());
            return NULL;
        }
        if (!SetSecurityDescriptorDacl(&sDesc, TRUE, (PACL) NULL, FALSE))	  
        {
            RSLOG(RSLOG_ALERT, _T(" SetSecurityDescriptorDacl FAILED : Err(%d)"), GetLastError());
            return NULL;
        }
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = &sDesc;
        sa.bInheritHandle = TRUE;
        lpSA = &sa;
    }
    else
    {
        lstrcpy(szEventName, RSTASK_EVENT);
    }

    TCHAR szProduct[64] = {0};	
    if(GetProductType(szProduct,64))
        lstrcat(szEventName, szProduct);

    HANDLE hEvent = CreateEvent(lpSA,FALSE, FALSE,szEventName );
    if( NULL == hEvent)
        RSLOG(RSLOG_ALERT,_T("创建Event对象失败"));
    return hEvent;
}
