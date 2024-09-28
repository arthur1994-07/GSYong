#include "../pch.h"

#include <boost/lexical_cast.hpp>

#include "../../SigmaCore/xml/2.5.3/ticpp.h"
#include "../../Enginelib/Common/profile.h"
#include "../../Enginelib/DxTools/DebugSet.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../Database/DbAction/DbActionUser.h"
#include "../Client/NetAgentClientMan.h"
#include "./AgentServerConfigXml.h"
#include "./AgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CAgentServer::ThaiMsgLogin (DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_THAILAND || nmg == NULL) return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing (dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing (dwClient, true);
	}

	THAI_NET_LOGIN_DATA* tnld = ( THAI_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	/// Tea 알고리듬으로 교체
	/// 2005-11-15 Jgkim
	
	/*
	char* pBuffer = (char*) tnld;
	CRYPT_KEY ck;
	ck = m_pClientManager->GetCryptKey (dwClient);
	m_Bit.buf_decode (pBuffer+sizeof(NET_MSG_GENERIC), 
					  (tnld->nmg.dwSize)-sizeof(NET_MSG_GENERIC),
					  ck.nKeyDirection,
					  ck.nKey);
	*/
	m_Tea.decrypt( tnld->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( tnld->szPassword, USR_PASS_LENGTH+1 );
	
	// 접속하려는 채널 검사
	int nChannel = tnld->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL; // 채널이 Full 이라고 메시지를 보낸다.
		SendClient (dwClient, &nlfd); // 결과전송
//		CloseClient (dwClient); // 접속종료		

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));

		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull (nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient (dwClient, &nlfd); // 결과전송
//		CloseClient (dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL",	nChannel) );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel (dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser (nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel (dwClient, nChannel);
		}
		else // 기존접속자
		{
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID (dwClient, tnld->szUserid);		
	
	// 인증통과 실패 세팅
	m_pClientManager->SetAccountPass (dwClient, false); 

	// DB 에 인증요청
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentThaiUserCheck(
                tnld->szUserid, 
                tnld->szPassword,
                m_pClientManager->GetClientIP(dwClient),
                m_nServerGroup,
                m_nServerNum,
                0,
                dwClient)));
}

void CAgentServer::ThaiMsgLogInBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA	nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD dwClient = (DWORD) nlfd2->m_ClientSlot;
	DWORD dwSndBytes = 0;
	int	nUserNum   = 0;		

    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			 (strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0))
		{
			// 아이템 변환중이면 실패처리
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "ThaiMsgLogInBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));

				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));

				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  ( dwClient, true );
			m_pClientManager->SetUserDbNum      ( dwClient, nlfd2->nUserNum );       // 유저번호
			m_pClientManager->SetLoginTime    ( dwClient );                        // 로그인 시간
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // 유저타입세팅
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // 채팅 블록 기간
			m_pClientManager->ThaiSetFlag     ( dwClient, nlfd2->wThaiFlag );      // Thailand 사용자 플래그
			m_pClientManager->SetIPBonusClass ( dwClient, nlfd2->emIPBonus );  // Thailand 사용자 Class Type

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			// 18 세 이하일때는 오늘 게임한 플레이 시간(분)을 가져와야 한다.
			if ( nlfd2->wThaiFlag == THAI_UNDER18 )
			{
				// 오늘 플레이한 시간 가져오기
				WORD wTodayGameTime = m_pDbManager->ThaiGetTodayPlayTime( nlfd2->nUserNum );
				// 오늘 게임플레이 시간(분) 세팅
				m_pClientManager->ThaiSetTodayPlayTime( dwClient, wTodayGameTime );
			}

			m_pClientManager->SetAccountPass( dwClient, true ); // 인증통과했음

			// APEX 버젼 변경
			if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
			{
				APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

				std::string IpAddress = m_pClientManager->GetClientIP(dwClient);
				int nIp  = inet_addr(IpAddress.c_str());

				APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			}

			SendClient( dwClient, &nlfd ); // 결과전송
			return;
		}
		// 로그인성공이지만 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->szUserid));
	
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong(%s)", nlfd2->szUserid ));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass (dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline (dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp (m_pClientManager->GetUserID (dwClient), nlfd2->szUserid) == 0))
		{
			// 태국의 로그인방식은 다른국가들과 다르다. 다른 국가들은 중복접속이라면 기존의 사용자를 종료시키나, 
			// 태국은 종료시키지 않는다. 그로 인해 현재 매일 몇몇 사용자들은 실제 게임에 접속하고 있지않으나
			// 로그아웃 처리가 정상적으로 되지않아서 GM들이 수동으로 처리를 하고 있다.
			// 현재 정확한 원인은 파악이 안되고 있다. 로그인 처리를 할 때 DB에서 결과값이 중복접속이라는 결과가 왔을 때
			// 서버에 현재 그 사용자의 캐릭터가 남아있지 않다면 무언가 문제가 생겨서 갇힌 사용자이다.
			// 로그아웃처리를 해준다. 하지만, 정확한 원인을 찾아야 한다. 그렇지 않으면 문제가 생기는 사용자들의
			// 로그아웃 시간 등등 처리가 되지않는다.
			if ( nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP )
			{
				PGLCHARAG pCHAR = m_pGLAgentServer->GetCharByUserAccount( std::string( nlfd2->szUserid ) );
				if ( !pCHAR )
				{
					m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
					return;
				}
			}

			nlfd.m_Result = nlfd2->m_emResult;
			SendClient (dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline (dwClient) == false)
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));
		
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError (sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}
}

//! China 로그인 메시지
void CAgentServer::ChinaMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	

	// 중국 클라이언트가 보낸 메시지 인지 확인한다.
	if (m_ServiceProvider != SP_CHINA || nmg == NULL) return;	

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing( dwClient ) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( dwClient, true );
	}

	CHINA_NET_LOGIN_DATA* pNml = ( CHINA_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof(CHINA_NET_LOGIN_DATA) != pNml->Size())
	{
		sc::writeLogError(std::string("CHINA_NET_LOGIN_DATA Wrong Message Size") );
		return;
	}

	// 복호화
	m_Tea.decrypt( pNml->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( pNml->szPassword, USR_PASS_LENGTH );
	m_Tea.decrypt( pNml->szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};

	StringCchCopy( szUserid, USR_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_PASS_LENGTH, pNml->szPassword );
	StringCchCopy( szRandomPassword, USR_RAND_PASS_LENGTH+1, pNml->szRandomPassword );
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료

		sc::writeLogError(sc::string::format("ChinaMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull( nChannel ) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient, nChannel );
		}
		else // 기존접속자
		{
			
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB 에 인증요청
	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패 세팅
	
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentUserCheck(
		        szUserid, 
		        szPassword,
		        m_pClientManager->GetClientIP( dwClient ),
		        szRandomPassword,
		        m_pClientManager->GetRandomPassNumber( dwClient ),
		        m_nServerGroup,
		        m_nServerNum,
		        0,
		        dwClient,
		        static_cast<DWORD> (m_ServiceProvider))));
}

void CAgentServer::ChinaMsgLoginBack( NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->m_ClientSlot;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ),   nlfd2->szUserid ) == 0))
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  ( dwClient, true );
			m_pClientManager->SetUserDbNum      ( dwClient, nlfd2->nUserNum );       // 유저번호
			m_pClientManager->SetLoginTime    ( dwClient );                        // 로그인 시간
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // 유저타입세팅
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // 채팅 블록 기간

			m_pClientManager->SetChinaGameTime( dwClient, nlfd2->nChinaGameTime ); // 중국 게임시간 세팅
			m_pClientManager->SetChinaOfflineTime( dwClient, nlfd2->nChinaOfflineTime ); // 중국 오프라인 시간 세팅
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 중국 LastLoginDate 세팅
			m_pClientManager->SetChinaUserAge( dwClient, nlfd2->nChinaUserAge ); // 중국 UserAge 세팅

			m_pClientManager->SetAccountPass( dwClient, true ); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient( dwClient, &nlfd ); // 결과전송

			// Apex 적용
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

//			char* szIp = m_pClientManager->GetClientIP( dwClient );
//			int nIp  = inet_addr(szIp);
			
//			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			
			return;
		}		
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
            // 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
            sc::writeLogWarn(sc::string::format("ChinaMsgLoginBack User Already Offline %1%", nlfd2->szUserid));
			return;
		}		
		else 
		{
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패
		m_pClientManager->SetAccountPassing( dwClient, false );

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ), nlfd2->szUserid ) == 0))
		{
			nlfd.m_Result = nlfd2->m_emResult;
			SendClient( dwClient, &nlfd );
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}
}

/**
* 일반 로그인 : id / pwd 입력시
*/
void CAgentServer::MsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{		
	if (nmg == NULL)
		return;

	//if (m_ServiceProvider != SP_OFFICE_TEST && m_ServiceProvider != SP_TAIWAN )
	//{
	//	sc::writeLogError(sc::string::format("US Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
	//	return;
	//}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	NET_LOGIN_DATA* pPacket = (NET_LOGIN_DATA*) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof(NET_LOGIN_DATA) != pPacket->Size())
	{
		sc::writeLogError(std::string("NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

    // 버전 체크 추후에 작성
    //pPacket->m_LauncherVersion;
    //pPacket->m_GameVersion;

	/// 암호화 알고리듬 교체
	/// 2005-11-16 Jgkim	
	m_Tea.decrypt(pPacket->m_szUserid, USR_ID_LENGTH+1);
	m_Tea.decrypt(pPacket->m_szPassword, USR_PASS_LENGTH+1);
	m_Tea.decrypt(pPacket->m_szRandomPassword, USR_RAND_PASS_LENGTH+1);
	m_Tea.decrypt(pPacket->m_szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};

	StringCchCopy(szUserid, USR_ID_LENGTH+1, pPacket->m_szUserid);
	StringCchCopy(szPassword, USR_PASS_LENGTH+1, pPacket->m_szPassword);
	StringCchCopy(szRandomPassword, USR_RAND_PASS_LENGTH+1, pPacket->m_szRandomPassword);
	StringCchCopy(szEncrypt, ENCRYPT_KEY+1, pPacket->m_szEnCrypt);

	if (_tcscmp( szEncrypt, m_szEncrypt ))
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		sc::writeLogError(
            sc::string::format(
                "MsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,",
                pPacket->m_szUserid, szEncrypt, m_szEncrypt ));

		return;
	}	

	// 접속하려는 채널 검사
	int nChannel = pPacket->m_nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // 접속종료

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

//		CloseClient(dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}
/*
	// WhiteRock과의 통신 하는 부분을 Thread 처리
	CAgentJapnaWRLogin* pAction = new CAgentJapnaWRLogin(
													szUserid,
													szPassword,
													m_pClientManager->GetClientIP(dwClient),
													m_nServerGroup,
													m_nServerNum,
													dwClient
													);

	m_pDbManager->AddWebJob((CDbAction*) pAction);
*/

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	//
	//mjeon		
	//RandomPassword and sc::Random::RandomNumber are used by other countries like Thai
	//for account protection. 
	//Each service provider sends the random-pass stored in RanDB to the users
	//using their own solution just like e-mail and the users just uses the random-pass.	
	//
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentUserCheck( 
		        szUserid, 
		        szPassword,
		        m_pClientManager->GetClientIP(dwClient),
		        szRandomPassword,
		        m_pClientManager->GetRandomPassNumber(dwClient),
		        m_nServerGroup,
		        m_nServerNum,
		        0,
		        dwClient,
		        static_cast<DWORD> (m_ServiceProvider))));
}


void CAgentServer::IdnMsgLogIn(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL)
		return;

	//
	//이미 인증중이다
	//
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else //인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	IDN_NET_LOGIN_DATA* msg = (IDN_NET_LOGIN_DATA*) nmg;

	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof(IDN_NET_LOGIN_DATA) != msg->Size())
	{
		sc::writeLogError(std::string("CAgentServer::IdnMsgLogIn() - Wrong message size."));
		return;
	}
	

	if (m_ServiceProvider != SP_INDONESIA)
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType	= NET_MSG_LOGIN_FB;
		nlfd.m_Result= EM_LOGIN_FB_WRONG_SP;
		
		SendClient(dwClient, &nlfd);
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);

		sc::writeLogError(sc::string::format("CAgentServer::IdnMsgLogIn() - Wrong ServiceProvide(%1%).", (DWORD)m_ServiceProvider));
		return;
	}

	
	int nChannel = msg->m_nChannel;
	
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType	= NET_MSG_LOGIN_FB;
		nlfd.m_Result= EM_LOGIN_FB_CH_FULL;
		
		SendClient(dwClient, &nlfd);
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);

		sc::writeLogError(sc::string::format("CAgentServer::IdnMsgLogIn() - Invalid channel number (Channel:%d)", nChannel ));
		
		return;
	}
	
	if (IsChannelFull(nChannel))
	{		
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType	= NET_MSG_LOGIN_FB;
		nlfd.m_Result= EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd);

		//CloseClient(dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel));
		return;
	}
	else
	{
		// 신규접속자
		if (m_pClientManager->GetChannel(dwClient) == -1)
		{
			IncreaseChannelUser(nChannel);
			
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			;			
		}
	}

	std::string strId(msg->m_szUserid);
	std::string strPass(msg->m_szPassword);


	//int nUserNum = IdnAuthVTC(strId /*pain*/, strPass /*encryped by md5*/);


	m_pClientManager->SetUserID(dwClient, strId.c_str());
	
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new DIdnUserCheck(strId, strPass, m_pClientManager->GetClientIP(dwClient), m_nServerGroup,
						m_nServerNum, 0, dwClient, static_cast<DWORD>(m_ServiceProvider))
						));

}

//
//mjeon.indonesia.일반적인 ID/Password를 통한 인증
//
int CAgentServer::IdnAuthVTC(std::string &strID, std::string &strPW)
{
	int nUserNum = -1;
	
	CURL		*curl;
	VECBUFFER	byBuffer;
	std::string strResponse;
	
	//byBuffer.clear();

	curl = curl_easy_init();

	struct curl_slist *headers = NULL;

	//curl_easy_setopt( curl, CURLOPT_URL, "http://sandbox2.vtcebank.vn/GameServiceAPI/BillingService.asmx");
	curl_easy_setopt( curl, CURLOPT_URL, IDN_VTC_AUTH_URL);		//LiveServer
	
	std::string strSign = 
		sc::string::getMd5A(strID + strPW + std::string(IDN_MAKERCODE) + std::string(IDN_SERVICEID) + std::string(IDN_SERVICEKEY) + std::string(IDN_MD5KEY));

	std::string xml = 
		sc::string::format(
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"			\
		"<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"	\
		  "<soap12:Body>"										\
		    "<Authenticate xmlns=\"http://tempuri.org/\">"		\
		      "<AccountName>%1%</AccountName>"					\
		      "<AccountPasswordMD5>%2%</AccountPasswordMD5>"	\
		      "<MakerCode>%3%</MakerCode>"						\
		      "<Sign>%4%</Sign>"								\
		      "<ServiceID>%5%</ServiceID>"						\
		      "<ServiceKey>%6%</ServiceKey>"					\
		    "</Authenticate>"									\
		  "</soap12:Body>"										\
		"</soap12:Envelope>"
		, strID, strPW, IDN_MAKERCODE, strSign, IDN_SERVICEID, IDN_SERVICEKEY
		);
	
	std::string utf8 = sc::string::ansi_to_utf8(xml);

	std::string header = sc::string::format("Content-type:application/soap+xml; charset=utf-8\r\nContent-Length: %1%", utf8.length());


	printf("request header: %s\n\n", header.c_str());
	printf("request XML: %s\n\n", xml.c_str());
	printf("request UTF-8: %s\n\n", utf8.c_str());


	headers = curl_slist_append(headers, header.c_str());

	//curl_easy_setopt( curl, CURLOPT_VERBOSE, 1);

	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt( curl, CURLOPT_POST, TRUE );
	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, utf8.c_str( ) );


	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, wrapper_curl_write_callback);
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, &byBuffer );

	CURLcode ccOK = curl_easy_perform( curl );	

	if (ccOK == CURLE_OK)
	{
		//mjeon.curl.The result buffer that is input by "wrapper_curl_write_callback" will not be zero terminated.
		byBuffer.push_back('\0');

		long statLong;

		// HTTP 응답코드를 얻어온다. 
        if(CURLE_OK == curl_easy_getinfo( curl , CURLINFO_HTTP_CODE , &statLong))
		{	
			if (statLong >= 400)
			{
				sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC() - cURL response code is an error(%1%).", statLong));

				goto _auth_vtc_end;				
			}

			if (byBuffer.empty())
			{
				sc::writeLogError(std::string("CAgentServer::IdnAuthVTC() - response buffer is empty!"));

				goto _auth_vtc_end;				
			}			

#if defined(_RELEASED) || defined(_DEBUG)
			std::cout << "Response code:  " << statLong << std::endl ;
#endif
		}

#if defined(_RELEASED) || defined(_DEBUG)
		// get some info about the xfer:
        double statDouble;        
        char* statString = NULL;

		if (statLong < 400)
		{
			// Content-Type 를 얻어온다.
			if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			{
				sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC() - Content type: %1%", statString));
			}
		}

        // 다운로드한 문서의 크기를 얻어온다.
        if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC() - Download size: %1%bytes", statDouble));
        }

        // 
        if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC() - Download speed: %1%bytes/sec", statDouble));
        }
#endif

		strResponse = std::string(&byBuffer[0]);

		//
		// UTF-8-to-ANSI for the result
		//	
		//std::string result = sc::string::utf8_to_ansi(&byBuffer[0], (int)byBuffer.size());
		std::string result = sc::string::utf8_to_ansi(strResponse);


		if (result.length() <= 0)
		{
			sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC() - result is empty for the UserID(%1%).", strID));
						
			goto _auth_vtc_end;
		}

		//
		//parse the UserNum from the response.
		//
		
		/* Response form

		<?xml version="1.0" encoding="utf-8"?>
		<soap12:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap12="http://www.w3.org/2003/05/soap-envelope">
		  <soap12:Body>
			<AuthenticateResponse xmlns="http://tempuri.org/">
			  <AuthenticateResult>
				<ResponseCode>long</ResponseCode>
				<Sign>string</Sign>
			  </AuthenticateResult>
			</AuthenticateResponse>
		  </soap12:Body>
		</soap12:Envelope>
		*/

		try
		{
			ticpp::Document doc;
			doc.Parse(result);

			ticpp::Element* pHead = NULL;
			ticpp::Element* pElem = NULL;
			
			pHead = doc.FirstChildElement("soap:Envelope");
			pElem = pHead->FirstChildElement("soap:Body");
			pElem = pElem->FirstChildElement("AuthenticateResponse"); 
			pElem = pElem->FirstChildElement("AuthenticateResult");
			pElem = pElem->FirstChildElement("ResponseCode");
			
			//pElem->GetValue(&nUserNum);
			std::string strValue = pElem->GetText();
			nUserNum = boost::lexical_cast<int> (strValue);
		}
		catch ( ticpp::Exception& ex )
		{			
			sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC() - response XML couldn't be parsed for the UserID(%1%).", strID));
			sc::writeLogError(result);
			sc::writeLogError(ex.m_details);
		}
	}

_auth_vtc_end:
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return nUserNum;
}


//
//mjeon.indonesia.ID/2차Password를 통한 인증 (캐릭터 삭제시)
//
int CAgentServer::IdnAuthVTC_2ndPass(std::string &strID, std::string &str2ndPass)
{
	int nRet = -1;
	
	CURL		*curl;
	VECBUFFER	byBuffer;
	std::string strResponse;
	
	//byBuffer.clear();

	curl = curl_easy_init();

	struct curl_slist *headers = NULL;
	
	curl_easy_setopt( curl, CURLOPT_URL, IDN_VTC_ACCOUNT_FUNCTION_URL);		//LiveServer


	std::string xml = 
		sc::string::format(
		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"					\
		"<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"	\
		  "<soap12:Body>"												\
		    "<Account_CheckPassLevel2 xmlns=\"http://tempuri.org/\">"	\
			  "<userName>%1%</userName>"								\
		      "<passLevel2>%2%</passLevel2>"							\
		    "</Account_CheckPassLevel2>"								\
		  "</soap12:Body>"												\
		"</soap12:Envelope>"
		, strID, str2ndPass
		);
	
	std::string utf8 = sc::string::ansi_to_utf8(xml);

	std::string header = sc::string::format("Content-type:application/soap+xml; charset=utf-8\r\nContent-Length: %1%", utf8.length());


	printf("request header: %s\n\n", header.c_str());
	printf("request XML: %s\n\n", xml.c_str());
	printf("request UTF-8: %s\n\n", utf8.c_str());


	headers = curl_slist_append(headers, header.c_str());

	//curl_easy_setopt( curl, CURLOPT_VERBOSE, 1);

	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt( curl, CURLOPT_HTTPHEADER, headers);

	curl_easy_setopt( curl, CURLOPT_POST, TRUE );
	curl_easy_setopt( curl, CURLOPT_POSTFIELDS, utf8.c_str( ) );


	curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, wrapper_curl_write_callback);
	curl_easy_setopt( curl, CURLOPT_WRITEDATA, &byBuffer );

	CURLcode ccOK = curl_easy_perform( curl );	

	if (ccOK == CURLE_OK)
	{
		//mjeon.curl.The result buffer that is input by "wrapper_curl_write_callback" will not be zero terminated.
		//Add following code for the tracing of the crash in utf8_to_ansi.
		byBuffer.push_back('\0');

		long statLong;

		// HTTP 응답코드를 얻어온다. 
        if(CURLE_OK == curl_easy_getinfo( curl , CURLINFO_HTTP_CODE , &statLong))
		{	
			if (statLong >= 400)
			{
				sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - cURL response code is an error(%1%).", statLong));

				goto _auth_vtc_2ndPass_end;				
			}

			if (byBuffer.empty())
			{
				sc::writeLogError(std::string("CAgentServer::IdnAuthVTC_2ndPass() - response buffer is empty!"));

				goto _auth_vtc_2ndPass_end;				
			}

			// Response is about 541 bytes. Add following code for the tracing of the crash in utf8_to_ansi.
			if (byBuffer.size() > 550/*bytes*/)
			{
				sc::writeLogError(std::string("CAgentServer::IdnAuthVTC_2ndPass() - response size is too big!"));

				goto _auth_vtc_2ndPass_end;				
			}

			strResponse = std::string(&byBuffer[0]);

			// Response is about 541 bytes. Add following code for the tracing of the crash in utf8_to_ansi.
			if (strResponse.length() > 550/*bytes*/)
			{
				sc::writeLogError(std::string("CAgentServer::IdnAuthVTC_2ndPass() - response string-length is too big!"));

				goto _auth_vtc_2ndPass_end;				
			}


#if defined(_RELEASED) || defined(_DEBUG)
			std::cout << "Response code:  " << statLong << std::endl ;
#endif
		}

#if defined(_RELEASED) || defined(_DEBUG)
		// get some info about the xfer:
        double statDouble;        
        char* statString = NULL;

		if (statLong < 400)
		{
			// Content-Type 를 얻어온다.
			if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			{
				sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - Content type: %1%", statString));
			}
		}

        // 다운로드한 문서의 크기를 얻어온다.
        if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - Download size: %1%bytes", statDouble));
        }

        // 
        if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
		{
			sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - Download speed: %1%bytes/sec", statDouble));
        }
#endif

		//
		// UTF-8-to-ANSI for the result
		//	
		//std::string result = sc::string::utf8_to_ansi(&byBuffer[0], (int)byBuffer.size());
		std::string result = sc::string::utf8_to_ansi(strResponse);

		if (result.length() <= 0)
		{
			sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - result is empty for the UserID(%1%).", strID));
						
			goto _auth_vtc_2ndPass_end;
		}

		//
		//parse the UserNum from the response.
		//
		
		/* Response form (about 541 bytes)

		<?xml version="1.0" encoding="utf-8"?>
		<soap12:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soap12="http://www.w3.org/2003/05/soap-envelope">
		  <soap12:Body>
			<Account_CheckPassLevel2Response xmlns="http://tempuri.org/">
			  <Account_CheckPassLevel2Result>int</Account_CheckPassLevel2Result>
			</Account_CheckPassLevel2Response>
		  </soap12:Body>
		</soap12:Envelope>

		Result: 
		-2		- You must input Passlevel2
		-1		- Account did not have PassLevel2, please update
		-50 	- Account does not exist
		-53 	- Password is invalid
		-99 	- System Error
		 0		- Succesful
		*/

		try
		{
			ticpp::Document doc;
			doc.Parse(result);

			ticpp::Element* pHead = NULL;
			ticpp::Element* pElem = NULL;
			
			pHead = doc.FirstChildElement("soap:Envelope");
			pElem = pHead->FirstChildElement("soap:Body");
			pElem = pElem->FirstChildElement("Account_CheckPassLevel2Response");
			pElem = pElem->FirstChildElement("Account_CheckPassLevel2Result");
			
			std::string strValue = pElem->GetText();
			nRet = boost::lexical_cast<int> (strValue);
		}
		catch ( ticpp::Exception& ex )
		{
			sc::writeLogError(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - response XML couldn't be parsed for the UserID(%1%).", strID));
			sc::writeLogError(ex.m_details);
		}
	}

_auth_vtc_2ndPass_end:
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	return nRet;
}

void CAgentServer::GsMsgWBLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GS)
	{
		sc::writeLogError(sc::string::format("GS WB Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	if (!nmg)
		return;

	GS_NET_WB_LOGIN_DATA* pNml = ( GS_NET_WB_LOGIN_DATA* ) nmg;
	GS_NET_WB_LOGIN_DATA_AW msg;
		
	msg.nChannel  = pNml->nChannel;
	msg.nClientID = dwClient;
	
	::StringCchCopy(msg.szUserid,	GS_USER_ID+1,	pNml->szUserid);

	// 전장서버가 꺼져있거나 사용하지 않는 경우에는 무조건 로그인 성공
	if( SendAgent_ServerGroup( GetWorldBattleServerNumberInfo(), &msg ) == NET_ERROR )
	{
		GS_NET_WB_LOGIN_DATA_FB msg;
			
		msg.nChannel  = pNml->nChannel;
		msg.bLoginWB = false;
		msg.bWBOff = true;
		
		::StringCchCopy(msg.szUserid,	GS_USER_ID+1,	pNml->szUserid);

		SendClient( dwClient, &msg );
		sc::writeLogInfo(sc::string::format("GS WB OFFLINE. pass the login process ChaName : %1%", pNml->szUserid ));
	}
}

void CAgentServer::GsMsgWBLoginAW( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GS)
	{
		sc::writeLogError(sc::string::format("GS WB LoginAW ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	if (!nmg)
		return;

	GS_NET_WB_LOGIN_DATA_AW* pNml = ( GS_NET_WB_LOGIN_DATA_AW* ) nmg;
	GS_NET_WB_LOGIN_DATA_WA msg;
		
	msg.nChannel  = pNml->nChannel;
	msg.nClientID = pNml->nClientID;
	
	::StringCchCopy(msg.szUserid,	GS_USER_ID+1,	pNml->szUserid);
	
	/////////////////////////////////////
	// WB 서버인경우에만 동작
	// WB 에이젼트에 해당 유저가 존재하는지 확인
	msg.bLoginWB = false;

	for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
	{
		if (!m_pClientManager->IsClientSlot(i))
			continue;
		
		// 온라인 상태인경우
		if( strcmp(m_pClientManager->GetUserID(i), pNml->szUserid) == 0 )
		{
			msg.bLoginWB = true;
			break;
		}
	}
	/////////////////////////////////////

	SendClient( dwClient, &msg ); // 결과전송
}

void CAgentServer::GsMsgWBLoginWA( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GS)
	{
		sc::writeLogError(sc::string::format("GS WB LoginWA ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}
	
	GS_NET_WB_LOGIN_DATA_WA* pNml = ( GS_NET_WB_LOGIN_DATA_WA* ) nmg;
	GS_NET_WB_LOGIN_DATA_FB msg;
		
	msg.nChannel  = pNml->nChannel;
	msg.bLoginWB = pNml->bLoginWB;
	
	::StringCchCopy(msg.szUserid,	GS_USER_ID+1,	pNml->szUserid);

	SendClient( pNml->nClientID, &msg ); // 결과전송
}


void CAgentServer::GsMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GS)
	{
		sc::writeLogError(sc::string::format("GS Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	if (!nmg)
		return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	GS_NET_LOGIN_DATA* pNml = ( GS_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof(GS_NET_LOGIN_DATA) != pNml->Size())
	{
		sc::writeLogError(std::string("GS_NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

	m_Tea.decrypt (pNml->szUserid, GS_USER_ID+1);
	m_Tea.decrypt (pNml->szPassword, GS_USER_PASS+1);
	m_Tea.decrypt (pNml->szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};


	StringCchCopy (szUserid, USR_ID_LENGTH+1, pNml->szUserid);
	StringCchCopy (szPassword, USR_PASS_LENGTH+1, pNml->szPassword);
	StringCchCopy (szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt);

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{

		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = GS_NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		sc::writeLogError(sc::string::format("GsMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ));

		return;
	}
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // 접속종료

		sc::writeLogError(sc::string::format("GsMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

//		CloseClient(dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);

			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentGsUserCheck( 
		        szUserid, 
		        szPassword,
		        m_pClientManager->GetClientIP(dwClient),
		        m_nServerGroup,
		        m_nServerNum,
		        0,
		        dwClient,
		        static_cast<DWORD> (m_ServiceProvider))));
}

void CAgentServer::GsMsgLoginBack( GS_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		


    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS)
	{

		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// 아이템 변환중이면 실패처리
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "GsMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// 국가 시스템 클라이언트와 동기화;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.SetDaumGid(nlfd2->szUserid);

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			if( nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 1);
			else if( nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 2);
			else
				m_pClientManager->SetUse2ndPass(dwClient, 0);

			nlfd.m_ChaRemain = nlfd2->uChaRemain;
			nlfd.m_sCountryInfo = nlfd2->sCountryInfo;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			
			m_pClientManager->SetCountryInfo  (dwClient, nlfd2->sCountryInfo);   // 대륙 및 국가 정보;

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("GsMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else 
		{
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("GsMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) )
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
                bool bFind = FALSE;                
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum((DWORD) i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient((DWORD) i, &NetMsg);
                            bFind = TRUE;
                            sc::writeLogError(
                                sc::string::format(
                                    "GsMsgLoginBack Duplication Login CloseClient ID %1% UserDbNum %2% Slot %3%",
                                    nlfd2->szUserid,
                                    nUserDbNum,
                                    dwClient));
                            break;
                        }
                    }
                }

                if( bFind == FALSE )
                {
                    m_pDbManager->UserLogoutSimple3 (nlfd2->szUserid, m_nServerGroup );

                    sc::writeLogError(sc::string::format("GsMsgLoginBack Duplication Login UserLogoutSimple3(ID: %s, Server: %d,  ClientID: %d )", nlfd2->szUserid, m_nServerGroup, dwClient ));
                }
			}


			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("GsMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("GsMsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}

}

void CAgentServer::GmMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( !nmg )
		return;

	if (m_ServiceProvider != SP_EU)
	{
		sc::writeLogError(sc::string::format("EU Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	// 이미 인증중임...
	if ( m_pClientManager->IsAccountPassing( dwClient ) == true )
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( dwClient, true );
	}

	GM_NET_LOGIN_DATA* pNml = ( GM_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof( GM_NET_LOGIN_DATA ) != pNml->Size() )
	{
		sc::writeLogError( std::string( "GM_NET_LOGIN_DATA Wrong Message Size" ) );
		return;
	}

	m_Tea.decrypt( pNml->szUserid, GM_USER_ID+1 );
	m_Tea.decrypt( pNml->szPassword, GM_USER_PASS+1 );
	m_Tea.decrypt( pNml->szEnCrypt, ENCRYPT_KEY+1 );

	TCHAR szPassword[USR_EU_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_EU_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};

	StringCchCopy( szUserid, USR_EU_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_EU_PASS_LENGTH+1, pNml->szPassword );
	StringCchCopy( szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt );

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_SUB_FAIL;
		SendClient( dwClient, &nlfd ); // 결과전송
		m_pClientManager->SetAccountPassing( dwClient, false ); // 인증통과중 상태 false
		sc::writeLogError( sc::string::format( "GmMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ) );
		return;
	}

	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if ( nChannel < 0 || nChannel >= m_nServerChannelNumber )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // 접속종료
		sc::writeLogError( sc::string::format( "GmMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ) );
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if ( IsChannelFull( nChannel ) == true )
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
		//CloseClient( dwClient ); // 접속종료
		sc::writeLogWarn( sc::string::format( "CH(%d) is FULL", nChannel ) );
		return;
	}
	else
	{
		if ( m_pClientManager->GetChannel( dwClient ) == -1 ) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );

			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient,nChannel );
		}
		else // 기존접속자
		{

		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB 에 인증요청
	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패 세팅

	m_pDbManager->AddUserJob(
		db::DbActionPtr(
		new CAgentGmUserCheck( 
		szUserid, 
		szPassword,
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient,
		static_cast<DWORD> (m_ServiceProvider))));
}

void CAgentServer::GmMsgLoginBack( GM_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	// EU의 경우 GM의 아이디로 로그인 하기 때문에 변경해주어야 한다.
	if (m_ServiceProvider == SP_EU)
		m_pClientManager->SetUserID(dwClient, nlfd2->szUserid);

	// 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{

		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogWarn(sc::string::format("GmMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else 
		{
			// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("GmMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) )
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
				bool bFind = FALSE;                
				for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
				{
					if (!m_pClientManager->IsClientSlot(i))
						continue;

					// 온라인 상태인경우
					int nUserDbNum = m_pClientManager->UserDbNum((DWORD) i);
					if (nUserDbNum == nlfd2->nUserNum)
					{
						if (i != dwClient)
						{
							GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
							SendClient((DWORD) i, &NetMsg);
							bFind = TRUE;
							sc::writeLogError(
								sc::string::format(
								"GmMsgLoginBack Duplication Login CloseClient ID %1% UserDbNum %2% Slot %3%",
								nlfd2->szUserid,
								nUserDbNum,
								dwClient));
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					m_pDbManager->UserLogoutSimple3 (nlfd2->szUserid, m_nServerGroup );

					sc::writeLogError(sc::string::format("GmMsgLoginBack Duplication Login UserLogoutSimple3(ID: %s, Server: %d,  ClientID: %d )", nlfd2->szUserid, m_nServerGroup, dwClient ));
				}
			}


			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("GmMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("GmMsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}

}

void CAgentServer::UsMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( !nmg )
		return;

	if (m_ServiceProvider != SP_US)
	{
		sc::writeLogError(sc::string::format("US Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	// 이미 인증중임...
	if ( m_pClientManager->IsAccountPassing( dwClient ) == true )
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( dwClient, true );
	}

	US_NET_LOGIN_DATA* pNml = ( US_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof( US_NET_LOGIN_DATA ) != pNml->Size() )
	{
		sc::writeLogError( std::string( "US_NET_LOGIN_DATA Wrong Message Size" ) );
		return;
	}

	m_Tea.decrypt( pNml->szUserid, GS_USER_ID+1 );
	m_Tea.decrypt( pNml->szPassword, GS_USER_PASS+1 );
	m_Tea.decrypt( pNml->szEnCrypt, ENCRYPT_KEY+1 );

	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};


	StringCchCopy( szUserid, USR_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_PASS_LENGTH+1, pNml->szPassword );
	StringCchCopy( szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt );

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_SUB_FAIL;
		SendClient( dwClient, &nlfd ); // 결과전송
		m_pClientManager->SetAccountPassing( dwClient, false ); // 인증통과중 상태 false
		sc::writeLogError( sc::string::format( "UsMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ) );
		return;
	}

	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if ( nChannel < 0 || nChannel >= m_nServerChannelNumber )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // 접속종료
		sc::writeLogError( sc::string::format( "UsMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ) );
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if ( IsChannelFull( nChannel ) == true )
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
		//CloseClient( dwClient ); // 접속종료
		sc::writeLogWarn( sc::string::format( "CH(%d) is FULL", nChannel ) );
		return;
	}
	else
	{
		if ( m_pClientManager->GetChannel( dwClient ) == -1 ) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );

			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient,nChannel );
		}
		else // 기존접속자
		{

		}
	}

	// 사용자 id 세팅
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB 에 인증요청
	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패 세팅

	m_pDbManager->AddUserJob(
		db::DbActionPtr(
		new CAgentUsUserCheck( 
		szUserid, 
		szPassword,
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient,
		static_cast<DWORD> (m_ServiceProvider))));
}

void CAgentServer::UsMsgLoginBack( US_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		


	// 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{

		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// 아이템 변환중이면 실패처리
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "UsMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogWarn(sc::string::format("UsMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else 
		{
			// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("UsMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) )
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
				bool bFind = FALSE;                
				for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
				{
					if (!m_pClientManager->IsClientSlot(i))
						continue;

					// 온라인 상태인경우
					int nUserDbNum = m_pClientManager->UserDbNum((DWORD) i);
					if (nUserDbNum == nlfd2->nUserNum)
					{
						if (i != dwClient)
						{
							GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
							SendClient((DWORD) i, &NetMsg);
							bFind = TRUE;
							sc::writeLogError(
								sc::string::format(
								"UsMsgLoginBack Duplication Login CloseClient ID %1% UserDbNum %2% Slot %3%",
								nlfd2->szUserid,
								nUserDbNum,
								dwClient));
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					m_pDbManager->UserLogoutSimple3 (nlfd2->szUserid, m_nServerGroup );

					sc::writeLogError(sc::string::format("UsMsgLoginBack Duplication Login UserLogoutSimple3(ID: %s, Server: %d,  ClientID: %d )", nlfd2->szUserid, m_nServerGroup, dwClient ));
				}
			}


			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("UsMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("UsMsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}

}

/*
* 일반 로그인 결과 : id / pwd 입력시
*/
void CAgentServer::MsgLogInBack (NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->m_ClientSlot;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS )
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// 아이템 변환중이면 실패처리
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "MsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// 국가 시스템 클라이언트와 동기화;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.SetDaumGid(nlfd2->szUserid);

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			if( nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 1);
			else if( nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 2);
			else
				m_pClientManager->SetUse2ndPass(dwClient, 0);

			nlfd.m_ChaRemain = nlfd2->uChaRemain;
			nlfd.m_sCountryInfo = nlfd2->sCountryInfo;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			
			m_pClientManager->SetCountryInfo  (dwClient, nlfd2->sCountryInfo);	 // 대륙 및 국가 정보;

			// 내부 테스트를 위해서 말레이시아 User Class Type을 추가한다.
			//	IP보너스
/*
#if defined(_RELEASED)			
			m_pClientManager->SetIPBonusClass	  ( dwClient, nlfd2->emIPBonus ); // 테스트용
#endif
*/			

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 중국, 베트남 LastLoginDate 세팅
//#if defined( VN_PARAM ) || defined(_RELEASED) //vietnamtest%%%
			if (m_ServiceProvider == SP_VIETNAM)
			{
				m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // 베트남 게임시간 세팅
			}
//#endif

			
			//
			//mjeon.ToDo
			//m_pClientManager->SetAccountPass(dwClient, true); is doubly called.
			//It was already called on 1054th line in this src file.
			//Remove the following code line.
			//			
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			// 극강부 남/여 생성 가능 숫자를 넘겨준다.            
            // 극강부 추가
            if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA_TEST || m_ServiceProvider == SP_KOREA ||
                m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG ||
                m_ServiceProvider == SP_INDONESIA || m_ServiceProvider == SP_WORLD_BATTLE)
            {
				nlfd.m_Extreme = nlfd2->bExtreme;
            }
            else
            {
				nlfd.m_Extreme = nlfd2->bExtreme = FALSE;
            }
			nlfd.m_CheckFlag = nlfd2->nCheckFlag; // 한국 CheckFlag Test를 위해서 추가

			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

            //if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
            //{
			    int nTracing = CheckTracingUser( nlfd2->szUserid, nlfd2->nUserNum );
			    if( nTracing != -1 )
			    {
				    m_vecTracingData[nTracing].dwOnLine       = TRUE;
				    m_vecTracingData[nTracing].dwUserNum      = nlfd2->nUserNum;
				    m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
				    m_vecTracingData[nTracing].SetAccount(nlfd2->szUserid);

				    CDebugSet::ToTracingFile( nlfd2->szUserid, "############ Tracing User Login ############" );
				    CDebugSet::ToTracingFile( nlfd2->szUserid, "Account: [%s], UserNum: [%u], IP: [%s]", nlfd2->szUserid, nlfd2->nUserNum, nlfd2->szIp );
				    GLMSG::NET_UPDATE_TRACINGCHAR msg;
				    msg.tracingData = m_vecTracingData[nTracing];
				    msg.updateNum   = nTracing;
    							
				    SendSession( &msg );
			    }
            //}

			// APEX 버젼 변경
            if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
            {
			    APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

                std::string IpAddress = m_pClientManager->GetClientIP(dwClient);
			    int nIp  = inet_addr(IpAddress.c_str());

			    APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
            }
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
            return;
		}		
		else 
		{
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else 
	{
        // 로그인 실패일때
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0)  )
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
                bool bFind = FALSE;
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum((DWORD) i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient((DWORD) i, &NetMsg);
                            bFind = TRUE;
                            sc::writeLogError(
                                sc::string::format(
                                    "MsgLoginBack Duplication Login CloseClient ID %1% UserDbNum %2% Slot %3%",
                                    nlfd2->szUserid,
                                    nUserDbNum,
                                    dwClient));
                            break;
                        }
                    }
                }

                if( bFind == FALSE )
                {
                    m_pDbManager->UserLogoutSimple3(nlfd2->szUserid, m_nServerGroup);
                    sc::writeLogError(
                        sc::string::format(
                            "MsgLoginBack Duplication Login UserLogoutSimple3 ID %1% Server %2% Slot %3%",
                            nlfd2->szUserid,
                            m_nServerGroup,
                            dwClient));
                }
			}
			// 랜덤키 추가%%%
//#if defined TW_PARAM || defined HK_PARAM || defined _RELEASED
			else if( nlfd2->m_emResult == EM_LOGIN_FB_SUB_RANDOM_PASS )
			{
				if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_HONGKONG)
				{
					m_pClientManager->ResetRandomPassNumber( dwClient );
					MsgSndRandomNumber(dwClient, 0, NULL);
				}
			}
//#endif


			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}
}


//
//mjeon.indonesia
//
void CAgentServer::IdnMsgLogInBack(NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD)nlfd2->m_ClientSlot;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		//
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		//
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0)  )
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result = nlfd2->m_emResult;

			if (nlfd2->uChaRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain));
				nlfd2->uChaRemain = 0;
			}
			
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain));
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum    (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간			

			//
			// 내부 테스트를 위해서 말레이시아 User Class Type을 추가한다.
			//	IP보너스
/*
#if defined(_RELEASED)			
			m_pClientManager->SetIPBonusClass	  ( dwClient, nlfd2->emIPBonus ); // 테스트용
#endif
*/			

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 중국, 베트남 LastLoginDate 세팅

			
			//
			//mjeon.ToDo
			//m_pClientManager->SetAccountPass(dwClient, true); is doubly called.
			//It was already called on 1054th line in this src file.
			//Remove the following code line.
			//
			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
   			nlfd.m_CheckFlag = nlfd2->nCheckFlag;	// 한국 CheckFlag Test를 위해서 추가
			m_pClientManager->SetExtremeCreate(dwClient, nlfd2->bExtreme);

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

            
		    int nTracing = CheckTracingUser(nlfd2->szUserid, nlfd2->nUserNum);

		    if( nTracing != -1 )
		    {
			    m_vecTracingData[nTracing].dwOnLine       = TRUE;
			    m_vecTracingData[nTracing].dwUserNum      = nlfd2->nUserNum;
			    m_vecTracingData[nTracing].nConnectServer = m_nServerGroup;
			    m_vecTracingData[nTracing].SetAccount(nlfd2->szUserid);

			    CDebugSet::ToTracingFile( nlfd2->szUserid, "############ Tracing User Login ############" );
			    CDebugSet::ToTracingFile( nlfd2->szUserid, "Account: [%s], UserNum: [%u], IP: [%s]", nlfd2->szUserid, nlfd2->nUserNum, nlfd2->szIp );
			    
				GLMSG::NET_UPDATE_TRACINGCHAR msg;
			    msg.tracingData = m_vecTracingData[nTracing];
			    msg.updateNum   = nTracing;
							
			    SendSession( &msg );
		    }
            
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
            sc::writeLogWarn(sc::string::format("IdnMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else 
		{
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0) )
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
				BOOL bFind = FALSE;
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum(i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient((DWORD) i, &NetMsg);
							bFind = TRUE;                            
							sc::writeLogError(
                                sc::string::format(
                                    "IdnMsgLoginBack - Double Login detected. UserID %1% Slot %2%",
                                    nlfd2->szUserid,
                                    dwClient));
							break;
                        }
                    }
                }

                if (!bFind)
                {
                    m_pDbManager->UserLogoutSimple3(nlfd2->szUserid, m_nServerGroup);
                    sc::writeLogError(
                        sc::string::format(
                            "IdnMsgLoginBack - Double Login detected. UserID %1% Server %2% Slot %3%",
                            nlfd2->szUserid,
                            m_nServerGroup,
                            dwClient));
                }
			}

			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);

			return;
		}
		//
		// 로그인 실패이고 이미 오프라인일때...
		//
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("IdnMsgLoginBack() - Login failed and the client is already offline(%s)", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;

			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("IdnMsgLoginBack() - Login failed and ID or PW is wrong(%s)", nlfd2->szUserid));
			
			return;
		}
	}
}


void CAgentServer::DaumMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_KOREA && m_ServiceProvider != SP_KOREA_TEST )
	{
		sc::writeLogError(sc::string::format("DAUM Login ERROR (ServicePrivider is not match GaeaID : %1%, SP : %2%)", dwGaeaID, m_ServiceProvider ));
		return;
	}

	if (nmg == NULL)
		return;

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	DAUM_NET_LOGIN_DATA* pLogin = ( DAUM_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		sc::writeLogError(sc::string::format("DaumMsgLogin Invalid Channel Number (Channel:%d)", nChannel) );
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	// UUID Check
	// UUID copy
	TCHAR szUUID[sc::string::UUID_LENGTH] = {0};
	StringCchCopy( szUUID, sc::string::UUID_LENGTH, pLogin->szUUID);

	// 로그인 과정이 끝나고 계정을 체크하기 위해서 UUID 를 세팅한다.
	// 해킹을 통해서 다른 UUID 인것처럼 속일 가능성이 있다.
	m_pClientManager->KorSetUUID( dwClient, szUUID );

	// UUID를 이용해서 사용자 정보를 체크한다.
	m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentDaumUserCheck(
		        szUUID,
		        m_pClientManager->GetClientIP(dwClient),
		        m_nServerGroup,
		        m_nServerNum,
		        0,
		        dwClient)));
}

///////////////////////////////////////////////////////////////////////////////
// 다음 : 로그인 결과
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	if (!pMsg)
        return;

	NET_LOGIN_FEEDBACK_DATA	MsgAC;
	
	//MsgAC.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (pMsg->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	MsgAC.nType           = NET_MSG_LOGIN_FB;
	MsgAC.m_GameVersion = m_PatchVersion;
	MsgAC.m_LauncherVersion= m_LauncherVersion;

	m_pClientManager->SetUserID(dwClient, pMsg->szDaumGID);    

	if (pMsg->m_emResult == EM_LOGIN_FB_SUB_OK || pMsg->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || pMsg->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS ) // 로그인 성공일때
	{
		// 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == pMsg->szIp) &&
			(strcmp(m_pClientManager->KorGetUUID(dwClient), pMsg->szUUID) == 0))
		{
			// 아이템 변환중이면 실패처리
			if ( 2 == pMsg->ItemTransferResult )
			{
				MsgAC.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &MsgAC );
				m_pDbManager->UserLogoutSimple( pMsg->szDaumGID );
				sc::writeLogError( sc::string::format( "DaumMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", pMsg->szDaumGID ) );
				return;
			}

			// 국가 시스템 클라이언트와 동기화;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공            

			// 한국 셧다운제 체크
			if ((m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST) &&
				m_pConfig->GetKorShutdown())
			{
				// 셧다운제 16세
				if (pMsg->GetAge() < 16)
				{
					int CurrentHour = m_CurrentTime.GetHour();
					if (CurrentHour >= 0 && CurrentHour < 6)
					{
						pMsg->m_emResult = EM_LOGIN_FB_KOR_SHUTDOWN;
					}
				}
			}

			MsgAC.SetDaumGid(pMsg->szDaumGID);

			MsgAC.m_Result = pMsg->m_emResult;
            
			if (pMsg->uChaRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", pMsg->uChaRemain ));
				pMsg->uChaRemain = 0;
			}
			if (pMsg->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", pMsg->uChaTestRemain ));
				pMsg->uChaTestRemain = 0;
			}

            if (m_pConfig->IsTestServer()) 
            {
                MsgAC.m_ChaRemain = pMsg->uChaTestRemain;
            }
            else
            {
                MsgAC.m_ChaRemain = pMsg->uChaRemain;
            }

			if( pMsg->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 1);
			else if( pMsg->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS )
				m_pClientManager->SetUse2ndPass(dwClient, 2);
			else
				m_pClientManager->SetUse2ndPass(dwClient, 0);

			
			// Daum 패스워드를 받기위한 Check Flag 값
			MsgAC.m_CheckFlag = pMsg->nCheckFlag;
			MsgAC.m_sCountryInfo = pMsg->sCountryInfo;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum    (dwClient, pMsg->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, pMsg->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, pMsg->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, pMsg->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, pMsg->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, pMsg->tChatBlockTime); // 채팅 블록 기간
            m_pClientManager->SetAge          (dwClient, pMsg->m_wAge);         // 나이
			m_pClientManager->SetCountryInfo( dwClient, pMsg->sCountryInfo);
/*
            sc::writeLogInfo(
                sc::string::format(
                    "CAgentServer::DaumMsgLoginBack Age %1%", pMsg->m_wAge));
*/

            // 극강부 남/여 생성 가능 숫자를 넘겨준다.
            // 극강부 추가
            //if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST ||
            //    m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG ||
            //    m_ServiceProvider == SP_INDONESIA || m_ServiceProvider == SP_WORLD_BATTLE)
            //{
				//MsgAC.m_ExtremeM = pMsg->nExtremeM;
				//MsgAC.m_ExtremeW = pMsg->nExtremeW;
				MsgAC.m_Extreme = pMsg->bExtreme;
            //}
            //else
            //{
				//    MsgAC.m_ExtremeM = pMsg->nExtremeM = 0;
				//    MsgAC.m_ExtremeW = pMsg->nExtremeW = 0;
            //}

			m_pClientManager->SetExtremeCreate( dwClient, pMsg->bExtreme );

			MsgAC.m_bJumping = pMsg->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, pMsg->bJumping );

			MsgAC.m_bActor = pMsg->bActor;
			m_pClientManager->SetActorCreate( dwClient, pMsg->bActor );

			SendClient(dwClient, &MsgAC);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 이미 오프라인
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
			m_pDbManager->UserLogoutSimple(pMsg->szDaumGID);
			sc::writeLogWarn(sc::string::format("DaumMsgLoginBack User Already Offline %1%", pMsg->szDaumGID));
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(pMsg->szDaumGID);
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User IP/ID Wrong(%s)", pMsg->szDaumGID ));
			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false); // 인증중 false

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == pMsg->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   pMsg->szDaumGID) == 0))
		{
			if (pMsg->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
				bool bFind = FALSE;
				for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
				{
					if (!m_pClientManager->IsClientSlot(i))
						continue;

					// 온라인 상태인경우
					int nUserDbNum = m_pClientManager->UserDbNum(i);
					if (nUserDbNum == pMsg->nUserNum)
					{
						if (i != dwClient)
						{
							GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
							SendClient(i, &NetMsg);

							bFind = TRUE;
							sc::writeLogError(
								sc::string::format(
									"DaumMsgLoginBack() - Duplicated Login of ID(%1%), UserDbNum(%2%), Slot(%3%)"
										,pMsg->szDaumGID, pMsg->nUserNum, dwClient));
							break;
						}
					}
				}

				if( bFind == FALSE )
				{
					m_pDbManager->UserLogoutSimple(pMsg->szDaumGID);

					sc::writeLogError(
						sc::string::format(
							"DaumMsgLoginBack() - UserLogoutSimple of ID(%1%), UserDbNum(%2%), Slot(%3%)"
								,pMsg->szDaumGID, pMsg->nUserNum, dwClient));
				}
			}

			MsgAC.m_Result = pMsg->m_emResult;
			SendClient(dwClient, &MsgAC);
			return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)", pMsg->szDaumGID ));
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User Online ID/PWD Wrong(%s)", pMsg->szDaumGID ));
			// 현재사용자는 대기
			return;
		}
	}
}

void CAgentServer::GspMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GLOBAL) return;

	if (nmg == NULL) return;
	
	m_pClientManager->IncLoginAttempt( dwClient );

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing( dwClient ) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing( dwClient, true );
	}	

	GSP_NET_LOGIN_DATA* pLogin = ( GSP_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		sc::writeLogError(sc::string::format("GspMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull( nChannel ) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // 결과전송
//		CloseClient( dwClient ); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser( nChannel );
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel( dwClient, nChannel );
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}	

	// UUID Check
	// UUID copy	
	TCHAR szUUID[sc::string::UUID_LENGTH] = {0};
	StringCchCopy( szUUID, sc::string::UUID_LENGTH, pLogin->szUUID);

	// 로그인 과정이 끝나고 계정을 체크하기 위해서 UUID 를 세팅한다.
	// 해킹을 통해서 다른 UUID 인것처럼 속일 가능성이 있다.
	m_pClientManager->GspSetUUID( dwClient, szUUID );

	// UUID를 이용해서 사용자 정보를 체크한다.
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentGspUserCheck(
		        szUUID,
		        m_pClientManager->GetClientIP(dwClient),
		        m_nServerGroup,
		        m_nServerNum,
		        0,
		        dwClient)));
}

void CAgentServer::GspMsgLoginBack( GSP_NET_LOGIN_FEEDBACK_DATA2* nlfd2 )
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	//nlfd.nType = NET_MSG_LOGIN_FB;	

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	// GspUserID
	m_pClientManager->GspSetUserID( dwClient, nlfd2->szGspUserID );

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, Decoded TID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0) )
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			nlfd.m_Result    = nlfd2->m_emResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

            if (m_pConfig->IsTestServer()) 
            {
                nlfd.m_ChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.m_ChaRemain = nlfd2->uChaRemain;
            }

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 이미 오프라인
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
			m_pDbManager->UserLogoutSimple(nlfd2->szGspUserID);
			sc::writeLogWarn(sc::string::format("GspMsgLoginBack User Already Offline %1%", nlfd2->szGspUserID));
			return;
		}		
		else 
		{
            // 로그인성공, 온라인이지만 ip, DecodedTID 틀릴때...
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szGspUserID);
			sc::writeLogError(sc::string::format("GspMsgLoginBack User IP/UUID Wrong %1%/%2%", nlfd2->szGspUserID, nlfd2->szUUID));

			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // 인증통과 실패
		m_pClientManager->SetAccountPassing( dwClient, false );

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP( dwClient ) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0))
		{
			nlfd.m_Result = nlfd2->m_emResult;
			SendClient( dwClient, &nlfd );
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("GspMsgLoginBack User Offline AND Login Fail(%s)", nlfd2->szGspUserID ));
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			sc::writeLogError(sc::string::format("GspMsgLoginBack User Online Wrong(%s)", nlfd2->szGspUserID ));
			// 현재사용자는 대기
			return;
		}
	}
}

/**
 * Terra Login message
 * \param pMsg Login message
 */
void CAgentServer::TerraMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_PHILIPPINES && m_ServiceProvider != SP_MALAYSIA ) return;

	if (nmg == NULL) return;

	m_pClientManager->IncLoginAttempt(dwClient);

	// 로그인 시도횟수가 5회 이상이면 접속을 종료한다.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // 접속종료		
		return;
	}

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	CString strTID;

	TERRA_NET_LOGIN_DATA* pLogin = ( TERRA_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		sc::writeLogError(sc::string::format("TerraMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		sc::writeLogWarn( sc::string::format("CH(%d) is FULL", nChannel ));

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	char szDecodedTID[TERRA_TID_DECODE+1] = {0}; // Decoded TID

	// Encoding 된 문자열을 Decoding 한다.
	if (TerraGetDecodedTID(pLogin->szTID, szDecodedTID) == NET_ERROR) 
	{ 		
		// Decoding 실패시 코딩
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // 접속종료
		sc::writeLogError(sc::string::format("TerraGetDecodedTID %s", pLogin->szTID ));
		return;
	}
	else
	{
		// 로그인 과정이 끝나고 계정을 체크하기 위해서 TID 를 세팅한다.
		m_pClientManager->TerraSetDecodedTID(dwClient, szDecodedTID);

		// Decoding 된 TID 를 이용해서 사용자 정보를 체크한다.
        m_pDbManager->AddUserJob(
            db::DbActionPtr(
                new CAgentTerraUserCheck(
                    szDecodedTID,
                    m_pClientManager->GetClientIP(dwClient),
                    m_nServerGroup,
                    m_nServerNum,
                    0,
                    dwClient)));
	}
}

/**
* Terra Login message back
* \param nlfd2 로그인 Feedback data
*/
void CAgentServer::TerraMsgLoginBack(TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	//nlfd.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.	
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	// TLoginName 
	m_pClientManager->SetUserID(dwClient, nlfd2->szTerraTLoginName);

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, Decoded TID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			nlfd.SetDaumGid(nlfd2->szTerraTLoginName);

			nlfd.m_Result    = nlfd2->m_emResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

            if (m_pConfig->IsTestServer()) 
            {
                nlfd.m_ChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.m_ChaRemain = nlfd2->uChaRemain;
            }

			// 말레이지아 패스워드를 받기위한 Check Flag 값
			nlfd.m_CheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간
//	IP보너스
//#if	defined( MYE_PARAM ) || defined( MY_PARAM ) || defined ( PH_PARAM ) 
			if (m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
			{
				m_pClientManager->SetIPBonusClass ( dwClient, nlfd2->emIPBonus );  // 말레이시아 사용자 Class Type
			}
//#endif

			// 극강부 추가
//#if	defined( MYE_PARAM ) || defined ( MY_PARAM ) || defined ( PH_PARAM )
			if (m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
			{
				nlfd.m_Extreme = nlfd2->bExtreme;
			}
			else
			{
//#else
				nlfd.m_Extreme = nlfd2->bExtreme = FALSE;
			}
//#endif
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

//#if defined( VN_PARAM ) //vietnamTEst%%%

			if (m_ServiceProvider == SP_VIETNAM)
			{
				m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // 베트남 게임시간 세팅
				m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // 베트남 LastLoginDate 세팅
			}
//#endif
			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 이미 오프라인
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
			sc::writeLogWarn(sc::string::format("TerraMsgLoginBack User Already Offline %1%", nlfd2->szTerraTLoginName));

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 로그인 실패 전송
	
			return;
		}
		// 로그인성공, 온라인이지만 ip, DecodedTID 틀릴때...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User IP/DecodedTID Wrong(%s)", nlfd2->szTerraTLoginName ));

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			CloseClient( dwClient, net::KICKOUT_UNDEFINE );

			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
			{
                bool bFind = FALSE;                
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum(i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient(i, &NetMsg);
                            bFind = TRUE;
                            sc::writeLogError(
                                sc::string::format(
                                    "TerraMsgLoginBack Duplication Login. %1% UserDbNm %2% Slot %3%",
                                    nlfd2->szTerraTLoginName,
                                    nlfd2->nUserNum,
                                    dwClient));
                            break;
                        }
                    }
                }

				if (!bFind)
				{
					m_pDbManager->UserLogoutSimple3(nlfd2->szTerraTLoginName, m_nServerGroup);
					sc::writeLogError(
                        sc::string::format(
                            "TerraMsgLoginBack Duplication Login. UserLogoutSimple3 ID %1% Server %2% UserDbNum %3% Slot %4%",
                            nlfd2->szTerraTLoginName,
                            m_nServerGroup,
                            nlfd2->nUserNum,
                            dwClient));
				}
			}

			nlfd.m_Result = nlfd2->m_emResult;
			SendClient(dwClient, &nlfd);
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User Offline AND Login Fail(%s)", nlfd2->szTerraTLoginName ));

			// 인증통과 실패시 부여받은 ID값에 대한 CloseClient의 호출여부가 애매하다.
			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 로그인 실패 전송

		
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User Online Wrong(%s)", nlfd2->szTerraTLoginName ));
			// 현재사용자는 대기
			return;
		}
	}
}

/**
* Encoding 된 TID 에서 Decoding 된 TID 를 얻는다.
* \param szEncodedTID Encoding 된 TID
* \param szDecodedTID Decoding 된 TID
* \return
*/
int CAgentServer::TerraGetDecodedTID(char* szEncodedTID, char* szDecodedTID)
{
	if (szEncodedTID == NULL)
	{
		sc::writeLogError(std::string("CAgentServer::TerraGetDecodedTID() - szEncodedTID is NULL"));
		return NET_ERROR;
	}

	if (szDecodedTID == NULL)
	{
		sc::writeLogError(std::string("CAgentServer::TerraGetDecodedTID() - szDecodedTID is NULL"));
		return NET_ERROR;
	}

	
	_bstr_t bstrEncodedTID = szEncodedTID;
	_bstr_t bstrRestore;
	_bstr_t bstrDest;
	HRESULT hResult;

	if (m_ServiceProvider == SP_PHILIPPINES || m_ServiceProvider == SP_VIETNAM)
	{
		if (m_pIEGameEncrypto == NULL)
		{
			sc::writeLogError(std::string("CAgentServer::TerraGetDecodedTID() - m_pIEGameEncrypto is NULL"));
			return NET_ERROR;
		}

		hResult = m_pIEGameEncrypto->Decode(bstrEncodedTID, bstrDest.GetAddress());
	}
	else if (m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN)
	{
		if (m_pMyRossoEncrypto == NULL)
		{
			sc::writeLogError(std::string("CAgentServer::TerraGetDecodedTID() - m_pMyRossoEncrypto is NULL"));
			return NET_ERROR;
		}

		hResult = m_pMyRossoEncrypto->Decode(bstrEncodedTID, bstrDest.GetAddress());
	}


	if (FAILED(hResult))
	{
		sc::writeLogError(std::string("m_pMyRossoEncrypto->Decode()"));
		return NET_ERROR;
	}
	else
	{
		StringCchCopy ( szDecodedTID, TERRA_TID_DECODE+1, (char*) bstrDest );
		// Test Code Start
		/*
		FILE* fFile = fopen("TerraTid.txt", "a+");
		fprintf(fFile, "Encoded TID:%s \n", szEncodedTID);
		fprintf(fFile, "Decoded TID:%s \n", szDecodedTID);	
		fclose(fFile);
		*/
		// Test Code End
		return NET_OK;
	}
}

void CAgentServer::ExciteMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_JAPAN) return;

	if (nmg == NULL) return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	EXCITE_NET_LOGIN_DATA* pLogin = ( EXCITE_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

	// 접속하려는 채널 검사
	int nChannel = pLogin->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료

		sc::writeLogError(sc::string::format("ExciteMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
//		CloseClient(dwClient); // 접속종료
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// Full 이 아닐경우 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		else // 기존접속자
		{
		}
	}

	CString strUserID = pLogin->szUID;	
	CString strTDate = pLogin->szTDATE;	

	bool bMd5Result = false;
	bMd5Result = m_MinMd5.ExciteGetMd5(pLogin->szUID,
		                               pLogin->szTDATE,
						               pLogin->szMD5);
	if (bMd5Result == false)
	{
		// 정상적이지 않은 데이터이기 때문에 잘못된 사용자임.
		nlfd.m_Result = EM_LOGIN_FB_SUB_UNCON;
		SendClient(dwClient, &nlfd);
		sc::writeLogError(sc::string::format("ExciteMsgLogin ExciteGetMd5 false (%s)", pLogin->szUID ));
		return;
	} // if (bMd5Result == FALSE)

	bool bDateCheck = false;
	bDateCheck = m_MinMd5.ExciteCheckDate(strTDate);
	if (bDateCheck == false)
	{
		// 인증시간이 초과된 사용자이기 때문에, 웹에 재접속을 하라는 메시지를 보낸다.			
		nlfd.m_Result = EM_LOGIN_FB_SUB_EXPIRED;
		SendClient(dwClient, &nlfd);
		sc::writeLogError(sc::string::format("ExciteMsgLogin time expired (%s)", pLogin->szTDATE ));
		return;
	} // if (bDateCheck == FALSE)	

	// 사용자 ID 세팅
	m_pClientManager->SetUserID(dwClient, strUserID.GetString());

	// 사용자 UID 를 비교, 그에 맞는 로그인 피드백 데이타를 전송한다.
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentExciteUserCheck(
                strUserID.GetString(),
                m_pClientManager->GetClientIP(dwClient),
                m_nServerGroup,
                m_nServerNum,
                0,
                dwClient)));
}

void CAgentServer::ExciteMsgLoginBack(EXCITE_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	//nlfd.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// 로그인 요청한 클라이언트와 동일한지 검사한다.	
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szExciteUserID) == 0))
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과 성공

			nlfd.m_Result    = nlfd2->m_emResult;
            
			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

            if (m_pConfig->IsTestServer()) 
            {
                nlfd.m_ChaRemain = nlfd2->uChaTestRemain;
            }
            else 
            {
                nlfd.m_ChaRemain = nlfd2->uChaRemain;
            }

			nlfd.m_CheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 이미 오프라인
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
			sc::writeLogWarn(sc::string::format("ExciteMsgLoginBack User Already Offline %1%", nlfd2->szExciteUserID));
			return;
		}
		// 로그인성공, 온라인이지만 ip, id 가 틀릴때...
		else
		{
			m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패

			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User IP/ID Wrong(%s)", nlfd2->szExciteUserID ));
			// 현재사용자는 대기시킨다. 
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
        m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
        m_pClientManager->SetAccountPassing(dwClient, false); // 인증중 false

        // 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
        if ((m_pClientManager->IsOnline(dwClient) == true) &&
            (m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
            (strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szExciteUserID) == 0))
        {

            if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
            {
                bool bFind = FALSE;
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum(i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient(i, &NetMsg);
                            bFind = TRUE;
                            sc::writeLogError(
                                sc::string::format(
                                    "ExciteMsgLoginBack Duplication Login. UserID %1% UserDbNum %2% Slot %3%",
                                    nlfd2->szExciteUserID,
                                    nlfd2->nUserNum,
                                    dwClient));
                            break;
                        }
                    }
                }

                if (!bFind)
                {
                    m_pDbManager->UserLogoutSimple3(nlfd2->szExciteUserID, m_nServerGroup);
                    sc::writeLogError(
                        sc::string::format(
                            "ExciteMsgLoginBack Duplication Login. UserLogoutSimple3 UserID %1% Server %2% UserDbNum %3% Slot %4%",
                            nlfd2->szExciteUserID,
                            m_nServerGroup,
                            nlfd2->nUserNum,
                            dwClient));
                }
            }


            nlfd.m_Result = nlfd2->m_emResult;
            SendClient(dwClient, &nlfd);
            return;
        }
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User Offline AND ID/PWD Wrong(%s)", nlfd2->szExciteUserID ));
			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User Online ID/PWD Wrong(%s)", nlfd2->szExciteUserID ));
			// 현재사용자는 대기
			return;
		}
	}
}


/**
* 일본Gonzo 로그인 : id / pwd 입력시
*/
void CAgentServer::JapanMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_JAPAN) return;

	if (nmg == NULL)
		return;

	// 이미 인증중임...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // 인증중으로 세팅
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	JAPAN_NET_LOGIN_DATA* pNml = ( JAPAN_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	if (sizeof(JAPAN_NET_LOGIN_DATA) != pNml->Size())
	{
		sc::writeLogError(std::string("JAPAN_NET_LOGIN_DATA Wrong Message Size"));
		return;
	}

	// Tea 알고리즘으로 암호화된 패킷 복호화
	m_Tea.decrypt (pNml->szUserid, JAPAN_USER_ID+1);
	m_Tea.decrypt (pNml->szPassword, JAPAN_USER_PASS+1);
	m_Tea.decrypt (pNml->szEnCrypt, ENCRYPT_KEY+1);

	TCHAR szPassword[USR_PASS_LENGTH+1] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};
	TCHAR szEncrypt[ENCRYPT_KEY+1] = {0};

	StringCchCopy (szUserid, JAPAN_USER_ID+1, pNml->szUserid);
	StringCchCopy (szPassword, JAPAN_USER_PASS+1, pNml->szPassword);
	StringCchCopy (szEncrypt, ENCRYPT_KEY+1, pNml->szEnCrypt);

	if ( _tcscmp( szEncrypt, m_szEncrypt ) )
	{

		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // 결과전송
		m_pClientManager->SetAccountPassing (dwClient, false); // 인증통과중 상태 false

		sc::writeLogError(sc::string::format("JapanMsgLogin Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ));

		return;
	}
	
	// 접속하려는 채널 검사
	int nChannel = pNml->nChannel;

	// 채널 번호가 올바른지 검사한다.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // 접속종료

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// 해당 채널을 조사해서 full 인지 검사해야 한다.
	if (IsChannelFull(nChannel) == true)
	{
		// 해당 채널이 full 이면 연결을 끊어야 한다.
		// Full 일 경우 처리한다.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // 결과전송

		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // 신규접속자
		{
			// 해당 채널 유저수를 1 증가시킨다.
			IncreaseChannelUser(nChannel);
			// 클라이언트에 채널번호를 세팅한다.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // 기존접속자
		{
			
		}
	}

	// WhiteRock과의 통신 하는 부분을 Thread 처리
    m_pDbManager->AddWebJob(
        db::DbActionPtr(
            new CAgentJapnaWRLogin(
			    szUserid,
			    szPassword,
			    m_pClientManager->GetClientIP(dwClient),
			    m_nServerGroup,
			    m_nServerNum,
			    dwClient)));	
/*
	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패 세팅
	
	CAgentJapanUserCheck* pAction = new CAgentJapanUserCheck( 
		szUserid, 
		m_pClientManager->GetClientIP(dwClient),
		m_nServerGroup,
		m_nServerNum,
		0,
		dwClient);
	
	m_pDbManager->AddUserJob((CDbAction*) pAction);
*/
}

// 일본 Gonzo Japan WhiteRock 시스템과의 통신 실패
void CAgentServer::JapanWRFailed( DWORD dwClient, int nResult )
{
	DWORD dwClientNum = 0;
	dwClientNum = dwClient;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	nlfd.nType = NET_MSG_LOGIN_FB;
	
	if( HTTP_STATUS_BAD_GATEWAY == nResult )
		nlfd.m_Result = EM_LOGIN_FB_SUB_INCORRECT; // ID, PW 불일치로 로그인 실패
	else if( HTTP_STATUS_ALREADY_LOGIN == nResult )
		nlfd.m_Result = EM_LOGIN_FB_SUB_DUP;	// 중복로그인
	else
		nlfd.m_Result = EM_LOGIN_FB_SUB_SYSTEM; // 시스템 에러로 로그인 실패

	SendClient(dwClientNum, &nlfd); // 결과전송
	m_pClientManager->SetAccountPassing (dwClientNum, false); // 인증통과중 상태 false
	return;
}

// 일본 Gonzo Japan WhiteRock 시스템과의 통신 성공
void CAgentServer::JapanWRSuccess( DWORD dwClient, CString strUserID,
									int nServerGroup, int nServerNum, CString strUUID, int nUserNum )
{
	TCHAR szUserID[JAPAN_USER_ID+1] = {0};
	int nSGroup = 0;
	int nSNum = 0;
	int nUserNumber = 0;
	DWORD dwClientNum = 0;
	TCHAR szUUID[sc::string::UUID_LENGTH] = {0};

	StringCchCopy( szUserID, JAPAN_USER_ID+1, strUserID);
	nSGroup = nServerGroup;
	nSNum = nServerNum;
	dwClientNum = dwClient;
	nUserNumber = nUserNum;
	StringCchCopy( szUUID, sc::string::UUID_LENGTH, strUUID);

	m_pClientManager->JPSetUUID( dwClientNum, szUUID );

	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClientNum, szUserID);
	// DB 에 인증요청
	m_pClientManager->SetAccountPass(dwClientNum, false); // 인증통과 실패 세팅
	
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentJapanUserCheck( 
		        szUserID, 
		        m_pClientManager->GetClientIP(dwClientNum),
		        nSGroup,
		        nSNum,
		        0,
		        dwClientNum,
		        nUserNumber)));
}

/*
* 일본 로그인 결과 : id / pwd 입력시
*/
void CAgentServer::JapanMsgLoginBack (JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // 로그인 성공
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// 로그인 성공이고 온라인이고 IP, ID 가 동일할때...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->JapanUserID) == 0) )
		{
			// Use Features 동기화;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			nlfd.m_Result    = nlfd2->m_emResult;

			if (nlfd2->uChaRemain     > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaRemain Error : %hu", nlfd2->uChaRemain ));
				nlfd2->uChaRemain     = 0;
			}
			if (nlfd2->uChaTestRemain > MAX_CHAR_LENGTH)
			{
				sc::writeLogError(sc::string::format("uChaTestRemain Error : %hu", nlfd2->uChaTestRemain ));
				nlfd2->uChaTestRemain = 0;
			}

			nlfd.m_ChaRemain = nlfd2->uChaRemain;

			m_pClientManager->SetAccountPass  (dwClient, true);
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // 유저번호
			m_pClientManager->SetLoginTime    (dwClient);                        // 로그인 시간
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // 유저타입세팅
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // 만들수 있는 캐릭터 갯수
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // 테스트 서버에서 만들 수 있는 캐릭터 갯수
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // 프리미엄 서비스 기간
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // 채팅 블록 기간

			m_pClientManager->SetAccountPass(dwClient, true); // 인증통과했음

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // 결과전송

			// UUID를 클라이언트에 넘겨준다.
			JAPAN_NET_LOGIN_UUID netMsg;

			StringCchCopy( netMsg.szUUID, sc::string::UUID_LENGTH, m_pClientManager->JPGetUUID( dwClient ) ); 
			SendClient(dwClient, &netMsg );	

			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // 로그인성공이지만 이미 오프라인일때...
			// 로그아웃 시킨다.
			m_pDbManager->UserLogoutSimple(nlfd2->JapanUserID);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->JapanUserID));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송
			return;
		}		
		else 
		{
            // 로그인성공, 온라인이지만 ip, id 가 틀릴때...
			// 기존사용자는 로그아웃 시키고
			m_pDbManager->UserLogoutSimple (nlfd2->JapanUserID);			

			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->JapanUserID));

			// 현재사용자는 대기시킨다.
			// 현재 사용자가 로그인 요청중일 가능성이 있다.
			return;
		}
	}
	else // 로그인 실패일때
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pClientManager->SetAccountPassing(dwClient, false);

		// 로그인 실패이고 온라인이고 IP, ID 일치하거나 중복된 로그인일 경우
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->JapanUserID) == 0) )
		{
            if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_DUP)
            {
                bool bFind = FALSE;
                for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
                {
					if (!m_pClientManager->IsClientSlot(i))
						continue;

                    // 온라인 상태인경우
                    int nUserDbNum = m_pClientManager->UserDbNum(i);
                    if (nUserDbNum == nlfd2->nUserNum)
                    {
                        if (i != dwClient)
                        {
                            GLMSG::SNET_DUPLICATE_CLOSEGAME NetMsg;
                            SendClient(i, &NetMsg);
                            bFind = TRUE;
                            sc::writeLogError(
                                sc::string::format(
                                    "MsgLoginBack Duplication Login. UserID %1% UserDbNum %2% Slot %3%",
                                    nlfd2->JapanUserID,
                                    nUserDbNum,
                                    dwClient));
                            break;
                        }
                    }
                }

                if (!bFind)
                {
                    m_pDbManager->UserLogoutSimple3(nlfd2->JapanUserID, m_nServerGroup);
                    sc::writeLogError(
                        sc::string::format(
                            "MsgLoginBack Duplication Login. UserLogoutSimple3 UsesrID %1% Server %2% Slot %3%",
                            nlfd2->JapanUserID,
                            m_nServerGroup,
                            dwClient));
                }
            }

            nlfd.m_Result = nlfd2->m_emResult;
            SendClient(dwClient, &nlfd);
            return;
		}
		// 로그인 실패이고 이미 오프라인일때...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->JapanUserID ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // 결과전송

			return;
		}
		else // 로그인 실패이고 온라인이지만 IP, ID 가 일치하지 않을때
		{
			// 현재사용자는 대기
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->JapanUserID ));
			return;
		}
	}
}


/**
 * Excite Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::ExciteMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	EXCITE_NET_PASSCHECK_DATA* pPassCheck = (EXCITE_NET_PASSCHECK_DATA*)nmg;

	if (pPassCheck == NULL) return;

	TCHAR szExciteUserID[EXCITE_SEC_ID+1];
	TCHAR szUserPass[EXCITE_SEC_PASS+1];

	int nCheckFlag = pPassCheck->nCheckFlag;

	StringCchCopy( szExciteUserID, EXCITE_SEC_ID+1, pPassCheck->szExciteUserID );
	StringCchCopy( szUserPass, EXCITE_SEC_PASS+1, pPassCheck->szUserPass );	

	// UserID를 이용해서 사용자 패스워드를 체크한다.
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentExcitePassCheck(
                m_pClientManager->GetUserID(dwClient),
                szExciteUserID,
                szUserPass,
                nCheckFlag,
                0,
                dwClient)));
}

/**
 * Excite Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::ExciteMsgPassCheckBack(EXCITE_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// Use Features 동기화;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
		sc::writeLogError( sc::string::format("ExciteMsgPassCheckBack User Already Offline(%s)", nlfd2->szExciteUserID ));
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("ExciteMsgPassCheckBack Login Error(%s)", nlfd2->szExciteUserID ));
		return;
	}
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_SECID_ALREADY )	// 아뒤 중복
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;	
	}	

	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;
	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("ExciteMsgPassCheckBack Login Fail(%s)", nlfd2->szExciteUserID ));
		return;	
	}
}



/**
 * Terra Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::TerraMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//	Memo :	말레이지아, 한국에서 사용된다.
	//if (m_nServiceProvider != SP_TERRA) return;

	if (nmg == NULL) return;

	TERRA_NET_PASSCHECK_DATA* pPassCheck = (TERRA_NET_PASSCHECK_DATA*)nmg;

	if (pPassCheck == NULL) return;

	TCHAR szTLoginName[TERRA_TLOGIN_NAME+1];
	TCHAR szUserPass[TERRA_USERPASS+1];

	int nCheckFlag = pPassCheck->nCheckFlag;

	StringCchCopy( szTLoginName, TERRA_TLOGIN_NAME+1, pPassCheck->szTLoginName );
	StringCchCopy( szUserPass, TERRA_USERPASS+1, pPassCheck->szUserPass );	

	// TLoginName을 이용해서 사용자 패스워드를 체크한다.
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentTerraPassCheck(
                szTLoginName,
                szUserPass,
                nCheckFlag,
                0,
                dwClient)));
}

/**
 * Terra Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::TerraMsgPassCheckBack(TERRA_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;		

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// Use Features 동기화;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
		sc::writeLogError( sc::string::format("TerraMsgPassCheckBack User Already Offline(%s)", nlfd2->szTerraTLoginName ));
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("TerraMsgPassCheckBack Login Error(%s)", nlfd2->szTerraTLoginName ));
		return;
	}
	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("TerraMsgPassCheckBack Login Fail(%s)", nlfd2->szTerraTLoginName ));
		return;	
	}
}



/**
 * Daum Password Check message
 * \param pMsg Password Check message
 */
void CAgentServer::DaumMsgPassCheck(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	//	Memo :	말레이지아, 한국에서 사용된다.
	if (m_ServiceProvider != SP_KOREA) return;

	if (nmg == NULL) return;

	DAUM_NET_PASSCHECK_DATA* pPassCheck = (DAUM_NET_PASSCHECK_DATA*) nmg;

	if (pPassCheck == NULL) return;

	// TLoginName을 이용해서 사용자 패스워드를 체크한다.
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new CAgentDaumPassCheck(
                pPassCheck->szDaumGID,
                pPassCheck->szUserPass,
                pPassCheck->nCheckFlag,
                0,
                dwClient)));
}

/**
 * Daum Password Check message
 * \param nlfd2 Password Check Feedback data
 */
void CAgentServer::DaumMsgPassCheckBack(DAUM_NET_PASSCHECK_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_PASSCHECK_FEEDBACK_DATA	nlfd;
	
	DWORD dwClient = static_cast<DWORD> (nlfd2->nClient);

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // 로그인 성공일때
	{
		// Use Features 동기화;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// 이미 오프라인
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // 인증통과 실패
		m_pDbManager->UserLogoutSimple(m_pClientManager->GetUserID(dwClient));
		sc::writeLogError( sc::string::format("DaumMsgPassCheckBack User Already Offline(%s)", nlfd2->szDaumGID ));
		return;
	}
	// 로그인 실패
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("DaumMsgPassCheckBack Login Error(%s)", nlfd2->szDaumGID ));
		return;
	}
	// 초기 입력 성공
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // 로그인 실패일때
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("DaumMsgPassCheckBack Login Fail(%s)", nlfd2->szDaumGID ));
		return;	
	}
}

BOOL CAgentServer::GmHTTPAuth(CString &strID, CString &strPW, std::string &strUGID, int &nBlock)
{
	BOOL bRet = FALSE;
	CURL *curl;
	std::vector<char> byBuffer;
	std::string strResponse;
	struct curl_slist *headers = NULL;
	std::string strName;
	std::string strResult;

	//curl_global_init( CURL_GLOBAL_ALL ) ;

	curl = curl_easy_init();

	if ( curl )
	{
		curl_easy_setopt(curl, CURLOPT_URL,"http://93.90.178.90/ran/login/");
		curl_easy_setopt(curl, CURLOPT_USERPWD , "ranonline:soCv3XdJ9q6N2JT");
		
		curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, wrapper_curl_write_callback);
		curl_easy_setopt( curl, CURLOPT_WRITEDATA, &byBuffer );

		curl_easy_setopt( curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
		curl_easy_setopt( curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);     

        curl_easy_setopt( curl , CURLOPT_POST , 1 ) ;

		std::string postData = sc::string::format("username=%1%&password=%2%", strID.GetString(), strPW.GetString() );// "username=testuser@gmail.com&password=testpass";
        curl_easy_setopt( curl , CURLOPT_POSTFIELDS , postData.c_str() ) ;

		CURLcode ccOK = curl_easy_perform(curl);

		if ( ccOK == CURLE_OK )
		{
			byBuffer.push_back('\0');

			//long statLong;
			//double statDouble;        
			//char* statString = NULL;

			//// HTTP 응답코드를 얻어온다. 
			//if ( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_HTTP_CODE , &statLong) )
			//{	
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>> Response code (%1%).", statLong));	
			//}
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Response code (%1%).", statLong));	
			//}

			//// Content-Type 를 얻어온다.
			//if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			//{
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>> Content type: %1%", statString));
			//}
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Content type (%1%).", statString));	
			//}

			//// 다운로드한 문서의 크기를 얻어온다.
			//if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SIZE_DOWNLOAD , &statDouble ) )
			//{
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>> Download size: %1%bytes", statDouble));
			//}
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Download size: (%1%).", statDouble));	
			//}

			//// 
			//if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_SPEED_DOWNLOAD , &statDouble ) )
			//{
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>>  Download speed: %1%bytes/sec", statDouble));
			//}			
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Download speed: %1%bytes/sec.", statDouble));	
			//}


			strResponse = std::string(&byBuffer[0]);
			//sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>>  %1% ", strResponse));

			json_spirit::mValue outputValue;

			if( !json_spirit::read( strResponse, outputValue ) )
				return FALSE;

			const json_spirit::mObject outputObj = outputValue.get_obj();
			
			bRet = TRUE;

			if( !sc::Json::find_value( outputObj, "ugID", strUGID ) )
			{
				// ERROR
				sc::writeLogError(std::string("json ERROR Cannot find ID"));
				bRet = FALSE;
			}
			if( !sc::Json::find_value( outputObj, "name", strName ) )
			{
				// ERROR
				sc::writeLogError(std::string("json ERROR Cannot find NAME"));	
				bRet = FALSE;
			}

			nBlock = 0; // blcok 값이 bool 값에서 int 로 변경하기로 했는데 현지에서 수정하면 다시 적용해야함
			//if( !sc::Json::find_value( outputObj, "block", nBlock ) )
			//{
			//	// ERROR
			//	sc::writeLogError(std::string("json ERROR Cannot find BLOCK"));				
			//	bRet = FALSE;
			//}
			if( sc::Json::find_value( outputObj, "error", strResult ) )
			{
				sc::writeLogError(std::string("json ERROR Recv Fail Value"));		
				bRet = FALSE;
			}
		}
		else
		{
			// ERROR
			//sc::writeLogError(std::string(">>>>>>>>>>>>>>> EU 인증 실패"));			
			bRet = FALSE;
		}
	}
	else
	{
		// ERROR : curl == NULL 인 경우 
		//sc::writeLogError(std::string(">>>>>>>>>>>>>>> EU 초기화 실패 curl == NULL"));			
	}

	curl_easy_cleanup( curl ) ;
	curl_global_cleanup() ;

	sc::writeLogInfo(sc::string::format("EU GameMasters Web Login User Information : ugID [%1%], name [%2%], block [%3%]", strUGID, strName, nBlock));

	return bRet;
}

void CAgentServer::MsgCloseClient(DWORD ClientSlot, DWORD GaeaID, NET_MSG_GENERIC* pMsg)
{
    if ( m_pClientManager->IsAgentFieldSlot(ClientSlot) || ClientSlot == DBACTION_CLIENT_ID )
    {
        NET_CLOSE_CLIENT* pNetMsg = (NET_CLOSE_CLIENT*)pMsg;
        CloseClient(m_pClientManager->GetClientID(pNetMsg->dwCloseCharNum), net::UNKNOWN_SVR);
    }
}
