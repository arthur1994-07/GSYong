#include "../pch.h"
#include "./AgentServer.h"
#include "../Database/DbAction/DbActionLog.h"
#include "../Client/NetAgentClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::WriteNprotectLog( DWORD dwClient, DWORD dwResult )
{
#ifndef _NP30
	GG_AUTH_DATA Query = m_pClientManager->nProtectGetAuthQuery( dwClient );
	GG_AUTH_DATA Answer = m_pClientManager->nProtectGetAuthAnswer( dwClient );

    std::string strLog(
        sc::string::format(
            "NPROTECT:ID(%1%) Result(%2%) Query(%3% %4% %5% %6%) Answer(%7% %8% %9% %10%)",
            m_pClientManager->GetUserID(dwClient),
            dwResult,
            Query.dwIndex,
            Query.dwValue1,
            Query.dwValue2,
            Query.dwValue3,
            Answer.dwIndex,
            Answer.dwValue1,
            Answer.dwValue2,
            Answer.dwValue3));
	sc::writeLogError(strLog);        
	
	// 2006-08-16 �븸��û
	// DB ���� �α׸� �����.	
    if (m_pAdoManager)
    {
        m_pAdoManager->AddLogJob(
            db::DbActionPtr(
                new db::LogHackProgram(
			        m_pClientManager->UserDbNum( dwClient ),
			        m_pClientManager->ChaDbNum( dwClient ),
			        NPROTECT_HACK_NUM,
			        (char*) strLog.c_str())));
    }
#else
	// �α� ���
    std::string strLog(sc::string::format("NPROTECT:ID(%1%) Result(%2%)", m_pClientManager->GetUserID(dwClient), dwResult));
	sc::writeLogError(strLog);        
	
    if (m_pAdoManager)
    {
        m_pAdoManager->AddLogJob(
            db::DbActionPtr(
                new db::LogHackProgram(
			        m_pClientManager->UserDbNum( dwClient ),
			        m_pClientManager->ChaDbNum( dwClient ),
			        NPROTECT_HACK_NUM,
			        (char*) strLog.c_str())));
    }
#endif
}

//! nProtect ������ �޽��� ����
void CAgentServer::MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga )
{
	// �¶��� �����̰� ó�� üũ�ϴ� ���°� �ƴ϶��...	
	if (m_pClientManager->IsOnline( dwClient ) && m_pClientManager->nProtectIsFirstCheck(dwClient) == false)
    {
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth(dwClient);
		// ���Ӱ��� ���� ���� �������� �غ�
#ifndef _NP30
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// ���Ӱ��� ���� ���� ����
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient );
			SendClient(dwClient, &nga);
		}
#else
		int nReceivedPacketSize = m_pClientManager->nProtect30SetAuthQuery(dwClient);
		// ���Ӱ��� ���� ���� ����
		BYTE *pNGA =  m_pClientManager->nProtect30GetAuthQuery(dwClient );

		if( pNGA && nReceivedPacketSize >= 0)
		{
			for(int i=0; i<8; i++ )
			{
				nga.packetSize = nReceivedPacketSize;
				nga.packetSeq = i+1;
				memcpy(nga.packet, pNGA + (i*512), sizeof(BYTE)*512);
				SendClient(dwClient, &nga);
				//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_AUTH - MsgSndGameGuard : idx %1%, PacketSize %2%", i+1, nReceivedPacketSize));
			}
		}
		else
		{
			sc::writeLogError(std::string("GameGuard Packet ERROR (MsgSndGameGuard)"));
		}
#endif
	}
}

// nProtect ���� ������ ����Ѵٸ� ������ �ٷ� Ŭ���̾�Ʈ���� ������ �޽����� ������.
void CAgentServer::MsgSndGameGuardFirstCheck(DWORD dwClient)
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{		
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth(dwClient);
		// ���Ӱ��� ���� ���� �������� �غ�
#ifndef _NP30
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// nProtect ���� �������·� �����Ѵ�.
			m_pClientManager->nProtectFirstCheck(dwClient, true);
			// ���Ӱ��� ���� ���� ����
            NET_GAMEGUARD_AUTH_1 nga(m_pClientManager->nProtectGetAuthQuery(dwClient));
			SendClient(dwClient, &nga);
		}
        else
        {
            sc::writeLogError(
                std::string(
                    "MsgSndGameGuardFirstCheck nProtectSetAuthQuery"));
        }
#else
		m_pClientManager->nProtectFirstCheck(dwClient, true);

		int nReceivedPacketSize = m_pClientManager->nProtect30SetAuthQuery(dwClient);
		BYTE *pNGA =  m_pClientManager->nProtect30GetAuthQuery(dwClient);						  

		if( pNGA && nReceivedPacketSize >= 0)
		{
			for(int i=0; i<8; i++)
			{
				// ���Ӱ��� ���� ���� ����
				NET_GAMEGUARD_AUTH nga;
				nga.packetSize = nReceivedPacketSize;
				nga.packetSeq = i+1;
				memcpy(nga.packet, pNGA + (i*512), sizeof(BYTE)*512);
				SendClient(dwClient, &nga);
				//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_AUTH - MsgSndGameGuardFirstCheck : idx %1%, PacketSize %2%", i+1, nReceivedPacketSize));
			}
		}
		else
		{
			sc::writeLogError(std::string("GameGuard Packet ERROR (MsgSndGameGuardFirstCheck)"));
		}
#endif
	}
}

//! nProtect Ŭ���̾�Ʈ ���� Second
void CAgentServer::MsgSndGameGuardSecondCheck(DWORD dwClient)
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{		
		// �������¸� ���� ��Ų��.
		m_pClientManager->nProtectResetAuth(dwClient);

#ifndef _NP30
		// ���Ӱ��� ���� ���� �������� �غ�
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// ���Ӱ��� ���� ���� ����
            NET_GAMEGUARD_AUTH_2 nga;
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient);
			SendClient(dwClient, &nga);
		}
        else
        {
            sc::writeLogError(
                sc::string::format(
                    "MsgSndGameGuardSecondCheck %1%", dwClient));
        }
#else
		int nReceivedPacketSize = m_pClientManager->nProtect30SetAuthQuery(dwClient);
		BYTE *pNGA =  m_pClientManager->nProtect30GetAuthQuery(dwClient);						  

		if( pNGA && nReceivedPacketSize >= 0)
		{
			for(int i=0; i<8; i++)
			{
				// ���Ӱ��� ���� ���� ����
				NET_GAMEGUARD_AUTH_2 nga;
				nga.packetSize = nReceivedPacketSize;
				nga.packetSeq = i+1;
				memcpy(nga.packet, pNGA + (i*512), sizeof(BYTE)*512);
				SendClient(dwClient, &nga);
				//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_AUTH_2 - MsgSndGameGuardFirstCheck : idx %1%, PacketSize %2%", i+1, nReceivedPacketSize));
			}
		}
		else
		{
			sc::writeLogError(std::string("GameGuard Packet ERROR (MsgSndGameGuardSecondCheck)"));
		}
#endif
	}
}

//! GameGuard ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswer( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;

	NET_GAMEGUARD_ANSWER* nga = (NET_GAMEGUARD_ANSWER*)nmg;
#ifndef _NP30
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);

	TCHAR* szEncKey = m_pClientManager->GetEncKey( dwClient );
	if ( _tcslen( szEncKey) > 0  )
	{				
		TCHAR szEncryptKey[ENCRYPT_KEY+1];
		memcpy( szEncryptKey, nga->szEnCrypt, sizeof(TCHAR) * ENCRYPT_KEY+1 );

		m_Tea.decrypt (szEncryptKey, ENCRYPT_KEY+1);
		if ( _tcscmp( szEncryptKey, szEncKey ) )
		{
			sc::writeLogError(
                sc::string::format(
                    "HeartBeat %s %s CloseClient %d",
                    szEncryptKey,
                    szEncKey,
                    dwClient));
			return;
		}
	}

	m_pClientManager->SetEncKey( dwClient, nga->szEnCrypt );
#else
	UINT32 nPacketSize = nga->packetSize;
	int nPacketSeq = nga->packetSeq - 1;

	//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER - MsgGameGuardAnswer : nPacketSeq %1%, PacketSize %2%", nPacketSeq, nPacketSize));

	m_pClientManager->nProtect30SetAuthAnswer(dwClient, nga->packet, nPacketSeq);

	if( nga->packetSeq == 8 )
	{		
		m_pClientManager->nProtectFirstCheck(dwClient, false);

		DWORD dwResult = m_pClientManager->nProtect30CheckAuthAnswer(dwClient, nPacketSize);

		//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER - nProtect30CheckAuthAnswer : dwResult %1%", dwResult));

		if (dwResult != 0)
		{
			// nProtect ���� ����
			// ������ �����Ų��.
			WriteNprotectLog(dwClient, dwResult);
			CloseClient(dwClient, net::KICKOUT_GAMEGUARD);

			//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER - CLOSE Client : dwClient %1%", dwClient));
		}
	}
#endif

	m_pClientManager->SetHeartBeat( dwClient );
}

//! nProtect ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswerFirst( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (!nmg)
        return;

	NET_GAMEGUARD_ANSWER_1* nga = (NET_GAMEGUARD_ANSWER_1*)nmg;
#ifndef _NP30
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);
	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer(dwClient);
#else
	UINT32 nPacketSize = nga->packetSize;
	int nPacketSeq = nga->packetSeq - 1;

	m_pClientManager->nProtect30SetAuthAnswer(dwClient, nga->packet, nPacketSeq);
	DWORD dwResult = m_pClientManager->nProtect30CheckAuthAnswer(dwClient, nPacketSize);

	//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - MsgGameGuardAnswerFirst : dwResult %1%", dwResult));
#endif

	if (dwResult != 0)
	{
		// nProtect ���� ����
		// ������ �����Ų��.
		WriteNprotectLog(dwClient, dwResult);
		CloseClient(dwClient, net::KICKOUT_GAMEGUARD);

		//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - CLOSE Client : dwClient %1%", dwClient));
	}
	else
	{
		// �ι�° ������ �õ��Ѵ�.
		MsgSndGameGuardSecondCheck(dwClient);

		//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - MsgSndGameGuardSecondCheck : dwClient %1%", dwClient));
	}
}

//! nProtect ������ �޼��� ���
void CAgentServer::MsgGameGuardAnswerSecond( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (!nmg)
        return;

	NET_GAMEGUARD_ANSWER_2* nga = (NET_GAMEGUARD_ANSWER_2*) nmg;
#ifndef _NP30
	m_pClientManager->nProtectSetAuthAnswer(dwClient, nga->ggad);
	DWORD dwResult = m_pClientManager->nProtectCheckAuthAnswer(dwClient);
#else
	UINT32 nPacketSize = nga->packetSize;
	int nPacketSeq = nga->packetSeq - 1;

	m_pClientManager->nProtect30SetAuthAnswer(dwClient, nga->packet, nPacketSeq);
	DWORD dwResult = m_pClientManager->nProtect30CheckAuthAnswer(dwClient, nPacketSize);

	//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - MsgGameGuardAnswerSecond : nPacketSeq %1%, dwResult %2%", nPacketSeq, dwResult));
#endif

	if (dwResult != 0)
	{
		// nProtect ���� ����
		// ������ �����Ų��.
		WriteNprotectLog(dwClient, dwResult);
		CloseClient(dwClient, net::KICKOUT_GAMEGUARD);
	}
	else
	{		
		m_pClientManager->nProtectFirstCheck(dwClient, false); // ó������ ���� ��
		m_pClientManager->nProtectSetFirstAuthPass(dwClient, true); // ������� ����
	}
}