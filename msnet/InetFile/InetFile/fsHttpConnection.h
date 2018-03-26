
#pragma once
#include "fsServerConnection.h"
#include "fsinet.h"	

class fsHttpConnection : public fsInternetServerConnection  
{
public:
	fsInternetResult Connect (LPCTSTR pszServer, LPCTSTR pszUser, LPCTSTR pszPassword, INTERNET_PORT nPort = INTERNET_DEFAULT_HTTP_PORT);
	fsHttpConnection();
	virtual ~fsHttpConnection();
};
