#ifndef MS_UPGRADE_CODE_H
#define MS_UPGRADE_CODE_H

LONG MsUpgradeEncrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pUserID, LPCSTR pSn, DWORD dwTimeSetup, DWORD dwTimeOver, LONG lType, LONG lValidate);
LONG MsUpgradeEncrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pIn);

LONG MsUpgradeDecrypt(LPCSTR pOut, LONG lOutBufSize, LPCSTR pIn);

BOOL MsUpgradeEncryptFile(LPTSTR pInFile, LPTSTR pOutFile, BOOL bEncrypt = TRUE);


#endif MS_UPGRADE_CODE_H