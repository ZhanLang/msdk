#pragma once

#include "..\InetFile\fsFtpConnection.h"
#include "..\InetFile\fsFtpFile.h"
#include "..\InetFile\fsHttpConnection.h"
#include "..\InetFile\fsHttpFile.h"
#include "..\InetFile\fsinet.h"
#include "..\InetFile\fsInternetFile.h"
#include "..\InetFile\fsInternetSession.h"
#include "..\InetFile\fsServerConnection.h"
#include "..\InetFile\fsURL.h"

#ifdef _DEBUG
#	pragma comment(lib,"InetFile_d.lib")
#else
#	pragma comment(lib,"InetFile_r.lib")
#endif