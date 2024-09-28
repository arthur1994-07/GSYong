#include "../pch.h"
#include "./NetUser.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Database/ODBC/s_COdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! nProtect GameGuard 인증용 쿼리를 가져오기 전에 호출
bool CNetUser::nProtectSetAuthQuery()
{
#ifndef _DEBUG
#ifndef _NP30
    if (m_csa.GetAuthQuery() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
#else
	return true;
#endif
#else
    return true;
#endif
}

int CNetUser::nProtect30SetAuthQuery()
{
#ifndef _DEBUG 
#ifndef _NP30
		return true;
#else
	UINT32 nReceivedPacketSize = 0;
	UINT32 nReturn = m_csa.Get(&nReceivedPacketSize);

	//sc::writeLogDebug(sc::string::format("nProtect30SetAuthQuery nReturn : %1%, nReceivedPacketSize : %2%", nReturn, nReceivedPacketSize));

	if( nReturn >= 3000 )
	{
		//sc::writeLogDebug(std::string("NProtect CCSAuth3 Get ERROR"));
		return 0;
	}
	return nReceivedPacketSize;
#endif
#else
    return 0;
#endif
}

//! nProtect GameGuard 인증용 쿼리 가져오기
GG_AUTH_DATA CNetUser::nProtectGetAuthQuery()
{
#ifndef _DEBUG 
#ifndef _NP30
	return m_csa.m_AuthQuery;
#else
    static GG_AUTH_DATA AuthData;
    return AuthData;
#endif
#else
    static GG_AUTH_DATA AuthData;
    return AuthData;
#endif
}

//! nProtect GameGuard 인증용 쿼리 가져오기
BYTE* CNetUser::nProtect30GetAuthQuery()
{
#ifndef _DEBUG 
#ifndef _NP30
	return NULL;
#else
	return m_csa.packet;
#endif
#else
    return NULL;
#endif
}


//! nProtect GameGuard 인증 응답 가져오기
GG_AUTH_DATA CNetUser::nProtectGetAuthAnswer()
{
#ifndef _DEBUG 
#ifndef _NP30
	return m_csa.m_AuthAnswer;
#else
    static GG_AUTH_DATA AuthData;
    return AuthData;
#endif
#else
    static GG_AUTH_DATA AuthData;
    return AuthData;
#endif
}

BYTE* CNetUser::nProtect30GetAuthAnswer()
{
#ifndef _DEBUG 
#ifndef _NP30
	return NULL;
#else
	return m_csa.packet;
#endif
#else
    return NULL;
#endif
}

//! nProtect GameGuard 인증 응답 설정
void CNetUser::nProtectSetAuthAnswer(GG_AUTH_DATA& ggad)
{
#ifndef _DEBUG
#ifndef _NP30
	m_csa.m_AuthAnswer = ggad;
#endif
#endif
}

void CNetUser::nProtect30SetAuthAnswer(BYTE* ggad, int nPacketSeq)
{
#ifndef _DEBUG
#ifdef _NP30
	//sc::writeLogDebug(sc::string::format("CNetUser::nProtect30SetAuthAnswer m_casSeq : %1%, nPacketSeq : %2%", m_casSeq, nPacketSeq));

	if( ggad && m_casSeq == nPacketSeq )
	{
		if( nPacketSeq == 7 )
			m_casSeq = 0;
		else
			m_casSeq = nPacketSeq + 1;

		memcpy(m_csa.packet + (nPacketSeq * 512), ggad, sizeof(BYTE)*512);
	}
	else
	{
		m_casSeq = -1;
	}
#endif
#endif
}

//! nProtect GameGuard 인증 결과 가져오기
DWORD CNetUser::nProtectCheckAuthAnswer()
{
#ifndef _DEBUG
	// 최초접속시 한번은 true 를 준다.
	if (m_bNProtectAuth == true)
	{
		m_bNProtectAuth = false;
		return 0;
	}
	else
	{
#ifndef _NP30
		DWORD dwResult = m_csa.CheckAuthAnswer();
		if (dwResult == 0) // 인증성공
		{
			return 0;
		}
		else // 인증실패
		{
			return dwResult;
		}
#else
		return 0;
#endif
	}
#else
    return 0;
#endif
}


//! nProtect GameGuard 인증 결과 가져오기
DWORD CNetUser::nProtect30CheckAuthAnswer(UINT32 nReceivedSize)
{
#ifndef _DEBUG
	// 최초접속시 한번은 true 를 준다.
	if (m_bNProtectAuth == true)
	{
		m_bNProtectAuth = false;
		return 0;
	}
	else
	{
#ifndef _NP30
		return 0;
#else
		DWORD dwResult = m_csa.Check(nReceivedSize);

		//sc::writeLogDebug(sc::string::format("CNetUser::nProtect30CheckAuthAnswer dwResult : %1%, nReceivedSize : %2%", dwResult, nReceivedSize));

		if (dwResult == 0) // 인증성공
		{
			return 0;
		}
		else // 인증실패
		{
			//sc::writeLogDebug(std::string("NProtect CCSAuth3 Check ERROR"));
			return dwResult;
		}
#endif
	}
#else
    return 0;
#endif
}


void CNetUser::nProtectResetAuth()
{
	m_bNProtectAuth = false;
}

//! nProtect 처음 인증인가?
bool CNetUser::nProtectIsFirstCheck()
{
	return m_bNProtectFirstCheck;
}

//! nProtect 처음 인증여부 세팅
void CNetUser::nProtectSetFirstCheck(bool bState)
{
	m_bNProtectFirstCheck = bState;
}

bool CNetUser::nProtectFirstAuthPass()
{
	return m_bNProtectFirstAuthPass;
}

void CNetUser::nProtectSetFirstAuthPass(bool bState)
{
	m_bNProtectFirstAuthPass = bState;
}
