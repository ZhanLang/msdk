#ifndef __P2PDownloader_H__
#define __P2PDownloader_H__

#pragma once

#include "BaseDownloader.h"
#include "P2PManager.h"
#include "SimpleThread.h"
#include "HttpDownloadEx.h"


enum DownloadOptionIDEx
{
	Opt_DownUrl = 0x2000, 
	Opt_DownUrlHash, 
	Opt_DownRid, 
	Opt_DownTracker, 
	Opt_DownTrackerList, 
	Opt_DownModeChange,
	Opt_DownUrlsList, 
};

enum P2PReportType
{
	Rep_TaskInfo = 0x1, 
	Rep_DownInfo = 0x2, 
	Rep_UpInfo   = 0x4, 
};

enum ModeChangeType
{
	Change_No = 0x0, 
	Change_NoRid, 
	Change_ConnectionTimeout, 
	Change_SpeedTooLower, 
	Change_Http, 
};

enum P2PEvents
{
	#define GenericEventIdx(off)		(0x1 << (off))
	#define GenericIsEvent(evs, ev)		(evs &  (ev) )
	#define GenericSetEvent(evs, ev)	(evs |= (ev) )
	#define GenericResetEvent(evs, ev)	\
	{ \
		int evx = ~(ev); \
		evs &= evx; \
	}

	Event_Invalid        = 0x0, 
	Event_Redowner       = GenericEventIdx(0), 
	Event_CompleteNotify = GenericEventIdx(1), 
	Event_UploadersLimit = GenericEventIdx(2), 
	Event_Report         = GenericEventIdx(3), 
	Event_DestoryDowner  = GenericEventIdx(4), 
	Event_SaveConfiger   = GenericEventIdx(5), 
	Event_StartNotify    = GenericEventIdx(6), 
	Event_RetryDownload  = GenericEventIdx(7), 
};


class CMsHttpDownloader : public CMsBaseDownloader
{
public:
	CMsHttpDownloader(CMsBaseDownloaderContext* context);
	~CMsHttpDownloader();

	int Init();
	int Uninit();

protected:
	int Start();
	int Stop();
	int Pause();
	int Resume();
	int Timeout();


private:
	CHttpDownloadEx  m_http;
	const P2PString  m_filepath;
};


class CMsP2PDownloader : public CMsBaseDownloader
{
public:
	CMsP2PDownloader(CMsBaseDownloaderContext* context);
	~CMsP2PDownloader();

	int Init();
	int Uninit();

protected:
	int Start();
	int Stop();
	int Pause();
	int Resume();
	int Timeout();

private:
	// wrapper p2p interface: 'StartDownloadWithRID'
	void StartDownloadWithRIDW(const unsigned char* rid, 
		const CVectorString & trackersw, const CVectorString & urlsw, const wchar_t* savepathw);


private:
	// for p2p interface
	int     m_nTrackerSize;
	char**  m_lpTrackers;
};


class CVectorString;
class CMsP2PHttpDownloaderContext : public CMsBaseDownloaderContext
	,  public CSimpleThreadCallback
{
public:
	CMsP2PHttpDownloaderContext(CMsBaseDownloaderSetting* setting)
		: CMsBaseDownloaderContext(setting)
		, m_thread(NULL)
		, m_mchange(Change_No)
		, m_events(Event_Invalid)
		, m_type(Type_UrlHash)
		, m_pbasetimes(::GetTickCount())
		, m_pruntimes(0)
		, m_pdowntimes(0)
	{
		memset(&m_urlhash, 0, sizeof(m_urlhash));
		memset(&m_rid, 0, sizeof(m_urlhash));
	}

	~CMsP2PHttpDownloaderContext()
	{
		// state must be saved before it
		if(State_Downloading == m_state || State_Resume == m_state)
		{
			SetOpt(Opt_DownState, (void*)State_Stop, sizeof(State_Stop));
		}

		if(m_thread != NULL)
		{
			m_thread->Stop();
			delete m_thread;
			m_thread = NULL;
		}

		DestroyDownloader();
	}

	const P2PString Url()
	{
		return m_url.c_str();
	}

	const P2PLPHash UrlHash()
	{
		return m_urlhash.hash;
	}

	const P2PLPHash Rid()
	{
		return IsValidHash(m_rid) ? m_rid.hash : NULL;
	}

	CVectorString& TrackerList()
	{
		return m_trackers;
	}

	CVectorString& UrlList()
	{
		return m_urls;
	}

	void SetEvent(P2PEvents ev)
	{
		GenericSetEvent(m_events, ev);
	}

	void ResetEvent(P2PEvents ev)
	{
		GenericResetEvent(m_events, ev);
	}

	BOOL IsEvent(P2PEvents ev)
	{
		return GenericIsEvent(m_events, ev);
	}

	int DownType()
	{
		return m_type; 
	}

	void SetDownType(int type)
	{
		assert(type != Type_Unknown);
		m_type = type;
	}

	void ResetPtime()
	{
		m_pbasetimes = ::GetTickCount();
		m_pruntimes  = m_pdowntimes = 0;
	}

	void UpdatePtime()
	{
		m_pruntimes = int(::GetTickCount() - m_pbasetimes)/1000;
		if(m_result < Res_Finish) m_pdowntimes = m_pruntimes;
	}

	int RunPtime()
	{
		return m_pruntimes;
	}

	int DownPtime()
	{
		return m_pdowntimes;
	}


public:
	/*
	 * call order: 
	 * LoaderConfiger();    --> add for 'app name'
	 * GetRid();            --> add for thread,
	 * SetOpt();
	 */
	int LoadConfiger(const char* app);
	int SaveConfiger(const char* app, bool update = false);

	int SetOpt(int opt, void* data, size_t len);
	int GetOpt(int opt, void* data, size_t len);
	int Report();
	int StartDownThread();


protected:
	int CreatDownloader();
	int DestroyDownloader();
	int DestroyDownloader(int mode);


private:
	int Task();
	int CreateDownMode();
	int IsValidP2PMode();
	int GetFileNameFromUrl(std::wstring & filename);
	int CopyP2PFile();

	bool FindDownloadFile();
	bool CheckFileState(int state);
	int  GetDownFileName();
	int  GetDownFileSize();


private:
	// string is unicode
	P2PStdString     m_url;

	// hash
	P2PHash_t        m_urlhash;
	P2PHash_t        m_rid;
	std::string      m_srid;

	CVectorString    m_trackers;
	CVectorString    m_urls;

	// thread
	CSimpleThread*   m_thread;
	int              m_mchange;
	int              m_events;
	int              m_type;

	// recalculate times
	unsigned int     m_pbasetimes;
	int              m_pruntimes;
	int              m_pdowntimes;
};


#define CONTEXT_INFO								"MSP2P(M: %d, S: %d, R: %d, H: %s), "
#define MSP2PLOG_CONTEXT(con, level, format, ...)	MSP2PLOG(level, \
			CONTEXT_INFO##format, \
			(con)->Mode(), (con)->State(), (con)->DownResult(), \
			CP2PClientTool::HashToString(*(const P2PHash_t*)(con)->UrlHash()), \
			__VA_ARGS__); 


#endif
