#ifndef __HttpDownloadListener_H__
#define __HttpDownloadListener_H__


#include "httpdownload.h"


typedef void (* LPHttpFileLengthListener) (const unsigned char*  urlhash, unsigned int uFileLength);
typedef void (* LPHttpDownloadInfoListener) (const unsigned char* urlhash, unsigned int uDownloadBytes, unsigned int uDownloadSpeedInBytes,
                                         unsigned int uTimeUsed);
typedef void (* LPHttpFileDownloadCompleteListener) (const unsigned char* urlhash, unsigned int uDownloadSize);


class CMsBaseDownloaderContext;
class CHttpDownloadEx : public CHttpDownload
{
public:
	CHttpDownloadEx(CMsBaseDownloaderContext* context)
		: CHttpDownload()
		, m_context(context)
		, m_timer(0)
	{}

	~CHttpDownloadEx()
	{}


public:
	static void SetListener(LPHttpFileLengthListener fl, 
		LPHttpDownloadInfoListener di, 
		LPHttpFileDownloadCompleteListener fdc)
	{
		m_pfnFl = fl;
		m_pfnDi = di;
		m_pfnFdc = fdc;
	}

	HRESULT Download(LPCTSTR lpszURL, LPCTSTR lpszFileName = NULL, 
		BOOL bAutoRedirect = FALSE, LPCTSTR lpszResetHost = NULL);

	void    StopDownload();

protected:
	BOOL Notify(int mag, WPARAM wp, LPARAM lp);


private:
	unsigned int m_timer;

	// for urlhash
	CMsBaseDownloaderContext*  m_context;

	static LPHttpFileLengthListener   m_pfnFl;
	static LPHttpDownloadInfoListener m_pfnDi;
	static LPHttpFileDownloadCompleteListener m_pfnFdc;
};


#endif
