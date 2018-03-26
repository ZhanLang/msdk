#include "stdafx.h"

#include "dsTools.h"
using namespace dsTools;

#include "dsUIBase.h"

#include "dsGlobalStateMgr.h"


dsGlobalStateMgr dsGlobalStateMgr::g_fminstance;

dsUIBase * dsGlobalStateMgr::SetFocus( dsUIBase *puiFocus )
{
	return dsTools::Exchange2Objects(m_puiFocus, puiFocus);
}

dsUIBase * dsGlobalStateMgr::SetCapture( dsUIBase *puiCapture )
{

	::SetCapture(puiCapture->m_hwnd);
	return dsTools::Exchange2Objects(m_puiCapture, puiCapture);
}

dsUIBase * dsGlobalStateMgr::ReleaseCapture()
{

	::ReleaseCapture();
	dsUIBase *ret = NULL;
	return dsTools::Exchange2Objects(ret, m_puiCapture);
}

dsUIBase * dsGlobalStateMgr::SetHot( dsUIBase *puiHot )
{
	return dsTools::Exchange2Objects(m_puiHot, puiHot);
}

dsUIBase * dsGlobalStateMgr::SetMouseWheelCaptureUIPtr(__in dsUIBase *puiMouseWheel)
{
	return dsTools::Exchange2Objects(m_puiMouseWheel, puiMouseWheel);
}

void dsGlobalStateMgr::ReleaseMouseWheelCaptureUIPtr()
{
	m_puiMouseWheel = NULL;
}