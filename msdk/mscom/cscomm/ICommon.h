#ifndef _ICOMMON_H_
#define _ICOMMON_H_

#define NAMEDPIPE_NAME_HOMEPAGE				_T("\\\\.\\Pipe\\{C15EEBF6-8D19-4bd0-B694-7C6010CC9E7F}")
#define NAMEDPIPE_NAME_BLD					_T("\\\\.\\Pipe\\{FA6A5BDA-E027-4bb9-BFA3-932D4FF2C899}")


typedef  DWORD (WINAPI *pPipeNotifyCallBack)(LPVOID lpThis,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);

typedef struct _NPMESSAGEHEADER 
{
	DWORD dwSize;
	DWORD dwType;
	DWORD dwError;
	DWORD dwReserved;
	_NPMESSAGEHEADER()
	{
		dwSize = 0;
		dwType = 0;
		dwError = 0;
		dwReserved = 0;
	}
}NPMESSAGEHEADER,*PNPMESSAGEHEADER;

class ICommonFun
{
public:
	ICommonFun(){};
	virtual ~ICommonFun(){};

public:
	virtual DWORD ICFStartNamedPipeServer(LPTSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis) = 0;
	virtual DWORD ICFStopNamedPipeServer() = 0;
	virtual DWORD ICFSendMessageToServer(LPTSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize) = 0;
	virtual BOOL  ICFGetDiskSerialNumber(LPSTR lpDiskNum,DWORD dwDiskNumLen) = 0;
};
#endif 