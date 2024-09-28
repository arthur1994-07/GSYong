#include "stdafx.h"

#include "./SubPath.h"

SUBPATH::SUBPATH(EMSERVICE_PROVIDER ServiceProvider)
    : m_ServiceProvider(ServiceProvider)
{
	APP_ROOT = _T("");

    if (m_ServiceProvider == SP_CHINA)
    {
	    SAVE_ROOT			= _T("\\���ǻ�԰\\");

        PLAYINFO_ROOT		= _T("\\���ǻ�԰\\play\\");
        UIINFO_ROOT		    = _T("\\���ǻ�԰\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\���ǻ�԰\\errlog\\");
	    CAPTURE				= _T("\\���ǻ�԰\\screen\\");
	    CHATLOG				= _T("\\���ǻ�԰\\chatlog\\");
    }
    else if (m_ServiceProvider == SP_TAIWAN)
    {
	    SAVE_ROOT			= _T("\\�i online\\");

        PLAYINFO_ROOT		= _T("\\�i online\\play\\");
        UIINFO_ROOT			= _T("\\�i online\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\�i online\\errlog\\");
	    CAPTURE				= _T("\\�i online\\screen\\");
	    CHATLOG				= _T("\\�i online\\chatlog\\");
    }
    else if (m_ServiceProvider == SP_HONGKONG)
    {
	    SAVE_ROOT			= _T("\\�i online\\");

        PLAYINFO_ROOT		= _T("\\�i online\\play\\");
        UIINFO_ROOT			= _T("\\�i online\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\�i online\\errlog\\");
	    CAPTURE				= _T("\\�i online\\screen\\");
	    CHATLOG				= _T("\\�i online\\chatlog\\");
    }
    else
    {
	    SAVE_ROOT			= _T("\\RanOnline\\");

        PLAYINFO_ROOT		= _T("\\RanOnline\\play\\");
        UIINFO_ROOT		    = _T("\\RanOnline\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\RanOnline\\errlog\\");
	    CAPTURE				= _T("\\RanOnline\\screen\\");
	    CHATLOG				= _T("\\RanOnline\\chatlog\\");
    }
	
	FULL_PATH_VISUAL_MATERIAL   = _T("");
}

SUBPATH::~SUBPATH()
{
}
