#include "stdafx.h"
#include "P2PDownloader.h"
#include "UrlToRid.h"
#include "P2PConfiger.h"

#include "downloader.h"
#include "json\json.h"


// -------------------------------------------------------------
// class CMsHttpDownloader
#define HTTP_INTERNET_AGENT_NAME        L"p2p_http_down_agent"
#define P2P_HTTP_MAX_BUFFER_SIZE        1024

CMsHttpDownloader::CMsHttpDownloader(CMsBaseDownloaderContext* context)
	: CMsBaseDownloader(context, Mod_Http)
	, m_http(context)
	, m_filepath(NULL)
{}

CMsHttpDownloader::~CMsHttpDownloader()
{
	Stop();
}

int CMsHttpDownloader::Init()
{
	// get file save dir to 'm_savepath'
	// get file name to 'm_file'
	return CMsBaseDownloader::Init();
}

int CMsHttpDownloader::Uninit()
{
	return CMsBaseDownloader::Uninit();
}

int CMsHttpDownloader::Start()
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());

	std::wstring file = pContext->SavePath();
	if(m_filepath == NULL && pContext->FileName() != NULL)
	{
		file += pContext->FileName();
	}
	else
	{
		// parse url to get file path
		const wchar_t* url = pContext->Url();
		const wchar_t* p = wcsrchr(url, L'/');
		if(p == NULL)
		{
			return -1;
		}

		file += p + 1;
	}

	return ERR_HTTP_SUCCESS == m_http.Download(pContext->Url(), file.c_str()) ?
		0 : -1;
}

int CMsHttpDownloader::Stop()
{
	m_http.StopDownload();

	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());
	BOOL bRemoveFile = FALSE;
	pContext->GetOpt(Opt_DownRemoveFile, &bRemoveFile, sizeof(bRemoveFile));
	if(bRemoveFile)
	{
		std::wstring file = pContext->SavePath();
		file += pContext->FileName();
		if(!DeleteFile(file.c_str()))
		{
			DeleteFile((file + L".rs").c_str());
		}
	}
	return 0;
}

int CMsHttpDownloader::Pause()
{
	return Stop();
}

int CMsHttpDownloader::Resume()
{
	//if(ERR_HTTP_SUCCESS == m_http.Download(static_cast<CMsP2PHttpDownloaderContext*>(GetContext())->Url(), 
	//	m_filepath))
	//{
	//	return static_cast<CMsP2PHttpDownloaderContext*>(GetContext())->ResetTimer();
	//}

	return Start();
}

int CMsHttpDownloader::Timeout()
{
	return 0;
}


// -------------------------------------------------------------
// class CMsP2PDownloader
CMsP2PDownloader::CMsP2PDownloader(CMsBaseDownloaderContext* context)
	: CMsBaseDownloader(context, Mod_P2P)
	, m_lpTrackers(NULL)
	, m_nTrackerSize(0)
{}

CMsP2PDownloader::~CMsP2PDownloader()
{}

int CMsP2PDownloader::Init()
{
	return CMsBaseDownloader::Init();
}

int CMsP2PDownloader::Uninit()
{
	return CMsBaseDownloader::Uninit();
}

int CMsP2PDownloader::Start()
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());

	CVectorString trackers = static_cast<CMsP2PSetting*>(pContext->Setting())->TrackerList();
	pContext->SetOpt(Opt_DownTrackerList, (void*)&trackers, trackers.size());

	StartDownloadWithRIDW(pContext->Rid(), pContext->TrackerList(), pContext->UrlList(), pContext->SavePath());
	return 0;
}

int CMsP2PDownloader::Stop()
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());
	BOOL bRemoveFile = FALSE;
	pContext->GetOpt(Opt_DownRemoveFile, &bRemoveFile, sizeof(bRemoveFile));
	StopDownload(pContext->Rid(), bRemoveFile);
	return 0;
}

int CMsP2PDownloader::Pause()
{
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());
	//PauseDownload(pContext->Rid());
	StopDownload(pContext->Rid(), FALSE);
	return 0;
}

int CMsP2PDownloader::Resume()
{
	// 当 pause 时未加入 downloader 的 p2p 列表, resume 时就会出错. 
	CMsP2PHttpDownloaderContext* pContext = static_cast<CMsP2PHttpDownloaderContext*>(GetContext());
	//ResumeDownload(pContext->Rid());
	StartDownloadWithRIDW(pContext->Rid(), pContext->TrackerList(), pContext->UrlList(), pContext->SavePath());
	
	pContext->ResetTimer();
	return 0;
}

int CMsP2PDownloader::Timeout()
{
	return 0;
}

void CMsP2PDownloader::StartDownloadWithRIDW(const unsigned char* rid, const CVectorString& trackersw, const CVectorString& urlsw, const wchar_t* savepathw)
{
	assert(trackersw.size() > 0);							// size check before
	int i = 0;
	std::vector<std::string> trackersa;
	std::vector<std::string> urlsa;

	// add http url
	std::string stra;
	for(CVectorString::const_iterator it = urlsw.begin(); 
		it != urlsw.end(); it++)
	{
		stra.clear();
		CP2PClientTool::WideToMulti(it->c_str(), stra);
		urlsa.push_back(stra);
	}

	// add trackers
	for(CVectorString::const_iterator it = trackersw.begin(); 
		it != trackersw.end(); it++)
	{
		stra.clear();
		CP2PClientTool::WideToMulti(it->c_str(), stra);
		trackersa.push_back(stra);
	}

	// savepath
	stra.clear();
	CP2PClientTool::WideToMulti(savepathw, stra);

	// start p2p down
	StartDownloadWithRIDx(rid, trackersa, urlsa, stra);
}


// -------------------------------------------------------------
// class CMsDownloaderContext
int CMsP2PHttpDownloaderContext::LoadConfiger(const char* app)
{
	CConfiger* configer = static_cast<CMsP2PSetting*>(m_setting)->Configer();
	if(0 != configer->Load())
	{
		return -1;
	}

	//configer->Get(app, "state",    m_state);
	configer->Get(app, "mode",       m_mode);
	configer->Get(app, "filesize",   m_filelen);
	configer->Get(app, "downsize",   m_downsize);
	configer->Get(app, "downtime",   (int&)m_totaltimes);
	configer->Get(app, "upsize",     m_upsize);
	configer->Get(app, "uptime",     m_uptimes);
	configer->Get(app, "filename",   m_file);
	configer->Get(app, "savepath",   m_savepath);
	configer->Get(app, "url",        m_url);
	configer->Get(app, "mchange",    m_mchange);

	configer->Get(app, "result",     m_result);
	configer->Get(app, "removefile", m_removefile);

	// urlhash be setted outside
	// rid
	configer->Get(app, "rid", m_srid);
	CP2PClientTool::FromHex(m_srid.c_str(), m_srid.length(), m_rid.hash);

	int count = 0;
	configer->Get(app, "trackercount", count);
	for(int i = 0; i < count; i++)
	{
		std::wstring wstr;
		char key[256]  = {0};
		sprintf_s(key, SIZEOFV(key), "tracker%d", i);
		configer->Get(app, key, wstr);
		m_trackers.push_back(wstr);
	}

	count = 0;
	configer->Get(app, "urlcount", count);
	for(int i = 0; i < count; i++)
	{
		std::wstring wstr;
		char key[256]  = {0};
		sprintf_s(key, SIZEOFV(key), "url%d", i);
		configer->Get(app, key, wstr);
		m_urls.push_back(wstr);
	}

	// if downer is not completion. reset 'result'
	if(!Finish()) m_result = Res_Waiting;
	return 0;
}

int CMsP2PHttpDownloaderContext::SaveConfiger(const char* app, bool update)
{
	CConfiger* configer = static_cast<CMsP2PSetting*>(m_setting)->Configer();

	// add time
	if(!update && m_result > Res_Waiting && m_result < Res_Finish)
	{
		m_totaltimes += (GetTickCount() - m_starttime)/1000;
	}

	//configer->Set(app, "state",    m_state);
	configer->Set(app, "mode",       m_mode);
	configer->Set(app, "filesize",   m_filelen);
	configer->Set(app, "downsize",   m_downsize);
	configer->Set(app, "downtime",   m_totaltimes);
	configer->Set(app, "upsize",     m_upsize);
	configer->Set(app, "uptime",     m_uptimes);
	configer->Set(app, "filename",   m_file);
	configer->Set(app, "savepath",   m_savepath);
	configer->Set(app, "url",        m_url);
	configer->Set(app, "mchange",    m_mchange);

	configer->Set(app, "result",     m_result);
	configer->Set(app, "removefile", m_removefile);

	char hash[P2P_HASH_SIZE * 2 + 1] = {0};
	CP2PClientTool::ToHex(m_rid.hash, sizeof(m_rid.hash), hash);
	configer->Set(app, "rid", std::string(hash));
	CP2PClientTool::ToHex(m_urlhash.hash, sizeof(m_urlhash.hash), hash);
	configer->Set(app, "urlhash", std::string(hash));

	int i = 0;
	configer->Set(app, "trackercount", (int)m_trackers.size());
	for(CVectorString::const_iterator it = m_trackers.begin(); 
		it != m_trackers.end(); it++, i++)
	{
		char key[256] = {0};
		sprintf_s(key, SIZEOFV(key), "tracker%d", i);
		configer->Set(app, key, *it);
	}

	i = 0;
	configer->Set(app, "urlcount", (int)m_urls.size());
	for(CVectorString::const_iterator it = m_urls.begin(); 
		it != m_urls.end(); it++, i++)
	{
		char key[256] = {0};
		sprintf_s(key, SIZEOFV(key), "url%d", i);
		configer->Set(app, key, *it);
	}

	return 0;
}

int CMsP2PHttpDownloaderContext::SetOpt(int opt, void* data, size_t len)
{
	VariantType_t vt(data, len);

	switch(opt)
	{
	case Opt_DownState:
		// -- for http downloader is finished, it will auto to stop. when user call stop, 
		//    we will not handle it. so add http mode, stop to delete file. 
		if(Mod_Http == m_mode && vt.ival == State_Stop)
		{
			if(m_removefile && !m_file.empty())
			{
				std::wstring file = m_savepath + m_file;
				if(!DeleteFile(file.c_str()))
				{
					DeleteFile((file + L".rs").c_str());
				}
			}
		}

		// status tranfer check
		// context have not real status.
		if(Mod_Unknown == m_mode || !CMsBaseDownloader::StateCheck(vt.ival, m_state))
		{
			// 如果此时文件下载状态不符，则开始下载 ???
			// 仅仅处理下载完成后，使用者删除文件，但是没有停止（P2P），再次开始下载的时候
			if(CheckFileState(vt.ival))
			{
				MSP2PLOG_CONTEXT(this, MSP2PLOG_ERROR, "state change error! [%d --> %d] ***, %s", 
					m_state, vt.ival, Mod_Unknown == m_mode ? "mode is unknown." : "state check.");
				return -1;
			}

			CMsBaseDownloaderContext::SetOpt(Opt_DownState, (void*)State_Stop, sizeof(State_Stop));
		}

		// because of 'start' and 'resume' is synchronize, create new thread 
		if(State_Downloading == vt.ival || State_Resume == vt.ival)
		{
			// 'm_state is stop' and 'thread is runing' ==> 'thread block', http.
			if(State_Unknown != m_state && NULL != m_thread && m_thread->Running())
			{
				MSP2PLOG_CONTEXT(this, MSP2PLOG_ERROR, "start failed, it is blocking in stop, please wait ...");
				// here, danger... may memory leak.or dead lock.
				// m_thread->Stop();
				if(!Finish()) m_result = Res_StopBlock;
				static_cast<CMsP2PSetting*>(m_setting)->Manager()->SetDownError(P2P_BLOCK_IN_STOP);
				return -1;
			}

			if(!Finish()) m_result = Res_Downing;
			m_state = vt.ival;
			return StartDownThread();
		}

		break;

	case Opt_DownMode:
		if(NULL != m_downer && m_mode != vt.ival)
		{
			assert(Mod_Unknown != m_mode);
			SetEvent(Event_DestoryDowner);
			m_file.clear();   // reset p2p file name.
		}

		m_mode = vt.ival;
		return 0;

	case Opt_DownUrl:
		if(m_url.empty())
		{
			m_url = vt.sval;
		}

		m_urls.push_back(vt.sval);
		return 0;

	case Opt_DownUrlHash:
		assert(len == P2P_HASH_SIZE);
		memcpy(m_urlhash.hash, data, len);
		return 0;

	case Opt_DownRid:
		assert(len == P2P_HASH_SIZE);
		memcpy(m_rid.hash, data, len);
		return 0;

	case Opt_P2PTracker:
	case Opt_DownTracker:
		m_trackers.push_back(vt.sval);
		return 0;

	case Opt_P2PTrackerList:
	case Opt_DownTrackerList:
		for(CVectorString::const_iterator it1 = ((CVectorString*)data)->begin(); 
			it1 != ((CVectorString*)data)->end(); it1++)
		{
			m_trackers.push_back(*it1);
		}

		return 0;

	case Opt_DownUrlsList:
		for(CVectorString::const_iterator it = ((CVectorString*)data)->begin();
			it != ((CVectorString*)data)->end(); it++)
		{
			m_urls.push_back(*it);
		}
		return 0;

	case Opt_DownFileLen:
		ResetTimer();
		GetDownFileName();
		break;

	case Opt_RenameFileName:
		//if(m_mode == Mod_P2P)
		//{
		//	CopyP2PFile();
		//}
		return 0;

	case Opt_DownModeChange:
		m_mchange = vt.ival;
		return 0;

	case Opt_DownResult:
		if(vt.ival == Res_Error)
		{
			if(m_result >= Res_Error) return 0;
		}
		else if(vt.ival == Res_ReFinish)
		{
			if(m_file.empty()) GetDownFileName();
			if(m_filelen == 0) GetDownFileSize();
		}
		break;
	}

	return CMsBaseDownloaderContext::SetOpt(opt, data, len);
}

int CMsP2PHttpDownloaderContext::GetOpt(int opt, void* data, size_t len)
{
	switch(opt)
	{
	case Stat_FileName:
	case Opt_DownFileName:
		GetDownFileName();
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_file.c_str());
		return 0;

	case Stat_FileLength:
	case Opt_DownFileLen:
		GetDownFileSize();
		*(int*)data = m_filelen;
		return 0;

	case Opt_DownUrl:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_url.c_str());
		return 0;

	case Opt_DownUrlHash:
		assert(len >= P2P_HASH_SIZE);
		memcpy(data, m_urlhash.hash, len);
		return 0;

	case Opt_DownRid:
		assert(len >= P2P_HASH_SIZE);
		memcpy(data, m_rid.hash, len);
		return 0;

	case Opt_DownTracker:
		if(m_trackers.size() == 0)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PHttpDownloaderContext::GetOpt(), No Tracker.");
			return -1;
		}
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_trackers[0].c_str());
		return 0;

	case Opt_DownTrackerList:
		for(CVectorString::const_iterator it = m_trackers.begin(); 
			it != m_trackers.end(); it++)
		{
			((CVectorString*)data)->push_back(*it);
		}
		return 0;

	case Opt_DownUrlsList:
		for(CVectorString::const_iterator it = m_urls.begin(); 
			it != m_urls.end(); it++)
		{
			((CVectorString*)data)->push_back(*it);
		}
		return 0;

	case Stat_DownloadTimes:
		*(int*)data = m_pdowntimes;
		return 0;
	}

	return CMsBaseDownloaderContext::GetOpt(opt, data, len);
}

int CMsP2PHttpDownloaderContext::CreatDownloader()
{
	switch(m_mode)
	{
	case Mod_Http:
		m_downer = new CMsHttpDownloader(this);
		break;

	case Mod_P2P:
		if(!IsValidHash(m_rid))
		{
			MSP2PLOG(MSP2PLOG_NOTICE, "CMsP2PHttpDownloaderContext::CreatDownloader(), 'rid' invalid.");
			return -1;
		}

		if(TrackerList().size() == 0 && 
			static_cast<CMsP2PSetting*>(m_setting)->TrackerList().size() == 0)
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PHttpDownloaderContext::CreatDownloader(), No Tracker.");
			return -1;
		}

		m_downer = new CMsP2PDownloader(this);
		break;

	case Mod_P2SP:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::CreatDownloader(), " \
			"'Mod_P2SP' is no implement.");
		break;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::CreatDownloader(), " \
			"Unknown Downloader Mode(%d).", m_mode);
		break;
	}

	return m_downer != NULL ? 0 : -1;
}

int CMsP2PHttpDownloaderContext::DestroyDownloader()
{
	if(m_downer == NULL)
	{
		return 0;
	}

	if(0 != CMsBaseDownloaderContext::DestroyDownloader())
	{
		return -1;
	}

	switch(m_mode)
	{
	case Mod_Http:
		delete static_cast<CMsHttpDownloader*>(m_downer);
		break;

	case Mod_P2P:
		delete static_cast<CMsP2PDownloader*>(m_downer);
		break;

	case Mod_P2SP:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::DestroyDownloader(), " \
			"'Mod_P2SP' is no implement.");
		break;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::DestroyDownloader(), " \
			"Unknown Downloader Mode(%d).", m_mode);
		break;
	}

	m_downer = NULL;
	return 0;
}

int CMsP2PHttpDownloaderContext::DestroyDownloader(int mode)
{
	if(m_downer == NULL)
	{
		return 0;
	}

	switch(mode)
	{
	case Mod_Http:
		delete static_cast<CMsHttpDownloader*>(m_downer);
		break;

	case Mod_P2P:
		delete static_cast<CMsP2PDownloader*>(m_downer);
		break;

	case Mod_P2SP:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::DestroyDownloader(), " \
			"'Mod_P2SP' is no implement.");
		break;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsP2PHttpDownloaderContext::DestroyDownloader(), " \
			"Unknown Downloader Mode(%d).", mode);
		break;
	}

	m_downer = NULL;
	return 0;
}

int CMsP2PHttpDownloaderContext::GetFileNameFromUrl(std::wstring & filename)
{
	assert(m_url.c_str() != NULL);
	const wchar_t* name = wcsrchr(m_url.c_str(), L'/');
	if(name != NULL)
	{
		filename = name + 1;
		return 0;
	}

	return -1;
}


int CMsP2PHttpDownloaderContext::CopyP2PFile()
{
	assert(m_mode == Mod_P2P);
	// here, file name may is http_mode. 
	std::wstring file;
	{
		size_t len = 0;
		GetState(m_rid.hash, (DownloaderState)Stat_FileName, NULL, len);
		std::string name(len, 0);
		GetState(m_rid.hash, (DownloaderState)Stat_FileName, &name[0], len);
		CP2PClientTool::MultiToWide(name.c_str(), file);
	}

	if(m_file.size() == 0)
	{
		m_file = file;
	}

	std::wstring realname;
	if(0 == GetFileNameFromUrl(realname))
	{
		std::wstring src = m_savepath + file;
		std::wstring dst = m_savepath + realname;

		unsigned int times = 0;
		while(times < 1000)
		{
			if(CP2PClientTool::FindFileW(dst.c_str()))   break;
			if(CopyFile(src.c_str(), dst.c_str(), TRUE)) break;

			// 此处增加 p2p 下载完成后重命名失败处理
			std::wstring tmp = src + L"rs";
			if(CP2PClientTool::FindFileW(tmp.c_str())) 
			{
				MoveFileW(tmp.c_str(), src.c_str());
			}

			Sleep(100);
			times += 100;
			MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PHttpDownloaderContext::CopyP2PFile(), src = %S, dst = %S, err = %d", 
				src.c_str(), dst.c_str(), GetLastError());
		}
	}

	return 0;
}

bool CMsP2PHttpDownloaderContext::FindDownloadFile()
{
	return CP2PClientTool::FindFileW((m_savepath + L"\\" + m_file).c_str());
}

bool CMsP2PHttpDownloaderContext::CheckFileState(int state)
{
	if(State_Downloading == state && Mod_P2P == m_mode && 
		Finish()) return FindDownloadFile();
	return true;
}

int  CMsP2PHttpDownloaderContext::GetDownFileName()
{
	if(!m_file.empty()) return 0;

	if(m_mode == Mod_P2P)
	{
		size_t len = 0;
		GetState(m_rid.hash, (DownloaderState)Stat_FileName, NULL, len);
		std::string name(len, 0);
		GetState(m_rid.hash, (DownloaderState)Stat_FileName, &name[0], len);
		CP2PClientTool::MultiToWide(name.c_str(), m_file);
	}

	return 0;
}

int  CMsP2PHttpDownloaderContext::GetDownFileSize()
{
	// 不能对未下载完成任务的文件长度计算. 
	if(m_savepath.empty()) return -1;
	if(m_filelen > 0 || 
		!Finish())         return 0;

	if(m_file.empty())     GetDownFileName();
	if(m_file.empty())     return -2;

	m_filelen = CP2PClientTool::GetFileSize((m_savepath + L"\\" + m_file).c_str());

	return 0;
}


// -------------------------------------------------------
/*
 * 上报url：http://u.Margin.cn/p2p/upinfo.php?info=[%info%]
 *          %info%=｛rid:”rid信息”,speed:[speednum],downtime:[downtime]｝
 *          “pro”:”MSA”,”ver”:”01.00.00.82”
 *          P2P下载成功，则
 *          %info%=｛”p2pflag”:1,                     “rid”:”rid信息”,    “speed”:[speednum],“downtime”:[downtime]｝
 *          HTTP下载成功，则
 *          %info%=｛”p2pflag”:0, “url”:”url信息”,“urlhash”:”urlhash”,“speed”:[speednum],“downtime”:[downtime],“cause”:[原因标识int]｝
 *          说明: cause  0:代表未获取到rid, 1:代表连接超时，2:代表速度太低
 *          返回：无；
 */
int CMsP2PHttpDownloaderContext::Report()
{
	// json
	Json::Value jsroot;

	// product id, version, module version.
	std::string stmp;
	CP2PClientTool::WideToMulti(static_cast<CMsP2PSetting*>(m_setting)->m_productid.c_str(), stmp);
	if(stmp.size() > 0)
	{
		jsroot["pro"] = stmp;
		stmp.clear();
	}

	CP2PClientTool::WideToMulti(static_cast<CMsP2PSetting*>(m_setting)->m_pversion.c_str(), stmp);
	if(stmp.size() > 0)
	{
		jsroot["ver"] = stmp;
		stmp.clear();
	}

	CP2PClientTool::WideToMulti(static_cast<CMsP2PSetting*>(m_setting)->m_mversion.c_str(), stmp);
	if(stmp.size() > 0)
	{
		jsroot["p2pver"] = stmp;
		stmp.clear();
	}

	char hash[P2P_HASH_SIZE * 2 + 1] = {0};
	if(m_mode == Mod_P2P)
	{
		jsroot["p2pflag"] = 1;

		CP2PClientTool::ToHex(m_rid.hash, P2P_HASH_SIZE, hash);
		jsroot["rid"] = hash;
	}
	else if(m_mode == Mod_Http)
	{
		jsroot["p2pflag"] = 0;

		std::string urla;
		CP2PClientTool::WideToMulti(m_url.c_str(), urla);
		jsroot["url"] = urla.c_str();

		CP2PClientTool::ToHex(m_urlhash.hash, P2P_HASH_SIZE, hash);
		jsroot["urlhash"] = hash;

		assert(m_mchange > 0);
		jsroot["cause"] = m_mchange - 1;
	}
	else
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsP2PHttpDownloaderContext::Report(), Unknown Down Mode = %d", m_mode);
		return -1;
	}

	// down type check
	int type = static_cast<CMsP2PSetting*>(m_setting)->Report();
	if(Rep_TaskInfo & type)
	{
		// ..
	}

	if(Rep_DownInfo & type)
	{
		jsroot["speed"] = m_downrate;
		jsroot["downtime"] = m_totaltimes;
	}

	if(Rep_UpInfo   & type)
	{
		jsroot["upsize"] = m_upsize;
		jsroot["upspeed"] = m_uprate;
		jsroot["uptime"] = m_uptimes;
	}

	// encrypt, widecode
	std::string  eninfoa;
	CP2PClientTool::Encrypt(jsroot.toStyledString().c_str(), eninfoa);
	std::wstring eninfow;
	CP2PClientTool::MultiToWide(eninfoa.c_str(), eninfow);

	// report
	std::wstring reporturl = static_cast<CMsP2PSetting*>(m_setting)->ReportUrl();;
	reporturl += L"?info=" + eninfow;

#ifdef _DEBUG
	MSP2PLOG_CONTEXT(this, MSP2PLOG_TEST, "report, size = %d, speed = %d, time = %d, url = '%S'", 
		m_filelen, m_downrate, m_downtimes, reporturl.c_str());
#endif

	CHttpDownload http;
	http.Download(reporturl.c_str());

	return 0;
}


// -------------------------------------------------------
// thread task
int CMsP2PHttpDownloaderContext::StartDownThread()
{
	if(m_thread == NULL)
	{
		m_thread = new CSimpleThread(this);
	}

	MSP2PLOG_CONTEXT(this, MSP2PLOG_TEST, "start thread, thread is %s ...", m_thread != NULL && m_thread->Running() ? "running" : "stop");
	return (m_thread != NULL && !m_thread->Running()) ? m_thread->Start() : -1;
}

int CMsP2PHttpDownloaderContext::Task()
{
	assert(m_url.c_str() != NULL && IsValidHash(m_urlhash));
	assert(Mod_Unknown != m_mode);

	// 开始失败要加停止通知. ???
	if(!IsEvent(Event_StartNotify))
	{ 
		SetEvent(Event_StartNotify);
		MSP2PLOG_CONTEXT(this, MSP2PLOG_TEST, "Event_StartNotify ...");

		ResetPtime();
		static_cast<CMsP2PSetting*>(m_setting)->Manager()->OnNotify(
			m_urlhash.hash, Msg_StartDownload, m_state == State_Downloading ? 0 : 1, 0);
	}

	if(IsEvent(Event_Redowner))
	{
		if(m_type == Type_UrlHash)
		{
			// if delete m_downer, libtorrent_callback to 'manager -> pid -> context'
			// rid, not support.
			assert(Mod_P2P == m_mode);
			m_rid = *(P2PHash_t*)&NullUrlHash;
			m_mchange = Change_No;
			SetEvent(Event_DestoryDowner);
			ResetEvent(Event_Redowner);
		}
		else if(m_type == Type_Rid) 
			return -1;
	}

	if(IsEvent(Event_DestoryDowner))
	{
		assert(NULL != m_downer);
		DestroyDownloader(m_downer->Key());
		ResetEvent(Event_DestoryDowner);
	}

	if(NULL == m_downer && 0 != CreateDownMode())
	{
		m_result = Res_Error;
		return -1;
	}

	return m_downer->UpdateState();
}

int CMsP2PHttpDownloaderContext::CreateDownMode()
{
	switch(m_mode)
	{
	case Mod_P2P:
		if(0 == IsValidP2PMode())
		{
			static_cast<CMsP2PSetting*>(m_setting)->Manager()->InsertRidHashMap(m_rid.hash, m_urlhash.hash);
		}
		else
		{
			m_mode = Mod_Http;
			m_mchange = Change_NoRid;
			static_cast<CMsP2PSetting*>(m_setting)->Manager()->OnNotify(this, Msg_ModeChange, Mod_Http, NULL);
		}

		break;
	}

	if(m_mchange > 0) assert(m_mode != Mod_P2P);
	MSP2PLOG_CONTEXT(this, MSP2PLOG_TEST, "create mode, reason = %d ...", m_mchange);
	return CreatDownloader();
}

int CMsP2PHttpDownloaderContext::IsValidP2PMode()
{
	assert(Mod_P2P == m_mode);
	if(IsValidHash(m_rid))
	{
		return 0;
	}

	CP2PRid cpr(static_cast<CMsP2PSetting*>(m_setting)->RidUrl());
	if(0 == cpr.GetRidByUrlHash(m_urlhash.hash, m_rid.hash, sizeof(m_rid.hash)))
	{
		#ifdef _DEBUG
			char srid[41] = {0};
			char surlhash[41] = {0};
			CP2PClientTool::ToHex(m_rid.hash, 20, srid);
			CP2PClientTool::ToHex(m_urlhash.hash, 20, surlhash);
			MSP2PLOG(MSP2PLOG_TEST, "Get 'Rid' OK, 'Rid' = '%s', 'UrlHash' = '%s'...", srid, surlhash);
		#endif

		std::wstring strw;
		CP2PClientTool::MultiToWide(cpr.GetTracker(), strw);
		if(strw.size() > 0)
		{
			SetOpt(Opt_DownTracker, (void*)strw.c_str(), strw.length());
		}

		std::vector<std::string> & urls = cpr.GetDownurl();
		for(std::vector<std::string>::const_iterator it = urls.begin();
			it != urls.end(); it++)
		{
			CP2PClientTool::MultiToWide(it->c_str(), strw);
			SetOpt(Opt_DownUrl, (void*)strw.c_str(), strw.length() * sizeof(wchar_t));
		}

		return 0;
	}

	return -1;
}
