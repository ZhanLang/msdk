// managedb.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "genericdbimpl.h"
//#include <mslog/syslay/dbg.h>
#include "ado/adoconn.h"
#include "sqliteconn.h"


USE_DEFAULT_DLL_MAIN;
BEGIN_CLIDMAP
CLIDMAPENTRY_BEGIN
	//CLIDMAPENTRY_NOROT_PROGID(CLSID_DbAdoConn, CAdoConn,_T("DbAdoConn.1"))
	CLIDMAPENTRY_NOROT_PROGID(CLSID_DbSqliteConn, CSqliteConn,_T("DbSqliteConn.1"))
// CLIDMAPENTRY_PROGID(CLSID_RsDBManager, CRsDBManager,_T("RsDBManager"))
CLIDMAPENTRY_END
END_CLIDMAP_AND_EXPORTFUN

