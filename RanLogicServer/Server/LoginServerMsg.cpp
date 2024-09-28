#include "../pch.h"
#include "../../EngineLib/Common/profile.h"
#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../Client/NetLoginClientMan.h"
#include "./LoginServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CLoginServer::initPacketFunc()
{	
	m_MsgFunc[NET_MSG_CLIENT_VERSION_CS].Msgfunc	= &CLoginServer::MsgVersion;
	m_MsgFunc[NET_MSG_HEARTBEAT_CLIENT_ANS].Msgfunc	= &CLoginServer::MsgHeartbeatClientAns;
	m_MsgFunc[NET_MSG_REQ_GAME_SVR].Msgfunc			= &CLoginServer::MsgSndGameSvrInfo;    
	//// for global auth
	//m_MsgFunc[NET_MSG_AUTH_CERTIFICATION_ANS].Msgfunc = &CLoginServer::MsgAuthCertificationResponse;
}

int CLoginServer::MsgProcess(MSG_LIST* pMsg)
{	
	if (!pMsg)
		return NET_ERROR;

	NET_MSG_GENERIC* nmg = reinterpret_cast<NET_MSG_GENERIC*> (pMsg->Buffer);
	DWORD dwClient = pMsg->dwClient;

	//	MsgProfile
	PROFILE_MSG_COUNT_RECV(nmg->nType, nmg->dwSize);
	
    // ������ ��� �޽���
    if (dwClient == m_SessionServerSlot)
    {
        ServerMsgProcess(pMsg);
        return NET_OK;
    }

	EMNET_MSG emType = nmg->nType;
	if (emType < 0 || 
        emType >= NET_MSG_GCTRL_MAX || 
        NULL == m_MsgFunc[emType].Msgfunc )
	{
		sc::writeLogError(
            sc::string::format(
                "CLoginServer::MsgProcess Out of range or illegal message %1%", emType));
	}
	else
	{
		(this->*m_MsgFunc[emType].Msgfunc)(dwClient, nmg);
	}

	return NET_OK;
}

void CLoginServer::ServerMsgProcess(MSG_LIST* pMsg)
{
    if (pMsg)
    {
        DWORD dwClient = pMsg->dwClient;
        if (dwClient == m_SessionServerSlot)
            SessionMsgProcess(pMsg);
    }
}

void CLoginServer::MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg)
{	
	if (!nmg)
		return;

	//NET_CLIENT_VERSION_SC �� �����߾�����, �̹� ������Ǿ��ִ� Launcher������ �� �޽����� �޵��� �����Ǿ� ���� �����Ƿ�,
	//�ٽ� NET_CLIENT_VERSION_CS�� �����Ѵ�. Launcher patch�� �� �޽����� ���� �Ŀ� �����񱳸� �ؼ� �����ϱ� ������ �� �޽��� ��ȣ�� ����Ǿ�� �ȵȴ�.
    NET_CLIENT_VERSION_CS msg(net::INVALID_VERSION, net::INVALID_VERSION);

	SOCKET sSocket = m_pClientManager->GetSocket(dwClient);

	bool bIsAllowIp = CheckAllowIP( sSocket );
	// �������Ǵ� xml���� ���������ϱ⶧���� �����θ� �����ϵ��� ����
	//if( bIsAllowIp == false && CountryAccessApprove( sSocket ) == false )
	//{	
	//	msg.m_PatchVersion		= net::INVALID_COUNTRY; // ������ �� ���� �����ڵ�
	//	msg.m_LauncherVersion	= net::INVALID_COUNTRY;  // ������ �� ���� �����ڵ�
	//}
	// ���� ������ �ƴϸ� ���� �Ұ�
	if( CountryAccessApprove( sSocket ) == false )
	{	
		msg.m_PatchVersion		= net::INVALID_COUNTRY;
		msg.m_LauncherVersion	= net::INVALID_COUNTRY;
	}
	else
	{
		//
		//mjeon
		//GameProgramVer: Game Version
		//PatchProgramVer: Launcher Version
		//	
		if (m_bInspecting) //Check
		{
			msg.m_PatchVersion		= net::INVALID_VERSION;
			msg.m_LauncherVersion	= net::INVALID_VERSION;
		}	
		else // Check End
		{

			// �̸� �ٲ���!!!!

			//
			// GetAllowIP() ----> AllowIP�� ���Ӱ����ϰ� �� ���ΰ�? ��, OnlyAllowIPConnect
			//


			if( GetAllowIP() && !bIsAllowIp )
			{	
				msg.m_PatchVersion		= net::INVALID_VERSION;
				msg.m_LauncherVersion	= net::INVALID_VERSION;
			
			}
			else
			{
				NET_CLIENT_VERSION_CS* netMessage = (NET_CLIENT_VERSION_CS*)nmg;

				msg.m_PatchVersion		= net::EXCEED_VERSION;
				msg.m_LauncherVersion	= net::EXCEED_VERSION;

				// ������ �������� Ŭ ��쿡�� ��ġ�Ѵ�.
				if( m_PatchCutVersion < netMessage->m_PatchVersion )
				{
					msg.m_PatchVersion		= m_PatchVersion;
					msg.m_LauncherVersion	= m_LauncherVersion;
				}

				// 0�̰ų� ���İ� �����̶�� ���� ��Ĵ�� �����Ѵ�.
				if( m_PatchCutVersion == 0  || msg.m_LauncherVersion == -1 )
				{
					msg.m_PatchVersion		= m_PatchVersion;
					msg.m_LauncherVersion	= m_LauncherVersion;
				}
			}		
		}
	}

	//
	//mjeon
	//m_szEncrypt includes random-string that was made during Start().
	//
	NET_ENCRYPT_KEY	msgEnc;
	::StringCchCopy(msgEnc.szEncryptKey, ENCRYPT_KEY+1, m_szEncrypt);

	SendClient(dwClient, &msg);
	SendClient(dwClient, &msgEnc);
}

void CLoginServer::MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	m_pClientManager->SetHeartBeat(dwClient);
}

// ���Ӽ��� ������ Ŭ���̾�Ʈ�� �����Ѵ�.
void CLoginServer::MsgSndGameSvrInfo(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
    if (!nmg)
        return;

    DWORD m_dwTemp = timeGetTime();    
    // ���Ӽ��� ���� ������Ʈ �ð��� �����ð� �̻� ��������� ���Ӱ� ������ ��û�Ѵ�.
    if ((m_dwTemp - m_dwLastGameSvrUpdateTime) > SVR_STATE_UPDATE_TIME)	// 3min
    {
        SessionReqSvrInfo();
        m_dwLastGameSvrUpdateTime = m_dwTemp;
        sc::writeInfo(std::string("Request New Server Info"));
    }

    lockOn();

    DWORD dwCount=0;
    for (int i=0; i<MAX_SERVER_GROUP; ++i)
    {
        for (int j=0; j<MAX_CHANNEL_NUMBER; ++j)
        {
            if (m_sGame[i][j].nServerMaxClient > 0)
            {
                NET_CUR_INFO_LOGIN ncil;                
                ncil.SetType(NET_MSG_SND_GAME_SVR);
                ncil.SetData(m_sGame[i][j]);
                SendClient(dwClient, &ncil);
                dwCount++;
            }
        }
    }

    if (dwCount == 0)
    {
        sc::writeLogError(std::string("Check Session Server Connection"));
    }

    NET_MSG_GENERIC msg;
    msg.SetSize( sizeof(NET_MSG_GENERIC) );
    msg.nType = NET_MSG_SND_GAME_SVR_END;
    SendClient(dwClient, &msg);

    lockOff();
    //lockOff();
}

void CLoginServer::DoNothing(NET_MSG_GENERIC* pMsg)
{
    sc::writeLogInfo(
        sc::string::format(
            "Received message but do nothing. %1%", pMsg->Type()));
}