#include "../pch.h"
#include <stdlib.h>
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../RanLogic/Util/s_CClientConsoleMsg.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "./s_NetClient.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// �ܺο��� ������ ������ �����ϰ� �Ѵ�.
extern TSTRING g_strGameForceExit;

// �� �޽��� ó�� �Լ�
void CNetClient::MessageProcess()
{	
	NET_MSG_GENERIC* pNmg = NULL;
	while (true)
	{
		pNmg = (NET_MSG_GENERIC*) IsValid();
		if (!pNmg)
			break;

		Packet_Log_Read_begin();
		Packet_Log_Read_End(pNmg->nType);

		switch (m_nClientNetState)
		{
		case NET_STATE_LOGIN : // �α��� ���� ���ӻ���
			MessageProcessLogin(pNmg);
			break;
		case NET_STATE_AGENT : // Agent
		case NET_STATE_FIELD : // Field
		case NET_STATE_BOARD : // Board
			MessageProcessGame(pNmg);
			break;		
		case NET_STATE_CTRL : // �������α׷� ���ӻ���
			break;
		default :
			break;
		}

	}
}

void CNetClient::MessageProcessLogin(NET_MSG_GENERIC* nmg)
{
	switch(nmg->nType)
	{
	// ���Ӽ��� ��������
	case NET_MSG_SND_GAME_SVR:
		MsgGameSvrInfo(nmg);
		break;
	case NET_MSG_SND_GAME_SVR_END:
		MsgGameSvrInfoEnd(nmg);
		break;
	case NET_MSG_CLIENT_VERSION_CS:
    case NET_MSG_CLIENT_VERSION_SC:
		MsgVersionInfo(nmg);
		break;
	case NET_MSG_SND_ENCRYPT_KEY:
		MsgEncryptKey(nmg);
		break;
	default:
		break;
	}
}

void CNetClient::MessageProcessGame(NET_MSG_GENERIC* nmg)
{
	switch (nmg->nType)
	{		
	// ����
	case NET_MSG_CLIENT_VERSION_CS:
    case NET_MSG_CLIENT_VERSION_SC:
		MsgVersionInfo(nmg);
		break;
    case NET_MSG_CLIENT_VERSION_AC:
        MsgVersionCheck(nmg);
        break;
	case NET_MSG_SND_CRYT_KEY:
		MsgCryptKey(nmg);
		break;
	case NET_MSG_RANDOM_NUM:
		MsgRandomNum(nmg);
		break;		
	case NET_MSG_HEARTBEAT_CLIENT_REQ:
		SndHeartbeat();
		break;
	case NET_MSG_CONNECT_CLIENT_FIELD:
		MsgConnectClientToField(nmg);
		break;
	case MET_MSG_JOIN_INSTANCESVR_INFO:
		MsgConnectClientToInstance(nmg);
		break;
	case NET_MSG_SND_ENCRYPT_KEY:
		MsgEncryptKey(nmg);
		break;
	default:
		{
			// ���� �޽���, ä�� �޽���
			if (nmg->nType > NET_MSG_LOBBY)
			{
				if (m_pGlobalStage)
                    m_pGlobalStage->MsgProcess(nmg);
			}
		}
		break;
	}
}


//! Agent->Client:Field Server �� �����϶�.
int CNetClient::MsgConnectClientToField(NET_MSG_GENERIC* nmg)
{
	int nRetCode;
	NET_CONNECT_CLIENT_TO_FIELD_AC* pPacket = (NET_CONNECT_CLIENT_TO_FIELD_AC*) nmg;
	
	nRetCode = ConnectFieldServer(pPacket->szServerIP, pPacket->nServicePort);
	if (nRetCode != NET_OK)
	{
		sc::writeLogError(std::string("LOGIN ERROR. Field Svr Connection fail"));
        return NET_ERROR;
	}

	SetGaeaID			  (pPacket->dwGaeaID);
	m_pNetField->SetGaeaID(pPacket->dwGaeaID);

	NET_GAME_JOIN_FIELD_IDENTITY_CF MsgIdentity;
	MsgIdentity.emType				= pPacket->emType;
	MsgIdentity.typeAuthority		= pPacket->typeAuthority;
	MsgIdentity.dwGaeaID			= pPacket->dwGaeaID;
	MsgIdentity.dwChaNum			= pPacket->dwChaNum;
	MsgIdentity.previousMapID		= pPacket->previousMapID;
	MsgIdentity.ck						= m_ck;

	return SendToFieldNormal((char*) &MsgIdentity, MsgIdentity.Size());
}

//! Agent->Client:InstanceServer �� �����϶�.
int CNetClient::MsgConnectClientToInstance(NET_MSG_GENERIC* nmg)
{
	int nRetCode;
	NET_MSG_JOIN_INSTANCESVR_INFO* pPacket = (NET_MSG_JOIN_INSTANCESVR_INFO*) nmg;
	
	nRetCode = ConnectFieldServer(pPacket->szServerIP, pPacket->nServicePort);
	if (nRetCode != NET_OK)
	{
		sc::writeLogError(std::string("LOGIN ERROR. Instance Field Svr Connection fail"));
        return NET_ERROR;
	}

	SetGaeaID			  (pPacket->dwGaeaID);
	m_pNetField->SetGaeaID(pPacket->dwGaeaID);

	NET_GAME_JOIN_FIELD_IDENTITY_CF MsgIdentity;
	MsgIdentity.emType				= pPacket->emType;
	MsgIdentity.dwGaeaID			= pPacket->dwGaeaID;
	MsgIdentity.dwSvrGroupNum		= pPacket->dwSvrGroupNum;	
	MsgIdentity.ck					= m_ck;

	return SendToFieldNormal((char*) &MsgIdentity, MsgIdentity.Size());
}


void CNetClient::MsgRandomNum(NET_MSG_GENERIC* nmg)
{
	NET_RANDOMPASS_NUMBER* pMsg = (NET_RANDOMPASS_NUMBER*) nmg;

	sc::writeLogInfo( sc::string::format(
		"Change RandomPassNumber ( %1% -> %2% )",
		m_nRandomNumber,
		pMsg->nRandomNumber ) );

	m_nRandomNumber = pMsg->nRandomNumber;
}

// �������� ���۵Ǿ�� ��ȣŰ
void CNetClient::MsgCryptKey(NET_MSG_GENERIC* nmg)
{	
	NET_CRYPT_KEY* nck;
	nck = reinterpret_cast<NET_CRYPT_KEY*> (nmg);
	m_ck = nck->ck;
	m_bKeyReceived = TRUE;
}

// �������� ���۵Ǿ�� ���� ����
int CNetClient::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION_CS* pPacket = reinterpret_cast<NET_CLIENT_VERSION_CS*> (nmg);
	
	if( pPacket->m_PatchVersion == net::INVALID_COUNTRY && pPacket->m_LauncherVersion == net::INVALID_COUNTRY )
	{
		// GeoIP Block ������ ������ ���� �����̹Ƿ� ���� �Ұ�
		std::string ErrMsg(std::string("Current country can not access."));
        //AfxMessageBox( ErrMsg, MB_OK|MB_ICONEXCLAMATION);
		g_strGameForceExit = ErrMsg;
	}
	else
	{
		m_PatchVersion  = pPacket->m_PatchVersion;
		m_LauncherVersion = pPacket->m_LauncherVersion;	
	}

	return 0;
}

int CNetClient::MsgVersionCheck(NET_MSG_GENERIC* nmg)
{
    NET_CLIENT_VERSION_AC* pPacket = (NET_CLIENT_VERSION_AC*) nmg;
    int LauncherVer = 0;
    int PatchVer = 0;
    GetClientVer(LauncherVer, PatchVer);
    if (LauncherVer != pPacket->m_LauncherVersion || PatchVer != pPacket->m_PatchVersion)
    {
		std::string ErrMsg(
			sc::string::format(
			"Client/Server Version mismatch. Launcher %1%/%2% Patch %3%/%4%",
			LauncherVer,
			pPacket->m_LauncherVersion,
			PatchVer,
			pPacket->m_PatchVersion));

#ifndef NDEBUG
        AfxMessageBox(ErrMsg.c_str(), MB_OK|MB_ICONEXCLAMATION);
#endif
		// exit �� ����ϰ� �Ǹ� �˵� ���� SkyBox ���� Crash �� �߻��ϰ� �ȴ�.
		// ������ ������ crash �߻����� �ʵ��� ������.
        //exit(1);

		ErrMsg += _T("\r\n");
		ErrMsg += _T("\r\n");
		ErrMsg += _T("Please download the latest patch");

		g_strGameForceExit = ErrMsg;
    }
    return 0;
}

// �������� ���۵Ǿ�� ��ȣȭŰ��
int CNetClient::MsgEncryptKey(NET_MSG_GENERIC* nmg)
{
	NET_ENCRYPT_KEY* ncv;
	ncv = reinterpret_cast<NET_ENCRYPT_KEY*> (nmg);
	
	StringCchCopy(m_szEncryptKey, ENCRYPT_KEY+1, ncv->szEncryptKey);
	StringCchCopy(m_szEncryptKeyHeart, ENCRYPT_KEY+1, ncv->szEncryptKey);
	
	return 0;

}
/*
void CNetClient::MsgChaBAInfo(NET_MSG_GENERIC* nmg)
{
	NET_CHA_BBA_INFO_AC* pPacket = (NET_CHA_BBA_INFO_AC*) nmg;
	for (int i=0; i < pPacket->m_ChaServerTotalNum; ++i)
	{
        // ����� ĳ���� ��ȣ:%d", ncbi->nChaNum[i]);
		SndChaBasicInfo(pPacket->m_ChaDbNum[i]);		
	}
}
*/
void CNetClient::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;

	if( IsWorldBattle() )
	{
		// �������忡 �α��� �õ��� �ѹ��� �����ϴ�. ���� ���� ������ �α��� �����Ѵٸ� �ٽ� ���̺꼭���� �����ؼ� ������������ �̵� �ʿ�
		SetWorldBattle(FALSE);
		SetWorldBattleSvrInfoEnd(TRUE);
	}
}

/**
* Login �������� ���۵Ǿ�� Game ���� ����
* \param nmg 
*/
void CNetClient::MsgGameSvrInfo(NET_MSG_GENERIC* nmg)
{
	if (!nmg)
        return;
	
	NET_CUR_INFO_LOGIN* pPacket = reinterpret_cast<NET_CUR_INFO_LOGIN*> (nmg);
	
	if ((pPacket->gscil.nServerGroup >= MAX_SERVER_GROUP) || (pPacket->gscil.nServerNumber >= MAX_CHANNEL_NUMBER))
	{
		return;
	}
	else
	{
		m_sGame[pPacket->gscil.nServerGroup][pPacket->gscil.nServerNumber] = pPacket->gscil;
		TRACE(_T("Received Game Server Information \n"));
	}
}

// ���ο� ���� �ٿ�ε� �޽��� ó�� �Լ�
void CNetClient::MsgNewVersionDown()
{
	// todo : ���α׷� ��ġ���μ���	
	// Test code : �������� �ٿ�ε� ������ ����...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaDownload,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);	
}

void CNetClient::RegisterNewAccount(void)
{
	// Test code : �������� ȸ������ ������ ����...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaRegister,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);
}

// �α��� �ǵ�� ����Ÿ
void CNetClient::MsgLoginFeedBack(NET_MSG_GENERIC* nmg)
{
	NET_LOGIN_FEEDBACK_DATA* pPacket = (NET_LOGIN_FEEDBACK_DATA*) nmg;	

	m_PatchVersion  = pPacket->m_GameVersion;
	m_LauncherVersion = pPacket->m_LauncherVersion ;

	CDebugSet::ToView("NET_MSG_LOGIN_FB, %d", pPacket->m_Result);
	if (m_pGlobalStage)
        m_pGlobalStage->MsgProcess ( nmg );
}
/*
// ä�ø޽���
void CNetClient::MsgChat(NET_MSG_GENERIC* nmg)
{
//	NET_MSG_CHAT_GLOBAL : // ������ �۷ι� �޽���
//	NET_MSG_CHAT_NORMAL : // �Ϲ��� ä�� �޽���
//	NET_MSG_CHAT_PARTY : // ��Ƽ������ ���޵Ǵ� �޽���
//	NET_MSG_CHAT_PRIVATE : // �������� �޽���, �Ӹ�, �ӻ���
//	NET_MSG_CHAT_GUILD : // �������� ���޵Ǵ� �޽���
}
*/
/**
* Message Send Function. 
*/

int CNetClient::SndChaBasicInfoPack( const std::vector<DWORD>& vecChaNum )
{  
    // �޽��� ������ ��ü

    NET_CHA_BA_INFO_CA_PACK SendData;
    {
        SendData.vecChaDbNum = vecChaNum;
    }

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    NET_MSG_PACK MsgPack;
    if (MsgPack.SetData(NET_MSG_REQ_CHA_BINFO_CA, SendBuffer))
        return Send((char*)&MsgPack);
}


// Client �� ���� ������ ������ �����Ѵ�.
int CNetClient::SndVersion(int PatchVersion, int LauncherVersion)
{		
	NET_CLIENT_VERSION_CS ncv(LauncherVersion, PatchVersion);
	return Send(reinterpret_cast<char *> (&ncv));
}

int CNetClient::SndHeartbeat()
{
	NET_HEARTBEAT_CLIENT_ANS msg;

	switch ( m_nClientNetState )
	{
		case NET_STATE_LOGIN : // �α��� ���� ���ӻ���
			return Send(reinterpret_cast<char*> (&msg), msg.Size());
		case NET_STATE_AGENT : // Agent
			{
				m_Tea.encrypt (m_szEncryptKeyHeart, ENCRYPT_KEY+1);

				//	StringCchCopy�� �ٲٸ� �ȵ�. ( �ι��ڿ������� �޸� ī���ؾߵ� )
				memcpy( msg.szEnCrypt,  m_szEncryptKeyHeart, ENCRYPT_KEY+1 );

				return SendToAgent(reinterpret_cast<char*> (&msg), msg.Size());	
			}			
		case NET_STATE_FIELD : // Field
			return SendToField(reinterpret_cast<char*> (&msg), msg.Size());
		default :
			return 0;
	}
}

int	CNetClient::SndVersion(void)
{
	// ���Ϸ� ���� Ŭ���̾�Ʈ ������ �����Ѵ�.
	// if (GetClientVer() == NET_ERROR) 
	//	return NET_ERROR;
	return SndVersion( m_PatchVersion, m_LauncherVersion );
}

// ���Ӽ����� ������ ������ ��û�Ѵ�.
int CNetClient::SndReqServerInfo(void)
{
	{ // �ʱ�ȭ�� ������ ������ ���� ������ ������ �ִ�. UI���� �߸��� ������ ����Ѵ�.
		for ( int i=0; i<MAX_SERVER_GROUP; i++)
		{
			for ( int j=0; j<MAX_SERVER_NUMBER; j++)
			{
				::SecureZeroMemory(&m_sGame[i][j], sizeof(G_SERVER_CUR_INFO_LOGIN));
			}
		}

		m_bGameServerInfoEnd = FALSE;
	}

	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.SetSize( (DWORD) nSize );
	nmg.nType	= NET_MSG_REQ_GAME_SVR;
	return Send( (char *) &nmg, nSize );
}

int CNetClient::SndChaAllBasicInfo()
{
    int LauncherVer = 0;
    int PatchVer = 0;
    GetClientVer(LauncherVer, PatchVer);
	NET_CHA_REQ_BA_INFO MsgCA(LauncherVer, PatchVer);
    MsgCA.m_Crc = sc::string::getCrc32(&MsgCA, NET_CHA_REQ_BA_INFO_DATA_SIZE);
	// ncrbi.nChannel = nChannel;	
	return Send((char*) &MsgCA);
}

// ���λ����Ǵ� ĳ������ ������ �����Ѵ�.
int	CNetClient::SndCreateChaInfo(
	int nIndex,
	WORD wSchool,
	WORD wFace,
	WORD wHair,
	WORD wHairColor,
	WORD wSex,
	const char* strChaName )
{
	NET_NEW_CHA MsgCA;
	MsgCA.nIndex	 = nIndex;
	MsgCA.wSchool	 = wSchool;
	MsgCA.wFace		 = wFace;
	MsgCA.wHair		 = wHair;
	MsgCA.wHairColor = wHairColor;
	MsgCA.wSex		 = wSex;
	::StringCchCopy(MsgCA.szChaName, CHR_ID_LENGTH+1, strChaName);

    MsgCA.m_Crc = sc::string::getCrc32(&MsgCA, EM_NET_NEW_CHA_MSG_DATA_SIZE);

	return Send((char*) &MsgCA);
}

// ����� ������ ��û�Ѵ�.
int CNetClient::SndRequestUserInfo(void)
{	
	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.SetSize( (DWORD) nSize );
	nmg.nType	= NET_MSG_REQ_USER_INFO; // ����� ������û

	return Send((char *) &nmg);
}

//! ������ ����Ű�� ��û�Ѵ�.
//! client->agent
int CNetClient::SndRequestRandomKey(void)
{
	NET_REQ_RAND_KEY msg;	
	return Send((char*) &msg);
}

// 2�����
int CNetClient::SndGameJoin(int ChaDbNum, const char *psz2ndPass, BOOL bNewPass)
{
	NET_LOBBY_CHAR_JOIN_CA Msg(ChaDbNum, psz2ndPass, bNewPass);
	return Send((char*) &Msg);
}
////////////////////////////////////////////////
//
////! client->Agent
////! �ش�ĳ���ͷ� ���ӿ� �����Ѵ�.
//int CNetClient::SndGameJoin(int ChaDbNum)
//{
//	NET_LOBBY_CHAR_JOIN_CA Msg(ChaDbNum);
//	return Send((char*) &Msg);
//}


// �����̺�Ʈ
// ������ 10 �ڸ�
/*
int CNetClient::SndEventLottery(const char* szLotteryName)
{
	NET_EVENT_LOTTERY msg;
	::StringCchCopy(msg.szLotteryName, 11, szLotteryName);

	return Send((char*) &msg);
}
*/
//
//                                       /;    ;\
//                                   __  \\____//
//                                  /{_\_/   `'\____
//                                  \___   (o)  (o  }
//       _____________________________/          :--'   DRINKA
//   ,-,'`@@@@@@@@       @@@@@@         \_    `__\
//  ;:(  @@@@@@@@@        @@@             \___(o'o)
//  :: )  @@@@          @@@@@@        ,'@@(  `===='        PINTA
//  :: : @@@@@:          @@@@         `@@@:
//  :: \  @@@@@:       @@@@@@@)    (  '@@@'
//  ;; /\      /`,    @@@@@@@@@\   :@@@@@)                   MILKA
//  ::/  )    {_----------------:  :~`,~~;
// ;;'`; :   )                  :  / `; ;
//;;;; : :   ;                  :  ;  ; :                        DAY !!!
//`'`' / :  :                   :  :  : :
//    )_ \__;      ";"          :_ ;  \_\       `,','
//    :__\  \    * `,'*         \  \  :  \   *  8`;'*  *
//        `^'     \ :/           `^'  `-^-'   \v/ :  \/   
// jgkim

