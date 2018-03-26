#ifndef IRSXMLLORDER_H
#define IRSXMLLORDER_H

#include "objbase.h"
interface IRsXMLLoader
{
	virtual BOOL LoadXMLFiles(LPCTSTR lpPath, LPCTSTR lpFileName, PRISING_XML pBuff, UINT uSize = NULL, BOOL bLoadModuleXML = TRUE) =0;
	virtual BOOL SaveXMLFiles(LPCTSTR lpPath, LPCTSTR lpFileName, PRISING_XML pBuff, UINT uSize = NULL, BOOL bSaveModuleXML = TRUE) =0;

	virtual INT SetVerFunc(IN INT nIndex, IN BOOL bEnable, IN OUT LPTSTR lpVerFuncString, IN INT nMaxLen) =0;
	virtual BOOL GetVerFunc(IN INT nIndex, IN LPCTSTR lpVerFuncString) =0;

	virtual BOOL Release() =0;
};
#endif // IRSXMLLORDER_H	