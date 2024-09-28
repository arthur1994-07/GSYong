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
	
	// 2006-08-16 대만요청
	// DB 에도 로그를 남긴다.	
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
	// 로그 기록
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

//! nProtect 인증용 메시지 전송
void CAgentServer::MsgSndGameGuard( DWORD dwClient, NET_GAMEGUARD_AUTH& nga )
{
	// 온라인 상태이고 처음 체크하는 상태가 아니라면...	
	if (m_pClientManager->IsOnline( dwClient ) && m_pClientManager->nProtectIsFirstCheck(dwClient) == false)
    {
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth(dwClient);
		// 게임가드 인증 쿼리 가져오기 준비
#ifndef _NP30
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// 게임가드 인증 쿼리 전송
			nga.ggad = m_pClientManager->nProtectGetAuthQuery(dwClient );
			SendClient(dwClient, &nga);
		}
#else
		int nReceivedPacketSize = m_pClientManager->nProtect30SetAuthQuery(dwClient);
		// 게임가드 인증 쿼리 전송
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

// nProtect 서버 인증을 사용한다면 접속후 바로 클라이언트에게 인증용 메시지를 보낸다.
void CAgentServer::MsgSndGameGuardFirstCheck(DWORD dwClient)
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{		
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth(dwClient);
		// 게임가드 인증 쿼리 가져오기 준비
#ifndef _NP30
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// nProtect 최초 인증상태로 설정한다.
			m_pClientManager->nProtectFirstCheck(dwClient, true);
			// 게임가드 인증 쿼리 전송
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
				// 게임가드 인증 쿼리 전송
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

//! nProtect 클라이언트 인증 Second
void CAgentServer::MsgSndGameGuardSecondCheck(DWORD dwClient)
{
	if (m_bGameGuardInit == true && m_bGameGuardAuth == true)
	{		
		// 인증상태를 리셋 시킨다.
		m_pClientManager->nProtectResetAuth(dwClient);

#ifndef _NP30
		// 게임가드 인증 쿼리 가져오기 준비
		if (m_pClientManager->nProtectSetAuthQuery(dwClient))
		{
			// 게임가드 인증 쿼리 전송
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
				// 게임가드 인증 쿼리 전송
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

//! GameGuard 인증용 메세지 결과
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
			// nProtect 인증 실패
			// 연결을 종료시킨다.
			WriteNprotectLog(dwClient, dwResult);
			CloseClient(dwClient, net::KICKOUT_GAMEGUARD);

			//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER - CLOSE Client : dwClient %1%", dwClient));
		}
	}
#endif

	m_pClientManager->SetHeartBeat( dwClient );
}

//! nProtect 인증용 메세지 결과
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
		// nProtect 인증 실패
		// 연결을 종료시킨다.
		WriteNprotectLog(dwClient, dwResult);
		CloseClient(dwClient, net::KICKOUT_GAMEGUARD);

		//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - CLOSE Client : dwClient %1%", dwClient));
	}
	else
	{
		// 두번째 인증을 시도한다.
		MsgSndGameGuardSecondCheck(dwClient);

		//sc::writeLogDebug(sc::string::format("NET_GAMEGUARD_ANSWER_1 - MsgSndGameGuardSecondCheck : dwClient %1%", dwClient));
	}
}

//! nProtect 인증용 메세지 결과
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
		// nProtect 인증 실패
		// 연결을 종료시킨다.
		WriteNprotectLog(dwClient, dwResult);
		CloseClient(dwClient, net::KICKOUT_GAMEGUARD);
	}
	else
	{		
		m_pClientManager->nProtectFirstCheck(dwClient, false); // 처음인증 상태 끝
		m_pClientManager->nProtectSetFirstAuthPass(dwClient, true); // 인증통과 성공
	}
}