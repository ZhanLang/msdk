#pragma once
class CINetBaseMgr : public IINetBaseMgr , CUnknownImp
{
public:
	UNKNOWN_IMP1(IINetBaseMgr);
	CINetBaseMgr(void);
	virtual ~CINetBaseMgr(void);

	STDMETHOD(CreateInternetSession)(fsInternetSession_i**lpSession);
	STDMETHOD(CreateHttpConnection)(fsHttpConnection_i** lpHttpCon);
	STDMETHOD(CreateHttpFile)(fsHttpFile_i** lpHttpFile);
	STDMETHOD(CreateFtpConnection)(fsFtpConnection_i** lpFtpCon);
	STDMETHOD(CreateFtpFile)(fsFtpFile_i**lpFtpFile) ;
	STDMETHOD(CreateLocalFile)(fsLocalFile_i**lpLocalFile);
	STDMETHOD(CreateHttpFiles)(fsHttpFiles_i**lpHttpFiles);
	STDMETHOD(CreateFtpFiles)(fsFtpFiles_i**lpFtpFiles) ;
};

