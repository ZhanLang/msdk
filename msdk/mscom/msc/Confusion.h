#ifndef _CONFUSION_H_
#define _CONFUSION_H_

#include "RSA.h"

extern R_RSA_PRIVATE_KEY TRUE_PRIVATE_KEY;
/////////////////////////////////////////////////////////////////////////////
class CConfusion
{
public:
	static VOID FillModulus(BYTE lpModulus[MAX_RSA_MODULUS_LEN]);
	static VOID FillPublicExponent(BYTE lpPublicExponent[MAX_RSA_MODULUS_LEN]);
	static VOID FillExponent(BYTE lpExponent[MAX_RSA_MODULUS_LEN]);
	static VOID FillPrime1(BYTE lpPrime1[MAX_RSA_PRIME_LEN]);
	static VOID FillPrime2(BYTE lpPrime2[MAX_RSA_PRIME_LEN]);
	static VOID FillPrimeExponent1(BYTE lpPrimeExponent1[MAX_RSA_PRIME_LEN]);
	static VOID FillPrimeExponent2(BYTE lpPrimeExponent2[MAX_RSA_PRIME_LEN]);
	static VOID FillCoefficient(BYTE lpCoefficient[MAX_RSA_PRIME_LEN]);
	static VOID ReverseBuffer(LPBYTE lpBuffer, DWORD dwLen);

private:
	static VOID Confusion(LPBYTE lpBuffer, DWORD dwLen, DWORD dwFactor, DWORD dwOperation);
	enum
	{
		CONFUSION_ADD,
		CONFUSION_SUB,
		CONFUSION_MUL,
		CONFUSION_DIV,
		CONFUSION_SHL,
		CONFUSION_SHR
	};
};

/////////////////////////////////////////////////////////////////////////////
#endif