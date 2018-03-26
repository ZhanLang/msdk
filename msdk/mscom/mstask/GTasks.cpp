#include "StdAfx.h"
#include "windows.h"

#include "gtasks.h"
#include <Mmsystem.h>
#include <atltime.h>

//extern HANDLE g_hSingle;
#define TASK_TIMER_DELAY       (6*1000)
#define TASK_TIMER_AFTERHBOOT  (5*1000)


CGTasks::CGTasks(void)
{
	m_uPeriod = 0;
	m_nTimerID = 0;
	m_bFinish = 0;
	m_dwIntervalSecond = 0;
	m_dwIntervalHour  = 0;
	m_pTaskXML = NULL;

	m_bLoopTime1 = TRUE;
	m_pUserXML = NULL;
	m_bStartSceenSaver = FALSE;
	m_bStopSceenSaver = FALSE;
	m_dwInSceenSaver = 0;
	m_dwScreenSession = 0;
	m_dwUpdateSession = 0;
	m_dwSreenType= 0;
	m_dwPreStatus = 0;
	m_bUpdate= FALSE;
	m_dwPowerPercent = 50;
	m_bSetup = FALSE;
	GetXMLFile();
}

CGTasks::~CGTasks(void)
{
	Stop();
}

void CGTasks::ReleaseXmlBuf(TASK_XML* pTaskXML)
{
	if(pTaskXML)
	{	
		CLEAR_POINT_LIST(pTaskXML->task_list);
		delete pTaskXML;
		pTaskXML = NULL;
	}
}

BOOL CGTasks::GetXMLFile()
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


HRESULT CGTasks::SetConfigFile(const TCHAR * lpFileName)
{
	//if(lpFileName==NULL) return -1;
	//TCHAR szBuff[MAX_PATH];
	//ZeroMemory(szBuff,sizeof(szBuff));
	//GetTaskPath(szBuff,MAX_PATH);

	//lstrcpy(m_szRsTaskFile,szBuff );
	//lstrcat(m_szRsTaskFile,_T("\\rstask.xml"));

	//WritePrivateProfileString(_T("location"),_T("user"),lpFileName,szBuff);
	//WritePrivateProfileString(_T("location"),_T("rising"),m_szRsTaskFile,szBuff);

	return 0;
}


HRESULT CGTasks::SetTask(DWORD dwTaskIndex,DWORD dwSet)
{
	RASSERTP(m_pUserXML,E_FAIL);
	if(m_XMLMgr.SetTask(m_pUserXML,dwTaskIndex,dwSet) == 0 )
	{
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
		return 0;
	}
	return E_FAIL;
}

HRESULT CGTasks::SetAllTasks(DWORD dwSet)
{
	RASSERTP(m_pUserXML,E_FAIL);
	if(m_XMLMgr.SetAllTasks(m_pUserXML,dwSet) == 0 )
	{
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
		return 0;
	}
	return E_FAIL;
}

HRESULT CGTasks::AddTask(ST_RSTASK * lpTask)
{
	RASSERTP(m_pUserXML ,E_FAIL);
	RASSERTP(lpTask ,E_FAIL);
	if(m_XMLMgr.AddTask(m_pUserXML,lpTask) == 0 )
	{
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
		return 0;
	}
	return E_FAIL;
}

HRESULT CGTasks::DeleteTask( DWORD dwTaskIndex)
{
	RASSERTP(m_pUserXML ,E_FAIL);
	if(m_XMLMgr.DeleteTask(m_pUserXML,dwTaskIndex) == 0 )
	{
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
		return 0;
	}
	return E_FAIL;
}


void CGTasks::SaveFile(int nType)
{
	if(m_pUserXML)
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
}

HRESULT CGTasks::GetTask(DWORD dwTaskIndex,ST_RSTASK & lpTask)
{
	return E_FAIL;
}

HRESULT CGTasks::ModifyTask(ST_RSTASK * lpTask)
{
	RASSERTP(m_pUserXML ,E_FAIL);
	RASSERTP(lpTask ,E_FAIL);

	RFAILED( m_XMLMgr.ModifyTask(m_pUserXML,lpTask) );
	if(! m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML))
		return ERROR_SAVE_FILE;
	return S_OK;
}

HRESULT CGTasks::ControlTask(DWORD dwTaskIndex,DWORD dwControlID)
{
	return E_FAIL;
}

HRESULT CGTasks::ControlTasks(DWORD dwControlID)
{
	return E_FAIL;
}

HRESULT CGTasks::QueryTask(DWORD dwTaskClassID,IN OUT ST_RSTASK* pTaskArray,IN OUT DWORD dwCount)
{
	return E_FAIL;
}

HRESULT CGTasks::QueryTaskStatus(DWORD dwTaskIndex,SYSTEMTIME*pPrevStartTime, SYSTEMTIME*pNextStartTime)
{
	return E_FAIL;		
}

HRESULT CGTasks::CfgModify()
{
	//停止电源任务
	START_TASKVECTOR::SAFEARRAYVECTOR TempVector;	
	m_VectorInterval.GetAll(TempVector);
	int nNum  = m_VectorInterval.GetSize();
	//重读配置,但是不起with-boot的任务
	LoadXML(1);

	START_TASKVECTOR::SAFEARRAYVECTOR NewVector;	
	m_VectorInterval.GetAll(NewVector);

	START_TASKVECTOR::SAFEARRAYVECTOR::iterator i; 
	START_TASKVECTOR::SAFEARRAYVECTOR::iterator j;

	for(i = TempVector.begin(); i != TempVector.end(); i ++)
	{	
		ST_RSTASK& OldTask = (*i);

		for(j = NewVector.begin(); j != NewVector.end(); j ++)
		{
			ST_RSTASK& NewTask = (*j);
			if( OldTask.task_attrib.uIndex == NewTask.task_attrib.uIndex)
			{			
				if(OldTask.dwProgressID &&  FALSE == IsIdleScanRun(NewTask) )
				{
					//电池快用完，停止已启动的任务
					m_Start.StartProgram(this,&OldTask,1);				

				}
				else if(OldTask.dwProgressID )
				{
					//把已启动的标志放到新里去
					int nIndex = GetVectorIndexByTask(m_VectorInterval,NewTask.task_attrib.uIndex);
					if(nIndex == -1) 
						break;
 
					NewTask.dwProgressID = OldTask.dwProgressID;
					m_VectorInterval.SetData(nIndex,NewTask);
					
				}
				break;
			}
		}
	}

	return 0;
}

HRESULT CGTasks::OnMessage(DWORD msgid, LPVOID buffin, size_t cc)
{	
	DWORD hr = S_OK;
	switch(msgid)
	{
	case TASK_MSG_SCREENSAVER:
		{
			RSLOG(RSLOG_DETAIL, _T("CGTasks::屏保消息 \n"));

			if(buffin && sizeof(SCREENSAVERDETECTINFO) == cc)
			{
				LP_SCREENSAVERDETECTINFO pInfo = (LP_SCREENSAVERDETECTINFO)buffin;
				m_dwScreenSession = pInfo->dwSessionID;
				if(pInfo->dwScrState == SCRSTATE_START_SCREEN_SAVER)
				{
					m_bStartSceenSaver = TRUE;
					m_dwInSceenSaver = 0;

				}
				else if(pInfo->dwScrState == SCRSTATE_STILL_IN_SCREEN_SAVER)
				{
					if(m_dwInSceenSaver > 0xEFFFFFFF)
					{
						m_dwInSceenSaver = 1;
					}
					else
					{
						m_dwInSceenSaver++;
					}
				}
				else if(pInfo->dwScrState == SCRSTATE_OVER_SCREEN_SAVER) 
				{
					m_bStopSceenSaver = TRUE;

				}

			}
			else
			{
				hr = ERROR_INVALID_PARAMETER;
			}
			break;
		}
	case TASK_MSG_UPDATE:
		{
			m_bUpdate = TRUE;
			break;
		}
	case TASK_MSG_POWER:
		{
			break;
		}
	case TASK_MSG_SCANCFG:
		{		
			if(buffin && sizeof(DWORD) == cc)
			{   
				DWORD dwState = *((DWORD *)buffin);
				m_dwPowerPercent = dwState;
				RSLOG(RSLOG_ACTION, _T("CGTasks::TASK_MSG_SCANCFG 设置值 =%d\n"),m_dwPowerPercent);
			}
			break;
		}
	case  TASK_MSG_SETUP:
		{
			m_bSetup = (BOOL)buffin;
			break;
		}

	default:
		break;
	}
	return  hr;
}

DWORD CGTasks::ModifyTime(DWORD dwIndex,SYSTEMTIME*pTime)
{
	RASSERTP(m_pUserXML ,E_FAIL);
	RASSERTP(pTime ,E_FAIL);

	TASK_LIST* pBuff = &(m_pUserXML->task_list);
	for(TASK_LIST_ITER i = pBuff->begin(); i != pBuff->end(); i ++)
	{
		ST_RSTASK * pTask = (*i);
		if(pTask->task_attrib.uIndex == dwIndex)
		{
			memcpy(&(pTask->task_time.last_time),pTime,sizeof(SYSTEMTIME) );
			ModifyTimetoVertor(pTask->task_attrib.uType,dwIndex,pTime);
			m_XMLMgr.SaveXMLFile(m_szUserTaskFile,m_pUserXML);
			return S_OK;
		}
	}
	return E_FAIL;

}

int CGTasks::GetVectorIndexByTask(START_TASKVECTOR& task_vector,DWORD dwIndex)
{
	START_TASKVECTOR::SAFEARRAYVECTOR TempVector;	
	task_vector.GetAll(TempVector);
	//RSLOG(RSLOG_ACTION, "CGTasks::GetVectorIndexByTask 0 =%d,size=%d",dwIndex,task_vector.GetSize());
	START_TASKVECTOR::SAFEARRAYVECTOR::iterator it = TempVector.begin();
	int i=0;
	for(; it != TempVector.end(); it ++,i++)
	{
		ST_RSTASK task = (*it);
		//RSLOG(RSLOG_ACTION, "CGTasks::GetVectorIndexByTask 1 =%d", task.task_attrib.uIndex);
		if( dwIndex == task.task_attrib.uIndex)
		{
			return i;
		}
	}
	return -1;
}

DWORD CGTasks::ModifyTimetoVertor(DWORD dwType,DWORD dwIndex,SYSTEMTIME*pTime)
{
	ST_RSTASK task;
	if(dwType == 0 || pTime==0  ) return -1;
	memset(&task,0,sizeof(task));
	int nIndex = 0;
	if( dwType == START_TASK_TYPE_WITHBOOT)
	{

	}
	else if(dwType == START_TASK_TYPE_AFTERBOOT)
	{
		nIndex = GetVectorIndexByTask(m_VectorAfterBoot,dwIndex);
		if(nIndex == -1) return -1;
		if(m_VectorAfterBoot.GetData(nIndex,task))
		{   
			memcpy(&task.task_time.last_time,pTime,sizeof(SYSTEMTIME));
			m_VectorAfterBoot.SetData(nIndex,task);
		}
	}
	else 
	{
		nIndex = GetVectorIndexByTask(m_VectorInterval,dwIndex);
		if(nIndex == -1) return -1;
		if(m_VectorInterval.GetData(nIndex,task))
		{
			memcpy(&task.task_time.last_time,pTime,sizeof(SYSTEMTIME));
			m_VectorInterval.SetData(nIndex,task);
		}
	}
	return 0;

}

DWORD CGTasks::ModifyRunStaus(DWORD dwType,DWORD dwIndex,DWORD dwProcID)
{
	ST_RSTASK task;
	if(dwType == 0 || dwIndex==0 ) return -1;
	memset(&task,0,sizeof(task));

	int nIndex = 0;
	//RSLOG(RSLOG_ACTION, "CGTasks::ModifyRunStaus 0 =%d",dwIndex);

	if( dwType == START_TASK_TYPE_WITHBOOT)
	{
		/*
		nIndex = GetVectorIndexByTask(m_VectorWithBoot,dwIndex);
		if(m_VectorWithBoot.GetData(nIndex,task))
		{
		task.dwProgressID = dwProcID;
		m_VectorWithBoot.SetData(nIndex,task);
		}
		*/
	}
	else if(dwType == START_TASK_TYPE_AFTERBOOT)
	{
		nIndex = GetVectorIndexByTask(m_VectorAfterBoot,dwIndex);
		if(nIndex == -1) return -1;
		if(m_VectorAfterBoot.GetData(nIndex,task))
		{   
			task.dwProgressID = dwProcID;
			if(dwProcID)
			{
				//RSLOG(RSLOG_ACTION, "CGTasks::ModifyRunStaus 1 =%d",nIndex);
				m_VectorAfterBoot.SetData(nIndex,task);

			}
			else 
			{
				//RSLOG(RSLOG_ACTION, "CGTasks::ModifyRunStaus 2 =%d",nIndex);
				m_VectorAfterBoot.RemoveData(nIndex);
			}
			//RSLOG(RSLOG_ACTION, "CGTasks::ModifyRunStaus 3 =%d",nIndex);

		}
	}
	else 
	{
		nIndex = GetVectorIndexByTask(m_VectorInterval,dwIndex);
		if(nIndex == -1) 
		{
			RSLOG(RSLOG_DETAIL, _T("取vector的索引失败 任务ID= %d\n"),dwIndex);
			return -1;
		}
		if(m_VectorInterval.GetData(nIndex,task))
		{
			task.dwProgressID = dwProcID;
			m_VectorInterval.SetData(nIndex,task);
		}
	}
	return 0;
}


DWORD CGTasks::LoadXML(int nType)
{
	//-------------------------------------------------------------
	m_bUpdate = FALSE;
	m_XMLMgr.UnInit();

	if(	CheckXmlFile(m_XMLMgr) )
	{
		RSLOG(RSLOG_DETAIL, _T("ModifyXml错误"));
		return -1;
	}

	ReleaseXmlBuf(m_pUserXML);
	m_pUserXML = NULL;
	m_pUserXML = new TASK_XML;
	//load rstasku.xml
	if(! m_XMLMgr.LoadXML(m_szUserTaskFile,m_pUserXML,m_VectorInterval,m_VectorAfterBoot,nType,0))
	{
		ReleaseXmlBuf(m_pUserXML);
		m_pUserXML = NULL;
		return -1;
	}

	StartTimerThread();
	return 0;
}

HRESULT CGTasks::QueryTaskTime(ST_RSTASK& Task)
{
	return E_FAIL;   
}

BOOL CGTasks::CheckDontRunTask(SYSTEMTIME& lnowTime,  ST_RSTASK& Temp)
{
	TASKTIME *pTaskTime =& Temp.task_time;

	if(pTaskTime->stop_time.wYear&& pTaskTime->stop_time.wMonth&&
		pTaskTime->stop_time.wDay )
	{
		SYSTEMTIME tmTask;
		memcpy(&tmTask,&lnowTime,sizeof(tmTask));

		tmTask.wYear = Temp.task_time.stop_time.wYear;
		tmTask.wMonth = Temp.task_time.stop_time.wMonth;
		tmTask.wDay = Temp.task_time.stop_time.wDay;

		CTime t2(lnowTime);
		CTime t1(tmTask);
		LONGLONG lTimeSpan = (t2 - t1).GetDays();
		if(lTimeSpan>=0)
		  return FALSE;   
	}
	return TRUE;	
}

BOOL CGTasks::CheckCanRun(SYSTEMTIME& lnowTime, ST_RSTASK& Temp)
{
	//if(Temp.task_attrib.wPower && Temp.task_attrib.uClassID==RS_RAVTASK_CLASS_IDLE_SCAN && m_bPowerStop)
	//{
	//	//空闲扫描的任务，电池快用完了
	//	return FALSE;
	//}
	return CheckStartAndStopTime(lnowTime,Temp);
}

BOOL  CGTasks::IsIdleScanRun(ST_RSTASK& Temp)
{
	return TRUE;
	if(Temp.task_attrib.wPower && Temp.task_attrib.uClassID==RS_RAVTASK_CLASS_IDLE_SCAN)
	{
		//空闲扫描的任务，电池快用完了
		return IsPowerCanDo();
	}
	return TRUE;
}

BOOL CGTasks::IsPowerCanDo()
{
	SYSTEM_POWER_STATUS sps;
	ZeroMemory(&sps,sizeof(sps));
	GetSystemPowerStatus(&sps);
	if (sps.ACLineStatus == 0)
	{
		if(! (sps.BatteryFlag == 128 || sps.BatteryFlag == 255))
		{
			RSLOG(RSLOG_DETAIL,_T("AC电源未接入 电池电量 =%d 设置值=%d \n"),sps.BatteryLifePercent,m_dwPowerPercent);
			if(sps.BatteryLifePercent < m_dwPowerPercent )
			{
				return FALSE;
			}
		}
	} 
	else if (sps.ACLineStatus == 1)
	{
		RSLOG(RSLOG_DETAIL,_T("使用AC电源"));
	} 
	else
	{
		RSLOG(RSLOG_DETAIL,_T("AC电源状态不明"));
	}
	return TRUE;
}

DWORD CGTasks::CheckTask()
{
	m_dwIntervalSecond += (TASK_TIMER_DELAY/1000);
	if(m_dwIntervalSecond / 3600 )
	{
		m_dwIntervalHour ++;
		m_dwIntervalSecond = 0;
	}
	if( m_bFinish || ! m_bLoopTime1) return 0;

	m_bFinish = 1;
	START_TASKVECTOR::SAFEARRAYVECTOR TempVector;	
	m_VectorInterval.GetAll(TempVector);

	SYSTEMTIME lnowTime;
	memset(&lnowTime,0,sizeof(lnowTime));
	GetLocalTime(&lnowTime);//得到当前时间
	//RSLOG(RSLOG_DETAIL, "hour=%d,minute=%d,second=%d",lnowTime.wHour,lnowTime.wMinute,lnowTime.wSecond);

	DWORD dwModify = 0;
	START_TASKVECTOR::SAFEARRAYVECTOR::iterator it = TempVector.begin();
	for(; it != TempVector.end(); it ++)
	{	
		BOOL bStart = FALSE;
		ST_RSTASK& Temp = (*it);
		TASKTIME *pTaskTime =& Temp.task_time;
		if(! m_bLoopTime1) break;
		if(Temp.dwProgressID && _tcscmp(Temp.task_cmd.end,_T("")) && FALSE == IsIdleScanRun(Temp))
		{
			//电池快用完，停止已启动的任务
			m_Start.StartProgram(this,&Temp,1);				
		}
		else if(Temp.dwProgressID && Temp.task_time.last_time.wYear && 
			Temp.task_time.last_time.wMonth && 
			Temp.task_time.last_time.wDay  &&
			(Temp.task_finish.wHours ||Temp.task_finish.wMintues))
		{
			//停止已经启动的
			//时间快到了吗，快停吧
			LONG lMintue = Temp.task_finish.wHours*60 + Temp.task_finish.wMintues;
			CTime t1(lnowTime);
			CTime t2(Temp.task_time.last_time);
			LONGLONG  lMt = (t1-t2).GetTotalMinutes();

			if(lMt >= lMintue && _tcscmp(Temp.task_cmd.end,_T("")))
			{
				//用我们的命令行停止
				m_Start.StartProgram(this,&Temp,1);				
			}
			
		}

		switch(Temp.task_attrib.uType)
		{
		case START_TASK_TYPE_MONTH:
		{
			if( CheckCanRun(lnowTime,Temp) &&
				Temp.task_time.every.every_month.wHour == lnowTime.wHour &&
				Temp.task_time.every.every_month.wMinute == lnowTime.wMinute &&
				( pTaskTime->every.every_month.wMonthMark &  GetMonthMark(lnowTime.wMonth))	)
			{

				if(pTaskTime->every.every_month.wDay == lnowTime.wDay)
				{
					bStart = TRUE;
					break;
				}
				else if(pTaskTime->every.every_month.wDayOfWeek == lnowTime.wDayOfWeek && 
					GetWeekNums(lnowTime)==pTaskTime->every.every_month.wWeekNum)
				{
					bStart = TRUE;
				}

			}
			break;
		}

		case START_TASK_TYPE_WEEK:
		{
			if( CheckCanRun(lnowTime,Temp) && 
				Temp.task_time.every.every_week.wHour == lnowTime.wHour &&
				Temp.task_time.every.every_week.wMinute == lnowTime.wMinute &&
				(pTaskTime->every.every_week.wWeekMark &  GetWeekMark(lnowTime.wDayOfWeek)))
			{
				//计算间隔了的周数?
				LONGLONG days = GetDayNums(lnowTime,Temp);
				if(days>0)
				{
					int nWeeks = 0;
					LONGLONG nTmpDay = days- lnowTime.wDayOfWeek-1 ;
					if(nTmpDay>0)
					{
						nWeeks = 1;
						nWeeks += (INT)(nTmpDay/7);
						if(pTaskTime->every.every_week.uInterval_Week && (nWeeks%pTaskTime->every.every_week.uInterval_Week) == 0)
							bStart = TRUE;
					}
					else if(nTmpDay<=0 && pTaskTime->every.every_week.uInterval_Week==1)
					{
						bStart = TRUE;
					}
				}
				if(days==0 && pTaskTime->every.every_week.uInterval_Week==1)
				{
					bStart = TRUE;
				}
			}
			break;
		}
		case START_TASK_TYPE_DAY:
		{

			//RSLOG(RSLOG_DETAIL, "每天任务,%d,%d,%d",lnowTime.wHour,lnowTime.wMinute,CheckStartAndStopTime(lnowTime,Temp));
			//RSLOG(RSLOG_DETAIL,"config=%d,%d",Temp.task_time.every.every_day.wHour,Temp.task_time.every.every_day.wMinute);

			if( CheckCanRun(lnowTime,Temp) && 
				Temp.task_time.every.every_day.wHour == lnowTime.wHour &&
				Temp.task_time.every.every_day.wMinute == lnowTime.wMinute )
			{

			//隔了几天?
				LONGLONG lDay = GetDayNums(lnowTime,Temp);
				if(pTaskTime->every.every_day.uInterval_Day && 
					((lDay && ( lDay % pTaskTime->every.every_day.uInterval_Day ==0)) || lDay==0) )
				{	
					bStart = TRUE;	
				}
			}
			break; 
		}
		case START_TASK_TYPE_HOUR:
		{

			if( CheckCanRun(lnowTime,Temp) && pTaskTime->every.every_hour.wMinute == lnowTime.wMinute)
			{
				SYSTEMTIME ctm;
				memcpy(&ctm,&lnowTime,sizeof(ctm));
				ctm.wMinute = 0;
				ctm.wSecond =0;
				ctm.wMilliseconds = 0;

				CTime t2(ctm);
				CTime t1(m_starttime);
				LONG lHours = (t2-t1).GetHours();
				if(pTaskTime->every.every_hour.uInterval_Hour && 
					(lHours && ( lHours % pTaskTime->every.every_hour.uInterval_Hour ==0) ||lHours==0) )
				{
					bStart = TRUE;
				}
			}
			break;
		}
		case START_TASK_TYPE_INTERVAL_HOUR:
		{

			if( CheckCanRun(lnowTime,Temp) && Temp.task_specific.wSpecificMethod==0 &&
				m_dwIntervalSecond < 6 &&m_dwIntervalHour &&
				pTaskTime->every.interval_hour &&(m_dwIntervalHour % pTaskTime->every.interval_hour)==0)
			{
				bStart = TRUE;
			}
			else if(CheckCanRun(lnowTime,Temp) && Temp.task_specific.wSpecificMethod==2)
			{

				DWORD nHour= 0;
				DWORD nSend= 0;

				int nAll = m_dwIntervalSecond + m_dwIntervalHour*3600 - Temp.task_specific.wFloatSecond * 60;
				if(nAll >0 )
				{ nHour = nAll / 3600;
				  nSend = nAll % 3600;
				}

				if(nAll==0  || (nSend < 6 && nHour &&
					pTaskTime->every.interval_hour &&(nHour % pTaskTime->every.interval_hour)==0))
				{

					bStart = TRUE;
				}
			}

			break;
		}
		case START_TASK_TYPE_RUNONESHOT:
		{
			/*
			RSLOG(RSLOG_DETAIL, "START_TASK_TYPE_RUNONESHOT任务 %d-%d-%d :%2d:%2d:%2d",
				pTaskTime->every.oneshot_time.date.wYear,
				pTaskTime->every.oneshot_time.date.wMonth,
				pTaskTime->every.oneshot_time.date.wDay,
				pTaskTime->every.oneshot_time.wHour,
				pTaskTime->every.oneshot_time.wMinute);
				*/
			if(Temp.task_attrib.wUse &&
				pTaskTime->every.oneshot_time.date.wYear == lnowTime.wYear &&
				pTaskTime->every.oneshot_time.date.wMonth == lnowTime.wMonth&&
				pTaskTime->every.oneshot_time.date.wDay == lnowTime.wDay &&
				pTaskTime->every.oneshot_time.wHour == lnowTime.wHour &&
				pTaskTime->every.oneshot_time.wMinute == lnowTime.wMinute	 )
			{
				bStart = TRUE;
			}

			break;
		}
		case START_TASK_TYPE_MESSEAGE:
		{
			switch(Temp.task_attrib.uClassID)
			{
			case RS_RAVTASK_CLASS_SCREEN:
			{

				if(Temp.task_attrib.wUse == 0 )
					break;
				if(m_bStartSceenSaver )
				{
					//if(Temp.task_attrib.wPower &&  IsPowerCanDo() == FALSE )
					//{
					//	break;
					//}
					
					m_bStartSceenSaver = FALSE;

					m_Start.StartProgram(this,&Temp,0,m_dwScreenSession);
					m_dwSreenType = 1;
					m_dwTimeSum =0;
					m_dwPreStatus = m_dwInSceenSaver;
				}
				if(m_bStopSceenSaver)
				{
					m_bStopSceenSaver = FALSE;

					m_Start.StartProgram(this,&Temp,1,m_dwScreenSession);
					m_dwSreenType = 0;
				}
				if(m_dwSreenType)
				{
					//if(Temp.task_attrib.wPower && IsPowerCanDo() == FALSE)
					//{
					//	//电量不够了
					//	m_Start.StartProgram(this,&Temp,1,m_dwScreenSession);
					//	m_dwSreenType = 0;
					//	break;
					//}

					m_dwTimeSum ++;
					if(m_dwTimeSum >2 )
					{
						if(m_dwPreStatus == m_dwInSceenSaver)
						{
							m_Start.StartProgram(this,&Temp,1,m_dwScreenSession);	
							m_dwSreenType = 0;
						}
						else
						{
							m_dwTimeSum = 1;
							m_dwPreStatus = m_dwInSceenSaver;
						}
					}
				}
				break;
			}
			default :
				break;

			}
			break;
		}
		case START_TASK_TYPE_AFTERUPDATE:
		{
			if(Temp.task_attrib.wUse && m_bUpdate)
			{
				m_Start.StartProgram(this,&Temp,0,m_dwUpdateSession);	
			}
			m_bUpdate = FALSE;
			m_dwUpdateSession=0;

			break;
		}
		default:
			continue ;
		}

		if(	bStart )
		{
			//RSLOG(RSLOG_DETAIL, "EXPLore任务开始");
			//保证在一分钟内有且仅有一次触发，而且是在每分钟的开始
			int nTimeTick = ((int)(lnowTime.wSecond)/(TASK_TIMER_DELAY / 1000));

			 if((Temp.task_attrib.uType == START_TASK_TYPE_INTERVAL_HOUR || nTimeTick==0) && 
				 m_check.CheckExplorer() &&
				 IsIdleScanRun(Temp) )
			{
				if(Temp.task_specific.wFindAction==1 && _tcscmp(Temp.task_cmd.end,_T("")))
				{//到了时间.任务还在运行,停止任务,再重起任务
						m_Start.StartProgram(this,&Temp,1);
				}
				m_Start.StartProgram(this,&Temp);
			}
		}

		//完成后的操作
		//删除不在启动的任务
		if(Temp.task_attrib.uType !=0 && Temp.task_finish.wFinishDel )
		{
			BOOL bRunDel = FALSE;
			if(Temp.task_attrib.uType != START_TASK_TYPE_RUNONESHOT && ! CheckDontRunTask(lnowTime,Temp))
			{
				bRunDel = TRUE;
			}
			else if(Temp.task_attrib.uType == START_TASK_TYPE_RUNONESHOT)
			{
				SYSTEMTIME tmTask;
				memcpy(&tmTask,&lnowTime,sizeof(tmTask));
				tmTask.wYear = pTaskTime->every.oneshot_time.date.wYear;
				tmTask.wMonth = pTaskTime->every.oneshot_time.date.wMonth;
				tmTask.wDay = pTaskTime->every.oneshot_time.date.wDay;
				tmTask.wHour = pTaskTime->every.oneshot_time.wHour;
				tmTask.wMinute = pTaskTime->every.oneshot_time.wMinute;

				CTime t2(lnowTime);
				CTime t1(tmTask);
				LONGLONG lTimeSpan = (t2 - t1).GetTotalMinutes();
				if(lTimeSpan>0)
				{
					bRunDel = TRUE;
				}
			}

			if(bRunDel)
			{
				dwModify = 0x2 ;
				m_XMLMgr.DeleteTask(m_pUserXML, Temp.task_attrib.uIndex);
			}
		}
	}

	if(dwModify)
	{
		SaveFile(dwModify);
	}

	if(m_VectorAfterBoot.GetSize() && m_check.CheckExplorer() )
		CheckAfterBootTask();

	m_bFinish = 0;

	return 0;
}

DWORD CGTasks::CheckAfterBootTask()
{
	START_TASKVECTOR::SAFEARRAYVECTOR TempVector;	
	m_VectorAfterBoot.GetAll(TempVector);

	START_TASKVECTOR::SAFEARRAYVECTOR::iterator it = TempVector.begin();
	for (; it != TempVector.end(); it ++)
	{
		BOOL bStart = FALSE;
		if(! m_bLoopTime1) break;
		ST_RSTASK& Temp = (*it);

		if(Temp.task_attrib.wUse == 0 || m_bSetup)
		{
			int nIndex = GetVectorIndexByTask(m_VectorAfterBoot,Temp.task_attrib.uIndex);
			m_VectorAfterBoot.RemoveData(nIndex);
			continue;
		}

		if(Temp.task_time.every.after_boot.wHours==0 && Temp.task_time.every.after_boot.wMinutes==0 
			&&Temp.task_time.every.after_boot.wSeconds==0)
		{

			if(m_dwIntervalHour==0 && (m_dwIntervalSecond / 60) >3)
			{
				bStart = TRUE;
			}
		}
		else
		{
			DWORD dwSeconds = m_dwIntervalSecond -Temp.task_time.every.after_boot.wMinutes*60 ;
			if(dwSeconds < 0) dwSeconds =0 ;
			if(m_dwIntervalHour > Temp.task_time.every.after_boot.wHours ||
				(m_dwIntervalHour == Temp.task_time.every.after_boot.wHours && (m_dwIntervalSecond / 60) > Temp.task_time.every.after_boot.wMinutes) ||
				(m_dwIntervalHour == Temp.task_time.every.after_boot.wHours && (m_dwIntervalSecond / 60) == Temp.task_time.every.after_boot.wMinutes &&
				dwSeconds  >= Temp.task_time.every.after_boot.wSeconds ))
			{
				RSLOG(RSLOG_DETAIL, _T("After任务:s=%d,minute=%d,second=%d \n"),m_dwIntervalSecond / 60,Temp.task_time.every.after_boot.wMinutes,	dwSeconds);
				bStart = TRUE;
			}
		}

		if(bStart)
		{
			if(IsIdleScanRun(Temp))
			{
				m_Start.StartProgram(this,&Temp);
			}
			int nIndex = GetVectorIndexByTask(m_VectorAfterBoot,Temp.task_attrib.uIndex);
			m_VectorAfterBoot.RemoveData(nIndex);
		}	
	}	
	return TRUE;
}


void CALLBACK TaskTimer(  UINT uID,UINT uMsg,DWORD dwUser, DWORD dw1,DWORD dw2 )
{
	CGTasks *pThis= (CGTasks *)	dwUser;
	if(pThis==NULL) return;
	pThis->CheckTask();
}


HRESULT CGTasks::Stop() 
{
	m_dwIntervalSecond = 0;
	m_dwIntervalHour  = 0;
	m_bLoopTime1 = FALSE;

	StopimerThread();

	ReleaseXmlBuf(m_pUserXML);
	m_pUserXML = NULL;
	m_XMLMgr.UnInit();
	return 0;
}

HRESULT CGTasks::Run()
{
	m_bLoopTime1 = TRUE;
	m_dwIntervalSecond = 0;
	m_dwIntervalHour  = 0;

	m_XMLMgr.Init(&m_Start);

	if(S_OK !=  LoadXML(0) )
	{
		RSLOG(RSLOG_DETAIL, _T("LoadXML error"));
		return -1;
	}

	memset(&m_starttime,0,sizeof(m_starttime));
	GetLocalTime(&m_starttime);
	m_starttime.wMinute = 0;
	m_starttime.wSecond = 0;
	m_starttime.wMilliseconds =0;

	StartTimerThread();
	return S_OK;
} 

DWORD CGTasks::StartTimerThread()
{
	if( 0 == m_nTimerID && (m_VectorInterval.GetSize() || m_VectorAfterBoot.GetSize()) )
	{
		TIMECAPS tc;
		memset(&tc, 0, sizeof(TIMECAPS));
		timeGetDevCaps(&tc, sizeof(TIMECAPS));
		m_uPeriod = 1000;
		//if (m_uPeriod < tc.wPeriodMin)
		//	m_uPeriod = tc.wPeriodMin;
		timeBeginPeriod(m_uPeriod);
		m_nTimerID = timeSetEvent(TASK_TIMER_DELAY, m_uPeriod, (LPTIMECALLBACK )TaskTimer,
			(DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		if(m_nTimerID==0)
		{
			RSLOG(RSLOG_DETAIL, _T("Set TaskTimer error!"));
			return -1;
		}
	}
	return 0;
}

DWORD CGTasks::StopimerThread()
{
	try
	{
		if(m_nTimerID)
		{
			timeKillEvent(m_nTimerID);
			m_nTimerID =0;
		}
		if(m_uPeriod)
		{
			timeEndPeriod(m_uPeriod);
			m_uPeriod = 0;
		}

	}catch (...){
		RSLOG(RSLOG_DETAIL, _T("timeKillEvent error"));
	}
	return 0;
}
