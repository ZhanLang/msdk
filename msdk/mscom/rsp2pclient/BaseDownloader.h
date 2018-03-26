#ifndef __BaseDownloader_H__
#define __BaseDownloader_H__

#pragma once


#include "BaseDownloaderSetting.h"


enum DownloadStateID
{
	State_Unknown = 0, // in context, it mean running. unknown = downloading
	State_Downloading, // running
	State_Pause, 
	State_Stop,        // stop
	State_Resume, 
	State_Finish, 
	State_Timeout
};

// it is actually a extended 'StateMessageID'
enum DownloadOptionID
{
	Opt_DownMode = 0x1000, 
	Opt_DownState, 
	Opt_DownRemoveFile, 
	Opt_DownFileSavePath, 
	Opt_DownFileName, 
	Opt_DownFileLen, 
	Opt_DownloadSize,
	Opt_DownloadTimes, 
	Opt_DownloadRate, 
	Opt_UploadSize, 
	Opt_UploadTimes, 
	Opt_UploadRate, 
	Opt_DownStartTime, 
	Opt_DownTotalTimes, 
	Opt_DownTolerateSize, 
	Opt_DownResult, 
	Opt_MaxRunings, 
	Opt_IsRetry, 
	Opt_RenameFileName,		// rename file name, file name be getted from url
};



enum P2PTimeout
{
	Tm_Unknown = 0x0,
	Tm_Timeout,
	Tm_Timein, 
	Tm_RateLower, 
};

enum P2PDownResult
{
	Res_Waiting = 0x0, 
	Res_Downing, 
	Res_Finish = 0x4, 
	Res_ReFinish, 
	Res_Error = 0x8, 
	Res_UserCancel, 
	Res_StopBlock, 
};

enum P2PError
{
	P2P_ERROR = -1, 
	P2P_OK = 0x0, 
	P2P_TIMEOUT, 
	P2P_USERCANCEL, 
	P2P_HTTPDOWN_FAILED, 
	P2P_P2PDOWN_FAILED, 

	P2P_DUPLICATE_COMPLETE,  // is uping. 
	P2P_DUPLICATE_DOWNING,   // is downing
	P2P_BLOCK_IN_STOP, 

	// state
	P2P_STATE_ERROR = 0x30, 
	P2P_STATE_UNKNNOWN, 
	P2P_STATE_NO_START, 
	P2P_STATE_NO_STOP, 

	// mode
	P2P_MODE_ERROR = 0x60, 
	P2P_MODE_UNKNOWN
};


class CMsBaseDownloaderContext;
class CMsBaseDownloader
{
public:
	CMsBaseDownloader(CMsBaseDownloaderContext* context, int key);
	virtual ~CMsBaseDownloader();

	virtual int Init();		                // get 'file' and 'filelen'
	virtual int Uninit();
	virtual int UpdateState();

	int         Key() { return m_key; }

	static bool StateCheck(int news, int olds);


protected:
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int Pause() = 0;
	virtual int Resume() = 0;
	virtual int Timeout() = 0;

	CMsBaseDownloaderContext* GetContext()
	{
		return m_context;
	}


private:
	CMsBaseDownloaderContext* m_context;
	int                       m_rstate;	    // state assert
	bool                      m_start;      // state assert
	int                       m_key;		// downer mode
};


class CMsBaseDownloaderContext
{
public:
	CMsBaseDownloaderContext(CMsBaseDownloaderSetting* setting)
		: m_downer(NULL)
		, m_setting(setting)
		, m_state(State_Unknown)
		, m_mode(Mod_Unknown)
		, m_removefile(FALSE)
		, m_downsize(0)
		, m_downrate(0)
		, m_downtimes(0)
		, m_upsize(0)
		, m_uprate(0)
		, m_uptimes(0)
		, m_starttime(GetTickCount())
		, m_totaltimes(0)
		, m_istimeout(Tm_Unknown)
		, m_result(Res_Waiting)
		, m_filelen(0)
		, m_retrys(0)
		, m_error(0)
		, m_basetm(m_starttime)
		, m_basesz(m_downsize)
		, m_ratelower(Tm_Unknown)
		, m_p2pid(-1)
	{
		assert(m_setting != NULL);
		m_savepath = m_setting->SavePath();
		m_timeout  = m_setting->Timeout();
		m_limitsz  = m_setting->RateLimit();
	}

	virtual ~CMsBaseDownloaderContext()
	{
		DestroyDownloader();
	}

	CMsBaseDownloaderSetting* Setting()
	{
		return m_setting;
	}

	CMsBaseDownloader* GetDownloader()
	{
		return m_downer;
	}

	const P2PString SavePath()
	{
		return m_savepath.size() > 0 ? m_savepath.c_str() : NULL;
	}

	const P2PString FileName()
	{
		return m_file.size() > 0 ? m_file.c_str() : NULL;
	}

	// state
	int State()
	{
		return m_state;
	}

	int Mode()
	{
		return m_mode;
	}

	// times
	int Timeout()
	{
		return m_timeout;
	}

	int ResetTimer()
	{
		m_starttime = GetTickCount();
		m_istimeout = Tm_Unknown;

		m_ratelower = Tm_Unknown;
		m_basetm = GetTickCount();
		m_basesz = m_downsize;
		return m_starttime;
	}

	// down info
	int DownSize()
	{
		return m_downsize;
	}

	int DownRate()
	{
		return m_downrate;
	}

	int DownTimes()
	{
		return (GetTickCount() - m_starttime)/1000;
	}

	int DownError(int err = -1)
	{
		if(err != -1)
		{
			m_error = err;
		}

		return m_error;
	}

	BOOL Finish()
	{
		return m_result >= Res_Finish && m_result < Res_Error;
	}

	int  DownResult()
	{
		return m_result;
	}

	int  DownState()
	{
		return m_state;
	}

	int  GetId()
	{
		return m_p2pid;
	}

	void SetId(int id)
	{
		m_p2pid = id;
	}


public:
	// option: 'DownloadOptionID', it include 'StateMessageID'
	virtual int SetOpt(int opt, void* data, size_t len);
	virtual int GetOpt(int opt, void* data, size_t len);
	virtual bool IsTimeout();
	virtual bool DownTooLower();


protected:
	virtual int CreatDownloader()
	{
		return m_downer != NULL ? 
			m_downer->Init() : -1;
	}

	virtual int DestroyDownloader()
	{
		if(State_Stop != m_state && State_Unknown != m_state)
		{
			// loging error: please stop downloader before delete it
			MSP2PLOG(MSP2PLOG_ERROR, "CMsBaseDownloaderContext::DestroyDownloader(), please stop downloader before delete it.");
			return P2P_STATE_NO_STOP;
		}

		return m_downer != NULL ?
			m_downer->Uninit() : 0;
	}


protected:
	CMsBaseDownloaderSetting* m_setting;
	CMsBaseDownloader*        m_downer;

	// configer
	P2PStdString              m_savepath;
	P2PStdString              m_file;
	int                       m_filelen;

	// times
	int                       m_timeout;
	int                       m_istimeout;      // download(connect) timeout
	unsigned int              m_starttime;
	unsigned int              m_totaltimes;		// download total times

	// rate limit
	int                       m_ratelower;      // rate limit timeout
	int                       m_limitsz;
	int                       m_basetm;
	unsigned int              m_basesz;

	// if try
	int                       m_retrys;
	int                       m_error;			// download result; 0: ok, 

	// downer state
	int                       m_state;
	int                       m_mode;
	int                       m_result;
	BOOL                      m_removefile;
	int                       m_downsize;
	int                       m_downrate;
	int                       m_downtimes;
	int                       m_upsize;
	int                       m_uprate;
	int                       m_uptimes;

	// new add, p2p id
	int                       m_p2pid;
};


#endif
