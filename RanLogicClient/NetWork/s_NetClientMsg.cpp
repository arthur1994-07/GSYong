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

// 외부에서 강제로 게임을 종료하게 한다.
extern TSTRING g_strGameForceExit;

// 주 메시지 처리 함수
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
		case NET_STATE_LOGIN : // 로그인 서버 접속상태
			MessageProcessLogin(pNmg);
			break;
		case NET_STATE_AGENT : // Agent
		case NET_STATE_FIELD : // Field
		case NET_STATE_BOARD : // Board
			MessageProcessGame(pNmg);
			break;		
		case NET_STATE_CTRL : // 관리프로그램 접속상태
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
	// 게임서버 상태정보
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
	// 버전
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
			// 게임 메시지, 채팅 메시지
			if (nmg->nType > NET_MSG_LOBBY)
			{
				if (m_pGlobalStage)
                    m_pGlobalStage->MsgProcess(nmg);
			}
		}
		break;
	}
}


//! Agent->Client:Field Server 로 접속하라.
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

//! Agent->Client:InstanceServer 로 접속하라.
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

// 서버에서 전송되어온 암호키
void CNetClient::MsgCryptKey(NET_MSG_GENERIC* nmg)
{	
	NET_CRYPT_KEY* nck;
	nck = reinterpret_cast<NET_CRYPT_KEY*> (nmg);
	m_ck = nck->ck;
	m_bKeyReceived = TRUE;
}

// 서버에서 전송되어온 버전 정보
int CNetClient::MsgVersionInfo(NET_MSG_GENERIC* nmg)
{
	NET_CLIENT_VERSION_CS* pPacket = reinterpret_cast<NET_CLIENT_VERSION_CS*> (nmg);
	
	if( pPacket->m_PatchVersion == net::INVALID_COUNTRY && pPacket->m_LauncherVersion == net::INVALID_COUNTRY )
	{
		// GeoIP Block 접속이 허용되지 않은 국가이므로 접속 불가
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
		// exit 를 사용하게 되면 죄도 없는 SkyBox 에서 Crash 가 발생하게 된다.
		// 엉뚱한 곳에서 crash 발생하지 않도록 변경함.
        //exit(1);

		ErrMsg += _T("\r\n");
		ErrMsg += _T("\r\n");
		ErrMsg += _T("Please download the latest patch");

		g_strGameForceExit = ErrMsg;
    }
    return 0;
}

// 서버에서 전송되어온 암호화키값
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
        // 사용자 캐릭터 번호:%d", ncbi->nChaNum[i]);
		SndChaBasicInfo(pPacket->m_ChaDbNum[i]);		
	}
}
*/
void CNetClient::MsgGameSvrInfoEnd(NET_MSG_GENERIC* nmg)
{
	m_bGameServerInfoEnd = TRUE;

	if( IsWorldBattle() )
	{
		// 통합전장에 로그인 시도는 한번만 가능하다. 만약 여러 이유로 로그인 실패한다면 다시 라이브서버에 접속해서 통합전장으로 이동 필요
		SetWorldBattle(FALSE);
		SetWorldBattleSvrInfoEnd(TRUE);
	}
}

/**
* Login 서버에서 전송되어온 Game 서버 정보
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

// 새로운 버전 다운로드 메시지 처리 함수
void CNetClient::MsgNewVersionDown()
{
	// todo : 프로그램 패치프로세서	
	// Test code : 웹브라우저 다운로드 페이지 띄우기...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaDownload,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);	
}

void CNetClient::RegisterNewAccount(void)
{
	// Test code : 웹브라우저 회원가입 페이지 띄우기...
	//ShellExecute(NULL,
	//	"open",
	//	AlphaRegister,
	//	NULL,
	//	NULL,
	//	SW_SHOWNORMAL);
}

// 로그인 피드백 데이타
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
// 채팅메시지
void CNetClient::MsgChat(NET_MSG_GENERIC* nmg)
{
//	NET_MSG_CHAT_GLOBAL : // 관리용 글로벌 메시지
//	NET_MSG_CHAT_NORMAL : // 일반적 채팅 메시지
//	NET_MSG_CHAT_PARTY : // 파티원에게 전달되는 메시지
//	NET_MSG_CHAT_PRIVATE : // 개인적인 메시지, 귓말, 속삭임
//	NET_MSG_CHAT_GUILD : // 길드원에게 전달되는 메시지
}
*/
/**
* Message Send Function. 
*/

int CNetClient::SndChaBasicInfoPack( const std::vector<DWORD>& vecChaNum )
{  
    // 메시지 팩으로 교체

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


// Client 의 버전 정보를 서버로 전송한다.
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
		case NET_STATE_LOGIN : // 로그인 서버 접속상태
			return Send(reinterpret_cast<char*> (&msg), msg.Size());
		case NET_STATE_AGENT : // Agent
			{
				m_Tea.encrypt (m_szEncryptKeyHeart, ENCRYPT_KEY+1);

				//	StringCchCopy로 바꾸면 안됨. ( 널문자열때문에 메모리 카피해야됨 )
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
	// 파일로 부터 클라이언트 버전을 세팅한다.
	// if (GetClientVer() == NET_ERROR) 
	//	return NET_ERROR;
	return SndVersion( m_PatchVersion, m_LauncherVersion );
}

// 게임서버의 정보를 서버에 요청한다.
int CNetClient::SndReqServerInfo(void)
{
	{ // 초기화를 해주지 않으면 이전 정보를 가지고 있다. UI에서 잘못된 정보를 출력한다.
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

// 새로생성되는 캐릭터의 정보를 전송한다.
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

// 사용자 정보를 요청한다.
int CNetClient::SndRequestUserInfo(void)
{	
	int nSize;
	NET_MSG_GENERIC nmg;
	nSize		= sizeof(NET_MSG_GENERIC);
	nmg.SetSize( (DWORD) nSize );
	nmg.nType	= NET_MSG_REQ_USER_INFO; // 사용자 정보요청

	return Send((char *) &nmg);
}

//! 서버에 랜덤키를 요청한다.
//! client->agent
int CNetClient::SndRequestRandomKey(void)
{
	NET_REQ_RAND_KEY msg;	
	return Send((char*) &msg);
}

// 2차비번
int CNetClient::SndGameJoin(int ChaDbNum, const char *psz2ndPass, BOOL bNewPass)
{
	NET_LOBBY_CHAR_JOIN_CA Msg(ChaDbNum, psz2ndPass, bNewPass);
	return Send((char*) &Msg);
}
////////////////////////////////////////////////
//
////! client->Agent
////! 해당캐릭터로 게임에 조인한다.
//int CNetClient::SndGameJoin(int ChaDbNum)
//{
//	NET_LOBBY_CHAR_JOIN_CA Msg(ChaDbNum);
//	return Send((char*) &Msg);
//}


// 복권이벤트
// 복권은 10 자리
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

