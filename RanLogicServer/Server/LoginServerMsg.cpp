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
	
    // 서버간 통신 메시지
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

	//NET_CLIENT_VERSION_SC 로 변경했었으나, 이미 릴리즈되어있는 Launcher에서는 이 메시지를 받도록 구현되어 있지 않으므로,
	//다시 NET_CLIENT_VERSION_CS로 변경한다. Launcher patch도 이 메시지를 받은 후에 버전비교를 해서 수행하기 때문에 이 메시지 번호는 변경되어서는 안된다.
    NET_CLIENT_VERSION_CS msg(net::INVALID_VERSION, net::INVALID_VERSION);

	SOCKET sSocket = m_pClientManager->GetSocket(dwClient);

	bool bIsAllowIp = CheckAllowIP( sSocket );
	// 허용아이피는 xml에서 수정가능하기때문에 국가로만 설정하도록 변경
	//if( bIsAllowIp == false && CountryAccessApprove( sSocket ) == false )
	//{	
	//	msg.m_PatchVersion		= net::INVALID_COUNTRY; // 접속할 수 없는 국가코드
	//	msg.m_LauncherVersion	= net::INVALID_COUNTRY;  // 접속할 수 없는 국가코드
	//}
	// 허용된 국가가 아니면 접속 불가
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

			// 이름 바꾸자!!!!

			//
			// GetAllowIP() ----> AllowIP만 접속가능하게 할 것인가? 즉, OnlyAllowIPConnect
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

				// 설정한 버전보다 클 경우에만 패치한다.
				if( m_PatchCutVersion < netMessage->m_PatchVersion )
				{
					msg.m_PatchVersion		= m_PatchVersion;
					msg.m_LauncherVersion	= m_LauncherVersion;
				}

				// 0이거나 런쳐가 구형이라면 기존 방식대로 동작한다.
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

// 게임서버 정보를 클라이언트에 전송한다.
void CLoginServer::MsgSndGameSvrInfo(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
    if (!nmg)
        return;

    DWORD m_dwTemp = timeGetTime();    
    // 게임서버 정보 업데이트 시간이 지정시간 이상 경과했으면 새롭게 정보를 요청한다.
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