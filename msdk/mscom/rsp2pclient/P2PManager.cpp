#include "stdafx.h"
#include "P2PManager.h"
#include "P2PDownloader.h"
#include "HttpDownloadEx.h"
#include "UrlToRid.h"

#include "downloader.h"
#include "json\json.h"

// 获取产品 ID 和产品版本
//#include <productpath\rspathbase.h>


// -------------------------------------------------------------
// class CMsP2PSetting
CMsP2PSetting::CMsP2PSetting(CMsP2PManager* manager)
	: CMsBaseDownloaderSetting()
	, m_manager(manager)
	, m_configer(true)
	, m_uspeed(0)
	, m_dspeed(0)
	, m_connects(0)
	, m_load(P2P_DEFUALT_LOAD_TYPE)
	, m_listenport(-1)
	, m_report(P2P_DEFAULT_REPORT_TYPE)
	, m_ridurl(P2P_DEFAULT_RID_URL)
	, m_reporturl(P2P_DEFAULT_REPORT_URL)
	, m_saveconf(P2P_DEFAULT_CONFIGER_TYPE)
{
	m_trackers.push_back(P2P_DEFALUT_TRACKER);
	CP2PClientTool::GetSelfModuleVersion(m_mversion);

	TCHAR stmp[MAX_PATH] = {0};
	DWORD sz = SIZEOFV(stmp);
	//GetMsProductType(stmp, sz);
	m_productid = stmp;
	sz = SIZEOFV(stmp);
	//GetMsProductVersion(stmp, sz);
	m_pversion = stmp;

	#ifdef _DEBUG
		MSP2PLOG(MSP2PLOG_TEST, "product = %S, product_version = %S, module_version = %S", 
			m_productid.c_str(), m_pversion.c_str(), m_mversion.c_str());
	#endif

	LoadSetting();
}

CMsP2PSetting::~CMsP2PSetting()
{
}

int CMsP2PSetting::LoadSetting()
{
	if(0 != m_configer.Load())
	{
		return -1;
	}

	m_configer.Get("client", "maxdownspeed",  m_dspeed);
	m_configer.Get("client", "maxupspeed",    m_uspeed);
	m_configer.Get("client", "maxconnectnum", m_connects);
	m_configer.Get("client", "listenport",    m_listenport);
	m_configer.Get("client", "timeout",       m_timeout);
	m_configer.Get("client", "msginterval",   m_interval);

	m_configer.Get("client", "savepath",      m_savepath);
	m_configer.Get("client", "reporturl",     m_reporturl);
	m_configer.Get("client", "ridurl",        m_ridurl);
	m_configer.Get("client", "hostname",      m_host);

	m_configer.Get("client", "autoupload",    m_load);
	m_configer.Get("client", "reporttype",    m_report);
	m_configer.Get("client", "configertype",  m_saveconf);

	int count = 0;
	m_configer.Get("trackers", "count",       count);
	for(int i = 0; i < count; i++)
	{
		std::wstring wstr;
		char key[256] = {0};
		sprintf_s(key, SIZEOFV(key), "tracker%d", i);
		m_configer.Get("trackers", key,       wstr);
		m_trackers.push_back(wstr);
	}

	return 0;
}

int CMsP2PSetting::SaveSetting()
{
	m_configer.Set("client", "maxdownspeed",  m_dspeed);
	m_configer.Set("client", "maxupspeed",    m_uspeed);
	m_configer.Set("client", "maxconnectnum", m_connects);
	m_configer.Set("client", "listenport",    m_listenport);
	m_configer.Set("client", "timeout",       m_timeout);
	m_configer.Set("client", "msginterval",   m_interval);

	m_configer.Set("client", "savepath",      m_savepath);
	m_configer.Set("client", "reporturl",     m_reporturl);
	m_configer.Set("client", "ridurl",        m_ridurl);
	m_configer.Set("client", "hostname",      m_host);

	m_configer.Set("client", "autoupload",    m_load);
	m_configer.Set("client", "reporttype",    m_report);
	m_configer.Set("client", "configertype",  m_saveconf);
	m_configer.Set("trackers", "count",       (int)m_trackers.size());

	int i = 0;
	for(CVectorString::const_iterator it = m_trackers.begin(); 
		it != m_trackers.end(); it++, i++)
	{
		char key[256] = {0};
		sprintf_s(key, SIZEOFV(key), "tracker%d", i);
		m_configer.Set("trackers", key, *it);
	}

	return m_configer.Save();
}

int CMsP2PSetting::SetOpt(int opt, void* data, size_t len)
{
	VariantType_t vt(data, len);

	switch(opt)
	{
	case Opt_P2PMaxUpSpeedInBps:
		m_uspeed = vt.ival;
		return 0;

	case Opt_P2PMaxDownSpeedInBps:
		m_dspeed = vt.ival;
		return 0;

	case Opt_P2PMaxConnectNum:
		m_connects = vt.ival;
		return 0;

	case Opt_P2PListenPort:
		m_listenport = vt.ival;
		return 0;

	case Opt_P2PAutoUpload:				// *** only golabol setting ***
		m_load = vt.ival;
		return 0;

	case Opt_P2PTracker:
		m_trackers.push_back(vt.sval);
		return 0;

	case Opt_P2PTrackerList:
		for(CVectorString::const_iterator it1 = ((CVectorString*)data)->begin(); 
			it1 != ((CVectorString*)data)->end(); it1++)
		{
			m_trackers.push_back(*it1);
		}

		return 0;

	case Opt_P2PHostName:
		m_host = vt.sval;
		GetProductionInfo();
		return 0;

	case Opt_P2PDataReport:
		m_report = vt.ival;
		return 0;

	case Opt_P2PReportUrl:
		m_reporturl = vt.sval;
		return 0;

	case Opt_P2PRidUrl:
		m_ridurl = vt.sval;
		return 0;

	case Opt_P2PSaveConfiger:
		m_saveconf = vt.ival;
		return 0;
	}

	return CMsBaseDownloaderSetting::SetOpt(opt, data, len);
}

int CMsP2PSetting::GetOpt(int opt, void* data, size_t len)
{
	switch(opt)
	{
	case Opt_P2PMaxUpSpeedInBps:
		*(int*)data = m_uspeed;
		return 0;

	case Opt_P2PMaxDownSpeedInBps:
		*(int*)data = m_dspeed;
		return 0;

	case Opt_P2PMaxConnectNum:
		*(int*)data = m_connects;
		return 0;

	case Opt_P2PListenPort:
		*(int*)data = m_listenport;
		return 0;

	case Opt_P2PAutoUpload:
		*(int*)data = m_load;
		return 0;

	case Opt_P2PTracker:
		if(m_trackers.size() == 0)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PSetting::GetOpt(), No Tracker.");
			return -1;
		}
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_trackers[0].c_str());
		return 0;

	case Opt_P2PTrackerList:
		for(CVectorString::const_iterator it = m_trackers.begin(); 
			it != m_trackers.end(); it++)
		{
			((CVectorString*)data)->push_back(*it);
		}
		return 0;

	case Opt_P2PHostName:
		if(m_host.size() == 0)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PSetting::GetOpt(), No Host Name.");
			return -1;
		}

		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_host.c_str());
		return 0;

	case Opt_P2PDataReport:
		*(int*)data = m_report;
		return 0;

	case Opt_P2PReportUrl:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_reporturl.c_str());
		return 0;

	case Opt_P2PRidUrl:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_ridurl.c_str());
		return 0;

	case Opt_P2PSaveConfiger:
		*(int*)data = m_saveconf;
		return 0;
	}

	return CMsBaseDownloaderSetting::GetOpt(opt, data, len);
}

int CMsP2PSetting::GetProductionInfo()
{
	// analyze hostname. 

	return 0;
}


// -------------------------------------------------------------
// class CBaseDownloaderSetting
CMsP2PManager* CMsP2PManager::m_manager = NULL;

// -------------
// public method
CMsP2PManager::CMsP2PManager()
	: m_setting(this)
	, m_notify(NULL)
	, m_init(false)
	, m_maxRunings(P2P_MAX_TASKS)
	, m_upscount(0)
	, m_upslimit(-1)
	, m_error(P2P_OK)
	, m_p2pmid(0)
	, m_updatesets(false)
{
}

CMsP2PManager::~CMsP2PManager()
{
}

STDMETHODIMP CMsP2PManager::Init()
{
	if(m_init)
	{
		return S_OK;
	}

	// p2p start
	StartUp();
	SetOption(Opt_MaxUpSpeedInBps,   &m_setting.m_uspeed);
	SetOption(Opt_MaxDownSpeedInBps, &m_setting.m_dspeed);
	SetOption(Opt_MaxConnectNum,     &m_setting.m_connects);
	if(-1 == m_setting.m_listenport)
	{
		m_setting.m_listenport = 11935;
	}
	SetOption(Opt_ListenPort,	     &m_setting.m_listenport);

	// p2p funciton callback
	SetFileLengthListener(P2PFileLengthListener);
	SetDownloadInfoListener(P2PDownloadInfoListener, m_setting.Interval());
	SetUploadInfoListener(P2PUploadInfoListener);
	SetFileDownloadCompleteListener(P2PFileDownloadCompleteListener);

	// http function callback
	CHttpDownloadEx::SetListener(HttpFileLengthListener, 
		HttpDownloadInfoListener, 
		HttpFileDownloadCompleteListener);

	m_init = true;

	// load configer
	if(Type_UpDownDisable == m_setting.LoadType())
	{
		MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PSetting::GetOpt(), Up and Down Type is Disable.");
		return S_OK;
	}

	LoadConfigerTasks();
	MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::Init(), Load Tasks, %d ...", m_upscount);
	return S_OK;
}

STDMETHODIMP CMsP2PManager::Uninit()
{
	MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::Uninit()...in");

	if(!m_init)
	{
		return E_FAIL;
	}

	// m_lsRunTasks, m_lsWaitTasks
	{
		CBaseLockHandler l(m_rclock);
		m_lsWaitTasks.clear();

		for(std::list<CMsBaseDownloaderContext*>::const_iterator it = m_lsRunTasks.begin(); 
			it != m_lsRunTasks.end(); it++)
		{
			CMsP2PHttpDownloaderContext *d = static_cast<CMsP2PHttpDownloaderContext*>(*it);
			d->SetOpt(Opt_DownState, (void*)State_Stop, 4);
		}

		m_lsRunTasks.clear();
	}

	// p2p clear
	ClearUp();

	// self thread exit. 
	CSimpleThread::Pool()->SafeStop();

	// manager clear
	// no delete downloader
	bool bSaveConf = IsSaveConfiger(true);

	for(MapUrlHashToDownloader_t::iterator it = m_mapHashToDowner.begin(); 
		it != m_mapHashToDowner.end(); it++)
	{
		int id = it->second->GetId();
		CMsP2PHttpDownloaderContext * p = static_cast<CMsP2PHttpDownloaderContext*>(it->second);

		if(id == -1 && bSaveConf)
		{
			SetConfigerId(p);
			id = p->GetId();
			assert(id != -1);

			char app[64] = {0};
			sprintf_s(app, SIZEOFV(app), "task%d", id);
			p->SaveConfiger(app);
		}

		delete p;
	}

	if(bSaveConf)
	{
		m_setting.Configer()->Set("client", "upslimit", (int)m_upslimit);
		m_setting.Configer()->Set("client", "taskcount", (int)m_p2pmid);
		m_setting.SaveSetting();
	}

	m_mapHashToDowner.clear();
	m_mapRidToHash.clear();

	m_init = false;
	MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::Uninit()...out");
	return S_OK;
}

STDMETHODIMP CMsP2PManager::SetOpt(P2POption option, void *data, size_t nlen)
{
	switch(option)
	{
	case Opt_P2PMaxUpSpeedInBps:
	case Opt_P2PMaxDownSpeedInBps:
	case Opt_P2PMaxConnectNum:
		if(m_init)
		{
			SetOption((DownloaderOption)option, &data);
		}
		break;

	case Opt_MaxRunings:
		m_maxRunings = *((int*)&data);
		return S_OK;

	case Opt_P2PUpsLimit:
		if(m_upslimit != *((int*)&data))
		{
			m_upslimit = *((int*)&data);
			if(m_upslimit != m_upscount) UpdateUploaders();
		}
		return S_OK;
	}

	return m_setting.SetOpt(option, data, nlen) == 0 ? S_OK : S_FALSE;
}

STDMETHODIMP CMsP2PManager::GetOpt(P2POption option, void *data, size_t nlen)
{
	if(!m_init)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::GetOpt(), No Init() ...");
		return S_FALSE;
	}

	switch(option)
	{
	#define Opt_LastError	0xFF
	case Opt_LastError:
		*(int*)data = GetDownError();
		return S_OK;

	case Opt_MaxRunings:
		*(int*)data = m_maxRunings;
		return S_OK;

	case Opt_P2PUpsLimit:
		*(int*)data = m_upslimit;
		return S_OK;
	}

	return m_setting.GetOpt(option, data, nlen) == 0 ? S_OK : S_FALSE;
}

STDMETHODIMP CMsP2PManager::SetNotify(IMSP2PClientNotify *data)
{
	m_notify = data;
	return S_OK;
}

STDMETHODIMP CMsP2PManager::OnNotify(const unsigned char* urlhash, NotifyMessageID atype, WPARAM wp, LPARAM lp)
{
	if(m_notify != NULL)
	{
		return m_notify->OnNotify(urlhash, atype, wp, lp);
	}

	return S_OK;
}

STDMETHODIMP CMsP2PManager::OnNotify(CMsBaseDownloaderContext* context, NotifyMessageID  atype, WPARAM wp, LPARAM lp)
{
	assert(context != NULL);
	if(m_notify != NULL/* && State_Downloading == context->State() && P2P_USERCANCEL != context->DownError()*/)
	{
		return m_notify->OnNotify(static_cast<CMsP2PHttpDownloaderContext*>(context)->UrlHash(), atype, wp, lp);
	}

	return S_OK;
}

STDMETHODIMP CMsP2PManager::Report(const unsigned char* urlhash)
{
	CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	if(d == NULL)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::Report(), No UrlHash!");
		return S_FALSE;
	}

	return d->Report() == 0 ? S_OK : S_FALSE;
}


STDMETHODIMP CMsP2PManager::GetState(const unsigned char *urlhash, StateMessageID statid, void *data, size_t len)
{
	if(!m_init)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::GetState(), No Init() ...");
		return E_FAIL;
	}

	if(IsAnyHash(*(P2PHash_t*)urlhash))
	{
		return GetAllState(urlhash, statid, data, len);
	}
	else
	{
		return GetOneState(urlhash, statid, data, len);
	}
}

STDMETHODIMP CMsP2PManager::SetState(const unsigned char* urlhash, DownloadOptionID statid, void *data, size_t len)
{
	if(IsAnyHash(*(P2PHash_t*)urlhash))
	{
		return SetAllState(urlhash, statid, data, len);
	}
	else
	{
		return SetOneState(urlhash, statid, data, len);
	}
}

STDMETHODIMP CMsP2PManager::GetAllState(const unsigned char *urlhash, StateMessageID statid, void *data, size_t len)
{
	switch(statid)
	{
	case Stat_DownloadSize:
	case Stat_DownloadRate:
	case Stat_UploadSize:
	case Stat_UploadRate:
		break;

	default:
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::SetAllState() tasks(%d), id = %d, state = %d", 
			m_mapHashToDowner.size(), *(int*)&statid, *(int*)&data);
		return E_FAIL;
	}

	for(MapUrlHashToDownloader_t::const_iterator it = m_mapHashToDowner.begin(); 
		it != m_mapHashToDowner.end(); it++)
	{
		int tmp = 0;
		CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(it->second);
		d->GetOpt(statid, &tmp, len);
		*(int*)&data += tmp;
	}

	return S_OK;
}

STDMETHODIMP CMsP2PManager::SetAllState(const unsigned char *urlhash, DownloadOptionID statid, void *data, size_t len)
{
	bool restart = false;
	switch(statid)
	{
	case Stat_Info:
		statid = Opt_DownState;

	case Opt_DownState:
		switch(*(int*)&data)
		{
		case State_Stop:
		case State_Pause:
			RcClear();
			break;
		}

	case Opt_DownRemoveFile:
		break;

	default:
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::SetAllState() tasks(%d), id = %d, state = %d", 
			m_mapHashToDowner.size(), *(int*)&statid, *(int*)&data);
		return E_FAIL;
	}

	for(MapUrlHashToDownloader_t::const_iterator it = m_mapHashToDowner.begin(); 
		it != m_mapHashToDowner.end(); it++)
	{
		CMsP2PHttpDownloaderContext * d = static_cast<CMsP2PHttpDownloaderContext*>(it->second);
		SetOneState(d->UrlHash(), statid, data, len);
	}

	return S_OK;
}

STDMETHODIMP CMsP2PManager::GetOneState(const unsigned char *urlhash, StateMessageID statid, void *data, size_t len)
{
	// one task
	CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	if(d == NULL)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::GetState(), No UrlHash(%s)!", CP2PClientTool::HashToString(*(P2PHash_t*)urlhash));
		return S_FALSE;
	}

	return d->GetOpt(statid, data, len) == 0 ? S_OK : S_FALSE;
}

STDMETHODIMP CMsP2PManager::SetOneState(const unsigned char* urlhash, DownloadOptionID statid, void *data, size_t len)
{
	assert(urlhash != NULL && !IsAnyHash(*(P2PHash_t*)urlhash));
	int nRet = 0;
	CMsBaseDownloaderContext *dnext = NULL;
	CMsP2PHttpDownloaderContext *d = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	assert(d != NULL);

	switch(statid)
	{
	case Opt_DownState:
		switch(*(int*)&data)
		{
		case State_Downloading:
		case State_Resume:
			// !RcPush(d) ???
			if(RcPush(d) && 0 != (nRet = d->SetOpt(statid, data, len)))
			{
				SetOneState(urlhash, statid, (void*)State_Finish, sizeof(State_Finish));
			}
			else
			{
				// if only 'start', task notify.
				d->ResetEvent(Event_CompleteNotify);
				d->ResetEvent(Event_StartNotify);
			}
			break;

		case State_Stop:
		case State_Pause:
		case State_Finish:
			// to rcpop before to stop task.
			dnext = RcPop(*(P2PHash_t*)d->UrlHash());

			if(State_Finish != *(int*)&data)
			{
				nRet = d->SetOpt(statid, data, len);
				if(d->IsEvent(Event_UploadersLimit))
				{
					d->ResetEvent(Event_UploadersLimit);
					m_upscount--;
				}
			}

			// laster than notify. 
			// else pause at this task(d)'s 99%, now pause at new task(dnext)'s 0%. ?
			if(d->IsEvent(Event_Report))
			{
				d->ResetEvent(Event_Report);
				Report(urlhash);
			}

			// later.
			if(d->IsEvent(Event_SaveConfiger))
			{
				d->ResetEvent(Event_SaveConfiger);
				assert(Res_Finish == d->DownResult());
				UpdateConfiger(d);

				m_updatesets = true;
			}

			break;
		}

		if(NULL != dnext)
		{
			// 它一定发生在一个任务完成时触发下一个任务时刻
			// 此时如果开始任务失败，则没有消息回调. 
			if(E_FAIL == SetOneState(reinterpret_cast<CMsP2PHttpDownloaderContext *>(dnext)->UrlHash(), 
				Opt_DownState, (void*)State_Downloading, sizeof(State_Downloading)))
			{
				OnNotify(dnext, Msg_StartDownload, 0x0, 0);
				OnNotify(dnext, Msg_FileComplete,  0x0, 0);
			}
		}

		// 配置回写检测流程: updatesets <- Event_SaveConfiger <- down_size. 
		if(m_updatesets && 
			IsSaveConfiger())
		{
			m_updatesets = false;

			// 由于在回写配置的时候使用了加密算法. 
			// 在下载完成一个保存一次配置的情况下, 当网速快, 下载文件多而小时, CPU 仍然会达到 90% 多. 
			// 修改为在当下载队列为空时, 进行回写配置. 
			m_setting.SaveSetting();
		}

		return 0 == nRet ? S_OK : E_FAIL;
	}

	return d->SetOpt(statid, data, len) == 0 ? S_OK : E_FAIL;
}

STDMETHODIMP CMsP2PManager::AddDownloader(const unsigned char* urlhash, int type, const P2PString url, const P2PString savepath)
{
	if(!IsValidHash(*(P2PHash_t*)urlhash))
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::AddDownloader(), 'UrlHash' is invalidate!");
		return S_FALSE;
	}

	// add downloader
	CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	if(d != NULL)
	{
		// task be stopped when state is unkown. : 
		// when configer's task be loads, and its state is unknown, we start a new duplicate task.
		if(d->State() == State_Stop/* || d->State() == State_Unknown*/)
		{
			if(d->Finish())
			{
				if(d->DownType() == Type_UrlHash) d->SetOpt(Opt_DownMode, (void*)Mod_P2P, sizeof(Mod_P2P));
				d->SetOpt(Opt_DownResult, (void*)Res_Waiting, sizeof(Res_Waiting));
				if(type != Type_Unknown) d->SetEvent(Event_Redowner);
				d->ResetEvent(Event_CompleteNotify);
			}

			d->ResetEvent(Event_StartNotify);
			MSP2PLOG_CONTEXT(d, MSP2PLOG_NOTICE, "add downloader %s!", d->Finish() ? "redown" : "restart");
			return S_OK;
		}

		SetDownError(d->Finish() ? P2P_DUPLICATE_COMPLETE : P2P_DUPLICATE_DOWNING);
		MSP2PLOG_CONTEXT(d, MSP2PLOG_ERROR, "Attempt to add a exist downloader!");
		return S_FALSE;
	}

	d = new CMsP2PHttpDownloaderContext(&m_setting);
	if(d == NULL)
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::AddDownloader(), Create new downloader fail!");
		return S_FALSE;
	}

	d->SetOpt(Opt_DownUrlHash, (void*)urlhash, P2P_HASH_SIZE);

	if(url != NULL)
	{
		d->SetOpt(Opt_DownUrl, (void*)url, wcslen(url));
	}

	if(Type_UrlHash == type)
	{
		d->SetOpt(Opt_DownMode, (void*)Mod_P2P, sizeof(Mod_P2P));
	}
	else if(Type_Http == type)
	{
		d->SetOpt(Opt_DownModeChange, (void*)Change_Http, sizeof(Change_Http));
		d->SetOpt(Opt_DownMode, (void*)Mod_Http, sizeof(Mod_Http));
	}
	else if(Type_Rid == type)
	{
		// this code is for no 'url', it use only 'rid' to download.
		// NOTICE: it may result in a error for changing 'downrid' when Init() load ini earlier than it.
		d->SetOpt(Opt_DownRid, (void*)urlhash, P2P_HASH_SIZE);
		d->SetOpt(Opt_DownMode, (void*)Mod_P2P, sizeof(Mod_P2P));
	}

	if(Type_Unknown != type) d->SetDownType(type);
	m_mapHashToDowner.insert(std::make_pair(*(P2PHash_t*)d->UrlHash(), d));
	assert(d->DownResult() == Res_Waiting);
	return S_OK;
}

STDMETHODIMP CMsP2PManager::DelDownloader(const unsigned char* urlhash)
{
	if(*(P2PHash_t*)AnyUrlHash == *(P2PHash_t*)urlhash)
	{
		return S_OK;
	}

	MapUrlHashToDownloader_t::iterator it = m_mapHashToDowner.find(*(P2PHash_t*)urlhash);
	if(it == m_mapHashToDowner.end())
	{
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PManager::DelDownloader(), Delete a no exist downloader!");
		return S_OK;
	}

	// not delete, noly stop. ???
	//CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(it->second);
	//m_mapHashToDowner.erase(it);
	//delete d;

	return S_OK;
}

CMsP2PManager* CMsP2PManager::GetManager()
{
	if(m_manager == NULL)
	{
		m_manager = new CMsP2PManager();
		assert(m_manager != NULL);
	}

	return m_manager;
}

void CMsP2PManager::DestroyManager()
{
	if(m_manager != NULL)
	{
		delete m_manager;
		m_manager = NULL;
	}
}

CMsBaseDownloaderContext* CMsP2PManager::GetDownloaderByUrlHash(const unsigned char* urlhash)
{
	if(urlhash == NULL)
	{
		return NULL;
	}

	MapUrlHashToDownloader_t::const_iterator it = m_mapHashToDowner.find(*(P2PHash_t*)urlhash);
	if(it != m_mapHashToDowner.end())
	{
		return it->second;
	}

	return NULL;
}

void CMsP2PManager::DeleteDownloaderByUrlHash(const unsigned char* urlhash)
{
	MapUrlHashToDownloader_t::const_iterator it = m_mapHashToDowner.find(*(P2PHash_t*)urlhash);
	if(it != m_mapHashToDowner.end())
	{
		m_mapHashToDowner.erase(it);
	}
}

CMsBaseDownloaderContext* CMsP2PManager::GetDownloaderByRid(const unsigned char* rid)
{
	MapDownloaderToHash_t::const_iterator it = m_mapRidToHash.find(*(P2PHash_t*)rid);
	if(it != m_mapRidToHash.end())
	{
		return GetDownloaderByUrlHash(it->second.hash);
	}

	return NULL;
}

const unsigned char* CMsP2PManager::GetUrlHashByRid(const unsigned char* rid)
{
	MapDownloaderToHash_t::const_iterator it = m_mapRidToHash.find(*(P2PHash_t*)rid);
	if(it != m_mapRidToHash.end())
	{
		return it->second.hash;
	}
	
	return NULL;
}

void CMsP2PManager::InsertRidHashMap(const unsigned char* rid, const unsigned char* urlhash)
{
	CBaseLockHandler lh(m_csRidToHash);
	m_mapRidToHash.insert(std::make_pair(*(P2PHash_t*)rid, *(P2PHash_t*)urlhash));
}

CMsBaseDownloaderSetting* CMsP2PManager::GetSetting()
{
	return &m_setting;
}

int CMsP2PManager::UpdateConfiger()
{
	// save .configer
	m_setting.Configer()->Set("client", "upslimit",  (int)m_upslimit);
	m_setting.Configer()->Set("client", "taskcount", (int)m_mapHashToDowner.size());
	int i = 0;
	for(MapUrlHashToDownloader_t::iterator it = m_mapHashToDowner.begin(); 
		it != m_mapHashToDowner.end(); it++, i++)
	{
		char app[64] = {0};
		sprintf_s(app, SIZEOFV(app), "task%d", i);
		static_cast<CMsP2PHttpDownloaderContext*>(it->second)->SaveConfiger(app, true);
	}

	m_setting.SaveSetting();
	return 0;
}

int  CMsP2PManager::LoadConfigerTasks()
{
	// 测试中得出, 大约 500 个任务配置文件大小为 550 K. 
	// 加密要求配置不能超过 1 M. 
	// 
	// 初始化时最大加载任务数为 MAX_LOAD_TASKS. 加载方式从后往前.  
	// 加载到的任务有效. 重新设置其 ID 号. 其余为无效, 
	// 在重新保存时被重写. 
	// 
	// 运行后不限制添加的任务数. 
	// 不处理任务数不连续造成实际加载小于 MAX_LOAD_TASKS 的情况. 
	// 

	#define MAX_LOAD_TASKS		64
	typedef std::pair<P2PHash_t, CMsP2PHttpDownloaderContext*> P2PHashTaskNode;
	typedef std::list<P2PHashTaskNode> ListP2PHashTask;


	if(-1 == m_upslimit) 
	{
		m_setting.Configer()->Get("client", "upslimit",  m_upslimit);
	}

	// 1 加载旧任务 ID. 
	ListP2PHashTask listTasks;
	int tasks = 0;
	m_setting.Configer()->Get("client", "taskcount", tasks);
	for(int i = 0; i < tasks; i++)
	{
		char    app[256] = {0};
		sprintf_s(app, SIZEOFV(app), "task%d", i);

		// create task
		std::string surlhash;
		m_setting.Configer()->Get(app, "urlhash", surlhash);
		P2PHash_t urlhash = {0};
		CP2PClientTool::FromHex(surlhash.c_str(), surlhash.length(), urlhash.hash);

		AddDownloader(urlhash.hash, Type_Unknown, NULL, NULL);
		CMsP2PHttpDownloaderContext* d = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash.hash));
		if(d == NULL)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PManager::Init(), load task failed, urlhash = %s", CP2PClientTool::HashToString(urlhash));
			continue;
		}

		// task load configer
		d->LoadConfiger(app);

		if(!IsValid(d))
		{
			DeleteDownloaderByUrlHash(urlhash.hash);
			delete d;
			continue;
		}

		listTasks.push_back(std::make_pair(urlhash, d));
	}

	// 2 重置 configer. 
	m_setting.Configer()->Clear();

	// 3 如果超过加载数量 从前往后删除已加载的任务
	while(listTasks.size() > MAX_LOAD_TASKS)
	{
		P2PHashTaskNode & t = listTasks.front();

		DeleteDownloaderByUrlHash(t.first.hash);
		delete t.second;

		listTasks.pop_front();
	}

	// 4 重置加载的 Task Id. 
	for(ListP2PHashTask::const_iterator it = listTasks.begin(); 
		it != listTasks.end(); it++)
	{
		P2PHashTaskNode & t = listTasks.front();
		SetConfigerId(t.second);
		UpdateConfiger(t.second);
	}

	// 5 运行 Task. 
	for(ListP2PHashTask::const_iterator it = listTasks.begin(); 
		it != listTasks.end(); it++)
	{
		P2PHashTaskNode & t = listTasks.front();
		StartConfigerTask(t.second);
	}

	return 0;
}

int CMsP2PManager::UpdateConfiger(CMsBaseDownloaderContext * context)
{
	int  nPid    = -1;
	char app[64] = {0};

	nPid = context->GetId();
	if(nPid == -1)
	{
		SetConfigerId(context);
		nPid = context->GetId();
		assert(nPid != -1);
	}

	sprintf_s(app, SIZEOFV(app), "task%d", nPid);
	static_cast<CMsP2PHttpDownloaderContext*>(context)->SaveConfiger(app, true);
	m_setting.Configer()->Set("client", "taskcount", (int)m_p2pmid);
	m_setting.Configer()->Set("client", "upslimit",  (int)m_upslimit);

	// m_setting.SaveSetting();
	return 0;
}

bool CMsP2PManager::IsValid(CMsBaseDownloaderContext * context)
{
	assert(context);

	BOOL valid = TRUE;
	switch(m_setting.LoadType())
	{
	case Type_AutoUp:
	case Type_DownDisable:
		valid = context->Finish();
		break;

	case Type_AutoDown:
	case Type_UpDisable:
		valid = !context->Finish();
		break;
	}

	// task validate check, only exist in Init(). 
	// Init() start earlier than all outside loads. no lock. 
	if(valid && context->Finish())
	{
		assert(NULL != context->FileName());
		std::wstring file = context->SavePath();
		if(Mod_Http == context->Mode() || 
			NULL == context->FileName() || 
			!CP2PClientTool::FindFileW((file + context->FileName()).c_str()))
		{
			valid = FALSE;
		}
	}

	return valid ? true : false;
}

int  CMsP2PManager::StartConfigerTask(CMsBaseDownloaderContext * context)
{
	assert(context);
	CMsP2PHttpDownloaderContext * p = static_cast<CMsP2PHttpDownloaderContext*>(context);

	if(p->Finish())
	{
		p->SetEvent(Event_CompleteNotify);
	}
	else
	{
		// for to download task, we use one down_url of 'itself'
		p->SetOpt(Opt_DownUrl, (void*)p->Url(), 6);
	}

	if(Mod_Unknown == p->Mode())
	{
		assert(!p->Finish());
		assert(Type_AutoUpDown == m_setting.LoadType() || 
			Type_AutoDown == m_setting.LoadType() ||
			Type_DownDisable == m_setting.LoadType());

		SetState(p->UrlHash(), Opt_DownMode, (void*)Mod_P2P, sizeof(Mod_P2P));
	}

	if(m_upslimit != -1 && m_upscount >= m_upslimit)
	{
		return 1;
	}

	m_upscount++;

	// start up task
	SetState(p->UrlHash(), Opt_DownState, (void*)State_Downloading, sizeof(State_Downloading));
	return 0;
}

void CMsP2PManager::SetConfigerId(CMsBaseDownloaderContext * context)
{
	assert(context);

	context->SetId(m_p2pmid);
	m_p2pmid++;
}

bool CMsP2PManager::IsSaveConfiger(bool bExit)
{
	bool bRes = false;

	switch(m_setting.ConfigerType())
	{
	case Type_TasksEmptySave:
		bRes = m_lsRunTasks.empty();
		break;

	case Type_TaskCompletionSave:
		bRes = true;
		break;

	case Type_ModuleExitSave:
		bRes = bExit;
		break;

	case Type_TasksNoSave:
		bRes = false;
		break;
	}

	return bRes;
}



// -------------------------------------------------------------
// private method
// common event callback
void CMsP2PManager::FileLengthListener(const unsigned char* urlhash, unsigned int nFileLength)
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	assert(pContext != NULL);

	if(pContext->IsEvent(Event_CompleteNotify)) return ;

	pContext->SetOpt(Opt_DownFileLen, (void*)nFileLength, sizeof(nFileLength));
	OnNotify(pContext, Msg_FileLength, NULL, nFileLength);
}

void CMsP2PManager::DownloadInfoListener(const unsigned char* urlhash, unsigned int uDownloadBytes, unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed)
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	assert(pContext != NULL);

	// finish, not notify
	// notice: when file be delete, it will not notify.
	pContext->UpdatePtime();
	if(pContext->IsEvent(Event_CompleteNotify)) return ;

	pContext->SetOpt(Opt_DownloadSize,  (void*)uDownloadBytes, sizeof(uDownloadBytes));
	pContext->SetOpt(Opt_DownloadTimes, (void*)uTimeUsed, sizeof(uTimeUsed));

	// timeout handle 
	if( Mod_P2P == pContext->Mode() && 
		P2P_USERCANCEL != pContext->DownError() && 
		(pContext->IsTimeout() || pContext->DownTooLower())
		)
	{
		MSP2PLOG_CONTEXT(pContext, MSP2PLOG_NOTICE, "timeout, mode change ...");

		// try 3
		BOOL bTry = FALSE;
		int  mchange = pContext->IsTimeout() ? Change_ConnectionTimeout : Change_SpeedTooLower;

		if(Change_ConnectionTimeout == mchange)
		{
			pContext->GetOpt(Opt_IsRetry, &bTry, 4);
		}

		if(!bTry)
		{
			pContext->SetOpt(Opt_DownRemoveFile, (void*)TRUE, sizeof(BOOL));
		}

		pContext->SetEvent(Event_RetryDownload);
		pContext->SetOpt(Opt_DownState, (void*)State_Stop, sizeof(State_Stop));

		if(!bTry)
		{
			pContext->SetOpt(Opt_DownRemoveFile, (void*)FALSE, sizeof(BOOL));
			pContext->SetOpt(Opt_DownMode, (void*)Mod_Http, sizeof(Mod_Http));
			pContext->SetOpt(Opt_DownModeChange, (void*)mchange, sizeof(mchange));
			OnNotify(pContext, Msg_ModeChange, Mod_Http, NULL);
		}

		// start task, but no add 'refcount'
		pContext->SetOpt(Opt_DownState, (void*)State_Downloading, sizeof(State_Downloading));
		return ;
	}

	OnNotify(pContext, Msg_DownloadInfo, uDownloadBytes, pContext->DownPtime());
}

void CMsP2PManager::UploadInfoListener(const unsigned char* urlhash, unsigned int uUploadBytes, unsigned int uUploadSpeedInBytes, unsigned int uTimeUsed)
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	assert(pContext != NULL);

	pContext->UpdatePtime();
	if(pContext->IsEvent(Event_CompleteNotify) && 
		(pContext->State() == State_Stop || pContext->State() == State_Pause)) return ;

	pContext->SetOpt(Opt_UploadSize,  (void*)uUploadBytes, sizeof(uUploadBytes));
	pContext->SetOpt(Opt_UploadRate,  (void*)uUploadSpeedInBytes, sizeof(uUploadSpeedInBytes));
	pContext->SetOpt(Opt_UploadTimes, (void*)uTimeUsed, sizeof(uTimeUsed));
	OnNotify(pContext, Msg_UploadInfo, uUploadBytes, pContext->RunPtime());
}

void CMsP2PManager::FileDownloadCompleteListener(const unsigned char* urlhash, unsigned int size)
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetDownloaderByUrlHash(urlhash));
	assert(pContext != NULL);

	// 判断完成消息是否冗余. 如果冗余*可能*需要忽略. 
	// 冗余后是否触发下一个任务的判断是在 setstate 中.
	pContext->UpdatePtime();
	if(pContext->IsEvent(Event_CompleteNotify))
	{
		if(size == -2)
		{
			assert(pContext->Finish());
			OnNotify(pContext, Msg_FileComplete, 4, NULL);
			return;
		}
	}

	if(pContext->IsEvent(Event_RetryDownload))
	{
		pContext->ResetEvent(Event_RetryDownload);
		return ;
	}

	if(size == -1)
	{
		// http down failed, or user cancel, maybe p2p.
		pContext->SetOpt(Opt_DownResult, (void*)Res_Error, sizeof(int));
	}
	else if(size == -2)
	{
		pContext->SetOpt(Opt_DownResult, (void*)Res_UserCancel, sizeof(int));
	}
	else if(size == 0)
	{
		// lost previous download info: configer ...
		// so: rate = 0, time = 0, or no set
		// no report
		pContext->SetOpt(Opt_DownResult, (void*)Res_ReFinish, sizeof(int));
	}
	else if(size > 0)
	{
		// 'filelenght' notification may be laster than 'filecomplete' notification
		// so set 'size' at here.
		pContext->SetOpt(Opt_DownloadSize,  (void*)size, sizeof(size));
		pContext->SetOpt(Opt_RenameFileName, (void*)TRUE, sizeof(BOOL));
		pContext->SetOpt(Opt_DownResult, (void*)Res_Finish, sizeof(int));

		pContext->SetEvent(Event_Report);
		pContext->SetEvent(Event_SaveConfiger);
	}

	if(!pContext->IsEvent(Event_CompleteNotify))
	{
		pContext->SetEvent(Event_CompleteNotify);

		int nRet = 0;
		if(pContext->Finish()) nRet = 1;
		else if(State_Stop  == pContext->State()) nRet = 2;
		else if(State_Pause == pContext->State()) nRet = 3;
		OnNotify(pContext, Msg_FileComplete, nRet, NULL);
		MSP2PLOG_CONTEXT(pContext, MSP2PLOG_TEST, "FileCompletion_Event ...Result = %d, Times = (%d, %d)...", nRet
			, pContext->DownPtime(), pContext->RunPtime());
	}

	// stop task ? continue task ?
	int state = State_Finish;
	if(size < 0 || Mod_Http == pContext->Mode() || 
		(-1 != m_upslimit && m_upscount >= m_upslimit))
	{
		state = State_Stop;
	}
	else
	{
		m_upscount++;
		pContext->SetEvent(Event_UploadersLimit);
	}

	SetState(urlhash, Opt_DownState, reinterpret_cast<void* &>(state), sizeof(state));
}


// p2p event callback
void CMsP2PManager::P2PFileLengthListener(const unsigned char* rid, unsigned int nFileLength)
{
	if(m_manager != NULL)
	{
		m_manager->FileLengthListener(m_manager->GetUrlHashByRid(rid), nFileLength);
	}
}

void CMsP2PManager::P2PDownloadInfoListener(const unsigned char* rid, unsigned int uDownloadBytes, unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed)
{
	if(m_manager != NULL)
	{
		m_manager->DownloadInfoListener(m_manager->GetUrlHashByRid(rid), uDownloadBytes, uDownloadSpeedInBytes, uTimeUsed);
	}
}

void CMsP2PManager::P2PUploadInfoListener(const unsigned char* rid, unsigned int uUploadBytes, unsigned int uUploadSpeedInBytes, unsigned int uTimeUsed)
{
	if(m_manager != NULL)
	{
		m_manager->UploadInfoListener(m_manager->GetUrlHashByRid(rid), uUploadBytes, uUploadSpeedInBytes, uTimeUsed);
	}
}

void CMsP2PManager::P2PFileDownloadCompleteListener(const unsigned char* rid, unsigned int size)
{
	if(m_manager != NULL)
	{
		m_manager->FileDownloadCompleteListener(m_manager->GetUrlHashByRid(rid), size);
	}
}


// http event callback
void CMsP2PManager::HttpFileLengthListener(const unsigned char* urlhash, unsigned int nFileLength)
{
	if(m_manager != NULL)
	{
		m_manager->FileLengthListener(urlhash, nFileLength);
	}
}

void CMsP2PManager::HttpDownloadInfoListener(const unsigned char* urlhash, unsigned int uDownloadBytes, 
									 unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed)
{
	if(m_manager != NULL)
	{
		m_manager->DownloadInfoListener(urlhash, uDownloadBytes, uDownloadSpeedInBytes, uTimeUsed);
	}
}

void CMsP2PManager::HttpFileDownloadCompleteListener(const unsigned char* urlhash, unsigned int size)
{
	if(m_manager != NULL)
	{
		m_manager->FileDownloadCompleteListener(urlhash, size);
	}
}


// -------------------------------------------------------------
// class CMsP2PManager::CRuningCount
int CMsP2PManager::RcRunningCount()
{
	CBaseLockHandler l(m_rclock);
	return int(m_lsRunTasks.size());
}

void CMsP2PManager::RcClear()
{
	CBaseLockHandler l(m_rclock);
	m_lsWaitTasks.clear();
	m_lsRunTasks.clear();
}

CMsBaseDownloaderContext* CMsP2PManager::RcPop(P2PHash_t & h)
{
	CBaseLockHandler l(m_rclock);

	// notice: when stop all, can not stop one by one.
	//         if do it, and this tasks is runing, 
	//         program process: 
	//         --> stop rt1.
	//         -->     pop  rt1 from runing_list, 
	//         -->     pop  wt2 form wait_list, 
	//         -->     push wt2 in runnig_list, 
	//         -->     start it.
	//         --> stop rt2
	//         -->     ...
	//         again and again ... all task.
	//         this is a long time.
	for(std::list<CMsBaseDownloaderContext*>::iterator it = m_lsWaitTasks.begin();
		it != m_lsWaitTasks.end(); it++)
	{
		if(0 == memcmp(static_cast<CMsP2PHttpDownloaderContext*>(*it)->UrlHash(), h.hash, sizeof(P2PHash_t)))
		{
			m_lsWaitTasks.erase(it);
			MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::RcPop(),  [Wait --]. [%d, %d] ...return.", 
				m_lsRunTasks.size(), m_lsWaitTasks.size());
			return NULL;
		}
	}

	// is pop ?
	bool find = false;
	for(std::list<CMsBaseDownloaderContext*>::iterator it = m_lsRunTasks.begin();
		it != m_lsRunTasks.end(); it++)
	{
		if(0 == memcmp(static_cast<CMsP2PHttpDownloaderContext*>(*it)->UrlHash(), h.hash, sizeof(P2PHash_t)))
		{
			find = true;
			m_lsRunTasks.erase(it);
			MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::RcPop(),  [Run  --]. [%d, %d] ...continue", 
				m_lsRunTasks.size(), m_lsWaitTasks.size());
			break;
		}
	}

	CMsBaseDownloaderContext* context = NULL;
	if(find && !m_lsWaitTasks.empty())
	{
		context = m_lsWaitTasks.front();
		m_lsWaitTasks.pop_front();
		//m_lsRunTasks.push_back(context);
		MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::RcPop(),  [Wait --]. [%d, %d] ...continue", 
			m_lsRunTasks.size(), m_lsWaitTasks.size());
	}

	// may to stop finish tasks, it no in m_rc. 
	// so 'assert()' failed
	//assert(context != NULL || (context == NULL && m_lsWaitTasks.empty()));
	return context;
}

bool CMsP2PManager::RcPush(CMsBaseDownloaderContext * context)
{
	CBaseLockHandler l(m_rclock);
	for(std::list<CMsBaseDownloaderContext*>::const_iterator it = m_lsWaitTasks.begin(); 
		it != m_lsWaitTasks.end(); it++)
	{
		if(context == *it)
		{
			return false;
		}
	}

	for(std::list<CMsBaseDownloaderContext*>::const_iterator it = m_lsRunTasks.begin(); 
		it != m_lsRunTasks.end(); it++)
	{
		if(context == *it)
		{
			return false;
		}
	}

	if(int(m_lsRunTasks.size()) >= m_maxRunings)
	{
		m_lsWaitTasks.push_back(context);
		MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::RcPush(), [Wait ++]. [%d, %d] ...", 
			m_lsRunTasks.size(), m_lsWaitTasks.size());
		return false;
	}
	else
	{
		m_lsRunTasks.push_back(context);
		MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PManager::RcPush(), [Run  ++]. [%d, %d] ...", 
			m_lsRunTasks.size(), m_lsWaitTasks.size());
		return true;
	}
}

void CMsP2PManager::UpdateUploaders()
{
	// 'upscount' is temp variable, for improve effective. 
	// it maybe cause 'm_upscount != m_upslimit'
	int upscount = m_upscount;
	for(MapUrlHashToDownloader_t::const_iterator it = m_mapHashToDowner.begin(); 
			it != m_mapHashToDowner.end(); it++)
	{
		CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(it->second);
		if(pContext->DownState() && (!pContext->Finish() || Mod_Http == pContext->Mode()))
		{
			continue;
		}

		if(upscount < m_upslimit || -1 == m_upslimit)
		{
			if(State_Stop == pContext->DownState() || State_Unknown == pContext->DownState())
			{
				assert(!pContext->IsEvent(Event_UploadersLimit));
				SetState(pContext->UrlHash(), Opt_DownState, (void*)State_Downloading, sizeof(State_Downloading));
				upscount++;
			}
		}
		else if(upscount > m_upslimit)
		{
			if(State_Downloading == pContext->DownState())
			{
				assert(pContext->IsEvent(Event_UploadersLimit));
				SetState(pContext->UrlHash(), Opt_DownState, (void*)State_Stop, sizeof(State_Stop));
				upscount--;
			}
		}
		else
		{
			break;
		}
	}

	assert(-1 == m_upslimit || m_upscount <= m_upslimit);
}
