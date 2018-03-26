#pragma once
#include <windows.h>
#include <openssl/des.h>

#define HASH_CODE_LEN 28
#define SEPARATOR	";;;"
class CDes
{
public:
	CDes(void);
	~CDes(void);
	BOOL Encrypt(const BYTE* pIn, BYTE* pOut, UINT uSize, const BYTE* pKey);
	BOOL Decrypt(const BYTE* pIn, BYTE* pOut, UINT uSize, const BYTE* pKey);
	void GenKey(BYTE* pIn);
};
