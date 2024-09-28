#include "StdAfx.h"
#include "PrivateMarketShow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CPrivateMarketShow::CPrivateMarketShow (EngineDeviceMan* pEngineDevice)
	: CBasicVarTextBox(pEngineDevice)
    , m_dwGaeaID ( UINT_MAX )
	, m_bUsedMemPool( false )
    , m_strName(_T(""))
    , m_strMarket(_T(""))
{
}

CPrivateMarketShow::~CPrivateMarketShow ()
{
}