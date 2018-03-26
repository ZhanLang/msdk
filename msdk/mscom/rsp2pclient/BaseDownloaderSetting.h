#ifndef __BaseDownloaderSetting_H__
#define __BaseDownloaderSetting_H__


#include <windows.h>
#include "P2PClientTool.h"


#define MIN_TOLERATE_DOWN_SIZE			0
#define MIN_TOLERATE_DOWN_TIME			90


typedef struct VariantType_t
{
	union
	{
		int       ival;
		P2PString sval;
	};

	VariantType_t(void* data, size_t len)
	{
		assert(len > 0);
		if(sizeof(int) == len)
		{
			ival = *(int*)&data;
		}
		else
		{
			sval = (P2PString)data;
		}
	}
}VariantType_t;


class CMsBaseDownloaderSetting
{
public:
	CMsBaseDownloaderSetting()
		: m_savepath(L".\\")
		, m_timeout(30)
		, m_starttime(GetTickCount())
		, m_totaltimes(0)
		, m_interval(1000)
		, m_limitsz(1024)
	{}
	virtual ~CMsBaseDownloaderSetting()
	{}

	// simple method
	const P2PStdString SavePath()
	{
		return m_savepath.c_str();
	}

	int Interval()
	{
		return m_interval;
	}

	int Timeout()
	{
		return m_timeout;
	}

	int RateLimit()
	{
		return m_limitsz;
	}

	// setting: 'P2POption'
	virtual int SetOpt(int opt, void* data, size_t len);
	virtual int GetOpt(int opt, void* data, size_t len);


protected:
	P2PStdString            m_savepath;		// default
	int                     m_timeout;      // -1: infinited
	int                     m_starttime;	// it be setted is newest, when program run
	int                     m_totaltimes;   // program has run total times
	int                     m_interval;     // interval
	int                     m_limitsz;      // rate limit: rate = 1 * 1024, times = 90s, m_limitsz = 1 * 1024 * 90;
};


#endif
