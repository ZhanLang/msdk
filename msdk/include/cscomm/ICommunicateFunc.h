#ifndef _ICOMMUNICATEFUNC_H_
#define _ICOMMUNICATEFUNC_H_

#define NAMEDPIPE_NAME_HOMEPAGE				L"\\\\.\\Pipe\\{C15EEBF6-8D19-4bd0-B694-7C6010CC9E7F}"
#define NAMEDPIPE_NAME_BLD					L"\\\\.\\Pipe\\{FA6A5BDA-E027-4bb9-BFA3-932D4FF2C899}"


typedef  DWORD (WINAPI *pPipeNotifyCallBack)(LPVOID lpThis,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);

typedef enum _URLTYPE
{
	URLTYPE_UNKNOWN = 0,
	
}URLTYPE;

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

class ICommunicateFunc
{
public:
	ICommunicateFunc(){};
	virtual ~ICommunicateFunc(){};

public:
	virtual DWORD ICFStartNamedPipeServer(LPWSTR lpPipeName,DWORD dwMaxConn,pPipeNotifyCallBack callback,LPVOID lpThis) = 0;
	virtual DWORD ICFStopNamedPipeServer() = 0;
	virtual DWORD ICFSendMessageToServer(LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize) = 0;
	virtual DWORD ICFNamedPipeExists(LPWSTR lpPipeName) = 0;
};

ICommunicateFunc * GetStaticCommonObject();
DWORD NamedPipeSendMessageToServer(LPWSTR lpPipeName,LPVOID lpInBuff,DWORD dwInBuffSize,LPVOID lpOutBuff,DWORD& dwOutBuffSize);

#endif 