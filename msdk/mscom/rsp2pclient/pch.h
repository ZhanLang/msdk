// pch.h

#ifndef _DOWNLOADER_PCH_H_
#define _DOWNLOADER_PCH_H_

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <iostream>
#include <string>
#include <cstdio>

#include "../opensrc/libtorrent-rasterbar-0.15.0/include/libtorrent/entry.hpp"
#include "../opensrc/libtorrent-rasterbar-0.15.0/include/libtorrent/session.hpp"
#include "../opensrc/libtorrent-rasterbar-0.15.0/include/libtorrent/alert_types.hpp"
#include "../opensrc/libtorrent-rasterbar-0.15.0/include/libtorrent/extensions/ut_pex.hpp"
#include "../opensrc/libtorrent-rasterbar-0.15.0/include/libtorrent/bencode.hpp"

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#endif