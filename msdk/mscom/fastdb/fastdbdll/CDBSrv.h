#pragma once
#include <mscom/msplugin.h>
#include "database/fastdb/ITaskDBFast.h"
#include "DBConn.h"
#include "SyncObject/AsynFunc.h"
#include <string>
#include <map>
using namespace std;

#include "Helper.h"

//字节数,是给定16倍
//size_t fileSize = initSize ? initSize : dbDefaultInitDatabaseSize;
//
//if (fileSize < indexSize*sizeof(offs_t)*4) {
//	fileSize = indexSize*sizeof(offs_t)*4;
//}
//fileSize = DOALIGN(fileSize, dbBitmapSegmentSize);
//有盘模式用这个
#ifdef _DEBUG
#define DEFAULT_INITDBSIZE  1  * 1024
#else
#define DEFAULT_INITDBSIZE  64  * 1024 
#endif
//无盘模式，可以用这个，大约320MB耗用。
//#define DEFAULT_INITDBSIZE  20 * 1024 *1024


#define DEFAULT_PORT    2017
#define MAX_PORT_COUNT  200


class CDBFastSrv : public ITaskDBFast,public CUnknownImp
{
public:
	CDBFastSrv(void);
	virtual ~CDBFastSrv(void);

	STDMETHOD(init_class)(IMSBase *prot, IMSBase *punkOuter);

	UNKNOWN_IMP2(ITaskDBFast,IMsPlugin);

	//IMsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	//ITaskDBFast
	STDMETHOD( CreateTask )(IN LPCWSTR szDBName,IN LPWSTR szTableName,OUT DWORD& dwPort) ;

	STDMETHOD( ExecDML )(IN LPCWSTR szDBName,LPCWSTR szSql) ;
	STDMETHOD( Execute )(IN LPCWSTR szDBName,LPCTSTR szSql,IGenericMS** ppRS ) ;

	STDMETHOD( CloseTask )(IN LPCWSTR szDBName) ;

private:
	HRESULT GetConn(IN LPCTSTR lpDBName,OUT IGenericDB** ppConn );
	HRESULT GetConnRemote(IN LPCTSTR lpDBName,OUT IGenericDB** ppConn );

	HRESULT DeleteAllFdb();

	HRESULT InitPortMap();
	HRESULT AllocOnePort(DWORD& dwPort,wstring& strDBName);
	HRESULT ReleaseOnePort(wstring& strDBName);
	BOOL	IsOccupanedPort(DWORD& dwPort);


	HRESULT SetUninited();
	BOOL IsUninited();
	

	
private:
	UTIL::com_ptr<IMscomRunningObjectTable> m_pRot;

private:
	Helper::CMyEvent				m_theExitEvent;
	DeclareDefaultAsynFunc;
	Helper::CMySemaphore	m_theMsgSem;  


private:
	Helper::CMyLockObj m_theLock;
	map<wstring,UTIL::com_ptr<IGenericDB>  > m_mapDBConnSet;
	map<DWORD ,wstring>						 m_mapDBPort;
	map<wstring,DWORD>						 m_mapDBPortGot;

	DWORD m_dwPortBase;


};
