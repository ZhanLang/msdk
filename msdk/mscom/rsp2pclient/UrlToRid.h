#ifndef __UrlToRid_H__
#define __UrlToRid_H__


#include <string>
#include <vector>
#include <p2pclient/IP2PClient.h>


class CUpper
{
public:
	CUpper()
	{
		for(int i = 0; i < SIZEOFV(m_upperTable); i++)
		{
			m_upperTable[i] = i;
		}

		for(int i = 0x61; i <= 0x7A; i++)
		{
			m_upperTable[i] -= 0x20;
		}
	}

	void ToUpper(char* data, size_t len)
	{
		for(size_t i = 0; i < len; i++)
		{
			data[i] = m_upperTable[data[i]];
		}
	}

private:
	char m_upperTable[128];
};



class CLower
{
public:
	CLower()
	{
		for(int i = 0; i < SIZEOFV(m_lowerTable); i++)
		{
			m_lowerTable[i] = i;
		}

		for(int i = 0x41; i <= 0x5A; i++)
		{
			m_lowerTable[i] += 0x20;
		}
	}

	void ToLower(char* data, size_t len)
	{
		for(size_t i = 0; i < len; i++)
		{
			data[i] = m_lowerTable[data[i]];
		}
	}


private:
	char m_lowerTable[128];
};


class CSHA
{
public:
	int Hash(const P2PString url, unsigned char* urlhash);

private:
	static CLower m_lower;
};


class CHttpDownload;
class CP2PRid
{
public:
	CP2PRid(const P2PString ridurl);
	~CP2PRid();

	int GetRidByUrlHash(const unsigned char* urlhash, unsigned char* rid, size_t nRidSize);
	const char* GetTracker();
	std::vector<std::string> & GetDownurl();

private:
	int HttpGetUrl(std::wstring & url);
	int GetRid(IN const char* buf, size_t size, OUT unsigned char* rid);
	int GetHashFile();

private:
	CHttpDownload*       m_http;

	const P2PString      m_ridurl;
	const unsigned char* m_urlhash;
	std::string          m_rid;
	std::string          m_tracker;
	std::string          m_hashurl;
	std::vector<std::string> m_downurl;
};


#endif
