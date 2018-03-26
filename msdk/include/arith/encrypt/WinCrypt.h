#pragma once

#include <vector>


namespace winfunc {

#define		RC4_S_BOX			256
#define		RC4_S_BOX_MOD(x)	((x) & 0xff)

	static unsigned char rc4_default_key[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};

	class WinCrypt {
	public:
		void RC4_Init (unsigned char *key,  int length) {
			if (key == NULL || length <= 0)
				key = rc4_default_key, length = sizeof(rc4_default_key);
			rc4_setup(key, length);
		}

		char* RC4_Decrypt(unsigned char* data, int length) {
			int box[RC4_S_BOX];
			memcpy(box, m_rc4_box, sizeof(box));
			m_result.resize(length + 1, 0);
			memcpy(&m_result[0], data, length);
			rc4_crypt(box, &m_result[0], length);
			m_result[length] = '\0';
			return (char*)&m_result[0];
		}

		char* RC4_Encrypt(unsigned char* data, int length) {
			int box[RC4_S_BOX];
			memcpy(box, m_rc4_box, sizeof(box));
			m_result.resize(length + 1, 0);
			memcpy(&m_result[0], data, length);
			rc4_crypt(box, &m_result[0], length);
			m_result[length] = '\0';
			return (char*)&m_result[0];
		}

		char* Base64_Encrypt(unsigned char* data, int length) {
			base64_encode(data, length);
			return (char*)&m_result[0];
		}

		// just for lua
		char* RC4_Decrypt_File(LPCTSTR path) {
			FILE *pf = _tfopen(path, _T("rb"));
			if (pf == NULL) return NULL;

			unsigned char *buff   = NULL;
			char*		   result = NULL;
			do {
				if (fseek(pf, 0, SEEK_END) != 0)
					break;

				int length = ftell(pf);
				if (length < 0)
					break;
				if ((buff = new (std::nothrow) unsigned char[length + 1]) == NULL)
					break;

				if (fseek(pf, 0, SEEK_SET) != 0)
					break;
				if (fread(buff, sizeof(unsigned char), length, pf) != length)
					break;
				result = RC4_Decrypt(buff, length);
			} while (0);
			if (buff != NULL)
				delete [] buff;

			fclose(pf);
			return result;
		}

		bool RC4_Encrypt_File(LPCTSTR srcfile, LPCTSTR dstfile) {
			FILE *pf  = _tfopen(srcfile, _T("rb")),
				 *pf2 = _tfopen(dstfile, _T("wb"));
			if (pf == NULL || pf2 == NULL) 
				return NULL;

			unsigned char *buff   = NULL;
			bool		   result = false;
			do {
				if (fseek(pf, 0, SEEK_END) != 0)
					break;

				int length = ftell(pf);
				if (length < 0)
					break;
				if ((buff = new (std::nothrow) unsigned char[length + 1]) == NULL)
					break;

				if (fseek(pf, 0, SEEK_SET) != 0)
					break;
				if (fread(buff, sizeof(unsigned char), length, pf) != length)
					break;
				char* temp = RC4_Encrypt(buff, length);

				if (fwrite(temp, sizeof(unsigned char), length, pf2) != length)
					break;
				result = true;
			} while (0);
			if (buff != NULL)
				delete [] buff;

			fclose(pf);
			fclose(pf2);
			return result;
		}

		const char* RC4_Encrypt_UTF8_Base64_Code (LPCSTR data, int length) {
			wchar_t* wbuff  = NULL;
			char*	 ubuff  = NULL;
			char*	 result = NULL;
			do {
				int wlen = ::MultiByteToWideChar(CP_ACP, 0, data, length, NULL, 0);
				if (wlen <= 0)
					break;
				if ((wbuff = new (std::nothrow) wchar_t[wlen + 1]) == NULL)
					break;
				::MultiByteToWideChar(CP_ACP, 0, data, length, wbuff, wlen + 1);

				int ulen = ::WideCharToMultiByte(CP_UTF8, 0, wbuff, wlen, NULL, 0, NULL, NULL);
				if (ulen <= 0)
					break;
				if ((ubuff = new (std::nothrow) char[ulen + 1]) == NULL)
					break;
				::WideCharToMultiByte(CP_UTF8, 0, wbuff, wlen, ubuff, ulen + 1, NULL, NULL);

				int box[RC4_S_BOX];
				memcpy(box, m_rc4_box, sizeof(box));
				rc4_crypt(box, (unsigned char*)ubuff, ulen);

				base64_encode((unsigned char*)ubuff, ulen);

				result = (char*)&m_result[0];
			} while (0);
			if (wbuff != NULL)
				delete [] wbuff;
			if (ubuff != NULL)
				delete [] ubuff;

			return result;
		}

	private:
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

		void base64_encode(unsigned char* data, int length) {
			char*		   table	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			unsigned char  buff[3];

			m_result.resize(int(length * 1.351 + 2), 0);

			int i = 0, j = 0, k = 0;
			char* result = (char*)&m_result[0];
			while (i < length) {
				buff[2] = data[i++];
				buff[1] = i < length ? data[i] : '\0'; ++i;
				buff[0] = i < length ? data[i] : '\0'; ++i;

				result[j++] = table[buff[2] >> 2];
				result[j++] = table[((buff[2] & 0x03) << 4) + ((buff[1] & 0xf0) >> 4)];
				result[j++] = table[((buff[1] & 0x0f) << 2) + ((buff[0] & 0xc0) >> 6)];
				result[j++] = table[buff[0] & 0x3F];

				if ((k += 4) == 76) {
					result[j++] = '\n';
					k = 0;
				}
			}
			result[j] = '\0';
			while (i-- > length) result[--j] = '=';
		}

	private:
		int						m_rc4_box[RC4_S_BOX];
		std::vector<unsigned char>	m_result;
	};
}