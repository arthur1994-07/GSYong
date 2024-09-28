#include "../pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./NetUser.h"
#include "./NetAgentClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! nProtect GameGuard ������ ������ �������� ���� ȣ��
bool NetAgentClientMan::nProtectSetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient20");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectSetAuthQuery();
	}
	else
		return m_ClientMan[dwClient]->nProtectSetAuthQuery();
}

int NetAgentClientMan::nProtect30SetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient20");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtect30SetAuthQuery();
	}
	else
		return m_ClientMan[dwClient]->nProtect30SetAuthQuery();
}


//! nProtect GameGuard ���� ���� ��������
GG_AUTH_DATA NetAgentClientMan::nProtectGetAuthAnswer( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient21");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectGetAuthAnswer();
	}
	else
		return m_ClientMan[dwClient]->nProtectGetAuthAnswer();
}

BYTE* NetAgentClientMan::nProtect30GetAuthAnswer( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient21");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtect30GetAuthAnswer();
	}
	else
		return m_ClientMan[dwClient]->nProtect30GetAuthAnswer();
}


//! nProtect GameGuard ������ ���� ��������
GG_AUTH_DATA NetAgentClientMan::nProtectGetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient22");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectGetAuthQuery();
	}
	else
		return m_ClientMan[dwClient]->nProtectGetAuthQuery();
}

//! nProtect GameGuard ������ ���� ��������
BYTE* NetAgentClientMan::nProtect30GetAuthQuery(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient22");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtect30GetAuthQuery();
	}
	else
		return m_ClientMan[dwClient]->nProtect30GetAuthQuery();
}

//! nProtect GameGuard ���� ���� ����
void NetAgentClientMan::nProtectSetAuthAnswer(DWORD dwClient, GG_AUTH_DATA& ggad)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient23");
		m_ClientMan[dwClient-m_nMaxClient]->nProtectSetAuthAnswer(ggad);
	}
	else
		m_ClientMan[dwClient]->nProtectSetAuthAnswer(ggad);
}

void NetAgentClientMan::nProtect30SetAuthAnswer(DWORD dwClient, BYTE* ggad, int nPacketSeq)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient23");
		m_ClientMan[dwClient-m_nMaxClient]->nProtect30SetAuthAnswer(ggad, nPacketSeq);
	}
	else
		m_ClientMan[dwClient]->nProtect30SetAuthAnswer(ggad, nPacketSeq);
}

//! nProtect GameGuard ���� ��� ��������
DWORD NetAgentClientMan::nProtectCheckAuthAnswer(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient24");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectCheckAuthAnswer();
	}
	else
		return m_ClientMan[dwClient]->nProtectCheckAuthAnswer();
}

DWORD NetAgentClientMan::nProtect30CheckAuthAnswer(DWORD dwClient, UINT32 nReceivedSize)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient24");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtect30CheckAuthAnswer(nReceivedSize);
	}
	else
		return m_ClientMan[dwClient]->nProtect30CheckAuthAnswer(nReceivedSize);
}


//! nProtect GameGuard ���� ���¸� ���½�Ų��.
void NetAgentClientMan::nProtectResetAuth(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient25");
		m_ClientMan[dwClient-m_nMaxClient]->nProtectResetAuth();
	}
	else
		m_ClientMan[dwClient]->nProtectResetAuth();
}

//! nProtect ó�� �������� ����
void NetAgentClientMan::nProtectFirstCheck(DWORD dwClient, bool bState)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient26");
		m_ClientMan[dwClient-m_nMaxClient]->nProtectSetFirstCheck(bState);
	}
	else
		m_ClientMan[dwClient]->nProtectSetFirstCheck(bState);
}

//! nProtect ó�� �����ΰ�?
bool NetAgentClientMan::nProtectIsFirstCheck(DWORD dwClient)
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient27");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectIsFirstCheck();
	}
	else
		return m_ClientMan[dwClient]->nProtectIsFirstCheck();
}

//! nProtect GameGuard ó�� ������ ����ߴ°�?
//! �� ������ ����ؾ� �����÷��̸� �� �� �ִ�.
bool NetAgentClientMan::nProtectFirstAuthPass( DWORD dwClient )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient28");
		return m_ClientMan[dwClient-m_nMaxClient]->nProtectFirstAuthPass();
	}
	else
		return m_ClientMan[dwClient]->nProtectFirstAuthPass();
}

//! nProtect GameGuard ó������ ������� ����
void NetAgentClientMan::nProtectSetFirstAuthPass( DWORD dwClient, bool bState )
{
	if (dwClient >= (DWORD) m_nMaxClient)
	{
		GASSERT(0&&"m_nMaxClient29");
		m_ClientMan[dwClient-m_nMaxClient]->nProtectSetFirstAuthPass(bState);
	}
	else
		m_ClientMan[dwClient]->nProtectSetFirstAuthPass(bState);
}
//GASSERT(dwClient<(DWORD)m_nMaxClient);