#include "../pch.h"
#include "./AgentServer.h"
#include "../Database/DbAction/DbActionGame.h"
#include "../Database/DbAction/DbActionUser.h"
#include "../Database/DbAction/DbActionGameClub.h"
#include "../Client/NetAgentClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Client->Agent : 케릭터 삭제
void CAgentServer::MsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;
	
	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass( dwClient ) == false)
	{
		CloseClient( dwClient, net::KICKOUT_UNDEFINE );
		return;
	}
		
	NET_CHA_DEL* pPacket = (NET_CHA_DEL*) nmg;
	int nUserNum = m_pClientManager->UserDbNum(dwClient);

    if (pPacket->m_ChaDbNum > 0)
	{
		if (m_ServiceProvider == SP_CHINA)
		{
			m_Tea.decrypt(pPacket->m_szPass2, USR_PASS_LENGTH);
		}
		else
		{
			m_Tea.decrypt(pPacket->m_szPass2, USR_PASS_LENGTH+1);
		}
        
	    AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDelete(
		            nUserNum,
		            pPacket->m_ChaDbNum,
		            pPacket->m_szPass2,
		            dwClient,
		            m_pClientManager->GetClientIP(dwClient),
		            m_pClientManager->GetClientPort(dwClient))));
	}
}

/**
* 게임서버->클라이언트 : 캐릭터 삭제 결과 전송
*/
void CAgentServer::MsgSndChaDelInfoBack(
		int nChaNum, 
		int nUserNum, 
		int nResult, 
		DWORD dwClient, 
		const TCHAR* szUserIP, 
		USHORT uPort )
{
	NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
	if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
		
		// 극강부 남/여 생성 가능 숫자를 캐릭터 삭제성공하고 클라이언트에게 넘겨준다.
		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 감소
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));
            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if ( wChaRemain < 0 ||
				 wChaRemain >= MAX_CHAR_LENGTH )
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain( dwClient, wChaRemain );
            // 캐릭터 생성갯수 증가            
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));
            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
            AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));
			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
		m_pClientManager->InvalidateChaNum(dwClient, nChaNum);

		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::GspMsgSndChaDelInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if ( m_pClientManager->IsAccountPass( dwClient ) == false )
	{
		CloseClient( dwClient, net::KICKOUT_UNDEFINE );
		return;
	}
	
	GSP_NET_CHA_DEL* pMsg2 = (GSP_NET_CHA_DEL*)nmg;

	if (pMsg2 == NULL) return;

	int   nUserNum  = m_pClientManager->UserDbNum( dwClient );

    if (pMsg2->nChaNum > 0)
	{        
        AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteGsp(
                    nUserNum, 
                    pMsg2->nChaNum,
                    dwClient, 
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
}

/**
 * Agent->Client : 캐릭터 삭제요청
 * \param pMsg 
 */
void CAgentServer::TerraMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	
	TERRA_NET_CHA_DEL* ncd = (TERRA_NET_CHA_DEL*)nmg;

	if (ncd == NULL) return;

	int   nUserNum  = m_pClientManager->UserDbNum(dwClient);

    if (ncd->nChaNum > 0)
	{        
        AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteTerra(
                    nUserNum, 
                    ncd->nChaNum,
                    dwClient, 
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
}

/**
* Agent->Client:캐릭터 삭제결과전송
* \param nChaNum 캐릭터번호
* \param nUserNum 유저번호
* \param nResult 결과
* \param dwClient 클라이언트번호
* \param szUserIP IP
* \param uPort PORT
*/
void CAgentServer::GspMsgSndChaDelInfoBack(
	int nChaNum, 
	int nUserNum, 
	int nResult, 
	DWORD dwClient, 
	const TCHAR* szUserIP, 
	USHORT uPort )
{
	NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
    //  0 : 삭제성공
    // -1 : db error
    if (nResult == sc::db::DB_OK) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain( dwClient ) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain( dwClient, wChaRemain );
            // 테스트서버 캐릭터 생성갯수 감소
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain( dwClient ) + 1;
			if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain( dwClient, wChaRemain );
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum( (DWORD) nChaNum );
        if ( (nGuNum != sc::db::DB_ERROR) && (nGuNum > 0) )
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
            AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient( dwClient, &ncbi );
	}
}

/**
* Agent->Client:캐릭터 삭제결과전송
* \param nChaNum 캐릭터번호
* \param nUserNum 유저번호
* \param nResult 결과
* \param dwClient 클라이언트번호
* \param szUserIP IP
* \param uPort PORT
*/
void CAgentServer::TerraMsgSndChaDelInfoBack(int nChaNum, 
	                                         int nUserNum, 
							                 int nResult, 
							                 DWORD dwClient, 
							                 const char* szUserIP, 
							                 USHORT uPort)
{
	NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 감소
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));
            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
 * Agent->Client : 캐릭터 삭제요청
 * \param pMsg 
 */
void CAgentServer::DaumMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	
	DAUM_NET_CHA_DEL* pPacket = (DAUM_NET_CHA_DEL*) nmg;

	if (!pPacket)
        return;

	int nUserNum  = m_pClientManager->UserDbNum(dwClient);

    if (pPacket->m_ChaDbNum > 0)
	{		
		m_Tea.decrypt(pPacket->m_szPass2, USR_PASS_LENGTH+1);

        AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteDaum(
                    nUserNum, 
                    pPacket->m_ChaDbNum,
					pPacket->m_szPass2,
                    dwClient, 
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
}


void CAgentServer::IdnMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL)
    {
        sc::writeLogError(std::string("CAgentServer::IdnMsgSndChaDelInfo() - nmg is null."));

		return;
    }

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
        sc::writeLogError(std::string("CAgentServer::IdnMsgSndChaDelInfo() - AccountPass failed."));

		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	
	IDN_NET_CHA_DEL* pPacket = (IDN_NET_CHA_DEL*)nmg;

	if (!pPacket)
    {
        sc::writeLogError(std::string("CAgentServer::IdnMsgSndChaDelInfo() - pPacket is null."));

		return;
    }

	int nUserNum = m_pClientManager->UserDbNum(dwClient);
	
	std::string strUserID = std::string( m_pClientManager->GetUserID(dwClient) );

	if (strUserID.empty())
	{
		sc::writeLogError(sc::string::format("CAgentServer::IdnMsgSndChaDelInfo() - UserID is empty for the User(%1%).", nUserNum));
		return;	
	}

	std::string str2ndPass(pPacket->sz2ndPass);

	if (str2ndPass.empty())
	{
		sc::writeLogError(sc::string::format("CAgentServer::IdnMsgSndChaDelInfo() - 2ndPass is empty for the User(%1%).", nUserNum));
		return;
	}

    if (pPacket->nChaNum > 0)
	{        
        AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteIdn(
                    nUserNum,
                    pPacket->nChaNum,
                    dwClient,
				    strUserID,
				    str2ndPass,
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
    else
        sc::writeLogError(std::string("CAgentServer::IdnMsgSndChaDelInfo() - nChaNum is invalid value."));
}


void CAgentServer::DaumMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
            if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

			ncbi.wChaRemain = wChaRemain;			
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}


void CAgentServer::IdnMsgSndChaDelInfoBack(int nChaNum, int nUserNum, int nResult, DWORD dwClient, const char* szUserIP, USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
            if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

			ncbi.wChaRemain = wChaRemain;			
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::ExciteMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	
	EXCITE_NET_CHA_DEL* ncd = (EXCITE_NET_CHA_DEL*)nmg;

	if (ncd == NULL) return;

	int   nUserNum  = m_pClientManager->UserDbNum(dwClient);

    if (ncd->nChaNum > 0)
	{        
	    AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteExciteJapan(
                    nUserNum, 
                    ncd->nChaNum,
                    dwClient, 
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
}

void CAgentServer::ExciteMsgSndChaDelInfoBack(int nChaNum,
											  int nUserNum,
											  int nResult,
											  DWORD dwClient,
											  const char* szUserIP,
											  USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -2 : 길드마스터 길드삭제가 필요하다
    //  0 : 삭제성공
    // -1 : db error
    if (nResult == sc::db::DB_OK) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent:캐릭터 삭제요청
* \param pMsg 
*/
void CAgentServer::JapanMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass(dwClient) == false)
	{
		CloseClient(dwClient, net::KICKOUT_UNDEFINE);
		return;
	}
	
	JAPAN_NET_CHA_DEL* ncd = (JAPAN_NET_CHA_DEL*)nmg;

	if (ncd == NULL) return;

	int   nUserNum  = m_pClientManager->UserDbNum(dwClient);

    if (ncd->nChaNum > 0)
	{        
	    AddGameAdoJob(
            db::DbActionPtr(
                new db::CharacterDeleteJapan(
                    nUserNum, 
                    ncd->nChaNum,
                    dwClient, 
                    m_pClientManager->GetClientIP(dwClient),
                    m_pClientManager->GetClientPort(dwClient))));
	}
}

void CAgentServer::JapanMsgSndChaDelInfoBack(int nChaNum,
											  int nUserNum,
											  int nResult,
											  DWORD dwClient,
											  const char* szUserIP,
											  USHORT uPort)
{
    NET_CHA_DEL_FB ncbi;

	// -1 : db error
    //  0 : 삭제성공    
	//  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent : 케릭터 삭제
*/
// GS와 GM , US 동시에 사용
void CAgentServer::GsMsgSndChaDelInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;
	
	// 인증을 통과하지 못함, 삭제 취소, 연결종료
	if (m_pClientManager->IsAccountPass( dwClient ) == false)
	{
		CloseClient( dwClient, net::KICKOUT_UNDEFINE );
		return;
	}
		
	GS_NET_CHA_DEL* pPacket = (GS_NET_CHA_DEL*) nmg;

	int   nUserNum  = m_pClientManager->UserDbNum( dwClient );

    if (pPacket->m_ChaDbNum > 0)
	{
		m_Tea.decrypt( pPacket->m_szPass2, GS_USER_PASS+1 );
        
        AddGameAdoJob(
            db::DbActionPtr(
			// GS와 GM , US 동시에 사용
	            new db::CharacterDeleteGs(
		            nUserNum,
		            pPacket->m_ChaDbNum,
		            pPacket->m_szPass2,
		            dwClient,
		            m_pClientManager->GetClientIP( dwClient ),
		            m_pClientManager->GetClientPort( dwClient ))));
	}
}

/**
* 게임서버->클라이언트 : 캐릭터 삭제 결과 전송
*/
void CAgentServer::GsMsgSndChaDelInfoBack(
		int nChaNum, 
		int nUserNum, 
		int nResult, 
		DWORD dwClient, 
		const TCHAR* szUserIP, 
		USHORT uPort )
{
    NET_CHA_DEL_FB ncbi;

    // nResult
    // -1 : db error
    //  0 : 삭제성공    
    //  1 : 극강부 남자 삭제
    //  2 : 극강부 여자 삭제
    if (nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // 삭제성공
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;

		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );

		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // 테스트서버 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserTestChaNumIncrease(nUserNum)));            
		}
		else
		{
			wChaRemain = m_pClientManager->GetChaRemain(dwClient) + 1;
			if (wChaRemain >= MAX_CHAR_LENGTH)
			{
				wChaRemain = 0;
			}

            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaRemain(dwClient, wChaRemain);
            // 캐릭터 생성갯수 증가
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // 캐릭터의 클럽번호를 가져와서 처리한다.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // 클럽에서 삭제된 캐릭터 제거 메시지 전송.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// 클럽 권한 리셋
			AddGameDbJob(
                db::DbActionPtr(
                    new db::ClubMasterFlagsSet((DWORD) nGuNum, (DWORD) nChaNum, 0)));			
        }
	}
    else if (nResult == -2)
    {
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_CLUB;
    }	
    else // 삭제 에러
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT 가 동일할때만 처리
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}
