
#include "stdafx.h"
#include "fsFtpFile.h"
#include <stdio.h>
#include <limits.h>
#include "fsFtpConnection.h"

fsFtpFile::fsFtpFile()
{
	m_dwTransferType = FTP_TRANSFER_TYPE_BINARY;
	m_bDontUseLIST = FALSE;

	m_sFile = m_sRcv = (UINT) -1;
}

fsFtpFile::~fsFtpFile()
{
	CloseHandle ();
}

fsInternetResult fsFtpFile::Open(LPCTSTR pszFilePath, UINT64 uStartPos)
{
	return OpenEx (pszFilePath, uStartPos, _UI64_MAX);
}

fsInternetResult fsFtpFile::Read(LPBYTE pBuffer, DWORD dwToRead, DWORD *pdwRead)
{
	if (m_sFile == UINT (-1))
		return IR_NOTINITIALIZED;

	try {*pdwRead = recv (m_sFile, (char*) pBuffer, dwToRead, 0);}
	catch (...){return IR_ERROR;}

	if (*pdwRead == 0 || *pdwRead == DWORD (SOCKET_ERROR))
		return fsWSAErrorToIR ();

	return IR_SUCCESS;
}

fsInternetResult fsFtpFile::SetTransferType(fsFtpTransferType enType)
{
	switch (enType)
	{
		case FTT_BINARY:
			m_dwTransferType = FTP_TRANSFER_TYPE_BINARY;
			break;

		case FTT_ASCII:
			m_dwTransferType = FTP_TRANSFER_TYPE_ASCII;
			break;

		default:
			return IR_INVALIDPARAM;
	}

	return IR_SUCCESS;
}

VOID fsFtpFile::ReceiveExtError()
{
	DWORD dwLen = 0;
	DWORD dwErr;

	SAFE_DELETE_ARRAY (m_pszLastError);

	InternetGetLastResponseInfo (&dwErr, NULL, &dwLen);

	if (::GetLastError () == ERROR_INSUFFICIENT_BUFFER)
	{
		dwLen++;
		fsnew (m_pszLastError, TCHAR, dwLen);
		InternetGetLastResponseInfo (&dwErr, m_pszLastError, &dwLen);
	}
}

VOID fsFtpFile::CloseHandle()
{
	if (m_sFile != UINT (-1))
		closesocket (m_sFile);

	if (m_sRcv != UINT (-1))
		closesocket (m_sRcv);

	m_sFile = m_sRcv = (UINT) -1;
}

SHORT fsFtpFile::OpenSocket()
{
	sockaddr_in addr;
	hostent *he;
	
	
	
	static SHORT _portStart = IPPORT_RESERVED + 10;

	if (_portStart >= IPPORT_RESERVED + 10000)
		_portStart = IPPORT_RESERVED + 10;

	
	SHORT port;

	char szName [10000]={0};
	gethostname (szName, 10000);

	
	he = gethostbyname (szName);

	if (he == NULL)
		return 0;

	ZeroMemory (&addr, sizeof (addr));
	CopyMemory (&addr.sin_addr, he->h_addr, he->h_length);
	addr.sin_family = he->h_addrtype;

	m_sRcv = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sRcv == UINT (-1))
		return 0;

	int bindErr;

	
	do
	{
		port = _portStart++;
		addr.sin_port = htons (port);
		_portStart++;
		bindErr = bind (m_sRcv, (sockaddr*) &addr, sizeof (addr));
	}
	while (bindErr && ::GetLastError () == WSAEADDRINUSE);

	if (bindErr) 
		return 0;

	listen (m_sRcv, 1); 

	TCHAR ipAddr [100]={0};
	_stprintf(ipAddr, _T("%d,%d,%d,%d,%d,%d"), (int) (BYTE) he->h_addr_list [0][0], (int) (BYTE) he->h_addr_list [0][1], (int) (BYTE) he->h_addr_list [0][2], (int) (BYTE) he->h_addr_list [0][3],
					DWORD (port) >> 8, DWORD (port) & 0xff);
	m_strPORT = ipAddr; 

	return port;
}

fsInternetResult fsFtpFile::FtpError()
{
	fsInternetResult ir = fsWinInetErrorToIR ();
	if (ir == IR_EXTERROR)
		ReceiveExtError ();
	return ir;
}

BOOL fsFtpFile::ParseSIZE()
{
	ReceiveExtError ();
	
	int len = (int)_tcslen (m_pszLastError);
	int pos = len - 2;

	
	
	while (pos > 0 && m_pszLastError [pos - 1] >= '0' && m_pszLastError [pos - 1] <= '9')
		pos--;

	if (pos <= 0 || pos == len - 2)
		return FALSE; 

	UINT64 uSize = (UINT64) _ttoi (m_pszLastError + pos);
	if (uSize > 1000ui64*1024*1024*1024)
		return FALSE;
		
	m_uFileSize = uSize;

	return TRUE;
}

fsInternetResult fsFtpFile::PASV_ConnectSocket()
{
	
	LPCTSTR pszPORT = m_pszLastError;

	
	do
	{
		pszPORT = _tcschr(pszPORT+1, ',');
		if (pszPORT == NULL)
			return IR_ERROR;
	}
	while (IsDigit (*(pszPORT - 1)) == FALSE || IsDigit (*(pszPORT + 1)) == FALSE);

	while (IsDigit (*(--pszPORT))); 
	pszPORT++;

	sockaddr_in addr;

	ZeroMemory (&addr, sizeof (addr));
	addr.sin_family = AF_INET;

	BYTE aAddr [6]={0}; 

	
	for (int i = 0; i < 6; i++)
	{
		aAddr [i] = (BYTE) _ttoi (pszPORT);
		
		
		while (*pszPORT >= '0' && *pszPORT <= '9')
			pszPORT++;
		while (*pszPORT && (*pszPORT < '0' || *pszPORT > '9'))
			pszPORT++;

		if (*pszPORT == 0 && i != 5)
			return IR_ERROR;
	}

	
	addr.sin_addr.S_un.S_un_b.s_b1 = aAddr [0];
	addr.sin_addr.S_un.S_un_b.s_b2 = aAddr [1];
	addr.sin_addr.S_un.S_un_b.s_b3 = aAddr [2];
	addr.sin_addr.S_un.S_un_b.s_b4 = aAddr [3];
	
	addr.sin_port = short ((unsigned char)aAddr [5] << 8 | (unsigned char)aAddr [4]); 

	m_sFile = socket (AF_INET, SOCK_STREAM, 0);
	if (m_sFile == -1)
		return fsWSAErrorToIR ();

	if (connect (m_sFile, (sockaddr*)&addr, sizeof (addr)) == -1) 
		return fsWSAErrorToIR ();

	return IR_SUCCESS;
}

BOOL fsFtpFile::IsDigit(TCHAR c)
{
	return c <= '9' && c >= '0';
}

fsInternetResult fsFtpFile::QuerySize(LPCTSTR pszFilePath)
{
	
	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = m_pServer->GetHandle (); 

	if (!hServer)	
		return IR_NOTINITIALIZED;

	m_bContentTypeValid = FALSE;
	m_bDateValid = FALSE;

	CloseHandle (); 

	TCHAR szCmd [10000]={0};	

	TCHAR szFile [10000]={0};
	
	_tcscpy(szFile, pszFilePath);

	TCHAR *pszFile = const_cast<TCHAR*>(max (_tcsrchr (pszFilePath, '/'), _tcsrchr (pszFilePath, '\\')));

	if (pszFile)
	{
		if (pszFile - pszFilePath)
		{
			szFile [pszFile - pszFilePath] = 0;

			_stprintf(szCmd,_T("CWD %s"), szFile);	
												
			

			if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL))
				_tcscpy(szFile, pszFilePath);	
			else
				_tcscpy(szFile, pszFile + 1);	
			
			
		}
	}

	BOOL bListOK = FALSE;

	m_uFileSize = _UI64_MAX;

	if (m_bDontUseLIST == FALSE)
		bListOK = Send_LIST (szCmd, _countof(szCmd),szFile);

	if (bListOK == FALSE)
	{
		
		lstrcpy (szCmd, _T("TYPE I"));
		
		fsInternetResult ir;
		if (!FtpCommand (hServer, FALSE, FTP_TRANSFER_TYPE_BINARY, szCmd, NULL, NULL)) 
		{
			ir = FtpError ();
			
			return ir;
		}

		
		_stprintf(szCmd, _T("SIZE %s"), szFile);
		
		if (FtpCommand (hServer, FALSE, FTP_TRANSFER_TYPE_BINARY, szCmd, NULL, NULL))
			ParseSIZE ();
	

		if (m_uFileSize == _UI64_MAX && m_bDontUseLIST)
		{
			
			bListOK = Send_LIST (szCmd,_countof(szCmd), szFile);
		}
	}

	return IR_SUCCESS;
}

VOID fsFtpFile::SetDontUseLIST(BOOL b)
{
	m_bDontUseLIST = b;
}

BOOL fsFtpFile::Send_LIST(LPTSTR pszCmd,DWORD dwCount, LPCTSTR pszFile)
{
	_stprintf(pszCmd,_T("LIST %s"), pszFile);
	

	WIN32_FIND_DATA wfd;

	HINTERNET hFind = FtpFindFirstFile (m_pServer->GetHandle (), pszFile, &wfd, INTERNET_FLAG_NO_CACHE_WRITE | 
		INTERNET_FLAG_RELOAD, 0);



	if (hFind)
	{
		InternetCloseHandle (hFind);

		
		
		if (m_uFileSize == _UI64_MAX && wfd.nFileSizeLow != UINT_MAX)
		{
			UINT64 u = ((UINT64)wfd.nFileSizeHigh * ((UINT64)MAXDWORD+1)) + wfd.nFileSizeLow;
			
			if (u) 
				m_uFileSize = u;
		}

		if (wfd.ftCreationTime.dwLowDateTime != 0 || wfd.ftCreationTime.dwHighDateTime != 0)
			m_date = wfd.ftCreationTime;
		else if (wfd.ftLastWriteTime.dwLowDateTime != 0 || wfd.ftLastWriteTime.dwHighDateTime != 0)
			m_date = wfd.ftLastWriteTime;
		else
			m_date = wfd.ftLastAccessTime;

		m_bDateValid = TRUE;
	}

	
	
	return FALSE;
}

fsFtpTransferType fsFtpFile::GetTransferType()
{
	if (m_dwTransferType & FTP_TRANSFER_TYPE_BINARY)
		return FTT_BINARY;
	else if (m_dwTransferType & FTP_TRANSFER_TYPE_ASCII)
		return FTT_ASCII;
	else
		return FTT_UNKNOWN;
}

fsInternetResult fsFtpFile::OpenEx(LPCTSTR pszFilePath, UINT64 uStartPos, UINT64 , UINT64 uUploadTotalSize)
{
	if (!m_pServer) 
		return IR_NOTINITIALIZED;

	HINTERNET hServer = m_pServer->GetHandle (); 

	if (!hServer)	
		return IR_NOTINITIALIZED;

	m_bContentTypeValid = FALSE;
	m_bDateValid = FALSE;

	CloseHandle (); 

	TCHAR szCmd [10000]={0};	

	TCHAR szFile [10000]={0};
	_tcscpy(szFile, pszFilePath);

	
	
	
	TCHAR *pszFile = const_cast<TCHAR*>(max (_tcsrchr (pszFilePath, '/'), _tcsrchr (pszFilePath, '\\')));

	if (pszFile)
	{
		if (pszFile - pszFilePath)
		{
			szFile [pszFile - pszFilePath] = 0;

			_stprintf(szCmd, _T("CWD %s"), szFile);	
												
			

			if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL))
				lstrcpy (szFile, pszFilePath);	
			else
				lstrcpy (szFile, pszFile + 1);	
			
			
		}
	}

	BOOL bListOK = FALSE;

	m_uFileSize = _UI64_MAX;

	if (m_bDontUseLIST == FALSE && uStartPos == 0)
		bListOK = Send_LIST (szCmd,_countof(szCmd), szFile);

	if (bListOK == FALSE)
	{
		
		lstrcpy (szCmd, _T("TYPE I"));
		
		fsInternetResult ir;
		if (!FtpCommand (hServer, FALSE, FTP_TRANSFER_TYPE_BINARY, szCmd, NULL, NULL)) 
		{
			ir = FtpError ();
			
			return ir;
		}

		
		_stprintf(szCmd, _T("SIZE %s"), szFile);
		
		if (FtpCommand (hServer, FALSE, FTP_TRANSFER_TYPE_BINARY, szCmd, NULL, NULL))
			ParseSIZE ();
		

		if (m_uFileSize == _UI64_MAX && m_bDontUseLIST)
		{
			
			
			bListOK = Send_LIST (szCmd, _countof(szCmd),szFile);
		}
	}

	
	_stprintf(szCmd, _T("TYPE %c"), m_dwTransferType & FTP_TRANSFER_TYPE_ASCII ? 'A' : 'I');
		

	fsInternetResult ir;

	if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL)) 
	{
		ir = FtpError ();
		
		return ir;
	}

	

	
	BOOL bPassive = ((fsFtpConnection*) m_pServer)->IsPassiveMode ();

	if (bPassive)
	{
		
		
		if (!FtpCommand (hServer, FALSE, m_dwTransferType, _T("PASV"), NULL, NULL))
		{
			return FtpError ();
		}

		
		ReceiveExtError (); 

		fsInternetResult ir = PASV_ConnectSocket ();	
														
		if (ir != IR_SUCCESS)
			return ir;
	}
	else
	{
		
		if (0 == OpenSocket ())
			return IR_ERROR;

		
		/*
		_stprintf_s (szCmd,_countof(szCmd), _T("PORT %s"), m_strPORT.c_str());
		
		if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL)) 
		{
			
			return FtpError ();
		}
		*/
		
	}

	if (uUploadTotalSize == _UI64_MAX)
	{
		if (uStartPos) 
		{
			_stprintf(szCmd, _T("REST %I64u"), uStartPos); 
		

			if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL))
				m_enRST = RST_NONE; 
			

			if (m_enRST == RST_NONE)
				return IR_RANGESNOTAVAIL;

			m_enRST = RST_PRESENT;
		}
		else
		{
			if (!FtpCommand (hServer, FALSE, m_dwTransferType, _T("REST 100"), NULL, NULL))
			{
				m_enRST = RST_NONE;
				
			}
			else
			{
				m_enRST = RST_PRESENT;
				FtpCommand (hServer, FALSE, m_dwTransferType, _T("REST 0"), NULL, NULL);
			}
		}

		_stprintf(szCmd, _T("RETR %s"), szFile); 
	}
	else
	{
		if (uStartPos)
			_stprintf(szCmd ,_T("APPE %s"), szFile);
		else
			_stprintf(szCmd,_T("STOR %s"), szFile);			
	}

	if (!FtpCommand (hServer, FALSE, m_dwTransferType, szCmd, NULL, NULL))
	{
		ir = FtpError ();
		return ir;
	}


	if (bPassive == FALSE)
	{
		m_sFile = accept (m_sRcv, NULL, 0); 
											
		if (m_sFile == UINT (-1))			
			return fsWSAErrorToIR ();
	}

	return IR_SUCCESS;
}

fsInternetResult fsFtpFile::Write (LPBYTE pBuffer, DWORD dwToWrite, DWORD *pdwWritten)
{
	if (m_sFile == UINT (-1))
		return IR_NOTINITIALIZED;

	try {*pdwWritten = send (m_sFile, (char*) pBuffer, dwToWrite, 0);}
	catch (...){return IR_ERROR;}

	if (*pdwWritten == 0 || *pdwWritten == DWORD (SOCKET_ERROR))
		return fsWSAErrorToIR ();

	return IR_SUCCESS;
}

BOOL fsFtpFile::FtpCommand(HINTERNET hConnect, BOOL fExpectResponse, DWORD dwFlags, LPCTSTR pszCommand, DWORD_PTR dwContext, HINTERNET* phFtpCommand)
{
	TCHAR szCmd [1000]={0};
	if (_tcschr (pszCommand, '%'))
	{
		int pos = 0;

		while (*pszCommand)
		{
			if (*pszCommand == '%')
			{
				szCmd [pos++] = '%';
				szCmd [pos++] = '%';
			}
			else
				szCmd [pos++] = *pszCommand;

			pszCommand++;
		}

		szCmd [pos] = 0;
		pszCommand = szCmd;
	}

	return ::FtpCommand (hConnect, fExpectResponse, dwFlags, pszCommand, dwContext, phFtpCommand);
}

VOID fsFtpFile::DeleteLatter()
{
	this->CloseHandle();
	delete this;
}