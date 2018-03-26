#include <assert.h>
/*
  Free Download Manager Copyright (c) 2003-2011 FreeDownloadManager.ORG
*/

#ifndef __IF_SYSTEM_H_
#define __IF_SYSTEM_H_

extern void fsIFOnMemoryError ();

#pragma warning (disable:4127)

#define fsnew(p, T, L) while (1) { try { p = new T [L]; if (p == NULL) assert(TRUE); else break; } catch (...) {assert(TRUE);} }

#endif