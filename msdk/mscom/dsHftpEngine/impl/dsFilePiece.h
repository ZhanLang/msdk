/************************************************************************/
/* 
Author:

lourking. All rights reserved.

Create Time:

	3,25th,2014

Module Name:

	dsNetFilePiece.h 

Abstract: 文件、数据切片


*/
/************************************************************************/


#ifndef __DSFILEPIECE_H__
#define __DSFILEPIECE_H__

#include "dsTools.h"



#define FP_NORMAL		0x0000
#define FP_ERROR		0x0001
#define FP_DATAOK		0x0002
#define FP_USED			0x0004
#define FP_WRITED		FP_USED
#define FP_MARKED		0x0008
#define FP_COMPLETE		(FP_WRITED | FP_MARKED)

#define NFP_NORMAL		FP_NORMAL
#define NFP_ERROR		FP_ERROR
#define NFP_DATAOK		FP_DATAOK
#define NFP_USED		FP_USED
#define NFP_WRITED		FP_WRITED
#define NFP_MARKED		FP_MARKED
#define NFP_SUSPEND		0x0010
#define NFP_COMPLETE	FP_COMPLETE



#define FORMAT_PIECE _T("<begin=%d,end=%d,status=%d>")
#define TEXT_PIECEHEAD	_T("<")
#define TEXT_PIECETAIL	_T(">")
#define TEXT_COMMA		_T(",")

#define CHAR_PIECEHEAD	'<'
#define CHAR_PIECETAIL	'>'
#define CHAR_COMMA		','


static const TCHAR _szPiece_Format[] = _T("<begin=%d,end=%d,status=%d>\r\n");
static const TCHAR _szPiece_Begin[] = _T("begin=");
static const TCHAR _szPiece_End[] = _T("end=");
static const TCHAR _szPiece_Status[] = _T("status=");

#define TEXTLEN_PIECEBEGIN	5
#define TEXTLEN_PIECEEND	3
#define TEXTLEN_PIECESTATUS	6

#define MINLEN_PIECEMARK_STR	22
#define MAXLEN_PIECEMARK_STR	50 //22 + 10 + 10 + 8


#define PIECELINK_SEAMLESS_NULL		0x0000
#define PIECELINK_SEAMLESS_LEFT		0x0001
#define PIECELINK_SEAMLESS_RIGHT	0x0002


#define PIECELINK_SEMLESS_TWOWAY	PIECELINK_SEAMLESS_LEFT | PIECELINK_SEAMLESS_RIGHT


class dsLocalFilePiece
{
public:
	DWORD m_dwBegin;
	DWORD m_dwEnd;
	int m_nStatus;

private:
	CString m_strPiece;

public:
	dsLocalFilePiece():m_dwBegin(0),m_dwEnd(0),m_nStatus(FP_NORMAL)
	{

	}

	dsLocalFilePiece(DWORD dwBegin, DWORD dwEnd, int nStatus)
	{
		SetMark(dwBegin, dwEnd, nStatus);	
	}

	dsLocalFilePiece(LPCTSTR lpszPiece){

	}

	//dsLocalFilePiece()
	//{
	//	ZeroMemory(this, sizeof dsLocalFilePiece - sizeof CString);
	//}

	BOOL SetMark(DWORD dwBegin, DWORD dwEnd, int nStatus = FP_NORMAL){

		if(dwBegin < dwEnd)
		{
			m_dwBegin = dwBegin;
			m_dwEnd = dwEnd;
			m_nStatus = nStatus;

			Range2Str();

			return TRUE;
		}

		return FALSE;
	}


	int SetMark(LPCTSTR lpszPiece)//返回截取单个碎片的长度
	{

		int nLenght = 0;

		if(NULL == lpszPiece)
			return -1;
		

		LPCTSTR lpszHead = _tcschr(lpszPiece, CHAR_PIECEHEAD);

		if(NULL == lpszHead)
			return 0;

		LPCTSTR lpszTail = _tcschr(lpszHead, CHAR_PIECETAIL);

		if(NULL == lpszTail)
			return 0;


		nLenght = lpszTail - lpszHead + 1;

		//CString s = _T("aaaa");
	//	m_strPiece.AssignCopy(nLenght, lpszHead);

		Str2Range();

		return nLenght;
	}

	inline int GetStatus(){
		return m_nStatus;
	}

	inline int GetSize(){
		return m_dwEnd - m_dwBegin;
	}

	inline BOOL IsWrited(){
		return FP_WRITED == m_nStatus;
	}


	BOOL Mix(dsLocalFilePiece *piece){

		if(!IsWrited() || !piece->IsWrited())
			return FALSE;

		if(m_dwEnd == piece->m_dwBegin)
		{
			m_dwEnd = piece->m_dwEnd;
			return TRUE;
		}
		else if(m_dwBegin == piece->m_dwEnd){

			m_dwBegin = piece->m_dwBegin;

			return TRUE;

		}

		return FALSE;
	}

	LPCTSTR GetPieceStr(){
		/*if(!Range2Str())
			return NULL;*/
		return m_strPiece;
	}

	int GetPieceStrLenght(){
		return m_strPiece.GetLength();
	}

public:

	void Range2Str(){
		m_strPiece.Format(_szPiece_Format, m_dwBegin, m_dwEnd, m_nStatus);
	}

	BOOL Str2Range(){

		if(!CheckFormat())
			return FALSE;

		GetPieceAttrValue(_szPiece_Begin, m_dwBegin);
		GetPieceAttrValue(_szPiece_End, m_dwEnd);
		GetPieceAttrValue(_szPiece_Status, m_nStatus);

		return TRUE;
	}

	inline BOOL CheckFormat(){

		int nPieceLen = m_strPiece.GetLength();

		if(nPieceLen <= MINLEN_PIECEMARK_STR || nPieceLen > MAXLEN_PIECEMARK_STR)
			return FALSE;

		int nIndexHead = m_strPiece.Find(CHAR_PIECEHEAD);
		if(-1 == nIndexHead)
			return FALSE;

		int nIndexTail = m_strPiece.Find(CHAR_PIECETAIL);

		if(nIndexTail <= nIndexHead)
			return FALSE;

		return TRUE;
	}

	template<typename T>
	inline BOOL GetPieceAttrValue(LPCTSTR lpszAttr, T &_Value){
		if(NULL == lpszAttr)
			return FALSE;

		int nScrLenght = _tcslen(lpszAttr);

		int nIndexAttr = m_strPiece.Find(lpszAttr);

		if(-1 == nIndexAttr)
			return FALSE;

		int nIndexAttrEnd = m_strPiece.Find(CHAR_COMMA, nIndexAttr);

		nIndexAttr += nScrLenght;

		CString strValue;

		if(-1 == nIndexAttrEnd)
		{
			strValue = m_strPiece.Mid(nIndexAttr, m_strPiece.GetLength() - 1 - nIndexAttr);
		}
		else{
			strValue = m_strPiece.Mid(nIndexAttr, nIndexAttrEnd - nIndexAttr);
		}

		_Value = _tstoi(strValue);
		
		return TRUE;
	}
};

class dsNetFilePiece
{
public:
	DWORD m_dwBegin;
	DWORD m_dwEnd;
	byte *m_data;

	HANDLE m_hThreadDownload;
	int m_nStatus;

	dsNetFilePiece(DWORD dwBegin, DWORD dwEnd, int nStatus)
	{
		ZeroMemory(this, sizeof dsNetFilePiece);

		m_nStatus = nStatus;
		SetMark(dwBegin, dwEnd);

		/*if(NFP_NORMAL == nStatus){
			Allocate(dwBegin, dwEnd);
		}*/
	}


	dsNetFilePiece()
	{
		ZeroMemory(this, sizeof dsNetFilePiece);
	}

	BOOL SetMark(DWORD dwBegin, DWORD dwEnd){

		if(dwBegin < dwEnd)
		{
			m_dwBegin = dwBegin;
			m_dwEnd = dwEnd;
			return TRUE;
		}

		return FALSE;
	}

	BOOL Allocate(DWORD dwBegin, DWORD dwEnd){
		
		if(NULL != m_data || FP_NORMAL != m_nStatus)
			return FALSE;

		ZeroMemory(this, sizeof dsNetFilePiece);
		if(dwBegin < dwEnd)
		{
			m_dwBegin = dwBegin;
			m_dwEnd = dwEnd;
			m_data = new byte[m_dwEnd - m_dwBegin];
		}	

		return TRUE;
	}

	//void Burn(){
	//	if(NULL != m_data)
	//		delete[] m_data;

	//	ZeroMemory(this, sizeof dsNetFilePiece);
	//}

	void JustBurnData(){
		if(NULL != m_data)
			delete[] m_data;

		m_data = NULL;
	}

	inline int GetStatus(){
		return m_nStatus;
	}

	inline int GetSize(){
		return m_dwEnd - m_dwBegin;
	}


	inline BOOL IsUsed(){
		return NFP_USED == m_nStatus;
	}

	inline int SetStatus(int nStatus){
		
		return dsTools::Exchange2Objects(m_nStatus, nStatus);
	}

	BOOL Mix(dsNetFilePiece *piece){

		if(!IsUsed() || !piece->IsUsed())
			return FALSE;

		if(m_dwEnd == piece->m_dwBegin)
		{
			JustBurnData();
			piece->JustBurnData();

			m_dwEnd = piece->m_dwEnd;

			return TRUE;
		}
		else if(m_dwBegin == piece->m_dwEnd){

			JustBurnData();
			piece->JustBurnData();

			m_dwBegin = piece->m_dwBegin;

			return TRUE;

		}

		return FALSE;
	
	}
};




typedef map<DWORD, dsLocalFilePiece> MAP_LOCALFILEPIECE;
typedef map<DWORD, dsNetFilePiece> MAP_NETFILEPIECE;



#endif /*__DSFILEPIECE_H__*/