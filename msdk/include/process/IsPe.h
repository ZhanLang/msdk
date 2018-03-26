
namespace coltool{
enum
{
	PE_RET_NO_PE = 0 ,   //不是pe
	PE_RET_PE = 1 ,      //是pe
	PE_RET_DAMAED_PE = 2 ,  //是坏pe

};

static BOOL IsPeBuffer(const BYTE* pb, DWORD dwSize, DWORD &dwRet)
{
	if (pb == NULL || dwSize == 0/* || dwSize> 0x80000*/) return FALSE;

	dwRet = PE_RET_NO_PE;

	IMAGE_DOS_HEADER *pDosHead = NULL;
	IMAGE_NT_HEADERS *pPeHead = NULL;

	BOOL bIsSucceeded = FALSE;
	BOOL bIsPe = TRUE;
	BOOL bIsDamaged = FALSE;

	__try
	{
		//判断大小
		if (dwSize < (sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)))
			__leave;

		//判断是否是pe文件
		pDosHead=(IMAGE_DOS_HEADER *)pb;
		if(IMAGE_DOS_SIGNATURE!=pDosHead->e_magic)
			__leave;

		//通过Dos头找到pe头
		pPeHead=(IMAGE_NT_HEADERS *)(pb+pDosHead->e_lfanew);
		//判断pe头
		if(IMAGE_NT_SIGNATURE!=pPeHead->Signature)
			__leave;

		//经过上面的判断，认为是一个PE文件
		bIsPe = TRUE;

		//目前先排除驱动
		if(pPeHead->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_NATIVE)
			__leave;

		DWORD dwFileAlignment = pPeHead->OptionalHeader.FileAlignment;
		DWORD dwSectionAlignment = pPeHead->OptionalHeader.SectionAlignment;
		DWORD dwSectionNum = pPeHead->FileHeader.NumberOfSections;
		DWORD dwSectionAllVSize = 0;


		//节的文件对齐或内存对齐为0的不再判断，认为是pe
		if(!dwSectionAlignment || !dwFileAlignment)
			__leave;

		PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)&pPeHead->OptionalHeader + pPeHead->FileHeader.SizeOfOptionalHeader);

		DWORD dwTmp = pPeHead->OptionalHeader.SizeOfImage / dwSectionAlignment;
		DWORD dwImageSize_Alignment = pPeHead->OptionalHeader.SizeOfImage % dwSectionAlignment ? dwTmp * dwSectionAlignment + dwSectionAlignment : pPeHead->OptionalHeader.SizeOfImage;
		for ( DWORD i = 0;i < dwSectionNum;i++)
		{
			DWORD dwVSize = pSectionHeader[i].Misc.VirtualSize;
			DWORD dwVAddr = pSectionHeader[i].VirtualAddress;
			DWORD dwRSize = pSectionHeader[i].SizeOfRawData;
			DWORD dwRAddr = pSectionHeader[i].PointerToRawData;

			DWORD dwVSize_Alignment;

			if (dwVSize) 
			{
				dwTmp = dwVSize / dwSectionAlignment;
				dwVSize_Alignment = dwVSize % dwSectionAlignment ? dwTmp * dwSectionAlignment + dwSectionAlignment : dwVSize;
			}
			else
			{
				dwTmp = dwRSize / dwSectionAlignment;
				dwVSize_Alignment = dwRSize % dwSectionAlignment ? dwTmp * dwSectionAlignment + dwSectionAlignment : dwRSize;

			}

			dwSectionAllVSize += dwVSize_Alignment;

			//////////////////////////////////////////////////////////////////////////
			//VirtualAddress必须内存对齐
			if (dwVAddr % dwSectionAlignment != 0) 
			{
				bIsDamaged = TRUE;
				bIsSucceeded = TRUE;
				__leave;
			}

			//////////////////////////////////////////////////////////////////////////
			//VirtualSize不能大于镜像大小
			if(dwVSize_Alignment > dwImageSize_Alignment)
			{
				bIsDamaged = TRUE;
				bIsSucceeded = TRUE;
				__leave;
			}
			if(dwVAddr + dwVSize_Alignment > dwImageSize_Alignment)
			{
				bIsDamaged = TRUE;
				bIsSucceeded = TRUE;
				__leave;
			}

			//////////////////////////////////////////////////////////////////////////
			//内存节之间不能有空隙和重叠
			if (i == dwSectionNum - 1)
			{
				if(dwVSize_Alignment + dwVAddr != dwImageSize_Alignment)
				{
					bIsDamaged = TRUE;
					bIsSucceeded = TRUE;
					__leave;
				}
			} 
			else
			{
				if(dwVSize_Alignment + dwVAddr != pSectionHeader[i+1].VirtualAddress){
					bIsDamaged = TRUE;
					bIsSucceeded = TRUE;
					__leave;
				}
			}
		}

		//内存节总大小不等于于镜像大小
		if(dwSectionAllVSize + pSectionHeader[0].VirtualAddress != dwImageSize_Alignment)
		{
			bIsDamaged = TRUE;
			bIsSucceeded = TRUE;
			__leave;
		}

		//文件大小判断最后一个节即可,如果最后一个节的SizeOfRawData为0，则也不用判断
		dwTmp  = dwSize / dwFileAlignment;
		DWORD dwFileSize_ByFileAlignment = dwSize % dwFileAlignment ? dwTmp * dwFileAlignment + dwFileAlignment : dwSize;
		if (pSectionHeader[dwSectionNum - 1].SizeOfRawData != 0)
		{
			DWORD d1,d2;
			d1 = pSectionHeader[dwSectionNum - 1].SizeOfRawData;
			d2 = pSectionHeader[dwSectionNum - 1].PointerToRawData;
			if(pSectionHeader[dwSectionNum - 1].SizeOfRawData + pSectionHeader[dwSectionNum - 1].PointerToRawData > dwFileSize_ByFileAlignment)
			{
				bIsDamaged = TRUE;
				bIsSucceeded = TRUE;
				__leave;
			}
		}

		bIsSucceeded = TRUE;

	}
	__except (EXCEPTION_EXECUTE_HANDLER )
	{

	}
	//__finally
	{
		if (bIsSucceeded && bIsPe)
		{
			if(bIsDamaged) dwRet = PE_RET_DAMAED_PE;
			else dwRet = PE_RET_PE;
		}
	}
	//////////////////////////////////////////////////////////////////////////


	return bIsSucceeded;
}

static BOOL IsPeFile(LPCTSTR lpFileName,DWORD &dwRet)
{
	if (lpFileName == NULL) return FALSE;

	dwRet = PE_RET_NO_PE;

	HANDLE hMapping = NULL;
	BYTE* pBasePoint =NULL;

	BOOL bIsSucceeded = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	__try
	{
		hFile=CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if (hFile==INVALID_HANDLE_VALUE)
			__leave;

		//判断大小
		DWORD dwFileSize = GetFileSize(hFile,NULL);
		if (/*dwFileSize >0x80000 || */dwFileSize < (sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)))
			__leave;

		//映射文件
		if (!(hMapping = CreateFileMapping(hFile,0,PAGE_READONLY|SEC_COMMIT,0,0,0)))
			__leave;

		if (!(pBasePoint=(LPBYTE)MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0)))
			__leave;

		bIsSucceeded = IsPeBuffer(pBasePoint, dwFileSize, dwRet);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
	//__finally
	{
		if (pBasePoint)
		{
			UnmapViewOfFile(pBasePoint);
		}

		if (hMapping)
		{
			CloseHandle(hMapping);
		}

		CloseHandle(hFile);
	}
	//////////////////////////////////////////////////////////////////////////

	return bIsSucceeded;
}
}//namespace coltool