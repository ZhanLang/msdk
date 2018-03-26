// dsHftpEngine.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "dsHftpEngine.h"

DSHTTPFTPENGINE_API IdsHftpEngine * IdsNew_HftpEngine()
{
	return new dsHftpEngine();
}