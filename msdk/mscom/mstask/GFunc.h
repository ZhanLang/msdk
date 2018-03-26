#pragma once
#include <atltime.h>
#include "mstask\imstask.h"
#include "xmldefine.h"
#include "xmlmgr.h"

#define CLEAR_POINT_LIST(X) \
	X.remove_pitems();\
	X.clear()

class CTaskXmlObj
{
public:
	CTaskXmlObj(TASK_XML* pTaskXML)
	{
		m_pTaskXML = pTaskXML;
	}
	~CTaskXmlObj(void)
	{
		if( m_pTaskXML)
		{	
			CLEAR_POINT_LIST(m_pTaskXML->task_list);
			delete m_pTaskXML;
			m_pTaskXML = NULL;
		}
	}
private:
	TASK_XML* m_pTaskXML;
};

#define AutoObject(X)  CTaskXmlObj   _taskObjXml(X)
#define AutoObject2(X)  CTaskXmlObj  _taskObjXml2(X)

typedef std::vector<UINT> INDEX_LIST; 

class CGFunc
{
public:
	CGFunc(void);
	~CGFunc(void);

	WORD GetMonthMark(WORD wMonth);
	WORD GetWeekMark(WORD wWeek);
	WORD GetWeekNums(SYSTEMTIME& lnowTime);
	LONGLONG GetDayNums(SYSTEMTIME& lnowTime,ST_RSTASK& Temp);

	WORD GetNextMonth(WORD wMonths,WORD wCrtMonth,WORD& uYear);
	WORD GetNextWeek(WORD wWeeks,WORD wCrtWeek,int& nNums);

	void GetTaskPath(TCHAR * lpFile,int nLen );
	BOOL GetNextStartTime(ST_RSTASK* pTask);
	BOOL CheckStartAndStopTime(SYSTEMTIME& lnowTime,  ST_RSTASK& Temp);
	BOOL GetNextStartTime(ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime);
    BOOL GetNextStartTimeByMonth(SYSTEMTIME*pNextStartTime, const EVERYMONTH& everymonth);
	BOOL CheckRunOneShot(SYSTEMTIME& lnowTime,  ST_RSTASK& Temp);


	DWORD CheckXmlFile(CXMLMgr& m_XMLMgr);
private:
	BOOL GetRunWeekTime(LONG lDays,ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime);
	BOOL SumNextWeekTime(LONG lDays,SYSTEMTIME& lnowTime,ST_RSTASK* pTask,SYSTEMTIME*pNextStartTime);

protected:
	TCHAR		m_szRsTaskFile[MAX_PATH];
	TCHAR		m_szUserTaskFile[MAX_PATH];
	CXMLMgr     m_XMLMgr;

};
