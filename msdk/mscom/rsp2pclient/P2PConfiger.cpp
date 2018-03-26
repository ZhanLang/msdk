#include "stdafx.h"
#include "P2PClientTool.h"
#include "P2PManager.h"
#include "P2PConfiger.h"


// -------------------------------------------------------------------
// class CConfiger
CConfiger::CConfiger(bool encry)
	: m_encry(encry)
{
	m_filename = CP2PClientTool::GetModuleDirectoryA();
	m_filename += P2P_META_DIR;
	if(CreateDirectoryA(m_filename.c_str(), NULL))
	{
		SetFileAttributesA(m_filename.c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	m_filename += P2P_CONFIGER_FILEA;
}

CConfiger::~CConfiger()
{
}

int CConfiger::Get(const char* szAppName, const char* szKeyName, OUT int & value)
{
	Json::Value jv;
	jv = m_jsroot.get(szAppName, Json::Value::null);
	if(!jv.isNull() && 
		jv.isObject() && 
		jv.isMember(szKeyName))
	{
		value = jv[szKeyName].asInt();
		return 0;
	}

	return -1;
}

int CConfiger::Get(const char* szAppName, const char* szKeyName, OUT std::string & astr)
{
	Json::Value jv;
	jv = m_jsroot.get(szAppName, Json::Value::null);
	if(!jv.isNull() && 
		jv.isObject() && 
		jv.isMember(szKeyName))
	{
		astr = jv[szKeyName].asString();
		return 0;
	}

	return -1;
}

int CConfiger::Get(const char* szAppName, const char* szKeyName, OUT std::wstring & wstr)
{
	std::string astr;
	if(0 == Get(szAppName, szKeyName, astr))
	{
		CP2PClientTool::MultiToWide(astr.c_str(), wstr);
		return 0;
	}

	return -1;
}

int CConfiger::Set(const char* szAppName, const char* szKeyName, int value)
{
	Json::Value & jv = m_jsroot[szAppName];
	jv[szKeyName] = value;

	return 0;
}

int CConfiger::Set(const char* szAppName, const char* szKeyName, const std::string astr)
{
	Json::Value & jv = m_jsroot[szAppName];
	jv[szKeyName] = astr;

	return 0;
}

int CConfiger::Set(const char* szAppName, const char* szKeyName, const std::wstring wstr)
{
	std::string astr;
	CP2PClientTool::WideToMulti(wstr.c_str(), astr);

	return Set(szAppName, szKeyName, astr);
}

int CConfiger::Del(const char* szAppName)
{
	assert(NULL != szAppName);
	m_jsroot.removeMember(szAppName);

	return 0;
}

int CConfiger::Clear()
{
	m_jsroot.clear();

	return 0;
}


int CConfiger::Load()
{
	if(!m_jsroot.isNull())
	{
		return 0;
	}

	FILE* f = NULL;
	int ret = fopen_s(&f, m_filename.c_str(), "rb");
	if(ret != 0 || f == NULL)
	{
		return -1;
	}

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);
	std::string configer(len + 1, 0);
	if(len != fread(&configer[0], 1, len, f))
	{
		fclose(f);
		return -1;
	}

	configer[len] = 0;
	std::string decry;
	if(m_encry)
	{
		CP2PClientTool::Decrypt(configer.c_str(), decry);
	}
	else
	{
		decry = configer.c_str();
	}

	fclose(f);
	Json::Reader jsreader;
	return jsreader.parse(decry, m_jsroot) ? 0 : -1;
}

int CConfiger::Save()
{
	if(m_jsroot.isNull())
	{
		return 0;
	}

	std::string str;
	if(m_encry)
	{
		CP2PClientTool::Encrypt(m_jsroot.toStyledString().c_str(), str);
	}
	else
	{
		str = m_jsroot.toStyledString();
	}

	FILE* f = NULL;
	int ret = fopen_s(&f, m_filename.c_str(), "wb");
	if(ret == 0 && f != NULL)
	{
		fwrite(str.c_str(), 1, str.size(), f);
		fflush(f);
		fclose(f);
		return 0;
	}

	return -1;
}

