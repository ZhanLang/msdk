#pragma once

extern "C" {
BOOL init_global_encryptfile(LPCWSTR lpszWorkPath, const BYTE* lpKey, DWORD dwKeyLen, DWORD dwReserve);
BOOL uninit_global_encryptfile();

};
