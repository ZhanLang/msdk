#pragma once

#include <mscom/SrvBaseImp.h>
#include <msconfig/IMsConfig.h>

class CMsConfig : public IMsConfig, public CSrvPluginImp<CMsConfig>
{
public:
	UNKNOWN_IMP3_( IMsPlugin , IMsPluginRun,IMsConfig);
};