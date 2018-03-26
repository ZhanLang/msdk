#pragma once
#include "SafeArray.h"
#include "mstask\imstask.h"
#include "xmldefine.h"
#include "XMLite.h"
#include "StartTask.h"
#include "RandRun.h"

typedef CSafeArray<ST_RSTASK> START_TASKVECTOR;
//typedef std::vector<ST_RSTASK> AFTERBOOT_TASKLIST;

#define RSTASK_MUTEX _T("61044833-9CF3-4a35-AE4C-751875D12C39")
class CXMLMgr
{
public:
	CXMLMgr(void);
	~CXMLMgr(void);
	BOOL LoadXMLFile(LPCTSTR lpFileName, PTASK_XML pBuff);
	BOOL SaveXMLFile(LPCTSTR lpFileName, PTASK_XML pBuff);


	DWORD AddTask(TASK_XML* pBuff,ST_RSTASK* pTask);
	DWORD ModifyTask(TASK_XML* pBuff,ST_RSTASK* pTask);
	DWORD SetTask(TASK_XML* pBuff,DWORD dwTaskIndex,DWORD dwSet) ;
	DWORD SetAllTasks(TASK_XML* pBuff,DWORD dwSet);
    DWORD DeleteTask(TASK_XML* pBuff,DWORD dwTaskIndex) ;

	//for ravtask.exe
	BOOL LoadXML(LPCTSTR lpFileName, PTASK_XML pBuff,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML);
	void UnInit();
	void Init(CStartTask *pStart);

	CStartTask * GetStartClass();

private:
	DWORD ReadXML(LPCTSTR lpFileName,tstring& szBuffer);

    //for ravtask.exe
	BOOL xFillAll(LPXNode lpNode, PTASK_XML pBuff,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML);
	BOOL xFillTaskList(LPXNode lpNode, TASK_LIST& TaskList,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML);

	BOOL PreFormatXML(tstring &rssFile);

	//read
	BOOL xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPTSTR lpBuff, UINT uSize);
	BOOL xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT& uValue);
	LPCTSTR xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname);

	BOOL xFillProduct(LPXNode lpNode, PPRODUCT_ATTRIBUTE pBuff);
	BOOL xFillAll(LPXNode lpNode, PTASK_XML pBuff);
	BOOL xFillTaskList(LPXNode lpNode, TASK_LIST& TaskList);
	BOOL xFillTask(LPXNode lpNode, ST_RSTASK* pBuff);

	BOOL xFillTaskAttrib(LPXNode lpNode, TASKATTRIB* pAttrib);
	BOOL xFillTaskCMD(LPXNode lpNode, TASKCMD* pCMD);
	BOOL xFillTaskSpecific(LPXNode lpNode, TASKSPECIFIC* pSpecific);
	BOOL xFillTaskTime(LPXNode lpNode, ST_RSTASK* pTime);
	BOOL xFillTaskFinish(LPXNode lpNode, TASKFINISH* pFinish);

    //write
	BOOL xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPCTSTR lpBuff);
	BOOL xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT uValue);
	LPXNode xCreateChildNode(LPXNode lpNode, LPCTSTR szNodename, LPCTSTR lpBuff);

	BOOL xFillProduct(PPRODUCT_ATTRIBUTE pBuff, LPXNode lpParentNode);
	BOOL xFillAll(PTASK_XML pBuff, LPXNode lpParentNode);
	BOOL xFillTaskList(TASK_LIST& TaskList, LPXNode lpParentNode);
	BOOL xFillTask(ST_RSTASK* pBuff, LPXNode lpParentNode);

	BOOL xFillTaskAttrib( TASKATTRIB* pAttrib, LPXNode lpNode);
	BOOL xFillTaskCMD(TASKCMD* pCMD, LPXNode lpParentNode);
	BOOL xFillTaskSpecific(TASKSPECIFIC* pSpecific, LPXNode lpParentNode);
	BOOL xFillTaskTime(ST_RSTASK* pTime, LPXNode lpParentNode);
	BOOL xFillTaskFinish(TASKFINISH* pFinish, LPXNode lpParentNode);


private:
	BOOL m_bRunSmartup;
	CStartTask* m_pStart;
	CRandRun m_RandRun;

	HANDLE m_hSingle;
	//WORD m_wRandValue;
};


