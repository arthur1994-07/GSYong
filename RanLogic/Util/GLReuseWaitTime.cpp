#include "pch.h"

#include <algorithm>

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../RanLogic/Market/MarketDefine.h"

#include "./GLReuseWaitTime.h"

#include "../../SigmaCore/DebugInclude.h"


GLReuseWaitTime::GLReuseWaitTime( int nType, DWORD dwReuseWaitTime )
: m_nType( nType )
, m_dwUsableTime( 0 )
, m_dwReuseWaitTime( dwReuseWaitTime )
{
}

GLReuseWaitTime::~GLReuseWaitTime()
{
}

DWORD GLReuseWaitTime::GetRemainReuseWaitTime()
{
	if ( 0 == m_dwUsableTime )
	{
		return 0;
	}

	const DWORD dwTime = ::GetTickCount();
	if ( dwTime > m_dwUsableTime )
	{
		return 0;
	}

	return m_dwUsableTime - dwTime;
}

bool GLReuseWaitTime::IsReuseWaitTime( bool bSetReuseWaitTime )
{
	bool bReuseWaitTime = true;
	if ( 0 == m_dwUsableTime || ::GetTickCount() > m_dwUsableTime )
	{
		bReuseWaitTime = false;
	}

	if ( false == bReuseWaitTime && true == bSetReuseWaitTime )
	{
		SetReuseWaitTime();
	}

	return bReuseWaitTime;
}

void GLReuseWaitTime::SetReuseWaitTime( DWORD dwReuseWaitTime /*= GAEAID_NULL*/ )
{
	if ( GAEAID_NULL == dwReuseWaitTime )
	{
		m_dwUsableTime = ::GetTickCount() + m_dwReuseWaitTime;
	}
	else
	{
		m_dwUsableTime = ::GetTickCount() + dwReuseWaitTime;
	}
}

GLReuseWaitTimeClient::GLReuseWaitTimeClient()
{
}

GLReuseWaitTimeClient::~GLReuseWaitTimeClient()
{
}

void GLReuseWaitTimeClient::InsertReuseWaitTime()
{
	m_vecReuseWaitTime.clear();

	GLReuseWaitTime sItemBank( REUSE_WAIT_TYPE_ITEMBANK, 60000 );
	m_vecReuseWaitTime.push_back( sItemBank );

	GLReuseWaitTime sPrivateMarketItemSearch( REUSE_WAIT_TYPE_PRIVATE_MARKET_ITEM_SEARCH, private_market::GLPrivateMarketDefine::Instance()->GetSearchDelayMillisecond() );
	m_vecReuseWaitTime.push_back( sPrivateMarketItemSearch );

	GLReuseWaitTime sPrivateMarketLogSearch( REUSE_WAIT_TYPE_PRIVATE_MARKET_LOG_SEARCH, private_market::GLPrivateMarketDefine::Instance()->GetLogDelayMillisecond() );
	m_vecReuseWaitTime.push_back( sPrivateMarketLogSearch );
}

DWORD GLReuseWaitTimeClient::GetRemainReuseWaitTime( int nType )
{
	GLReuseWaitTime sDummy( nType, 0 );
	std::vector< GLReuseWaitTime >::iterator iterFind = std::find( m_vecReuseWaitTime.begin(), m_vecReuseWaitTime.end(), sDummy );
	if ( iterFind == m_vecReuseWaitTime.end() )
	{
		return 0;
	}

	GLReuseWaitTime& sReuseWaitTime = *iterFind;
	return sReuseWaitTime.GetRemainReuseWaitTime();
}

bool GLReuseWaitTimeClient::IsReuseWaitTime( int nType )
{
	GLReuseWaitTime sDummy( nType, 0 );
	std::vector< GLReuseWaitTime >::iterator iterFind = std::find( m_vecReuseWaitTime.begin(), m_vecReuseWaitTime.end(), sDummy );
	if ( iterFind == m_vecReuseWaitTime.end() )
	{
		return false;
	}

	GLReuseWaitTime& sReuseWaitTime = *iterFind;
	return sReuseWaitTime.IsReuseWaitTime( true );
}
