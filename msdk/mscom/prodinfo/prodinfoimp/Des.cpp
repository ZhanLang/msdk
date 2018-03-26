#include "StdAfx.h"
#include "Des.h"

CDes::CDes(void)
{
}

CDes::~CDes(void)
{
}

BOOL CDes::Encrypt(const BYTE* pIn, BYTE* pOut, UINT uSize, const BYTE* pKey)
{
	if (pKey == NULL)
		return FALSE;
	if (pIn == NULL)
		return FALSE;
	if (pOut == NULL)
		return FALSE;
	DES_key_schedule schedule;
	DES_set_key_checked((DES_cblock*)pKey, &schedule); 
	UINT i;
	for (i = 0; i < uSize; i+= 8)
	{
		if (uSize - i < 8)
			break;

		DES_cblock* input = (DES_cblock*)(pIn + i);
		DES_cblock* output = (DES_cblock*)(pOut + i);
		DES_ecb_encrypt(input, output, &schedule, DES_ENCRYPT);
	}

	if (i < uSize)
		memcpy(pOut + i, pIn + i, uSize - i);

	return TRUE;
}

BOOL CDes::Decrypt(const BYTE* pIn, BYTE* pOut, UINT uSize, const BYTE* pKey)
{
	if (pKey == NULL)
		return FALSE;
	if (pIn == NULL)
		return FALSE;
	if (pOut == NULL)
		return FALSE;

	DES_key_schedule schedule;
	DES_cblock key;
	memcpy(&key, pKey, sizeof(DES_cblock));
	DES_set_key_checked(&key, &schedule); 
	UINT i;
	for (i = 0; i < uSize; i+=8)
	{
		if (uSize - i < 8)
			break;

		DES_cblock* input = (DES_cblock*)(pIn + i);
		DES_cblock* output = (DES_cblock*)(pOut + i);
		DES_ecb_encrypt(input, output, &schedule, DES_DECRYPT);
	}

	if (i < uSize)
		memcpy(pOut + i, pIn + i, uSize - i);

	return TRUE;
}

void CDes::GenKey(BYTE* pIn)
{
	DES_random_key((DES_cblock*)pIn);
}