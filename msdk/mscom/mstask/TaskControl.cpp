#include "StdAfx.h"
#include "taskcontrol.h"
#include <atltime.h>

CTaskControl::CTaskControl(void)
{
	GetXMLFile();
	CheckXmlFile(m_XMLMgr);
	InitEvent();
}

BOOL CTaskControl::InitEvent()
{
	m_hEvent = CreateCfgEvent();
	return TRUE;
}

CTaskControl::~CTaskControl(void)
{
	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}


void CTaskControl::PostModifyXmlMsg(TASKMSG *lpMsg)
{
	if(m_hEvent)
		SetEvent(m_hEvent);
}

BOOL CTaskControl::GetXMLFile()
{
	TCHAR szIniFile[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	GetTaskPath(szPath,MAX_PATH);

	lstrcpy(szIniFile,szPath);
	lstrcat(szIniFile,_T("\\rstask.ini"));

	GetPrivateProfileString(_T("location"),_T("rising"),_T(""),m_szRsTaskFile,sizeof(TCHAR)*MAX_PATH,szIniFile);
	if( !IsFileExist(m_szRsTaskFile))
	{
       lstrcpy(m_szRsTaskFile,szPath);
	   lstrcat(m_szRsTaskFile,_T("\\rstask.xml"));
	}
	GetPrivateProfileString(_T("location"),_T("user"),_T(""),m_szUserTaskFile,sizeof(TCHAR)*MAX_PATH,szIniFile);
	if( !IsFileExist(m_szUserTaskFile))
	{
		lstrcpy(m_szUserTaskFile,szPath);
		lstrcat(m_szUserTaskFile,_T("\\rstasku.xml"));
	}

	return TRUE;
}


HRESULT CTaskControl::SetConfigFile(const TCHAR * lpFileName)
{
	//if(lpFileName==NULL) return -1;
	//TCHAR szBuff[MAX_PATH];
	//ZeroMemory(szBuff,sizeof(szBuff));
	//GetTaskPath(szBuff,MAX_PATH);

	//lstrcpy(m_szRsTaskFile,szBuff );
	//lstrcat(m_szRsTaskFile,_T("\\rstask.xml"));

	//WritePrivateProfileString(_T("location"),_T("user"),lpFileName,szBuff);
	//WritePrivateProfileString(_T("location"),_T("rising"),m_szRsTaskFile,szBuff);

	//TASKMSG msg;
	//ZeroMemory(&msg,sizeof(msg));
	//msg.dwType = TASK_MSG_SETCONFIG;
	//PostModifyXmlMsg(&msg);

	return 0;
}

HRESULT CTaskControl::SetTask(DWORD dwTaskIndex,DWORD dwSet)
{
	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL) {
		return -1;
	}
	AutoObject(lpXML);
	GetXMLFile();

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
		return ERROR_LOAD_FILE;
	

	TASK_LIST*  pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uIndex == dwTaskIndex )
		{
			if(pTask->task_attrib.wUse != (WORD) dwSet)
			{
				pTask->task_attrib.wUse =(WORD) dwSet;
				if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,lpXML))
					return ERROR_SAVE_FILE;

				SendXmlMsg();
				return 0;
			}
			return 0;
		}
	}

	return ERROR_NOEXIST_TASK;
}

HRESULT CTaskControl::SetAllTasks(DWORD dwSet)
{
	GetXMLFile();
	//user xml
	//--------------------------------------------------------------------
	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL) {
		return -1;
	}
	AutoObject(lpXML);

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
	{
		return ERROR_LOAD_FILE;
	}
	TASK_LIST* pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		pTask->task_attrib.wUse =(WORD) dwSet;
	}

	if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,lpXML))
		return ERROR_SAVE_FILE;

	SendXmlMsg();
	return 0;
}


HRESULT CTaskControl::AddTask(ST_RSTASK * lpTask)
{
	BOOL bFind =FALSE;
	UINT uMaxIndex = 0;
	BOOL bIsExistFile = TRUE;

	if(lpTask == NULL){
		return -1;
	}
	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL) {
		return -1;
	}
	AutoObject(lpXML);
	GetXMLFile();

	uMaxIndex = START_TASK_INDEX_CUSTOM;
	if( !IsFileExist(m_szUserTaskFile))
	{
		lstrcpy(lpXML->product_attribute.szVersion,_T("1.0"));
		bIsExistFile = FALSE;
	}

	//开始时间不能为零
	if(lpTask->task_time.start_time.wYear==0||
		lpTask->task_time.start_time.wMonth==0||
		lpTask->task_time.start_time.wDay==0)
	{
		SYSTEMTIME stm;
		GetLocalTime(&stm);
		lpTask->task_time.start_time.wYear=stm.wYear;
		lpTask->task_time.start_time.wMonth=stm.wMonth;
		lpTask->task_time.start_time.wDay=stm.wDay;
	}

	TASK_LIST*  pList = &(lpXML->task_list);
	if(bIsExistFile)
	{
		if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
		{
			return ERROR_LOAD_FILE;
		}

		for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
		{
			ST_RSTASK* pTask = (*i);
			if(uMaxIndex <  pTask->task_attrib.uIndex)
				uMaxIndex = pTask->task_attrib.uIndex;
			if(pTask->task_attrib.uIndex == lpTask->task_attrib.uIndex )
			{
				memcpy(pTask,lpTask,sizeof(ST_RSTASK));
				bFind = TRUE;
				break;
			}
		}
	}

	if(! bFind)
	{  
		ST_RSTASK * pNewTask = new  ST_RSTASK;
		if(pNewTask) return -1;

		memcpy(pNewTask,lpTask,sizeof(ST_RSTASK));
		if( pNewTask->task_attrib.uIndex == 0)
		{
			//如果没给index号,将自动从START_TASK_INDEX_CUSTOM 开始给一个值
			pNewTask->task_attrib.uIndex = uMaxIndex+1;
			//将生成的index传回
			lpTask->task_attrib.uIndex =  pNewTask->task_attrib.uIndex;
		}
		pList->push_back(pNewTask);
		
	}

	if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,lpXML))
		return  ERROR_SAVE_FILE;

	SendXmlMsg();
	return 0;

}

HRESULT CTaskControl::GetTask(DWORD dwTaskIndex,ST_RSTASK & Task)
{
	TASK_XML* lpXML = new TASK_XML;
	AutoObject(lpXML);
	GetXMLFile();

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
	{
		return ERROR_LOAD_FILE;
	}

	TASK_LIST*  pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uIndex == dwTaskIndex && pTask->task_attrib.wDelete==0 )
		{
			memcpy(&Task,pTask,sizeof(ST_RSTASK));
			return ERROR_SUCCESS;
		}
	}
	return	ERROR_NOEXIST_TASK;
}

HRESULT CTaskControl::DeleteTask( DWORD dwTaskIndex)
{
	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL ||  dwTaskIndex==0) {
		return -1;
	}
	AutoObject(lpXML);
	GetXMLFile();
	BOOL bFind =FALSE;
	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
	{
		return -1;
	}
	TASK_LIST*  pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uIndex == dwTaskIndex && dwTaskIndex > START_TASK_INDEX_CUSTOM)
		{
			pList->erase(i);
			delete pTask;
			bFind = TRUE;
			break;
		}
		else if(pTask->task_attrib.uIndex == dwTaskIndex && dwTaskIndex <= START_TASK_INDEX_CUSTOM)
		{
			pTask->task_attrib.wUse = 0;
			pTask->task_attrib.wDelete = 1;
			bFind = TRUE;
			break;
		}
	}
	if(! bFind )
		return ERROR_NOEXIST_TASK;


	if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,lpXML))
		return ERROR_SAVE_FILE;

	SendXmlMsg();
	return 0;
}

void CTaskControl::SendXmlMsg()
{
	TASKMSG msg;
	ZeroMemory(&msg,sizeof(msg));
	msg.dwType = TASK_MSG_XMLMODIFY;
	PostModifyXmlMsg(&msg);
}


HRESULT CTaskControl::ModifyTask(ST_RSTASK * lpTask)
{
	if(lpTask == NULL || lpTask->task_attrib.uIndex==0 ){
		return -1;
	}
	TASK_XML* lpXML = new TASK_XML;
	AutoObject(lpXML);
	GetXMLFile();

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
	{
		return -1;
	}

	if(lpTask->task_time.start_time.wYear==0||
		lpTask->task_time.start_time.wMonth==0||
		lpTask->task_time.start_time.wDay==0)
	{
		SYSTEMTIME stm;
		GetLocalTime(&stm);
		lpTask->task_time.start_time.wYear=stm.wYear;
		lpTask->task_time.start_time.wMonth=stm.wMonth;
		lpTask->task_time.start_time.wDay=stm.wDay;
	}

	TASK_LIST*  pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uIndex == lpTask->task_attrib.uIndex )
		{
			memcpy(pTask,lpTask,sizeof(ST_RSTASK));
			if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,lpXML))
				return ERROR_SAVE_FILE;
			SendXmlMsg();
			return 0;
		}
	}

	return ERROR_NOEXIST_TASK;
}

HRESULT CTaskControl::ControlTask(DWORD dwTaskIndex,DWORD dwControlID)
{

	TASKMSG msg;
	ZeroMemory(&msg,sizeof(msg));
	msg.dwType = TASK_MSG_CONTROLTASK;
	msg.dwIndex = dwTaskIndex;
	msg.dwControl = dwControlID;
	PostModifyXmlMsg(&msg);		
	return 0;
}

HRESULT CTaskControl::ControlTasks(DWORD dwControlID)
{
	TASKMSG msg;
	ZeroMemory(&msg,sizeof(msg));
	msg.dwType = TASK_MSG_CONTROLALLTASK;
	msg.dwControl = dwControlID;
	PostModifyXmlMsg(&msg);	
	return 0;
}

HRESULT CTaskControl::QueryTaskTime(ST_RSTASK& Task)
{
	return GetNextStartTime(&Task);
}

HRESULT CTaskControl::PutArray(ST_RSTASK* pTaskArray,DWORD dwCount,ST_RSTASK* pItem,DWORD dwIndex)
{
	if(NULL == pTaskArray) 
		return S_OK;
	if(dwIndex > dwCount)
		return E_OUTOFMEMORY;
	GetNextStartTime(pItem);
	ST_RSTASK* pArray = pTaskArray+ (dwIndex-1);
	memcpy(pArray,pItem,sizeof(ST_RSTASK));
	return S_OK;
}

HRESULT CTaskControl::QueryTask(IN DWORD dwTaskClassID,IN OUT ST_RSTASK* pTaskArray,IN OUT DWORD dwCount)
{
	GetXMLFile();
	if( !IsFileExist(m_szUserTaskFile))
	{
		return ERROR_NOEXIST_FILE;
	}

	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL)
		return -1;
	AutoObject(lpXML);

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
	{
		return ERROR_LOAD_FILE;
	}

	DWORD dwMaxSize =0;
	TASK_LIST* pList = &(lpXML->task_list);
	for(TASK_LIST_ITER i = pList->begin(); i != pList->end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uClassID == dwTaskClassID && pTask->task_attrib.wDelete==0)
		{
			dwMaxSize ++;
			PutArray(pTaskArray, dwCount,pTask,dwMaxSize);
		}
		else if(dwTaskClassID==0 && pTask->task_attrib.wDelete==0){
			dwMaxSize ++;
			PutArray(pTaskArray,dwCount,pTask,dwMaxSize);
		}
	}
	dwCount = dwMaxSize;
	return 0;
}

HRESULT CTaskControl::QueryTaskStatus(DWORD dwTaskIndex,SYSTEMTIME*pPrevStartTime, SYSTEMTIME*pNextStartTime)
{
	TASK_XML* lpXML = new TASK_XML;
	if(lpXML==NULL) {
		return -1;
	}
	AutoObject(lpXML);
	GetXMLFile();

	if(!m_XMLMgr.LoadXMLFile(m_szUserTaskFile,lpXML))
		return ERROR_LOAD_FILE;

	for(TASK_LIST_ITER i = lpXML->task_list.begin(); i != lpXML->task_list.end(); i ++)
	{
		ST_RSTASK* pTask = (*i);
		if(pTask->task_attrib.uIndex == dwTaskIndex )
		{
			memcpy(pPrevStartTime,&pTask->task_time.last_time,sizeof(SYSTEMTIME));
			GetNextStartTime(pTask,pNextStartTime);
			return 0;
		}
	}
	return ERROR_NOEXIST_TASK;		
}

//保留给以后升级用
HRESULT CTaskControl::UpdateXMLCfg()
{
	return 0;
}

