#include "StdAfx.h"
#include "xmlmgr.h"
#include "StartTask.h"
#include <time.h>

CXMLMgr::CXMLMgr(void)
{
	m_bRunSmartup= FALSE;
	//m_wRandValue = 0;
	srand( (unsigned)time( NULL ) );
	//m_wRandValue = rand() % 60;
	m_pStart = NULL;

	DWORD dwOSVersion = (GetVersion() & 0xFF);
	TCHAR szMutexName[MAX_PATH] = { 0 };
	if(dwOSVersion == 5)
	{
		lstrcpy(szMutexName, _T("Global\\"));
		lstrcat(szMutexName, RSTASK_MUTEX);
	}
	else
	{
		lstrcpy(szMutexName, RSTASK_MUTEX);
	}
    TCHAR szProduct[64] = { 0 };
    if(GetProductType(szProduct,64))
        lstrcat(szMutexName, szProduct);

	m_hSingle = CreateMutex(NULL, FALSE,szMutexName );
}

CXMLMgr::~CXMLMgr(void)
{
	if(m_hSingle)
	{
		CloseHandle(m_hSingle);
		m_hSingle  = NULL;
	}
}

CStartTask * CXMLMgr::GetStartClass()
{
	return m_pStart;
}

void CXMLMgr::UnInit()
{
	if(m_bRunSmartup)
	{
		m_RandRun.UnInit();
		m_bRunSmartup= FALSE;
	}
}
void CXMLMgr::Init(CStartTask *pStart)
{
	m_pStart = pStart;
}
//-----------------------------------------------------------------------------------------------
DWORD CXMLMgr::ReadXML(LPCTSTR lpFileName,tstring& rssFile)
{
	WaitForSingleObject( m_hSingle, INFINITE );

	FILE *pFile =NULL;
	_tfopen_s(&pFile,lpFileName, TEXT("rb"));
	if(NULL == pFile)
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to open file %s.\n"), lpFileName);
		ReleaseMutex(m_hSingle);
		return FALSE;
	}
	fseek(pFile, 0, SEEK_END);
	int nFileLen = ftell(pFile);

	tstring  rsRead;
	rsRead.resize(nFileLen + 1);
#if _MSC_VER > 1300
	LPTSTR lpChr = (LPTSTR)rsRead.c_str();
#else
	LPSTR lpChr = rsRead.begin();
#endif

	fseek(pFile, 0, SEEK_SET);
	fread(lpChr, 1, nFileLen, pFile);
	fclose(pFile);
	rsRead.resize(_tcslen(lpChr));


	CHAR* data = (CHAR*)lpChr ;
	while( *data )
	{
		if( *data == '<' )
			break;
		data++;
	}

#ifdef _UNICODE
	//int len = MultiByteToWideChar( CP_UTF8, 0, data, -1, NULL, 0 );
	//rssFile.resize( (len + 1) * sizeof(TCHAR));
	//LPWSTR szW = (LPTSTR)rssFile.c_str();
	//MultiByteToWideChar( CP_UTF8, 0, data, -1, szW, len );
	rssFile = rsRead;

#else

	rssFile.resize( (nFileLen + 1) * sizeof(TCHAR));
	rssFile = rsRead;
#endif

	ReleaseMutex(m_hSingle);
	return TRUE;
}

BOOL CXMLMgr::LoadXML(LPCTSTR lpFileName, PTASK_XML pBuff,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML)
{
	if(pBuff == NULL ||  lpFileName== NULL)
		return FALSE;

	tstring rssFile;
	ReadXML(lpFileName,rssFile);


	XNode xRsXML;
	PreFormatXML(rssFile);
	if(!xRsXML.Load(rssFile.c_str()))
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to parse  XML file %s.\n"), lpFileName);
		return FALSE;
	}

	if(!xFillAll(&xRsXML, pBuff,Task,AfterList,nType,nUserXML))
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to fill files struct %s."), lpFileName);
		xRsXML.Close();
		return FALSE;
	}
	xRsXML.Close();
	return TRUE;

}

BOOL CXMLMgr::xFillAll(LPXNode lpNode, PTASK_XML pBuff,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), ROOT_PRODUCT) || !pBuff)
		return FALSE;
	//ZeroMemory(&pBuff->product_attribute, sizeof(pBuff->product_attribute));
	xFillProduct(lpNode, &pBuff->product_attribute);
	xFillTaskList(lpNode, pBuff->task_list,Task,AfterList,nType,nUserXML);

	return TRUE;
}

BOOL CXMLMgr::xFillTaskList(LPXNode lpNode, TASK_LIST& TaskList,START_TASKVECTOR& Task,START_TASKVECTOR& AfterList,int nType,int nUserXML)
{
	if(_tcsicmp(lpNode->name.c_str(), ROOT_PRODUCT) )
		return FALSE;

	XNodes ItemNodes = lpNode->GetChilds(ROOT_TASK);
	if(!ItemNodes.size()) 
		return FALSE;

	START_TASKVECTOR::SAFEARRAYVECTOR TempVector;
	START_TASKVECTOR::SAFEARRAYVECTOR TempVector2;
	m_bRunSmartup = FALSE;
	
	for(UINT n = 0; n < ItemNodes.size(); n ++)
	{
		LPST_RSTASK pNewTask = new ST_RSTASK;
		if(pNewTask==NULL) continue;
		memset(pNewTask, 0,sizeof(ST_RSTASK));
		if(!xFillTask(ItemNodes[n], pNewTask))
		{
			//RSLOG(RSXML_WAINNING, _T("CXMLMgr -- Fail to read XML module index:%s .\n"), ItemNodes[n]->GetAttr(_T("index"))->value.c_str() );
		}
		else
		{
			if( pNewTask->task_attrib.uType == START_TASK_TYPE_AFTERBOOT && nType==0)
			{
				TempVector2.push_back(*pNewTask);
			}
			else if(m_pStart && pNewTask->task_attrib.uType == START_TASK_TYPE_WITHBOOT && pNewTask->task_attrib.wUse && nType==0)
			{
				m_pStart->StartProgram(NULL,pNewTask);
			}
			else if(pNewTask->task_attrib.uType > 2 ) {

				if( pNewTask->task_specific.wSpecificMethod == 2)
				{
					pNewTask->task_specific.wFloatSecond =   rand() % 60;
					RSLOG(RSXML_FATAL, _T("上报随机时间 %d 分钟.\n"), pNewTask->task_specific.wFloatSecond);
				}

				if(pNewTask->task_attrib.uType == START_TASK_TYPE_INSTANTUPDATE )
				{
					if( pNewTask->task_attrib.wUse==1)
					{
						m_RandRun.SetSmartupCmd(pNewTask->task_cmd.cmd, TRUE);
						m_bRunSmartup = TRUE;
					}
				}				
				else
				   TempVector.push_back(*pNewTask);
			}
		}
		//放到pBuff里,释放修改pNewTask在里操作
		TaskList.push_back(pNewTask);
	}

	if(m_bRunSmartup)
	{
		m_RandRun.Init(m_pStart);
	}

	if(TempVector.size() >0)
	{
		if (nUserXML)
		{ 
			Task.Insert(TempVector);
		}
		else
		{
			Task.DeleteAllAndInsert(TempVector);
		}
	}

	if (TempVector2.size() >0)
	{
		if (nUserXML)
		{ 
			AfterList.Insert(TempVector2);
		}
		else
		{
			AfterList.DeleteAllAndInsert(TempVector2);
		}
	}

	return TRUE;	
}

//----------------------------------------------------------------------------------------

BOOL CXMLMgr::LoadXMLFile(LPCTSTR lpFileName, PTASK_XML pBuff)
{
	if(lpFileName==NULL)
		return FALSE;
	tstring rssFile;
	ReadXML(lpFileName,rssFile);

	XNode xRsXML;
	PreFormatXML(rssFile);

	if(!xRsXML.Load(rssFile.c_str()))
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to parse  XML file %s.\n"), lpFileName);
		return FALSE;
	}

	if(!xFillAll(&xRsXML, pBuff))
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to fill files struct %s.\n"), lpFileName);
		xRsXML.Close();
		return FALSE;
	}
	xRsXML.Close();
	return TRUE;
}

BOOL CXMLMgr::SaveXMLFile( LPCTSTR lpFileName, PTASK_XML pBuff)
{
	XNode xRsXML;
	if(lpFileName==NULL || pBuff== NULL)
		return FALSE;

	WaitForSingleObject( m_hSingle, INFINITE );

	if(!xFillAll(pBuff, &xRsXML))
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to fill file struct to files %s.\n"), lpFileName);
		ReleaseMutex(m_hSingle);
		return FALSE;
	}
	tstring rssFile = xRsXML.GetXML();

	FILE *pFile = NULL;
	_tfopen_s(&pFile,lpFileName, TEXT("wb"));
	if(NULL == pFile)
	{
		RSLOG(RSXML_FATAL, _T("CXMLMgr -- Fail to save xml files %s.\n"), lpFileName);
		ReleaseMutex(m_hSingle);
		return FALSE;
	}
	fseek(pFile, 0, SEEK_SET);

#define XML_TITLE _T("<?xml version=\"1.0\" ?>")

#ifdef _UNICODE
	BYTE mark[2] = { 0xFF, 0xFE };
	fwrite(mark, 2,1, pFile);
#endif
	fwrite(XML_TITLE, _tcslen(XML_TITLE)*sizeof(TCHAR),1, pFile);

	fwrite(rssFile.c_str(),rssFile.size()*sizeof(TCHAR),1, pFile);
	fclose(pFile);
	ReleaseMutex(m_hSingle);

	return TRUE;
}

BOOL CXMLMgr::xFillAll(PTASK_XML pBuff, LPXNode lpParentNode)
{
	lpParentNode->name = ROOT_PRODUCT;

	xFillProduct(&pBuff->product_attribute, lpParentNode);
	xFillTaskList(pBuff->task_list, lpParentNode);

	return TRUE;
}

BOOL CXMLMgr::xFillAll(LPXNode lpNode, PTASK_XML pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), ROOT_PRODUCT) || !pBuff)
		return FALSE;
	//ZeroMemory(&pBuff->product_attribute, sizeof(pBuff->product_attribute));
	TASK_LIST_ITER it =  pBuff->task_list.begin();

	xFillProduct(lpNode, &(pBuff->product_attribute));
	TASK_LIST_ITER it2 =  pBuff->task_list.begin();

	xFillTaskList(lpNode, pBuff->task_list);

	return TRUE;
}

BOOL CXMLMgr::xFillTaskList(LPXNode lpNode, TASK_LIST& TaskList)
{
	if(_tcsicmp(lpNode->name.c_str(), ROOT_PRODUCT) ) return FALSE;

	XNodes ItemNodes = lpNode->GetChilds(ROOT_TASK);
	if(!ItemNodes.size()) return FALSE;

	TASK_LIST_ITER it0 = TaskList.begin();
	for(UINT n = 0; n < ItemNodes.size(); n ++)
	{
		LPST_RSTASK pNewTask = new ST_RSTASK;
		if(pNewTask == NULL) continue; 
		ZeroMemory(pNewTask, sizeof(ST_RSTASK));
		if(!xFillTask(ItemNodes[n], pNewTask))
		{
			RSLOG(RSXML_WAINNING, _T("CXMLMgr -- Fail to read XML module infomation name .\n"));
			TaskList.push_back(pNewTask);
			return FALSE;
		}
		TaskList.insert( TaskList.end(),pNewTask);
	}
	return TRUE;
}

BOOL CXMLMgr::xFillTaskList(TASK_LIST& TaskList, LPXNode lpParentNode)
{
	if(!TaskList.size()) return TRUE;
	LPXNode lpNode = NULL;
	for(TASK_LIST_ITER i = TaskList.begin(); i != TaskList.end(); i ++)
	{
		lpNode = xCreateChildNode(lpParentNode, ROOT_TASK, NULL);
		if(!lpNode) return FALSE;

		if(!xFillTask( (*i) ,lpNode))
		{

		}
	}
	return TRUE;

}

BOOL CXMLMgr::PreFormatXML(tstring &rssFile)
{
	LPCTSTR lpFind = rssFile.c_str();
	LPCTSTR lpFindEnd = rssFile.c_str();

	while(lpFind)
	{
		BOOL bFind = FALSE;
		while(!bFind)
		{
			lpFind = _tcschr(lpFind, _T('<'));
			if(!lpFind) 
			{
				bFind = TRUE;
				break;
			}

			if(*(lpFind + 1) == _T('?'))
				bFind = TRUE;
			else if(*(lpFind + 1) == _T('!'))
				bFind = TRUE;
			else 
				lpFind ++;
		}

		if(lpFind && *lpFind == _T('<'))
		{
			lpFindEnd =  _tcschr(lpFind, _T('>'));
			if(!lpFindEnd) return FALSE;

			rssFile.erase(lpFind - rssFile.c_str(), lpFindEnd - lpFind + 1);
			// rssFile.Delete(lpFind - (LPCTSTR)rssFile, lpFindEnd - lpFind + 1);
		}
	}
	return TRUE;
}

BOOL CXMLMgr::xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPTSTR lpBuff, UINT uSize)
{
	ZeroMemory(lpBuff, uSize);
	LPXAttr lpAttTemp = lpNode->GetAttr(attrname);
	if(!lpAttTemp || _tcsicmp(lpAttTemp->name.c_str(), attrname))
		return FALSE;

	_tcsnccpy_s(lpBuff,uSize/sizeof(TCHAR),lpAttTemp->value.c_str(),lpAttTemp->value.size());
	return TRUE;
}

LPCTSTR CXMLMgr::xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname)
{
	LPXAttr lpAttTemp = lpNode->GetAttr(attrname);
	if(!lpAttTemp || _tcsicmp(lpAttTemp->name.c_str(), attrname))
		return NULL;
	return lpAttTemp->value.c_str();
}


BOOL CXMLMgr::xGetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT& uValue)
{
	uValue = 0;
	LPXAttr lpAttTemp = lpNode->GetAttr(attrname);
	if(!lpAttTemp || _tcsicmp(lpAttTemp->name.c_str(), attrname))
		return FALSE;

	uValue =  _ttoi(lpAttTemp->value.c_str());
	return TRUE;
}

BOOL CXMLMgr::xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, LPCTSTR lpBuff)
{
	LPXAttr lpAttTemp = lpNode->GetAttr(attrname);
	if(!lpAttTemp || _tcsicmp(lpAttTemp->name.c_str(), attrname))
	{// 没有此属性 添加
		if(!lpBuff || !*lpBuff) return TRUE;
		lpAttTemp = lpNode->CreateAttr(attrname, lpBuff);
		if(lpAttTemp)
		{
			lpNode->AppendAttr(lpAttTemp);
			return TRUE;
		}

		RSLOG(RSXML_WAINNING, _T("CXMLMgr -- Fail to add XML attributes %s = %s."), attrname, lpBuff);
		return FALSE;
	}
	if(lpBuff && *lpBuff)
		lpAttTemp->value = lpBuff;
	else
		lpNode->RemoveAttr(lpAttTemp);
	return TRUE;
}

BOOL CXMLMgr::xSetNodeAttr(LPXNode lpNode, LPCTSTR attrname, UINT uValue)
{
	// ssTemp.Format("%d", uValue);
	TCHAR ss[100] = {0};
	_itot_s(uValue, ss, 10);
	tstring ssTemp = ss;

	LPXAttr lpAttTemp = lpNode->GetAttr(attrname);
	if(uValue == 0)
	{
		if(lpAttTemp)
			return lpNode->RemoveAttr(lpAttTemp);
		return TRUE;
	}

	return xSetNodeAttr(lpNode, attrname, ssTemp.c_str());
}

LPXNode CXMLMgr::xCreateChildNode(LPXNode lpNode, LPCTSTR szNodename, LPCTSTR lpBuff)
{
	LPXNode lpNodeTemp = lpNode->GetChild(szNodename);
	BOOL bMultyNode = FALSE;
	if(0 == _tcscmp(szNodename, ROOT_TASK)) bMultyNode = TRUE;

	if(bMultyNode || !lpNodeTemp || _tcsicmp(lpNodeTemp->name.c_str(), szNodename))
	{// 没有此节点 添加
		lpNodeTemp = lpNode->CreateNode(szNodename, lpBuff);
		if(lpNodeTemp)
		{
			return lpNode->AppendChild(lpNodeTemp);
		}

		RSLOG(RSXML_WAINNING, _T("CXMLMgr -- Fail to add XML node %s = %s.\n"), szNodename, lpBuff);
		return NULL;
	}

	if(lpBuff)
		lpNodeTemp->value = lpBuff;
	return lpNodeTemp;
}

BOOL CXMLMgr::xFillProduct(PPRODUCT_ATTRIBUTE pBuff, LPXNode lpParentNode)
{
	lpParentNode->name = ROOT_PRODUCT;
	xSetNodeAttr(lpParentNode, ATTRIB_VERSION,	pBuff->szVersion);

	return TRUE;
}

BOOL CXMLMgr::xFillProduct(LPXNode lpNode, PPRODUCT_ATTRIBUTE pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), ROOT_PRODUCT)) return FALSE;

	ZeroMemory(pBuff, sizeof(PRODUCT_ATTRIBUTE));
	xGetNodeAttr(lpNode, ATTRIB_VERSION, pBuff->szVersion, sizeof(pBuff->szVersion));

	return TRUE;
}

BOOL CXMLMgr::xFillTask(LPXNode lpNode, ST_RSTASK* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), ROOT_TASK ) || !pBuff) return FALSE;

	xFillTaskAttrib(lpNode, &pBuff->task_attrib);
	xFillTaskCMD(lpNode->GetChild(NODE_COMMANDLINE), &pBuff->task_cmd);
	xFillTaskSpecific(lpNode->GetChild(NODE_SPECIFIC), &pBuff->task_specific);
	xFillTaskTime(lpNode->GetChild(NODE_TIME), pBuff);
	xFillTaskFinish(lpNode->GetChild(NODE_FINISH), &pBuff->task_finish);
	return TRUE;
}

BOOL CXMLMgr::xFillTask(ST_RSTASK* pBuff, LPXNode lpParentNode)
{
	if(_tcsicmp(lpParentNode->name.c_str(), ROOT_TASK))
		lpParentNode->name = ROOT_TASK;

	xFillTaskAttrib(&pBuff->task_attrib, lpParentNode);
	//
	xFillTaskCMD(&pBuff->task_cmd, lpParentNode);
	xFillTaskSpecific(&pBuff->task_specific, lpParentNode);
	xFillTaskTime(pBuff, lpParentNode);
	xFillTaskFinish(&pBuff->task_finish, lpParentNode);
	return TRUE;
}
//r
BOOL CXMLMgr::xFillTaskAttrib(LPXNode lpNode, TASKATTRIB* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), ROOT_TASK)  || !pBuff) 
		return FALSE;
	xGetNodeAttr(lpNode, ATTRIB_INDEX, pBuff->uIndex);
	xGetNodeAttr(lpNode, ATTRIB_TYPE,  pBuff->uType);
	UINT nTmp=0;
	xGetNodeAttr(lpNode, ATTRIB_USE, nTmp);
	pBuff->wUse = (WORD)nTmp;

	xGetNodeAttr(lpNode, ATTRIB_POWER, nTmp);
	pBuff->wPower = (WORD)nTmp;
	
	xGetNodeAttr(lpNode, ATTRIB_DELETE, nTmp);
	pBuff->wDelete = (WORD)nTmp;
	
	xGetNodeAttr(lpNode, ATTRIB_LOCK, nTmp);
	pBuff->wLock = (WORD)nTmp;
	xGetNodeAttr(lpNode, ATTRIB_CLASS, nTmp);
	pBuff->uClassID = (WORD)nTmp;
	xGetNodeAttr(lpNode, ATTRIB_ISRISING, pBuff->uIsRising);
	xGetNodeAttr(lpNode, ATTRIB_TASKNAME,pBuff->szTaskName,sizeof(TCHAR)*NAME_LEN);
	xGetNodeAttr(lpNode, ATTRIB_TASKDESC,pBuff->szTaskDesc,sizeof(TCHAR)*MAX_PATH);

	return TRUE;
}
BOOL CXMLMgr::xFillTaskCMD(LPXNode lpNode, TASKCMD* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), NODE_COMMANDLINE)  || !pBuff) return FALSE;
	 xGetNodeAttr(lpNode, ATTRIB_CMD,pBuff->cmd,sizeof(TCHAR)*MAX_PATH );
	 xGetNodeAttr(lpNode, ATTRIB_END,pBuff->end,sizeof(TCHAR)*MAX_PATH );

	return TRUE;
}

                        
BOOL CXMLMgr::xFillTaskSpecific(LPXNode lpNode, TASKSPECIFIC* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), NODE_SPECIFIC)  || !pBuff) return FALSE;
	UINT uTmp=0;
	xGetNodeAttr(lpNode, ATTRIB_ACTION,uTmp );
	pBuff->wFindAction = (WORD)uTmp;
	xGetNodeAttr(lpNode, ATTRIB_RIGHT, uTmp );
	pBuff->wRight = (WORD)uTmp;
	xGetNodeAttr(lpNode, ATTRIB_SECOND, uTmp);
	pBuff->wFloatSecond = (WORD)uTmp;
	xGetNodeAttr(lpNode, ATTRIB_METHOD, uTmp);
	pBuff->wSpecificMethod = (WORD)uTmp;

	return TRUE;
}

static  BOOL  ConvertToDate(IN LPCTSTR lpBuff, OUT ST_DATE* pDate)
{
	if(lpBuff==NULL || pDate ==NULL  ) 
		return FALSE;

	int nLen = _tcslen(lpBuff);
	if( 7> nLen || nLen >10 ) 
		return FALSE;

	DWORD dwYear=0;
	DWORD dwMonth=0;
	DWORD dwDay=0;
	TCHAR szFormat[] = _T("%04d-%02d-%02d");
	_sntscanf_s(lpBuff, 10, szFormat, &(dwYear),&(dwMonth),&(dwDay));
	pDate->wYear = (WORD)dwYear;
	pDate->wMonth = (WORD)dwMonth;
	pDate->wDay = (WORD)dwDay;
	return TRUE;
}

BOOL CXMLMgr::xFillTaskTime(LPXNode lpNode, ST_RSTASK* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), NODE_TIME)  || !pBuff)
		return FALSE;
	TASKTIME* pTaskTime = &(pBuff->task_time);
	TCHAR szTmp[60];
	memset(szTmp,0,60*sizeof(TCHAR));

	if(	xGetNodeAttr(lpNode, ATTRIB_STARTDATE,szTmp,sizeof(szTmp) ))
	{
		ConvertToDate(szTmp, &(pTaskTime->start_time) );
	}
	else
	{
		SYSTEMTIME nowtime;
		GetLocalTime(&nowtime);
		pTaskTime->start_time.wYear = nowtime.wYear;
		pTaskTime->start_time.wMonth = nowtime.wMonth;
		pTaskTime->start_time.wDay = nowtime.wDay;
	}
	
	memset(szTmp,0,60*sizeof(TCHAR));
	if(xGetNodeAttr(lpNode, ATTRIB_STOPDATE,szTmp,sizeof(szTmp) ))
		ConvertToDate(szTmp, &(pTaskTime->stop_time) );
	
	memset(szTmp,0,60*sizeof(TCHAR));
	if(xGetNodeAttr(lpNode, ATTRIB_LASTTIME,szTmp,sizeof(szTmp) ))
		ConvertToSystemtime(szTmp, &(pTaskTime->last_time) );

	UINT uTmp=0;	
	switch(pBuff->task_attrib.uType)
	{
	case START_TASK_TYPE_AFTERBOOT:
		{

			LPXNode lpChildNode = lpNode->GetChild(NODE_AFTERBOOT);
			if(lpChildNode==NULL) return FALSE; 

			if(xGetNodeAttr(lpChildNode, ATTRIB_HOURS, uTmp))
				pTaskTime->every.after_boot.wHours =(WORD) uTmp;

			if(xGetNodeAttr(lpChildNode, ATTRIB_MINUTES, uTmp))
				pTaskTime->every.after_boot.wMinutes =(WORD) uTmp;

			if(xGetNodeAttr(lpChildNode, ATTRIB_SECONDS,uTmp ))
				pTaskTime->every.after_boot.wSeconds =(WORD) uTmp;

			break;
		}
	case START_TASK_TYPE_WITHBOOT:
		{
			break;
		}
	case START_TASK_TYPE_RUNONESHOT:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_RUNONESHOT);
			if(lpChildNode==NULL) return FALSE; 

			memset(szTmp,0,60*sizeof(TCHAR));
			if(xGetNodeAttr(lpChildNode, ATTRIB_DATE,szTmp,sizeof(szTmp) ))
			   ConvertToDate(szTmp, &(pTaskTime->every.oneshot_time.date) );

			if(xGetNodeAttr(lpChildNode, ATTRIB_HOUR, uTmp))
			   pTaskTime->every.oneshot_time.wHour =(WORD) uTmp;

			if(xGetNodeAttr(lpChildNode, ATTRIB_MINUTE, uTmp))
				pTaskTime->every.oneshot_time.wMinute =(WORD) uTmp;

			break;
		}
	case START_TASK_TYPE_WEEK:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_EVERYWEEK);
			if(lpChildNode==NULL) return FALSE; 
			xGetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_week.uInterval_Week);
			xGetNodeAttr(lpChildNode, ATTRIB_HOUR, uTmp);
			pTaskTime->every.every_week.wHour = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_MINUTE, uTmp);
			pTaskTime->every.every_week.wMinute = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_WEEKMARK, uTmp);
			pTaskTime->every.every_week.wWeekMark = uTmp;
			break;
		}
	case START_TASK_TYPE_MONTH:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_EVERYMONTH);
			if(lpChildNode==NULL) return FALSE; 
			xGetNodeAttr(lpChildNode, ATTRIB_MONTHMARK, uTmp);
			pTaskTime->every.every_month.wMonthMark = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_HOUR, uTmp);
			pTaskTime->every.every_month.wHour = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_MINUTE, uTmp);
			pTaskTime->every.every_month.wMinute = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_DAY, uTmp);
			pTaskTime->every.every_month.wDay = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_WEEKNUM, uTmp);
			pTaskTime->every.every_month.wWeekNum = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_DAYOFWEEK, uTmp);
			pTaskTime->every.every_month.wDayOfWeek = uTmp;

			xGetNodeAttr(lpChildNode, ATTRIB_DAYMARK, uTmp);
			pTaskTime->every.every_month.wDayMark = uTmp;

			break;
		}
	case START_TASK_TYPE_DAY:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_EVERYDAY);
			if(lpChildNode==NULL) return FALSE; 
			xGetNodeAttr(lpChildNode, ATTRIB_HOUR, uTmp);
			pTaskTime->every.every_day.wHour = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_MINUTE, uTmp);
			pTaskTime->every.every_day.wMinute = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_day.uInterval_Day);
			break;
		}
	case START_TASK_TYPE_HOUR:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_EVERYHOUR);
			if(lpChildNode==NULL) return FALSE; 
			xGetNodeAttr(lpChildNode, ATTRIB_MINUTE, uTmp);
			pTaskTime->every.every_hour.wMinute = uTmp;
			xGetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_hour.uInterval_Hour);
			break;

		}
	case START_TASK_TYPE_INTERVAL_HOUR:
		{
			LPXNode lpChildNode = lpNode->GetChild(NODE_INTERVALHOURS);
			if(lpChildNode==NULL) return FALSE; 
			xGetNodeAttr(lpChildNode, ATTRIB_HOURS, pTaskTime->every.interval_hour);
			break;
		}
	case START_TASK_TYPE_MESSEAGE:
		{
			break;
		}

	}

	return TRUE;
}

BOOL CXMLMgr::xFillTaskFinish(LPXNode lpNode, TASKFINISH* pBuff)
{
	if(!lpNode || _tcsicmp(lpNode->name.c_str(), NODE_FINISH)  || !pBuff) return FALSE;
	UINT uTmp=0;
	xGetNodeAttr(lpNode, ATTRIB_DELETE,uTmp );
	pBuff->wFinishDel = (WORD)uTmp;
	xGetNodeAttr(lpNode, ATTRIB_HOURS, uTmp );
	pBuff->wHours = (WORD)uTmp;
	xGetNodeAttr(lpNode, ATTRIB_MINUTES, uTmp);
	pBuff->wMintues = (WORD)uTmp;

	return TRUE;
}

//w
BOOL CXMLMgr::xFillTaskAttrib( TASKATTRIB* pBuff, LPXNode lpNode)
{
	if(pBuff==NULL || lpNode== NULL) return FALSE;

	xSetNodeAttr(lpNode, ATTRIB_INDEX, pBuff->uIndex);
	xSetNodeAttr(lpNode, ATTRIB_TYPE,  pBuff->uType);
	xSetNodeAttr(lpNode, ATTRIB_USE,	pBuff->wUse);
	xSetNodeAttr(lpNode, ATTRIB_POWER,	pBuff->wPower);
	xSetNodeAttr(lpNode, ATTRIB_DELETE, pBuff->wDelete);
	xSetNodeAttr(lpNode, ATTRIB_LOCK, pBuff->wLock);
	xSetNodeAttr(lpNode, ATTRIB_CLASS, pBuff->uClassID);
	xSetNodeAttr(lpNode, ATTRIB_ISRISING, pBuff->uIsRising);
	xSetNodeAttr(lpNode, ATTRIB_TASKNAME, pBuff->szTaskName);
	xSetNodeAttr(lpNode, ATTRIB_TASKDESC, pBuff->szTaskDesc);

	return TRUE;
}

BOOL CXMLMgr::xFillTaskCMD(TASKCMD* pBuff, LPXNode lpParentNode)
{
	if(pBuff==NULL || lpParentNode== NULL) return FALSE;
	LPXNode lpNode = xCreateChildNode(lpParentNode, NODE_COMMANDLINE,NULL );
	if(lpNode == NULL) return FALSE;
	xSetNodeAttr(lpNode, ATTRIB_CMD, pBuff->cmd);
	xSetNodeAttr(lpNode, ATTRIB_END, pBuff->end);

	return TRUE;
}

BOOL CXMLMgr::xFillTaskSpecific(TASKSPECIFIC* pBuff, LPXNode lpParentNode)
{
	if(pBuff==NULL || lpParentNode== NULL) return FALSE;
	LPXNode lpNode = xCreateChildNode(lpParentNode, NODE_SPECIFIC, NULL);
	if(lpNode ==NULL) return FALSE;
	xSetNodeAttr(lpNode, ATTRIB_ACTION,pBuff->wFindAction );
	xSetNodeAttr(lpNode, ATTRIB_RIGHT, pBuff->wRight );
	xSetNodeAttr(lpNode, ATTRIB_SECOND, pBuff->wFloatSecond);
	xSetNodeAttr(lpNode, ATTRIB_METHOD, pBuff->wSpecificMethod);
	return TRUE;
}

BOOL CXMLMgr::xFillTaskTime(ST_RSTASK* pBuff, LPXNode lpParentNode)
{
	if(pBuff==NULL || lpParentNode== NULL) return FALSE;
	LPXNode lpNode = xCreateChildNode(lpParentNode, NODE_TIME, NULL);
	if(lpNode==NULL) return FALSE;

	TASKTIME* pTaskTime = &(pBuff->task_time);
	TCHAR szTmp[100];
	if(pTaskTime->start_time.wYear && pTaskTime->start_time.wMonth && pTaskTime->start_time.wDay)
	{	
		_stprintf_s(szTmp,100,_T("%d-%02d-%02d"),pTaskTime->start_time.wYear, 
		        pTaskTime->start_time.wMonth ,pTaskTime->start_time.wDay );
		xSetNodeAttr(lpNode, ATTRIB_STARTDATE,szTmp );
	}

	if(pTaskTime->stop_time.wYear && pTaskTime->stop_time.wMonth && pTaskTime->stop_time.wDay)
	{
		_stprintf_s(szTmp,100,_T("%d-%02d-%02d"),pTaskTime->stop_time.wYear, 
		pTaskTime->stop_time.wMonth ,pTaskTime->stop_time.wDay );
		xSetNodeAttr(lpNode, ATTRIB_STOPDATE,szTmp);
	}

	if(pTaskTime->last_time.wYear && pTaskTime->last_time.wMonth && pTaskTime->last_time.wDay)
	{
		_stprintf_s(szTmp,100,_T("%d-%02d-%02d %02d:%02d:%02d"),pTaskTime->last_time.wYear, 
			pTaskTime->last_time.wMonth ,pTaskTime->last_time.wDay,
			pTaskTime->last_time.wHour ,pTaskTime->last_time.wMinute,
			pTaskTime->last_time.wSecond
			);
		xSetNodeAttr(lpNode, ATTRIB_LASTTIME,szTmp);
	}

	UINT uTmp=0;	
	switch(pBuff->task_attrib.uType)
	{
	case START_TASK_TYPE_AFTERBOOT:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_AFTERBOOT,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_HOURS, pTaskTime->every.after_boot.wHours);
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTES, pTaskTime->every.after_boot.wMinutes);
			xSetNodeAttr(lpChildNode, ATTRIB_SECONDS,pTaskTime->every.after_boot.wSeconds);
			break;
		}
	case START_TASK_TYPE_WITHBOOT:
		{
			break;
		}
	case START_TASK_TYPE_RUNONESHOT:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_RUNONESHOT,NULL);
			if(lpChildNode==NULL) return FALSE; 
			memset(szTmp,0,60*sizeof(TCHAR));

			_stprintf_s(szTmp,_T("%d-%02d-%02d"),pTaskTime->every.oneshot_time.date.wYear, 
				pTaskTime->every.oneshot_time.date.wMonth ,pTaskTime->every.oneshot_time.date.wDay );

			xSetNodeAttr(lpChildNode, ATTRIB_DATE,szTmp);
			xSetNodeAttr(lpChildNode, ATTRIB_HOUR, pTaskTime->every.oneshot_time.wHour);
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTE, pTaskTime->every.oneshot_time.wMinute);

			break;
		}
	case START_TASK_TYPE_WEEK:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_EVERYWEEK,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_week.uInterval_Week);
			xSetNodeAttr(lpChildNode, ATTRIB_HOUR, pTaskTime->every.every_week.wHour);
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTE, pTaskTime->every.every_week.wMinute);
			xSetNodeAttr(lpChildNode, ATTRIB_WEEKMARK, pTaskTime->every.every_week.wWeekMark);
			break;
		}
	case START_TASK_TYPE_MONTH:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_EVERYMONTH,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_MONTHMARK, pTaskTime->every.every_month.wMonthMark);
			xSetNodeAttr(lpChildNode, ATTRIB_HOUR, pTaskTime->every.every_month.wHour);
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTE, pTaskTime->every.every_month.wMinute);
			if(pTaskTime->every.every_month.wDay)
				xSetNodeAttr(lpChildNode, ATTRIB_DAY, pTaskTime->every.every_month.wDay);
			if(pTaskTime->every.every_month.wWeekNum )
			{	
				xSetNodeAttr(lpChildNode, ATTRIB_WEEKNUM, pTaskTime->every.every_month.wWeekNum);
				xSetNodeAttr(lpChildNode, ATTRIB_DAYOFWEEK, pTaskTime->every.every_month.wDayOfWeek);
			}
			if(pTaskTime->every.every_month.wDayMark)
				xSetNodeAttr(lpChildNode, ATTRIB_DAYMARK, pTaskTime->every.every_month.wDayMark);
			break;
		}
	case START_TASK_TYPE_DAY:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_EVERYDAY,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_HOUR, pTaskTime->every.every_day.wHour);
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTE, pTaskTime->every.every_day.wMinute);
			xSetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_day.uInterval_Day);
			break;
		}
	case START_TASK_TYPE_HOUR:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_EVERYHOUR,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_MINUTE, pTaskTime->every.every_hour.wMinute);
			xSetNodeAttr(lpChildNode, ATTRIB_NUMBER, pTaskTime->every.every_hour.uInterval_Hour);
			break;
		}
	case START_TASK_TYPE_INTERVAL_HOUR:
		{
			LPXNode lpChildNode = xCreateChildNode(lpNode,NODE_INTERVALHOURS,NULL);
			if(lpChildNode==NULL) return FALSE; 
			xSetNodeAttr(lpChildNode, ATTRIB_HOURS, pTaskTime->every.interval_hour);
			break;
	    }
	case START_TASK_TYPE_MESSEAGE:
		{
			break;
		}
	}
	return TRUE;

}

BOOL CXMLMgr::xFillTaskFinish(TASKFINISH* pBuff, LPXNode lpParentNode)
{
	if(pBuff==NULL || lpParentNode== NULL) return FALSE;
	LPXNode lpNode = xCreateChildNode(lpParentNode, NODE_FINISH, NULL);
	if(lpNode ==NULL) return FALSE;
	xSetNodeAttr(lpNode, ATTRIB_DELETE,pBuff->wFinishDel );
	xSetNodeAttr(lpNode, ATTRIB_HOURS, pBuff->wHours );
	xSetNodeAttr(lpNode, ATTRIB_MINUTES, pBuff->wMintues);
	return TRUE;
}

//-----------------------------------------------------------------------
DWORD CXMLMgr::AddTask(TASK_XML* pBuff,ST_RSTASK* pTask)
{
	if(pBuff==NULL || pTask== NULL) return -1;

	TASK_LIST_ITER i;
    for(i=pBuff->task_list.begin();i!=pBuff->task_list.end();i++)
	{
		ST_RSTASK* pT  = (*i);
		if( pT->task_attrib.uIndex == pTask->task_attrib.uIndex )
		{
			return ERROR_EXIST_TASK;
		}
	}

	pBuff->task_list.push_back(pTask);

	return 0;
}

DWORD CXMLMgr::ModifyTask(TASK_XML* pBuff,ST_RSTASK* pTask)
{
	if(pBuff==NULL || pTask== NULL) return -1;
	TASK_LIST_ITER i;
	for(i=pBuff->task_list.begin();i!=pBuff->task_list.end();i++)
	{
		ST_RSTASK* pT  = (*i);
		if( pT->task_attrib.uIndex == pTask->task_attrib.uIndex )
		{
			memcpy(pT,pTask,sizeof(ST_RSTASK) );
			return ERROR_SUCCESS;
		}
	}
	return ERROR_NOEXIST_TASK;
}

DWORD CXMLMgr::SetTask(TASK_XML* pBuff,DWORD dwTaskIndex,DWORD dwSet)
{
	if(pBuff==NULL ) return -1;
	TASK_LIST_ITER i;
	for(i=pBuff->task_list.begin();i!=pBuff->task_list.end();i++)
	{
		ST_RSTASK* pT  = (*i);
		if( pT->task_attrib.uIndex == dwTaskIndex )
		{
			pT->task_attrib.wUse = (WORD)dwSet;
			return ERROR_SUCCESS;
		}
	}
	return ERROR_NOEXIST_TASK;
}

DWORD CXMLMgr::SetAllTasks(TASK_XML* pBuff,DWORD dwSet)
{
	if(pBuff==NULL ) return -1;
	TASK_LIST_ITER i;
	for(i=pBuff->task_list.begin();i!=pBuff->task_list.end();i++)
	{
		ST_RSTASK* pT  = (*i);
		pT->task_attrib.wUse = (WORD)dwSet;
	}	
	return ERROR_SUCCESS;
}

DWORD CXMLMgr::DeleteTask(TASK_XML* pBuff,DWORD dwTaskIndex)
{
	if(pBuff==NULL ) return -1;
	TASK_LIST_ITER i;
	for(i=pBuff->task_list.begin();i!=pBuff->task_list.end();i++)
	{
		ST_RSTASK* pT  = (*i);
		if( pT->task_attrib.uIndex == dwTaskIndex )
		{
			pBuff->task_list.erase(i);
			delete pT;
			return ERROR_SUCCESS;
		}
	}
	return ERROR_NOEXIST_TASK;
}


