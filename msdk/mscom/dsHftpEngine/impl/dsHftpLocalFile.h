/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	4,1th,2014

Module Name:

	dsHftpLocalFile.h  

Abstract:


*/
/************************************************************************/


#ifndef __DSHFTPLOCALFILE_H__
#define __DSHFTPLOCALFILE_H__

#include "dsLock.h"
#include "dsLocalBuf.h"
#include "dsFilePiece.h"

static const TCHAR _szTailTemp[] = _T(".ds");
static const TCHAR _szTailMark[] = _T(".mk");

#define LOCALFILE_NOFILE		0x0000
#define LOCALFILE_REALFILE		0x0001
#define LOCALFILE_TEMPFILE		0x0002
#define LOCALFILE_MARKFILE		0x0004

class dsHftpLocalFile;

typedef LRESULT (CALLBACK *OBSERVERPROC_LOCALFILE)(dsHftpLocalFile */*pfileLocal*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

class dsHftpLocalFile
{
public:

	dsLock m_lockPFW;

	int m_nMissionID;

	CString m_strSavePath;

	CString m_strPieceMarkFile;
	CString m_strTempFile;
	CString m_strLocalFile;
	CString m_strLocalShortFileName;
	
	DWORD m_dwFileSize;

	MAP_LOCALFILEPIECE m_mapLFP;

	HANDLE m_hPMFile;
	HANDLE m_hLocalFile;

	int m_nFileResFlag;

	int m_nLocalFileWriteFlag;

	OBSERVERPROC_LOCALFILE m_procObserver;
	LPVOID m_lpParam;

public:

	dsHftpLocalFile():
		m_hPMFile(NULL),
		m_hLocalFile(NULL),
		m_nFileResFlag(LOCALFILE_NOFILE),
		m_nLocalFileWriteFlag(DSHFTP_FILE_IFEXIT_OK),
		m_dwFileSize(0),
		m_procObserver(NULL),
		m_lpParam(NULL)
	{
		
	}

	  dsHftpLocalFile(PDSHFTPMISSON pmission):
		m_nMissionID(0),
		m_hPMFile(NULL),
		m_hLocalFile(NULL),
		m_procObserver(NULL),
		m_lpParam(NULL)
	{
		Attatch(pmission);
	}

	dsHftpLocalFile(dsMultiUrlMission *pmission):
	m_nMissionID(0),
		m_hPMFile(NULL),
		m_hLocalFile(NULL),
		m_procObserver(NULL),
		m_lpParam(NULL)
	{
		Attatch(pmission);
	}

	//int CheckFileRes(){

	//	m_nFileResFlag = LOCALFILE_NOFILE;

	//	HANDLE hFileExit = ::CreateFile(m_strLocalFile, 0, 0, NULL, OPEN_EXISTING, 0, NULL);

	//	if(NULL != hFileExit){
	//		m_nFileResFlag = LOCALFILE_REALFILE;
	//		CloseHandle(hFileExit);
	//		
	//		return m_nFileResFlag;
	//	}

	//	HANDLE hMarkFile = ::CreateFile(m_strPieceMarkFile, 0, 0, NULL, OPEN_EXISTING, 0, NULL);

	//	if(NULL != hMarkFile){
	//		m_nFileResFlag |= LOCALFILE_MARKFILE;
	//		CloseHandle(hMarkFile);

	//	}

	//	HANDLE hTempFile = ::CreateFile(m_strTempFile, 0, 0, NULL, OPEN_EXISTING, 0, NULL);

	//	if(NULL != hTempFile){
	//		m_nFileResFlag |= LOCALFILE_TEMPFILE;
	//		CloseHandle(hTempFile);
	//	}
	//	
	//	return m_nFileResFlag;
	//}

	int CheckFileRes(){

		m_nFileResFlag = LOCALFILE_NOFILE;

		if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(m_strLocalFile)){
			m_nFileResFlag = LOCALFILE_REALFILE;
			return m_nFileResFlag;
		}

		if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(m_strPieceMarkFile)){
			m_nFileResFlag |= LOCALFILE_MARKFILE;
		}

		if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(m_strTempFile)){
			m_nFileResFlag |= LOCALFILE_TEMPFILE;
		}
		
		return m_nFileResFlag;
	}

	BOOL InitLocalFile()
	{

		if(DSHFTP_FILE_IFEXIT_OK == m_nLocalFileWriteFlag){
			if(LOCALFILE_REALFILE == m_nFileResFlag)
				return FALSE;
		}
		
		else if(DSHFTP_FILE_CHANGENAME == m_nLocalFileWriteFlag){
			if(!UpdateLocalFileNameWhenExit(m_strLocalFile))
				return FALSE;
		}
		else if(DSHFTP_FILE_REPLACE == m_nLocalFileWriteFlag){

		}

		MakeTempFileName();
		MakeMarkFileName();

		m_hLocalFile = CreateFile(m_strTempFile, OPEN_ALWAYS);
		m_hPMFile = CreateFile(m_strPieceMarkFile, OPEN_ALWAYS);

		if(NULL == m_hLocalFile || NULL == m_hPMFile)
			return FALSE;

		return TRUE;
	}

	BOOL ReadMark(){
		if(!CheckTempMarkFileOK())
			return FALSE;

		DWORD dwMarkFileSize = GetFileSize(m_hPMFile, NULL);

		if(0 == dwMarkFileSize)
			return TRUE;

		int nMaxSize = max(4*1024*1024, m_dwFileSize * MAXLEN_PIECEMARK_STR / (4*1024*1024));

		if(dwMarkFileSize >= nMaxSize )//need
			return FALSE;

		dsLocalBuf<wchar_t> pbuf;
		if(NULL == pbuf.Allocate(dwMarkFileSize / 2 + 1))
			return FALSE;

		pbuf.SetZeroMemory();

		DWORD dwReaded = 0;

		if(!::ReadFile(m_hPMFile, pbuf, dwMarkFileSize, &dwReaded, NULL)){
			return FALSE;
		}

		//dsLocalBuf<char> pbufTemp;
		//if(NULL == pbufTemp.Allocate(dwMarkFileSize + 1))
		//	return FALSE;

		//ZeroMemory(pbufTemp.GetData(), pbufTemp.GetLenght());

		//DWORD dwReaded = 0;

		//if(!::ReadFile(m_hPMFile, pbufTemp, dwMarkFileSize, &dwReaded, NULL)){
		//	return FALSE;
		//}

		
		//int nLen = ::MultiByteToWideChar(CP_ACP, 0, pbufTemp, dwMarkFileSize, NULL, NULL);

		//if(0 == nLen)
		//	return FALSE;

		//dsLocalBuf<wchar_t> pbuf;

		//if(NULL == pbuf.Allocate(nLen + 1))
		//	return FALSE;

		//::MultiByteToWideChar(CP_ACP, 0, pbufTemp, dwMarkFileSize, pbuf, nLen);

		dsLocalRefLock lock(m_lockPFW);

		dsLocalFilePiece piece;
		int nUseLenght = 0;
		BOOL bMarkError = FALSE;
		do 
		{


			int nTempLenght = piece.SetMark(pbuf.GetData() + nUseLenght);

			if(nTempLenght <= 0)
			{	
				bMarkError = TRUE;
				break;
			}

			m_mapLFP.insert(make_pair(piece.m_dwBegin, piece));
			nUseLenght += nTempLenght;

		} while (TRUE);

		FixMarksChain();
		

		SetFilePointer(m_hPMFile, 0, NULL, FILE_BEGIN);
		SetEndOfFile(m_hPMFile);

		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.begin();
		while(it != m_mapLFP.end()){

			WritePieceMarkToFile(&it->second);

			it ++;
		}
		

		return !bMarkError;
	}

	//	
	//	return TRUE;
	//}

	DWORD GetDownloadedSize(){

		dsLocalRefLock lock(m_lockPFW);

		DWORD dwSize = 0;

		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.begin();

		while(it != m_mapLFP.end()){

			if(FP_COMPLETE == it->second.m_nStatus){
				dwSize += it->second.GetSize();
			}
			
			it ++;
		}
		
		return dwSize;
	}


	BOOL OutputNetFilePiece(MAP_NETFILEPIECE &mapNFP){
		
		dsLocalRefLock lock(m_lockPFW);

		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.begin();

		while(it != m_mapLFP.end()){
			dsNetFilePiece nfp(it->second.m_dwBegin, it->second.m_dwEnd, it->second.m_nStatus);
			mapNFP.insert(make_pair(nfp.m_dwBegin, nfp));
	
			it ++;
		}

		return TRUE;
	}

	BOOL WriteNetPieceToFile(__inout dsNetFilePiece *pnfp){
		if(NULL == pnfp || NFP_DATAOK != pnfp->m_nStatus || NULL == m_hLocalFile)
			return FALSE;

		::SetFilePointer(m_hLocalFile, pnfp->m_dwBegin, 0, FILE_BEGIN);

		DWORD dwWriteSize = 0;
		if(!::WriteFile(m_hLocalFile, pnfp->m_data, pnfp->GetSize(), &dwWriteSize, NULL))
			return FALSE;

		if(!::FlushFileBuffers(m_hLocalFile))
			return FALSE;

		pnfp->m_nStatus = NFP_WRITED;
		return TRUE;
	}

	BOOL WritePieceMarkToFile(__inout dsNetFilePiece *pnfp){

		if(NULL == pnfp || NFP_WRITED != pnfp->m_nStatus || NULL == m_hPMFile)
			return FALSE;

		dsLocalFilePiece lfp;

		if(!lfp.SetMark(pnfp->m_dwBegin, pnfp->m_dwEnd, FP_COMPLETE))
			return FALSE;


		::SetFilePointer(m_hPMFile, 0, 0, FILE_END);

		DWORD dwWriteSize = 0;
		if(!::WriteFile(m_hPMFile, lfp.GetPieceStr(), lfp.GetPieceStrLenght() * sizeof TCHAR, &dwWriteSize, NULL))
			return FALSE;

		if(!::FlushFileBuffers(m_hPMFile))
			return FALSE;

		pnfp->m_nStatus = NFP_WRITED | NFP_MARKED;
		
		return TRUE;
	}

	BOOL WritePieceMarkToFile(__inout dsLocalFilePiece *plfp){

		if(NULL == plfp || NULL == m_hPMFile)
			return FALSE;

		if(FP_COMPLETE != plfp->m_nStatus){
			return FALSE;
		}

		::SetFilePointer(m_hPMFile, 0, 0, FILE_END);

		DWORD dwWriteSize = 0;
		if(!::WriteFile(m_hPMFile, plfp->GetPieceStr(), plfp->GetPieceStrLenght() * sizeof TCHAR, &dwWriteSize, NULL))
			return FALSE;

		if(!::FlushFileBuffers(m_hPMFile))
			return FALSE;


		return TRUE;
	}


	BOOL UpdateLocalPieceData(__in dsNetFilePiece *pnfp){
		
		dsLocalRefLock lock(m_lockPFW);

		if(NULL == pnfp)
			return FALSE;

		if(NFP_COMPLETE != pnfp->m_nStatus)
			return FALSE;


		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.find(pnfp->m_dwBegin);
		

		BOOL bMixed = FALSE;

		if (it != m_mapLFP.end())
		{
			it->second.m_nStatus = pnfp->m_nStatus;
			
			if(NFP_COMPLETE == pnfp->m_nStatus){
				MAP_LOCALFILEPIECE::iterator itPrev = it;
				
				if(itPrev != m_mapLFP.begin()){
					itPrev --;
					if(NFP_COMPLETE == itPrev->second.m_nStatus){

						if(itPrev->second.m_dwEnd != it->second.m_dwBegin){
						
						}
						else{
							itPrev->second.SetMark(itPrev->second.m_dwBegin, it->second.m_dwEnd, NFP_COMPLETE);
							m_mapLFP.erase(it);
							it = itPrev;

							bMixed = TRUE;
						}
					}
				}

				MAP_LOCALFILEPIECE::iterator itNext = it;
				if(++itNext != m_mapLFP.end()){
					
					if(NFP_COMPLETE & itNext->second.m_nStatus){

						if(itNext->second.m_dwBegin != it->second.m_dwEnd){
						}
						else{
							it->second.SetMark(it->second.m_dwBegin, itNext->second.m_dwEnd, NFP_COMPLETE);
							m_mapLFP.erase(itNext);

							bMixed = TRUE;
						}
					}
				}

				
			}

			//if(!bMixed)
			//{
			//	m_mapLFP.insert(make_pair(pnfp->m_dwBegin, dsLocalFilePiece(pnfp->m_dwBegin, pnfp->m_dwEnd, NFP_COMPLETE)));
			//}
		}
		else
		{
			m_mapLFP.insert(make_pair(pnfp->m_dwBegin, dsLocalFilePiece(pnfp->m_dwBegin, pnfp->m_dwEnd, NFP_COMPLETE)));
			UpdateLocalPieceData(pnfp);
		}

		
		

		return TRUE;
	}

	//BOOL CreateNewPieceMisson(__out dsNetFilePiece *pnfp){

	//	if(NULL == pnfp)
	//		return FALSE;
	//		
	//	

	BOOL CheckMissionComplete(){

		dsLocalRefLock lock(m_lockPFW);

		if(1 != m_mapLFP.size())
			return FALSE;
		
		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.begin();

		if (it != m_mapLFP.end()){
			return (FP_COMPLETE == it->second.m_nStatus && 0 == it->second.m_dwBegin && m_dwFileSize == it->second.m_dwEnd);
		}

		return FALSE;
	}

	inline BOOL CompleteFile(){

		DWORD dwError;

		::CloseHandle(m_hLocalFile);

		dwError = GetLastError();
		if(0 != dwError)
			int a =0;

		m_hLocalFile = NULL;
		if(!::MoveFileEx(m_strTempFile, m_strLocalFile, MOVEFILE_REPLACE_EXISTING))
		{
			dwError = GetLastError();
			return FALSE;
		}
		/*if(!::SetFileShortName(m_hLocalFile, m_strLocalShortFileName))
			return FALSE;*/

		::CloseHandle(m_hPMFile);


		dwError = GetLastError();
		if(0 != dwError)
			int a =0;

		m_hPMFile = NULL;
		BOOL bRet = ::DeleteFile(m_strPieceMarkFile);

		dwError = GetLastError();
		if(0 != dwError)
			int a =0;

		return bRet;
	}

	void Term(){


		if(NULL != m_hLocalFile){
			::CloseHandle(m_hLocalFile);m_hLocalFile = NULL;
		}

		if(NULL != m_hPMFile){
			::CloseHandle(m_hPMFile);m_hPMFile = NULL;
		}
	}

private:
	void FixMarksChain(){

		dsLocalRefLock lock(m_lockPFW);

		MAP_LOCALFILEPIECE::iterator it = m_mapLFP.begin();
		MAP_LOCALFILEPIECE::iterator itNext = it;

		while(it != m_mapLFP.end()){
			itNext ++;

			BOOL bMixed = FALSE;
			if(itNext != m_mapLFP.end()){
				
				if(it->second.m_dwEnd > itNext->second.m_dwBegin){
					
					dsLocalFilePiece lfp = it->second;

					m_mapLFP.clear();
					m_mapLFP.insert(make_pair(lfp.m_dwBegin, lfp));

					break;
					
				}
				else if(it->second.m_dwEnd == itNext->second.m_dwBegin){
					if(FP_COMPLETE == it->second.m_nStatus && FP_COMPLETE == itNext->second.m_nStatus){
						
						it->second.SetMark(it->second.m_dwBegin, itNext->second.m_dwEnd, FP_COMPLETE);
						m_mapLFP.erase(itNext);
						bMixed = TRUE;
					}
				}
				else{

					/*dsLocalFilePiece lfp;
					lfp.SetMark(it->second.m_dwEnd, itNext->second.m_dwBegin, FP_COMPLETE);

					MAP_LOCALFILEPIECE::_Pairib pib = m_mapLFP.insert(make_pair(lfp.m_dwBegin, lfp));
					_ASSERT(pib.second);
					it = pib.first;*/
				}
			}

			if(!bMixed)
				it ++;
			itNext = it;
		}
	}



	inline BOOL CheckTempMarkFileOK(){
		return (NULL != m_hLocalFile && NULL != m_hPMFile);
	}

	inline HANDLE CreateFile(LPCTSTR lpszFile, DWORD dwCreationDisposition){

		return ::CreateFile(lpszFile, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE/*GENERIC_ALL*/,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
			dwCreationDisposition, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	}

	inline void MakeTempFileName(){
		 m_strTempFile = m_strLocalFile + _szTailTemp;
	}
	
	inline void MakeMarkFileName(){
		m_strPieceMarkFile = m_strTempFile + _szTailMark;
	}
	

	BOOL Attatch(__in dsMultiUrlMission *pmission){

		if(NULL == pmission || !pmission->CheckOK(IHM_LOCALFILENAME | IHM_SAVEPATH))
			return FALSE;

		//m_dwFileSize = pmission->dwFileSize;

		m_strSavePath = pmission->szLocalSavePath;

		CompletePath_SlashTail(m_strSavePath);
		
		m_strLocalShortFileName = pmission->szLocalSaveName;

		m_strLocalFile = m_strSavePath + m_strLocalShortFileName;
		MakeTempFileName();
		MakeMarkFileName();

		return TRUE;
	}

	BOOL Attatch(__in PDSHFTPMISSON pmission){

		if(NULL == pmission || !dsMultiUrlMission::CheckOK(pmission, IHM_LOCALFILENAME | IHM_SAVEPATH))
			return FALSE;

		//m_dwFileSize = pmission->dwFileSize;

		m_strSavePath = pmission->szLocalSavePath;

		CompletePath_SlashTail(m_strSavePath);

		m_strLocalShortFileName = pmission->szLocalSaveName;

		m_strLocalFile = m_strSavePath + m_strLocalShortFileName;
		MakeTempFileName();
		MakeMarkFileName();

		return TRUE;
	}


	inline BOOL CompletePath_SlashTail(CString &strPath){

		if(!PathIsDirectory(strPath))
		{
			strPath = L"";
			return FALSE;
		}

		if('\\' != strPath.GetAt(strPath.GetLength() -1)){
			strPath += _T("\\");
		}
		return TRUE;
	}

	BOOL UpdateLocalFileNameWhenExit(CString &strFile, int nChangeCount = 1){
		CString strTemp = strFile;

		CString strTail;
		strTail.Format(_T("(%d)"),nChangeCount);

		int nIndex = strTemp.ReverseFind(CHAR_COMMA);

		if(-1 == nIndex){
			strTemp += strTail;
		}
		else{
			strTemp.Insert(nIndex, strTail);
		}
		
		if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(strTemp)){
			strFile = strTemp;
			return TRUE;
		}
		else if(UpdateLocalFileNameWhenExit(strFile, ++nChangeCount))
		{
			return TRUE;
		}
	
		return FALSE;
	}
};





#endif /*__DSHFTPLOCALFILE_H__*/
