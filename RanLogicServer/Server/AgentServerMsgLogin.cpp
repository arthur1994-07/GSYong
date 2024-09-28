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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing (dwClient) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing (dwClient, true);
	}

	THAI_NET_LOGIN_DATA* tnld = ( THAI_NET_LOGIN_DATA* ) nmg;
	DWORD dwFlags    = 0;
	DWORD dwSndBytes = 0;

	/// Tea �˰������� ��ü
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
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = tnld->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL; // ä���� Full �̶�� �޽����� ������.
		SendClient (dwClient, &nlfd); // �������
//		CloseClient (dwClient); // ��������		

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));

		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull (nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA	nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient (dwClient, &nlfd); // �������
//		CloseClient (dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL",	nChannel) );
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel (dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser (nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel (dwClient, nChannel);
		}
		else // ����������
		{
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID (dwClient, tnld->szUserid);		
	
	// ������� ���� ����
	m_pClientManager->SetAccountPass (dwClient, false); 

	// DB �� ������û
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

    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ( (m_pClientManager->IsOnline(dwClient) == true) &&
			 (m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			 (strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0))
		{
			// ������ ��ȯ���̸� ����ó��
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "ThaiMsgLogInBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      ( dwClient, nlfd2->nUserNum );       // ������ȣ
			m_pClientManager->SetLoginTime    ( dwClient );                        // �α��� �ð�
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // ä�� ��� �Ⱓ
			m_pClientManager->ThaiSetFlag     ( dwClient, nlfd2->wThaiFlag );      // Thailand ����� �÷���
			m_pClientManager->SetIPBonusClass ( dwClient, nlfd2->emIPBonus );  // Thailand ����� Class Type

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			// 18 �� �����϶��� ���� ������ �÷��� �ð�(��)�� �����;� �Ѵ�.
			if ( nlfd2->wThaiFlag == THAI_UNDER18 )
			{
				// ���� �÷����� �ð� ��������
				WORD wTodayGameTime = m_pDbManager->ThaiGetTodayPlayTime( nlfd2->nUserNum );
				// ���� �����÷��� �ð�(��) ����
				m_pClientManager->ThaiSetTodayPlayTime( dwClient, wTodayGameTime );
			}

			m_pClientManager->SetAccountPass( dwClient, true ); // �����������

			// APEX ���� ����
			if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
			{
				APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

				std::string IpAddress = m_pClientManager->GetClientIP(dwClient);
				int nIp  = inet_addr(IpAddress.c_str());

				APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			}

			SendClient( dwClient, &nlfd ); // �������
			return;
		}
		// �α��μ��������� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->szUserid));
	
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong(%s)", nlfd2->szUserid ));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass (dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline (dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp (m_pClientManager->GetUserID (dwClient), nlfd2->szUserid) == 0))
		{
			// �±��� �α��ι���� �ٸ�������� �ٸ���. �ٸ� �������� �ߺ������̶�� ������ ����ڸ� �����Ű��, 
			// �±��� �����Ű�� �ʴ´�. �׷� ���� ���� ���� ��� ����ڵ��� ���� ���ӿ� �����ϰ� ����������
			// �α׾ƿ� ó���� ���������� �����ʾƼ� GM���� �������� ó���� �ϰ� �ִ�.
			// ���� ��Ȯ�� ������ �ľ��� �ȵǰ� �ִ�. �α��� ó���� �� �� DB���� ������� �ߺ������̶�� ����� ���� ��
			// ������ ���� �� ������� ĳ���Ͱ� �������� �ʴٸ� ���� ������ ���ܼ� ���� ������̴�.
			// �α׾ƿ�ó���� ���ش�. ������, ��Ȯ�� ������ ã�ƾ� �Ѵ�. �׷��� ������ ������ ����� ����ڵ���
			// �α׾ƿ� �ð� ��� ó���� �����ʴ´�.
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline (dwClient) == false)
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));
		
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			sc::writeLogError (sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}
}

//! China �α��� �޽���
void CAgentServer::ChinaMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	

	// �߱� Ŭ���̾�Ʈ�� ���� �޽��� ���� Ȯ���Ѵ�.
	if (m_ServiceProvider != SP_CHINA || nmg == NULL) return;	

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing( dwClient ) == true)
	{
		return;
	}
	else // ���������� ����
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

	// ��ȣȭ
	m_Tea.decrypt( pNml->szUserid, USR_ID_LENGTH+1 );
	m_Tea.decrypt( pNml->szPassword, USR_PASS_LENGTH );
	m_Tea.decrypt( pNml->szRandomPassword, USR_RAND_PASS_LENGTH+1 );

	TCHAR szRandomPassword[USR_RAND_PASS_LENGTH+1] = {0};
	TCHAR szPassword[USR_PASS_LENGTH] = {0};
	TCHAR szUserid[USR_ID_LENGTH+1] = {0};

	StringCchCopy( szUserid, USR_ID_LENGTH+1, pNml->szUserid );
	StringCchCopy( szPassword, USR_PASS_LENGTH, pNml->szPassword );
	StringCchCopy( szRandomPassword, USR_RAND_PASS_LENGTH+1, pNml->szRandomPassword );
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������

		sc::writeLogError(sc::string::format("ChinaMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull( nChannel ) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient, nChannel );
		}
		else // ����������
		{
			
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB �� ������û
	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ���� ����
	
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

    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ),   nlfd2->szUserid ) == 0))
		{
			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      ( dwClient, nlfd2->nUserNum );       // ������ȣ
			m_pClientManager->SetLoginTime    ( dwClient );                        // �α��� �ð�
			m_pClientManager->SetUserType     ( dwClient, nlfd2->nUserType );      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  ( dwClient, nlfd2->uChaRemain );     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain( dwClient, nlfd2->uChaTestRemain ); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  ( dwClient, nlfd2->tPremiumTime );   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate( dwClient, nlfd2->tChatBlockTime ); // ä�� ��� �Ⱓ

			m_pClientManager->SetChinaGameTime( dwClient, nlfd2->nChinaGameTime ); // �߱� ���ӽð� ����
			m_pClientManager->SetChinaOfflineTime( dwClient, nlfd2->nChinaOfflineTime ); // �߱� �������� �ð� ����
			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // �߱� LastLoginDate ����
			m_pClientManager->SetChinaUserAge( dwClient, nlfd2->nChinaUserAge ); // �߱� UserAge ����

			m_pClientManager->SetAccountPass( dwClient, true ); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient( dwClient, &nlfd ); // �������

			// Apex ����
			APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

//			char* szIp = m_pClientManager->GetClientIP( dwClient );
//			int nIp  = inet_addr(szIp);
			
//			APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
			
			return;
		}		
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
            // �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
            sc::writeLogWarn(sc::string::format("ChinaMsgLoginBack User Already Offline %1%", nlfd2->szUserid));
			return;
		}		
		else 
		{
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����
		m_pClientManager->SetAccountPassing( dwClient, false );

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline( dwClient ) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GetUserID( dwClient ), nlfd2->szUserid ) == 0))
		{
			nlfd.m_Result = nlfd2->m_emResult;
			SendClient( dwClient, &nlfd );
			return;
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline( dwClient ) == false)
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}
}

/**
* �Ϲ� �α��� : id / pwd �Է½�
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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

    // ���� üũ ���Ŀ� �ۼ�
    //pPacket->m_LauncherVersion;
    //pPacket->m_GameVersion;

	/// ��ȣȭ �˰��� ��ü
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		sc::writeLogError(
            sc::string::format(
                "MsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,",
                pPacket->m_szUserid, szEncrypt, m_szEncrypt ));

		return;
	}	

	// �����Ϸ��� ä�� �˻�
	int nChannel = pPacket->m_nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // ��������

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

//		CloseClient(dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}
/*
	// WhiteRock���� ��� �ϴ� �κ��� Thread ó��
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

	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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
	//�̹� �������̴�
	//
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else //���������� ����
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

		//CloseClient(dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel));
		return;
	}
	else
	{
		// �ű�������
		if (m_pClientManager->GetChannel(dwClient) == -1)
		{
			IncreaseChannelUser(nChannel);
			
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			;			
		}
	}

	std::string strId(msg->m_szUserid);
	std::string strPass(msg->m_szPassword);


	//int nUserNum = IdnAuthVTC(strId /*pain*/, strPass /*encryped by md5*/);


	m_pClientManager->SetUserID(dwClient, strId.c_str());
	
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
	
    m_pDbManager->AddUserJob(
        db::DbActionPtr(
            new DIdnUserCheck(strId, strPass, m_pClientManager->GetClientIP(dwClient), m_nServerGroup,
						m_nServerNum, 0, dwClient, static_cast<DWORD>(m_ServiceProvider))
						));

}

//
//mjeon.indonesia.�Ϲ����� ID/Password�� ���� ����
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

		// HTTP �����ڵ带 ���´�. 
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
			// Content-Type �� ���´�.
			if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			{
				sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC() - Content type: %1%", statString));
			}
		}

        // �ٿ�ε��� ������ ũ�⸦ ���´�.
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
//mjeon.indonesia.ID/2��Password�� ���� ���� (ĳ���� ������)
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

		// HTTP �����ڵ带 ���´�. 
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
			// Content-Type �� ���´�.
			if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			{
				sc::writeLogInfo(sc::string::format("CAgentServer::IdnAuthVTC_2ndPass() - Content type: %1%", statString));
			}
		}

        // �ٿ�ε��� ������ ũ�⸦ ���´�.
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

	// ���弭���� �����ְų� ������� �ʴ� ��쿡�� ������ �α��� ����
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
	// WB �����ΰ�쿡�� ����
	// WB ������Ʈ�� �ش� ������ �����ϴ��� Ȯ��
	msg.bLoginWB = false;

	for (DWORD i=1; i<(DWORD)m_nMaxClient; ++i)
	{
		if (!m_pClientManager->IsClientSlot(i))
			continue;
		
		// �¶��� �����ΰ��
		if( strcmp(m_pClientManager->GetUserID(i), pNml->szUserid) == 0 )
		{
			msg.bLoginWB = true;
			break;
		}
	}
	/////////////////////////////////////

	SendClient( dwClient, &msg ); // �������
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

	SendClient( pNml->nClientID, &msg ); // �������
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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		sc::writeLogError(sc::string::format("GsMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ));

		return;
	}
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // ��������

		sc::writeLogError(sc::string::format("GsMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

//		CloseClient(dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);

			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}

	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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


    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS)
	{

		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// ������ ��ȯ���̸� ����ó��
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "GsMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// ���� �ý��� Ŭ���̾�Ʈ�� ����ȭ;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			
			m_pClientManager->SetCountryInfo  (dwClient, nlfd2->sCountryInfo);   // ��� �� ���� ����;

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("GsMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else 
		{
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("GsMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

                    // �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("GsMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

	// �̹� ��������...
	if ( m_pClientManager->IsAccountPassing( dwClient ) == true )
	{
		return;
	}
	else // ���������� ����
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
		SendClient( dwClient, &nlfd ); // �������
		m_pClientManager->SetAccountPassing( dwClient, false ); // ��������� ���� false
		sc::writeLogError( sc::string::format( "GmMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ) );
		return;
	}

	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if ( nChannel < 0 || nChannel >= m_nServerChannelNumber )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // ��������
		sc::writeLogError( sc::string::format( "GmMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ) );
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if ( IsChannelFull( nChannel ) == true )
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
		//CloseClient( dwClient ); // ��������
		sc::writeLogWarn( sc::string::format( "CH(%d) is FULL", nChannel ) );
		return;
	}
	else
	{
		if ( m_pClientManager->GetChannel( dwClient ) == -1 ) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );

			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient,nChannel );
		}
		else // ����������
		{

		}
	}

	// ����� id ����
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB �� ������û
	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ���� ����

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

	// EU�� ��� GM�� ���̵�� �α��� �ϱ� ������ �������־�� �Ѵ�.
	if (m_ServiceProvider == SP_EU)
		m_pClientManager->SetUserID(dwClient, nlfd2->szUserid);

	// �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{

		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogWarn(sc::string::format("GmMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else 
		{
			// �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("GmMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("GmMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

	// �̹� ��������...
	if ( m_pClientManager->IsAccountPassing( dwClient ) == true )
	{
		return;
	}
	else // ���������� ����
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
		SendClient( dwClient, &nlfd ); // �������
		m_pClientManager->SetAccountPassing( dwClient, false ); // ��������� ���� false
		sc::writeLogError( sc::string::format( "UsMsgLogIn Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ) );
		return;
	}

	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if ( nChannel < 0 || nChannel >= m_nServerChannelNumber )
	{
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // ��������
		sc::writeLogError( sc::string::format( "UsMsgLogIn Invalid Channel Number (Channel:%d)", nChannel ) );
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if ( IsChannelFull( nChannel ) == true )
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
		//CloseClient( dwClient ); // ��������
		sc::writeLogWarn( sc::string::format( "CH(%d) is FULL", nChannel ) );
		return;
	}
	else
	{
		if ( m_pClientManager->GetChannel( dwClient ) == -1 ) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );

			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient,nChannel );
		}
		else // ����������
		{

		}
	}

	// ����� id ����
	m_pClientManager->SetUserID( dwClient, szUserid );
	// DB �� ������û
	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ���� ����

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


	// �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{

		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// ������ ��ȯ���̸� ����ó��
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "UsMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			// �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogWarn(sc::string::format("UsMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else 
		{
			// �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);

			sc::writeLogError(sc::string::format("UsMsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("UsMsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
			sc::writeLogError(sc::string::format("UsMsgLoginBack User Login Fail And ID/PWD Wrong(%s)", nlfd2->szUserid ));
			return;
		}
	}

}

/*
* �Ϲ� �α��� ��� : id / pwd �Է½�
*/
void CAgentServer::MsgLogInBack (NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->m_ClientSlot;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || nlfd2->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS )
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->szUserid) == 0)  )
		{
			// ������ ��ȯ���̸� ����ó��
			if ( 2 == nlfd2->ItemTransferResult )
			{
				nlfd.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &nlfd );
				m_pDbManager->UserLogoutSimple( nlfd2->szUserid );
				sc::writeLogError( sc::string::format( "MsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", nlfd2->szUserid ) );
				return;
			}

			// ���� �ý��� Ŭ���̾�Ʈ�� ����ȭ;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			
			m_pClientManager->SetCountryInfo  (dwClient, nlfd2->sCountryInfo);	 // ��� �� ���� ����;

			// ���� �׽�Ʈ�� ���ؼ� �����̽þ� User Class Type�� �߰��Ѵ�.
			//	IP���ʽ�
/*
#if defined(_RELEASED)			
			m_pClientManager->SetIPBonusClass	  ( dwClient, nlfd2->emIPBonus ); // �׽�Ʈ��
#endif
*/			

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // �߱�, ��Ʈ�� LastLoginDate ����
//#if defined( VN_PARAM ) || defined(_RELEASED) //vietnamtest%%%
			if (m_ServiceProvider == SP_VIETNAM)
			{
				m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // ��Ʈ�� ���ӽð� ����
			}
//#endif

			
			//
			//mjeon.ToDo
			//m_pClientManager->SetAccountPass(dwClient, true); is doubly called.
			//It was already called on 1054th line in this src file.
			//Remove the following code line.
			//			
			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			// �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.            
            // �ذ��� �߰�
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
			nlfd.m_CheckFlag = nlfd2->nCheckFlag; // �ѱ� CheckFlag Test�� ���ؼ� �߰�

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

			// APEX ���� ����
            if (m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
            {
			    APEX_SERVER::NoticeApexProxy_UserLogin( dwClient, nlfd2->szUserid );

                std::string IpAddress = m_pClientManager->GetClientIP(dwClient);
			    int nIp  = inet_addr(IpAddress.c_str());

			    APEX_SERVER::NoticeApexProxy_UserIP( dwClient, nIp );
            }
			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
            return;
		}		
		else 
		{
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
			// m_pDbManager->UserLogoutSimple2(nlfd2->nUserNum);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else 
	{
        // �α��� �����϶�
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

                    // �¶��� �����ΰ��
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
			// ����Ű �߰�%%%
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->szUserid ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		//
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		//
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szUserid) == 0)  )
		{
			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum    (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ			

			//
			// ���� �׽�Ʈ�� ���ؼ� �����̽þ� User Class Type�� �߰��Ѵ�.
			//	IP���ʽ�
/*
#if defined(_RELEASED)			
			m_pClientManager->SetIPBonusClass	  ( dwClient, nlfd2->emIPBonus ); // �׽�Ʈ��
#endif
*/			

			m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // �߱�, ��Ʈ�� LastLoginDate ����

			
			//
			//mjeon.ToDo
			//m_pClientManager->SetAccountPass(dwClient, true); is doubly called.
			//It was already called on 1054th line in this src file.
			//Remove the following code line.
			//
			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
   			nlfd.m_CheckFlag = nlfd2->nCheckFlag;	// �ѱ� CheckFlag Test�� ���ؼ� �߰�
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
            
			SendClient(dwClient, &nlfd); // �������

			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->szUserid);
            sc::writeLogWarn(sc::string::format("IdnMsgLoginBack User Already Offline %1%", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else 
		{
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple (nlfd2->szUserid);
			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->szUserid));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

                    // �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		//
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("IdnMsgLoginBack() - Login failed and the client is already offline(%s)", nlfd2->szUserid));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;

			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	DAUM_NET_LOGIN_DATA* pLogin = ( DAUM_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		sc::writeLogError(sc::string::format("DaumMsgLogin Invalid Channel Number (Channel:%d)", nChannel) );
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}

	// UUID Check
	// UUID copy
	TCHAR szUUID[sc::string::UUID_LENGTH] = {0};
	StringCchCopy( szUUID, sc::string::UUID_LENGTH, pLogin->szUUID);

	// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� UUID �� �����Ѵ�.
	// ��ŷ�� ���ؼ� �ٸ� UUID �ΰ�ó�� ���� ���ɼ��� �ִ�.
	m_pClientManager->KorSetUUID( dwClient, szUUID );

	// UUID�� �̿��ؼ� ����� ������ üũ�Ѵ�.
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
// ���� : �α��� ���
void CAgentServer::DaumMsgLoginBack(DAUM_NET_LOGIN_FEEDBACK_DATA2* pMsg)
{
	if (!pMsg)
        return;

	NET_LOGIN_FEEDBACK_DATA	MsgAC;
	
	//MsgAC.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (pMsg->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.
	MsgAC.nType           = NET_MSG_LOGIN_FB;
	MsgAC.m_GameVersion = m_PatchVersion;
	MsgAC.m_LauncherVersion= m_LauncherVersion;

	m_pClientManager->SetUserID(dwClient, pMsg->szDaumGID);    

	if (pMsg->m_emResult == EM_LOGIN_FB_SUB_OK || pMsg->m_emResult == EM_LOGIN_FB_KR_OK_USE_PASS || pMsg->m_emResult == EM_LOGIN_FB_KR_OK_NEW_PASS ) // �α��� �����϶�
	{
		// �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == pMsg->szIp) &&
			(strcmp(m_pClientManager->KorGetUUID(dwClient), pMsg->szUUID) == 0))
		{
			// ������ ��ȯ���̸� ����ó��
			if ( 2 == pMsg->ItemTransferResult )
			{
				MsgAC.m_Result = EM_LOGIN_FB_ITEM_TRANSFER;
				SendClient( dwClient, &MsgAC );
				m_pDbManager->UserLogoutSimple( pMsg->szDaumGID );
				sc::writeLogError( sc::string::format( "DaumMsgLoginBack EM_LOGIN_FB_ITEM_TRANSFER %1%", pMsg->szDaumGID ) );
				return;
			}

			// ���� �ý��� Ŭ���̾�Ʈ�� ����ȭ;
			if( m_pGLAgentServer )
				m_pGLAgentServer->CountrySystemSyncState( dwClient );

			// Use Features ����ȭ;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����            

			// �ѱ� �˴ٿ��� üũ
			if ((m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_KOREA || m_ServiceProvider == SP_KOREA_TEST) &&
				m_pConfig->GetKorShutdown())
			{
				// �˴ٿ��� 16��
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

			
			// Daum �н����带 �ޱ����� Check Flag ��
			MsgAC.m_CheckFlag = pMsg->nCheckFlag;
			MsgAC.m_sCountryInfo = pMsg->sCountryInfo;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum    (dwClient, pMsg->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, pMsg->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, pMsg->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, pMsg->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, pMsg->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, pMsg->tChatBlockTime); // ä�� ��� �Ⱓ
            m_pClientManager->SetAge          (dwClient, pMsg->m_wAge);         // ����
			m_pClientManager->SetCountryInfo( dwClient, pMsg->sCountryInfo);
/*
            sc::writeLogInfo(
                sc::string::format(
                    "CAgentServer::DaumMsgLoginBack Age %1%", pMsg->m_wAge));
*/

            // �ذ��� ��/�� ���� ���� ���ڸ� �Ѱ��ش�.
            // �ذ��� �߰�
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
            // �̹� ��������
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
			m_pDbManager->UserLogoutSimple(pMsg->szDaumGID);
			sc::writeLogWarn(sc::string::format("DaumMsgLoginBack User Already Offline %1%", pMsg->szDaumGID));
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(pMsg->szDaumGID);
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User IP/ID Wrong(%s)", pMsg->szDaumGID ));
			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false); // ������ false

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
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

					// �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User Offline AND ID/PWD Wrong(%s)", pMsg->szDaumGID ));
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			sc::writeLogError(sc::string::format("DaumMsgLoginBack User Online ID/PWD Wrong(%s)", pMsg->szDaumGID ));
			// �������ڴ� ���
			return;
		}
	}
}

void CAgentServer::GspMsgLogin( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (m_ServiceProvider != SP_GLOBAL) return;

	if (nmg == NULL) return;
	
	m_pClientManager->IncLoginAttempt( dwClient );

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient( dwClient, net::KICKOUT_UNDEFINE ); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing( dwClient ) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing( dwClient, true );
	}	

	GSP_NET_LOGIN_DATA* pLogin = ( GSP_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		sc::writeLogError(sc::string::format("GspMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull( nChannel ) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient( dwClient, &nlfd ); // �������
//		CloseClient( dwClient ); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel( dwClient ) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser( nChannel );
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel( dwClient, nChannel );
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}	

	// UUID Check
	// UUID copy	
	TCHAR szUUID[sc::string::UUID_LENGTH] = {0};
	StringCchCopy( szUUID, sc::string::UUID_LENGTH, pLogin->szUUID);

	// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� UUID �� �����Ѵ�.
	// ��ŷ�� ���ؼ� �ٸ� UUID �ΰ�ó�� ���� ���ɼ��� �ִ�.
	m_pClientManager->GspSetUUID( dwClient, szUUID );

	// UUID�� �̿��ؼ� ����� ������ üũ�Ѵ�.
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

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	// GspUserID
	m_pClientManager->GspSetUserID( dwClient, nlfd2->szGspUserID );

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, Decoded TID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0) )
		{
			// Use Features ����ȭ;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �̹� ��������
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
			m_pDbManager->UserLogoutSimple(nlfd2->szGspUserID);
			sc::writeLogWarn(sc::string::format("GspMsgLoginBack User Already Offline %1%", nlfd2->szGspUserID));
			return;
		}		
		else 
		{
            // �α��μ���, �¶��������� ip, DecodedTID Ʋ����...
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szGspUserID);
			sc::writeLogError(sc::string::format("GspMsgLoginBack User IP/UUID Wrong %1%/%2%", nlfd2->szGspUserID, nlfd2->szUUID));

			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass( dwClient, false ); // ������� ����
		m_pClientManager->SetAccountPassing( dwClient, false );

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP( dwClient ) == nlfd2->szIp) &&
			(strcmp( m_pClientManager->GspGetUUID( dwClient ), nlfd2->szUUID) == 0))
		{
			nlfd.m_Result = nlfd2->m_emResult;
			SendClient( dwClient, &nlfd );
		}
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("GspMsgLoginBack User Offline AND Login Fail(%s)", nlfd2->szGspUserID ));
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			sc::writeLogError(sc::string::format("GspMsgLoginBack User Online Wrong(%s)", nlfd2->szGspUserID ));
			// �������ڴ� ���
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

	// �α��� �õ�Ƚ���� 5ȸ �̻��̸� ������ �����Ѵ�.
	if (m_pClientManager->GetLoginAttempt(dwClient) > 5)
	{
		sc::writeLogWarn(
            sc::string::format(
                "User Login Attempt is over 5 times %1%",
                m_pClientManager->GetClientIP(dwClient)));
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // ��������		
		return;
	}

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	CString strTID;

	TERRA_NET_LOGIN_DATA* pLogin = ( TERRA_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;	
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		sc::writeLogError(sc::string::format("TerraMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
		sc::writeLogWarn( sc::string::format("CH(%d) is FULL", nChannel ));

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
		{
		}
	}

	char szDecodedTID[TERRA_TID_DECODE+1] = {0}; // Decoded TID

	// Encoding �� ���ڿ��� Decoding �Ѵ�.
	if (TerraGetDecodedTID(pLogin->szTID, szDecodedTID) == NET_ERROR) 
	{ 		
		// Decoding ���н� �ڵ�
		NET_LOGIN_FEEDBACK_DATA nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_SUB_FAIL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // ��������
		sc::writeLogError(sc::string::format("TerraGetDecodedTID %s", pLogin->szTID ));
		return;
	}
	else
	{
		// �α��� ������ ������ ������ üũ�ϱ� ���ؼ� TID �� �����Ѵ�.
		m_pClientManager->TerraSetDecodedTID(dwClient, szDecodedTID);

		// Decoding �� TID �� �̿��ؼ� ����� ������ üũ�Ѵ�.
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
* \param nlfd2 �α��� Feedback data
*/
void CAgentServer::TerraMsgLoginBack(TERRA_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	if (nlfd2 == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	
	//nlfd.nType = NET_MSG_LOGIN_FB;

	DWORD	dwClient   = static_cast<DWORD> (nlfd2->nClient);
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;	

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.	
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	// TLoginName 
	m_pClientManager->SetUserID(dwClient, nlfd2->szTerraTLoginName);

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, Decoded TID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->TerraGetDecodedTID(dwClient), nlfd2->szTerraDecodedTID) == 0))
		{
			// Use Features ����ȭ;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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

			// ���������� �н����带 �ޱ����� Check Flag ��
			nlfd.m_CheckFlag = nlfd2->nCheckFlag;

			m_pClientManager->SetAccountPass  (dwClient, true);
            m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);
			m_pClientManager->SetLoginTime    (dwClient);
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ
//	IP���ʽ�
//#if	defined( MYE_PARAM ) || defined( MY_PARAM ) || defined ( PH_PARAM ) 
			if (m_ServiceProvider == SP_MALAYSIA || m_ServiceProvider == SP_MALAYSIA_EN || m_ServiceProvider == SP_PHILIPPINES)
			{
				m_pClientManager->SetIPBonusClass ( dwClient, nlfd2->emIPBonus );  // �����̽þ� ����� Class Type
			}
//#endif

			// �ذ��� �߰�
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
				m_pClientManager->SetVTGameTime( dwClient, nlfd2->nVTGameTime ); // ��Ʈ�� ���ӽð� ����
				m_pClientManager->SetLastLoginDate( dwClient, nlfd2->tLastLoginTime ); // ��Ʈ�� LastLoginDate ����
			}
//#endif
			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �̹� ��������
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
			sc::writeLogWarn(sc::string::format("TerraMsgLoginBack User Already Offline %1%", nlfd2->szTerraTLoginName));

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �α��� ���� ����
	
			return;
		}
		// �α��μ���, �¶��������� ip, DecodedTID Ʋ����...
		else 
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User IP/DecodedTID Wrong(%s)", nlfd2->szTerraTLoginName ));

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			CloseClient( dwClient, net::KICKOUT_UNDEFINE );

			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ
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

                    // �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User Offline AND Login Fail(%s)", nlfd2->szTerraTLoginName ));

			// ������� ���н� �ο����� ID���� ���� CloseClient�� ȣ�⿩�ΰ� �ָ��ϴ�.
			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �α��� ���� ����

		
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			sc::writeLogError(sc::string::format("TerraMsgLoginBack User Online Wrong(%s)", nlfd2->szTerraTLoginName ));
			// �������ڴ� ���
			return;
		}
	}
}

/**
* Encoding �� TID ���� Decoding �� TID �� ��´�.
* \param szEncodedTID Encoding �� TID
* \param szDecodedTID Decoding �� TID
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

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
	{
		m_pClientManager->SetAccountPassing(dwClient, true);
	}

	EXCITE_NET_LOGIN_DATA* pLogin = ( EXCITE_NET_LOGIN_DATA* ) nmg;

	if (pLogin == NULL) return;

	NET_LOGIN_FEEDBACK_DATA	nlfd;
	nlfd.nType = NET_MSG_LOGIN_FB;

	m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

	// �����Ϸ��� ä�� �˻�
	int nChannel = pLogin->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������

		sc::writeLogError(sc::string::format("ExciteMsgLogin Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
//		CloseClient(dwClient); // ��������
		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));
		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// Full �� �ƴҰ�� �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		} // if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		else // ����������
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
		// ���������� ���� �������̱� ������ �߸��� �������.
		nlfd.m_Result = EM_LOGIN_FB_SUB_UNCON;
		SendClient(dwClient, &nlfd);
		sc::writeLogError(sc::string::format("ExciteMsgLogin ExciteGetMd5 false (%s)", pLogin->szUID ));
		return;
	} // if (bMd5Result == FALSE)

	bool bDateCheck = false;
	bDateCheck = m_MinMd5.ExciteCheckDate(strTDate);
	if (bDateCheck == false)
	{
		// �����ð��� �ʰ��� ������̱� ������, ���� �������� �϶�� �޽����� ������.			
		nlfd.m_Result = EM_LOGIN_FB_SUB_EXPIRED;
		SendClient(dwClient, &nlfd);
		sc::writeLogError(sc::string::format("ExciteMsgLogin time expired (%s)", pLogin->szTDATE ));
		return;
	} // if (bDateCheck == FALSE)	

	// ����� ID ����
	m_pClientManager->SetUserID(dwClient, strUserID.GetString());

	// ����� UID �� ��, �׿� �´� �α��� �ǵ�� ����Ÿ�� �����Ѵ�.
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

	// �α��� ��û�� Ŭ���̾�Ʈ�� �������� �˻��Ѵ�.	
	nlfd.nType       = NET_MSG_LOGIN_FB;
	nlfd.m_GameVersion = m_PatchVersion;
	nlfd.m_LauncherVersion= m_LauncherVersion;

	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient), nlfd2->szExciteUserID) == 0))
		{
			// Use Features ����ȭ;
			if ( m_pGLAgentServer )
				m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
			else
				m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

			m_pClientManager->SetAccountPass(dwClient, true); // ������� ����

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
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
            m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // Premium �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			SendClient(dwClient, &nlfd);
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �̹� ��������
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
			sc::writeLogWarn(sc::string::format("ExciteMsgLoginBack User Already Offline %1%", nlfd2->szExciteUserID));
			return;
		}
		// �α��μ���, �¶��������� ip, id �� Ʋ����...
		else
		{
			m_pClientManager->SetAccountPass(dwClient, false); // ������� ����

			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User IP/ID Wrong(%s)", nlfd2->szExciteUserID ));
			// �������ڴ� ����Ų��. 
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
        m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
        m_pClientManager->SetAccountPassing(dwClient, false); // ������ false

        // �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

                    // �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User Offline AND ID/PWD Wrong(%s)", nlfd2->szExciteUserID ));
			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			sc::writeLogError(sc::string::format("ExciteMsgLoginBack User Online ID/PWD Wrong(%s)", nlfd2->szExciteUserID ));
			// �������ڴ� ���
			return;
		}
	}
}


/**
* �Ϻ�Gonzo �α��� : id / pwd �Է½�
*/
void CAgentServer::JapanMsgLogin(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (m_ServiceProvider != SP_JAPAN) return;

	if (nmg == NULL)
		return;

	// �̹� ��������...
	if (m_pClientManager->IsAccountPassing(dwClient) == true)
	{
		return;
	}
	else // ���������� ����
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

	// Tea �˰������� ��ȣȭ�� ��Ŷ ��ȣȭ
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
		SendClient(dwClient, &nlfd); // �������
		m_pClientManager->SetAccountPassing (dwClient, false); // ��������� ���� false

		sc::writeLogError(sc::string::format("JapanMsgLogin Not Correct Encrypt UserID : %s, Client : %s, Server : %s,", pNml->szUserid, szEncrypt, m_szEncrypt ));

		return;
	}
	
	// �����Ϸ��� ä�� �˻�
	int nChannel = pNml->nChannel;

	// ä�� ��ȣ�� �ùٸ��� �˻��Ѵ�.
	if (nChannel < 0 || nChannel >= m_nServerChannelNumber)
	{
		NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������
		CloseClient(dwClient, net::KICKOUT_UNDEFINE); // ��������

		sc::writeLogError(sc::string::format("MsgLogIn Invalid Channel Number (Channel:%d)", nChannel ));
		return;
	}

	// �ش� ä���� �����ؼ� full ���� �˻��ؾ� �Ѵ�.
	if (IsChannelFull(nChannel) == true)
	{
		// �ش� ä���� full �̸� ������ ����� �Ѵ�.
		// Full �� ��� ó���Ѵ�.
        NET_LOGIN_FEEDBACK_DATA		nlfd;
		nlfd.nType = NET_MSG_LOGIN_FB;
		nlfd.m_Result   = EM_LOGIN_FB_CH_FULL;
		SendClient(dwClient, &nlfd); // �������

		sc::writeLogWarn(sc::string::format("CH(%d) is FULL", nChannel ));

		return;
	}
	else
	{
		if (m_pClientManager->GetChannel(dwClient) == -1) // �ű�������
		{
			// �ش� ä�� �������� 1 ������Ų��.
			IncreaseChannelUser(nChannel);
			// Ŭ���̾�Ʈ�� ä�ι�ȣ�� �����Ѵ�.
			m_pClientManager->SetChannel(dwClient,nChannel);
		}
		else // ����������
		{
			
		}
	}

	// WhiteRock���� ��� �ϴ� �κ��� Thread ó��
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
	// ����� id ����
	m_pClientManager->SetUserID(dwClient, szUserid);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClient, false); // ������� ���� ����
	
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

// �Ϻ� Gonzo Japan WhiteRock �ý��۰��� ��� ����
void CAgentServer::JapanWRFailed( DWORD dwClient, int nResult )
{
	DWORD dwClientNum = 0;
	dwClientNum = dwClient;

	NET_LOGIN_FEEDBACK_DATA		nlfd;
	nlfd.nType = NET_MSG_LOGIN_FB;
	
	if( HTTP_STATUS_BAD_GATEWAY == nResult )
		nlfd.m_Result = EM_LOGIN_FB_SUB_INCORRECT; // ID, PW ����ġ�� �α��� ����
	else if( HTTP_STATUS_ALREADY_LOGIN == nResult )
		nlfd.m_Result = EM_LOGIN_FB_SUB_DUP;	// �ߺ��α���
	else
		nlfd.m_Result = EM_LOGIN_FB_SUB_SYSTEM; // �ý��� ������ �α��� ����

	SendClient(dwClientNum, &nlfd); // �������
	m_pClientManager->SetAccountPassing (dwClientNum, false); // ��������� ���� false
	return;
}

// �Ϻ� Gonzo Japan WhiteRock �ý��۰��� ��� ����
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

	// ����� id ����
	m_pClientManager->SetUserID(dwClientNum, szUserID);
	// DB �� ������û
	m_pClientManager->SetAccountPass(dwClientNum, false); // ������� ���� ����
	
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
* �Ϻ� �α��� ��� : id / pwd �Է½�
*/
void CAgentServer::JapanMsgLoginBack (JAPAN_NET_LOGIN_FEEDBACK_DATA2* nlfd2)
{
	NET_LOGIN_FEEDBACK_DATA		nlfd;

	nlfd.nType = NET_MSG_LOGIN_FB;
	
	DWORD	dwClient   = (DWORD) nlfd2->nClient;
	DWORD	dwSndBytes = 0;
	int		nUserNum   = 0;

    // �α��� ����
	if (nlfd2->m_emResult == EM_LOGIN_FB_SUB_OK)
	{
		// �α��� �����̰� �¶����̰� IP, ID �� �����Ҷ�...
		if ((m_pClientManager->IsOnline(dwClient) == true) &&
			(m_pClientManager->GetClientIP(dwClient) == nlfd2->szIp) &&
			(strcmp(m_pClientManager->GetUserID(dwClient),   nlfd2->JapanUserID) == 0) )
		{
			// Use Features ����ȭ;
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
			m_pClientManager->SetUserDbNum      (dwClient, nlfd2->nUserNum);       // ������ȣ
			m_pClientManager->SetLoginTime    (dwClient);                        // �α��� �ð�
			m_pClientManager->SetUserType     (dwClient, nlfd2->nUserType);      // ����Ÿ�Լ���
			m_pClientManager->SetChaRemain	  (dwClient, nlfd2->uChaRemain);     // ����� �ִ� ĳ���� ����
			m_pClientManager->SetChaTestRemain(dwClient, nlfd2->uChaTestRemain); // �׽�Ʈ �������� ���� �� �ִ� ĳ���� ����
			m_pClientManager->SetPremiumDate  (dwClient, nlfd2->tPremiumTime);   // �����̾� ���� �Ⱓ
			m_pClientManager->SetChatBlockDate(dwClient, nlfd2->tChatBlockTime); // ä�� ��� �Ⱓ

			m_pClientManager->SetAccountPass(dwClient, true); // �����������

			nlfd.m_Extreme = nlfd2->bExtreme;
			m_pClientManager->SetExtremeCreate( dwClient, nlfd2->bExtreme );

			nlfd.m_bJumping = nlfd2->bJumping;
			m_pClientManager->SetJumpingCreate( dwClient, nlfd2->bJumping );

			nlfd.m_bActor = nlfd2->bActor;
			m_pClientManager->SetActorCreate( dwClient, nlfd2->bActor );

			SendClient(dwClient, &nlfd); // �������

			// UUID�� Ŭ���̾�Ʈ�� �Ѱ��ش�.
			JAPAN_NET_LOGIN_UUID netMsg;

			StringCchCopy( netMsg.szUUID, sc::string::UUID_LENGTH, m_pClientManager->JPGetUUID( dwClient ) ); 
			SendClient(dwClient, &netMsg );	

			return;
		}		
		else if (m_pClientManager->IsOnline(dwClient) == false)
		{
            // �α��μ��������� �̹� ���������϶�...
			// �α׾ƿ� ��Ų��.
			m_pDbManager->UserLogoutSimple(nlfd2->JapanUserID);
            sc::writeLogWarn(sc::string::format("MsgLoginBack User Already Offline %1%", nlfd2->JapanUserID));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������
			return;
		}		
		else 
		{
            // �α��μ���, �¶��������� ip, id �� Ʋ����...
			// ��������ڴ� �α׾ƿ� ��Ű��
			m_pDbManager->UserLogoutSimple (nlfd2->JapanUserID);			

			sc::writeLogError(sc::string::format("MsgLoginBack User IP/ID Wrong %1%", nlfd2->JapanUserID));

			// �������ڴ� ����Ų��.
			// ���� ����ڰ� �α��� ��û���� ���ɼ��� �ִ�.
			return;
		}
	}
	else // �α��� �����϶�
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pClientManager->SetAccountPassing(dwClient, false);

		// �α��� �����̰� �¶����̰� IP, ID ��ġ�ϰų� �ߺ��� �α����� ���
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

                    // �¶��� �����ΰ��
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
		// �α��� �����̰� �̹� ���������϶�...
		else if (m_pClientManager->IsOnline(dwClient) == false )
		{
			sc::writeLogError(sc::string::format("MsgLoginBack User Login Fail And Offline(%s)", nlfd2->JapanUserID ));

			nlfd.nType = NET_MSG_LOGIN_FB;
			nlfd.m_Result   = EM_LOGIN_FB_SUB_ALREADYOFFLINE;
			SendClient(dwClient, &nlfd); // �������

			return;
		}
		else // �α��� �����̰� �¶��������� IP, ID �� ��ġ���� ������
		{
			// �������ڴ� ���
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

	// UserID�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// Use Features ����ȭ;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pDbManager->UserLogoutSimple(nlfd2->szExciteUserID);
		sc::writeLogError( sc::string::format("ExciteMsgPassCheckBack User Already Offline(%s)", nlfd2->szExciteUserID ));
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("ExciteMsgPassCheckBack Login Error(%s)", nlfd2->szExciteUserID ));
		return;
	}
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_SECID_ALREADY )	// �Ƶ� �ߺ�
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;	
	}	

	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;
	}
	else // �α��� �����϶�
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
	//	Memo :	����������, �ѱ����� ���ȴ�.
	//if (m_nServiceProvider != SP_TERRA) return;

	if (nmg == NULL) return;

	TERRA_NET_PASSCHECK_DATA* pPassCheck = (TERRA_NET_PASSCHECK_DATA*)nmg;

	if (pPassCheck == NULL) return;

	TCHAR szTLoginName[TERRA_TLOGIN_NAME+1];
	TCHAR szUserPass[TERRA_USERPASS+1];

	int nCheckFlag = pPassCheck->nCheckFlag;

	StringCchCopy( szTLoginName, TERRA_TLOGIN_NAME+1, pPassCheck->szTLoginName );
	StringCchCopy( szUserPass, TERRA_USERPASS+1, pPassCheck->szUserPass );	

	// TLoginName�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// Use Features ����ȭ;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pDbManager->UserLogoutSimple(nlfd2->szTerraTLoginName);
		sc::writeLogError( sc::string::format("TerraMsgPassCheckBack User Already Offline(%s)", nlfd2->szTerraTLoginName ));
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult    = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("TerraMsgPassCheckBack Login Error(%s)", nlfd2->szTerraTLoginName ));
		return;
	}
	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult	= nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // �α��� �����϶�
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
	//	Memo :	����������, �ѱ����� ���ȴ�.
	if (m_ServiceProvider != SP_KOREA) return;

	if (nmg == NULL) return;

	DAUM_NET_PASSCHECK_DATA* pPassCheck = (DAUM_NET_PASSCHECK_DATA*) nmg;

	if (pPassCheck == NULL) return;

	// TLoginName�� �̿��ؼ� ����� �н����带 üũ�Ѵ�.
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

	if (nlfd2->nResult == EM_LOGIN_FB_SUB_OK) // �α��� �����϶�
	{
		// Use Features ����ȭ;
		if ( m_pGLAgentServer )
			m_pGLAgentServer->SynchronizeUseFeatures( dwClient );
		else
			m_pGLAgentServer->SynchronizeUseFeaturesJoinWorldBattle( dwClient );

		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
	}
	// �̹� ��������
	else if (m_pClientManager->IsOnline(dwClient) == false)
	{
		m_pClientManager->SetAccountPass(dwClient, false); // ������� ����
		m_pDbManager->UserLogoutSimple(m_pClientManager->GetUserID(dwClient));
		sc::writeLogError( sc::string::format("DaumMsgPassCheckBack User Already Offline(%s)", nlfd2->szDaumGID ));
		return;
	}
	// �α��� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_FAIL )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);
		sc::writeLogError( sc::string::format("DaumMsgPassCheckBack Login Error(%s)", nlfd2->szDaumGID ));
		return;
	}
	// �ʱ� �Է� ����
	else if( nlfd2->nResult == EM_LOGIN_FB_SUB_PASS_OK )
	{
		nlfd.nResult = nlfd2->nResult;
		SendClient(dwClient, &nlfd);		
		return;

	}
	else // �α��� �����϶�
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

			//// HTTP �����ڵ带 ���´�. 
			//if ( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_HTTP_CODE , &statLong) )
			//{	
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>> Response code (%1%).", statLong));	
			//}
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Response code (%1%).", statLong));	
			//}

			//// Content-Type �� ���´�.
			//if( CURLE_OK == curl_easy_getinfo( curl , CURLINFO_CONTENT_TYPE , &statString ) )
			//{
			//	sc::writeLogInfo(sc::string::format(">>>>>>>>>>>>>>> Content type: %1%", statString));
			//}
			//else
			//{
			//	sc::writeLogError(sc::string::format(">>>>>>>>>>>>>>> ERROR Content type (%1%).", statString));	
			//}

			//// �ٿ�ε��� ������ ũ�⸦ ���´�.
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

			nBlock = 0; // blcok ���� bool ������ int �� �����ϱ�� �ߴµ� �������� �����ϸ� �ٽ� �����ؾ���
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
			//sc::writeLogError(std::string(">>>>>>>>>>>>>>> EU ���� ����"));			
			bRet = FALSE;
		}
	}
	else
	{
		// ERROR : curl == NULL �� ��� 
		//sc::writeLogError(std::string(">>>>>>>>>>>>>>> EU �ʱ�ȭ ���� curl == NULL"));			
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
