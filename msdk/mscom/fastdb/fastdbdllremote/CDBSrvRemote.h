#pragma once
#include <rscom/rsplugin.h>
#include <string>
#include <map>
using namespace std;
#include "../../include/ITaskDBFast.h"
#include "Helper.h"


class CDBFastSrvRemote : public ITaskDBFast,public CUnknownImp
{
public:
	CDBFastSrvRemote(void);
	virtual ~CDBFastSrvRemote(void);

	STDMETHOD(init_class)(IRSBase *prot, IRSBase *punkOuter);

	UNKNOWN_IMP2(ITaskDBFast,IRsPlugin);

	//IRsPlugin
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();

	//ITaskDBFast
	STDMETHOD( CreateTask )(IN LPCWSTR szDBName,IN LPWSTR szTableName,OUT DWORD& dwPort) ;

	STDMETHOD( ExecDML )(IN LPCWSTR szDBName,LPCWSTR szSql) ;
	STDMETHOD( Execute )(IN LPCWSTR szDBName,LPCTSTR szSql,IGenericRS** ppRS ) ;

	STDMETHOD( CloseTask )(IN LPCWSTR szDBName) ;

private:
	HRESULT GetConn(IN LPCTSTR lpDBName,OUT IGenericDB** ppConn );


private:
	UTIL::com_ptr<IRscomRunningObjectTable> m_pRot;

	Helper::CMyLockObj	m_theLock;
private:
	map<wstring,UTIL::com_ptr<IGenericDB> > m_mapDBConnSet;

};
