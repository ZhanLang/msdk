#pragma once

#include <jsonrpc/jsonrpc.h>
#include <network/HttpImplement.h>

namespace msdk{;
namespace network{;

class CallJsonRpc
{
public:
	CallJsonRpc(LPCSTR lpszHost, WORD dwPort):m_host(lpszHost), m_dwPort(dwPort){}

	template<class T>
	HRESULT Call(LPCSTR fName, T t)
	{
		Json::Value query;
		msdk::network::CHttpImplement http;

		query["jsonrpc"] = "2.0";
		query["params"][(Json::Value::UInt)0] = t;
		query["method"] = fName;

		query["id"] = 111;

		http.JSonRequest(m_host.c_str(), m_dwPort, "",Json::FastWriter().write(query).c_str());
		return S_OK;
	}

	template<class R, class T0, class T1>
	Json::Value Call( LPCSTR fName,R def, T0 t , T1 t_1)
	{
		Json::Value query;
		Json::Value result;
		msdk::network::CHttpImplement http;

		query["jsonrpc"] = "2.0";
		query["id"]		 = (int)GetTickCount();
		query["method"]	 = fName;


		query["params"][(Json::Value::UInt)0] = t;
		query["params"][(Json::Value::UInt)1] = t_1;


		if (http.JSonRequest(m_host.c_str(), m_dwPort, "",Json::FastWriter().write(query).c_str()))
			return def;


		if ( ( Json::Reader().parse( (char* )http.GetRequestData(), result) && !result["result"].isNull()))
		{
			return result["result"];
		}

		return def;
	}

	template<class R, class T0, class T1, class T2>
	Json::Value Call( LPCSTR fName,R def, T0 t ,T1 t1,T2 t2)
	{
		Json::Value query;
		Json::Value result;
		msdk::network::CHttpImplement http;

		query["jsonrpc"] = "2.0";
		query["id"]		 = (int)GetTickCount();
		query["method"]	 = fName;


		query["params"][(Json::Value::UInt)0] = t;
		query["params"][(Json::Value::UInt)1] = t1;
		query["params"][(Json::Value::UInt)2] = t2;

		if (http.JSonRequest(m_host.c_str(), m_dwPort, "",Json::FastWriter().write(query).c_str()))
			return def;


		if ( ( Json::Reader().parse( (char* )http.GetRequestData(), result) && !result["result"].isNull()))
		{
			return result["result"];
		}

		return def;
	}

	template<class R, class T0, class T1, class T2, class T3>
	Json::Value Call( LPCSTR fName,R def, T0 t ,T1 t1,T2 t2 ,T3 t3)
	{
		Json::Value query;
		Json::Value result;
		msdk::network::CHttpImplement http;

		query["jsonrpc"] = "2.0";
		query["id"]		 = (int)GetTickCount();
		query["method"]	 = fName;


		query["params"][(Json::Value::UInt)0] = t;
		query["params"][(Json::Value::UInt)1] = t1;
		query["params"][(Json::Value::UInt)2] = t2;
		query["params"][(Json::Value::UInt)3] = t3;

		if (http.JSonRequest(m_host.c_str(), m_dwPort, "",Json::FastWriter().write(query).c_str()))
			return def;


		if ( ( Json::Reader().parse( (char* )http.GetRequestData(), result) && !result["result"].isNull()))
		{
			return result["result"];
		}

		return def;
	}

private:
	std::string m_host;
	WORD m_dwPort;
};

};};