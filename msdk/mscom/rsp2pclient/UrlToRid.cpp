#include "stdafx.h"
#include "P2PManager.h"
#include "UrlToRid.h"

#include "httpdownload.h"
#include "libtorrent\hasher.hpp"
#include "json\json.h"
#include "lib_json\json_value.cpp"
#include "lib_json\json_reader.cpp"
#include "lib_json\json_writer.cpp"


// -------------------------------------------------------------
// class CSHA
CLower CSHA::m_lower;

int CSHA::Hash(const P2PString url, unsigned char* urlhash)
{
	// lower code
	std::string urla;
	CP2PClientTool::WideToMulti(url, urla);
	m_lower.ToLower((char*)urla.c_str(), urla.length());

	// sha1
	libtorrent::sha1_hash hash = libtorrent::hasher(urla.c_str(), (int)urla.length()).final();
	memcpy(urlhash, &hash[0], sizeof(hash));
	return 0;
}


// -------------------------------------------------------------
// class CP2PRid
// 
// 说明：
// 询问url：http://s.Margin.cn/p2p/searchrid.php?info=[%info%]
// 		%info%={urlhash:”string”}
// 		返回内容：
// 有rid时｛rid:”rid信息”，tracker:”tracker信息”｝（密文）
// 无rid时 notfound（明文）

#define P2P_RID_TMP_FILEA		"rid.tmp"
#define P2P_RID_TMP_FILEW		_T(P2P_RID_TMP_FILEA)
#define RID_INFO_MAX_SIZE		1024

#define P2P_RID_GET_URL			L"%s?info=%s"
#define P2P_RID_NO_FIND			"notfound"


CP2PRid::CP2PRid(const P2PString ridurl)
: m_ridurl(ridurl)
{
	m_http = new CHttpDownload;
	assert(m_http != NULL);

	if(m_ridurl == NULL)
	{
		m_ridurl = P2P_DEFAULT_RID_URL;
	}
}

CP2PRid::~CP2PRid()
{
	if(m_http != NULL)
	{
		delete m_http;
		m_http = NULL;
	}
}

int CP2PRid::GetRidByUrlHash(const unsigned char* urlhash, unsigned char* rid, size_t size)
{
	//#ifdef _DEBUG
	//	const char* srid = "518d976f2b58fab00b4ef6f96fe2d9882f2e0dcd";
	//	//const char* srid = "3fea897a1cab1ddcbf38c022b4fbe35c30b5878d";
	//	CP2PClientTool::FromHex(srid, strlen(srid), rid);
	//	return 0;
	//#endif

	assert(size >= P2P_HASH_SIZE);
	m_urlhash = urlhash;

	std::wstring url;
	HttpGetUrl(url);

	BYTE  ridinfo[RID_INFO_MAX_SIZE] = {0};
	DWORD nRidInfo = SIZEOFV(ridinfo);
	HRESULT hr = ERR_HTTP_SUCCESS;
	//if(ERR_HTTP_SUCCESS == (hr = m_http->Download(url.c_str(), P2P_RID_TMP_FILEW)))
	if(ERR_HTTP_SUCCESS == (hr = m_http->PostData(url.c_str(), NULL, 0, ridinfo, &nRidInfo)))
	{
		return GetRid((const char*)ridinfo, strlen((const char*)ridinfo), rid);
	}

	return -1;
}

int CP2PRid::HttpGetUrl(std::wstring & url)
{
	char urlhash[P2P_HASH_SIZE * 2 + 1] = {0};
	CP2PClientTool::ToHex(m_urlhash, P2P_HASH_SIZE, urlhash);

	// json
	Json::Value jsroot;
	jsroot["urlhash"] = urlhash;
	std::string info = jsroot.toStyledString();

	std::string hasha;
	CP2PClientTool::Encrypt(info.c_str(), hasha);
	std::wstring hashw;
	CP2PClientTool::MultiToWide(hasha.c_str(), hashw);

	url = m_ridurl;
	url += L"?info=" + hashw;

	return 0;
}

int CP2PRid::GetRid(const char* buf, size_t size, unsigned char* rid)
{
	if(buf == NULL)
	{
		return -1;
	}

	if(stricmp(P2P_RID_NO_FIND, buf) == 0)
	{
		return -1;
	}

	// found
	std::string str;
	CP2PClientTool::Decrypt(buf, str);

	// json 
	try
	{
		Json::Value jsroot;
		Json::Reader jsreader;
		if(!jsreader.parse(str, jsroot))
		{
			return -1;
		}

		// one tracker 
		m_rid     = jsroot[P2P_JSON_RID_ID].asString();
		m_tracker = jsroot[P2P_JSON_TRACKER_ID].asString();
		m_hashurl = jsroot[P2P_JSON_HASHURL_ID].asString();

		Json::Value jsarray;
		jsarray   = jsroot[P2P_JSON_DOWNURL_ID];
		m_downurl.clear();
		for(Json::Value::iterator it = jsarray.begin(); it != jsarray.end(); it++)
		{
			m_downurl.push_back((*it).asString());
		}
	}
	catch(...)
	{
		m_rid.clear();
		m_tracker.clear();
		m_hashurl.clear();
		m_downurl.clear();
		return -2;
	}

	// hash
	CP2PClientTool::FromHex(m_rid.c_str(), m_rid.length(), rid);

	// add get hash file
	GetHashFile();
	return 0;
}

int CP2PRid::GetHashFile()
{
	if(m_rid.size() > 0 && m_hashurl.size() > 0)
	{
		std::wstring urlw;
		std::wstring filew;
		CP2PClientTool::MultiToWide(m_hashurl.c_str(), urlw);
		extern std::string hash_dirp;
		CP2PClientTool::MultiToWide((hash_dirp + m_rid + ".hash").c_str(), filew);
		return ERR_HTTP_SUCCESS == m_http->Download(urlw.c_str(), filew.c_str()) ? 
			0 : -1;
	}

	return 0;
}

const char* CP2PRid::GetTracker()
{
	return m_tracker.c_str();
}

std::vector<std::string> & CP2PRid::GetDownurl()
{
	return m_downurl;
}
