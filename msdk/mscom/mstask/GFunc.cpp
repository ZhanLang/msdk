#include "StdAfx.h"
#include ".\gfunc.h"

CGFunc::CGFunc(void)
{
}

CGFunc::~CGFunc(void)
{
}

DWORD CGFunc::CheckXmlFile(CXMLMgr& m_XMLMgr)
{
	TASK_XML* pTaskXML = NULL;
	TASK_XML* pUserXML = NULL;
	pTaskXML = new TASK_XML;
	AutoObject(pTaskXML);

	if(! m_XMLMgr.LoadXMLFile(m_szRsTaskFile,pTaskXML))
		return ERROR_LOAD_FILE;
	if( !IsFileExist(m_szUserTaskFile))
	{
		//没有用户rstasku.xml文件,将ratask.xml复制给它
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,pTaskXML);
		return 0;
	}

	pUserXML = new TASK_XML;
	AutoObject2(pUserXML);
	//有这个文件,就要将ratask.xml里最后改变的任务,也要改到rstasku.xml里去.
	m_XMLMgr.LoadXMLFile(m_szUserTaskFile,pUserXML);

	TASK_LIST_ITER i,j;
	BOOL bFind = FALSE;
	BOOL bModify = FALSE;
	INDEX_LIST  index_list;
	for(i=pUserXML->task_list.begin();i!=pUserXML->task_list.end();i++)
	{
		ST_RSTASK* dT  = (*i);
		if(dT->task_attrib.uIndex <= START_TASK_INDEX_CUSTOM)
			index_list.push_back(dT->task_attrib.uIndex);
	}

	//删除更新后没有的任务
	INDEX_LIST::iterator m;
	for(m=index_list.begin();m!=index_list.end();m++)
	{
		bFind = FALSE;
		for(j=pTaskXML->task_list.begin();j!=pTaskXML->task_list.end();j++)
		{
			ST_RSTASK* dT  = (*j);
			if( dT->task_attrib.uIndex == *m )
			{
				bFind = TRUE;
				break;
			}
		}
		if(! bFind )
		{
			m_XMLMgr.DeleteTask(pUserXML,*m);
			bModify = TRUE;
		}

	}
	//添加新的任务
	for(i=pTaskXML->task_list.begin();i!=pTaskXML->task_list.end();i++)
	{
		bFind = FALSE;
		ST_RSTASK* sT  = (*i);
		for(j= pUserXML->task_list.begin();j!= pUserXML->task_list.end();j++)
		{
			ST_RSTASK* dT  = (*j);
			if( sT->task_attrib.uIndex == dT->task_attrib.uIndex )
			{
				bFind = TRUE;
				break;
			}
		}
		if(! bFind )
		{
			ST_RSTASK* nT  = new ST_RSTASK;
			memcpy(nT,sT,sizeof(ST_RSTASK));
			pUserXML->task_list.push_back(nT);
			bModify = TRUE;
		}
	}
	if(bModify)
		m_XMLMgr.SaveXMLFile(m_szUserTaskFile,pUserXML);

	return 0;
}



WORD CGFunc::GetMonthMark(WORD wMonth)
{
	//查表法
	WORD WORKS[13] ={0x0,0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800};
	if(wMonth<0 || wMonth>12)
		return 0;	
	return WORKS[wMonth];
}

WORD CGFunc::GetWeekMark(WORD wWeek)
{
	WORD WORKS[7] ={0x1,0x2,0x4,0x8,0x10,0x20,0x40};
	if(wWeek<0 || wWeek >6)
		return 0;
	//wWeek =0 //星期日
	return WORKS[wWeek];
}

//得到今天是这月的第几周.
WORD CGFunc::GetWeekNums(SYSTEMTIME& lnowTime)
{
	WORD wWeeks=0;
	int nDay = lnowTime.wDay - lnowTime.wDayOfWeek-1;
	if(nDay <= 0 )
		wWeeks = 1;
	else {
		wWeeks = nDay / 7;
		if(nDay % 7) 
			wWeeks ++;
		wWeeks++;
	}
	return wWeeks;
}



LONGLONG CGFunc::GetDayNums(SYSTEMTIME& lnowTime,ST_RSTASK& Temp)
{
	SYSTEMTIME tmTask;
	memcpy(&tmTask,&lnowTime,sizeof(tmTask));

	tmTask.wYear = Temp.task_time.start_time.wYear;
	tmTask.wMonth = Temp.task_time.start_time.wMonth;
	tmTask.wDay = Temp.task_time.start_time.wDay;

	CTime t2(lnowTime);
	CTime t1(tmTask);

	LONGLONG lTimeSpan = (t2 - t1).GetDays();
	return lTimeSpan;
}

WORD CGFunc::GetNextMonth(WORD wMonths,WORD wCrtMonth,WORD& uYear)
{
	for(WORD i=wCrtMonth;i<=12;i++)
	{
		if(GetMonthMark(i) & wMonths)
			return i;
	}
	uYear ++;
	for(WORD i=0;i<=wCrtMonth;i++)
	{
		if(GetMonthMark(i) & wMonths)
			return i;
	}

	return -1;
}

WORD CGFunc::GetNextWeek(WORD wWeeks,WORD wCrtWeek,int& nNums)
{
	for(WORD i = wCrtWeek;i<7;i++)
	{
		if(wWeeks &  GetWeekMark(i))
			return i;
	}

	nNums ++;
	for(WORD i=0;i<7;i++)
	{
		if(wWeeks &  GetWeekMark(i))
			return i;		
	}
	return -1;
}

void CGFunc::GetTaskPath(TCHAR * lpFile,int nLen )
{
	RsGetCurentPath(lpFile,nLen);
}

BOOL CGFunc::CheckRunOneShot(SYSTEMTIME& lnowTime,  ST_RSTASK& Temp)
{
	SYSTEMTIME tmTask;
	memcpy(&tmTask,&lnowTime,sizeof(tmTask));
	TASKTIME *pTaskTime =& (Temp.task_time) ;
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
		return TRUE;
	}	
	return FALSE;
}

BOOL CGFunc::CheckStartAndStopTime(SYSTEMTIME& lnowTime,  ST_RSTASK& Temp)
{
	TASKTIME *pTaskTime =& Temp.task_time;

	if(Temp.task_attrib.wUse == 0)
		return FALSE;

	SYSTEMTIME tmTask;
	memcpy(&tmTask,&lnowTime,sizeof(tmTask));

	tmTask.wYear = Temp.task_time.start_time.wYear;
	tmTask.wMonth = Temp.task_time.start_time.wMonth;
	tmTask.wDay = Temp.task_time.start_time.wDay;

	CTime t2(lnowTime);
	CTime t1(tmTask);
	LONGLONG lTimeSpan = (t2 - t1).GetDays();

	if(lTimeSpan < 0)
		return FALSE;

	if(pTaskTime->stop_time.wYear&& pTaskTime->stop_time.wMonth&&
		pTaskTime->stop_time.wDay)
	{
		tmTask.wYear = Temp.task_time.stop_time.wYear;
		tmTask.wMonth = Temp.task_time.stop_time.wMonth;
		tmTask.wDay = Temp.task_time.stop_time.wDay;
		CTime t0(tmTask);
		LONGLONG lSpan = (t2 - t0).GetDays();
		if(lSpan>=0)
			return FALSE;   
	}
	return TRUE;
}


BOOL CGFunc::GetNextStartTime(ST_RSTASK* pTask)
{
   return GetNextStartTime(pTask,& (pTask->task_time.next_time));
}

BOOL CGFunc::GetRunWeekTime(LONG lDays,ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime)
{
	SYSTEMTIME ctm;
	memset(&ctm,0,sizeof(ctm));
	ctm.wYear = pTask->task_time.start_time.wYear;
	ctm.wMonth = pTask->task_time.start_time.wMonth;
	ctm.wDay = pTask->task_time.start_time.wDay;

	CTime t1(ctm);
	CTimeSpan t2(lDays,0,0,0);
	CTime tNew = t1+t2;
	pNextStartTime->wYear = tNew.GetYear();
	pNextStartTime->wMonth = tNew.GetMonth();
	pNextStartTime->wDay = tNew.GetDay();
	pNextStartTime->wHour = pTask->task_time.every.every_week.wHour;
	pNextStartTime->wMinute =  pTask->task_time.every.every_week.wMinute;
	return TRUE;
}

BOOL  CGFunc::SumNextWeekTime(LONG lDays,SYSTEMTIME& lnowTime,ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime)
{
	//去掉本天可能运行的
	TASKTIME *pTaskTime =(TASKTIME *)&(pTask->task_time);
	if(pTaskTime->every.every_week.wWeekMark & GetWeekMark(lnowTime.wDayOfWeek)&&
		(pTaskTime->every.every_week.wHour < lnowTime.wHour || 
		(pTaskTime->every.every_week.wHour == lnowTime.wHour && pTaskTime->every.every_week.wMinute < lnowTime.wMinute )
		) )
	{
		pNextStartTime->wYear = lnowTime.wYear;
		pNextStartTime->wMonth = lnowTime.wMonth;
		pNextStartTime->wDay = lnowTime.wDay;
		pNextStartTime->wHour = pTaskTime->every.every_week.wHour;
		pNextStartTime->wMinute =  pTaskTime->every.every_week.wMinute;	
		return TRUE;
	}
	int nWk =0;
	WORD wNewWeek = GetNextWeek(pTaskTime->every.every_week.wWeekMark,lnowTime.wDayOfWeek,nWk);
	if(wNewWeek == -1) return -1;
	LONG tmpDays = 0;
	if(nWk == 0 && pTaskTime->every.every_week.uInterval_Week==1)
	{
		//还在本周
		tmpDays = lDays + wNewWeek - lnowTime.wDayOfWeek;
	}
	else
	{
		if(pTaskTime->every.every_week.uInterval_Week == 1)
			tmpDays = lDays+ wNewWeek + (7-lnowTime.wDayOfWeek);
		else if(pTaskTime->every.every_week.uInterval_Week > 1)
			tmpDays = lDays+ wNewWeek + (7-lnowTime.wDayOfWeek) + (pTaskTime->every.every_week.uInterval_Week-1)*7;
	}

	return	GetRunWeekTime(tmpDays,pTask,pNextStartTime);

}

//判断日期是否合法
static BOOL CheckDay(SYSTEMTIME* pNextStartTime)
{
	const int DAY[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (pNextStartTime->wYear > 1999 && pNextStartTime->wYear <2100 
		&& pNextStartTime->wMonth > 0 && pNextStartTime->wMonth <= 12
		&& pNextStartTime->wDay > 0 && pNextStartTime->wMonth <= 31)
	{
		if (pNextStartTime->wMonth == 2 && ((pNextStartTime->wYear%100)?((pNextStartTime->wYear%4)?FALSE:TRUE):((pNextStartTime->wYear%400)?FALSE:TRUE)))
		{
            if (pNextStartTime->wDay <= 29)
				return TRUE;
		}
		else
		{
			if (pNextStartTime->wDay <= DAY[pNextStartTime->wMonth])
				return TRUE;
		}
	}
	return FALSE;
}

static BOOL IsOverTime(SYSTEMTIME& systime, SYSTEMTIME& cacltime)
{
	if (systime.wYear > cacltime.wYear)
		return TRUE;
	if (systime.wYear < cacltime.wYear)
		return FALSE;

	if (systime.wMonth > cacltime.wMonth)
		return TRUE;
	if (systime.wMonth < cacltime.wMonth)
		return FALSE;

	if (systime.wDay > cacltime.wDay)
		return TRUE;
	if (systime.wDay < cacltime.wDay)
	    return FALSE;

	if (systime.wHour > cacltime.wHour)
		return TRUE;
	if (systime.wHour < cacltime.wHour)
		return FALSE;

	if (systime.wMinute > cacltime.wMinute)
		return TRUE;
	if (systime.wMinute < cacltime.wMinute)
		return FALSE;

	if (systime.wSecond > cacltime.wSecond)
		return TRUE;
	if (systime.wSecond < cacltime.wSecond)
		return FALSE;

	return TRUE;
}

BOOL CGFunc::GetNextStartTimeByMonth(SYSTEMTIME*pNextStartTime, const EVERYMONTH& everymonth)
{
	memset(pNextStartTime, 0, sizeof(SYSTEMTIME));

	SYSTEMTIME lnowTime;
	memset(&lnowTime,0,sizeof(lnowTime));
	GetLocalTime(&lnowTime);
    SYSTEMTIME tTemp;
	memcpy(&tTemp, &lnowTime, sizeof(lnowTime));
	tTemp.wHour = everymonth.wHour;
	tTemp.wMinute = everymonth.wMinute;
	tTemp.wSecond = 0;

    for (int i=0; i<14; i++)
	{
		if (everymonth.wMonthMark & GetMonthMark(tTemp.wMonth))
		{
			if (everymonth.wDay)
			{
               tTemp.wDay = everymonth.wDay;
			}
			else
			{
				//取到当月1号是星期几
				CTime t5(tTemp.wYear, tTemp.wMonth, 1, 0, 0, 0);
				WORD wTmpWeek = t5.GetDayOfWeek()-1;
                tTemp.wDay = ((7-wTmpWeek+everymonth.wDayOfWeek)%7)+(everymonth.wWeekNum-1)*7 + 1;
			}
            if (CheckDay(&tTemp))
			{
				 //算时间过了吗？
				 if (!IsOverTime(lnowTime, tTemp))
				 {
                     memcpy(pNextStartTime, &tTemp, sizeof(SYSTEMTIME));
                     return TRUE;
				 }			            
			}
		}
        tTemp.wMonth++;
		if (tTemp.wMonth > 12)
		{
            tTemp.wMonth = 1;
            tTemp.wYear++;
		}
	}
    return FALSE;
}

BOOL CGFunc::GetNextStartTime(ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime)
{
	SYSTEMTIME lnowTime;
	memset(&lnowTime,0,sizeof(lnowTime));
	GetLocalTime(&lnowTime);
	if(pTask == NULL )
		return FALSE;

    if(pNextStartTime)
	{
		memset(pNextStartTime,0,sizeof(SYSTEMTIME));
	}

	if(! CheckStartAndStopTime(lnowTime,(*pTask)))
	{
		memset(pNextStartTime,0,sizeof(SYSTEMTIME));
		return FALSE;
	}

	TASKTIME *pTaskTime =& pTask->task_time;
	WORD wCrtMonth = lnowTime.wMonth;

	switch(pTask->task_attrib.uType)
	{
	case START_TASK_TYPE_MONTH:
		{
			GetNextStartTimeByMonth(pNextStartTime, pTaskTime->every.every_month);
			break;

			pNextStartTime->wYear = lnowTime.wYear;
			if(pTaskTime->every.every_month.wDay )
			{
				if(pTaskTime->every.every_month.wMonthMark &  GetMonthMark(wCrtMonth) &&
					(lnowTime.wDay < pTaskTime->every.every_month.wDay || (lnowTime.wDay == pTaskTime->every.every_month.wDay &&
					(lnowTime.wHour < pTaskTime->every.every_month.wHour ||	(lnowTime.wMinute < pTaskTime->every.every_month.wMinute &&lnowTime.wHour == pTaskTime->every.every_month.wHour)) 
					)))
				{
					//当月会启动 年月日不变
					pNextStartTime->wMonth = lnowTime.wMonth;
				}
				else
				{
					//下月的几号才启动 ,要算出年月
					if(wCrtMonth == 12){
						wCrtMonth =1;
						pNextStartTime->wYear ++;
					}
					else{
						wCrtMonth ++;
					}
					//取下月或者当月是否满足
					pNextStartTime->wMonth = GetNextMonth(pTaskTime->every.every_month.wMonthMark, 
						wCrtMonth,pNextStartTime->wYear);
					if(pNextStartTime->wMonth == -1){
						memset(pNextStartTime,0,sizeof(SYSTEMTIME));
						return FALSE;
					}
				}
				pNextStartTime->wDay = pTaskTime->every.every_month.wDay;
				pNextStartTime->wHour = pTaskTime->every.every_month.wHour;
				pNextStartTime->wMinute = pTaskTime->every.every_month.wMinute;
				pNextStartTime->wSecond = 0;
				return TRUE;
			}
			else if(pTaskTime->every.every_month.wWeekNum )
			{
				WORD wWeekNum = GetWeekNums(lnowTime);
				if( pTaskTime->every.every_month.wMonthMark &  GetMonthMark(lnowTime.wMonth)&&
					(wWeekNum<pTaskTime->every.every_month.wWeekNum  || 
					(wWeekNum==pTaskTime->every.every_month.wWeekNum && pTaskTime->every.every_month.wDayOfWeek > lnowTime.wDayOfWeek ) ||
					(wWeekNum==pTaskTime->every.every_month.wWeekNum && pTaskTime->every.every_month.wDayOfWeek == lnowTime.wDayOfWeek && lnowTime.wHour < pTaskTime->every.every_month.wHour) ||
					(wWeekNum==pTaskTime->every.every_month.wWeekNum && pTaskTime->every.every_month.wDayOfWeek == lnowTime.wDayOfWeek && lnowTime.wHour == pTaskTime->every.every_month.wHour &&lnowTime.wMinute < pTaskTime->every.every_month.wMinute ) 
					))
				{
					//当月会启动,算出日
					pNextStartTime->wMonth = lnowTime.wMonth;
					WORD wTmpDay = 0;
					if(wWeekNum==pTaskTime->every.every_month.wWeekNum)
					{
						wTmpDay = -lnowTime.wDayOfWeek+pTaskTime->every.every_month.wDayOfWeek;
					}
					else if(wWeekNum < pTaskTime->every.every_month.wWeekNum)
					{
						int nTmp = pTaskTime->every.every_month.wWeekNum - wWeekNum;
						wTmpDay = (7-lnowTime.wDayOfWeek + pTaskTime->every.every_month.wDayOfWeek) + (nTmp-1)*7;

					}
					pNextStartTime->wDay = lnowTime.wDay + wTmpDay;
				}
				else
				{  //到下月或再下月才会启动,算出年月日
					if(wCrtMonth == 12){
						wCrtMonth =1;
						pNextStartTime->wYear ++;
					}
					else{
						wCrtMonth ++;
					}
					//取下月或者当月是否满足
					pNextStartTime->wMonth = GetNextMonth(pTaskTime->every.every_month.wMonthMark, 
						wCrtMonth,pNextStartTime->wYear);
					if(pNextStartTime->wMonth == -1){
						memset(pNextStartTime,0,sizeof(SYSTEMTIME));
						return FALSE;
					}
					
					//取到当月1号是星期几
					SYSTEMTIME stoptime;
					memcpy(&stoptime,&lnowTime,sizeof(SYSTEMTIME));
					stoptime.wYear =pNextStartTime->wYear;
					stoptime.wMonth = pNextStartTime->wMonth;
					stoptime.wDay = 1;
					CTime t1(lnowTime);
					CTime t2(stoptime);
					LONGLONG lTimeSpan = (t2 - t1).GetDays();
					CTimeSpan t3(lTimeSpan-1,0,0,0);
					CTime t4 = t1+t3;
					WORD wTmpWeek  = t4.GetDayOfWeek();
					WORD wTmpDay = 0;
		
					//if(pTaskTime->every.every_month.wWeekNum>2)
					//	wTmpDay = (7-wTmpWeek) + pTaskTime->every.every_month.wDayOfWeek+1 + (pTaskTime->every.every_month.wWeekNum-2)*7;
					//else if(pTaskTime->every.every_month.wWeekNum ==2)
					//	wTmpDay = (7-wTmpWeek) + pTaskTime->every.every_month.wDayOfWeek+1;
					//else if(pTaskTime->every.every_month.wWeekNum ==1)
					//	wTmpDay = pTaskTime->every.every_month.wDayOfWeek+1;

                    wTmpDay = (7-wTmpWeek)+(pTaskTime->every.every_month.wWeekNum-1)*7;

					pNextStartTime->wDay = wTmpDay;
					
				}
				pNextStartTime->wHour = pTaskTime->every.every_month.wHour;
				pNextStartTime->wMinute = pTaskTime->every.every_month.wMinute;
				pNextStartTime->wSecond = 0;
			}
			break;
		}
	case START_TASK_TYPE_WEEK:
		{
			if(!(pTaskTime->every.every_week.wWeekMark & 0x7F) )
			{
				return FALSE;
			}

			LONGLONG days = GetDayNums(lnowTime,*pTask);
			LONGLONG nTmpDay = days- lnowTime.wDayOfWeek-1 ;
			if(nTmpDay>0)
			{
				//计算间隔了的周数?
				int nWeeks =1;
				nWeeks += (INT)nTmpDay/7;
				//判断是否到了间隔周
				int nWk = nWeeks%pTaskTime->every.every_week.uInterval_Week;
				if( nWk == 0 )
				{
					return SumNextWeekTime(days,lnowTime,pTask,pNextStartTime);
				}
				else
				{	
					WORD wRunWeek;
					for(wRunWeek = 0; wRunWeek<7;wRunWeek++ )
					{
					    if(pTaskTime->every.every_week.wWeekMark & GetWeekMark(wRunWeek))
						{
							break;
						}
					}
					int nNN = pTaskTime->every.every_week.uInterval_Week-nWk-1;
					days =days +(7- lnowTime.wDayOfWeek);
					days += wRunWeek;
					if(nNN>0){
						days += 7*nNN;
					}
					GetRunWeekTime(days,pTask,pNextStartTime);
					return TRUE;
				}
			}
			else
			{
				return SumNextWeekTime(days,lnowTime,pTask,pNextStartTime);
			}
			return FALSE;
			break;
		}
	case START_TASK_TYPE_DAY:
		{
			DWORD dwDays = GetDayNums(lnowTime,*pTask) % pTaskTime->every.every_day.uInterval_Day;

			if(dwDays == 0 &&
			   (lnowTime.wHour < pTaskTime->every.every_day.wHour ||
			   (lnowTime.wHour == pTaskTime->every.every_day.wHour && lnowTime.wMinute < pTaskTime->every.every_day.wMinute)
			   ) )
			{
				dwDays = 0;// pTaskTime->every.every_day.uInterval_Day;
			}
			else
			{
				dwDays = pTaskTime->every.every_day.uInterval_Day- dwDays;
			}

			SYSTEMTIME ctm;
			memset(&ctm,0,sizeof(ctm));
			ctm.wYear = lnowTime.wYear;
			ctm.wMonth = lnowTime.wMonth;
			ctm.wDay = lnowTime.wDay;

			CTime t1(ctm);
			CTimeSpan t2(dwDays,0,0,0);
			CTime tNew = t1+t2;
			pNextStartTime->wYear = tNew.GetYear();
			pNextStartTime->wMonth = tNew.GetMonth();
			pNextStartTime->wDay = tNew.GetDay();
			pNextStartTime->wHour = pTaskTime->every.every_day.wHour;
			pNextStartTime->wMinute =  pTaskTime->every.every_day.wMinute;		

			break;
		}
	case START_TASK_TYPE_HOUR:
		{
			/*
			//需task服务内部时间m_starttime,要去查询服务
			SYSTEMTIME ctm;
			memset(&ctm,0,sizeof(ctm));
			memcpy(&ctm,&lnowTime,sizeof(ctm));
			ctm.wMinute =0;
			ctm.wSecond = 0;
			ctm.wMilliseconds =0;

			CTime t1(m_starttime);
			CTime t2(ctm);
			CTimeSpan t3 = t2-t1;
			WORD whour = t3.GetHours() % pTaskTime->every.every_hour.uInterval_Hour ;

			CTime tCrt(ctm);
			CTimeSpan t4(0,pTaskTime->every.every_hour.uInterval_Hour-whour,0,0);

			CTime tNew = tCrt + t4;

			pNextStartTime->wYear = tNew.GetYear();
			pNextStartTime->wMonth = tNew.GetMonth();
			pNextStartTime->wDay = tNew.GetDay();
			pNextStartTime->wHour = tNew.GetHour();
			pNextStartTime->wMinute = pTaskTime->every.every_hour.wMinute;
			*/

			break;
		}
	case START_TASK_TYPE_INTERVAL_HOUR:
		{
			/*
			//需task服务内部时间m_dwIntervalHour,m_dwIntervalSecond,要去查询服务
			WORD wHour = m_dwIntervalHour % pTaskTime->every.interval_hour;
			WORD wMinute = (m_dwIntervalSecond /60);
			WORD wSecond = m_dwIntervalSecond % 60;
			CTimeSpan t1(0,wHour,wMinute,wSecond);
			CTimeSpan t2(0,pTaskTime->every.interval_hour,0,0);
			CTimeSpan t3 = t2-t1;

			CTime tCrt(lnowTime);
			CTime tNew = tCrt + t3;
			pNextStartTime->wYear = tNew.GetYear();
			pNextStartTime->wMonth = tNew.GetMonth();
			pNextStartTime->wDay = tNew.GetDay();
			pNextStartTime->wHour = tNew.GetHour();
			pNextStartTime->wMinute = tNew.GetMinute();
			*/
			break;
		}
	case START_TASK_TYPE_RUNONESHOT:
		{
			if( ! CheckRunOneShot(lnowTime,*pTask ))
			{
				pNextStartTime->wYear = pTaskTime->every.oneshot_time.date.wYear;
				pNextStartTime->wMonth = pTaskTime->every.oneshot_time.date.wMonth;
				pNextStartTime->wDay = pTaskTime->every.oneshot_time.date.wDay;
				pNextStartTime->wHour = pTaskTime->every.oneshot_time.wHour;
				pNextStartTime->wMinute = pTaskTime->every.oneshot_time.wMinute;
			}
			break;
		}
	default:
		break;
	}
	return FALSE;

}