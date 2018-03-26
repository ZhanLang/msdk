#ifndef __P2PManager_H__
#define __P2PManager_H__


#include <string>
#include <vector>
#include <map>
#include <list>
#include "BaseDownloaderSetting.h"
#include "P2PConfiger.h"


#define P2P_DEFAULT_REPORT_TYPE Rep_DownInfo

#define P2P_DEFALUT_TRACKER		L"udp://tracker.Margin.cn:7369"
#define P2P_DEFAULT_RID_URL     L"http://s.Margin.cn/p2p/searchrid.php"
#define P2P_DEFAULT_REPORT_URL  L"http://u.Margin.cn/p2p/upinfo.php"
#define P2P_JSON_RID_ID			"rid"
#define P2P_JSON_TRACKER_ID		"tracker"
#define P2P_JSON_HASHURL_ID		"ridurl"
#define P2P_JSON_DOWNURL_ID		"downurl"
#define P2P_CONFIGER_FILEA		".\\.configer"
#define P2P_CONFIGER_FILE		_T(P2P_CONFIGER_FILEA)
#define P2P_META_DIR			".\\sys"
#define P2P_META_DIRP			".\\sys\\"
#define P2P_MAX_TASKS			5
#define Opt_P2PExtendBase		0x80

enum P2PLoadType
{
	Type_AutoUpDown = 0,        // 
	Type_AutoDown,              // to download, only when complete
	Type_AutoUp,                // to upload, only when uncomplete

	Type_UpDownDisable,         // not to 
	Type_UpDisable,             // not to upload. only complete
	Type_DownDisable,           // not to download, when uncomplete
};

#define P2P_DEFUALT_LOAD_TYPE	Type_AutoUp

enum HashType 
{ 
	Type_Unknown = 0, 
	Type_UrlHash, 
	Type_Http, 
	Type_Rid, 
};

enum SaveConfigerType
{
	Type_ModuleExitSave = 0, 
	Type_TasksEmptySave, 
	Type_TaskCompletionSave, 
	Type_TasksNoSave, 
};

#define P2P_DEFAULT_CONFIGER_TYPE	Type_TasksEmptySave
#define Opt_P2PSaveConfiger			(Opt_P2PExtendBase + 1)


class CMsP2PManager;
class CMsP2PSetting : public CMsBaseDownloaderSetting
{
public:
	CMsP2PSetting(CMsP2PManager* m = NULL);
	~CMsP2PSetting();

	// setting: 'P2POption'
	int SetOpt(int opt, void* data, size_t len);
	int GetOpt(int opt, void* data, size_t len);


public:
	// simple method
	CMsP2PManager* Manager()
	{
		return m_manager;
	}

	const P2PString HostName()
	{
		return m_host.c_str();
	}

	const P2PString RidUrl()
	{
		return m_ridurl.c_str();
	}

	CVectorString& TrackerList()
	{
		return m_trackers;
	}

	int Report()
	{
		return m_report;
	}

	const P2PString ReportUrl()
	{
		return m_reporturl.c_str();
	}

	int LoadType()
	{
		return m_load;
	}
	
	CConfiger* Configer()
	{
		return &m_configer;
	}

	int ConfigerType()
	{
		return m_saveconf;
	}


public:
	int LoadSetting();
	int SaveSetting();

	int GetProductionInfo();


public:
	// p2p setting
	int              m_uspeed;
	int              m_dspeed;
	int              m_connects;
	int              m_listenport;

	// product info
	P2PStdString     m_productid;
	P2PStdString     m_pversion;
	P2PStdString     m_mversion;


private:
	CMsP2PManager*   m_manager;

	// p2p option
	CVectorString    m_trackers;
	P2PStdString     m_host;
	P2PStdString     m_ridurl;

	int              m_load;
	int              m_saveconf;

	// report
	int              m_report;
	P2PStdString     m_reporturl;

	CConfiger        m_configer;
};


enum DownloadOptionID;
class CMsBaseDownloaderContext;
typedef std::map<P2PHash_t, CMsBaseDownloaderContext*>    MapUrlHashToDownloader_t;
typedef std::map<P2PHash_t, P2PHash_t>                    MapDownloaderToHash_t;


class CMsP2PManager
{
public:
	STDMETHODIMP Init();
	STDMETHODIMP Uninit();

	STDMETHODIMP SetOpt(P2POption option, void *data, size_t nlen);
	STDMETHODIMP GetOpt(P2POption option, void *data, size_t nlen);
	STDMETHODIMP SetNotify(IMSP2PClientNotify *data);
	STDMETHODIMP SetState(const unsigned char* urlhash, DownloadOptionID statid, void *data, size_t len);
	STDMETHODIMP GetState(const unsigned char *urlhash, StateMessageID   statid, void *data, size_t len);
	STDMETHODIMP OnNotify(const unsigned char* urlhash, NotifyMessageID  atype, WPARAM wp, LPARAM lp);
	STDMETHODIMP Report(const unsigned char* urlhash);

	// add downloader is not to start, 
	// when to set 'down state = start', it only save a status.
	// when to set 'down mode', it create 'downloader(p2p or http)' and update its status
	// notice: when mode is 'http', downloader create thread.
	// example: 
	// -- AddDownloader()
	// -- SetState(...);	// maybe not
	// -- SetState(Opt_DownState);
	// -- SetState(Opt_DownMode);
	STDMETHODIMP AddDownloader(const unsigned char* urlhash, int type, const P2PString url, const P2PString savepath);
	STDMETHODIMP DelDownloader(const unsigned char* urlhash);
	STDMETHODIMP OnNotify(CMsBaseDownloaderContext* context, NotifyMessageID  atype, WPARAM wp, LPARAM lp);

	static CMsP2PManager* GetManager();
	static void DestroyManager();

	CMsBaseDownloaderContext* GetDownloaderByUrlHash(const unsigned char* urlhash);
	CMsBaseDownloaderContext* GetDownloaderByRid(const unsigned char* rid);
	const unsigned char*      GetUrlHashByRid(const unsigned char* rid);
	void                      InsertRidHashMap(const unsigned char* rid, const unsigned char* urlhash);
	void                      DeleteDownloaderByUrlHash(const unsigned char* urlhash);
	CMsBaseDownloaderSetting* GetSetting();

	int  UpdateConfiger();
	void SetDownError(int e) { m_error = e; }
	int  GetDownError()      { return m_error; }

	int  LoadConfigerTasks();
	bool IsValid(CMsBaseDownloaderContext * context);
	int  StartConfigerTask(CMsBaseDownloaderContext * context);
	int  UpdateConfiger(CMsBaseDownloaderContext * context);
	void SetConfigerId(CMsBaseDownloaderContext * context);
	bool IsSaveConfiger(bool bExit = false);


private:
	CMsP2PManager();
	~CMsP2PManager();

	STDMETHODIMP SetAllState(const unsigned char* urlhash, DownloadOptionID statid, void *data, size_t len);
	STDMETHODIMP SetOneState(const unsigned char* urlhash, DownloadOptionID statid, void *data, size_t len);
	STDMETHODIMP GetAllState(const unsigned char* urlhash, StateMessageID statid, void *data, size_t len);
	STDMETHODIMP GetOneState(const unsigned char* urlhash, StateMessageID statid, void *data, size_t len);

	void FileLengthListener(const unsigned char* rid, unsigned int nFileLength);
	void DownloadInfoListener(const unsigned char* rid, unsigned int uDownloadBytes, 
									 unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed);
	void UploadInfoListener(const unsigned char* rid, unsigned int uUploadBytes, 
									unsigned int uUploadSpeedInBytes, unsigned int uTimeUsed);
	void FileDownloadCompleteListener(const unsigned char* rid, unsigned int size);

	static void P2PFileLengthListener(const unsigned char* rid, unsigned int nFileLength);
	static void P2PDownloadInfoListener(const unsigned char* rid, unsigned int uDownloadBytes, 
									 unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed);
	static void P2PUploadInfoListener(const unsigned char* rid, unsigned int uUploadBytes, 
									 unsigned int uUploadSpeedInBytes, unsigned int uTimeUsed);
	static void P2PFileDownloadCompleteListener(const unsigned char* rid, unsigned int size);

	static void HttpFileLengthListener(const unsigned char* urlhash, unsigned int nFileLength);
	static void HttpDownloadInfoListener(const unsigned char* urlhash, unsigned int uDownloadBytes, 
									 unsigned int uDownloadSpeedInBytes, unsigned int uTimeUsed);


public:
	static void HttpFileDownloadCompleteListener(const unsigned char* urlhash, unsigned int size);


private:
	CMsP2PSetting               m_setting;

	// notify
	IMSP2PClientNotify*         m_notify;

	// golobal manager
	static CMsP2PManager*       m_manager;

	// urlhash <--> rid
	CCriticalSetionObject       m_csRidToHash;
	MapUrlHashToDownloader_t    m_mapHashToDowner;
	MapDownloaderToHash_t       m_mapRidToHash;

	bool                        m_init;
	bool                        m_updatesets;
	int                         m_upscount;			// 上传计数
	int                         m_upslimit;			// 上传数限制
	int                         m_error;

	int                         m_p2pmid;


private:
	// number control
	CMsBaseDownloaderContext* RcPop(P2PHash_t & h);
	bool RcPush(CMsBaseDownloaderContext* context);
	int  RcRunningCount();
	void RcClear();
	void UpdateUploaders();


	int                         m_maxRunings;
	CCriticalSetionObject       m_rclock;
	std::list<CMsBaseDownloaderContext*> m_lsWaitTasks;
	std::list<CMsBaseDownloaderContext*> m_lsRunTasks;
};


#endif;
