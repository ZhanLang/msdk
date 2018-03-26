#ifndef __P2PClient_H__
#define __P2PClient_H__


#include <vector>
#include <p2pclient/IP2PClient.h>
#include "P2PClientTool.h"

class CMsP2PManager;
class CMsP2PClient : public IMSP2PClient, 
	private ::msdk::mscom::CUnknownImp_Outer
{
public:
	CMsP2PClient();
	~CMsP2PClient();

	UNKNOWN_IMP1(IMSP2PClient);

	STDMETHOD(Init)(void*);
	STDMETHOD(Uninit)();


public:
	STDMETHOD(init_class)(IUnknown *prot, IUnknown *punkOuter);


public:
	STDMETHOD(StartWithURL)(const P2PString url, const P2PString strSaveFilePathName, OUT unsigned char* urlhash);
	STDMETHOD(Pause)       (const unsigned char* urlhash);
	STDMETHOD(Resume)      (const unsigned char* urlhash);
	STDMETHOD(Stop)        (const unsigned char* urlhash, BOOL remove_files = false);

	// 任务实时状态获取
	STDMETHOD(GetStat)     (const unsigned char* urlhash, StateMessageID statid, void* data, int nlen);
	STDMETHOD(SetStat)     (const unsigned char* urlhash, StateMessageID statid, void* data, int nlen);

	// P2P 配置
	STDMETHOD(SetOption)(P2POption option, void * data, int nlen);
	STDMETHOD(GetOption)(P2POption option, void * data, int nlen);

	// P2P 下载事件通知
	STDMETHOD(SetNotify)(IMSP2PClientNotify * data);


private:
	CMsP2PManager*   m_manager;
	CCriticalSetionObject m_mlock;
};


#endif

