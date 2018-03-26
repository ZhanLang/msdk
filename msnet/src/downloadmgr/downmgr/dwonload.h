#pragma once

//下载任务全属性
typedef struct _downloadTask_Property : public DownLoadTaskInfo
{
    _downloadTask_Property()
    {
        ZeroMemory(Url, sizeof(Url));
        ZeroMemory(Path, sizeof(Path));
        ZeroMemory(Cookies, sizeof(Cookies));
        ZeroMemory(Referer, sizeof(Referer));
    }


    TCHAR		Url[1000];
    TCHAR       Path[MAX_PATH];
    TCHAR		Cookies[MAX_PATH];
    TCHAR		Referer[MAX_PATH];

    _downloadTask_Property(const DownLoadTaskInfo &info)
    {
        this->task_hash = info.task_hash;
        this->Fsc = info.Fsc;
        memcpy(&DLSize , &info.DLSize, sizeof(DLSize));
        memcpy(&DLTime, &info.DLTime, sizeof(DLTime));

        _tcscpy_s(Url , _countof(Url), info.lpUrl);
        _tcscpy_s(Path ,_countof(Url),info.lpSavePath);
    }

    _downloadTask_Property operator=(const _downloadTask_Property &p)
    {
        this->task_hash = p.task_hash;
        this->Fsc = p.Fsc;
        memcpy(&DLSize , &p.DLSize, sizeof(DLSize));
        memcpy(&DLTime, &p.DLTime, sizeof(DLTime));
        _tcscpy_s(Url ,_countof(Url), p.lpUrl);


        _tcscpy_s(Path , _countof(Path),p.lpSavePath);
        _tcscpy_s(Cookies, _countof(Cookies),p.Cookies);
        _tcscpy_s(Referer, _countof(Referer),p.Referer);

        this->lpUrl = Url;
        this->lpSavePath  = Path;

        return *this;
    }
    _downloadTask_Property operator=(const _downloadTask_Property *p)
    {
        this->task_hash = p->task_hash;
        this->Fsc = p->Fsc;
        memcpy(&DLSize , &p->DLSize, sizeof(DLSize));
        memcpy(&DLTime, &p->DLTime, sizeof(DLTime));


        _tcscpy_s(Url , _countof(Url),p->Url);
        _tcscpy_s(Path , _countof(Path),p->Path);
        _tcscpy_s(Cookies ,_countof(Cookies), p->Cookies);
        _tcscpy_s(Referer, _countof(Referer),p->Referer);


        this->lpUrl = Url;
        this->lpSavePath  = Path;

        return *this;
    }

    operator  DownLoadTaskInfo *()
    {
        return static_cast<DownLoadTaskInfo *>(this);
    }

} DownloadTaskProp;


#define Max_TempBufSize (1024*1024)
interface ISpeed : public IMSBase
{
	/*
	功能:由 CSpeedReporter 主动调用一次
	*/
	STDMETHOD_(DWORD,GetSpeed)() = 0;
};
MS_DEFINE_IID(ISpeed,"{70EAC784-2745-492d-B8A5-E00831BA0B77}");


//设置接口
interface IDwonloadSetting
{
    virtual HRESULT SetOption(DWORD option, LPVOID lpdata, DWORD dwLen) = 0;
    virtual HRESULT GetOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen) = 0;;
};

//任务接口
interface IDownloadTask
{
    virtual HRESULT InitDownloadTask(IDownLoadNotify * pNotift, IDwonloadSetting * pSetting) = 0;
    virtual HRESULT SetTaskOption(DWORD option, LPVOID lpData, DWORD dwLen) = 0;
    virtual HRESULT GetTaskOption(DWORD option, OUT LPVOID * lpData, OUT DWORD & dwLen) = 0;
    virtual IDwonloadSetting * GetSetting() = 0;
};
