#include "pch.h"

#include <algorithm>

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "./GLSearchDelay.h"

#include "../../SigmaCore/DebugInclude.h"


private_market::GLSearchDelay::GLSearchDelay()
: m_dwChaDbNum( 0 )
, m_dwPlusMilliseconds( 0 )
, m_dwLastRequestTime( 0 )
{
	//SetDelay();
}

private_market::GLSearchDelay::GLSearchDelay( DWORD dwChaDbNum, DWORD dwPlusMilliseconds )
: m_dwChaDbNum( dwChaDbNum )
, m_dwPlusMilliseconds( dwPlusMilliseconds )
, m_dwLastRequestTime( 0 )
{
	SetDelay();
}

private_market::GLSearchDelay::~GLSearchDelay()
{
}

bool private_market::GLSearchDelay::IsSearchLimitTime()
{
	if ( 0 == m_dwLastRequestTime )
	{
		SetDelay();
		return false;
	}

	if ( ::GetTickCount() > m_dwLastRequestTime )
	{
		SetDelay();
		return false;
	}

	return true;
}

void private_market::GLSearchDelay::SetDelay()
{
	m_dwLastRequestTime = ::GetTickCount() + m_dwPlusMilliseconds;
}

private_market::GLSearchDelayMgr::GLSearchDelayMgr()
: m_dwPlusMilliseconds( 3000 )
{
}

private_market::GLSearchDelayMgr::~GLSearchDelayMgr()
{
	m_vecSearchDelay.clear();
}

void private_market::GLSearchDelayMgr::SetDelay( DWORD dwMilliseconds )
{
	m_dwPlusMilliseconds = dwMilliseconds;
	if ( 0 == m_dwPlusMilliseconds )
	{
		m_vecSearchDelay.clear();
	}
}

bool private_market::GLSearchDelayMgr::IsSearchLimitTime( DWORD dwChaDbNum )
{
	if ( 0 == m_dwPlusMilliseconds )
	{
		return false;
	}

	GLSearchDelay sDelay( dwChaDbNum, m_dwPlusMilliseconds );
	std::vector< GLSearchDelay >::iterator iterFind = std::find( m_vecSearchDelay.begin(), m_vecSearchDelay.end(), sDelay );
	if ( iterFind == m_vecSearchDelay.end() )
	{
		m_vecSearchDelay.push_back( sDelay );
		return false;
	}

	GLSearchDelay& sDelayFind = *iterFind;
	return sDelayFind.IsSearchLimitTime();
}


DWORD private_market::GLSearchDelay::GetRemainSearchLimitTimeClient()
{
	if ( 0 == m_dwLastRequestTime )
	{
		return 0;
	}
    const DWORD dwTime = ::GetTickCount();

	if ( dwTime > m_dwLastRequestTime )
	{
		return 0;
	}

	return m_dwLastRequestTime - dwTime;
}