#ifndef __P2PConfiger_H__
#define __P2PConfiger_H__


#include "json\json.h"


class CConfiger
{
public:
	CConfiger(bool encry = false);
	~CConfiger();

	int Load();
	int Save();

	int Get(const char* szAppName, const char* szKeyName, OUT int & value);
	int Get(const char* szAppName, const char* szKeyName, OUT std::string & str);
	int Get(const char* szAppName, const char* szKeyName, OUT std::wstring & str);

	int Set(const char* szAppName, const char* szKeyName, int value);
	int Set(const char* szAppName, const char* szKeyName, const std::string str);
	int Set(const char* szAppName, const char* szKeyName, const std::wstring wstr);

	int Del(const char* szAppName);
	int Clear();


private:
	Json::Value m_jsroot;
	bool        m_encry;
	std::string m_filename;
};


#endif
