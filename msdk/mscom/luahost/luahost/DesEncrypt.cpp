#include "StdAfx.h"
#include "DesEncrypt.h"
#include "base64.h"
#include "Des.h"
CDesEncrypt::CDesEncrypt(void)
{
}


CDesEncrypt::~CDesEncrypt(void)
{
}

std::string CDesEncrypt::Encrypt(LPCSTR lpszContext, LPSTR lpszKey)
{
	DWORD dwLen = strlen(lpszContext);
	UTIL::sentry<BYTE*, UTIL::default_array_sentry> pOut = new BYTE[dwLen];
	ZeroMemory(pOut, dwLen);

	CDes des;
	des.Encrypt((const BYTE*)lpszContext, pOut, dwLen, (const BYTE*)lpszKey);
	return Base64Encode(pOut, dwLen);
}

std::string CDesEncrypt::Decrypt(LPCSTR lpszContext, LPSTR lpszKey)
{
	std::vector<BYTE> Data = Base64Decode(lpszContext, strlen(lpszContext));
	RASSERT(Data.size(), "");

	DWORD dwLen = Data.size();
	UTIL::sentry<BYTE*, UTIL::default_array_sentry> pOut = new BYTE[dwLen];
	ZeroMemory(pOut, dwLen);

	CDes des;
	des.Decrypt(&Data[0], pOut, dwLen, (const BYTE*)lpszKey);
	return (char*)pOut.m_p;
}
