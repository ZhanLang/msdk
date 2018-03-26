#ifndef _DOWNLOADER_H_
#define _DOWNLOADER_H_

# if defined(BUILDING_SHARED)
#  define SHARED_EXPORT __declspec(dllexport)
# elif defined(LINKING_SHARED)
#  define SHARED_EXPORT __declspec(dllimport)
# else
#  define SHARED_EXPORT
# endif

#ifdef __cplusplus
extern "C" {
#endif

enum ErrorCode
{
    not_found,
    open_failed,
    invalid_torrent,
    undefined
};

void SHARED_EXPORT StartUp();
void SHARED_EXPORT ClearUp();

void SHARED_EXPORT StartDownloadWithRIDx(const unsigned char* strRID, const std::vector<std::string> & trackers, 
										 const std::vector<std::string> & urls, const std::string & save_path);

void SHARED_EXPORT StartDownloadWithRID(const unsigned char* rid, int trkc, const char** trkv, 	
										const char* strSaveFilePath, size_t svlen);

void SHARED_EXPORT PauseDownload(const unsigned char* rid);

void SHARED_EXPORT ResumeDownload(const unsigned char* rid);

void SHARED_EXPORT StopDownload(const unsigned char* rid, BOOL remove_files = FALSE);

void SHARED_EXPORT OnAlertTimer();

enum DownloaderOption
{
    Opt_MaxUpSpeedInBps = 0,
    Opt_MaxDownSpeedInBps,
    Opt_ListenPort,
    Opt_MaxConnectNum
};
void SHARED_EXPORT SetOption(DownloaderOption option, void * data);
void SHARED_EXPORT GetOption(DownloaderOption option, void * data);

enum DownloaderState
{
};
void SHARED_EXPORT GetState(const unsigned char* rid, DownloaderState option, void * data, size_t & len);

typedef void (* LPFileLengthListener) (const unsigned char* rid, unsigned int uFileLength);
void SHARED_EXPORT SetFileLengthListener(LPFileLengthListener listener);

typedef void (* LPDownloadInfoListener) (const unsigned char* rid, unsigned int uDownloadBytes, unsigned int uDownloadSpeedInBytes,
                                         unsigned int uTimeUsed);
void SHARED_EXPORT SetDownloadInfoListener(LPDownloadInfoListener listener, unsigned int interval_ms);

typedef void (* LPUploadInfoListener) (const unsigned char* rid, unsigned int uUploadBytes, unsigned int uUploadSpeedInBytes, 
									   unsigned int uTimeUsed);
void SHARED_EXPORT SetUploadInfoListener(LPUploadInfoListener listener);

// zcg+, 2011/04/27
// 文件下载完成回调中，
// 如果上次已经下载完成也再重新启动时也会回调，这样没有办法区分到底是下载完的回调还是文件检查完的回调
// 增加 uDownloadBytes 是本次下载的总大小，如果为 0，则为文件上传的回调
typedef void (* LPFileDownloadCompleteListener) (const unsigned char* rid, unsigned int uDownloadBytes);
void SHARED_EXPORT SetFileDownloadCompleteListener(LPFileDownloadCompleteListener listener);
#ifdef __cplusplus
}
#endif

#endif