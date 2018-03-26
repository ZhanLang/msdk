#pragma once

#include <mscom/srvbase.h>
#include <softeng/softengdef.h>
#include <softeng/ISoftReportPerson.h>
#include <thread/threadex.h>
#include "SoftScan.h"
/*
	»Ìº˛…œ±®
*/

class CSoftReportPerson : public IMsPlugin, public IMsPluginRun , public CMsComBase<CSoftReportPerson>
{
public:
	UNKNOWN_IMP2_( IMsPlugin,IMsPluginRun);
	STDMETHOD(init_class)(IMSBase* pRot , IMSBase* pOuter)
	{
		m_pRot = pRot;
		return S_OK;
	}

	CSoftReportPerson(void);
	~CSoftReportPerson(void);

protected:
	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();
	STDMETHOD(Start)();
	STDMETHOD(Stop)();

private:
	VOID Do();

private:
	CSoftScan m_softScan;
	CThreadEx m_worker;
};

