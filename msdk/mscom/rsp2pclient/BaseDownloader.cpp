#include "stdafx.h"
#include "BaseDownloader.h"


// -------------------------------------------------------------
// class CMsBaseDownloader
CMsBaseDownloader::CMsBaseDownloader(CMsBaseDownloaderContext* context, int key)
	: m_context(context)
	, m_rstate(State_Unknown)
	, m_start(false)
	, m_key(key)
{
	assert(m_context != NULL);
}

CMsBaseDownloader::~CMsBaseDownloader()
{
}

int CMsBaseDownloader::Init()
{
	return UpdateState();
}

int CMsBaseDownloader::Uninit()
{
	return 0;
}

int CMsBaseDownloader::UpdateState()
{
	int state = State_Unknown;
	m_context->GetOpt(Opt_DownState, &state, sizeof(state));
	if(!StateCheck(state, m_rstate))
	{
		MSP2PLOG(MSP2PLOG_ERROR, "CMsBaseDownloader::UpdateState(), downloader state change error! [%d --> %d] ***", 
			m_rstate, state);
		return -1;
	}

	m_rstate = state;
	switch(state)
	{
	case State_Downloading:
		m_start = true;
		m_context->ResetTimer();
		Start();
		break;

	case State_Pause:
		Pause();
		break;

	case State_Resume:
		Resume();
		break;

	case State_Stop:
		Stop();
		m_start = false;
		break;

	case State_Timeout:
		Timeout();
		m_start = false;

	default:
		// loging warning: unknown state id 
		MSP2PLOG(MSP2PLOG_WARNING, "CMsBaseDownloader::UpdateState(), unknown state id");
		break;
	}

	return 0;
}

bool CMsBaseDownloader::StateCheck(int news, int olds)
{
	// state check.
	// state: unknown, downing, stop, start, pause, resume
	// unknown -> downing
	// downing -> pause, stop
	// stop    -> start
	// pause   -> resume
	// resume  -> downing

	bool ret = false;
	switch(olds)
	{
	case State_Unknown:
		ret = true;
		break;

	case State_Stop:
		ret = State_Downloading == news;
		break;

	case State_Downloading:
	case State_Resume:
		ret = State_Pause == news || State_Stop == news;
		break;

	case State_Pause:
		ret = State_Resume == news || State_Downloading == news
			|| State_Stop == news;
		break;

	default:
		break;
	}

	return ret;
}


// -------------------------------------------------------------
// class CMsBaseDownloaderContext
int CMsBaseDownloaderContext::SetOpt(int opt, void* data, size_t len)
{
	VariantType_t vt(data, len);

	switch(opt)
	{
	case Opt_DownState:
		// status tranfer check
		if(!CMsBaseDownloader::StateCheck(vt.ival, m_state))
		{
			MSP2PLOG(MSP2PLOG_ERROR, "CMsBaseDownloaderContext::SetOpt(), downloader state change error! [%d --> %d] ***", 
				m_state, vt.ival);
			return -1;
		}

		m_state = vt.ival;
		if(m_downer != NULL)
		{
			m_downer->UpdateState();
		}
		break;

	case Opt_DownMode:
		if(m_mode != vt.ival || m_downer == NULL)
		{
			// mode change
			if(m_downer != NULL && DestroyDownloader() != 0)
			{
				MSP2PLOG(MSP2PLOG_ERROR, "CMsBaseDownloaderContext::SetOpt(), " \
					"Mode Change, Destroy downloader(oldmode: %d) failed.", m_mode);
				return -1;
			}

			m_result = Res_Downing;
			m_mode = vt.ival;
			if(0 != CreatDownloader())
			{
				MSP2PLOG(MSP2PLOG_ERROR, "CMsBaseDownloaderContext::SetOpt(), " \
					"Mode Change, Create downloader(newmode: %d) failed.", m_mode);
				return -1;
			}
		}
		break;

	case Opt_DownFileSavePath:
		m_savepath = vt.sval;
		if(m_savepath.size() > 0 && L'\\' != m_savepath[m_savepath.size() - 1])
		{
			m_savepath += L"\\";
		}
		break;

	case Opt_DownRemoveFile:
		m_removefile = vt.ival;
		break;

	case Opt_DownFileName:
		m_file = vt.sval;
		break;

	case Opt_DownFileLen:
		m_filelen = vt.ival;
		break;

	// state info
	case Stat_DownloadSize:
	case Opt_DownloadSize:
		// assert((int)data >= m_downsize);
		m_downrate = (vt.ival > m_downsize ? (vt.ival - m_downsize) : 0)/
			(m_setting->Interval() > 1000 ? m_setting->Interval()/1000 : 1);
		m_downsize = vt.ival;
		break;

	case Stat_DownloadTimes:
	case Opt_DownloadTimes:
		m_downtimes = vt.ival;
		break;

	case Stat_DownloadRate:
	case Opt_DownloadRate:
		m_downrate = vt.ival;
		break;

	case Stat_UploadSize:
	case Opt_UploadSize:
		//m_uprate = (vt.ival > m_upsize ? (vt.ival - m_upsize) : 0) / 
		//	(m_setting->Interval() > 1000 ? m_setting->Interval()/1000 : 1);
		m_upsize = vt.ival;
		break;

	case Stat_UploadTimes:
	case Opt_UploadTimes:
		m_uptimes = vt.ival;
		break;

	case Stat_UploadRate:
	case Opt_UploadRate:
		m_uprate = vt.ival;
		break;

	case Stat_DownResult:
	case Opt_DownResult:
		// it maybe be reloaded.
		if(m_downsize > 0)
		{
			m_filelen = m_downsize;
			m_downtimes = (GetTickCount() - m_starttime)/1000;
			assert(m_downtimes >= 0 && m_downtimes >= m_downtimes);
			m_totaltimes += m_downtimes;
			m_downrate = m_totaltimes > 0 ? m_filelen/m_totaltimes : m_filelen;
		}

		m_result = vt.ival;
		break;

	case Stat_Seeds:
	case Stat_Peers:
		break;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsBaseDownloaderContext::SetOpt(), " \
			"Unknown Option ID(%u).", opt);
		break;
	}

	return 0;
}

int CMsBaseDownloaderContext::GetOpt(int opt, void* data, size_t len)
{
	switch(opt)
	{
	case Stat_Info:
	case Opt_DownState:
		*(int*)data = m_state;
		break;

	case Stat_Mode:
	case Opt_DownMode:
		if(m_mode == Mod_Unknown)
		{
			*(int*)data = Mod_P2P;		// default is 'Mod_P2P'
		}
		else
		{
			*(int*)data = m_mode;
		}
		break;

	case Opt_DownRemoveFile:
		*(BOOL*)data = m_removefile;
		break;

	case Stat_FileSavePath:
	case Opt_DownFileSavePath:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_savepath.c_str());
		break;

	case Stat_FileName:
	case Opt_DownFileName:
		wcscpy_s((wchar_t*)data, P2PStringSize(len), m_file.c_str());
		break;

	case Stat_FileLength:
	case Opt_DownFileLen:
		*(int*)data = m_filelen;
		break;

	// state info 
	case Stat_DownloadSize:
	case Opt_DownloadSize:
		*(int*)data = m_downsize;
		break;

	case Stat_DownloadTimes:
	case Opt_DownloadTimes:
		if(!Finish())
		{
			*(int*)data = m_totaltimes + m_downtimes;
		}
		else
		{
			*(int*)data = m_totaltimes;
		}
		break;

	case Stat_DownloadRate:
	case Opt_DownloadRate:
		*(int*)data = m_downrate;
		break;

	case Stat_UploadSize:
	case Opt_UploadSize:
		*(int*)data = m_upsize;
		break;

	case Stat_UploadTimes:
	case Opt_UploadTimes:
		*(int*)data = m_uptimes;
		break;

	case Stat_UploadRate:
	case Opt_UploadRate:
		*(int*)data = m_uprate;
		break;

	case Stat_DownResult:
	case Opt_DownResult:
		*(int*)data = m_result;
		break;

	case Opt_IsRetry:
		m_retrys++;
		if(m_retrys < 3)
		{
			*(BOOL*)data = TRUE;
			ResetTimer();
		}
		else
		{
			*(BOOL*)data = FALSE;
			m_retrys = 0;
		}
		break;

	case Stat_Seeds:
	case Stat_Peers:
		break;

	default:
		MSP2PLOG(MSP2PLOG_WARNING, "CMsBaseDownloaderContext::GetOpt(), " \
			"Unknown Option ID(%u).", opt);
		break;
	}

	return 0;
}

bool CMsBaseDownloaderContext::IsTimeout()
{
	if(m_timeout == -1)
	{
		return false;
	}

	if(m_istimeout != Tm_Unknown)
	{
		return m_istimeout == Tm_Timeout ? true : false;
	}

	int step = (GetTickCount() - m_starttime)/1000;
	if(step < m_timeout)
	{
		return false;
	}

	if(m_downsize > MIN_TOLERATE_DOWN_SIZE)
	{
		m_istimeout = Tm_Timein;
		return false;
	}

	m_istimeout = Tm_Timeout;
	MSP2PLOG(MSP2PLOG_NOTICE, "CMsBaseDownloaderContext::IsTimeout(): connect timeout(%d)", m_timeout);
	return true;
}

bool CMsBaseDownloaderContext::DownTooLower()
{
	if(m_limitsz == -1 || m_istimeout == Tm_Unknown)
	{
		return false;
	}

	if(m_ratelower != Tm_Unknown)
	{
		assert(m_ratelower == Tm_RateLower);
		return m_ratelower == Tm_RateLower;
	}

	#define MIN_TOLERATE_DOWN_TIME_EX	MIN_TOLERATE_DOWN_TIME * 1000
	bool ret  = false;
	int  tick = GetTickCount();
	int  step = tick - m_basetm;
	int  sz   = int(m_downsize - m_basesz);
	if(sz < 0 || sz > m_limitsz)
	{
		m_basetm = tick;
		m_basesz = m_downsize;
		return ret;
	}

	if(step < MIN_TOLERATE_DOWN_TIME_EX)
	{
		return ret;
	}

	if(sz < m_limitsz)
	{
		m_ratelower = Tm_RateLower;
		MSP2PLOG(MSP2PLOG_NOTICE, "CMsBaseDownloaderContext::DownTooLower(): down rate too lower(%d, %d)", sz, m_limitsz);
		ret = true;
	}

	m_basetm = tick;
	m_basesz = m_downsize;
	return ret;
}
