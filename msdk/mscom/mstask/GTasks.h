#pragma once
#include "mstask\imstask.h"
#include "xmldefine.h"
#include "msgtype.h"
#include "xmlmgr.h"
#include "StartTask.h"
#include "GFunc.h"
#include "check.h"
#include "mstask\ScrDetectInfo.h"

#include <vector>
#include <string>

#ifndef tstring
#define tstring std::basic_string<TCHAR> ;
#endif

typedef CSafeArray<ST_RSTASK> START_TASKVECTOR;

class CGTasks :	public IRSTaskSrv,
				public CGFunc,
			    public CUnknownImp
{
public:
	CGTasks(void);
	virtual ~CGTasks(void);

	UNKNOWN_IMP2_(ITASK,IRSTaskSrv);
	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter)
	{return S_OK;}

	STDMETHOD(SetConfigFile)(const TCHAR * lpFileName);
	STDMETHOD(SetTask)(DWORD dwTaskIndex,DWORD dwSet) ;
	STDMETHOD(SetAllTasks)(DWORD dwSet);
	STDMETHOD(AddTask)(ST_RSTASK * lpTask) ;
	STDMETHOD(DeleteTask)(DWORD dwTaskIndex);
	STDMETHOD(ModifyTask)(ST_RSTASK * lpTask) ;
	STDMETHOD(GetTask)(DWORD dwTaskIndex,ST_RSTASK& lpTask) ;
	STDMETHOD(QueryTask)(DWORD dwTaskClassID,IN OUT ST_RSTASK* pTaskArray,IN OUT DWORD dwCount) ;
	STDMETHOD(ControlTask)(DWORD dwTaskIndex,DWORD dwControlID);
	STDMETHOD(ControlTasks)(DWORD dwControlID) ;
	STDMETHOD(QueryTaskStatus)(DWORD dwTaskIndex,SYSTEMTIME*pPrevStartTime, SYSTEMTIME*pNextStartTime) ;
	STDMETHOD(QueryTaskTime)(ST_RSTASK& Task);


	//提供给任务主程序,或服务使用的函数
	STDMETHOD(Run)();
	STDMETHOD(Stop)();
	STDMETHOD(CfgModify)();
	STDMETHOD(OnMessage)(DWORD msgid, LPVOID buffin, size_t cc);

public:
	//启动后,要写时间到xml
	DWORD ModifyTime(DWORD dwIndex,SYSTEMTIME*pTime);
	DWORD ModifyRunStaus(DWORD dwType,DWORD dwIndex,DWORD dwProcID);
	DWORD ModifyTimetoVertor(DWORD dwType,DWORD dwIndex,SYSTEMTIME*pTime);

	DWORD CheckTask();
	DWORD CheckAfterBootTask();
private:
	DWORD LoadXML(int type);
	int  GetVectorIndexByTask(START_TASKVECTOR& task_vector,DWORD dwIndex);
	BOOL CheckDontRunTask(SYSTEMTIME& lnowTime, ST_RSTASK& Temp);
	BOOL CheckCanRun(SYSTEMTIME& lnowTime, ST_RSTASK& Temp);
	BOOL GetXMLFile();
	void ReleaseXmlBuf(TASK_XML* pTaskXML);

	void SaveFile(int nType);

	BOOL IsIdleScanRun(ST_RSTASK& Temp);
	BOOL IsPowerCanDo();

	//精度高，可不用此函数，用XThread 类做
	//径SrvWrap的线程整合到一起
	UINT       m_uPeriod;
	int        m_nTimerID;
	DWORD StartTimerThread();
	DWORD StopimerThread();

private:
	CCheck m_check;
	CStartTask m_Start;

	TASK_XML*  m_pTaskXML;
	TASK_XML*  m_pUserXML;

	BOOL       m_bFinish;
	BOOL       m_bLoopTime1;
	//----------------------
	BOOL m_bSetup;
	BOOL m_bUpdate;
	DWORD m_dwUpdateSession;
	//屏保扫描
	DWORD m_dwTimeSum;
	DWORD m_dwSreenType;
	DWORD m_dwPreStatus;

	BOOL m_bStartSceenSaver;
	BOOL m_bStopSceenSaver;
	DWORD m_dwInSceenSaver;
	DWORD m_dwScreenSession;
	//用于记录每小时 几分的操作
	SYSTEMTIME m_starttime;

	//用于周期记时的程序
	DWORD m_dwIntervalSecond;
	DWORD m_dwIntervalHour;

	START_TASKVECTOR    m_VectorInterval;	//周期/定时 队列
	START_TASKVECTOR    m_VectorAfterBoot;	//启动后过一段时间运行	

	//BOOL m_bPowerStop;  //电池状态太低，不能启动空闲扫描和屏保扫描
	DWORD m_dwPowerPercent; //配置中的电量百分比，小于不启动空闲扫描和屏保扫描
};
