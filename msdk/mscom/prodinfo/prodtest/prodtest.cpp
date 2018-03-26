// prodtest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "prodinfo/ProdInfoImp.h"
#include "stdafx.h"  
#include "stdio.h"  
#include "string.h"  
#include <string>
#include <vector>


int _tmain(int argc, _TCHAR* argv[])
{

	SetSubProdInfo(L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"1", L"admin");

	TCHAR szValue[MAX_PATH] = {0};
	GetSubProdInfo(L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}", L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"1",szValue,MAX_PATH);
	
	DelSubProdInfo(L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"version");
	DelSubProd(L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}",L"{4B2932E8-320F-4c06-B122-F6F7A8A3F425}");
	return 0;
}

