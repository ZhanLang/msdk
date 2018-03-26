#include "StdAfx.h"
#include "INetBaseMgr.h"
#include "fsHttpFile_i.h"
#include "fsFtpFile_i.h"
#include "fsLocalFile_i.h"
#include "fsInternetFiles_i.h"


CINetBaseMgr::CINetBaseMgr(void)
{
}


CINetBaseMgr::~CINetBaseMgr(void)
{
}
STDMETHODIMP CINetBaseMgr::CreateInternetSession(fsInternetSession_i**lpSession)
{
	return __MS_DllGetClassObject( CLISD_fsInternetSession, re_uuidof(fsInternetSession_i) , (LPVOID *)lpSession , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateHttpConnection(fsHttpConnection_i** lpHttpCon)
{
	return __MS_DllGetClassObject(CLSID_fsHttpConnection , re_uuidof(fsHttpConnection_i) , (LPVOID *)lpHttpCon , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateHttpFile(fsHttpFile_i** lpHttpFile)
{
	return __MS_DllGetClassObject(CLSID_fsHttpFile , re_uuidof(fsHttpFile_i) , (LPVOID *)lpHttpFile , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateFtpConnection(fsFtpConnection_i** lpFtpCon)
{
	return __MS_DllGetClassObject(CLISD_fsFtpConnection , re_uuidof(fsFtpConnection_i) , (LPVOID *)lpFtpCon , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateFtpFile(fsFtpFile_i**lpFtpFile) 
{
	return __MS_DllGetClassObject(CLSID_fsFtpFile , re_uuidof(fsFtpFile_i) , (LPVOID *)lpFtpFile , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateLocalFile(fsLocalFile_i**lpLocalFile)
{
	return __MS_DllGetClassObject(CLSID_fsLocalFile , re_uuidof(fsLocalFile_i) , (LPVOID *)lpLocalFile , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateHttpFiles(fsHttpFiles_i**lpFtpFiles)
{
	return __MS_DllGetClassObject(CLISD_fsHttpFiles , re_uuidof(fsHttpFiles_i) , (LPVOID *)lpFtpFiles , NULL);
}
STDMETHODIMP CINetBaseMgr::CreateFtpFiles(fsFtpFiles_i**lpFtpFiles)
{
	return __MS_DllGetClassObject( CLSID_fsFtpFiles, re_uuidof(fsFtpFiles_i) , (LPVOID *)lpFtpFiles , NULL);
}