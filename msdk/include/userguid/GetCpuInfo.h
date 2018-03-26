#ifndef _MSGETCPUINFO_H
#define _MSGETCPUINFO_H

namespace msdk {

//#define RSCPU_SIZE	8
#define RSCPU_SIZE	16
	class CGetCpuInfo
	{
	public:
		static bool GetCpuId(BYTE pvendor[RSCPU_SIZE])
		{
			/*
			bool bGet = false;

			unsigned long s1=0, s2=0;
			BYTE byCpuid[8];

			__try
			{
				_asm
				{
					mov eax, 01h
					xor edx, edx
					cpuid
					mov s1, edx
					mov s2, eax
				}

				//bool isSupport = (s1&(1<<8));	//edx是否为1代表cpu是否存在序列号
				//if(!isSupport)
				//	return false;
				
				memcpy(byCpuid, &s1, sizeof(s1));
				memcpy(byCpuid+sizeof(s1), &s2, sizeof(s2));
				bGet = true;
			}
			__except(EXCEPTION_EXECUTE_HANDLER) 
			{ 
				bGet = false; 
			}

			if(bGet)
			{
				memcpy(cpuId, byCpuid, 8);
			}
			*/
			/*
			char pvendor[16];
	INT32 dwBuf[4];

 	__cpuid(dwBuf, 0);
 	*(INT32*)&pvendor[0] = dwBuf[1];    // ebx: 前四个字符
 	*(INT32*)&pvendor[4] = dwBuf[3];    // edx: 中间四个字符
 	*(INT32*)&pvendor[8] = dwBuf[2];    // ecx: 最后四个字符
 	pvendor[12] = '\0';
 
 	__cpuid(dwBuf, 0x1);
 	int family = (dwBuf[0] >> 8) & 0xf;
 
 	char pbrand[64];
 	__cpuid(dwBuf, 0x80000000);

	if (dwBuf[0] < 0x80000004)
 	{
 		return 0;
 	}
 
 	__cpuid((INT32*)&pbrand[0], 0x80000002);    // 前16个字符
 	__cpuid((INT32*)&pbrand[16], 0x80000003);    // 中间16个字符
 	__cpuid((INT32*)&pbrand[32], 0x80000004);    // 最后16个字符
 	pbrand[48] = '\0';


	__cpuidex(dwBuf, 1, 1);
	char szTmp[33]={NULL};
	sprintf(szTmp, "%08X%08X", dwBuf[3], dwBuf[0]);

	printf("%s \n", szTmp );

	return 0;

*/
			return bGet;
		}
	};

} //namespace msdk

#endif	//_MSGETCPUINFO_H