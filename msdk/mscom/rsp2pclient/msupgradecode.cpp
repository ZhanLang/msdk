#include "stdafx.h"
#include "MsUpgradeCode.h"
#include "Base64.h"
#include "time.h"

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ASSERT
	#include "assert.h"
	#define ASSERT assert
#endif

#define PARAMETER_NAME_ID			"id="
#define PARAMETER_NAME_SN			"&sn="
#define PARAMETER_NAME_SETUP_TIME	"&Time_Setup="
#define PARAMETER_NAME_OVER_TIME	"&Time_OverTime="
#define PARAMETER_NAME_TYPE			"&type="
#define PARAMETER_NAME_VALIDATE		"&Validate="

//加密的结果：
//	6字节加解密随机数 ＋ 加密后的内容
#define CONVER_BOOK_SIZE		256

char g_szConvertBookD[CONVER_BOOK_SIZE + 1] = (  "welcome Margin*youarelawless!y2a"
											      "3n4g5Y6U7q8i@S9I0N#A.C%O(M-)<>AB"
											      "I993JIEM,;'{jkliewaqlsiqomv.z^iw"
											      "aql}-_=+)_(l;2j2f90aslkjflkasjas"
											      "32092JKLSJFbASAUI/Z/A[/,./|@~`FS"
											      "'.Z,MF920SDLAFJKAL9320QFFMmlajfl"
											      ",.<>//|348q9729|fjlail3jo798,ksa"
											      "fa302-s;akfa;=_++-0-_))0-0-p23is"
												);

char g_szConvertBook[CONVER_BOOK_SIZE + 1] = (   "welcome Margin*youarelawless!y2a" 
												  "$n4g5Y6U7q8i@S9I0N#A.C%O(M-)<>AB" 
												  "I99*JIEM,;'{jkliewaqlsiqomv.z^iw" 
												  "aql}-_=+)_(l;2j@f90aslkjflkasjas" 
												  "6j09kJKLSJFbASAUI/Z/A[/,./|@~`FS" 
												  "'.Z,MF920SDLAFJKAL9320QFFMmlajfl" 
												  ",.<>//|348q9729|fjlail3jo798,ksa" 
												  "fa302-s;akfa;=_++-0-_))0-0-p^bis" 
												);

LONG MsUpgradeEncrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pUserID, LPCSTR pSn, DWORD dwTimeSetup, DWORD dwTimeOver, LONG lType, LONG lValidate)
{
	ZeroMemory((void*)pOut, lOutBufSize);
	if(!pOut || !pUserID || !pSn)
		return 0;

	char szSetupTime[33] = {0};
	char szOverTime[33] = {0};
	char szType[33] = {0};
	char szValidate[33] = {0};
	sprintf(szSetupTime, "%d", dwTimeSetup);
	sprintf(szOverTime, "%d", dwTimeOver);
	sprintf(szType, "%d", lType);
	sprintf(szValidate, "%d", lValidate);

	size_t lNeedLength = 0;
	lNeedLength += strlen(PARAMETER_NAME_ID) + strlen(pUserID);	//id=12345678&sn=xxxxxxxxx;xxxxx&Time_Setup=98765432
	lNeedLength += strlen(PARAMETER_NAME_SN) + strlen(pSn);
	lNeedLength += strlen(PARAMETER_NAME_SETUP_TIME) + strlen(szSetupTime);
	lNeedLength += strlen(PARAMETER_NAME_OVER_TIME) + strlen(szOverTime);
	lNeedLength += strlen(PARAMETER_NAME_TYPE) + strlen(szType);
	lNeedLength += strlen(PARAMETER_NAME_VALIDATE) + strlen(szValidate);

	char* szTemp = new char[lNeedLength + 1];
	memset(szTemp, 0, lNeedLength + 1);

	//填充内容
	strcat(szTemp, PARAMETER_NAME_ID);
	strcat(szTemp, pUserID);
	strcat(szTemp, PARAMETER_NAME_SN);
	strcat(szTemp, pSn);
	strcat(szTemp, PARAMETER_NAME_SETUP_TIME);
	strcat(szTemp, szSetupTime);
	strcat(szTemp, PARAMETER_NAME_OVER_TIME);
	strcat(szTemp, szOverTime);
	strcat(szTemp, PARAMETER_NAME_TYPE);
	strcat(szTemp, szType);
	strcat(szTemp, PARAMETER_NAME_VALIDATE);
	strcat(szTemp, szValidate);

	//加密
	LONG lRet = MsUpgradeEncrypt(pOut, lOutBufSize, szTemp);

	delete[] szTemp;

	return lRet;
}

LONG MsUpgradeEncrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pIn)
{
	ZeroMemory((void*)pOut, lOutBufSize);
	if(!pOut || !pIn)
		return 0;
	LONG lInLength = (LONG)strlen(pIn);	// zcg+, 优化. 当 pIn 很大时. 在循环中将很耗 CPU. 
	if(1 > lInLength)
		return 0;

	LONG lNeedLength = 7 + lInLength;	// 加解密随机数在开始6个字符, 最后一个字节是校验码
	LONG lBeforeBase64Length = lNeedLength;
	//BASE64后的长度
	lNeedLength = (lNeedLength + 2) / 3;
	lNeedLength *= 4;

	if(lNeedLength >= lOutBufSize)
		return 0 - lNeedLength - 1;	//返回需要的内存大小

	char* szTemp = new char[lNeedLength + 1];
	memset(szTemp, 0, lNeedLength + 1);

	//生成随机数
	srand( (unsigned)time( NULL ) );
	WORD wTemp;
	for(int i = 0; i < 6; i++)
	{
		wTemp = rand();
		szTemp[i] = 0x00ff & wTemp;
	}

	//填充内容
	strcat(szTemp + 6, pIn);
	//计算校验和
	for(int i = 0; i < (int)lInLength; i++)
	{
		szTemp[lBeforeBase64Length-1] += pIn[i];
	}

	//加密
	g_szConvertBook[32] = '3';
	g_szConvertBook[67] = '3';
	g_szConvertBook[111] = '2';
	g_szConvertBook[128] = '3';
	g_szConvertBook[129] = '2';
	g_szConvertBook[132] = '2';
	g_szConvertBook[252] = '2';
	g_szConvertBook[253] = '3';
	int nRandomIndex = 0;
	for(int i = 6; i < lBeforeBase64Length; i++)
	{
		szTemp[i] ^= g_szConvertBook[(BYTE)szTemp[nRandomIndex]];
		nRandomIndex ++;
		if(nRandomIndex > 5)
			nRandomIndex = 0;
	}

	//BASE64
	memset((void*)pOut, 0, lOutBufSize);
	LONG lOutLength = Base64Encode((BYTE*)szTemp, lBeforeBase64Length, (char*)pOut);

	//‘+’号在传输时有问题，变成‘-’就没问题了。
	char* pFind = strchr((char*)pOut, '+');
	while(pFind)
	{
		*pFind = '-';
		pFind = strchr(pFind, '+');
	}

	delete[] szTemp;

	return lOutLength;
}

LONG MsUpgradeDecrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pIn)
{
	ZeroMemory((void*)pOut, lOutBufSize);
	LONG lOutNeedSize = (LONG)strlen(pIn) * 3 / 4 - 6;
	if(lOutNeedSize < 1)
		return 0;
	if(lOutNeedSize >= lOutBufSize)
		return 0 - lOutNeedSize - 1;	//返回需要的内存大小

	//‘+’号在传输时有问题，现在把‘-’变回‘+’。
	char* pFind = strchr((char *)pIn, '-');
	while(pFind)
	{
		*pFind = '+';
		pFind = strchr(pFind, '-');
	}

	char* szTemp = new char[strlen(pIn)];
	memset(szTemp, 0, strlen(pIn));

	//BASE64
	LONG lBase64After = Base64Decode((const char *)pIn, (BYTE*)szTemp);
	if(lBase64After > 7)	//6字节的随机数，1字节的校验和
	{
		//解密
		BYTE bytCheck = 0;
		int nRandomIndex = 0;
		for(int i = 6; i < lBase64After; i++)
		{
			szTemp[i] ^= g_szConvertBookD[(BYTE)szTemp[nRandomIndex]];
			nRandomIndex ++;
			if(nRandomIndex > 5)
				nRandomIndex = 0;

			//计算校验和
			if(i < lBase64After - 1)
				bytCheck += szTemp[i];
		}

		//判校验和
		if(bytCheck != (BYTE)szTemp[lBase64After - 1])
			lBase64After = 7;

		memcpy((void*)pOut, szTemp + 6, lBase64After - 7);
	}
	delete[] szTemp;

	return lBase64After - 7;
}

//hxj: 加密或者解密一个文件，注意文件不能太大
BOOL MsUpgradeEncryptFile(LPTSTR pInFile, LPTSTR pOutFile, BOOL bEncrypt/*=TRUE*/)
{
	HANDLE hFile = CreateFile(pInFile, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile) return FALSE;

	BOOL bSuccess = FALSE;
	DWORD dwLength = 0;
	DWORD dwInFileSize = GetFileSize(hFile, NULL);
	DWORD dwOutFileSize = dwInFileSize * 2 + 12;	//密文可能会比明文长，需要额外空间，理论上好像只需要*4/3+12，保险起见多申请一些
	ASSERT(dwInFileSize <= 10240);	//太大的文件下面的ReadFile可能一次读不回来，并不适用
	char* pszInText = new char[dwInFileSize + 1];
	ZeroMemory(pszInText, dwInFileSize + 1);
	char* pszOutText = new char[dwOutFileSize];
	ZeroMemory(pszOutText, dwInFileSize + 1);
	if (pszInText && pszOutText &&
		ReadFile(hFile, pszInText, dwInFileSize, &dwLength, NULL))
	{
		if (bEncrypt)
			MsUpgradeEncrypt(pszOutText, dwOutFileSize, pszInText);
		else
			MsUpgradeDecrypt(pszOutText, dwInFileSize + 1, pszInText);
		SetFileAttributes(pOutFile, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(pOutFile);
		HANDLE hDecryptedFile = CreateFile(pOutFile,
			GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
		if (INVALID_HANDLE_VALUE != hDecryptedFile)
		{
			bSuccess = WriteFile(hDecryptedFile, pszOutText, strlen(pszOutText) + 1,
				&dwInFileSize, NULL);
			CloseHandle(hDecryptedFile);
		}
		delete[] pszInText;
		delete[] pszOutText;
	}
	CloseHandle(hFile);
	return bSuccess;
}
