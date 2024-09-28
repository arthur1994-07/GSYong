#include "stdafx.h"

#include "./SubPath.h"

SUBPATH::SUBPATH(EMSERVICE_PROVIDER ServiceProvider)
    : m_ServiceProvider(ServiceProvider)
{
	APP_ROOT = _T("");

    if (m_ServiceProvider == SP_CHINA)
    {
	    SAVE_ROOT			= _T("\\流星花园\\");

        PLAYINFO_ROOT		= _T("\\流星花园\\play\\");
        UIINFO_ROOT		    = _T("\\流星花园\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\流星花园\\errlog\\");
	    CAPTURE				= _T("\\流星花园\\screen\\");
	    CHATLOG				= _T("\\流星花园\\chatlog\\");
    }
    else if (m_ServiceProvider == SP_TAIWAN)
    {
	    SAVE_ROOT			= _T("\\玦 online\\");

        PLAYINFO_ROOT		= _T("\\玦 online\\play\\");
        UIINFO_ROOT			= _T("\\玦 online\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\玦 online\\errlog\\");
	    CAPTURE				= _T("\\玦 online\\screen\\");
	    CHATLOG				= _T("\\玦 online\\chatlog\\");
    }
    else if (m_ServiceProvider == SP_HONGKONG)
    {
	    SAVE_ROOT			= _T("\\玦 online\\");

        PLAYINFO_ROOT		= _T("\\玦 online\\play\\");
        UIINFO_ROOT			= _T("\\玦 online\\ui\\");
	    DEBUGINFO_ROOT		= _T("\\玦 online\\errlog\\");
	    CAPTURE				= _T("\\玦 online\\screen\\");
	    CHATLOG				= _T("\\玦 online\\chatlog\\");
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
