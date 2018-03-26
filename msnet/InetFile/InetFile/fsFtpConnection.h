
#include "fsServerConnection.h"
#include "fsinet.h"	

class fsFtpConnection : public fsInternetServerConnection  
{
public:
	fsInternetResult Connect (LPCTSTR pszServer, LPCTSTR pszUser, LPCTSTR pszPassword, INTERNET_PORT nPort = INTERNET_DEFAULT_FTP_PORT);
	VOID UsePassiveMode (BOOL bUse);
	BOOL IsPassiveMode();
	fsInternetResult SetCurrentDirectory (LPCTSTR pszDir);
	fsFtpConnection();
	virtual ~fsFtpConnection();
private:
protected:
	VOID ReceiveExtError();
	BOOL m_bPassiveMode;	
};


