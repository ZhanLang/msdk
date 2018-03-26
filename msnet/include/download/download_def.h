#ifndef _DOWNLOAD_DEF_H_
#define _DOWNLOAD_DEF_H_

/*************************下载类型***********************************/
#define DownloadType DWORD 

#define DT_UNKNOWN	(0)
#define	DT_HTTP		(1<<1)	//包括https
#define	DT_P2P		(1<<2)
#define	DT_P2SP		(1<<3)
#define DT_FTP		(1<<4)
#define DT_ALL		(0XFFFF)
/*********************************************************************/

/*************************Option Tools********************************/
#define DECLARE_DOWNLOAD_OPTION(dlt,value,type) (MAKELONG(dlt,value))
#define GET_DOWNLOAD_TYPE(dlt) (LOWORD(dlt))
#define GET_DOWNLOAD_OPTION(opt) (HIWORD(opt))
#define IS_DOWNLOAD_EQUALITY(x,y) (x & y)
/*********************************************************************/


/*********************************Proxy Setting***********************/

enum InternetAccessType	
{
	IAT_NO_PROXY,			
	IAT_PRECONFIG_PROXY,		
	IAT_MANUAL_PROXY			
};

struct IAT_ProxyInfo
{
	InternetAccessType accessType;
	WCHAR			   szProxyName[MAX_PATH];
	WCHAR			   szProxyUser[MAX_PATH];
	WCHAR			   szProxyPwd[MAX_PATH];
};

/*********************************************************************/



/*************************Option Definition***************************/

//代理设置，只针对winnet
#define ProxyInfoOption		DECLARE_DOWNLOAD_OPTION(DT_HTTP|DT_FTP, (1<<1),IAT_ProxyInfo)
/*********************************************************************/



#endif	//_DOWNLOAD_DEF_H_

