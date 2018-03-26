
#pragma once

#include <vector>
#include "Base64.h"
#include "safebase64.h"
//rc4加密

#define		RC4_S_BOX			256
#define		RC4_S_BOX_MOD(x)	((x) & 0xff)

static unsigned char rc4_default_key[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};


class CRC4Base
{
public:
	unsigned char* GetResult()
	{
		return (unsigned char*)&m_result[0];
	}

	virtual int	GetResultLength()
	{
		return (int)m_result.size();
	}


protected:
	void RC4_Init (unsigned char *key,  int length) {
		if (key == NULL || length <= 0)
			key = rc4_default_key, length = sizeof(rc4_default_key);
		rc4_setup(key, length);
	}

	void rc4_setup(unsigned char *key,  int length) {
		for(int i = 0; i < RC4_S_BOX; m_rc4_box[i] = i++);

		int t, j = 0, k = 0;
		for(int i = 0; i < RC4_S_BOX; ++i) {
			j = RC4_S_BOX_MOD(j + m_rc4_box[i] + key[k]);
			t = m_rc4_box[i]; m_rc4_box[i] = m_rc4_box[j]; m_rc4_box[j] = t;
			if(++k >= length) k = 0;
		}
	}

	void rc4_crypt(int* box, unsigned char *data, int length) {
		int x = 0, y = 0, a, b;
		for(int i = 0; i < length; ++i) {
			x = RC4_S_BOX_MOD(x + 1);
			a = box[x];
			y = RC4_S_BOX_MOD(y + a);
			b = box[x] = box[y]; box[y] = a;
			data[i] ^= box[RC4_S_BOX_MOD(a + b)];
		}
	}

protected:
	int						m_rc4_box[RC4_S_BOX];
	std::vector<unsigned char> m_result ;

};

/*
CRC4Encrypt rc4Encrypt((unsigned char*)"magj", sizeof("magj"));
CRC4Decrypt rc4Decrypt((unsigned char*)"magj", sizeof("magj"));

rc4Encrypt.Encrypt((unsigned char*) L"/report/install?userid=maguojun" , sizeof(L"/report/install?userid=maguojun"));
rc4Decrypt.Decrypt(rc4Encrypt.GetResult(), rc4Encrypt.GetResultLenght());
wchar_t* p = (wchar_t*)rc4Decrypt.GetResult();
*/


class CRC4Encrypt : public CRC4Base
{
public:
	CRC4Encrypt(unsigned char* key, int length)
	{
		RC4_Init(key, length);
	}
	
	void Encrypt(unsigned char* data, int length)
	{
		int box[RC4_S_BOX];
		memcpy(box, m_rc4_box, sizeof(box));
		m_result.resize(length + 1, 0);
		memcpy(&m_result[0], data, length);
		rc4_crypt(box, &m_result[0], length);
	}


};

class CRC4Decrypt : public CRC4Base
{
public:
	CRC4Decrypt(unsigned char* key, int length)
	{
		RC4_Init(key, length);
	}

	void Decrypt(unsigned char* data, int length)
	{
		int box[RC4_S_BOX];
		memcpy(box, m_rc4_box, sizeof(box));
		m_result.resize(length + 1, 0);
		memcpy(&m_result[0], data, length);
		rc4_crypt(box, &m_result[0], length);
		
	}

    


};


/*

CRC4EncryptBase64 rc4Encrypt((unsigned char*)"magj", sizeof("magj"));
CRC4DecryptBase64 rc4Decrypt((unsigned char*)"magj", sizeof("magj"));

rc4Encrypt.Encrypt((unsigned char*) L"/report/install?userid=maguojun" , sizeof(L"/report/install?userid=maguojun"));
rc4Decrypt.Decrypt(rc4Encrypt.GetResult());
wchar_t* p = (wchar_t*)rc4Decrypt.GetResult();

*/

//先使用 RC4 加密，然后将结果使用 Base64 编码
template<class T >
class CRC4EncryptBase64Imp: public CRC4Base
{
public:
	CRC4EncryptBase64Imp(unsigned char* key, int length)
	{
		RC4_Init(key, length);
	}

	void Encrypt(unsigned char* data, int length)
	{
		int box[RC4_S_BOX];
		memcpy(box, m_rc4_box, sizeof(box));
		m_result.resize(length + 1, 0);
		memcpy(&m_result[0], data, length);
		rc4_crypt(box, &m_result[0], length);
		Base64Encode.Encode(&m_result[0], (int)length);
	}

	const char* GetResult()
	{
		return Base64Encode.GetResult();
	}

	int	GetResultLength()
	{
		return Base64Encode.GetResultLength();
	}

private:
	T Base64Encode;
};


template<class T>
class CRC4DecryptBase64Imp : public CRC4Base
{
public:
	CRC4DecryptBase64Imp(unsigned char* key, int length)
	{
		RC4_Init(key, length);
	}

	void Decrypt(const char* data)
	{
		Base64Decode.Decode(data);

		int box[RC4_S_BOX];
		memcpy(box, m_rc4_box, sizeof(box));
		m_result.resize(Base64Decode.GetResultLength() + 1, 0);
		memcpy(&m_result[0], Base64Decode.GetResult(), Base64Decode.GetResultLength());
		rc4_crypt(box, &m_result[0], Base64Decode.GetResultLength());
	}

private:
	T Base64Decode;
};

typedef CRC4EncryptBase64Imp<CBase64Encode>		CRC4EncryptBase64;
typedef CRC4EncryptBase64Imp<CSafeBase64Encode> CRC4EncryptSafeBase64;

typedef CRC4DecryptBase64Imp<CBase64Decode>		CRC4DecryptBase64;
typedef CRC4DecryptBase64Imp<CSafeBase64Decode> CRC4DecryptSafeBase64;