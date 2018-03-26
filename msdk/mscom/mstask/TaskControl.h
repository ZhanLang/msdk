#pragma once
#include "mstask\imstask.h"
#include "xmldefine.h"
#include "msgtype.h"
#include "xmlmgr.h"
#include "GFunc.h"


class CTaskControl:public IRSTask,
				   public CGFunc,
				   public CUnknownImp
{
public:
	CTaskControl(void);
	virtual ~CTaskControl(void);

	UNKNOWN_IMP2_(ITASK,IRSTask);

	//rot й╣ож
	STDMETHOD(init_class)(IUnknown* prot, IUnknown* punkOuter){return S_OK;}

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

	STDMETHOD(UpdateXMLCfg)();

private:
	BOOL GetItem(DWORD dwIndex,ST_RSTASK& Task);
	HRESULT PutArray(ST_RSTASK* pTaskArray,DWORD dwCount,ST_RSTASK* pItem,DWORD dwIndex);

	BOOL InitEvent();
	BOOL GetXMLFile();
	void PostModifyXmlMsg(TASKMSG *lpMsg);
	void SendXmlMsg();

private:
	HANDLE     m_hEvent;
 
};
