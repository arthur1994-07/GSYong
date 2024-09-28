#include "../pch.h"
#include "./AgentServer.h"
#include "../Database/DbAction/DbActionGame.h"
#include "../Database/DbAction/DbActionUser.h"
#include "../Database/DbAction/DbActionGameClub.h"
#include "../Client/NetAgentClientMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Client->Agent : �ɸ��� ����
void CAgentServer::MsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;
	
	// ������ ������� ����, ���� ���, ��������
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
* ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
	if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
		
		// �ذ��� ��/�� ���� ���� ���ڸ� ĳ���� ���������ϰ� Ŭ���̾�Ʈ���� �Ѱ��ش�.
		ncbi.bExtreme = m_pClientManager->GetExtreme( dwClient );
		ncbi.bActor = m_pClientManager->GetCreateActor( dwClient );
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����            
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));
            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{
		m_pClientManager->InvalidateChaNum(dwClient, nChaNum);

		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::GspMsgSndChaDelInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
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
 * Agent->Client : ĳ���� ������û
 * \param pMsg 
 */
void CAgentServer::TerraMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
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
* Agent->Client:ĳ���� �����������
* \param nChaNum ĳ���͹�ȣ
* \param nUserNum ������ȣ
* \param nResult ���
* \param dwClient Ŭ���̾�Ʈ��ȣ
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
    //  0 : ��������
    // -1 : db error
    if (nResult == sc::db::DB_OK) // ��������
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain( dwClient ) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain( dwClient, wChaRemain );
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum( (DWORD) nChaNum );
        if ( (nGuNum != sc::db::DB_ERROR) && (nGuNum > 0) )
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain( dwClient );
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain( dwClient );
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient( dwClient, &ncbi );
	}
}

/**
* Agent->Client:ĳ���� �����������
* \param nChaNum ĳ���͹�ȣ
* \param nUserNum ������ȣ
* \param nResult ���
* \param dwClient Ŭ���̾�Ʈ��ȣ
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
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
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));
            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
 * Agent->Client : ĳ���� ������û
 * \param pMsg 
 */
void CAgentServer::DaumMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
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

	// ������ ������� ����, ���� ���, ��������
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
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
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
	// -1 : db error
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
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
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}

/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::ExciteMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
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
    // -2 : ��帶���� �������� �ʿ��ϴ�
    //  0 : ��������
    // -1 : db error
    if (nResult == sc::db::DB_OK) // ��������
	{
		ncbi.nType    = NET_MSG_CHA_DEL_FB_OK;
		ncbi.nChaNum      = nChaNum;
		WORD wChaRemain   = 0;
				
		if (m_bTestServer) 
		{
			wChaRemain = m_pClientManager->GetChaTestRemain(dwClient) + 1;
            ncbi.wChaRemain = wChaRemain;
			m_pClientManager->SetChaTestRemain(dwClient, wChaRemain);
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort) )
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent:ĳ���� ������û
* \param pMsg 
*/
void CAgentServer::JapanMsgSndChaDelInfo(DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	// ������ ������� ����, ���� ���, ��������
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
    //  0 : ��������    
	//  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if ( nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
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
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer) ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else               ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}


/**
* Client->Agent : �ɸ��� ����
*/
// GS�� GM , US ���ÿ� ���
void CAgentServer::GsMsgSndChaDelInfo( DWORD dwClient, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if (nmg == NULL) return;
	
	// ������ ������� ����, ���� ���, ��������
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
			// GS�� GM , US ���ÿ� ���
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
* ���Ӽ���->Ŭ���̾�Ʈ : ĳ���� ���� ��� ����
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
    //  0 : ��������    
    //  1 : �ذ��� ���� ����
    //  2 : �ذ��� ���� ����
    if (nResult == sc::db::DB_OK || 1 == nResult || 2 == nResult ) // ��������
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
            // �׽�Ʈ���� ĳ���� �������� ����
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
            // ĳ���� �������� ����
            m_pDbManager->AddLogJob(
                db::DbActionPtr(
                    new CUserChaNumIncrease(nUserNum)));            
		}
        // ĳ������ Ŭ����ȣ�� �����ͼ� ó���Ѵ�.
        int nGuNum = m_pDbManager->GetChaGuildNum((DWORD) nChaNum);		
        if ((nGuNum != sc::db::DB_ERROR) && (nGuNum > 0))
        {
            // Ŭ������ ������ ĳ���� ���� �޽��� ����.
			//
			GLMSG::SNET_CLUB_MEMBER_DB2DEL NetMsgDb2Del;
			NetMsgDb2Del.dwClub = nGuNum;
			NetMsgDb2Del.dwMember = nChaNum;
			InsertMsg ( DBACTION_CLIENT_ID, (char*) &NetMsgDb2Del );

			// Ŭ�� ���� ����
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
    else // ���� ����
	{
        if (m_bTestServer)
            ncbi.wChaRemain = m_pClientManager->GetChaTestRemain(dwClient);
        else
            ncbi.wChaRemain = m_pClientManager->GetChaRemain(dwClient);
		ncbi.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}

	// IP/PORT �� �����Ҷ��� ó��
	if ((m_pClientManager->GetClientIP(dwClient) == szUserIP) &&
		(m_pClientManager->GetClientPort(dwClient) == uPort))
	{	
		SendClient(dwClient, &ncbi);
	}
}
