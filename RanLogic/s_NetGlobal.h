#ifndef S_NETGLOBAL_H_
#define S_NETGLOBAL_H_

#pragma once

#ifndef FD_SETSIZE
#define FD_SETSIZE      1024
#endif /* FD_SETSIZE */

//! \ingroup NetServerLib
//! \date 2002.05.30
#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

#include <winsock2.h>
#include <atltime.h>
#include <Mmsystem.h>
#include <queue>

#include "../=MsgPack/include/msgpack.hpp"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include "../SigmaCore/Net/NetDefine.h"
#include "../SigmaCore/Compress/MinLzo.h"
#include "../SigmaCore/String/StringDefine.h"
#include "../EngineLib/G-Logic/GLDefine.h"
#include "../RanLogic/WorldBattle/WorldBattleDefine.h"
#include "./Network/NetLogicDefine.h"
#include "./Character/GLCharDefine.h"
#include "./Party/GLPartyDefine.h"
#include "./InstanceSystem/GLInstanceSystemDefine.h"

#include "../RanLogic/msg/MessageEnumMixer.h"

#ifndef _NP30
	#include "../=nProtect/ggsrv25.h"
	#pragma comment(lib, "../=nProtect/ggsrvlib25_32_MD.lib") // 프로젝트-속성-링커-입력-추가종속성에서  "$(SolutionDir)=nProtect/ggsrvlib25_32_MD.lib"  제외하고 소스에서 추가하도록 적용
#else
	#include "../=nProtect/ggsrv30.h"
	#include "../=nProtect/NPGameLib_Ver30_20140731.h"

	#pragma comment(lib, "../=nProtect/ggsrv30lib_x86_MD.lib")
#endif


//#ifndef _DEBUG
//    // nProtect GameGuard Server module
//    #include "../=nProtect/ggsrv25.h"
//    //#include "./Util/nProtect/ggsrv.h"
//    //#pragma comment(lib, "../=Library/ggsrvlib.lib")
//#else
//    //#include "../=Include/nprotect.h"
//    #include "../=nProtect/ggsrv25.h"
//#endif

// Apex RSA
#include "./Util/gamecode.h"

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE
#include <strsafe.h> // Safe string function

// Auto Link Library Files
//#pragma comment (lib, "ws2_32.lib") // Windows socket2 library
//#pragma comment (lib, "Winmm.lib") // Windows multimedia library
//#pragma comment (lib, "strsafe.lib") // Safe string library

static sc::CMinLzo* g_pMinLzo;

// DELETE Function definition
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif


//mjeon.AF

#define DBACTION_CLIENT_ID 0
#define CLIENTID_NULL      MAXDWORD

//AgentServer info
struct A_SERVER_INFO
{
	char szServerName[SERVER_NAME_LENGTH+1];
	char szPublicIp[MAX_IP_LENGTH+1];
    unsigned long ulPublicIp;
	int	nServicePort;
    bool bLoad;
    DWORD NetSlotNum;

	A_SERVER_INFO()
        :bLoad(false)
        ,ulPublicIp(INADDR_NONE)
		,nServicePort(0)
        ,NetSlotNum( net::INVALID_SLOT)
	{
        memset(szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1));
        memset(szPublicIp, 0, sizeof(char) * (MAX_IP_LENGTH+1));        
	}

    A_SERVER_INFO& operator = (const A_SERVER_INFO& rvalue)
    {
        StringCchCopyA(szServerName, SERVER_NAME_LENGTH+1, rvalue.szServerName);
        StringCchCopyA(szPublicIp, MAX_IP_LENGTH+1, rvalue.szPublicIp);
        ulPublicIp = rvalue.ulPublicIp;   
		nServicePort = rvalue.nServicePort;
        bLoad = rvalue.bLoad;
        NetSlotNum = rvalue.NetSlotNum;

        return *this;
    }

	friend bool operator == (const A_SERVER_INFO& lhs, const A_SERVER_INFO& rhs)
	{
		if ( ( ::strcmp( lhs.szPublicIp, rhs.szPublicIp ) == 0 ) &&
			( lhs.nServicePort == rhs.nServicePort ) )
			return true;
		else
			return false;
	}

	bool IsValid()
	{
		return ( szServerName[0] && 
			szPublicIp[0] && 
			nServicePort && 
			(ulPublicIp!=INADDR_NONE) );
	}
};
typedef A_SERVER_INFO* LPA_SERVER_INFO;

//! Field Server Information
struct F_SERVER_INFO
{			
	char szServerName[SERVER_NAME_LENGTH+1];
	char szPublicIp[MAX_IP_LENGTH+1];
    unsigned long ulPublicIp;
    char szPrivateIP[MAX_IP_LENGTH+1];
	unsigned long ulPrivateIP;
	int	nServicePort;
    int bPk;
	bool bCreateInstanceLand;
    bool bLoad;
    DWORD NetSlotNum;

	F_SERVER_INFO()
        : bPk(false)
		, bCreateInstanceLand(true)
        , bLoad(false)        
        , ulPublicIp(INADDR_NONE)
        , ulPrivateIP(INADDR_NONE)
        , nServicePort(0)        
        , NetSlotNum(net::INVALID_SLOT)
	{
        memset(szServerName, 0, sizeof(char) * (SERVER_NAME_LENGTH+1));
        memset(szPublicIp, 0, sizeof(char) * (MAX_IP_LENGTH+1));
        memset(szPrivateIP, 0, sizeof(char) * (MAX_IP_LENGTH+1));
	}

    F_SERVER_INFO& operator = (const F_SERVER_INFO& rvalue)
    {
        StringCchCopyA(szServerName, SERVER_NAME_LENGTH+1, rvalue.szServerName);
        StringCchCopyA(szPublicIp, MAX_IP_LENGTH+1, rvalue.szPublicIp);
        ulPublicIp = rvalue.ulPublicIp;
        StringCchCopyA(szPrivateIP, MAX_IP_LENGTH+1, rvalue.szPrivateIP);
        ulPrivateIP = rvalue.ulPrivateIP;
        nServicePort = rvalue.nServicePort;
        bPk = rvalue.bPk;
		bCreateInstanceLand = rvalue.bCreateInstanceLand;
        bLoad = rvalue.bLoad;
        NetSlotNum = rvalue.NetSlotNum;

        return *this;
    }

	friend bool operator == (const F_SERVER_INFO& lhs, const F_SERVER_INFO& rhs)
	{
		if ((::strcmp(lhs.szPublicIp, rhs.szPublicIp) == 0) &&(lhs.nServicePort == rhs.nServicePort))
			return true;
		else
			return false;
	}

	bool IsValid()
	{
		return (szServerName[0] && szPublicIp[0] && nServicePort && (ulPublicIp!=INADDR_NONE));
	}
};
typedef F_SERVER_INFO* LPF_SERVER_INFO;

struct CHANNEL_INFO
{
	bool bPK;

	CHANNEL_INFO()
	{
		bPK = true;
	};
};
typedef CHANNEL_INFO* LPCHANNEL_INFO;

// Login information
enum EMNET_MSG
{
	NET_MSG_BASE_ZERO           = 0,

	///////////////////////////////////////////////////////////////////////////
	// Version check and downloading new version
	// Do not change version and Crypt key message
	NET_MSG_VERSION_OK			= 100, //! 버전이 일치함
	NET_MSG_CLIENT_VERSION_CS	= 110, //! 버전 정보를 보냄
    NET_MSG_VERSION_LS          = 111, //! Login->Session:버전 정보를 보냄
    NET_MSG_VERSION_SS          = 112, //! Session->Server:버전 정보를 보냄
    NET_MSG_CLIENT_VERSION_SC	= 113, //! Login->Client 버전 정보를 보냄
    NET_MSG_CLIENT_VERSION_AC   = 114, //! Agent->Client 버전 정보를 보냄
	NET_MSG_VERSION_REQ			= 120, //! 버전 정보를 요청함
	NET_MSG_REQ_CRYT_KEY		= 130, //! 클라이언트->게임서버 : 암호키 요청

    NET_MSG_CS_AES_REQ_SERVER_PUB_KEY = 131, //! Aes

	NET_MSG_SND_CRYT_KEY		= 140, //! 게임서버->클라이언트 : 암호키 전송	
	NET_MSG_RANDOM_NUM          = 141, //! Agent->Client : 랜덤넘버
//	NET_MSG_PING_REQUEST		= 150, //! 서버로의 ping 을 요청한다.
//	NET_MSG_PING_ANSWER		    = 151, //! 서버에서 ping 을 응답한다.
	
    NET_MSG_HEARTBEAT_SERVER_REQ= 153, //! SERVER->SERVER Server HeartBeat Request
    NET_MSG_HEARTBEAT_SERVER_ANS= 154, //! SERVER->SERVER Server HeartBeat Answer
    NET_MSG_SVR_DOWN            = 155, //! Server System Down!
	NET_MSG_SVR_CLOSECLIENT     = 156, //! Server Close Client	
	NET_MSG_HEARTBEAT_CLIENT_REQ= 160, //! SERVER->CLIENT Client HeartBeat Request
	NET_MSG_HEARTBEAT_CLIENT_ANS= 161, //! CLIENT->SERVER Client HeartBeat Answer
	NET_MSG_COMPRESS            = 170, //! Compressed message	
    
	///////////////////////////////////////////////////////////////////////////	
	// Server info message

	//================================== 기준점 ==================================
	NET_MSG_LGIN				= 171, //! 171 
	//============================================================================
	
	NET_MSG_REQ_GAME_SVR		= 172, //! 클라이언트가 -> 로그인서버 : 게임 서버의 정보를 요청
	NET_MSG_SND_GAME_SVR		= 173, //! 로그인서버   -> 클라이언트 : 게임 서버의 정보를 전송
	NET_MSG_SND_GAME_SVR_END	= 174, //! 로그인서버   -> 클라이언트 : 게임 서버 정보 전달 끝
	NET_MSG_REQ_FULL_SVR_INFO	= 175, //! 세션서버 -> 로그인서버, 게임서버 : 서버 전체 정보를 요청한다.
	NET_MSG_SND_FULL_SVR_INFO	= 176, //! 로그인서버, 게임서버 -> 세션서버 : 서버 전체 정보를 Session 서버에 전송한다.
	NET_MSG_REQ_CUR_STATE		= 177, //! 세션서버 -> 로그인서버, 게임서버 : 현재 서버상태= (간략한),를 요청한다.
	NET_MSG_SND_CUR_STATE		= 178, //! 로그인서버, 게임서버 -> 세션서버 : 현재 서버상태= (간략한),를 전송한다.
	NET_MSG_SND_CHANNEL_STATE	= 179, //! Agent->Session : 현재 채널상태를 전송한다.
	NET_MSG_REQ_SVR_INFO		= 180, //! 로그인서버 -> 세션서버 : 게임서버의 상태정보를 요청한다.
	NET_MSG_SND_SVR_INFO		= 181, //! 세션서버 -> 로그인서버 : 게임서버의 상태정보를 전송한다.
	NET_MSG_SND_SVR_INFO_RESET	= 182, //! 세션서버 -> 로그인서버 : 게임서버의 상태정보를 리셋하라고 명령.
	NET_MSG_REQ_ALL_SVR_INFO	= 183, //! 관리프로그램->세션서버 : 모든 서버의 상태정보를 요청한다.
	NET_MSG_SND_ALL_SVR_INFO	= 184, //! 세션서버->관리프로그램 : 모든 서버의 상태정보를 전송한다.
	NET_MSG_REQ_AUTH_SVR_INFO	= 185, 
	NET_MSG_SND_AUTH_SVR_INFO	= 186,
	NET_MSG_SND_ALL_SVR_INFO_S	= 187, //! 세션서버->관리프로그램 : 상태정보전송 시작
	NET_MSG_SND_ALL_SVR_INFO_E	= 188, //! 세션서버->관리프로그램 : 상태정보를 모두 보냈음을 알린다.
	NET_MSG_RECONNECT_FIELD		= 189, //! 세션서버->에이전트 : 필드로 다시 접속하라.
	NET_MSG_I_AM_AGENT			= 190, //! Agent->(Field or Match): 자신이 서버통신용 슬롯임을 알림	
	NET_MSG_NEW_INSTANCE		= 191, //! Instance->Match: 인스턴스 서버 등록
	NET_MSG_RECONNECT_MATCH		= 192, //! 세션서버->에이전트 : Match로 다시 접속하라.

	NET_MSG_LOGOUT_G_S			= 193, //! Agent->Session : 로그아웃 정보	
	NET_MSG_LOGOUT_G_S_S		= 194, //! Agent->Session : 로그아웃 정보. 사용자 ID 만 전송
	DAUM_NET_MSG_LOGOUT_G_S		= 195, //! Agent->Session : Daum 로그아웃 정보

	//! User Information message
	NET_MSG_REQ_USER_INFO		= 196, //! 사용자 정보를 요청한다.
	NET_MSG_USER_INFO			= 197, //! 사용자 정보를 전송한다.

	//! Server control message	
	NET_MSG_SVR_CMD				= 198,
	//NET_MSG_SVR_PAUSE, //! 서버를 잠시 멈춘다.
	//NET_MSG_SVR_RESUME, //! 멈추어진 서버를 다시 가동시킨다.	
	NET_MSG_SVR_RESTART			= 199, //! 서버를 완전히 멈추고 새롭게 가동시킨다.
	NET_MSG_SVR_STOP			= 200, //! 서버를 완전히 멈춘다.
	NET_MSG_SND_CHANNEL_FULL_STATE	= 201, //! 서버의 채널 FULL 상태를 LoginServer에 보낸다.

    NET_MSG_CLOSE_CLIENT_SESSION	= 202, //! Session:Client socket closed

	
	// ============ 인증서버 메세지 때문에 아래의 프로토콜 번호는 반드시 유지 ================
	// for global auth
	NET_MSG_CLOSE_CLIENT_AUTH			= 203,
	NET_MSG_AUTH_CERTIFICATION_REQUEST	= 204,
	NET_MSG_AUTH_CERTIFICATION_ANS		= 205,
	NET_MSG_AUTH_STATE_REQ				= 206,
	NET_MSG_AUTH_STATE_ANS				= 207,
	NET_MSG_AUTH_EVENT_INFO_REQ			= 208,
	NET_MSG_AUTH_EVENT_INFO_ANS			= 209,
	NET_MSG_AUTH_SERVER_STATE_REQ		= 210,
	NET_MSG_AUTH_SERVER_STATE_ANS		= 211,

	NET_MSG_COUNTRY_ACCESS_APPROVE		= 212,

	/////////////////////////////////////////////////////////////////////////////////////////
	// 추가되는 프로토콜들은 반드시 아래 기준점 이후로 추가해야함
	//////////////////////////////////////////////////////////////////////////////////////////
	//================================== 기준점 ==================================
	//
	NET_MSG_LGIN_MAX	= 213,
	NET_MSG_LOBBY		= 214,
	
	//============================================================================
	// 패킷 Enum 랜덤 변경 기능으로 인해서 서버매니져에서 사용하는 패킷중에서 게임관련 패킷을 이쪽으로 추가 ( 기존패킷은 게임서버에서 사용할수 있기때문에 그대로 유지 ) - 2014.12.18
	NET_MSG_INSPECTION_REWARD_SET_REQ				= 215,	//기간제 아이템 시간 연장
	NET_MSG_INSPECTION_REWARD_SET_ACK				= 216,
	NET_MSG_CHAT_CTRL_SVRMANAGER					= 217,
	NET_MSG_EVENT_EXP_MS							= 218,
	NET_MSG_EVENT_EXP_END_MS						= 219,
	NET_MSG_UPDATE_TRACING_ALL						= 220,
	NET_MSG_UPDATE_TRACING_CHARACTER				= 221,
	

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 지금부터 아래에 추가되는 패킷은 Enum 값이 빌드마다 랜덤으로 변경됨	!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//!-----------------------------------------------------------------------------------------------------[LOBBY]	
	// NET_MSG_LOGIN, //! 로그인 
	NET_MSG_LOGIN_2, //! 로그인
	NET_MSG_LOGIN_FB, //! 로그인 결과	
    NET_MSG_LOGIN_FEEDBACK_DATA2, //! 
	NET_MSG_LOGIN_FB_SUB_OK_FIELD, //! 로그인성공결과를 필드서버로 전송해서 세팅하게 한다.		

	
	DAUM_NET_MSG_LOGIN, //! Daum 로그인
	DAUM_NET_MSG_LOGIN_FB, //! Daum 로그인 결과 Agent->Client	

	MET_MSG_SVR_FULL, //! 서버 Full

	CHINA_NET_MSG_LOGIN, //! China 로그인
	CHINA_NET_MSG_LOGIN_FB, //! China 로그인 결과 Agent->Client

	GSP_NET_MSG_LOGIN, //! GSP 로그인
	GSP_NET_MSG_LOGIN_FB, //! GSP 로그인 결과 AGent->Client

	IDN_NET_MSG_LOGIN, //mjeon.indonesia
	IDN_NET_MSG_LOGIN_FB,

	TERRA_NET_MSG_LOGIN, //! Terra 로그인
	TERRA_NET_MSG_LOGIN_FB, //! Terra 로그인 결과 Agent->Client

	TERRA_NET_MSG_PASSCHECK, //! Terra Password Check
	DAUM_NET_MSG_PASSCHECK, //! Daum Password Check
	NET_MSG_PASSCHECK_FB, //! Password Check 결과 Agent->Client
	EXCITE_NET_MSG_PASSCHECK, //! Excite Password Check

	EXCITE_NET_MSG_LOGIN, //! Excite 로그인
	EXCITE_NET_MSG_LOGIN_FB, //! Excite 로그인 결과 Agent->Client

	JAPAN_NET_MSG_LOGIN, //! 일본(Gonzo) 로그인
	JAPAN_NET_MSG_LOGIN_FB, //! 일본(Gonzo) 로그인 결과 Agent->Client
	JAPAN_NET_MSG_UUID, //! 일본(Gonzo) 로그인 UUID

	GS_NET_MSG_WB_LOGIN, // GS 로그인전에 WB 접속 여부를 확인하기 위해서 전송
	GS_NET_MSG_WB_LOGIN_AW, // GS 로그인전에 WB 접속 여부를 확인하기 위해서 전송
	GS_NET_MSG_WB_LOGIN_WA, // GS 로그인전에 WB 접속 여부를 확인하기 위해서 전송
	GS_NET_MSG_WB_LOGIN_FB, //! 로그인 결과	
	GS_NET_MSG_LOGIN, //! GS 로그인
	GS_NET_MSG_LOGIN_FB, //! GS 로그인 결과 Agent->Client


	THAI_NET_MSG_LOGIN, //! Thailand 로그인
	THAI_NET_MSG_LOGIN_FB, //! Thailand 로그인 결과 Agent->Client

	GM_NET_MSG_LOGIN, //! GM 로그인
	GM_NET_MSG_LOGIN_FB, //! GM 로그인 결과 Agent->Client

	US_NET_MSG_LOGIN, //! US 로그인
	US_NET_MSG_LOGIN_FB, //! US 로그인 결과 Agent->Client

	NET_MSG_REQ_RAND_KEY, //! 랜덤키 요청 Client->Agent
	
	NET_MSG_SND_ENCRYPT_KEY, //! 로그인 랜덤키 전송 Login->Session

    NET_MSG_CLOSE_CLIENT, //! 클라이언트 강제 접속 종료시킴;

	//! Character create message
	//NET_MSG_CHA_INFO, //! 캐릭터 정보
	NET_MSG_CHA_NEW, //! 새로운 캐릭터 생성
	NET_MSG_CHA_NEW_FB, //! 캐릭터 생성 성공/실패 메시지
	
	NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	NET_MSG_CHA_DEL_FB, //! Agent->Client : 케릭터 삭제 결과
	NET_MSG_CHA_DEL_FB_OK, //! Agent->Client : 케릭터 삭제 성공
	NET_MSG_CHA_DEL_FB_ERROR, //! Agent->Client : 케릭터 삭제 실패
	DAUM_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
    NET_MSG_CHA_DEL_FB_CLUB, //! Agent->Client : 케릭터 삭제 실패(클럽마스터이기 때문에 삭제가 되지 않는다)
	TERRA_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	EXCITE_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	GSP_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	JAPAN_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	GS_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제
	IDN_NET_MSG_CHA_DEL, //! Client->Agent : 케릭터 삭제

	NET_MSG_CHA_DECREASE, //! Agent->Session : 캐릭터 생성, 갯수 감소
	NET_MSG_TEST_CHA_DECREASE, //! Agent->Session : 캐릭터 생성, 갯수 감소
	DAUM_NET_MSG_CHA_DECREASE, //! DAUM : Agent->Session : 캐릭터 생성, 갯수 감소
	DAUM_NET_MSG_TEST_CHA_DECREASE, //! DAUM : Agent->Session : 캐릭터 생성, 갯수 감소

	NET_MSG_CHA_INCREASE, //! Agent->Session : 캐릭터 삭제, 갯수 증가
	NET_MSG_TEST_CHA_INCREASE, //! Agent->Session : 캐릭터 삭제, 갯수 증가
	DAUM_NET_MSG_CHA_INCREASE, //! DAUM : Agent->Session : 캐릭터 삭제, 갯수 증가
	DAUM_NET_MSG_TEST_CHA_INCREASE,//! DAUM : Agent->Session : 캐릭터 삭제, 갯수 증가

	//NET_MSG_REQ_CHA_ALL_INFO, //! 모든 캐릭터의 정보를 요청한다.
	//NET_MSG_REQ_CHA_INFO, //! 특정 캐릭터의 정보를 요청한다.
	NET_MSG_REQ_CHA_BINFO_CA, //! 특정 캐릭터의 기본= (캐릭터선택화면),정보 요청한다. // 279
	//NET_MSG_REQ_CHA_FEEDBACK, //! 캐릭터 정보 피드백데이터
	//NET_MSG_CHA_INFO_LOGIN, //! 로그인서버에서 클라이언트에게 넘겨주는 캐릭터 정보 
	NET_MSG_REQ_CHA_BAINFO, //! 클라이언트->게임서버 : 캐릭터 기초정보 요청 // 280
	NET_MSG_CHA_BAINFO_AC, //! Agent->Client : 사용자 캐릭터 DB 번호 정보 // 281
    NET_MSG_CHA_BAINFO_DH, //! Database->Cache : 사용자 캐릭터 DB 번호 정보
    NET_MSG_CHA_BAINFO_HA, //! Cache->Agent : 사용자 캐릭터 DB 번호 정보
	NET_MSG_CHA_BAINFO_DA, //! DB->Agent : 아이템 변환을 위해서 잠시 메시지 릴레이를 한다.
    NET_MSG_LOBBY_CHAR_INFO_AH, //! Agent->Cache : 캐릭터 Lobby Display 용 정보 요청
	NET_MSG_LOBBY_CHAR_INFO_AH_END, 
    NET_MSG_LOBBY_CHAR_INFO_DH, //! Database->Cache : 캐릭터 Lobby Display 용 정보
    NET_MSG_LOBBY_CHAR_INFO_HA, //! Cache->Agent : 캐릭터 Lobby Display 용 정보
	NET_MSG_LOBBY_CHAR_INFO_HA_END, //! Cache->Agent : 캐릭터 Lobby Display 용 정보
    NET_MSG_LOBBY_USER_INFO_DH, //! Database->Cache : 유저 Lobby Display 용 정보
    NET_MSG_LOBBY_CHAR_INFO_DH2, //! Database->Cache : 캐릭터 Lobby Display 용 정보
    NET_MSG_LOBBY_USER_INFO_END_DH, //! Database->Cache : 유저 Lobby Display 용 캐릭터 정보 끝
    NET_MSG_LOBBY_NEW_CHAR_INFO_DH, //! Database->Cache : 캐릭터 Lobby Display 용 정보
	

	NET_MSG_LOBBY_CHARINFO_AC,  //! (AC) 캐릭터 선택용 정보. // 291
	NET_MSG_LOBBY_CHARINFO_AC_END,  //! (AC) 캐릭터 선택용 정보. // 291
	NET_MSG_LOBBY_CHAR_JOIN,    //! 필드서버->클라이언트 : 선택된 캐릭터 정보를 보냄.
	NET_MSG_LOBBY_CHAR_ITEM,    //! 필드서버->클라이언트 : 선택된 캐릭터 인벤토리 아이템을 보냄.
	NET_MSG_LOBBY_CHAR_DEFAULT, //! 필드서버->클라이언트 : 설정을 보냄.
	NET_MSG_LOBBY_CHAR_JOIN_FB, //! 필드서버->클라이언트 : 선택된 캐릭터의 접속참가 실패 메시지.
	NET_MSG_LOBBY_CHAR_SKILL,   //! 필드서버->클라이언트 : 선택된 캐릭터가 배운 스킬 정보 전송.
	NET_MSG_LOBBY_QUEST_END,    //! 퀘스트 완료 정보.
	NET_MSG_LOBBY_QUEST_PROG,   //! 퀘스트 진행 정보.
	NET_MSG_LOBBY_CHARGE_ITEM,  //! 구입 아이템 전송.
	//NET_MSG_LOBBY_CHAR_PUTON, //! 필드서버->클라이언트 : 선택된 캐릭터 착용 아이템을 보냄.
	
	NET_MSG_CHARPOS_FROMDB2AGT, //!	캐릭터 위치 DB 저장 결과
	
	NET_MSG_LOBBY_ITEM_COOLTIME, //! 아이템 쿨타임

	NET_MSG_LOBBY_INSTANCE_LIST, //! 인던 리스트;
	
	NET_MSG_LOBBY_CHAR_JOIN_CA,    //! 로비에서 선택한 캐릭터로 게임접속	// 303
	NET_MSG_LOBBY_GAME_COMPLETE,   //! 클라이언트->클라이언트 : 게임에 들어갈 정보를 모두 받아서 게임에 들어감.
	NET_MSG_GAME_JOIN_OK,          //! 필드서버    ->세션서버 : 해당 캐릭터가 조인성공
	MET_MSG_GAME_JOIN_FIELDSVR,    //! 에이전트서버->필드서버 : 해당캐릭터가 게임에 조인요청
	MET_MSG_GAME_JOIN_FIELDSVR_FB, //! 필드서버->에이전트서버 : 선택된 캐릭터로 조인함	
	MET_MSG_JOIN_INSTANCESVR_INFO,

	NET_MSG_CONNECT_CLIENT_FIELD, //! 에이전트서버->클라이언트   : 해당필드서버로 접속 명령
	NET_MSG_JOIN_FIELD_IDENTITY,  //! 클라이언트  ->필드서버     : 새로운 Field 서버에 접속후 자신의 인식자 전송.

	NET_MSG_LOBBY_REQ_GAME_JOIN,  //! 클라이언트  ->클라이언트   : 인터페이스로 선택된 케릭으로 접속을 명령하기 위해서.
	
	NET_MSG_AGENT_REQ_JOIN, //! 에이젼트DB ->에이젼트   : db에서 읽은 char로 접속시도.
	NET_MSG_FIELD_REQ_JOIN, //! 필드DB		->필드서버   : db에서 읽은 char로 접속시도.

	//NET_MSG_EVENT_LOTTERY, //! 클라이언트  ->에이전트   : 복권입력
	//NET_MSG_EVENT_LOTTERY_FB, //! 에이전트    ->클라이언트 : 복권결과

	NET_MSG_LOBBY_CHARJOIN_2NDPASS, // 2차비번 // 캐릭터 선택시 2차비번을 같이 보내는데 비번이 오류 또는 실패인경우
	NET_IN_MSG_CHAJOIN_2NDPASSWORD, // 2차비번


	NET_MSG_LOBBY_CHINA_ERROR, //! 중국 로그인시 플레이 누적시간이 5시간이 넘고 오프라인 누적시간이 5시간이 되지 않았을 경우 에러 메시지

	NET_MSG_LOBBY_CHAR_VIETNAM_ITEM, //! 베트남 인벤토리 아이템 전송 메시지

	NET_MSG_LOBBY_CHAR_PUTON_EX, //! 필드서버->클라이언트 : 선택된 캐릭터 착용 아이템을 보냄. ( 한개씩 보냄 )

    NET_MSG_LOGIN_WB_CA, //! World battle login

	// User System Info
	NET_MSG_USER_SYSTEM_INFO_CA,					// 유저들의 System 정보를 수집한다. // 323

	/// Country - 패킷 모니터링을 위하여 모두 한곳에 정의;
	NET_MSG_GCTRL_COUNTRY_CHANGE_CA,
	NET_MSG_GCTRL_COUNTRY_CHANGE_AFC,
	/// ---------------------------------------------------------------- ///

	//================================== 기준점 ==================================
	NET_MSG_LOBBY_MAX,
	NET_MSG_GCTRL,
	//============================================================================

	//!-----------------------------------------------------------------------------------------------------[GAME CONTRL]
	
	NET_MSG_COMBINE,  //! 병합 메시지. ( 필드 서버 -> 클라이언트 )
    NET_MSG_JSON,  //! Json message

	///////////////////////////////////////////////////////////////////////////
	// nProtect
	NET_MSG_GAMEGUARD_AUTH, //! nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER, //! nProtect GameGuard Auth Answer 메시지
	NET_MSG_GAMEGUARD_AUTH_1, //! nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER_1, //! nProtect GameGuard Auth Answer 메시지
	NET_MSG_GAMEGUARD_AUTH_2, //! nProtect GameGuard Auth 요청 메시지
	NET_MSG_GAMEGUARD_ANSWER_2, //! nProtect GameGuard Auth Answer 메시지

    NET_MSG_KOR_SHUTDOWN_WARNING_AC, //! 한국 셧다운 Shutdown

	// Apex 적용
	NET_MSG_APEX_DATA, //! Apex Data 메세지
	NET_MSG_APEX_ANSWER, //! Apex Answer 메세지
//	NET_MSG_APEX_RSAKEY, //! Apex RSA Key 메세지
	// Apex 적용( 홍콩 )
	NET_MSG_APEX_RETURN, //! Apex Client Return 	

	NET_MSG_REQ_FRIENDLIST,     //! 친구 목록 전송 요청.
	NET_MSG_REQ_FRIENDADD,      //! 친구 추가 요청.
	NET_MSG_REQ_FRIENDADD_LURE, //! 친구 추가 문의.
	NET_MSG_REQ_FRIENDADD_ANS,  //! 친구 추가 수락 여부 회신.
	NET_MSG_REQ_FRIENDADD_FB,   //! 친구 추가 요청 결과.
	NET_MSG_REQ_FRIENDDEL,      //! 친구 삭제 요청.
	NET_MSG_REQ_FRIENDDEL_FB,   //! 친구 삭제 결과.

	NET_MSG_FRIENDINFO, //! 친구 정보.
	NET_MSG_FRIENDSTATE, //! 친구 상태.

	NET_MSG_REQ_FRIENDBLOCK, //! 귓속말 블럭 요청.
	NET_MSG_REQ_FRIENDBLOCK_FB, //! 귓속말 블럭 요청 결과.

	NET_MSG_CHAT_CTRL, //! 채팅메시지 정의 ( 클라이언트->게임서버 )

//	NET_MSG_CHAT, //! 채팅메시지 정의 ( 클라이언트->게임서버 )
//	NET_MSG_CHAT_FB, //! 채팅메시지 정의 ( 게임서버->클라이언트 )

	NET_MSG_CHAT_PRIVATE_FAIL, //! 채팅, 귓속말 보내기에 실패하였을 경우. ( 게임서버->클라이언트 )	

	NET_MSG_CHAT_LOUDSPEAKER, //! 확성기 메시시.
	NET_MSG_CHAT_LOUDSPEAKER_AGT, //! 확성기 메시지 agt 에 보냄.
	NET_MSG_CHAT_LOUDSPEAKER_FB, //! 확성기 메시지 처리 FB.

	NET_MSG_CHAT_BLOCK, //! 채팅 차단 상태일 경우. ( 게임서버 -> 클라이언트 )

	NET_MSG_CHAT_LINK, //! 채팅메시지 정의 ( 클라이언트->필드서버 )
	NET_MSG_CHAT_LINK_FB, //! 채팅메시지 정의 ( 필드서버->클라이언트 )

	NET_MSG_FIELDSVR_CHARCHK, //! 필드서버에 케릭터 진입전에 잔존 CHAR 제거 위한 메시지 전송.
	NET_MSG_FIELDSVR_CHARCHK_FB, //! 필드서버에 케릭터 진입전에 잔존 CHAR 제거 위한 메시지 전송.

	NET_MSG_MOVEMAP_AF, //! 필드서버에서 나오기 위한 예비 처리를 부탁. ( 필드서버 이동시. )
	NET_MSG_FIELDSVR_OUT_FB, //! 필드서버에서 나오기 위한 예비 처리 결과. ( 필드서버 이동시. )

	NET_MSG_DROP_ITEM,
	NET_MSG_DROP_MONEY,

	NET_MSG_GCTRL_DROP_PC_BASE,
    NET_MSG_GCTRL_DROP_PC_SKILLFACT,
    NET_MSG_GCTRL_DROP_PC_PUT_ON_ITEMS,
    NET_MSG_GCTRL_DROP_PC_END,
	NET_MSG_GCTRL_DROP_CROW,
	NET_MSG_GCTRL_DROP_MATERIAL,

	NET_MSG_GCTRL_DROP_OUT,
	NET_MSG_GCTRL_DROP_OUT_FORCED,

	NET_MSG_GCTRL_REQ_GATEOUT_REQ,
	NET_MSG_GCTRL_REQ_GATEOUT,
	NET_MSG_GCTRL_REQ_GATEOUT_FB,
	NET_MSG_GCTRL_REQ_LANDIN,

	NET_MSG_GCTRL_REQ_READY,

	NET_MSG_GCTRL_LAND_INFO,
	NET_MSG_GCTRL_SERVER_INFO,
	NET_MSG_GCTRL_SCHOOLFREEPK_FLD,
	NET_MSG_GCTRL_SERVER_PKEVENT_INFO,

	NET_MSG_GCTRL_ACTSTATE,
	NET_MSG_GCTRL_MOVESTATE,
	NET_MSG_GCTRL_MOVESTATE_BRD,

	NET_MSG_GCTRL_GOTO,
	NET_MSG_GCTRL_GOTO_BRD,

	NET_MSG_GCTRL_ANIMOVE_START_CF,		// AnimationMove
	NET_MSG_GCTRL_ANIMOVE_END_CF,		// AnimationMove
	NET_MSG_GCTRL_ANIMOVE_START_BRD,	// AnimationMove

	NET_MSG_GCTRL_ATTACK,
	NET_MSG_GCTRL_ATTACK_BRD,
	NET_MSG_GCTRL_ATTACK_CANCEL,
	NET_MSG_GCTRL_ATTACK_CANCEL_BRD,
	NET_MSG_GCTRL_ATTACK_AVOID,
	NET_MSG_GCTRL_ATTACK_AVOID_BRD,
	NET_MSG_GCTRL_ATTACK_DAMAGE,
	NET_MSG_GCTRL_ATTACK_DAMAGE_BRD,
	NET_MSG_GCTRL_ATTACK_HEAL,
	NET_MSG_GCTRL_ATTACK_HEAL_BRD,

	NET_MSG_GCTRL_ACTION_BRD,	//!	행동 변화.
	NET_MSG_GCTRL_UPDATE_STATE,	//!	상태 변화 업대이트.
	NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE,	//! 고용 소환수 GaeaID 업데이트;
	NET_MSG_GCTRL_UPDATE_EXP,	//!	경험치 변화시 발생.
	NET_MSG_CHAR_UPDATE_MONEY_FC,	//!	경험치 변화시 발생.

	NET_MSG_GCTRL_UPDATE_SP,	//!	SP 변화시 발생.
	NET_MSG_GCTRL_UPDATE_LP,	//!	Living Point 변화시 발생.
	NET_MSG_GCTRL_UPDATE_SKP,	//!	스킬 포인트 변화시 발생.

	NET_MSG_GCTRL_UPDATE_STATE_BRD,	//!	상태 변화 업대이트.
	NET_MSG_GCTRL_UPDATE_PASSIVE_BRD,	//!	passive skill 상태 변화 업대이트.

	NET_MSG_GCTRL_UPDATE_BRIGHT,	//!	bright(속성)치 업대이트.
	NET_MSG_GCTRL_UPDATE_BRIGHT_BRD,	//!	bright(속성)치 업대이트.
	NET_MSG_GCTRL_UPDATE_STATS,	//!	stats 업대이트.

	NET_MSG_GCTRL_UPDATE_FLAGS,	//!	flags 업대이트.	

	NET_MSG_GCTRL_PUSHPULL_BRD,	//!	밀거나 당기는 액션 발생시.
	NET_MSG_GCTRL_JUMP_POS_BRD,	//!	케릭터의 위치가 순간적으로 변경되었을 경우. 클라이언트에 반영.

	NET_MSG_GCTRL_DOMINATE_MOVETO_CF,	// 지배 대상 이동 요청.
	NET_MSG_GCTRL_DOMINATE_MOVETO_CF_FB,	// 지배 대상 이동 요청 응답.
	NET_MSG_GCTRL_DOMINATE_MOVETO_FC,	// 지배 대상 이동.	

	NET_MSG_GCTRL_SELECT_TARGET,	//!	강제타겟설정.

	NET_MSG_GCTRL_INVEN_INSERT,	//!	인밴에 아이탬 삽입됨.
	NET_MSG_GCTRL_INVEN_DELETE,	//!	인밴   아이탬 삭제.
	NET_MSG_GCTRL_INVEN_DEL_INSERT,	//!	인밴에 있던 아이탬 삭제후 아이탬 삽입.

	NET_MSG_GCTRL_PUTON_RELEASE,	//!	슬롯의 아이탬을 제거함.
	NET_MSG_GCTRL_PUTON_RELEASE_BRD,	//!	슬롯의 아이탬을 제거함.
	NET_MSG_GCTRL_PUTON_UPDATE,	//!	슬롯에 아이탬 변경.
	NET_MSG_GCTRL_PUTON_UPDATE_BRD,	//!	[방송] 사용 아이탬이 변경됨.

	NET_MSG_GCTRL_INVEN_DRUG_UPDATE,	//!	소모형 아이탬 소모됨을 업대이트.
	NET_MSG_GCTRL_PUTON_DRUG_UPDATE,	//!	소모형 아이탬 소모됨을 업대이트.

	NET_MSG_GCTRL_INVEN_ITEM_UPDATE,	//!	인밴 아이템 수치 업데이트.

	NET_MSG_GCTRL_PICKUP_MONEY,	//!	돈 습득.
	NET_MSG_GCTRL_PICKUP_ITEM,	//!	아이템 습득 알림.

	NET_MSG_REFER_CHAR_PUBLIC,	// 캐릭터 정보보기 공개/비공개
	NET_MSG_REFER_CHAR_REQ,	// 캐릭터 정보보기 요청
	NET_MSG_REFER_CHAR_FB,	// 캐릭터 정보보기 요청 응답
	NET_MSG_REFER_CHAR_PUTON,	// 캐릭터 정보보기 장착 아이템
	NET_MSG_REFER_CHAR_SKILL,	// 캐릭터 정보보기 스킬
	NET_MSG_REFER_CHAR_INFO,	// 캐릭터 정보보기 능력치

	NET_MSG_GCTRL_2_FRIEND_REQ,	//!	친구로 가기 요청.
	NET_MSG_GCTRL_2_FRIEND_CK,	//!	친구로 가기 친구가 있는지 검사후 위치 반환.
	NET_MSG_GCTRL_2_FRIEND_AG,	//!	친구로 가기 AG로 회신 ( 친구 위치 정보 ).
	NET_MSG_GCTRL_2_FRIEND_FLD,	//!	친구로 가기 FLD로 이동 명령.
	NET_MSG_GCTRL_2_FRIEND_FB,	//!	친구로 가기 처리 결과 통보.

	NET_MSG_GCTRL_REQ_TAKE_FB,	//!	줍기 시도 결과.
	NET_MSG_GCTRL_REQ_FIELD_TO_INVEN,	//!	아이탬,돈 줍기 시도.
	NET_MSG_GCTRL_REQ_FIELD_TO_HOLD,	//!	Field에 있는 아이탬을 들기 시도.

	NET_MSG_GCTRL_REQ_INVEN_TO_HOLD,	//!	인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_INVEN_EX_INVEN,	//!	인밴에 있는 아이템과 인벤 아이템 교환.
    NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN,	//!	인밴에 있는 아이템과 창고 아이템 교환.
    NET_MSG_GCTRL_REQ_SLOT_EX_INVEN,	//!	인밴에 있는 아이템과 슬롯 아이템 교환.

	NET_MSG_GCTRL_REQ_SLOT_TO_HOLD,	//!	아이탬, 슬롯에서 손으로.

	NET_MSG_GCTRL_REQ_INVEN_TO_FIELD,	//!	아이탬을 바닥에 떨어트림.
    NET_MSG_GCTRL_REQ_HOLD_TO_INVEN,	//!	인벤에서 인벤으로 아이템 옮김.
	NET_MSG_GCTRL_REQ_INVEN_TO_INVEN,	//!	인벤에서 인벤으로 아이템 옮김.
    NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN,	//!	창고에서 인벤으로 아이템 옮김.
	NET_MSG_GCTRL_REQ_HOLD_TO_SLOT,	//!	아이탬, 손에서 슬롯으로.

	NET_MSG_GCTRL_REQ_HOLD_FB,	//!	인밴에 있는 아이탬을 들기 시도시 결과 피드백.
	NET_MSG_GCTRL_REQ_INVEN_TO_SLOT,	//!	인밴에 있는 아이탬을 슬롯에 넣기.

	NET_MSG_GCTRL_REQ_MONEY_TO_FIELD,	//!	돈을 필드에 버리기.
	NET_MSG_GCTRL_REQ_INVEN_SPLIT,	//!	인벤에 있는 겹침 아이템 분리.

	NET_MSG_GCTRL_PUTON_CHANGE,	//!	무기 스왑.
	NET_MSG_GCTRL_PUTON_CHANGE_BRD,	//!	[방송] 무기 스왑 알림.
	NET_MSG_GCTRL_PUTON_CHANGE_AG,	//!	Agent로 무기 스왑 알림.
	NET_MSG_GCTRL_REQ_SLOT_CHANGE,	//!	무기 슬롯 체인지.

    NET_MSG_GCTRL_REQ_SLOT_TO_INVEN,	//!	슬롯에 있는 아이템을 인벤으로

	NET_MSG_GCTRL_REQ_BUY_FROM_NPC,	//!	NPC에게 물품 판매 시도.
	NET_MSG_GCTRL_REQ_SALE_TO_NPC,	//!	NPC에게 물품 판매 시도.

    NET_MSG_GCTRL_RESURRECT_STATE,  // 부활 상태;
    NET_MSG_GCTRL_RESURRECT_FAFC,  // 부활 통보;
    NET_MSG_GCTRL_REQ_RESURRECT, // 부활 요청;
    NET_MSG_GCTRL_REQ_RESURRECT_FB,  // 부활 요청에 대한 응답;
    NET_MSG_RESURRECT_MOVEMAP_FA, // 부활에 의한 필드서버 이동;
    NET_MSG_RESURRECT_MOVEMAP_AF, // 부활에 의한 필드서버 이동;
    NET_MSG_RESURRECT_MOVEMAP_FB, // 부활에 의한 필드서버 아동;

	NET_MSG_GCTRL_REQ_REBIRTH,	//!	부활 요청.
	NET_MSG_GCTRL_REQ_REBIRTH_FB,	//!	부활 요청 응답.

    NET_MSG_CHAR_LEVEL_UP_FA,	//!	Field->Agent Character level up
	NET_MSG_GCTRL_REQ_LEVELUP,	//!	랩업 요청.
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP,
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_CA,
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_FB,
	NET_MSG_GCTRL_REQ_JUMPING_LEVLEUP_BYEXP,

	NET_MSG_GCTRL_REQ_LEVELUP_FB,	//!	랩업 요청 응답.
	NET_MSG_GCTRL_REQ_LEVELUP_BRD,	//!	랩업 효과.

	NET_MSG_GCTRL_REQ_STATSUP,	//!	STATS UP
	NET_MSG_GCTRL_REQ_STATSUP_FB,	//!	STATS UP 응답.

	NET_MSG_GCTRL_REQ_STATSUP_EX,	//!	STATS_EX UP
	NET_MSG_GCTRL_REQ_STATSUP_EX_FB,	//!	STATS_EX UP 응답.

    
	NET_MSG_GCTRL_REQ_INVENDRUG,	//!	인밴 약품 먹기.
	NET_MSG_GCTRL_REQ_LEARNSKILL,	//!	인밴 스킬 습득 요청.
	NET_MSG_GCTRL_REQ_LEARNSKILL_FB,	//!	인밴 스킬 습득 응답.
	NET_MSG_GCTRL_REQ_SKILLUP,	//!	스킬 업그래이드 요청.
    NET_MSG_GCTRL_REQ_RNSKILLUP,    //! 새로운 스킬 레벨업(배우기) : 2012-05-09 cwback
	NET_MSG_GCTRL_REQ_SKILLUP_FB,	//!	스킬 업그래이드 응답.
	NET_MSG_GCTRL_REQ_RNSKILLUP_COMPLETE,	//!	새로운 스킬 레벨업 완료

	NET_MSG_GCTRL_REQ_RECALL,	//!	귀환 아이템 사용 요청.
	NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB,	//!	귀환 아이템 사용 응답.
	NET_MSG_GCTRL_REQ_MOVEMAP_FA,	//!	귀환을 에이젼트 서버에 요청.

	NET_MSG_GCTRL_REQ_BUS,	//!	버스 승차 요청.
	NET_MSG_GCTRL_REQ_BUS_FB,	//!	버스 승차 요청 응답.

	NET_MSG_GCTRL_REQ_SKILL,	//!	스킬 발동 요청.
	NET_MSG_GCTRL_REQ_SKILL_FB,	//!	스킬 발동 요청 응답.
	NET_MSG_GCTRL_REQ_SKILL_BRD,	//!	스킬 발동.
	NET_MSG_GCTRL_SKILLFACT_BRD,	//!	스킬 적용시 체력 변화.
	NET_MSG_GCTRL_SKILLHOLD_BRD,	//!	스킬 적용시 지속 효과.
	NET_MSG_GCTRL_SKILLCONSUME_FB,	//!	스킬 발동후 소모되는 수치값.
	NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION,
	NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION_BRD,

	NET_MSG_GCTRL_SKILL_CANCEL,	//!	스킬 취소.
	NET_MSG_GCTRL_SKILL_CANCEL_BRD,	//!	스킬 취소 클라이언트에 반영.

	NET_MSG_GCTRL_REQ_SKILLHOLD_RS,	//!	스킬 지속 효과 제거 요청.
	NET_MSG_GCTRL_REQ_SKILLHOLD_RS_FB,	//!	스킬 지속 효과 제거 요청 응답.
	NET_MSG_GCTRL_SKILLHOLD_RS_BRD,	//!	스킬 지속 효과 제거.

	NET_MSG_GCTRL_SAVESKILLFACT_BRD,	//!	저장된 스킬 지속 효과 INFO

	NET_MSG_GCTRL_STATEBLOW_BRD,	//!	상태이상.
	NET_MSG_GCTRL_CURESTATEBLOW_BRD,	//!	상태이상.
	NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD,	//! 지형에 관련된 상태 이상 업데이트

	NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE,	//!	스킬 퀵슬롯의 액티브 스킬.
	NET_MSG_GCTRL_REQ_SKILLQ_SET,	//!	스킬 퀵슬롯 지정 요청.
	NET_MSG_GCTRL_REQ_SKILLQ_RESET,	//!	스킬 퀵슬롯 해지 요청.
	NET_MSG_GCTRL_REQ_SKILLQ_FB,	//!	스킬 퀵슬롯 요청에 따른 변경사항 클라이언트에 통지.

	NET_MSG_GCTRL_REQ_ACTIONQ_SET,	//!	엑션슬롯 지정 요청.
	NET_MSG_GCTRL_REQ_ACTIONQ_RESET,	//!	엑션슬롯 해지 요청.
	NET_MSG_GCTRL_REQ_ACTIONQ_FB,	//!	엑션슬롯 요청에 따른 변경사항 클라이언트에 통지.

	//! 창고의 특정 아이템 정보 가져오기specific
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM,	//!	창고 특정 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB,	//!	창고 특정 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_FB,	//!	창고 특정 정보 가져오기.

	NET_MSG_GCTRL_REQ_GETSTORAGE,	//!	창고 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_FB,	//!	창고 정보 가져오기.
	NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM,	//!	창고 정보 가져오기.

	NET_MSG_GCTRL_REQ_STORAGEDRUG,	//!	창고 약품 먹기.
	NET_MSG_GCTRL_REQ_STORAGESKILL,	//!	창고 스킬 먹기.

	NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD,	//!	창고에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_INVEN_EX_STORAGE,	//!	창고에 있는 아이탬과 인벤 아이템 교환.
    NET_MSG_GCTRL_REQ_STORAGE_EX_STORAGE, //! 창고에 있는 아이탬 교환.
	NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE,	//!	아이탬을 창고에 넣음.
    NET_MSG_GCTRL_REQ_STORAGE_TO_STORAGE, //! 창고에서 창고로 아이템 옮김.
    NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB, //! 창고에 아이템 넣기 피드백.

	NET_MSG_GCTRL_STORAGE_INSERT,	//!	창고에 아이탬 삽입됨.
	NET_MSG_GCTRL_STORAGE_DELETE,	//!	창고 아이탬 삭제.
	NET_MSG_GCTRL_STORAGE_DEL_INSERT,	//!	창고 아이탬 삭제 및 삽입.
	NET_MSG_GCTRL_STORAGE_DRUG_UPDATE,	//!	창고 소모품 사용횟수 파악.

	NET_MSG_GCTRL_STORAGE_SAVE_MONEY,	//!	창고에 돈을 저장.
	NET_MSG_GCTRL_STORAGE_DRAW_MONEY,	//!	창고에서 돈을 빼냄.
	NET_MSG_GCTRL_STORAGE_UPDATE_MONEY,	//!	창고의 돈 액수 변화.

	NET_MSG_GCTRL_REQ_STORAGE_SPLIT,	//!	창고에 있는 겹침 아이템 분리.
	NET_MSG_GCTRL_STORAGE_STATE,	//!	창고 사용가능 속성.
	NET_MSG_GCTRL_PREMIUM_STATE,	//!	프리미엄 속성.

	NET_MSG_GCTRL_PARTY_LURE,	//!	파티 가입 요청.			클라이언트 -> 서버.
    NET_MSG_GCTRL_PARTY_LURE_WITH_CHARNUM,	//!	파티 가입 요청.			클라이언트 -> 서버.
	NET_MSG_GCTRL_PARTY_LURE_FB,	//!	파티 가입 요청의 결과.	클라이언트 -> 서버.
    NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME,	//!	파티 가입 요청의 결과.	클라이언트 -> 서버.
	NET_MSG_GCTRL_PARTY_LURE_TAR,	//!	파티 가입 요청.			서버 -> 클라이언트.
    NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME,	//!	파티 가입 요청.			서버 -> 클라이언트.
	NET_MSG_GCTRL_PARTY_LURE_TAR_ANS,	//!	파티 가입 요청 응답.	클라이언트 -> 서버.

	NET_MSG_GCTRL_PARTY_SECEDE,	//!	파티 탈퇴 요청.

	NET_MSG_GCTRL_PARTY_ADD,	//!	파티원 추가.
	NET_MSG_GCTRL_PARTY_ADD_FB,	//!	파티원 추가 FB.

	NET_MSG_GCTRL_PARTY_DEL,	//!	파티원 제거.
	NET_MSG_GCTRL_PARTY_DEL_FB,	//!	파티원 제거 FB
	NET_MSG_GCTRL_PARTY_AUTHORITY,	//! 파티장 위임
	NET_MSG_GCTRL_PARTY_DISSOLVE,	//!	파티 해체.
	NET_MSG_GCTRL_PARTY_REFRESH_REQ, //! 파티 정보 갱신 요청;
	NET_MSG_GCTRL_PARTY_ERROR, //! 파티 에러 메세지 답장;
	
	NET_MSG_GCTRL_PARTY_FNEW,	//!	파티 생성. (field).
	NET_MSG_GCTRL_PARTY_MBR_DETAIL,	//!	가입원의 세부 정보.
	NET_MSG_GCTRL_PARTY_MBR_DETAIL_PARTY, //! 기존 파티원의 세부 정보;
	NET_MSG_GCTRL_PARTY_MBR_MOVEMAP,	//!	파티원의 map 이동.
	NET_MSG_GCTRL_PARTY_MBR_UPDATE, //! 파티원의 상태 변화.
    NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD,	//!	파티원의 아이템 습득을 알림.
    NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD,	//!	파티원의 아이템 습득을 알림. (아이템 링크)
	NET_MSG_GCTRL_PARTY_MBR_LEVELUP,	

	NET_MSG_GCTRL_PARTY_BRD,	//!	케릭터의 파티 소속 변화를 주변 클라이언트들에 알림.

	NET_MSG_GCTRL_EXPEDITION_FNEW_CA, // 원정대 생성;	
	NET_MSG_GCTRL_EXPEDITION_FNEW_FB,
	NET_MSG_GCTRL_PARTY_CHANGE_OPTION, // 파티/원정대 옵션 변경;
	NET_MSG_GCTRL_EXPEDITION_DEL_AF, // 원정대 제거;
	NET_MSG_GCTRL_EXPEDITION_DEL_AC, // 원정대 제거;
	NET_MSG_GCTRL_EXPEDITION_ERROR,	 // 원정대 관련 에러

	NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA, // 준비 확인 요청;
	NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA, // 준비 확인 요청에 대한 응답;

	NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT, // 자리 교체;	

	NET_MSG_GCTRL_PARTY_ADD_TENDER_ITEM, // 주사위 입찰 아이템 드랖;
	NET_MSG_GCTRL_PARTY_DELETE_TENDER_ITEM, // 주사위 입찰 아이템 대기 시간 만료;
	NET_MSG_GCTRL_PARTY_TENDER_ITEM, // 주사위 아이템 입찰;
	NET_MSG_GCTRL_PARTY_TENDER_ITEM_FB, // 주사위 아이템 입찰에 대한 결과 알림;
	NET_MSG_GCTRL_PARTY_JUNK_ITEM, // 마스터 분배/아이템 버리기;
	NET_MSG_GCTRL_PARTY_JUNK_ITEM_FB, // 마스터 분배/아이템 버리기에 대한 결과 알림;
	NET_MSG_GCTRL_PARTY_TRANSFER_ITEM, // 마스터 분배/아이템 양도;
	NET_MSG_GCTRL_PARTY_TRANSFER_ITEM_FB, // 마스터 분배/아이템 양도에 대한 결과 알림;		

	NET_MSG_GCTRL_TRADE,	//!	트래이드 요청. ( 클라이언트1 -> 서버 )
	NET_MSG_GCTRL_TRADE_FB,	//!	트래이드 요청 응답. ( 서버 -> 클라이언트1 )
	NET_MSG_GCTRL_TRADE_TAR,	//!	트래이드 상대에게 요청. ( 서버 -> 클라이언트2 )
	NET_MSG_GCTRL_TRADE_TAR_ANS,	//!	트래이드 상대의 응답. ( 클라이언트2 -> 서버 )

	NET_MSG_GCTRL_TRADE_AGREE,	//!	트래이드 승인 지정.
	
	NET_MSG_GCTRL_TRADE_AGREE_TAR,	//!	트래이드 승인 지정 상대방에게 알림. ( 상대방에게. )

	NET_MSG_GCTRL_TRADE_ITEM_REGIST,	//!	트래이드할 아이템 등록.
	NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR,	//!	트래이드할 아이템 등록. ( 상대방에게. )
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE,	//!	트래이드할 아이템 목록에서 제거.
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR,	//!	트래이드할 아이템 목록에서 제거. ( 상대방에게. )

	NET_MSG_GCTRL_TRADE_MONEY,	//!	트래이드할 금액.
	NET_MSG_GCTRL_TRADE_MONEY_TAR,	//!	트래이드할 금액을 상대방에게 알림.

	NET_MSG_GCTRL_TRADE_COMPLETE_TAR,	//!	트래이드 완료.

	NET_MSG_GCTRL_TRADE_CANCEL,	//!	트래이드 취소.
	NET_MSG_GCTRL_TRADE_CANCEL_TAR,	//!	트래이드 취소. ( 상대방에게. )

	NET_MSG_GCTRL_INVEN_BOXOPEN,	//!	box open 요청.
	NET_MSG_GCTRL_INVEN_BOXOPEN_FB,	//!	box open 요청 FB.

	NET_MSG_GCTRL_INVEN_DISGUISE,	//!	복장에 코스춤을 덧씌움.
	NET_MSG_GCTRL_INVEN_DISGUISE_FB,	//!	복장에 코스춤을 덧씌움 FB.

    NET_MSG_GCTRL_INVEN_BOX_WRAPPING,	//!	아이템에 포장지를 씌움.;
    NET_MSG_GCTRL_INVEN_BOX_WRAPPING_FB,	//!	아이템에 포장지를 씌움 FB.;

    NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING,	//!	포장지 박스를 풂.;
    NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING_FB,	//!	포장지 박스를 풂 FB.;

	NET_MSG_GCTRL_INVEN_GRINDING,	//!	INVEN 아이템 연마 시도.
	NET_MSG_GCTRL_INVEN_LOCKBOX_UNLOCK,	//! 잠금 상자 열기 시도,
	NET_MSG_GCTRL_INVEN_GRINDING_FB,	//!	INVEN 아이템 연마 시도.

	NET_MSG_GCTRL_INVEN_PERIODEXTEND,	//!	아이템 기간 연장 시도;
	NET_MSG_GCTRL_INVEN_PERIODEXTEND_FB,

	NET_MSG_GCTRL_REGEN_GATE,	//!	부활 위치 지정.
	NET_MSG_GCTRL_REGEN_GATE_FB,	//!	부활 위치 지정 FB.

	NET_MSG_GCTRL_CURE,	//!	치료 요청.
	NET_MSG_GCTRL_CURE_FB,	//!	치료 요청 FB.

	NET_MSG_GCTRL_CHARRESET,	//!	케릭터 정보 리셋 요청. ( stats, skill )
	NET_MSG_GCTRL_CHARRESET_FB,	//!	케릭터 정보 리셋 요청 FB. ( stats, skill )
	
	NET_MSG_GCTRL_NPC_ITEM_TRADE,	//!	아이템 교환 요청.
	NET_MSG_GCTRL_NPC_ITEM_TRADE_FB,	//!	아이템 교환 요청 FB.

	NET_MSG_GCTRL_CONFRONT,	//!	대련 요청 to (서버)
	NET_MSG_GCTRL_CONFRONT_TAR,	//!	대련 요청 to (상대방)
	NET_MSG_GCTRL_CONFRONT_ANS,	//!	대련 요청 응답 to (서버)
	NET_MSG_GCTRL_CONFRONT_FB,	//!	대련 요청 응답 to (요청자)

	NET_MSG_GCTRL_CONFRONT_START2_FLD,	//!	대련 시작 to (필드)
	NET_MSG_GCTRL_CONFRONT_START2_CLT,	//!	대련 시작 to (클라이언트)
	NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT,	//!	대련 FIGHT to (클라이언트)

	NET_MSG_GCTRL_CONFRONT_END2_FLD,	//!	대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONT_END2_CLT,	//!	대련 종료 to (클라이언트).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD,	//!	대련 종료 to (주변 클라이언트).
	NET_MSG_GCTRL_CONFRONT_END2_AGT,	//!	대련 종료 to (에이전트).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR,	//!	대련 종료 to (대련 당사자 클라이언트).

	NET_MSG_GCTRL_CONFRONT_RECOVE,	//!	대련옵션 회복약 사용 횟수 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTPTY_START2_FLD,	//!	(파티) 대련 시작 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_START2_CLT,	//!	(파티) 대련 시작 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTPTY_END2_FLD,	//!	(파티) 대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_END2_CLT,	//!	(파티) 대련 종료 to (클라이언트).

	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD,	//!	(파티) 대련에 참가할 멤버 검사 to (필드).
	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT,	//!	(파티) 대련에 참가할 멤버 to (에이젼트).

	NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD,	//!	학원 파티 대련 승리시 경험치 전체 유저에 exp 부여.

	NET_MSG_GCTRL_CONFRONTCLB_START2_FLD,	//!	(클럽) 대련 시작 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_START2_CLT,	//!	(클럽) 대련 시작 to (클라이언트)

	NET_MSG_GCTRL_CONFRONTCLB_END2_FLD,	//!	(클럽) 대련 종료 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_END2_CLT,	//!	(클럽) 대련 종료 to (클라이언트).

	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD,	//!	(클럽) 대련에 참가할 멤버 검사 to (필드).
	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT,	//!	(클럽) 대련에 참가할 멤버 to (에이젼트).

	NET_MSG_GCTRL_SERVERSTOP,	// Purpose for field server test
	NET_MSG_GCTRL_SERVERSTOP_FB,

	NET_MSG_GCTRL_REQ_GENITEM_AGT,	//!	아이템 생성 질의 to (에이전트).
	NET_MSG_GCTRL_REQ_GENITEM_FLD_FB,	//!	아이템 생성 답변 to (필드).

	NET_MSG_GCTRL_REQ_GESTURE,	//!	제스쳐 동작.
	NET_MSG_GCTRL_REQ_GESTURE_BRD,	//!	제스쳐 동작 BRD.

	NET_MSG_GCTRL_QITEMFACT_BRD,	//!	?아이템 수치 BRD.
	NET_MSG_GCTRL_QITEMFACT_END_BRD,	//!	?아이템 종료 BRD.

	NET_MSG_GCTRL_EVENTFACT_BRD,	//!	?아이템 이벤트 시작 BRD.
	NET_MSG_GCTRL_EVENTFACT_END_BRD,	//!	?아이템 이벤트 종료 BRD.
	NET_MSG_GCTRL_EVENTFACT_INFO,	//!	?아이템 이벤트 BRD.

    NET_MSG_GCTRL_IPEVENT_BRD, //! IP Event 여부.

	NET_MSG_GCTRL_INVEN_CLEANSER,	//!	망각의 세제를 사용하여 변환된 형상을 되돌림.
	NET_MSG_GCTRL_INVEN_CLEANSER_FB,	//!	망각의 세제를 사용하여 변환된 형상을 되돌림 FB.

	NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT,	//!	제한 시간 만료로 아이템 삭제.

	NET_MSG_GCTRL_INVEN_RESET_SKST,	//!	스텟 스킬 리셋 요청.
	NET_MSG_GCTRL_INVEN_RESET_SKST_FB,	//!	스텟 스킬 리셋 응답.

	NET_MSG_GCTRL_INVEN_CHARCARD,	//!	케릭터 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_CHARCARD_FB,	//!	케릭터 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_INVENLINE,	//!	인벤라인 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_INVENLINE_FB,	//!	인벤라인 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_STORAGECARD,	//!	창고 추가 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_STORAGECARD_FB,	//!	창고 추가 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_STORAGEOPEN,	//!	긴급 창고 이용 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB,	//!	긴급 창고 이용 카드 사용 요청 FB.
	NET_MSG_GCTRL_INVEN_STORAGECLOSE,	//!	긴급 창고 이용 카드 사용 완료.

	NET_MSG_GCTRL_INVEN_PREMIUMSET,	//!	프리미엄셋 사용 요청.
	NET_MSG_GCTRL_INVEN_PREMIUMSET_FB,	//!	프리미엄셋 사용 요청 FB.

	NET_MSG_GCTRL_FIRECRACKER,	//!	폭죽 사용 요청.
	NET_MSG_GCTRL_FIRECRACKER_FB,	//!	폭죽 사용 요청 FB.
	NET_MSG_GCTRL_FIRECRACKER_BRD,	//!	폭죽 사용.

	NET_MSG_GCTRL_PLAYERKILLING_ADD,	//!	player killing 당사자 추가.
	NET_MSG_GCTRL_PLAYERKILLING_DEL,	//!	player killing 당사자 제거.

	NET_MSG_GET_CHARGED_ITEM_CF,        //! 구입한 아이템 정보 DB에서 가져오기 요청
	NET_MSG_GET_CHARGED_ITEM_FC,       //! 구입한 아이템 정보 DB에서 가져오기 요청 FB
	NET_MSG_GCTRL_CHARGED_ITEM_GET,	   //! 구입한 아이템 가져오기 요청.
	NET_MSG_GCTRL_CHARGED_ITEM_GET_FB, //! 구입한 아이템 가져오기 요청 FB.
	NET_MSG_GCTRL_CHARGED_ITEM_DEL,	//!	구입한 아이템 리스트에서 삭제.
	NET_MSG_GCTRL_CHARGED_ITEM2_INVEN,	//!	구입한 아이템 인벤에 넣기 요청.

	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN,	//!	random box open 요청.
	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB,	//!	random box open 요청 FB.
	NET_MSG_GCTRL_INVEN_LOCKBOXOPEN,	//! 잠금 상자 열기 요청;
	NET_MSG_GCTRL_INVEN_LOCKBOX_FB,		//! 잠금 상자 관련 요청 결과;
	NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN,	// 선택상자 오픈;
	NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN_FB,

	NET_MSG_GCTRL_INVEN_DISJUNCTION,	//!	코스툼 분리 요청.
	NET_MSG_GCTRL_INVEN_DISJUNCTION_FB,	//!	코스툼 분리 요청 FB.

	NET_MSG_GCTRL_INVEN_HAIR_CHANGE,	//!	헤어스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB,	//!	헤어스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD,	//!	헤어스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE,	//!	헤어스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_FB,	//!	헤어스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_BRD,	//!	헤어스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE,	//!	헤어컬러 변경 요청.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB,	//!	헤어컬러 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD,	//!	헤어컬러 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_FACE_CHANGE,	//!	얼굴스타일 변경 요청.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB,	//!	얼굴스타일 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD,	//!	얼굴스타일 변경 결과 BRD.

	NET_MSG_GCTRL_INVEN_REMODELOPEN,	//!	개조 기능 이용 카드 사용 요청.
	NET_MSG_GCTRL_INVEN_REMODELOPEN_FB,	//!	개조 기능 이용 카드 사용 요청 FB.

	NET_MSG_GCTRL_INVEN_GARBAGEOPEN,	//!	휴지통 기능 이용 카드 사용 요청
	NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB,	//!	휴지통 기능 이용 카드 사용 요청 FB.
	NET_MSG_GCTRL_GARBAGE_RESULT,	//!	아이템 삭제 요청
	NET_MSG_GCTRL_GARBAGE_RESULT_FB,	//!	아이템 삭제 요청 FB.

	NET_MSG_CROW_MOVETO,
	NET_MSG_CROW_ATTACK,
	NET_MSG_CROW_AVOID,
	NET_MSG_CROW_DAMAGE,
    NET_MSG_CROW_SKILL,
	NET_MSG_CROW_SKILL_DEALY_ACTION,
	NET_MSG_CROW_END_SKILL,
    NET_MSG_CROW_UPDATE_STATE,

	NET_MSG_GCTRL_INVEN_RENAME,	//!	이름 변경 요청.
	NET_MSG_GCTRL_INVEN_RENAME_FB,	//!	이름 변경 결과 FB.
	NET_MSG_GCTRL_INVEN_RENAME_FROM_DB,	//!	DB 저장 결과 FB.
	NET_MSG_GCTRL_INVEN_RENAME_AGTBRD,	//!	Agent에 모든채널 BRD 요청.
	NET_MSG_GCTRL_INVEN_RENAME_BRD,	//!	이름 변경 결과 BRD.

	NET_MSG_GCTRL_PARTY_MBR_RENAME,	//! 이름 변경을 파티멤버에게 알림.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT,	//! 이름 변경을 클럽멤버에게 알림.
	NET_MSG_GCTRL_FRIEND_RENAME_CLT,	//! 이름 변경을 친구들에게 알림.
	//NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT,	//! 폰번호 변경을 친구들에게 알림.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD,	//! 이름 변경을 FieldServer에 알림.
	NET_MSG_GCTRL_FRIEND_RENAME_FLD,	//! 이름 변경을 FieldServer에 알림.
	NET_MSG_GCTRL_CHANGE_NAMEMAP	,	//! GLLandMan 의 NameMap 변경

	NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN, // NPC 대화 요청
	NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN_FB, // NPC 대화 요청 FB
	NET_MSG_GCTRL_REQ_QUEST_START,	//!	퀘스트 시작을 요청.
	NET_MSG_GCTRL_REQ_QUEST_START_WITHOUT_NPC,
	NET_MSG_GCTRL_REQ_QUEST_START_FB,	//!	퀘스트 시작을 요청 FB.
	NET_MSG_GCTRL_REQ_QUEST_TALK,	//!	퀘스트 대화 진행 요청.
	NET_MSG_GCTRL_REQ_QUEST_TALK_FB,	//!	퀘스트 대화 진행 요청 FB.

	NET_MSG_GCTRL_QUEST_PROG_GIVEUP,	//!	퀘스트 진행 포기 요청.
	NET_MSG_GCTRL_QUEST_PROG_DEL,	//!	퀘스트 진행 리스트에서 삭제.
	NET_MSG_GCTRL_QUEST_END_DEL,	//!	퀘스트 완료 리스트에서 삭제.
	NET_MSG_GCTRL_QUEST_END_STREAM,	//!	퀘스트 완료 정보.

	NET_MSG_GCTRL_QUEST_PROG_STREAM,	//!	퀘스트 진행 정보.
	NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM,	//!	퀘스트 진행 step.
	NET_MSG_GCTRL_QUEST_PROG_INVEN,	//!	퀘스트 INVENTORY.

	NET_MSG_GCTRL_QUEST_PROG_NPCTALK,	//!	퀘스트 진행 npc 대화.
	NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB,	//!	퀘스트 진행 npc 대화.

	NET_MSG_GCTRL_QUEST_PROG_MOBKILL,	//!	퀘스트 진행 mob 제거.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL,	//!	퀘스트 진행 mob 제거.( 파티퀘스트 완료시 )
	NET_MSG_GCTRL_QUEST_PROG_QITEM,	//!	퀘스트 퀘스트 아이템 습득.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM,	//!	퀘스트 진행 퀘스트 아이템 습득.( 파티퀘스트 완료시 )
	NET_MSG_GCTRL_QUEST_PROG_ITEMUSE,	//!퀘스트 진행 퀘스트 아이템 사용
	NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING,//!퀘스트 진행 연마 퀘스트 아이템 연마
	NET_MSG_GCTRL_QUEST_PROG_CONFTING,//!퀘스트 진행 대련
	NET_MSG_GCTRL_QUEST_PROG_REACHZONE,	//!	퀘스트 진행 위치 도달.
	NET_MSG_GCTRL_QUEST_PROG_READ,	//!	퀘스트 읽기 요청 리셋.
	NET_MSG_GCTRL_QUEST_PROG_TIME,	//!	퀘스트 경과 시간.
	NET_MSG_GCTRL_QUEST_PROG_TIMEOVER,	//!	퀘스트 시간 제한 초과됨.
	NET_MSG_GCTRL_QUEST_PROG_NONDIE,	//!	퀘스트 사망 제한.
	NET_MSG_GCTRL_QUEST_PROG_LEVEL,	//!	퀘스트 레벨 도달.
	NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP,//!퀘스트 레벨 업

	NET_MSG_GCTRL_QUEST_COMPLETE_FB,	//!	퀘스트가 완료되었으나 보상 물품을 주지 못한상태.
	NET_MSG_GCTRL_REQ_QUEST_COMPLETE,	//!	퀘스트 완료 요청 ( 보상물품 습득 ).

	NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN,	//!	퀘스트 인밴 수량 변경.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP,	//!	퀘스트 인밴로 아이템 습득.

	NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT,	//!	퀘스트 인밴 아이탬 삽입.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE,	//!	퀘스트 인밴 아이탬 삭제.

	
	NET_MSG_GCTRL_PMARKET_TITLE,	//!	개인 상점의 타이틀 설정.
	NET_MSG_GCTRL_PMARKET_TITLE_FB,	//!	개인 상점의 타이틀 설정.

	NET_MSG_GCTRL_PMARKET_REGITEM,	//!	개인 상점에 판매 아이템 등록.
	NET_MSG_GCTRL_PMARKET_REGITEM_FB,	//!	개인 상점에 판매 아이템 등록 FB.

	NET_MSG_GCTRL_PMARKET_DISITEM,	//!	개인 상점에서 아이템 제거.
	NET_MSG_GCTRL_PMARKET_DISITEM_FB,	//!	개인 상점에서 아이템 제거 FB.
	NET_MSG_GCTRL_PMARKET_OPEN,	//!	개인 상점 오픈.
	NET_MSG_GCTRL_PMARKET_OPEN_FB,	//!	개인 상점 오픈 FB.
	NET_MSG_GCTRL_PMARKET_OPEN_BRD,	//!	개인 상점 오픈 BRD.

	NET_MSG_GCTRL_PMARKET_CLOSE,	//!	개인 상점 종료.
	NET_MSG_GCTRL_PMARKET_CLOSE_FB,	//!	개인 상점 종료 FB.
	NET_MSG_GCTRL_PMARKET_CLOSE_BRD,	//!	개인 상점 종료 BRD.
	
	NET_MSG_GCTRL_PMARKET_BUY,	//!	개인 상점에서 구입.
	NET_MSG_GCTRL_PMARKET_BUY_FB,	//!	개인 상점에서 구입 FB.

	NET_MSG_GCTRL_PMARKET_ITEM_INFO,	//!	개인 상점의 판매 아이템 정보 요청.
	NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD,	//!	개인 상점의 판매 아이템 BRD.
	NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD,	//!	개인 상점의 판매 아이템 Update BRD.

	//! 개인상점 검색

	NET_MSG_GCTRL_PMARKET_REGITEM_FH,	//!	개인 상점에 판매 아이템 등록.
	NET_MSG_GCTRL_PMARKET_DISITEM_FH,	//!	개인 상점에서 아이템 제거.
	NET_MSG_GCTRL_PMARKET_SEARCH_REQ_CAH,//! 개인 상점에서 아이템 검색 요청
	NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC,//!개인 상점 아이템 검색 결과

    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_CF,	//!	개인상점 검색 카드 사용 요청.    ( 클라 -> 필드서버 );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FA,	//!	개인상점 검색 카드 사용 알림.    ( 필드서버 -> 에이전트서버 );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_AF,	//!	개인상점 검색 카드 사용 알림응답.( 에이전트서버 -> 필드서버 );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FB,	//!	개인상점 검색 카드 사용 결과 FB. ( 필드or에이전트서버 -> 클라 );
    NET_MSG_GCTRL_PMARKET_SEARCH_CLOSE,	    //!	긴급 창고 이용 카드 사용 완료.    ( 클라 -> 에이전트서버 );

	//NET_MSG_GCTRL_PMARKET_BUY,	//!	개인 상점에서 구입.
	//NET_MSG_GCTRL_PMARKET_BUY_FB,	//!	개인 상점에서 구입 FB.

	//NET_MSG_GCTRL_PMARKET_ITEM_INFO,	//!	개인 상점의 판매 아이템 정보 요청.
	//NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD,	//!	개인 상점의 판매 아이템 BRD.
	//NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD,	//!	개인 상점의 판매 아이템 Update BRD.
	//----------------

	NET_MSG_GCTRL_LEVEL_EVENT_END_FLD,	//!	레벨 이벤트 종료.
	NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD,	//! 레벨 이벤트 경고 메세지.
	NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD,	//! 레벨 이벤트 카운트 메세지.

	NET_MSG_SERVER_CLOSE_FIELD, //! 필드 서버 접속 끊김;
	NET_MSG_SERVER_CLOSE_AGENT, //! Agent Server 접속 끊김 ( Agent Server에서 연결되는 다른 Agent Server를 의미함 );

	NET_MSG_GM_EVENT_EX,	//!	EX이벤트 시작.
	NET_MSG_GM_EVENT_EX_END,	//!	EX이벤트 종료.

    //NET_MSG_EVENT_EXP_MS,       //! 경험치 이벤트 ServerManager->Session // 패킷 랜덤 으로 인해서 상단으로 이동 2014.12.18
    NET_MSG_EVENT_EXP_SA,       //! 경험치 이벤트 Session->Agent
	NET_MSG_GM_EVENT_EXP_CAF,	//!	경험치 이벤트 시작.
    //NET_MSG_EVENT_EXP_END_MS,   //! 경험치 이벤트 종료 ServerManager->Session // 패킷 랜덤 으로 인해서 상단으로 이동 2014.12.18
    NET_MSG_EVENT_EXP_END_SA,   //! 경험치 이벤트 종료 Session->Agent
	NET_MSG_GM_EVENT_EXP_END_CAF,	//!	경험치 이벤트 종료.
	NET_MSG_GM_EVENT_EXP_END_FB,	//!	경험치 이벤트 종료 FB.

	NET_MSG_GM_EVENT_ITEM_GEN,	//!	아이탬 발생율 이벤트.
	NET_MSG_GM_EVENT_ITEM_GEN_FB,	//!	아이탬 발생율 이벤트 FB.

	NET_MSG_GM_EVENT_ITEM_GEN_END,	//!	아이탬 발생율 이벤트 종료.
	NET_MSG_GM_EVENT_ITEM_GEN_END_FB,	//!	아이탬 발생율 이벤트 종료 FB.

	NET_MSG_GM_EVENT_MONEY_GEN,	//!	돈 발생율 이벤트.
	NET_MSG_GM_EVENT_MONEY_GEN_END,	//!	돈 발생율 이벤트 종료.

	NET_MSG_GM_EVENT_GRADE,	//!	연마 확률 이벤트
	NET_MSG_GM_EVENT_GRADE_FB,	//!	연마 확률 이벤트 FB.

	NET_MSG_GM_EVENT_GRADE_END,	//!	연마 확률 이벤트 종료
	NET_MSG_GM_EVENT_GRADE_END_FB,	//!	연마 확률 이벤트 종료 FB.

	NET_MSG_GCTRL_PERIOD,
	NET_MSG_GCTRL_WEATHER,
	NET_MSG_GCTRL_WHIMSICAL,
	NET_MSG_GCTRL_FIELDINFO_RESET,

	NET_MSG_SERVER_CTRL_WEATHER,	//!	서버의 날씨 변경.
	NET_MSG_SERVER_CTRL_TIME,	//!	서버의 시간 변경.
	NET_MSG_SERVER_CTRL_MONTH,	//!	서버의 월 변경.
	NET_MSG_SERVER_CTRL_GENITEMHOLD,	//!	서버의 아이템 독점권 변경.
	NET_MSG_SERVER_PLAYERKILLING_MODE,	//!	player killing on/off.

	NET_MSG_GM_MOVE2GATE,	//!	케릭터를 gate no 로 이동.
	NET_MSG_GM_MOVE2GATE_FLD,	//!	케릭터를 gate no 로 이동 Agent -> Field
	NET_MSG_GM_MOVE2GATE_FB,	//!	케릭터를 gate no 로 이동.
	NET_MSG_GM_KICK_USER,	//!	특정 플레이어를 내보냄.
	NET_MSG_GM_KICK_USER_PROC,	//!	특정 플레이어를 내보냄.
	NET_MSG_GM_KICK_USER_PROC_FLD,    //!  특정 플레이어 강제 접속종료 Agent -> Field
	NET_MSG_GM_MOVE2MAPPOS,	//!	캐릭터를 특정맵의 특정위치로 이동.
	NET_MSG_GM_MOVE2MAPPOS_FLD,	//!	캐릭터를 특정맵의 특정위치로 이동. Agent -> Field

	NET_MSG_BLOCK_DETECTED,	//!	케릭터를 gate no 로 이동.
	NET_MSG_SERVER_GENERALCHAT,	//!	서버 알림 메시지 발생시.

	NET_MSG_GM_WHERE_NPC,	//!	NPC가 어디 위치에 있는지 검사.
	NET_MSG_GM_WHERE_NPC_FLD,	//!	NPC가 어디 위치에 있는지 검사. Agent -> Field
	NET_MSG_GM_WHERE_NPC_FB,	//!	NPC가 어디 위치에 있는지 검사.

	NET_MSG_GM_WHERE_NPC_ALL,	//!	맵에 모든 NPC 위치 검사
	NET_MSG_GM_WHERE_NPC_ALL_FLD,	//!	NPC가 어디 위치에 있는지 검사. Agent -> Field
	NET_MSG_GM_WHERE_NPC_ALL_FB,	//!	NPC가 어디 위치에 있는지 검사.

	NET_MSG_GM_WHERE_PC_MAP,	//!	PC가 어느 멥에 있는지 검사.
	NET_MSG_GM_WHERE_PC_MAP_FB,	//!	PC가 어느 멥에 있는지 검사 FB.

	NET_MSG_GM_WHERE_PC_POS,	//!	PC가 어디 위치에 있는지 검사.
	NET_MSG_GM_WHERE_PC_POS_FB,	//!	PC가 어디 위치에 있는지 검사 FB.

	NET_MSG_GM_MOVE2CHAR,	//!	캐릭터를 특정캐릭터로 이동 요청.
	NET_MSG_GM_MOVE2CHAR_POS,	//!	캐릭터를 특정캐릭터로 이동 위치 확인.
	NET_MSG_GM_MOVE2CHAR_AG,	//!	캐릭터를 특정캐릭터로 이동 위치 확인 응답.
	NET_MSG_GM_MOVE2CHAR_FLD,	//!	캐릭터를 특정캐릭터로 이동 요청 실행.
	NET_MSG_GM_MOVE2CHAR_FB,	//!	캐릭터를 특정캐릭터로 이동 실행 결과.

	NET_MSG_GM_BIGHEAD,	//!	BIG HEAD 모드 on/off.
	NET_MSG_GM_BIGHEAD_BRD,	//!	BIG HEAD 모드 on/off.

	NET_MSG_GM_BIGHAND,	//!	BIG HAND 모드 on/off.
	NET_MSG_GM_BIGHAND_BRD,	//!	BIG HAND 모드 on/off.

	NET_MSG_GM_MOB_GEN,	//!	몹 발생.
	NET_MSG_GM_MOB_GEN_FLD,	//!	몹 발생. Agent->Field
	NET_MSG_GM_MOB_DEL,	//!	몹 제거.
	NET_MSG_GM_MOB_DEL_FLD,	//!	몹 제거. Agent->Field
	NET_MSG_GM_MOB_LEVEL,	//!	몹(LEVEL FILE) 발생.
	NET_MSG_GM_MOB_LEVEL_LAYER,	//!	레벨 파일의 몹 레이어 변경
	NET_MSG_GM_MOB_LEVEL_CLEAR,	//!	몹(LEVEL FILE) 발생 제거.
	NET_MSG_GM_MOB_LEVEL_LIST, //! Layer list 알림 _ agent -> field;
	NET_MSG_GM_MOB_LEVEL_LIST_FC, //! Layer List 알림 _ field -> client;

	NET_MSG_GM_CHAT_BLOCK_UACCOUNT,	//!	채팅 블록 (유저이름)
	NET_MSG_GM_CHAT_BLOCK_CHARNAME,	//!	채팅 블록 (캐릭이름)
	NET_MSG_GM_CHAT_BLOCK_CHARID,	//!	채팅 블록 (캐릭번호)
	NET_MSG_GM_CHAT_BLOCK_FB,	//!	채팅 블록 FB
	NET_MSG_GM_CHAT_BLOCK_FLD,	//!	채팅 블록 FLD
	NET_MSG_GM_CHAT_BLOCK_NOTIFY, //! 채팅 블록 상태 알림

	NET_MSG_USER_CHAR_INFO_4NAME,	//! 캐릭터의 정보 출력 (유저명령어)
	NET_MSG_USER_CHAR_INFO_AGT_FB,	//!	캐릭터의 정보 출력 agt FB.
	NET_MSG_USER_CHAR_INFO_FLD_FB,	//!	캐릭터의 정보 출력 fld FB.
	NET_MSG_USER_CHAR_INFO_FLD,	//!	캐릭터의 정보 출력 FLD.

	NET_MSG_GM_CHAR_INFO_4NAME,	//!	캐릭터의 정보 출력 (캐릭이름).
	NET_MSG_GM_CHAR_INFO_4CHARID,	//!	캐릭터의 정보 출력 (캐릭번호).
	NET_MSG_GM_CHAR_INFO_AGT_FB,	//!	캐릭터의 정보 출력 agt FB.
	NET_MSG_GM_CHAR_INFO_FLD_FB,	//!	캐릭터의 정보 출력 fld FB.
	NET_MSG_GM_CHAR_INFO_FLD,	//!	캐릭터의 정보 출력 FLD.

   
//-----------------------------------------------------------------------------------------------------------

	NET_MSG_GM_FREEPK,	//!	자유 피케이
	NET_MSG_GM_FREEPK_BRD,	//!	자유 피케이

	NET_FREEPK_GM_COMMAND_CA, //! GM명령어 프리 PK ON/OFF

	NET_MSG_GM_WARNING_MSG,	//!	자유 피케이
	NET_MSG_GM_WARNING_MSG_FLD,	//!	자유 피케이
	NET_MSG_GM_WARNING_MSG_BRD,	//!	자유 피케이

	NET_MSG_GM_VIEWALLPLAYER,	//!  맵의 모든 플레이어 표시
	NET_MSG_GM_VIEWALLPLAYER_FLD_REQ,	//!  필드서버에 요청
	NET_MSG_GM_VIEWALLPLAYER_FLD_FB ,	//!  필드서버 처리결과 회신

	NET_MSG_GM_GENCHAR,	//!  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_POS,	//!  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_AG,	//!  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_FLD,	//!  케릭터 소환 요청
	NET_MSG_GM_GENCHAR_FB,	//!  케릭터 소환 요청

	NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO,	// 성향이벤트

	NET_MSG_GCTRL_UPDATE_LASTCALL,	// 직전 귀환 위치 클라이언트로 전송
	NET_MSG_GCTRL_UPDATE_STARTCALL,	// 시작 귀환 위치 클라이언트로 전송

	NET_MSG_GM_WHYSOSERIOUS_EMULATOR_CF,	
    NET_MSG_GM_SHOWMETHEMONEY_EMULATOR_CF,	
    NET_MSG_GM_COUNTDOWN_MSG_BRD,	// 자동레벨 카운트 메세지
	NET_MSG_GM_SHOWMETHEMONEY,	//!  캐릭터로 돈 저장
	NET_MSG_GM_SHOWMETHEMONEY_FLD,	//!  캐릭터로 돈 저장
    NET_MSG_GM_WHYSOSERIOUS,	//!  캐릭터로 아이템 지급
    NET_MSG_GM_WHYSOSERIOUS_FLD,	//!  캐릭터로 아이템 지급
	NET_MSG_GM_WHYSOSERIOUS_FB_FAC, //! 아이템 지급 결과;

	NET_MSG_GM_HITRATESCRIPT_MSG_CF, // 공격성공률 스크립트 리로드 요청;

	NET_MSG_GM_FLYCAMERACONTROL_CF,
	NET_MSG_GM_FLYCAMERACONTROL_FC,
	NET_MSG_GM_FLYCAMERA_SETVIEWSIGHT_CF,
	NET_MSG_GM_FLYCAMERACONTROL_CA,	//! C->A, GM 카메라 자유 조절;
	NET_MSG_GM_FLYCAMERACONTROL_FB,	//! A->C, GM 카메라 자유 조절;
	NET_MSG_GM_FLYCAMERACONTROL_MOVE_CF,

    NET_MSG_GM_LOG_TO_DB_CAF,   //!  텍스트로 저장중인 로그를 강제로 DB 로 올리게 함

	NET_MSG_GM_AUTO_LEVEL, // autoLevel Control 메세지 _ client -> agent;
	NET_MSG_GM_AUTO_LEVEL_LIST, // autoLevel Control 메세지 _ agent -> client;
	NET_MSG_GM_AUTO_LEVEL_RESULT, // autoLevel Control 메세지 _ agent -> client;
	NET_MSG_GM_AUTO_LEVEL_INFORMATION, // autoLevel Control 메세지 _ agent -> client;

	NET_MSG_GM_LAND, // Land Control Message _ client -> server;
	NET_MSG_GM_LAND_INFORMATION, // Land Information Lotify Message _ field -> client;

	NET_MSG_GM_MAP_LAYER, // Map Layer Control Message _ client -> agent;
	NET_MSG_GM_MAP_LAYER_NEW_NOTIFY, // Map Layer Notify Message _ agent -> client;
	NET_MSG_GM_MAP_LAYER_DEL_NOTIFY, // Map Layer Notify Message _ agent -> client;
	NET_MSG_GM_MAP_LAYER_ERROR_NOTIFY, // Map Layer Notify Message _ agent -> client;

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_REBUILD_RESULT,	// 아이템 개조 - 시작, 종료 결과	// ITEMREBUILD_MARK
	NET_MSG_REBUILD_MOVE_ITEM,	// 아이템 개조 - 아이템 이동
	NET_MSG_REBUILD_COST_MONEY,	// 아이템 개조 - 개조 비용
	NET_MSG_REBUILD_INPUT_MONEY,	// 아이템 개조 - 유저 입력 금액

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_PET_DROPPET,	// ANYPET 생성
	NET_MSG_CREATE_ANYPET,	// ANYPET 생성
	NET_MSG_PET_REQ_USECARD,	// 팻카드 사용
	NET_MSG_PET_REQ_USECARD_FB,
	NET_MSG_PET_REQ_UNUSECARD,	// 팻 소멸
	NET_MSG_PET_REQ_UNUSECARD_FB,
	NET_MSG_PET_REQ_DISAPPEAR,	// 강제로 소멸(맵이동시, 게임종료등...)

	NET_MSG_CREATE_PET_FROMDB_FB,	// 팻생성(DB)
	NET_MSG_GET_PET_FROMDB_FB,	// 팻생성(DB)
	NET_MSG_GET_PET_FROMDB_ERROR,	// 팻생성(DB) 에러

	NET_MSG_PET_REQ_UPDATE_MOVE_STATE,  // 움직임 상태 변경
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB,
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD,

	NET_MSG_PET_REQ_SKILL_CHANGE_CF,	// 스킬변경
	NET_MSG_PET_REQ_SKILLCHANGE_FB,	// 스킬변경
	NET_MSG_PET_REQ_SKILL_CHANGE_FC,	// 스킬변경

	NET_MSG_PET_ADD_SKILLFACT,	// SkillFact 추가
	NET_MSG_PET_REMOVE_SKILLFACT,	// SkillFact 추가

	NET_MSG_PET_REQ_GOTO,	// 이동처리
	NET_MSG_PET_REQ_STOP,	// 이동처리
	NET_MSG_PET_REQ_GOTO_FB,	// 이동처리
	NET_MSG_PET_GOTO_BRD,	// 이동처리
	NET_MSG_PET_STOP_BRD,	// 이동처리

	NET_MSG_PET_REQ_GIVEFOOD,	// 사료주기
	NET_MSG_PET_REQ_GIVEFOOD_FB,	// 사료주기
	NET_MSG_PET_REQ_GETFULL_FROMDB_FB,	// 사료주기

	NET_MSG_PET_GETRIGHTOFITEM,	// 소유권한이 있는 아이템정보 요청
	NET_MSG_PET_GETRIGHTOFITEM_FB,	// 소유권한이 있는 아이템정보 요청
	NET_MSG_PET_NOTENOUGHINVEN,	// 인벤토리가 부족하여 팻을 사라지게 함

	NET_MSG_PET_REQ_RENAME,	// 이름변경
	NET_MSG_PET_REQ_RENAME_FB,	// 이름변경
	NET_MSG_PET_REQ_RENAME_BRD,	// 이름변경
	NET_MSG_PET_REQ_RENAME_FROMDB_FB,	// 이름변경

	NET_MSG_PET_REQ_CHANGE_COLOR,	// 컬러변경
	NET_MSG_PET_REQ_CHANGE_COLOR_FB,	// 컬러변경
	NET_MSG_PET_REQ_CHANGE_COLOR_BRD,	// 컬러변경

	NET_MSG_PET_REQ_CHANGE_STYLE,	// 스타일변경
	NET_MSG_PET_REQ_CHANGE_STYLE_FB,	// 스타일변경
	NET_MSG_PET_REQ_CHANGE_STYLE_BRD,	// 스타일변경

	// 클라이언트 상태 변경 (Field -> Client)
	NET_MSG_PET_UPDATECLIENT_FULL,	// 팻포만감 갱신

	NET_MSG_PET_REQ_INVEN_EX_SLOT,	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_FB,	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD,	// 아이템 탈착(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_INVEN,	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_FB,	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_BRD,	// 아이템 탈착(SLOT->HOLD)
	NET_MSG_PET_REQ_INVEN_TO_SLOT,	// 아이템 탈착(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_FB,	// 아이템 탈착(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_BRD,	// 아이템 탈착(HOLD->SLOT)

	NET_MSG_PET_REQ_LEARNSKILL,	// 팻스킬 배우기
	NET_MSG_PET_REQ_LEARNSKILL_FB,	// 팻스킬 배우기
	NET_MSG_PET_REQ_LEARNSKILL_BRD,	// 팻스킬 배우기

	NET_MSG_PET_REQ_FUNNY,	// Funny Gesture
	NET_MSG_PET_REQ_FUNNY_BRD,	// Funny Gesture

	NET_MSG_PET_REMOVE_SLOTITEM,	// 팻슬롯아이템 제거
	NET_MSG_PET_REMOVE_SLOTITEM_FB,	// 팻슬롯아이템 제거
	NET_MSG_PET_REMOVE_SLOTITEM_BRD,	// 팻슬롯아이템 제거

	NET_MSG_PET_ATTACK,	// 공격모션
	NET_MSG_PET_ATTACK_BRD,	// 공격모션

	NET_MSG_PET_SAD,	// Sad모션
	NET_MSG_PET_SAD_BRD,	// Sad모션

	NET_MSG_PET_REQ_PETCARDINFO,	// 팻카드 정보
	NET_MSG_PET_REQ_PETCARDINFO_FB,	// 팻카드 정보

	NET_MSG_PET_REQ_PETREVIVEINFO,	// 팻부활 정보
	NET_MSG_PET_REQ_PETREVIVEINFO_FB,	// 팻부활 정보

	NET_MSG_PET_REQ_REVIVE,	// 삭제된 팻 부활
	NET_MSG_PET_REQ_REVIVE_FB,	// 삭제된 팻 부활
	NET_MSG_PET_REQ_REVIVE_FROMDB_FB,	// 삭제된 팻 부활

	NET_MSG_PET_ACCESSORY_DELETE,	// 팻악세사리 삭제
	NET_MSG_PET_ACCESSORY_DELETE_BRD,	// 팻악세사리 삭제

	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION,	// 팻 스킬 확장 배우기
	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_FB,	// 팻 스킬 확장 배우기
	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_BRD,	// 팻 스킬 확장 배우기

	//NET_MSG_SMS_PHONE_NUMBER,	// 캐릭터 폰번호 저장/업데이트 요청
	//NET_MSG_SMS_PHONE_NUMBER_FB,	//!
	//NET_MSG_SMS_PHONE_NUMBER_FROM_DB,	//!	DB 저장 결과 FB.
	//NET_MSG_SMS_PHONE_NUMBER_AGTBRD,	//!	Agent에 모든채널 BRD 요청.

	NET_MSG_SMS_SEND,
	NET_MSG_SMS_SEND_FB,
	NET_MSG_SMS_SEND_FROM_DB,

	NET_MSG_GM_MOB_GEN_EX,   // 몹소환(확장기능추가)
	NET_MSG_GM_MOB_GEN_EX_FLD,   // 몹소환(확장기능추가)
    NET_MSG_GM_MOB_DEL_EX,   // 몹제거(확장기능추가)
	NET_MSG_GM_MOB_DEL_EX_FLD,   // 몹제거(확장기능추가)	

    NET_MSG_ODD_EVEN_LOG_FH, //! 홀짝게임 결과 로그 Field->Cache
	NET_MSG_SERVERTIME_BRD,	// 서버 현재시간 BroadCasting

	NET_MSG_GCTRL_MINIGAME, // 미니게임;

	NET_MSG_GCTRL_EVENT, // 이벤트;

	// Instance System; 
	NET_MSG_GCTRL_INSTANCE_SYSTEM, // 인스턴스 던전 시스템;
    NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, // 인스턴스 던전 시스템 (MSGPACK용);
    NET_MSG_GCTRL_INSTANCE_CONTENTS, // 인스턴스 던전시스템을 사용한 컨텐츠용 메시지;
    NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, // 인스턴스 던전시스템을 사용한 컨텐츠용 메시지(MSGPACK용);

	NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_FB,	// Faction 재도전;
	NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_CF,	// Faction 재도전 응답;
	NET_MSG_GCTRL_INSTANCE_SHOW_TIME_UI_FB,		// Show Time UI;
	NET_MSG_GCTRL_INSTANCE_SHOW_ENTRANCE_STATE_UI_FB,	// Show Entrance State UI;
	NET_MSG_GCTRL_INSTANCE_SET_RETURNMAP_INFORMATION_FA,	// ReturnMap Information 세팅;


	// Matching System
	NET_MSG_GCTRL_MATCHING_SYSTEM, // 매칭 시스템
	NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, // 매칭 시스템

	NET_MSG_GCTRL_MATCHING_UPDATE_JOIN_COUNT_AC,	// Join Count 갱신;

    // Faction System;
    NET_MSG_FACTION_SYSTEM,  // 진영 관련 메시지;

	// ping;
	NET_MSG_GCTRL_PING,

	//
	//mjeon.Add
	//Messages for Pingtrace command
	//
	NET_MSG_GM_PINGTRACE_ON,
	NET_MSG_GM_PINGTRACE_ON_FB,
	NET_MSG_GM_PINGTRACE_OFF,
	NET_MSG_GM_PINGTRACE_OFF_FB,


	NET_MSG_GCTRL_SET_SERVER_DELAYTIME,	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB,	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT,	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FLD,	// 서버 장애1
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET,	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB,	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT,	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FLD,	// 서버 장애2
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME,	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB,	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT,	// 서버 장애3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FLD,	// 서버 장애3


	//
	//mjeon.Add
	//Messages for Ping process
	//
	NET_MSG_GM_PING,	//!  Ping 요청
	NET_MSG_GM_PING_FB,	//!  Ping 응답



	NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT,	//로그인서버 MainTenance기능 On/Off 

	NET_MSG_GCTRL_STORAGE_ITEM_UPDATE,	//! 락커 아이템 업데이트
	//NET_MSG_REQ_FRIEND_CLUB_OPEN,	//! 친구,클럽창 열림(클라이언트->에이젼트)

	
	//
	//mjeon.Add
	//Messages for Post
	//

	NET_MSG_POST_REQ_CHANUM,	// ChaNum 요청 to Agent
	NET_MSG_POST_REQ_CHANUM_FB,	// ChaNum 요청에 대한 응답


	// Sender --------------------> FieldS --------------------> Recver (DB Create)
	NET_MSG_POST_SEND,	// 우편 보내기 요청
	NET_MSG_POST_SEND_FB,	// 우편 보내기 응답


	// Recver <-------------------- AgentS <-------------------- FieldS
	NET_MSG_POST_NOTIFY,	// 우편 도착 알림 (Field->Agent )
	NET_MSG_POST_NOTIFY_RELAY,	// 우편 도착 알림 (Agent->Client)


	// Recver <-------------------> AgentS
	NET_MSG_POST_GET,	// 우편 목록 요청
	NET_MSG_POST_GET_INFO_FB,	// 우편 개수 및 정보 전송
	NET_MSG_POST_GET_DATA_FB,	// 실제 우편 데이터 전송


	// Recver가 우편 개봉/확인
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_OPEN,	// 우편 읽기(개봉)
	NET_MSG_POST_OPEN_TAKE,	// 우편 읽기 & 첨부 수취 (Client->Agent)
	NET_MSG_POST_OPEN_TAKE_AF,	// 우편 읽기 & 첨부 수취 (Agent->Field)
	NET_MSG_POST_OPEN_TAKE_AF_FB,	// 우편 읽기 & 첨부 수취 (Field->Agent)
	NET_MSG_POST_OPEN_TAKE_FB,	// 우편 읽기 & 첨부 수취 (Field->Agent)


	// Recver가 우편 반송
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_RETURN,	// 우편 반송 요청
	NET_MSG_POST_RETURN_FB,	// 우편 반송 응답


	// 반송 시에도 Sender에게 POST_NOTIFY 사용


	// Sender가 반송 우편 확인
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_RETURN_OPEN,	// 반송된 우편 읽기(받기)
	NET_MSG_POST_RETURN_OPEN_FB,	// 반송된 우편 읽기(받기) 응답


	// 우편 삭제
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update	
	NET_MSG_POST_DELETE,	// 우편 삭제
	NET_MSG_POST_DELETE_FB,	// 우편 삭제 응답

	
	// Recver <-------------------> AgentS
    NET_MSG_POST_GET_STATE,	// 우편 현황 요청
    NET_MSG_POST_GET_STATE_FB,	// 우편 현황 개수 및 정보 전송
    NET_MSG_POST_GET_SEND_STATE,	// 보낸 우편 현황 요청
    NET_MSG_POST_GET_SEND_STATE_FB,	// 보낸 우편 현황 개수 및 정보 전송
    NET_MSG_POST_GET_RECV_STATE,	// 받은 우편 현황 요청
    NET_MSG_POST_GET_RECV_STATE_FB,	// 받은 우편 현황 개수 및 정보 전송

	NET_MSG_POST_TODO_AFTER_SEND_FF,	//우편 보내기를 한 후에, GaeaServer에서 처리해줄 사항들



	//
	//mjeon.sns
	//

	//
	// --------------------------- FACEBOOK -------------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_FB_AUTHED,	// Facebook 인증정보 저장
	NET_MSG_SNS_FB_AUTHED_FB,	// Facebook 인증정보 저장 응답
	NET_MSG_SNS_FB_UNAUTHED,	// Facebook 인증정보 삭제
	NET_MSG_SNS_FB_UNAUTHED_FB,	// Facebook 인증정보 삭제 응답	
	NET_MSG_SNS_FB_GETUID,	// Facebook UID 요청	
	NET_MSG_SNS_FB_GETUID_FB,	// Facebook UID 응답

	// AgentS <-------------------> FieldS ---------------------> DB
	NET_MSG_SNS_FB_UPDATE_AF,	// FieldServer의 FB 정보 갱신 - 단방향

	// FieldS <---------- Broadcasting to ViewAround -----------> Client(s)
	NET_MSG_SNS_FB_UPDATE_BR,	// FieldServer -> Clients - 단방향



	//
	// ---------------------------- TWITTER -------------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_TW_AUTHED,	// Twitter 인증정보 저장
	NET_MSG_SNS_TW_AUTHED_FB,	// Twitter 인증정보 저장 응답
	NET_MSG_SNS_TW_UNAUTHED,	// Twitter 인증정보 삭제
	NET_MSG_SNS_TW_UNAUTHED_FB,	// Twitter 인증정보 삭제 응답
	NET_MSG_SNS_TW_GETUID,	// Twitter UID 요청	
	NET_MSG_SNS_TW_GETUID_FB,	// Twitter UID 응답
	
	// AgentS <-------------------> FieldS ---------------------> DB
	NET_MSG_SNS_TW_UPDATE_AF,	// FieldServer의 TW 정보 갱신 - 단방향

	// FieldS <---------- Broadcasting to ViewAround -----------> Client(s)
	NET_MSG_SNS_TW_UPDATE_BR,	// FieldServer -> Clients - 단방향

	//
	// --------------------------- SNS Common -----------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_GETINFO,	// SNS 정보 요청
	NET_MSG_SNS_GETINFO_FB,	// SNS 정보 응답
	

	NET_MSG_GM_GETWHISPERMSG,	//!	캐릭터의 귓속말을 엿듣는다.
	NET_MSG_CHINA_GAINTYPE  ,	//!	중국 등급별 수익

	NET_MSG_GM_LIMIT_EVENT_BEGIN,	//!	이벤트 시작.
	NET_MSG_GM_LIMIT_EVENT_END,	//!	이벤트 종료.

	NET_MSG_GM_LIMIT_EVENT_RESTART,	//!	이벤트 다시 시작.

	NET_MSG_GM_LIMIT_EVENT_APPLY_START,	//!	클라이언트에 이벤트 적용 시작했음을 알림
	NET_MSG_GM_LIMIT_EVENT_APPLY_END,	//!	클라이언트에 이벤트 적용 끝났음을 알림

	NET_MSG_GM_LIMIT_EVENT_BEGIN_FB,   //!	클라이언트에 이벤트가 시작 되었음을 알림
	NET_MSG_GM_LIMIT_EVENT_END_FB,   //!	클라이언트에 이벤트가 종료 되었음을 알림

	NET_MSG_GM_LIMIT_EVENT_TIME_REQ,   //!	클라이언트가 서버에 이벤트 정보를 요청
	NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB,   //!	서버가 클라이언트에 이벤트 정보를 보냄
	NET_MSG_GM_LIMIT_EVENT_TIME_RESET,   //!	AGENT서버가 FIELD서버에게 로그인 시간을 RESET하라고 보냄

	NET_MSG_GM_PRINT_CROWLIST,   //!	자신이 있는 맵의 몹 리스트를 프린트함.
	NET_MSG_GM_PRINT_CROWLIST_FLD,   //!	자신이 있는 맵의 몹 리스트를 프린트함.
	NET_MSG_GM_PRINT_CROWLIST_FB,   //!	자신이 있는 맵의 몹 리스트를 프린트함.

	NET_MSG_SERVER_CTRL_WEATHER2,	//!	서버의 날씨 변경. 새로운 메시지
	NET_MSG_GCTRL_MAPWEATHER,	
	NET_MSG_GCTRL_MAPWHIMSICAL,

	NET_MSG_CHAR_ACTIVE_VEHICLE_CF,	//! 클라이언트가 탈것을 활성/비활성 요청
	NET_MSG_CHAR_ACTIVE_VEHICLE_FC,	//! 클라이언트에 탈것을 활성/비활성 알림
	NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD,	//! 주위 클라이언트에 탈것을 활성/비활성 알림

	NET_MSG_GCTRL_GET_VEHICLE,	//! 클라이언트가 탈것의 정보 요청
	NET_MSG_GCTRL_GET_VEHICLE_FB,	//! 클라이언트에 탈것의 정보 받음
	NET_MSG_GCTRL_GET_VEHICLE_BRD,	//! 주위 클라이언트에 탈것 정보 전송
	NET_MSG_GCTRL_UNGET_VEHICLE_FB,	//! 탈것 정보 비활성화
	NET_MSG_GCTRL_UNGET_VEHICLE_BRD,	//! 탈것 정보 비활성화

	NET_MSG_VEHICLE_CREATE_FROMDB_FB,	//! 최초 탈것 생성 ( DB ) 
	NET_MSG_VEHICLE_GET_FROMDB_FB,	//! 탈것 생성 ( DB )
	NET_MSG_VEHICLE_GET_FROMDB_ERROR,	//! 탈것 생성 ( DB에러 )
	
	NET_MSG_VEHICLE_ACCESSORY_DELETE,	//! 탈것의 악세서리 삭제
	NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD,	// 탈것의 악세사리 삭제 알림
	
	NET_MSG_VEHICLE_REQ_INVEN_EX_SLOT,	// 탈것 slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB,	// 탈것 slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD,	// 탈것 slot ex hold
	
	NET_MSG_VEHICLE_REQ_INVEN_TO_SLOT,	// 탈것 hold to slot
	NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB,	// 탈것 hold to slot
	
	NET_MSG_VEHICLE_REQ_SLOT_TO_INVEN,	// 탈것 slot to hold
	NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB,	// 탈것 slot to hold
	
	NET_MSG_VEHICLE_REMOVE_SLOTITEM,	// item slot 해제
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB,	// item slot 해제
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD,	// item slot 해제

	NET_MSG_VEHICLE_REQ_GIVE_BATTERY,	// 배터리 충전
	NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB,	// 배터리 충전
	NET_MSG_VEHICLE_REQ_GETFULL_FROMDB_FB,	// 배터리 충전
	NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY,	// 배터리 갱신
	NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB,	// 배터리 가져오기 ( 내부 )

	NET_MSG_VEHICLE_REQ_ITEM_INFO,	// 탈것아이템 정보
	NET_MSG_VEHICLE_REQ_ITEM_INFO_FB,	// 탈것아이템 정보

	NET_MSG_VIETNAM_GAINTYPE,	// 베트남 탐닉 방지 시스템 현재 수익 타입
	NET_MSG_VIETNAM_TIME_REQ,	// 베트남 탐닉 방지 시스템 시간 요청
	NET_MSG_VIETNAM_TIME_REQ_FB,	// 베트남 탐닉 방지 시스템 시간 요청
	NET_MSG_VIETNAM_GAIN_EXP,	// 베트남 탐닉 방지 시스템 경험치
	NET_MSG_VIETNAM_GAIN_MONEY,	// 베트남 탐닉 방지 시스템 돈
	NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD,   //!	탐닉방지 인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD,	//!	탐닉방지 인밴에 있는 아이탬을 들기 시도.
	NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN,	//!	아이템을 탐닉방지 인밴에 넣음
	NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET,	//! 탐닉 방지 인밴 전체 초기화
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN,	//! 오른쪽 버튼으로 탐닉 방지 인밴에서 인밴으로 아이템 옮길때
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB,	//! 오른쪽 버튼으로 탐닉 방지 인밴에서 인밴으로 아이템 옮길때
	NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET,	//!	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB,	//!	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB,	//!	베트남 탐닉 방지 아이템 사용시 메시지
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE,	//!	베트남 탐닉 방지 아이템 사용시 메시지 
	NET_MSG_VIETNAM_ALLINITTIME,	//!	베트남 탐닉 방지시스템 12시가 지나서 초기화 되었을때
	
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE,	// 성별 변경
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB,	// 성별 변경

	NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE,	// 학원 변경
	NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE_FB,	// 학원 변경

	NET_MSG_MARKET_CHECKTIME,	// 상점이 열려있을 때 10분마다 메시지를 보내 튕김 현상을 방지한다.

	NET_MSG_CHAT_CTRL_FB, //! 관리자용 채팅메시지 정의 ( 게임서버->클라이언트 )

	NET_QBOX_OPTION_REQ_AG, //! QBox On/Off 옵션 마스터 -> 에이전트 서버
	NET_QBOX_OPTION_REQ_FLD, //! QBox On/Off 옵션 에이전트 서버 -> 필드서버
	NET_QBOX_OPTION_MEMBER, //! QBox On/Off 옵션 필드 서버 -> 멤버에게..

	//NET_MSG_UPDATE_TRACING_ALL, //! 추적할 캐릭터를 전부 업데이트한다. // 패킷 랜덤 으로 인해서 상단으로 이동 2014.12.18
	//NET_MSG_UPDATE_TRACING_CHARACTER, //! 추적할 캐릭터를 특정 캐릭터만 업데이트한다. // 패킷 랜덤 으로 인해서 상단으로 이동 2014.12.18
	NET_MSG_LOG_UPDATE_TRACING_CHARACTER, //! // 추적중인 유저의 로그를 agent서버에 보낸다.

	NET_MSG_TRACING_CHARACTER, //! 필드->클라이언트 추적 유저인지 아닌지를 보낸다.

	NET_MSG_GCTRL_REQ_TELEPORT,	//!	소환서 아이템 사용 요청.
	NET_MSG_GCTRL_REQ_TELEPORT_FB,	//!	소환서 아이템 사용 응답.
	NET_MSG_GCTRL_REQ_TELEPORT_AG,	//!	소환서 에이젼트 서버에 요청.

	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC,	// 경험치 회복 데이터 요청 (NPC)
	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB,	// 경험치 회복 데이터 요청 FB (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC,	// 경험치 회복 사용 (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC_FB,	// 경험치 회복 사용 FB (NPC)
	NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP,	//!	퀘스트 맵이동 제한.
	NET_MSG_GM_CLASS_EVENT,	//!	클래스별 이벤트

	NET_MSG_VIETNAM_TIME_RESET,	// 베트남 시간 리셋 //vietnamtest%%% && vietnamtest2

	NET_MSG_GCTRL_ITEMSHOPOPEN,	//!	ItemShopOpen ( Japan ) 
	NET_MSG_GCTRL_ITEMSHOPOPEN_BRD,	//!	ItemShopOpen ( Japan ) 

	NET_MSG_GM_VIEWWORKEVENT,	//! 현재 agent서버에 적용된 모든 이벤트 보기
	NET_MSG_GM_VIEWWORKEVENT_FB,	//! 현재 agent서버에 적용된 모든 이벤트 보기 회신

	NET_MSG_PET_PETSKINPACKOPEN,	//! 펫 스킨 팩 아이템 사용
	NET_MSG_PET_PETSKINPACKOPEN_FB,	//! 펫 스킨 팩 아이템 사용
	NET_MSG_PET_PETSKINPACKOPEN_BRD,	//! 펫 스킨 팩 아이템 사용

	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM,	//! 개인상점의 아이템 검색
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT,	//! 개인상점의 아이템 검색 결과
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ,	//! 개인상점의 아이템 검색 결과 요청
	
	NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE,	//! 피격시 스킬 발동

	NET_MSG_GCTRL_BEGIN_SKILL_FC, // 스킬 예약 후 등록 완료 되면 서버->클라 메세지 송신;
	NET_MSG_GCTRL_END_SKILL_FC, // 스킬이 종료 되었을 경우 서버->클라 메세지 송신;
	NET_MSG_GCTRL_END_SKILL_BRD, // 스킬이 종료 되었을 경우 서버->주변클라 메세지 송신;
	
	NET_MSG_REQ_ATTENDLIST,	//! 출석정보 리스트 요청
	NET_MSG_REQ_ATTENDLIST_FB,	//! 출석정보 리스트 결과
	NET_MSG_REQ_ATTENDANCE,	//! 출석 체크
	NET_MSG_REQ_ATTENDANCE_FB,	//! 출석 체크 답변
//	NET_MSG_REQ_ATTEND_REWARD_FLD,	//! 출석 체크 이벤트 아이템 지급 ( 필드용 )	
	NET_MSG_REQ_ATTEND_REWARD_CLT,	//! 출석 체크 이벤트 아이템 지급 ( 클라용 )

	NET_MSG_REQ_USE_SUMMON,	//! 소환을 요청
	NET_MSG_REQ_USE_SUMMON_EVENT_SKILL,	//! 소환수 이벤트 스킬;
	NET_MSG_HIRE_BASICSUMMON_CF,	//! 소환수 고용;
	NET_MSG_HIRE_BASICSUMMON_FC,	//! 소환수 고용 응답;
	NET_MSG_REQ_USE_SUMMON_FB,	//! 소환을 요청 응답
	NET_MSG_GCTRL_DROP_SUMMON,	//! 소환수 생성
	NET_MSG_GCTRL_FAILED_DROP_SUMMON,	//! 소환 실패;
	NET_MSG_GCTRL_SUMMON_GOTO,	//! 소환수 이동
	NET_MSG_GCTRL_SUMMON_ATTACK,	//! 소환수 공격
	NET_MSG_GCTRL_SUMMON_ATTACK_AVOID,	//! 소환수 공격 회피
	NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE,	//! 소환수 공격 데미지
	NET_MSG_GCTRL_SUMMON_SKILL,	//! 소환수 공격
	NET_MSG_GCTRL_SUMMON_END_SKILL,	//! 소환수 스킬 종료
	NET_MSG_GCTRL_SUMMON_EFFECT,	//! 소환수 이펙트 Render;
	NET_MSG_GCTRL_SUMMON_UPDATE_CUR_SKILLPAGE,	//! 소환수 스킬 페이지 업데이트;
	NET_MSG_GCTRL_SUMMON_UPDATE_STATE,	//! 소환수 상태 업데이트
	NET_MSG_GCTRL_SUMMON_LANDEFFECT_UPDATE,	//! 소환수 지형 효과 업데이트

	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE,	//! 얼굴 스타일 변경 요청
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_FB,	//! 얼굴 스타일 변경 결과
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_BRD,	//! 얼굴 스타일 변경 결과

	NET_MSG_REQ_MUST_LEAVE_MAP,	//! 레벨업시 조건이 맞지않아 맵을 떠나야 할 때
	NET_MSG_REQ_MUST_LEAVE_MAP_FB,	//! 레벨업시 조건이 맞지않아 맵을 떠나야 할 때
	NET_MSG_REQ_MUST_LEAVE_MAP_AG,	//! 레벨업시 조건이 맞지않아 맵을 떠나야 할 때

	NET_MSG_REQ_EXPEDITION_LEAVE_MAP, //! 원정대 조건이 맞지 않아 맵을 떠나야 할 때
	NET_MSG_REQ_EXPEDITION_LEAVE_MAP_FB, //! 원정대 조건이 맞지 않아 맵을 떠나야 할 때
	NET_MSG_REQ_EXPEDITION_LEAVE_MAP_AG, //! 원정대 조건이 맞지 않아 맵을 떠나야 할 때

	NET_MSG_CYBERCAFECLASS_UPDATE,	//! 태국 사이버 카페 등급 업데이트
	
	NET_MSG_GCTRL_REQ_TAXI,	//! 택시 탑승 요청
	NET_MSG_GCTRL_REQ_TAXI_FB,	//! 택시 탑승 요청 결과
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS,	//! 택시 탑승시 NPC좌표 요청
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB,	//! 택시 탑승시 NPC좌표 요청 결과
	
	NET_MSG_GCTRL_ETNRY_FAILED,	//! 진입 실패시 메시지
	
	NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE,	//! 쿨타임 업데이트
	NET_MSG_GCTRL_ITEM_COOLTIME_ERROR,	//! 쿨타임에 걸려서 사용못함

	NET_MSG_GCTRL_NPC_RECALL,	//! NPC 호출.
	NET_MSG_GCTRL_NPC_RECALL_FB,	//! NPC 호출 결과.

	NET_MSG_GCTRL_NPC_COMMISSION,	//! NPC 호출 COMMISION 업데이트 요청 ( Field->Agent ) 
	NET_MSG_GCTRL_NPC_COMMISSION_FB,	//! NPC 호출 COMMISION 결과 ( Agent -> Field ) 

	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD,	//! 인스턴스 맵 생성 AGENT -> FIELD
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB,	//! 인스턴스 맵 생성 요청 피드백
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ,	//! 인스턴스 맵 요청 CLIENT -> FIELD -> AGENT
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_NOGATE_REQ, //! 인스턴스 맵 요청 CLIENT -> FIELD -> AGENT
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL,	//! 인스턴스 맵 삭제 
	//NET_MSG_GM_TESTCREATE_INSTANT_MAP_REQ,	//! 인스턴스 맵 연속 테스트 요청 CLIENT -> FIELD -> AGENT

	NET_MSG_GM_SHOP_INFO_REQ,	//! 상점의 정보 요청(GM명령어)
	NET_MSG_GM_SHOP_INFO_FB,	//! 상점의 정보 응답(GM명령어)

	NET_MSG_GCTRL_INVEN_ITEM_MIX,	//! 아이템 조합 요청
	NET_MSG_INVEN_ITEM_MIX_FC,	//! 아이템 조합 요청 결과

	NET_MSG_GCTRL_LIMITTIME_OVER,	//! 진입 실패시 메시지

	NET_MSG_LOGIN_MAINTENANCE_SESSION,	// 세션으로 LoginMaintenance 정보 On/Off
	NET_MSG_LOGIN_MAINTENANCE_LOGIN,	// 로그인으로 LoginMaintenance 정보 On/Off
	
	NET_MSG_REQ_GATHERING,	//! 채집 요청
	NET_MSG_REQ_GATHERING_FB,	//! 채집 요청 결과
	NET_MSG_REQ_GATHERING_BRD,	//! 채집 요청 결과
	NET_MSG_REQ_GATHERING_RESULT,	//! 채집 결과
	NET_MSG_REQ_GATHERING_RESULT_BRD,	//! 채집 결과
	NET_MSG_REQ_GATHERING_CANCEL,	//! 채집중 중지
	NET_MSG_REQ_GATHERING_CANCEL_BRD,	//! 채집중 중지

	NET_MSG_GCTRL_UPDATE_CP,	//!	CP 업데이트 

	NET_MSG_GCTRL_INSTANTMAP_MOVEOUT_REQ,	//!	인스턴트맵 밖으로 강제 이동

	NET_MSG_INSTANT_MAP_REENTRY_CHECK_REQ,	//!	인스턴트맵 재입장 시간 요청
	NET_MSG_INSTANT_MAP_REENTRY_CHECK_FB,	//!	인스턴트맵 재입장 시간 알림

	NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD,	//!	CDM 무적시간 알림 BRD
	
	NET_MSG_GCTRL_REQ_NPCTALK_BASIC,	//!	NPC Talk 인증요청
	NET_MSG_GCTRL_REQ_NPCTALK_BASIC_FB,	//!	NPC Talk 인증요청 FB

    NET_MSG_CHAT_PARTY_RECRUIT_FB_AF,	// 파티모집채팅 메세지 FB
	NET_MSG_CHAT_AREA_FB,	// 지역채팅 메세지 FB
	NET_MSG_CHAT_PARTY_RECRUIT_FB,	// 파티모집채팅 메세지 FB
	NET_MSG_CHAT_FACTION_FB,	// 진영채팅 메세지 FB
	NET_MSG_GCTRL_PARTY_LINK_JOIN,	// 파티모집채팅 가입 요청
	NET_MSG_GCTRL_PARTY_LINK_JOIN_FB,	// 파티모집채팅 가입 요청 FB
	NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS,	// 파티모집채팅 가입 요청 보냄 ( 서버 -> 마스터 ) 
	NET_MSG_GCTRL_PARTY_LINK_LURE,	// 파티모집채팅 수락
	NET_MSG_GCTRL_PARTY_LINK_LURE_FB,	// 파티모집채팅 수락 FB
	NET_MSG_GCTRL_PARTY_LINK_REFUSE,	// 파티모집채팅 거절
	NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB,	// 파티모집채팅 거절 보냄 ( 서버 -> 요청자 ) 	


	NET_MSG_GM_CHECKINSTANTNUM,	// 인던 생성 개수 체크 메시지
	NET_MSG_GM_CHECKINSTANTNUM_FB,	// 인던 생성 개수 체크 메시지 FB

    NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE, // 소환문 랜드 이벤트 생성 (1635 ~ 1644 까지 임시)
    NET_MSG_GCTRL_LANDEVENT_REMOVE, // 랜드이벤트 삭제

	NET_MSG_GCTRL_ATTACK_BLOCKING, // 유저 블로킹 메세지
	NET_MSG_GCTRL_ATTACK_BLOCKING_BRD, // 유저 블로킹 메세지 알림
	NET_MSG_GCTRL_CROW_ATTACK_BLOCKING, // 몹 블로킹 메세지 알림
	NET_MSG_GCTRL_SUMMON_ATTACK_BLOCKING, // 소환수 블로킹 메세지 알림


	NET_MSG_GCTRL_WITH_VEHICLE,   //! 오토바이 같이타기
	NET_MSG_GCTRL_WITH_VEHICLE_TAR,   //! 오토바이 같이타기 상대편
	NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS,   //! 오토바이 같이타기 ANS
	NET_MSG_GCTRL_WITH_VEHICLE_FB,   //! 오토바이 같이타기 FB
	NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_FB,	//! 뒷자리 탑승 관련 메세지
	NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_BRD,	//! 주위 클라이언트에 탈것을 활성/비활성 알림
	NET_MSG_GCTRL_WITH_VEHICLE_MOVE_MAP,	//! 맵이동시 탑승 캐릭터 맵 이동 요청
	NET_MSG_GCTRL_VEHICLE_BRD,	// 바이크정보 DropChar에서 분리
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE,	// 탈것 컬러 변경
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_FB,	// 탈것 컬러 변경 FB
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_BRD,	// 탈것 컬러 변경 BRD
	NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE,	// 바이크 내리기
	NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE_FB,	// 바이크 내리기
	NET_MSG_GCTRL_LEARN_BIKE_BOOST,	// 바이크 부스트 습득
	NET_MSG_GCTRL_LEARN_BIKE_BOOST_FB,	// 바이크 부스트 습득 FB
	NET_MSG_GCTRL_LEARN_BIKE_BOOST_BRD,	// 바이크 부스트 습득 BRD
	NET_MSG_GCTRL_USE_BIKE_BOOST,	// 바이크 부스트 사용
	NET_MSG_GCTRL_USE_BIKE_BOOST_FB,	// 바이크 부스트 사용 FB
	NET_MSG_GCTRL_USE_BIKE_BOOST_BRD,	// 바이크 부스트 사용 BRD
	NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_FB,	// 바이크 부스트 종료 FB
	NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_BRD,	// 바이크 부스트 종료 BRD

	NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX, // 인벤 도시락 사용
	NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX_FB, // 인벤 도시락 사용 FB
	NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_CF,	// 아이템 분해 요청;
	NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_FB,	// 아이템 분해 결과;
	
	NET_MSG_GCTRL_TRADE_LOCK,	//! 트래이드 락.
	NET_MSG_GCTRL_TRADE_LOCK_TAR,	//!트래이드 락. field -> agent.

	NET_MSG_GM_NONCONFRONT_MODE,	// 대련 불가능 모드 GM 명령어
	NET_MSG_GM_NONCONFRONT_MODE_VIEW_REQ,	// 대련 불가능 모드 맵 보기 요청
	NET_MSG_GM_NONCONFRONT_MODE_VIEW_FB,	// 대련 불가능 모드 맵 보기 응답

	NET_MSG_GCTRL_POSTBOX_OPEN, // 우편함 열기 요청
	NET_MSG_GCTRL_POSTBOX_OPEN_FB, // 우편함 열기 요청 FB
	NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD, // 우편함 열기 요청 (카드)
	NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD_FB, // 우편함 열기 요청 FB (카드)
	NET_MSG_GCTRL_POSTBOX_CLOSE, // 우편함 닫기 알림

    NET_MSG_GM_GAME_DATA_UPDATE_CA,
    NET_MSG_GM_GAME_DATA_UPDATE_AS,
    NET_MSG_GM_GAME_DATA_UPDATE_SS,

	// 코스튬 염색 관련. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE,	// 코스튬 컬러 변경. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_FB,	// 코스튬 컬러 변경 FB. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_BRD,	// 바이크컬러 변경 BRD


	NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD,	// 맵효과 BRD
	NET_MSG_GCTRL_SUMMON_MAPEFFECT_UPDATE,	// 맵효과 BRD

    NET_MSG_GM_LOG_ITEM_RELOAD_CA,
    NET_MSG_GM_LOG_ITEM_RELOAD_AS,
    NET_MSG_GM_LOG_ITEM_RELOAD_SS,

    NET_MSG_GCTRL_REQ_ITEM_SORT,   // Cleint -> Filed로 아이템 정렬 요청.
	NET_MSG_GCTRL_REQ_ITEM_SORT_START,   // Cleint -> Filed로 아이템 정렬 요청 시작. 기존의 NET_MSG_GCTRL_REQ_ITEM_SORT 를 분리
	NET_MSG_GCTRL_REQ_ITEM_SORT_DATA,   // Cleint -> Filed로 아이템 정렬 요청 시작. 기존의 NET_MSG_GCTRL_REQ_ITEM_SORT 를 분리
	NET_MSG_GCTRL_REQ_ITEM_SORT_END,	// Cleint -> Filed로 아이템 정렬 요청 종료. 기존의 NET_MSG_GCTRL_REQ_ITEM_SORT 를 분리
    NET_MSG_GCTRL_ITEM_SORT_FAIL,   // Filed -> Client로 아이템 정렬 실패 알림.

    NET_MSG_GM_WHISPER_STATE,   // Client -> Agent로 귓속말 허용 상태 변경 요청.
    NET_MSG_GM_WHISPER_STATE_FB,   // Agent  -> Client로 귓속말 허용 상태 변경 알림.

    NET_MSG_GCTRL_REQ_SKILL_REVIVE,   // 부활대상자 부활 요청


	//PVE 무한의 재단
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_CF, // 스테이지 패스 사용 CF
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF, // 스테이지 패스 사용 CF
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_FB, // 스테이지 패스 사용 FB

	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_CF, // 인던 입장횟수 충전권
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA,
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_AF,
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FC,

    NET_MSG_GCTRL_INVEN_CONSIGNEMENT_SLOT_EXPIRATION_REQ, // 위탁판매 슬롯 확장 요청(카드아이템)


	//
	//mjeon.Attendance
	//Messages for AttendanceSystem
	//
	NET_MSG_ATTENDANCE_SYNC_FF,			// Attendance Sync to Field from DBthread
	NET_MSG_ATTENDANCE_RESET_FF,		// Attendance Reset to Field from DBthread
	NET_MSG_ATTENDANCE_UPGRADE_FF,		// Attendance Upgrade to Field from DBthread
	NET_MSG_ATTENDANCE_SYNC_FAC,		// Attendance Sync to Client (No Reset or No Upgrade)
	NET_MSG_ATTENDANCE_RESET_FAC,		// Attendance Reset to Client
	NET_MSG_ATTENDANCE_UPGRADE_FAC,		// Attendance Upgrade to Client
	NET_MSG_ATTENDANCE_TASK_UPDATE_FC,	// Attendance Task Update
	NET_MSG_ATTENDANCE_TASK_RESET_FC,	// Attendance Task Reset (only valid for EnchantWC, EnchantAC)
	NET_MSG_ATTENDANCE_TASK_COMPLETED_FC,	// Attendance Task Completed ( Attendance Completed = Task0 completed)
	NET_MSG_ATTENDANCE_POPUP_COMPLETE_FC,	// notifying Attendance completed to viewaround
	NET_MSG_ATTENDANCE_DAILYMAINTENANCE_START_AF,	// notifying Attendance Daily maintenance starts
	NET_MSG_ATTENDANCE_DAILYMAINTENANCE_READY_FA,	// preparation for maintenance at the fieldS has been finished. Just start maintenance.
	NET_MSG_ATTENDANCE_DAILYMAINTENANCE_END_AF,		// notifying Attendance Daily maintenance starts
	NET_MSG_ATTENDANCE_TAKEREWARD_CF,	// Take all rewards for this Task
	NET_MSG_ATTENDANCE_TAKEREWARD_FB,	// Response of NET_MSG_ATTENDANCE_TAKEREWARD_CF
	NET_MSG_ATTENDANCE_DONE_POINTITEM_CF,	// PointShop 장바구니에 출석부 과제에서 요구한 만큼의 아이템을 넣었다.
	NET_MSG_ATTENDANCE_DONE_POINTITEM_FB,	// PointShop 장바구니에 출석부 과제에서 요구한 만큼의 아이템을 넣었다.에 대한 응답
	NET_MSG_ATTENDANCE_WHISPER_DONE_AF,	// Whisper msg has been sent to the target client via Agent
	NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA,	// GM requests to do daily-maintenance manually
	NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB,	// Response of Manual AttendanceMaintenance
	NET_MSG_ATTENDANCE_LOAD_END,

    
	// Messages for rebuy list.
    // AgentS
    NET_MSG_ADD_REBUY_ITEM_FA, // 재매입 리스트에 추가. field -> agent.
    NET_MSG_ADD_REBUY_ITEM_AC, // 재매입 리스트에 추가 결과. agent -> client.
    NET_MSG_BUY_REBUY_ITEM_CA, // 재매입 리스트에서 아이템 구매. client -> agent.
    NET_MSG_BUY_REBUY_ITEM_AF, // 재매입 리스트에서 아이템 구매. agent -> field.
    NET_MSG_BUY_REBUY_ITEM_FC, // 재매입 리스트에서 아이템 구매 결과. field -> client.
    NET_MSG_BUY_REBUY_ITEM_FA, // 재매입 리스트에서 아이템 구매 결과. field -> agent.
    NET_MSG_CHECK_REBUY_SUPPLIES, // NPC 재매입 ui에서 소모품 아이템 체크.
	

	//
	//mjeon.Activity
	//Messages for Activities
	//
	NET_MSG_ACTIVITY_COMPLETED_FA,		// Activity 완료 통보(Agent서버로)
	NET_MSG_ACTIVITY_CLOSED_FA,			// Activity 완료 불가능 상태(선착순 종료)
	NET_MSG_ACTIVITY_COMPLETED_FF,		// Activity 완료 통보(DbAction에서 Field로)
	NET_MSG_ACTIVITY_NOTIFY_COMPLETED,	// Activity 완료 (to 여러 clients)
	NET_MSG_ACTIVITY_NOTIFY_CLOSED,		// Activity 완료 불가능 상태(선착순 종료) (to 여러 clients)
	NET_MSG_ACTIVITY_UPDATE_VALUE,		// Activity 진행상황 업데이트 (to 특활을 수행한 본인)
	NET_MSG_ACTIVITY_UPDATE_COMPLETED,	// Activity 완료상태로 업데이트 (to 특활을 수행한 본인)
	NET_MSG_ACTIVITY_POPUP_COMPLETE,	// Activity 완료 팝업 (to viewaround)
	NET_MSG_ACTIVITY_DONE_ITEMTAKE,		// Activity 아이템획득 업적 달성요청
	NET_MSG_ACTIVITY_DONE_ITEMTAKE_FB,	// Activity 아이템획득 업적 달성응답
	NET_MSG_ACTIVITY_SYNC,				// Activity 현황을 DB(Server)와 동기화 (최초 접속시)
	NET_MSG_ACTIVITY_REWARD_POINT_SYNC,	// Activity point 동기화
	NET_MSG_ACTIVITY_REWARD_BADGE_SYNC,	// Activity badge 동기화
	NET_MSG_ACTIVITY_CHANGE_BADGE,		// Activity badge 변경
	NET_MSG_ACTIVITY_CHANGE_BADGE_FB,	// Activity badge 변경응답
	NET_MSG_ACTIVITY_LOAD_COMPLETED_FF,	// ActivityLoad() completed
	NET_MSG_TITLE_NOTIFY_CHANGE,	// Title 변경 알림 (to viewaround)


	//
	//mjeon.CaptureTheField
	//
	NET_MSG_CTF_READY_AF,			// Notify all FieldServers that CTF is ready.
	NET_MSG_CTF_READY_AC,			// Notify all clients that CTF is ready.	
	NET_MSG_CTF_WARNING4DROP_FC,	// Notify all clients in the CTF land that they will be dropped if not registered for CTF.
	NET_MSG_CTF_JOIN_CF,			// The client requests to register for CTF.	
	NET_MSG_CTF_JOIN_FA,			// Send Join request to AgentS with result of checking conditions.
	NET_MSG_CTF_JOIN_RESULT_AC,		// Response of Join msgs.
	NET_MSG_CTF_UPDATE_QUEUE_AC,	// Update when a client canceled join.
	NET_MSG_CTF_PREPARE_AF,			// Notify all fields in channel0 starting of CTF. FieldServer will prepare for CTF.
	NET_MSG_CTF_PREPARE_AF_FB,		// Response of prepare-msg.	
	NET_MSG_CTF_RECALL_TO_AF,		// Recall chars to the specific position of FieldSvr which handle CTF.
	NET_MSG_CTF_START_AF,			// Notify CTF-Field starting of CTF.
	NET_MSG_CTF_START_2ALL_AC,		// Notify all clients starting of CTF.
	NET_MSG_CTF_START_2PLAYERS_FC,	// Notify only joiners starting of CTF.	
	NET_MSG_CTF_STOP_AF,			// Notify all fields in channel0 stop of CTF that means the 1-hr has passed.
	NET_MSG_CTF_STOP_AC,			// Notify all clients stop of CTF.
	NET_MSG_CTF_END_FA,				// Notify end of CTF to agent that means a school has captured the field.
    NET_MSG_CTF_DONE_AF_AC,			// Notify CTF done to CTF-FieldS and the CTF-FieldS will response with CTF-result.
	NET_MSG_CTF_DONE_AF_FB,			// Response of Done msg with CTF-result.
	NET_MSG_CTF_REWARD_FC,			// Notify all clients reward time starts.
	NET_MSG_CTF_REWARD_INFO,		// used to save/load reward info. (used in several cases need to get reward info: AF, AH, HF).
	NET_MSG_CTF_QUIT_CFA,			// Quit CTF during it's on the play or reward time.
	NET_MSG_CTF_SYNC_PLAYERNUM_AF,	// Sync the number of players for all schools.
	NET_MSG_CTF_CROW_AUTHED,		// An Authenticator has been authed.
	NET_MSG_CTF_SYNC_STATUS_AC,		// Sync the status of CTF to clients newly connected.	
	NET_MSG_CTF_LOAD_REWARD_INFO_AH,// Request to Load the CTF result	
	NET_MSG_CTF_STAT_LOG_AH,		// Log current CTF status for statistics
    NET_MSG_CTF_QUIT_AF,			// Notify Quit CTF during it's on the play to FieldServer    
	NET_MSG_CTF_UPDATE_PLAYERS_VECTOR_FC,	// Update players' vector for minimap update
	NET_MSG_CTF_STATISTICS_FC,		// Statistics for result of CTF.

	NET_MSG_GET_CHA_EXTRA_INFO_FH,	// Get character's extra info (incl. CTF ContributionPoint)
	NET_MSG_GET_CHA_EXTRA_INFO_DH,	// character's extra info from DB to CacheServer
	NET_MSG_GET_CHA_EXTRA_INFO_FH_FB,	// Response of above request msg.
	NET_MSG_SET_CHA_EXTRA_INFO_FH,	// Set character's extra info

	NET_MSG_CTF_MOVESTATE_BY_GM_CA,	// GM command to change CTF state
	NET_MSG_CTF_MOVESTATE_BY_GM_CA_FB,	// response of GM command to change CTF state

	NET_MSG_CTF_RANKING_FAC,		// ranking info

	NET_MSG_GM_NETWORKPROFILE_REQ,

	NET_MSG_GM_CHANGE_WBJOIN_STATE,

	NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE,
	NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_ANS,
	NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_CHAR_SAVE,
	NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_DB,

	NET_MSG_GM_RANMOBILE_COMMAND_REQUEST, // 란모바일 테스트 명령어
	NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE, // 란모바일 테스트 명령어

	NET_MSG_GM_REQ_USERCOUNT_LAND_CF,
	NET_MSG_GM_RESPONSE_USERCOUNT_LAND_FC,
	//
	//mjeon.instance
	//
	NET_MSG_MATCH_REQUEST_CA,		//Match or UnMatch
	NET_MSG_MATCH_REQUEST_CA_FB,
	NET_MSG_MATCH_REQUEST_AM,		//Match or UnMatch
	NET_MSG_MATCH_REQUEST_RESULT_MAC,

	NET_MSG_MATCH_CANCEL_CAM,
	NET_MSG_MATCH_CANCEL_CAM_FB,
	
	//FieldServer -> InstanceServer
	NET_MSG_TRANSFER_START_MA,		//the instance has been created, let's start transferring characters	
	NET_MSG_TRANSFER_CHAR,			//FAF or FAMI (M: Match, I: Instance)
	NET_MSG_TRANSFER_REQUEST_AF,
	NET_MSG_TRANSFER_REQUEST_AF_FB,
	NET_MSG_TRANSFER_CHAR_DONE_FA,
	NET_MSG_TRANSFERRED_CHAR_JOIN_AF,

	//InstanceServer -> FieldServer
	NET_MSG_TRANSFER_BACK_MI,		//first start transferring the character who will be unmatched to FieldServer
	NET_MSG_QUIT_ROOM_IM,			//process to unmatch on MatchServer while the character drop out from InstanceServer

	NET_MSG_TRANSFER_START_MI,		//the character leaves the instance

	NET_MSG_CREATE_INSTANCE_MI,		//MatchServer -> InstanceServer
	NET_MSG_CREATE_INSTANCE_MI_FB,	//InstanceServer -> MatchServer	

	NET_MSG_DELETE_INSTANCE_MI,
	NET_MSG_DELETE_INSTANCE_MI_FB,

	NET_MSG_INSTANCE_NEW_MEMBER_MI,	//update members in the instance room
	NET_MSG_INSTANCE_DEL_MEMBER_MI,	//update members in the instance room


	//
	//mjeon.groupchat
	//
	NET_MSG_GCHAT_INVITE_CAC,
	NET_MSG_GCHAT_INVITE_CAC_FB,	//Accept or Refuse
	NET_MSG_GCHAT_ERRORCODE,		//각종 GroupChat관련 errorcode 전달용
	NET_MSG_GCHAT_QUIT_CA,
	NET_MSG_GCHAT_KICK_CA,			//Kick out a player from the chat
	NET_MSG_GCHAT_MEMBER_SYNC_AC,	//Agent -> All members in the room
	NET_MSG_GCHAT_MEMBER_UPDATE_AC,	//Agent -> All members in the room (Join or Quit)
	NET_MSG_GCHAT_MESSAGE,			//chat messages	

	NET_MSG_GCHAT_CONFIG_AC,		//groupchat 관련 설정 등 Client가 전달받을 설정값들을 전송한다



    //! Cache Server ----------------------------------------------------------
    NET_MSG_LOG_ITEM_EXCHANGE,
    NET_MSG_LOG_MONEY_EXCHANGE,
    NET_MSG_LOG_ITEM_CONVERSION,
    NET_MSG_LOG_ACTION,
    NET_MSG_LOG_RANDOM_OPTION,
    NET_MSG_LOG_PET_ACTION,
    NET_MSG_LOG_VEHICLE_ACTION,

	NET_MSG_LOG_ADDON_OPTION,
	NET_MSG_LOG_LINKSKILL_OPTION,
	NET_MSG_LOG_BASICSTAT_OPTION,

    NET_MSG_GAME_JOIN_AH, //! 게임 접속
    NET_MSG_CHAR_CLOSE_AH, //! 게임 접속 종료

    NET_MSG_CHA_LIST_AH, //! Agent->Cache:캐릭터 리스트 요청

	// 경험치 압축 / 캡슐
	NET_MSG_GCTRL_EXP_COMPRESSOR_REQ,
	NET_MSG_GCTRL_EXP_COMPRESSOR_ANS,
	NET_MSG_GCTRL_EXP_CAPSULE_REQ,
	NET_MSG_GCTRL_EXP_CAPSULE_ANS,

    //! Point Shop ------------------------------------------------------------
    NET_MSG_PS_DATA_START_DS,
    NET_MSG_PS_DATA_START_HS,
    NET_MSG_PS_DATA_START_AC,
    NET_MSG_PS_DATA_END_DS,
    NET_MSG_PS_DATA_END_HS,
    NET_MSG_PS_DATA_END_AC,
    NET_MSG_PS_DATA_DS,
    NET_MSG_PS_DATA_HS,
    NET_MSG_PS_DATA_AC,

    NET_MSG_PS_REQ_DATA_SH,
    NET_MSG_PS_GM_RELOAD_CA,
    NET_MSG_PS_RELOAD_AS,
    NET_MSG_PS_RELOAD_SS,
    NET_MSG_PS_UPDATE_DATE_CA,

    NET_MSG_CHAR_POINT_DH,
    NET_MSG_CHAR_POINT_HF,
    NET_MSG_CHAR_POINT_FA,
    NET_MSG_CHAR_REQ_POINT_FH,

    NET_MSG_GM_CHAR_POINT_ADD_CF,
    NET_MSG_CHAR_PS_POINT_CHANGE_FH,
	NET_MSG_CHAR_CHANGE_POINT_CACHE_FH,

    NET_MSG_PS_BUY_ITEM_CF,
    NET_MSG_PS_BUY_ITEM_FC,

    //! World battle ----------------------------------------------------------
    NET_MSG_USER_CHANGE_COUNTRY_CA,
    NET_MSG_USER_CHANGE_COUNTRY_AC,
	NET_MSG_CHAR_POINT_AC,
    NET_MSG_PS_POINT_CARD_CF,
    NET_MSG_CHAR_POINT_ADD_FC,

    //
    NET_MSG_CHAR_WEB_SHOP_LOG_FH,

    NET_MSG_PROTOCOL_INFO_CA,
    NET_MSG_PROTOCOL_INFO_AC,



	NET_MSG_MAP_SEARCH_NAME_REQ_CA,
	NET_MSG_MAP_SEARCH_NAME_REQ_CF,
	NET_MSG_MAP_SEARCH_NAME_1_ACK_AC,
	NET_MSG_MAP_SEARCH_NAME_2_ACK_AC,


	NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA,
	NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF,
	NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA,
	NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC,


	NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_CA,
	NET_MSG_MAP_SEARCH_NAME_POSITION_REQ_AF,
	NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_FA,
	NET_MSG_MAP_SEARCH_NAME_POSITION_ACK_AC,


    NET_MSG_GM_DB_UNLOCK_USER,
    NET_MSG_GM_DB_UNLOCK_USER_AF,
	//////////////////////////////////////////////////////////////////////////

	NET_MSG_MAP_CREATE_PARTY_BOARD_REQ_CA,
	NET_MSG_MAP_DELETE_PARTY_BOARD_REQ_CA,
	NET_MSG_MAP_REFRESH_PARTY_BOARD_REQ_CA,
	NET_MSG_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC,
	NET_MSG_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC,
	NET_MSG_MAP_PARTY_INFO_DETAIL_REQ_CA,
	NET_MSG_MAP_PARTY_INFO_DETAIL_1_ACK_AC,
	NET_MSG_MAP_PARTY_INFO_DETAIL_2_ACK_AC,

    //! Friend Group/친구 그룹 -----------------------------------------------
    NET_MSG_FRIEND_GROUP_NEW_CA,
    NET_MSG_FRIEND_GROUP_NEW_AC,
    NET_MSG_FRIEND_GROUP_DEL_CA,
    NET_MSG_FRIEND_GROUP_DEL_AC,
    NET_MSG_FRIEND_SET_GROUP_CA,
    NET_MSG_FRIEND_SET_GROUP_AC,
    NET_MSG_FRIEND_GROUP_NAME_CHANGE_CA,
    NET_MSG_FRIEND_GROUP_NAME_CHANGE_AC,
    NET_MSG_FRIEND_GROUP_REQ_CA,
    NET_MSG_FRIEND_GROUP_REQ_AC,
    NET_MSG_OTHER_CHAR_LEVEL_CHANGE_AC,
    NET_MSG_MAP_CHANGE_FRIEND_CLUB_AC,

    //! Notify/알림 -----------------------------------------------------------
    NET_MSG_NOTIFY_REQ_CA,
    NET_MSG_NOTIFY_DATA_AC,
    NET_MSG_NOTIFY_DATA_AC_END,

    //! Club ------------------------------------------------------------------
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ,     //! 클럽마스터 권한 위임
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK, //! 클럽마스터 권한 위임 질문
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS, //! 클럽마스터 권한 위임 답변
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB,  //! 클럽마스터 권한 위임 결과
    NET_MSG_GCTRL_CLUB_AUTHORITY_CLT,	  //! 클럽마스터 변경 내용
    NET_MSG_GCTRL_CLUB_AUTHORITY_BRD,	  //! 클럽마스터 변경 내용
    NET_MSG_CLUB_MASTER_CHANGE_AF,	      //! 클럽마스터 변경 내용 (필드)

    NET_MSG_GCTRL_DUPLICATE_CLOSEGAME,	//!	중복로그인으로 접속을 종료해야 하는 경우

    NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD,	//!	클럽데스매치 남은시간.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD,	//!	클럽데스매치 준비.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD,	//!	클럽데스매치 시작.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD,	//!	클럽데스매치 시작.

    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD,	//!	클럽데스매치 종료.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT,	//!	클럽데스매치 종료. ( Fiedl 결과 -> AGT )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD,	//!	클럽데스매치 종료.

    NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO,	//!	클럽데스매치 정보 전송 ( 접속시 ) 
    NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG,	//!	클럽데스매치 정보 전송 ( Agent에 남은시간 요청 )

    NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE,	//!	클럽데스매치 Kill/Death 상황 ( 자기만 )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE,	//!	클럽데스매치 Kill/Death 상황 ( 자기만 )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE,	//!	클럽데스매치 랭킹 상황
    NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ,	//!	클럽데스매치 상황 요청
	NET_MSG_GCTRL_CLUB_DEATHMATCH_ON_OFF_CA,	//! 클럽데스매치 ON OFF 지엠 명령

    NET_MSG_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD, //! 선도맵의 Flag 셋팅

    NET_MSG_CLUB_COUNTRY_COUNT_CA,
    NET_MSG_CLUB_COUNTRY_COUNT_START_AC,
    NET_MSG_CLUB_COUNTRY_COUNT_AC,
    NET_MSG_CLUB_COUNTRY_COUNT_END_AC,
    NET_MSG_CLUB_COUNTRY_CA,
    //NET_MSG_CLUB_CHAR_LEVEL_CHANGE_AC,
    NET_MSG_REQ_DROP_CHAR_CLUB_NAME_CA,
    NET_MSG_UPDATE_DROP_CHAR_CLUB_NAME_AC,

    NET_MSG_LOBBY_CLUB_INFO,    //! 클럽 정보
    NET_MSG_LOBBY_CLUB_INFO_EX, //! 클럽 정보
    //NET_MSG_LOBBY_CLUB_MEMBER,  //! 클럽 멤버
    NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK, //!	클럽 멤버.
    NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK_END,
    NET_MSG_LOBBY_CLUB_ALLIANCE, //!	클럽 동맹.
    NET_MSG_LOBBY_CLUB_BATTLE,  //!	클럽 배틀.
    NET_MSG_LOBBY_CLUB_INFO_END, 

    NET_MSG_GCTRL_CLUB_NEW,	//!	클럽 생성.
    NET_MSG_CLUB_NEW_LOBBY_CA, //! 클럽 생성 Lobby
    NET_MSG_GCTRL_CLUB_NEW_2AGT,	//!	클럽 생성 to AGENT.
    NET_MSG_GCTRL_CLUB_NEW_DB2AGT,	//!	클럽 생성 DB to AGENT.
    NET_MSG_GCTRL_CLUB_NEW_2FLD,	//!	클럽 생성 to FIELD.
    NET_MSG_GCTRL_CLUB_NEW_FB,	//!	클럽 생성 FB.

    NET_MSG_GCTRL_CLUB_DISSOLUTION,	//!	클럽 해체 요청.
    NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD,	//!	클럽 해체 요청.
    NET_MSG_GCTRL_CLUB_DISSOLUTION_FB,	//!	클럽 해체 요청 FB.
    NET_MSG_GCTRL_CLUB_DEL_2FLD,	//!	클럽 해체 to FIELD.
    NET_MSG_GCTRL_CLUB_DEL_2CLT,	//!	클럽 삭제 to CLIENT. ( 해체 및 탈퇴 )
    NET_MSG_GCTRL_CLUB_DEL_BRD,	//!	클럽 삭제 BRD. ( 해체 및 탈퇴 )

    NET_MSG_GCTRL_CLUB_INFO_2FLD,	//!	클럽 정보 to FLIED.
    NET_MSG_GCTRL_CLUB_INFO_2CLT,	//!	클럽 정보 to CLIENT.
    NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION,	//!	클럽 해체예약 정보 to CLIENT.
    NET_MSG_GCTRL_CLUB_MEMBER_2CLT,	//!	클럽 멤버 to CLIENT.

    NET_MSG_GCTRL_CLUB_INFO_BRD,	//!	클럽 정보 BRD.
    NET_MSG_GCTRL_CLUB_INFO_MARK_BRD,	//!	클럽 마크 정보 BRD.
    NET_MSG_GCTRL_CLUB_INFO_NICK_BRD,	//!	클럽 닉네임 정보 BRD.

    NET_MSG_GCTRL_CLUB_MARK_INFO,	//!	클럽 마크 요청.
    NET_MSG_GCTRL_CLUB_MARK_INFO_FB,	//!	클럽 마크 반환.

    NET_MSG_GCTRL_CLUB_MARK_CHANGE,      //! 클럽 마크 변경 요청
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD, //! 클럽 마크 변경 요청
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT, //! 클럽 마크 변경 요청
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB,   //! 클럽 마크 변경 FB

    NET_MSG_CLUB_RANK_UP_CF,        //! 클럽 랭크 변경
    NET_MSG_CLUB_RANK_CHANGE_FA,    //! 클럽 랭크 변경
    NET_MSG_CLUB_RANK_CHANGE_AF,    //! 클럽 랭크 변경
    NET_MSG_CLUB_RANK_CHANGE_FC,    //! 클럽 랭크 변경
    NET_MSG_GCTRL_CLUB_RANK_FB,     //! 클럽 랭크 변경 FB

    NET_MSG_CLUB_MENBER_LOBBY_CA,	//!	클럽 가입 요청, Lobby
    NET_MSG_CLUB_MEMBER_REQ_CA, 	//!	클럽 참가 요청.
    NET_MSG_CLUB_MEMBER_REQ_ASK_AC,	//!	클럽 참가 질의
    NET_MSG_CLUB_MEMBER_REQ_ANS_CA,	//!	클럽 참가 답변
    NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT,	//!	클럽 참가 답변
    NET_MSG_CLUB_MEMBER_REQ_FB_AC,	//!	클럽 참가 요청 FB

    NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD,	//!	클럽 멤버 추가 to FIELD.

    NET_MSG_CLUB_MEMBER_DEL_CA,         //! 클럽 멤버 제명 요청.
    NET_MSG_CLUB_MEMBER_DEL_AF,         //! 클럽 멤버 제명 요청 to FIELD.
    NET_MSG_CLUB_MEMBER_DEL_FC,         //! 클럽 멤버 제명 요청 결과 to CLIENT.
    NET_MSG_CLUB_MEMBER_DEL_AC,         //! 클럽 멤버 제명 요청 결과 FB.

    NET_MSG_GCTRL_CLUB_MEMBER_NICK,	//!	클럽 멤버 별명.
    NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB,	//!	클럽 멤버 별명 FB.

    //NET_MSG_GCTRL_CLUB_MEMBER_STATE,	//!	클럽 멤버 상태.
    //NET_MSG_GCTRL_CLUB_MEMBER_POS,	//!	클럽 멤버 위치.

    NET_MSG_GCTRL_CLUB_MEMBER_SECEDE,	//!	클럽 멤버 탈퇴.
    NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB,	//!	클럽 멤버 탈퇴 FB.

    NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL,	//!	클럽 멤버 제명 요청.

    //선도전 관련
    NET_MSG_GCTRL_SERVER_CLUB_GUID_BATTLE_INFO,         // 선도전 시작 유무
    NET_MSG_GCTRL_REQ_SERVER_CLUB_GUID_BATTLE_REMAIN_AG,// 선도전 남은시간 요청(Field->Agent->Client)
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD,	        // 선도전 남은시간 전송
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_FLD,	        // 선도 클럽 결정전 시작.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_BRD,	        // 선도 클럽 결정전 시작.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_FLD,	            // 선도 클럽 결정전 종료.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_BRD,	            // 선도 클럽 결정전 종료.

    NET_MSG_GCTRL_CLUB_CD_CERTIFY,	       //! cd 인증
    NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB,	   //! cd FB
    NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD,	   //! cd 인증 BRD
    NET_MSG_CLUB_CD_CERTIFY_ING_BRD, //! cd 인증 결과 BRD

    NET_MSG_CLUB_CERTIFIED_FA,	//!	cd 인증 완료 ( agent에 알림 )
    NET_MSG_CLUB_CERTIFIED_AF,	//!	cd 인증 완료 ( field에 알림 )
    //NET_MSG_GCTRL_CLUB_CERTIFIED_CLT,	//!	cd 인증 완료 ( field에 알림 )

    NET_MSG_GCTRL_CLUB_COMMISSION,	//!	선도 지역 수수료 설정.
    NET_MSG_GCTRL_CLUB_COMMISSION_FB,	//!	선도 지역 수수료 설정 FB.
    NET_MSG_GCTRL_CLUB_COMMISSION_FLD,	//!	선도 지역 수수료 설정 FLD.
    NET_MSG_GCTRL_CLUB_COMMISSION_BRD,	//!	선도 지역 수수료 설정 BRD.
    NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT,	//!	클럽에 수익 발생 AGT.

    NET_MSG_CLUB_GETSTORAGE_CA,	        //! 창고 정보 요청.
    NET_MSG_GCTRL_CLUB_GETSTORAGE,      //! 창고 정보 요청.
    NET_MSG_GCTRL_CLUB_GETSTORAGE_FB,	//!	창고 정보 요청.
    NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM,	//!	창고 정보 회신.

    NET_MSG_CLUB_STORAGE_TO_INVEN_CA,	//!	창고 아이템 들어올리기.
    NET_MSG_CLUB_STORAGE_TO_INVEN_AF,	//!	창고 아이템 들어올리기.
    NET_MSG_CLUB_STORAGE_RESTORE_FA,	//!	창고 아이템 복구
    NET_MSG_CLUB_ITEM_EXCHANGE_TO_INVEN_CA,	//! 창고 아이템과 인벤에 들고있는 아이템 교환
    NET_MSG_CLUB_STORAGE_EX_CLUB_STORAGE_CA,	//! 창고 아이템과 창고 들고있는 아이템 교환
    NET_MSG_CLUB_STORAGE_TO_CLUB_STORAGE_CA,	//! 창고 아이템을 창고로 옮김.
    NET_MSG_CLUB_STORAGE_EX_INVEN_AF,	//!	창고 아이템과 손에 들린 아이템 교환.
    NET_MSG_CLUB_STORAGE_EX_INVEN_FA,	//!	창고 아이템과 손에 들린 아이템 교환.
    NET_MSG_CLUB_INVEN_TO_STORAGE_CF,	//!	손에 들린 아이템 창고에 넣기.
    NET_MSG_CLUB_INVEN_TO_STORAGE_FA,	//!	손에 들린 아이템 창고에 넣기.
    NET_MSG_CLUB_HOLD_TO_STORAGE_AF,	//!	손에 들린 아이템 창고에 넣기 실패/복구
    //NET_MSG_GCTRL_CLUB_STORAGE_SPLIT,	//!	창고의 아이템 분할하기.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_CF,	//!	돈을 저장.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA,	//!	돈을 저장.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF,	//!	돈을 저장.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_CA,	//!	돈을 인출.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF,	//!	돈을 인출.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_FA,	//!	돈을 인출.

    NET_MSG_GCTRL_CLUB_STORAGE_INSERT,	//!	창고에 넣기.
    NET_MSG_GCTRL_CLUB_STORAGE_DELETE,	//!	창고에서 삭제.
    NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS,	//!	삭제와 넣기 동시에 하기.
    NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM,	//!	창고에 있는 아이템의 정보 갱신.
    NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY,	//!	창고의 저장금액 갱신.

    NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD,	//!	선도 지역 수수료 설정 예약 BRD.

    NET_MSG_GCTRL_CLUB_STORAGE_RESET,	//!	클럽 창고 정보를 리샛.
    NET_MSG_CLUB_STORAGE_SEND_END,	//!	클럽 창고 정보
    NET_MSG_GCTRL_CLUB_STORAGE_GET_DB,	//!	클럽 창고 정보를 db에서 수신. 아이템 정보 수신
	NET_MSG_GCTRL_CLUB_STORAGE_GET_DB_END,	//!	클럽 창고 정보를 db에서 수신. 기타 정보(돈) 수신, 및 아이템 데이터 수신 완료 메시지

    NET_MSG_CLUB_INCOME_RENEW_CA,	//!	클럽 수익금 업뎃 요청 ( client->field ).
    //NET_MSG_GCTRL_CLUB_INCOME_UP,	//!	클럽 수익금 업뎃 요청 ( field->agent ).
    //NET_MSG_GCTRL_CLUB_INCOME_DN,	//!	클럽 수익금 업뎃 회신 ( agent->field ).
    NET_MSG_GCTRL_CLUB_INCOME_FB,	//!	클럽 수익금 업뎃 결과 ( field->agent ).

    NET_MSG_GCTRL_CLUB_NOTICE_REQ,	//!	클럽 공지 수정 요청. ( client->agent )
    NET_MSG_GCTRL_CLUB_NOTICE_FB,	//!	클럽 공지 수정 결과. ( agent->client )
    NET_MSG_GCTRL_CLUB_NOTICE_FLD,	//!	클럽 공지 필드에 알림. ( agent->field )
    NET_MSG_GCTRL_CLUB_NOTICE_CLT,	//!	클럽 공지 클라이언트에 알림. ( field->client )

    //NET_MSG_GCTRL_CLUB_SUBMASTER,	     //! 클럽 부마 설정.
    NET_MSG_GCTRL_CLUB_SUBMASTER_FB,	 //! 클럽 부마 설정 FB.
    //NET_MSG_GCTRL_CLUB_SUBMASTER_BRD,	 //! 클럽 부마 설정 알림 CLT.
    //NET_MSG_GCTRL_CLUB_SUBMASTER_FLD,	 //! 클럽 부마 설정 알림 FLD.
    NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AC, //! Agent->Client CD 인증자 변경
    NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AF, //! Agent->Field  CD 인증자 변경
    NET_MSG_CLUB_MASTER_CHNAGE_AC,       //! Agent->Client Club 마스터 변경

    NET_MSG_CLUB_ALLIANCE_REQ_CA,        //! 클럽 동맹 요청.
    NET_MSG_CLUB_ALLIANCE_REQ_ASK_AC,    //! 클럽 동맹 요청 ASK.
    NET_MSG_CLUB_ALLIANCE_REQ_ANS_CA,    //! 클럽 동맹 요청 ANS.
    NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB,	 //! 클럽 동맹 요청 결과 FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD, //! 클럽 동맹 클럽 ADD FLD.
    NET_MSG_CLUB_ALLIANCE_ADD_AC,        //! 클럽 동맹 클럽 ADD CLT.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ, //! 클럽 동맹 클럽 제명 요청.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB,	 //! 클럽 동맹 클럽 제명 요청 FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ, //! 클럽 동맹 탈퇴 요청.
    NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB,	 //! 클럽 동맹 탈퇴 요청 FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD, //! 클럽 동맹 클럽 DEL FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT, //! 클럽 동맹 클럽 DEL CLT.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ, //! 클럽 동맹 해체 FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB,  //! 클럽 동맹 해체 FLD.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD, //! 클럽 동맹 해체 FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT, //! 클럽 동맹 해체 CLT.


    NET_MSG_GCTRL_CLUB_BATTLE_REQ,      //! 클럽 배틀 요청 
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK,	//! 클럽 배틀 요청 ASK.
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS,	//! 클럽 배틀 요청 ANS.
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB,	//! 클럽 배틀 요청 결과 FB.

    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD,  //! 클럽 배틀 시작 FLD.
    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT,  //! 클럽 배틀 시작 CLT(대기).
    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2, //! 클럽 배틀 시작 CLT2(진짜시작).
    NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD,	  //! 클럽 배틀 종료 FLD.
    NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT,	  //! 클럽 배틀 종료 CLT.

    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ,     //! 클럽 배틀 휴전 요청.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK, //! 클럽 배틀 휴전 요청 ASK.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS, //! 클럽 배틀 휴전 요청 ANS.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB,	 //! 클럽 배틀 휴전 요청 결과 FB.

    NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ,    //! 클럽 배틀 항복 요청.
    NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB, //! 클럽 배틀 항복 요청 결과 FB.

    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT,	//! 클럽 배틀 킬수 UPDATE Field -> Agent
    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD,	//! 클럽 배틀 킬수 UPDATE Agent -> Field
    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE,      //! 클럽 배틀 킬수 UPDATE 클라이언트

    NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE,	//!	클럽 배틀 킬/데스수 증가

    NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD,	//!	클럽 배틀 종료시 Field정보 조합 Agent -> Field
    NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT,	//!	클럽 배틀 종료시 Field정보 조합 결과 Field -> Agent


    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ,	//!	동맹 배틀 요청 
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK,	//!	동맹 배틀 요청 ASK.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS,	//!	동맹 배틀 요청 ANS.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB,	//!	동맹 배틀 요청 결과 FB.

    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ,	//!	동맹 배틀 휴전 요청.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK,	//!	동맹 배틀 휴전 요청 ASK.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS,	//!	동맹 배틀 휴전 요청 ANS.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB,	//!	동맹 배틀 휴전 요청 결과 FB.

    NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ,	//!	동맹 배틀 항복 요청.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB,	//!	동맹 배틀 항복 요청 결과 FB.

    NET_MSG_CLUB_AUTH_AF,
    NET_MSG_CLUB_AUTH_FC,

	NET_MSG_CLUB_MASTER_MANDATE_CA, //! 클럽 마스터 위임 ON/OFF

    NET_MSG_CLUB_GRADE_FLAG_CHANGE_CA, //! 클럽 권한 Flag 조정 Client->Agent
    NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF, //! 클럽 권한 Flag 조정 Agent->Client

    NET_MSG_CLUB_PULIC_FLAG_CHANGE_CA, //! 클럽 공개 Flag 조정 Client->Agent
    NET_MSG_CLUB_PULIC_FLAG_CHANGE_AC, //! 클럽 공개 Flag 조정 Agent->Client
    NET_MSG_CLUB_PULIC_FLAG_CHANGE_AF, //! 클럽 공개 Flag 조정 Agent->Field
    NET_MSG_CLUB_AUTO_KICK_CA, //! 클럽 자동 탈퇴 설정 변경
    NET_MSG_CLUB_AUTO_KICK_AF, //! 클럽 자동 탈퇴 설정 변경
    NET_MSG_CLUB_AUTO_KICK_AC, //! 클럽 자동 탈퇴 설정 변경
    NET_MSG_CLUB_NEWBIE_NOTICE_CA, //! 클럽 신입생 소개글
    NET_MSG_CLUB_NEWBIE_NOTICE_AF, //! 클럽 신입생 소개글
    NET_MSG_CLUB_NEWBIE_NOTICE_AC, //! 클럽 신입생 소개글
    
    NET_MSG_CLUB_LOG,        //! 클럽 로그
    NET_MSG_CLUB_LOG_CA,     //! 클럽 로그
    NET_MSG_CLUB_LOG_AH,     //! 클럽 로그
    NET_MSG_CLUB_LOG_HA,     //! 클럽 로그
    NET_MSG_CLUB_LOG_HA_END, //! 클럽 로그
    NET_MSG_CLUB_LOG_AC,     //! 클럽 로그
    NET_MSG_CLUB_LOG_AC_END, //! 클럽 로그
    NET_MSG_CLUB_LOG_DH,     //! 클럽 로그
    NET_MSG_CLUB_LOG_DH_END, //! 클럽 로그

    NET_MSG_CLUB_GRADE_NAME_CHANGE_CA, //! 클럽 계급 이름
    NET_MSG_CLUB_GRADE_NAME_CHANGE_AF, //! 클럽 계급 이름
    NET_MSG_CLUB_GRADE_NAME_CHANGE_AC, //! 클럽 계급 이름
    NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, //! 클럽 계급 이름

    NET_MSG_CLUB_GRADE_FLAG_CHANGE_AC,
    NET_MSG_CLUB_MEMBER_GRADE_CHANGE_CA,
    NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AF,
    NET_MSG_CLUB_MEMBER_GRADE_CHANGE_AC2,

    NET_MSG_CLUB_JOIN_TIME_RESET_CA,
    NET_MSG_CLUB_JOIN_TIME_RESET_AC,
    NET_MSG_CLUB_JOIN_TIME_RESET_AF,

    NET_MSG_CLUB_ALLIANCE_TIME_RESET_CA,
    NET_MSG_CLUB_ALLIANCE_TIME_RESET_AC,
    NET_MSG_CLUB_ALLIANCE_TIME_RESET_AF,

	NET_MSG_CLUB_ID_2MASTERNAME_REQ_CA,
	NET_MSG_CLUB_ID_2MASTERNAME_ANS_AC,

    NET_MSG_CHAR_POS_FA, //! Field->Agent 캐릭터 현재 위치 알림

    NET_MSG_CHAR_ONLINE_AC, //! 캐릭터 상태 알림 online
    NET_MSG_CHAR_OFFLINE_AC, //! 캐릭터 상태 알림 offline
    NET_MSG_CHAR_POSITION_AC, //! 캐릭터 위치 알림

    NET_MSG_PARTY_INFO_REQ_CA, //! 파티 정보 요청
    NET_MSG_PARTY_INFO_REQ_AC,

    NET_MSG_CHAR_DB_NUM_REQ_CA, //! 캐릭터 이름으로 캐릭터 DB 번호를 요청
    NET_MSG_CHAR_DB_NUM_REQ_AC, //! 캐릭터 이름으로 캐릭터 DB 번호를 요청 결과
    NET_MSG_CHAR_DB_NUM_REQ_DA,  //! 캐릭터 이름으로 캐릭터 DB 번호를 요청 결과

    NET_MSG_FRIEND_STATE_REQ_CA, //! 캐릭터 친구상태 요청
    NET_MSG_FRIEND_STATE_REQ_AC, //! 캐릭터 친구상태 요청
    NET_MSG_FRIEND_STATE_REQ_DA, //! 캐릭터 친구상태 요청

    NET_MSG_GUIDANCE_MAP_INFO_CA, //! 선도클럽 맵 정보 요청.
    NET_MSG_GUIDANCE_MAP_INFO_AC,
    NET_MSG_GM_GUIDANCE_INFO_CA, //! GM이 선도클럽 정보 요청.
    NET_MSG_GM_GUIDANCE_INFO_AC,
    NET_MSG_GM_GUIDANCE_CHANGE_CA, //! 선도클럽 강제 변경
	NET_MSG_GM_GUIDANCE_ON_OFF_CA, //! 선도전 ON/OFF

    NET_MSG_OTHER_CHAR_PARTY_CHANGE_AC, //! 파티 변경 정보

    NET_LOG_ATTENDANCE_DS, //! 출석부 로그 DB->Server
    NET_LOG_ATTENDANCE_SH, //! 출석부 로그 Server->Cache
    NET_LOG_ATTENDANCE_TASK_DS, //! 출석부 로그 DB->Server
    NET_LOG_ATTENDANCE_TASK_SH, //! 출석부 로그 DB->Server

	NET_MSG_CHA_SAVE_DF,	//! 캐릭터 저장 후 처리 DB->Field


	//! 개인상점 Private Market ----------------------------------------------------------

	NET_MSG_GCTRL_PMARKET_SELLER_REGISTER_FH,	//! 개인상점 판매자 정보 등록(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_REGISTER_FH,	//! 개인상점 판매 아이템 등록(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_CF,	//! 개인상점 아이템 검색(기본) 요청(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_FH,	//! 개인상점 아이템 검색(기본) 요청(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_NUM_HFC,	//! 개인상점 검색 결과 아이템 개수(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_HFC,	//! 개인상점 검색 결과(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_END_HFC,	//! 개인상점 검색 결과 끝(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_CF,	//! 개인상점 아이템 구매 요청(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_FH,	//! 개인상점 아이템 구매 요청(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_HF,	//! 개인상점 아이템 대금 지불(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_FH,	//! 개인상점 아이템 대금 지불 결과(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF,	//! 개인상점 아이템 구매대금 롤백(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_HF,	//! 개인상점 아이템 구매 처리(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_ROLLBACK_FH,	//! 개인상점 아이템 구매 처리 롤백(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_RESULT_FH,	//! 개인상점 아이템을 구매했을 때 남아 있는 아이템 상태 전송(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_FH,	//! 개인상점 홀드아이템인지 질의(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_HF,	//! 개인상점 홀드아이템인지 질의 응답(Cache->Field)
	NET_MSG_GCTRL_PMARKET_COMMON_FB,	//! 개인상점 일반 FB (Server->Client)
	NET_MSG_GCTRL_PMARKET_LOG_CF,	//! 개인상점 거래내역 요청(Client->field)
	NET_MSG_GCTRL_PMARKET_LOG_FH,	//! 개인상점 거래내역 요청(field->Cache)
	NET_MSG_GCTRL_PMARKET_LOG_NUM_HFC,	//! 개인상점 거래내역 개수(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_LOG_HFC,	//! 개인상점 거래내역(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_LOG_END_HFC,	//! 개인상점 거래내역 끝(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_STATE_CF,	//! 개인상점 현재 설정 상태 요청(Client->field)
	NET_MSG_GCTRL_PMARKET_STATE_FH,	//! 개인상점 현재 설정 상태 요청(field->Cache)
	NET_MSG_GCTRL_PMARKET_STATE_HFC,	//! 개인상점 현재 설정 상태 응답(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_CF,	//! 개인상점 검색 요청(아이템 이름으로)(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_AH,	//! 개인상점 검색 요청(아이템 이름으로)(Agent->Cache)
	NET_MSG_GCTRL_PMARKET_OPTION_CA,	//! 개인상점 현재 설정 변경 요청(Client->Agent)
	NET_MSG_GCTRL_PMARKET_OPTION_FH,	//! 개인상점 현재 설정 변경 요청(Field->Cache)
	NET_MSG_GCTRL_PMARKET_OPTION_AF,	//! 개인상점 변경 요청(Agent->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC,	//! 개인상점 검색 결과 총 아이템 개수(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_CLOSE_FH,	//! 개인상점 닫는다.(Field->Cache)
	NET_MSG_GCTRL_PMARKET_POST_SEND_HF,	//! 개인상점 아이템 우편 전송 요청, 구매자의 필드로 가서 우편 전송 한다.(Cache->Field)

    // Product (제조) 타입
    // 기존 메세지
    NET_MSG_REQ_LEARN_PRODUCTBOOK,	    //! 조합서 배우기 요청
    NET_MSG_REQ_LEARN_PRODUCTBOOK_FB,	//! 조합서 배우기 요청 결과 통보
    NET_MSG_PRODUCT_REQ,	            //! 아이템 조합 요청
    NET_MSG_PRODUCT_FB,	                //! 아이템 조합 요청 결과
	NET_MSG_PRODUCT_FA,					//! 아이템 조합 요청 결과 ( FA );
	NET_MSG_PRODUCT_EXCHANGE_LOG_FA,	//! 아이템 Exchange Log;
    
    NET_MSG_PRODUCTBOOK,                //! 조합서 습득 목록 전송
    // cwback
    //! REQ = Request     (요청)     
    //! ACK = Acknowledge (응답)
    //신규 메세지
    NET_MSG_REQ_PRODUCTBOOK,            //! 조합서 습득 목록 요청

    NET_MSG_PRODUCT_TYPE_LIST_REQ,      //! 조합 분야 목록, 조합분야 포인트 요청
    NET_MSG_PRODUCT_TYPE_LIST_ACK,      //! 조합 분야 목록, 조합분야 포인트 응답

    NET_MSG_PRODUCT_TYPE_ADD_REQ,       //! 조합 분야 추가 요청
    NET_MSG_PRODUCT_TYPE_ADD_ACK,       //! 조합 분야 추가 응답

    NET_MSG_PRODUCT_TYPE_DEL_REQ,       //! 조합 분야 삭제 요청
    NET_MSG_PRODUCT_TYPE_DEL_ACK,       //! 조합 분야 삭제 요청

    NET_MSG_PRODUCT_ISGETEXP_LIST_REQ,  //! 조합으로 숙련도를 습득한 목록 요청
    NET_MSG_PRODUCT_ISGETEXP_LIST_ACK,  //! 조합으로 숙련도를 습득한 목록 응답

	NET_MSG_CATCH_DELAY,	// 필리핀락커복사 대응(추후 코드 정리) Server->Client
	NET_MSG_GM_MSG_DELAY_MODIFY,	// 관리할 메시지 관리

	NET_MSG_CHA_SAVE_FA, // 필드서버가 캐릭터 저장한다고 에이전트에 알림(아이템 복사 대비)
	NET_MSG_CHA_SAVE_END_FA, // 필드서버가 캐릭터 저장끝났다고 에이전트에 알림(아이템 복사 대비)

	NET_MSG_CHA_VALIDATION_FA,

    //! 아이템 수리
    NET_MSG_ITEM_REPAIR_REQ,            //! 아이템 수리
    NET_MSG_ITEM_REPAIR_ACK,            //!
    NET_MSG_ITEM_REPAIR_CARD_REQ,       //! 아이템 수리 창 열기
    NET_MSG_ITEM_REPAIR_CARD_ACK,       //! 
    

    // PVE 메시지
    NET_MSG_PVE_MAPTABLE_REQ_CA,
    NET_MSG_PVE_MAPTABLE_ACK_AC,
    NET_MSG_PVE_ENTRANCE_REQ_CA,
    NET_MSG_PVE_ENTRANCE_ACK_AC,
    NET_MSG_PVE_ENTRANCE_CONFIRM_REQ,
    NET_MSG_PVE_ENTRANCE_CONFIRM_ACK_AC,
    NET_MSG_PVE_CLEAROFBELONGING_REQ_CA,
	NET_MSG_PVE_CLEAROFBELONGING_ACK_AC,
    NET_MSG_PVE_CREATE_INSTANT_MAP_REQ_AF,
    NET_MSG_PVE_CREATE_INSTANT_MAP_ACK_FA,
    NET_MSG_PVE_ENTRANCE_INSTANT_MAP_REQ_AF,
    NET_MSG_PVE_ENTRANCE_INSTANT_MAP_ACK_FA,
    NET_MSG_PVE_ENTRANCE_STATE_FC,
    NET_MSG_PVE_MONSTERCLEAR_STATE_FC,
    NET_MSG_PVE_POSITIONINGCLEAR_STATE_FC,
    NET_MSG_PVE_START_STAGE_FC,
    NET_MSG_PVE_MSG_START_STAGE_FC,
    NET_MSG_PVE_MSG_FC,
    NET_MSG_PVE_STAGE_STATE_FC,
    NET_MSG_PVE_PORTAL_STATE_FC,
    NET_MSG_PVE_PORTAL_REQ_CF,
    NET_MSG_PVE_MOVE_REQ_FA,
    NET_MSG_PVE_RETURN_REQ_AF,
    NET_MSG_PVE_RETURN_REQ_FA,	
    NET_MSG_PVE_RETRY_REQ_CF,
	NET_MSG_PVE_RETRY_ACK_FC,
    NET_MSG_PVE_RETRY_REQ_FA,
    NET_MSG_PVE_STAGE_RESULT_FC,
    NET_MSG_PVE_STAGE_RESULT_FA_OR_AF,
    NET_MSG_PVE_EXIT_REQ_CF,
    NET_MSG_PVE_SAVEPOS_REQ_AF,
	NET_MSG_PVE_REMOVE_INDUN_AC,
	NET_MSG_PVE_REBIRTH_FC,
	NET_MSG_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF,
	NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_AC,
	NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_FA,
	NET_MSG_PVE_GM_GOSTAGE_REQ_CF,

    // Char 상태 메시지
    NET_MSG_CHAR_STATE_FC,

	// reference count 수정 요청(GM 메시지)
	NET_MSG_REFERENCE_COUNT_CAF,

    // ipevent 관련.(GM 메세지)
    NET_MSG_GM_LOAD_IPEVENT,
    NET_MSG_GM_SET_IPEVENT,

    // 경험치 출력 관련.
    NET_MSG_GM_ACQUIRE_EXP,
    NET_MSG_GM_ACQUIRE_EXP_ANS,
    NET_MSG_GCTRL_ACQUIRE_EXP,

	//RNATTENDANCE RN 출석부 메시지
	NET_MSG_RNATTENDANCE_QUEST_LIST_FC,
	NET_MSG_GCTRL_REQ_REQ_RNATTENDANCE_QUEST_START,

	/// Lotto System Message - 패킷 모니터링을 위하여 모두 한곳에 정의;
	// Client -> Field -> Agent;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_CF,	// 로또 구매 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA,	// 로또 구매 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_CF,	// 확인안된 구매리스트 확인 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA,	// 확인안된 구매리스트 확인 요청;


	// Client -> Agent;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA,	// 현재 상태 확인;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA,		// 현재 회차 확인;
	NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA,	// 회차별 누적 당첨금 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA,			// 로또 캐릭터 별 구매 리스트 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA,		// 회차 별 당첨 번호 요청;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA,	// 회차 별 당첨자 요청;

	// Client -> Anget ( GM );
	NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA,			// 상태 변경;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA,				// 사용자들이 현재까지 선택한 누적 번호의 수;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA,					// 로또 추첨 시의 각 번호가 번호당 몇개씩 들어 있는지 확인;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA,				// 로또의 각 번호를 하나가지고 있는 상태로 초기화;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA,				// 특정 번호의 개수를 추가;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA,				// 특정 번호의 개수를 삭제;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SET_INVENT_WIN_NUM_CA,	// 조작 당첨 번호 세팅;
	NET_MSG_GCTRL_LOTTO_SYSTEM_RESET_INVENT_WIN_NUM_CA,	// 조작 당첨 번호 리셋;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_CA,		// 구매 Lock 조작;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_CA,		// 턴 변경 Lock 조작;

	// Agent -> Field;
	NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_AF,	// 당첨금 우편;
	NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC,	// 당첨금 우편;

	// Agent -> Field -> Client;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_AF,	// 로또 구매 응답;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC,	// 로또 구매 응답;

	// Agent -> Client;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_GLOBAL_VALUE_AC,// 클라이언트 동기화 Global Value 패킷;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC,		// 클라이언트 동기화 패킷;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC,// 다음 추첨 일자 정보 패킷;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC,	// 현재 로또 시스템의 상태;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC,		// 현재 회차 동기화;
	NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC,	// 회차별 누적 당첨금 응답;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC,			// 로또 캐릭터 별 구매 리스트 응답;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC,	// 로또 리스트 개수;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC,		// 회차 별 당첨 번호 응답;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC,	// 회차 별 당첨자 응답;

	// Agent -> Client ( GM );
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC,			// 사용자들이 현재까지 선택한 누적 번호의 수;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC,				// 로또 추첨 시의 각 번호가 번호당 몇개씩 들어 있는지 확인;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC,			// 로또의 각 번호를 하나가지고 있는 상태로 초기화;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC,			// 특정 번호의 개수를 추가;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC,			// 특정 번호의 개수를 삭제;
	NET_MSG_GCTRL_LOTTO_SYSTEM_INVENT_NUM_DSP_AC,	// 조작 가능 시 DSP;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_AC,	// 구매 Lock 조작;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_AC,	// 턴 변경 Lock 조작;

	// Agent Myself;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_AGENT_MYSELF,	// 동기화;

	// Agent Broadcast Message;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB,		// 상태 변경;
	NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB,		// 회차 변경;
	NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB,		// 추첨 번호;
	/// ---------------------------------------------------------------- ///

	/// Reloading AI FSM - 패킷 모니터링을 위하여 모두 한곳에 정의;
	NET_MSG_GCTRL_RELOAD_AI_FSM_CF,
	/// ---------------------------------------------------------------- ///

	/// Country - 패킷 모니터링을 위하여 모두 한곳에 정의;
	NET_MSG_GCTRL_COUNTRY_CHANGE_DA,
	NET_MSG_GCTRL_VCOUNTRY_ADD_FAC,
	NET_MSG_GCTRL_VCOUNTRY_ADD_DAF,
	NET_MSG_GCTRL_VCOUNTRY_ADD_AA,
	NET_MSG_GCTRL_VCOUNTRY_RESET_FAC,
	NET_MSG_GCTRL_VCOUNTRY_RESET_DAF,
	NET_MSG_GCTRL_VCOUNTRY_RESET_AA,
	NET_MSG_GCTRL_VCOUNTRY_SYNC_AC,
	NET_MSG_GCTRL_INITIALIZE_COUNTRY_CF,
	NET_MSG_GCTRL_INITIALIZE_COUNTRY_FA,
	NET_MSG_GCTRL_SHOW_COUNTRY_MARK_FC,
	/// ---------------------------------------------------------------- ///

	/// Country - 패킷 모니터링을 위하여 모두 한곳에 정의;
	NET_MSG_GCTRL_SELECT_PROVIDE_REWARD_DF,
	/// ---------------------------------------------------------------- ///

	// Synchronize Use Features
	NET_MSG_GCTRL_SYNC_USE_FEATURES_AC,
	NET_MSG_GCTRL_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC,

	// 스킬 효과 적용;
	NET_MSG_GCTRL_APPLY_SKILLEFFECT_FC,

	// 맵 이동 요청
	NET_MSG_REQ_MOVE_MAP,
	// 프리미엄 맵 강제퇴장
	NET_MSG_PREMIUM_MAP_EXIT,

	NET_MSG_FIELD_STATE, //! Agent로 Field 서버의 상태를 보냄;



    // ConsignmentSale ( 위탁판매 )
    // cwback
    //! REQ = Request     (요청)     
    //! ACK = Acknowledge (응답)
    NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ,     // 아이템 목록 요청
    NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK,     // 아이템 목록 응답

    NET_MSG_CONSIGNMENT_ADD_ITEM_REQ,           // 아이템 등록 요청
    NET_MSG_CONSIGNMENT_ADD_ITEM_ACK,           // 아이템 등록 응답

    NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ,        // 개인상점 검색 아이템 등록 요청(Field->Cache, DB->Cache)

    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ, // 아이템 반환 요청
    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK, // 아이템 반환 응답
    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB, // 아이템 반환 응답( Field DB-> Field )
   
    NET_MSG_CONSIGNMENT_SALE_PURCHASE_REQ,      //! 개인상점 아이템 대금 지불 및 구매(Cache->Field)
    NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK,      //! 아이템 구매요청에 대한 DB응답(DB->Field)

    NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK, //! 개인상점 슬롯 만기 시간

	NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_REQ,      //! 개인상점 아이템 대금 지불(Cache->Field)
	NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_ACK,      //! 개인상점 아이템 대금 지불 결과(Field->Cache)
	NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK,    //! 개인상점 아이템 아이템 처리 결과(DB->Cache)
	NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ, //! 개인상점 우편 발송(Cache->Field)
	NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, // 아이템 구매 처리 응답( Field DB-> Field )
	NET_MSG_GCTRL_PMARKET_SEND_DATA_FF, // 개인상점 정보를 캐시에 보내주기 위해서 메시지 처리 방식으로 필드가 다시 받아서 처리( Field-> Field )

    //////////////////////////////////////////////////////////////////////////


	// 텍사스홀덤 메시지
	NET_MSG_GCTRL_TEXAS_HOLD_EM, 
	NET_MSG_BLOCK_USER,

	NET_MSG_POST_GOT_DATA_CHANGE, 
	NET_MSG_POST_GOT_STATE_CHANGE, 
	NET_MSG_POST_GOT_SEND_STATE_CHANGE, 
	NET_MSG_POST_GOT_RECV_STATE_CHANGE, 

	NET_MSG_CHAR_MONEY_NOTIFY_FA, 

	//////////////////////////////////////////////////////////////////////////
	// 우편 개선
	// cwback
	NET_MSG_POST_RENEW_LIST_REQ,	// 우편 목록
	NET_MSG_POST_RENEW_LIST_ACK,
	NET_MSG_POST_RENEW_SEND_REQ,	// 우편 전송
	NET_MSG_POST_RENEW_SEND_ACK,
	NET_MSG_POST_RENEW_CHA_CHECK_ACK,// 캐릭터 체크( 필드서버 사용 )
	NET_MSG_POST_RENEW_DEL_REQ,		// 우편 삭제
	NET_MSG_POST_RENEW_DEL_ACK,
	NET_MSG_POST_RENEW_ATTACH_REQ,	// 우편 받기
	NET_MSG_POST_RENEW_ATTACH_ACK,
	NET_MSG_POST_RENEW_READ_REQ,	// 우편 읽기
	NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ,	// 우편 첨부 롤백
	NET_MSG_POST_RENEW_RETURN_REQ,	// 우편 반송
	NET_MSG_POST_RENEW_RETURN_ACK,
	NET_MSG_POST_RENEW_NEW_POST,	// 새 우편 데이터

	// 스킬 쿨타임 감소 효과 버그 수정 ( 클라와 동기화 )
	NET_MSG_GCTRL_SKILLDELAY,		// 스킬 딜레이(쿨타임,딜래이) 감소 
	
	NET_MSG_COUNTRECORDER_INFO_REQ,
	NET_MSG_COUNTRECORDER_INFO_FB,

	NET_MSG_GM_NOTIFY_MONEY_CAF,
	NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC, 

	NET_MSG_GCTRL_CLUB_STORAGE_USER_RESET,
	NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_RESET, 
	NET_MSG_GCTRL_CLUB_STORAGE_USER_FB,
	NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_FB,

	NET_MSG_GCTRL_INVEN_COSTUME_STATS,		//!	코스튬 능력치 부여
	NET_MSG_GCTRL_INVEN_COSTUME_STATS_FB,	//!	코스튬 능력치 부여 FB.
	NET_MSG_LOG_INVEN_COSTUME_STATS,		//! 코스튬 능력치 로그 

	NET_MSG_DRUG_FB,

	NET_MSG_DEFAULT_GAME_OPTION,

	NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE,	//! 코스튬 능력치 부여 해제
	NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE_FB,	//! 코스튬 능력치 부여 해제 FB

	NET_MSG_GCTRL_PARTY_FNEW_UPDATE,	//!	파티 생성. (field).

	NET_MSG_DISCONNECT_CHARACTER, 
	NET_MSG_CTF_NEXT_READY_TIME, 
	NET_MSG_SEND_SERVER_STOP_MSG, 

	NET_MSG_GCTRL_INVEN_TL_GRINDING,	//!	INVEN 아이템 일시강화 시도.

	NET_MSG_GCTRL_LEVEL_LAYER_FC,		//! Level 의 Layer 가 활성 혹은 비활성 되었다고 Client 에게 알림.

	NET_MSG_MACROMODE_ONOFF_CF,		// 매크로 활성화;
	NET_MSG_MACROMODE_ONOFF_FB,		// 위 메세지 답변;
	NET_MSG_MACROMODE_CLOSE_FC,		// 매크로가 서버에 의해 종료될시 전달될 메세지;
	NET_MSG_MACROMODE_STATE_FC,		// 서버에서 상태 변화를 알림;
	NET_MSG_MACROMODE_REQNEARESTCROWINFO_CF,
	NET_MSG_MACROMODE_NEARESTCROWINFO_FC,

	NET_MSG_GCTRL_PRISON_AG,
	NET_MSG_GCTRL_FORCERESET_SKILL_STAT_FC,

    /*
     * redmine : #1161 Randombox 확률제어 시스템 처리
     * created : sckim, 2015.12.11, 신규등록
     * modified : 
     */
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA, // Randombox 확률제어 데이터 등록 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC,  // Randombox 확률제어 데이터 등록 결과 Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH, // Randombox 확률제어 데이터 등록 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA,  // Randombox 확률제어 데이터 등록 결과 Feedback

     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA,     // Randombox 확률제어 데이터 조회 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC,      // Randombox 확률제어 데이터 조회 결과 Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH,     // Randombox 확률제어 데이터 조회 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA,      // Randombox 확률제어 데이터 조회 결과 Feedback

     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA,   // Randombox 확률제어 데이터 삭제 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC,    // Randombox 확률제어 데이터 삭제 결과 Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH,   // Randombox 확률제어 데이터 삭제 요청
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA,    // Randombox 확률제어 데이터 삭제 결과 Feedback

	 //NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN,	//!	random box open 요청. 
	 //NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB,	//!	random box open 요청 FB.
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH, // Randombox Open시 확률제어 적용여부 요청
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF,  // Randombox Open시 확률제어 적용여부 결과 Feedback
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH, // Randombox Open시 확률제어 적상적용 여부를 CacheServer에 전달

     NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA,   // Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달
     NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC,   // Randombox Open시 당첨확률이 5%미만의 경우 공지 메시지를 전달

	 NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT2, 

	 NET_MSG_GCTRL_PET_JUMP_POS,
	 NET_MSG_GCTRL_PET_JUMP_POS_FB,

	// Money Log
	NET_MSG_GCTRL_LOG_CHANGEMONEY_FH,

	// Jackpot
	NET_MSG_GCTRL_JACKPOT_PUSH, 
	NET_MSG_GCTRL_JACKPOT_NOTIFY, 
	NET_MSG_GCTRL_JACKPOT_OPTION, 

	NET_MSG_GM_RELEASE_CTF_BUFF, 

    //! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
    NET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF,   //! 랜덤박스 당첨시 공지 확률 Reload 요청

	NET_MSG_GCTRL_CLUB_RENAME_CF, 
	NET_MSG_GCTRL_CLUB_RENAME_DF, 
	NET_MSG_GCTRL_CLUB_RENAME_FA, 
	NET_MSG_GCTRL_CLUB_RENAME_AF, 
	NET_MSG_GCTRL_CLUB_RENAME_CLT, 
	NET_MSG_GCTRL_CLUB_RENAME_BRD, 
	NET_MSG_GCTRL_CLUB_RENAME_2ALLIANCE, 
	NET_MSG_GCTRL_CLUB_RENAME_FB, 

	// 캐릭터 슬롯;
	NET_MSG_REQUEST_LOCK_CHARSLOT_CF,
	NET_MSG_REQUEST_UNLOCK_CHARSLOT_CF,
	NET_MSG_REQUEST_CHANGE_CHARSLOT_CF,
	NET_MSG_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF,
	NET_MSG_RESPONSE_LOCK_CHARSLOT_FC,
	NET_MSG_RESPONSE_UNLOCK_CHARSLOT_FC,
	NET_MSG_RESPONSE_CHANGE_CHARSLOT_FC,
	NET_MSG_UPDATE_CHARSLOT_FC,

	// 아이템 스킬 변경;
	NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_FB,

	// 아이템 기본능력치 변경;
	NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_FB,

	// 아이템 가산능력치 변경;
	NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_FB,

	NET_MSG_SKILL_RESET_CF, 

	//================================== 기준점 ==================================
	// !!!!!!!!!! 주의 !!!!!!!!!!
	// 네트워크 프로파일러가 적용되기 때문에 패킷 Define의 최대 크기는 5000 이 넘어간다면 CPROFILE 에서 최대 크기를 수정해야한다.
	// 동적으로 NET_MSG_GCTRL_MAX 을 가져가도록 할려고했지만 굳이 네트워크단과 엔진을 엮을필요가 없어서 고정으로 정의함. 
	// 또한 최대한 부하를 줄이기 위해서 맵등의 컨테이너를 사용하지 않고 배열을 사용해서 직접 접근하도록 함.
	NET_MSG_GCTRL_MAX,	
	//============================================================================
};

struct NET_MSG_PACK;
struct NET_MSG_GENERIC;

typedef std::tr1::shared_ptr<msgpack::sbuffer> MsgPackBufferPtr;

template <class T>
struct STemplate
{
    DWORD dwChaDbNum;

    std::vector<T> vecT;
    MSGPACK_DEFINE( dwChaDbNum, vecT );
};

template <class T>
class MSGPACK_AUTO_PACK
{
public:
    MSGPACK_AUTO_PACK( DWORD dwChaDbNum )
        : m_dwChaDbNum( dwChaDbNum )
    {
        int nLimite=0;

        int nHeadSize = sizeof(NET_MSG_GENERIC);
        int nBodySize = sizeof(T);

        while( nHeadSize + (nBodySize * nLimite) < NET_DATA_BUFSIZE )
            nLimite++;

        GASSERT( nLimite && "SIZE IS BIG!" );

        m_nLimite = nLimite;
    }

    void PushData( const T& t )
    {
        m_queValue.push(t);
    }

    std::vector<MsgPackBufferPtr> GetVecBuffer()
    {
        std::vector<MsgPackBufferPtr> vecBuffer;

        while( m_queValue.empty() == false )
        {
            STemplate<T> sTemp;
            {
                sTemp.dwChaDbNum = m_dwChaDbNum;
                sTemp.vecT.clear();
                sTemp.vecT.reserve( m_nLimite );
            }
            

            for( int i = 0 ; i < m_nLimite ; i++ )
            {
                if( m_queValue.empty() )
                    break;

                sTemp.vecT.push_back( m_queValue.front() );
                m_queValue.pop();
            }

            MsgPackBufferPtr spSBuffer = MsgPackBufferPtr( new msgpack::sbuffer );
            msgpack::pack( *spSBuffer, sTemp);

            vecBuffer.push_back( spSBuffer );
        }

        return vecBuffer;
    }

private:
    std::queue<T>       m_queValue;
    int                 m_nLimite;
    DWORD               m_dwChaDbNum;
};

template <class T>
class MSGPACK_AUTO_UNPACK
{
public:
    // ChaNum이 필요 없는 경우
    bool GetVecData( NET_MSG_GENERIC* msg, std::vector<T>& vecOut OUT )
    {
        DWORD dwTemp = 0;
        return GetVecData( (NET_MSG_PACK*)msg, vecOut, dwTemp );
    }

    bool GetVecData( NET_MSG_PACK* pPacket, std::vector<T>& vecOut OUT )
    {
        DWORD dwTemp = 0;
        return GetVecData( pPacket, vecOut, dwTemp );
    }

    // ChaDbNum이 필요할 때
    bool GetVecData( NET_MSG_GENERIC* msg, std::vector<T>& vecOut OUT, DWORD& dwChaDbNum OUT )
    {
        return GetVecData( (NET_MSG_PACK*)msg, vecOut, dwChaDbNum );
    }
    
    bool GetVecData( NET_MSG_PACK* pPacket, std::vector<T>& vecOut OUT, DWORD& dwChaDbNum OUT )
    {
        STemplate<T> RecvData;
        if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
            return false;

        vecOut.reserve( RecvData.vecT.size() );
        vecOut = RecvData.vecT;

        dwChaDbNum = RecvData.dwChaDbNum;
        return true;
    }
};


// Generic messaeg
// 8 bytes
struct NET_MSG_GENERIC
{ 
protected:
	DWORD	  dwSize; //! Size of Message Data
	// 메시지 셔플 기능
	// RanLogic\Msg\MessageEnumMixer.h 파일
	
public:
// 기능을 코드상에서 off 시키려면	
// _USE_MESSAGE_SHUFFLE_ 구문을 주석할것
#ifdef _USE_MESSAGE_SHUFFLE_
	EmType	  nType; //! Message type
	inline EMNET_MSG Type() const		{ return nType.GetType(); }
#else
	EMNET_MSG nType; //! Message type
	inline EMNET_MSG Type() const		{ return nType; }
#endif


    inline DWORD Size() const			{ return dwSize; }
    inline void SetSize(DWORD _Size)	{ dwSize=_Size; }
    inline void SetType(EMNET_MSG _Type){ nType=_Type; }

	NET_MSG_GENERIC(EMNET_MSG _Type=NET_MSG_BASE_ZERO)
        : nType(_Type)
	{
        dwSize = sizeof(NET_MSG_GENERIC);
	}
	NET_MSG_GENERIC(EMNET_MSG _Type, const DWORD nSize)
		: nType(_Type)
		, dwSize(nSize)
	{
	}
};
typedef NET_MSG_GENERIC* LPNET_MSG_GENERIC;

enum 
{ 
    NET_MSG_GENERIC_SIZE2 = sizeof(NET_MSG_GENERIC),
};

//mjeon.AF
// Character message
// 8 + 4 = 12 bytes
// FA, AF 혹은 이를 포함하는 CAF, FAC 종류의 모든 메시지는 이것을 상속받아라.
struct NET_MSG_CHARACTER : public NET_MSG_GENERIC	
{
	NET_MSG_CHARACTER()
		: dwChaNum(0)
	{
	}

	DWORD	dwChaNum;	// 어떤 캐릭터에 대한 메시지인지 명확할 경우, 이 값을 사용해서 Relay등을 한다.
};
typedef NET_MSG_CHARACTER* LPNET_MSG_CHARACTER;


/*
mjeon.instance

 Agent -> Match -> Instance(여러개 중 특정 인스턴스) 방향
	or
 Instance -> Match -> Agent(특정 서버군) 방향
   으로의 전달에 사용
*/
struct NET_MSG_UNIQUEKEY : public NET_MSG_CHARACTER
{
	NET_MSG_UNIQUEKEY()
		:dwSvrGroupNum(0)
	{
	}

	DWORD	dwSvrGroupNum;	// (인스턴스서버)특정 서버군의 특정 캐릭터에 대한 메시지인지 명확할 경우, 이 값을 사용해서 Relay등을 한다.
};
typedef NET_MSG_UNIQUEKEY* LPNET_MSG_UNIQUEKEY;



/************************************************************************/
/* Agent <-> Agent Message;                                             */
/************************************************************************/
struct NET_MSG_GENERIC_RELAY_AGENT : public NET_MSG_CHARACTER
{
	enum EMMSG_RELAY_TYPE
	{
		EMMSG_RELAY_TYPE_DEFAULT,
		EMMSG_RELAY_TYPE_AGENTTOCLIENT,
		EMMSG_RELAY_TYPE_AGENTTOFIELD,
		EMMSG_RELAY_TYPE_AGENTTOALLCLIENT,
	};

	NET_MSG_GENERIC_RELAY_AGENT ()
		: emRelayType ( EMMSG_RELAY_TYPE_DEFAULT )
	{

	}

	EMMSG_RELAY_TYPE emRelayType;
};


//! Json message
struct NET_JSON : public NET_MSG_GENERIC
{
    enum { BUF_SIZE=NET_DATA_BUFSIZE - sizeof(NET_MSG_GENERIC), };
    char Buffer[BUF_SIZE];

    NET_JSON()
    {
        dwSize = sizeof(NET_JSON);
        nType = NET_MSG_JSON;
        memset(Buffer, 0, sizeof(char)*BUF_SIZE);
        MIN_STATIC_ASSERT(sizeof(NET_JSON) <= NET_DATA_BUFSIZE);
    }

    bool SetData(const std::string& JsonStr)
    {
        size_t JsonStrLen = JsonStr.length();
        if (JsonStrLen > (BUF_SIZE-1))
            return false;
        StringCchCopy(Buffer, BUF_SIZE, JsonStr.c_str());
        dwSize = (WORD) (sizeof(NET_MSG_GENERIC) + JsonStrLen+1);
        return true;
    }
};

//! 2012-07-12 jgkim
//! MsgPack http://wiki.msgpack.org/display/MSGPACK/Home
struct NET_MSG_PACK : public NET_MSG_GENERIC
{
    enum {
        HEAD_SIZE   = sizeof(NET_MSG_GENERIC) + sizeof(size_t) + sizeof(unsigned int),
        BUFFER_SIZE = NET_DATA_BUFSIZE - HEAD_SIZE,
    };

    unsigned int m_Crc;
    size_t m_DataSize;
    char m_Buffer[BUFFER_SIZE];

	NET_MSG_PACK()
        : m_Crc(0)
        , m_DataSize(0)
	{
        MIN_STATIC_ASSERT(sizeof(NET_MSG_PACK) <= NET_DATA_BUFSIZE);

		dwSize = sizeof(NET_MSG_PACK);
		nType = NET_MSG_GCTRL_MAX;
	}

    bool SetData(EMNET_MSG MsgType, const msgpack::sbuffer& sbuf, bool UseCrc=false, bool MsgBox=false);
    inline size_t DataSize() const { return m_DataSize; }
    inline const char* DataBuffer() const { return m_Buffer; }
    inline unsigned int Crc() const { return m_Crc; }
};

template<class T>
bool CastNetMsg(const NET_MSG_GENERIC* const pMsg, T& NetMsg)
{
    if (pMsg)
    {
        const NET_MSG_PACK* const pPacket = static_cast<const NET_MSG_PACK* const>(pMsg);
        return msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), NetMsg);
    }

    return false;
}

struct NET_HEARTBEAT_CLIENT_REQ : public NET_MSG_GENERIC
{
    NET_HEARTBEAT_CLIENT_REQ()
    {
        dwSize = sizeof(NET_HEARTBEAT_CLIENT_REQ);
        nType = NET_MSG_HEARTBEAT_CLIENT_REQ;
    }
};

struct NET_HEARTBEAT_CLIENT_ANS : public NET_MSG_GENERIC
{
	CHAR	szEnCrypt[ENCRYPT_KEY+1];

	NET_HEARTBEAT_CLIENT_ANS()
	{
		dwSize = sizeof( NET_HEARTBEAT_CLIENT_ANS );
		nType = NET_MSG_HEARTBEAT_CLIENT_ANS;
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};

struct NET_HEARTBEAT_SERVER_REQ : public NET_MSG_GENERIC
{	
	NET_HEARTBEAT_SERVER_REQ()
	{
		dwSize = sizeof( NET_HEARTBEAT_SERVER_REQ );
		nType  = NET_MSG_HEARTBEAT_SERVER_REQ; //! Server HeartBeat Request
	}
};

struct NET_HEARTBEAT_SERVER_ANS : public NET_MSG_GENERIC
{	
	NET_HEARTBEAT_SERVER_ANS()
	{
		dwSize = sizeof(NET_HEARTBEAT_SERVER_ANS);
		nType  = NET_MSG_HEARTBEAT_SERVER_ANS; //! Server HeartBeat Answer
	}
};

//! Compressed or combined message
struct NET_COMPRESS : public NET_MSG_GENERIC
{	
	bool bCompress; //! Compressed or not (combined)

	NET_COMPRESS()
	{
		dwSize = sizeof(NET_COMPRESS);
		nType  = NET_MSG_COMPRESS;
		bCompress = true;
	}
};
typedef NET_COMPRESS* LPNET_COMPRESS;

// Confirmation Message
struct NET_CONF : public NET_MSG_GENERIC
{	
	USHORT			conf;
	NET_CONF()
	{
		dwSize = sizeof(NET_CONF);
	}
};
typedef NET_CONF* LPNET_CONF;

struct NET_FEEDBACK_DATA : public NET_MSG_GENERIC
{	
	USHORT nResult;
	NET_FEEDBACK_DATA()
	{
		dwSize = sizeof(NET_FEEDBACK_DATA);
	}
};
typedef NET_FEEDBACK_DATA* LPNET_FEEDBACK_DATA;

struct NET_CRYPT_KEY : public NET_MSG_GENERIC
{	
	CRYPT_KEY ck;

	NET_CRYPT_KEY()
	{
		dwSize = sizeof(NET_CRYPT_KEY);
		nType = NET_MSG_SND_CRYT_KEY;
	}
};
typedef NET_CRYPT_KEY* LPNET_CRYPT_KEY;

struct NET_RANDOMPASS_NUMBER : public NET_MSG_GENERIC
{	
	INT nRandomNumber;
	NET_RANDOMPASS_NUMBER()
	{
		dwSize = sizeof(NET_RANDOMPASS_NUMBER);
		nType  = NET_MSG_RANDOM_NUM;
		nRandomNumber = 0;
	}
};

// Control program->Session Server
// Server state change command 
// NET_MSG_SVR_PAUSE, // 서버를 잠시 멈춘다.
// NET_MSG_SVR_RESUME, // 멈추어진 서버를 다시 가동시킨다.	
// NET_MSG_SVR_RESTART, // 서버를 완전히 멈추고 새롭게 가동시킨다.
// NET_MSG_SVR_STOP, // 서버를 완전히 멈춘다.
struct NET_SVR_CMD_CHANGE_STATE : public NET_MSG_GENERIC
{	
	int nCmdType;
	int nSvrType;
	int nSvrGrp;
	int nSvrNum;
	int nSvrField;

	NET_SVR_CMD_CHANGE_STATE()
	{
		dwSize = sizeof(NET_SVR_CMD_CHANGE_STATE);
		nType = NET_MSG_SVR_CMD;
		nCmdType  = NET_MSG_SVR_RESTART;
		nSvrType  = -1;
		nSvrGrp   = -1;
		nSvrNum   = -1;
		nSvrField = -1;
	}
};
typedef NET_SVR_CMD_CHANGE_STATE* LPNET_SVR_CMD_CHANGE_STATE;

struct NET_CHARACTER_INC_DEC : public NET_MSG_GENERIC
{	
	int nUserNum;	

	NET_CHARACTER_INC_DEC()
	{
		dwSize = sizeof(NET_CHARACTER_INC_DEC);
		nType = NET_MSG_CHA_DECREASE;
		nUserNum  = 0;		
	}
};
typedef NET_CHARACTER_INC_DEC* LPNET_CHARACTER_INC_DEC;

struct NET_CREATE_ACCOUNT_DATA : public NET_MSG_GENERIC
{	
	CHAR szAccountName[USR_ID_LENGTH+1];
	CHAR szPassword[USR_PASS_LENGTH+1];
	CHAR szUserName[USR_ID_LENGTH+1];

	NET_CREATE_ACCOUNT_DATA()
	{
		nType = EMNET_MSG(0);
		dwSize = sizeof(NET_CREATE_ACCOUNT_DATA);
		memset(szAccountName, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szPassword, 0, sizeof(CHAR) * (USR_PASS_LENGTH+1));
		memset(szUserName, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
	}	
};
typedef NET_CREATE_ACCOUNT_DATA* LPNET_CREATE_ACCOUNT_DATA;

struct NET_CLIENT_VERSION_CS : public NET_MSG_GENERIC
{
    int	m_LauncherVersion;
	int	m_PatchVersion;

	NET_CLIENT_VERSION_CS(int LauncherVer, int PatchVer)
		: m_LauncherVersion(LauncherVer)
		, m_PatchVersion(PatchVer)
	{		
		dwSize = sizeof(NET_CLIENT_VERSION_CS);
        nType = NET_MSG_CLIENT_VERSION_CS;
	}
};

struct NET_CLIENT_VERSION_SC : public NET_CLIENT_VERSION_CS
{ 
    NET_CLIENT_VERSION_SC(int LauncherVer, int PatchVer)
        : NET_CLIENT_VERSION_CS(LauncherVer, PatchVer)
    {		
        dwSize = sizeof(NET_CLIENT_VERSION_SC);
        nType = NET_MSG_CLIENT_VERSION_SC;
    }
};

struct NET_CLIENT_VERSION_AC : public NET_CLIENT_VERSION_CS
{    
    NET_CLIENT_VERSION_AC(int LauncherVer, int PatchVer)
        : NET_CLIENT_VERSION_CS(LauncherVer, PatchVer)
    {		
        dwSize = sizeof(NET_CLIENT_VERSION_AC);
        nType = NET_MSG_CLIENT_VERSION_AC;
    }
};

//! Login->Session:버전 정보
struct NET_VERSION_LS : public NET_MSG_GENERIC
{
    int	m_LauncherVersion;
    int	m_PatchVersion;

    NET_VERSION_LS(int LauncherVer, int PatchVer)
        : m_LauncherVersion(LauncherVer)
        , m_PatchVersion(PatchVer)
    {		
        dwSize = sizeof(NET_VERSION_LS);
        nType = NET_MSG_VERSION_LS;
    }
};

struct NET_VERSION_SS : public NET_VERSION_LS
{
    NET_VERSION_SS(int LauncherVersion, int PatchVersion)
        : NET_VERSION_LS(LauncherVersion, PatchVersion)
    {
        dwSize = sizeof(NET_VERSION_SS);
        nType = NET_MSG_VERSION_SS;
    }
};

struct NET_REQ_RAND_KEY : public NET_MSG_GENERIC
{	
	NET_REQ_RAND_KEY()
	{		
		dwSize = sizeof(NET_REQ_RAND_KEY);
		nType = NET_MSG_REQ_RAND_KEY;
	}
};
typedef NET_REQ_RAND_KEY* LPNET_REQ_RAND_KEY;

struct NET_ENCRYPT_KEY : public NET_MSG_GENERIC
{	
	CHAR szEncryptKey[ENCRYPT_KEY+1];

	NET_ENCRYPT_KEY()
	{		
		dwSize = sizeof(NET_ENCRYPT_KEY);
		nType = NET_MSG_SND_ENCRYPT_KEY;
		memset(szEncryptKey, 0, sizeof(CHAR) * (ENCRYPT_KEY+1));
	}
};
typedef NET_ENCRYPT_KEY* LPNET_ENCRYPT_KEY;

/*
struct NET_MSG_PING
{
	NET_MSG_GENERIC nmg;
	DWORD			stime;
	NET_MSG_PING()
	{
		memset(this, 0, sizeof(NET_MSG_PING));
		nmg.dwSize = sizeof(NET_MSG_PING);
	}
};
typedef NET_MSG_PING* LPNET_MSG_PING;
*/

struct NET_NEW_CHA : public NET_MSG_GENERIC
{
	int	 nIndex;	 //! 케릭터 종류/class
	WORD wSchool;	 //! 학교
	WORD wFace;		 //! 얼굴
	WORD wHair;		 //! 헤어 스타일
	WORD wHairColor; //! 헤어 컬러
	WORD wSex;		 //! 성별
	char szChaName[CHR_ID_LENGTH+1];
    unsigned int m_Crc;

	NET_NEW_CHA()
        : nIndex(0)
        , wSchool(0)
        , wFace(0)
        , wHair(0)
        , wHairColor(0)
        , wSex(0)
        , m_Crc(0)
	{
		dwSize = sizeof(NET_NEW_CHA);
		nType = NET_MSG_CHA_NEW;
		memset(szChaName, 0, sizeof(char) * (CHR_ID_LENGTH+1));
	}
};
//typedef NET_NEW_CHA* LPNET_NEW_CHA;
enum { EM_NET_NEW_CHA_MSG_DATA_SIZE = sizeof(NET_NEW_CHA)-sizeof(unsigned int), };

struct NET_NEW_CHA_FB : public NET_MSG_GENERIC
{	
	int					nResult;
	int					nChaNum;
    WORD                wChaRemain;
	bool				bExtreme; // 극강부 생성 가능 여부;
	bool				bActor;
	bool				m_bJumping;

	NET_NEW_CHA_FB()
	{
		dwSize  = sizeof(NET_NEW_CHA_FB);
		nType  = NET_MSG_CHA_NEW_FB;
        nChaNum    = 0;
        wChaRemain = 0;
		bActor = false;
		m_bJumping = false;
	}
};
typedef NET_NEW_CHA_FB* LPNET_NEW_CHA_FB;

// 로비에서 디스플레이 할 캐릭터의 기초정보를 요청한다.
struct NET_CHA_REQ_BA_INFO : public NET_MSG_GENERIC
{
	// int             nChannel; // 사용자가 조인하려는 채널
    int m_LauncherVer;
    int m_PatchVer;
    unsigned int m_Crc;

    NET_CHA_REQ_BA_INFO(int LauncherVer, int PatchVer)
        : m_LauncherVer(LauncherVer)
        , m_PatchVer(PatchVer)
        , m_Crc(0)
	{
		dwSize = sizeof(NET_CHA_REQ_BA_INFO);
		nType = NET_MSG_REQ_CHA_BAINFO;
		// nChannel  = 0;
	}
};
//typedef NET_CHA_REQ_BA_INFO* LPNET_CHA_REQ_BA_INFO;
enum { NET_CHA_REQ_BA_INFO_DATA_SIZE=sizeof(NET_CHA_REQ_BA_INFO)-sizeof(unsigned int), };

// 해당 캐릭터의 정보를 요청한다.
struct NET_CHA_BA_INFO_CA : public NET_MSG_GENERIC
{
	int m_ChaDbNum;
	NET_CHA_BA_INFO_CA()
	{
		nType	= NET_MSG_REQ_CHA_BINFO_CA;
		dwSize = sizeof(NET_CHA_BA_INFO_CA);
		m_ChaDbNum = 0;
	}
};
typedef NET_CHA_BA_INFO_CA* LPNET_CHA_BA_INFO;

struct NET_CHA_BA_INFO_CA_PACK
{
    std::vector<DWORD> vecChaDbNum;
    // NET_MSG_REQ_CHA_BINFO_CA

    MSGPACK_DEFINE( vecChaDbNum );

    NET_CHA_BA_INFO_CA_PACK() {}
};



struct NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT	 m_ChaDbNum; // 삭제할 캐릭터 번호
	char m_szPass2[USR_PASS_LENGTH+1];
	NET_CHA_DEL(int ChaDbNum, const char* szPass2)
        : m_ChaDbNum(ChaDbNum)
	{
		nType  = NET_MSG_CHA_DEL;
		dwSize = sizeof(NET_CHA_DEL);
        if (szPass2)
		    StringCchCopyA(m_szPass2, USR_PASS_LENGTH+1, szPass2);
        else
            memset(m_szPass2, 0, USR_PASS_LENGTH+1);
	}
};
typedef NET_CHA_DEL* LPNET_CHA_DEL;

struct DAUM_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT m_ChaDbNum; //! 삭제할 캐릭터 번호
	char m_szPass2[USR_PASS_LENGTH+1];
	
	DAUM_NET_CHA_DEL(int ChaDbNum, const char* szPass2)
        : m_ChaDbNum(ChaDbNum)
	{
		nType	= DAUM_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(DAUM_NET_CHA_DEL);

        if (szPass2)
		    StringCchCopyA(m_szPass2, USR_PASS_LENGTH+1, szPass2);
        else
            memset(m_szPass2, 0, USR_PASS_LENGTH+1);
	}
};
typedef DAUM_NET_CHA_DEL* LPDAUM_NET_CHA_DEL;


//
//mjeon.indonesia
//
struct IDN_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // 삭제할 캐릭터 번호
	char				sz2ndPass[MD5_MAX_LENGTH+1];
	
	IDN_NET_CHA_DEL(int ChaDbNum)
        :nChaNum(ChaDbNum)
	{
		nType	= IDN_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(IDN_NET_CHA_DEL);
		
		ZeroMemory(sz2ndPass, sizeof(sz2ndPass));
	}
};
typedef IDN_NET_CHA_DEL* LPIDN_NET_CHA_DEL;



/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct TERRA_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	TERRA_NET_CHA_DEL()
	{
		nType	= TERRA_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(TERRA_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef TERRA_NET_CHA_DEL* LPTERRA_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct GSP_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	GSP_NET_CHA_DEL()
	{
		dwSize = sizeof(GSP_NET_CHA_DEL);
		nType = GSP_NET_MSG_CHA_DEL;
		nChaNum = 0;
	}
};
typedef GSP_NET_CHA_DEL* LPGSP_NET_CHA_DEL;

/**
 * 캐릭터 삭제정보 Client->Agent
 * \return 
 */
struct EXCITE_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	EXCITE_NET_CHA_DEL()
	{
		nType	= EXCITE_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(EXCITE_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef EXCITE_NET_CHA_DEL* LPEXCITE_NET_CHA_DEL;

//! 캐릭터 삭제정보 Client->Agent
struct JAPAN_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // 삭제할 캐릭터 번호
	
	JAPAN_NET_CHA_DEL()
	{
		nType	= JAPAN_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(JAPAN_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef JAPAN_NET_CHA_DEL* LPJAPAN_NET_CHA_DEL;

//! 캐릭터 삭제정보 Client->Agent
struct GS_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT	 m_ChaDbNum; // 삭제할 캐릭터 번호
	char m_szPass2[GS_USER_PASS+1];

	GS_NET_CHA_DEL(int ChaDbNum, const char* szPass2)
        : m_ChaDbNum(ChaDbNum)
	{	
		nType	= GS_NET_MSG_CHA_DEL; // 클라이언트->게임서버 : 케릭터 삭제
		dwSize = sizeof(GS_NET_CHA_DEL);
		if (szPass2)
            StringCchCopyA(m_szPass2, GS_USER_PASS+1, szPass2);
        else
		    memset(m_szPass2, 0, sizeof(char) * (GS_USER_PASS+1));
	}
};
typedef GS_NET_CHA_DEL* LPGS_NET_CHA_DEL;

struct NET_CHA_DEL_FB : public NET_MSG_GENERIC
{	
	INT					nChaNum;	// 삭제한 캐릭터 번호
	BOOL				bExtreme;
	BOOL				bActor;
    WORD                wChaRemain; // 남은 캐릭터 생성 갯수
	NET_CHA_DEL_FB()
	{
		dwSize  = sizeof(NET_CHA_DEL_FB);
		nChaNum		= 0;
        wChaRemain	= 0;
		bExtreme	= FALSE;
		bActor		= FALSE;
	}
};
typedef NET_CHA_DEL_FB* LPNET_CHA_DEL_FB;

// 2차비번
struct NET_LOBBY_CHAR_JOIN_CA : public NET_MSG_GENERIC
{
	INT m_ChaDbNum; //! 게임 참가할 캐릭터 번호
	BOOL m_bNew2ndPass;
	char m_sz2ndPass[USR_PASS_LENGTH+1];
	NET_LOBBY_CHAR_JOIN_CA(INT _ChaDbNum, const char *sz2ndPass, BOOL bNewPass)
        : m_ChaDbNum(_ChaDbNum)
	{
		nType	= NET_MSG_LOBBY_CHAR_JOIN_CA;
		dwSize  = sizeof(NET_LOBBY_CHAR_JOIN_CA);
		if( sz2ndPass )
            StringCchCopyA(m_sz2ndPass, USR_PASS_LENGTH+1, sz2ndPass);
        else
		    memset(m_sz2ndPass, 0, sizeof(char) * (USR_PASS_LENGTH+1));

		m_bNew2ndPass = bNewPass;
	}
};
typedef NET_LOBBY_CHAR_JOIN_CA* LPNET_GAME_JOIN;
//////////////////////////////////////

// 2차비번 기존 메세지
//! 게임참가 메시지
//! Client -> Agent Server
//struct NET_LOBBY_CHAR_JOIN_CA : public NET_MSG_GENERIC
//{
//	INT m_ChaDbNum; //! 게임 참가할 캐릭터 번호
//	NET_LOBBY_CHAR_JOIN_CA(INT _ChaDbNum)
//        : m_ChaDbNum(_ChaDbNum)
//	{
//		nType	= NET_MSG_LOBBY_CHAR_JOIN_CA;
//		dwSize  = sizeof(NET_LOBBY_CHAR_JOIN_CA);
//	}
//};
//typedef NET_LOBBY_CHAR_JOIN_CA* LPNET_GAME_JOIN;

///////////////////////////////////////////////////////

enum EMGAME_JOINTYPE
{
	EMJOINTYPE_FIRST	= 0,	// 신규 접속
	EMJOINTYPE_MOVEMAP	= 1,	// 맵이동에 의한 필드서버 변경
	EMJOINTYPE_REBIRTH	= 2,	// 부활에 의한 필더서버 변경
	EMJOINTYPE_TRANSFER = 3,	// Serialization
    EMJOINTYPE_PVEINDUN = 4,    // PVE 인던 이동에 의한 필드서버 변경
	EMJOINTYPE_GATE		= 5,	// 게이트 이동;
};

// 게임참가 메시지
// Agent -> Field
struct NET_GAME_JOIN_FIELDSVR : public NET_MSG_GENERIC
{
	EMGAME_JOINTYPE		emType;
	DWORD				dwSlotAgentClient;

	char				szUID[USR_ID_LENGTH+1];
	INT					nUserNum;
	DWORD				dwUserLvl;

	INT					nChaNum;
	DWORD				dwGaeaID;
	GLPartyID			PartyID;

	MapID				sStartMap;
	DWORD				dwStartGate;
	D3DXVECTOR3			vStartPos;

	DWORD				dwActState;

    __time64_t          tPREMIUM;
    __time64_t          tCHATBLOCK;

	EMIP_BONUS_CLASS	emIPBonus;

	SChinaTime			sChinaTime;
	SEventTime			sEventTime;
	SVietnamGainSystem	sVietnamGainSystem;

	BOOL				bUseArmSub;	

    wb::EM_COUNTRY m_Country;

    bool bChangeField; // 출석부 준비 여부.

	SSUMMONABLE			sSummonable;
	DWORD				dwCountryID;

	CRYPT_KEY ckey;

    NET_GAME_JOIN_FIELDSVR(wb::EM_COUNTRY Country)
        : emType(EMJOINTYPE_FIRST)
        , dwSlotAgentClient(0)
        , nUserNum(0)
        , dwUserLvl(0)
        , nChaNum(0)
        , dwGaeaID(0)
        , dwStartGate(UINT_MAX)
        , vStartPos(0,0,0)
        , dwActState(NULL)
        , tPREMIUM(0)
        , tCHATBLOCK(0)
        , bUseArmSub(FALSE)
        , emIPBonus(EMIP_BONUS_NONE)
        , m_Country(Country)
        , bChangeField(false)
		, dwCountryID(0)
	{
		dwSize = sizeof(NET_GAME_JOIN_FIELDSVR);
		nType = MET_MSG_GAME_JOIN_FIELDSVR;
		memset(szUID, 0, sizeof(char)*(USR_ID_LENGTH+1));

		sChinaTime.Init();
		sEventTime.Init();
		sVietnamGainSystem.Init();
	}

    void SetUserId(const char* _UserId)
    {
        if (_UserId)
            StringCchCopy(szUID, USR_ID_LENGTH+1, _UserId);
    }
};
//typedef NET_GAME_JOIN_FIELDSVR* LPNET_GAME_JOIN_FIELDSVR;

struct NET_GAME_JOIN_FIELDSVR_FB : public NET_MSG_GENERIC
{
	EMGAME_JOINTYPE	m_JoinType;	
	DWORD		m_FieldServerNum;
	DWORD		m_GaeaID;
	DWORD		m_ChaDbNum;
    MapID		m_MapID;
    D3DXVECTOR3     m_MapPos;
	//DWORD			dwSlotFieldAgent;

    NET_GAME_JOIN_FIELDSVR_FB(
        EMGAME_JOINTYPE	JoinType,
        DWORD			FieldServerNum,
        DWORD           GaeaID,
        DWORD			ChaDbNum,
        const MapID& mapID,
        const D3DXVECTOR3& MapPos)
		: m_JoinType(JoinType) // EMJOINTYPE_FIRST
		, m_FieldServerNum(FieldServerNum)
		, m_GaeaID(GaeaID)
		, m_ChaDbNum(ChaDbNum)
        , m_MapID(mapID)
        , m_MapPos(MapPos)
	{
		nType   = MET_MSG_GAME_JOIN_FIELDSVR_FB;
		dwSize   = sizeof(NET_GAME_JOIN_FIELDSVR_FB);
	};	
};
typedef NET_GAME_JOIN_FIELDSVR_FB* LPNET_GAME_JOIN_FIELDSVR_FB;



struct NET_MSG_JOIN_INSTANCESVR_INFO : public NET_MSG_UNIQUEKEY//NET_MSG_GENERIC
{
	EMGAME_JOINTYPE	emType;
	DWORD           dwGaeaID;
	DWORD			dwKey;
	
	int				nServicePort;
	CHAR			szServerIP[MAX_IP_LENGTH+1];

	NET_MSG_JOIN_INSTANCESVR_INFO()
		:emType(EMJOINTYPE_FIRST)
		,dwGaeaID(0)
		,dwKey(MAXDWORD)
		,nServicePort(0)
	{
		nType   = MET_MSG_JOIN_INSTANCESVR_INFO;
		dwSize   = sizeof(*this);
	};

	void SetIP(const char* szIp)
    {
        if (szIp)
            StringCchCopyA(szServerIP, MAX_IP_LENGTH+1, szIp);
    }
};


//! Agent->Client:해당 Field 서버로 접속하라
struct NET_CONNECT_CLIENT_TO_FIELD_AC : public NET_MSG_GENERIC
{	
	EMGAME_JOINTYPE	emType;
	InstanceSystem::EMAUTHORITY	typeAuthority;
	DWORD			dwGaeaID;
	DWORD			dwChaNum;
	MapID		previousMapID;
	int				nServicePort;
	CHAR			szServerIP[MAX_IP_LENGTH+1];

	NET_CONNECT_CLIENT_TO_FIELD_AC() 
		:emType(EMJOINTYPE_FIRST)
		,dwGaeaID(GAEAID_NULL)
		,dwChaNum(0)
		,nServicePort(0)
	{
		dwSize = sizeof(NET_CONNECT_CLIENT_TO_FIELD_AC);
		nType = NET_MSG_CONNECT_CLIENT_FIELD;
		//dwSlotFieldAgent = 0;
	}

    void SetIP(const char* szIp)
    {
        if (szIp)
            StringCchCopyA(szServerIP, MAX_IP_LENGTH+1, szIp);
    }
};
typedef NET_CONNECT_CLIENT_TO_FIELD_AC* LPNET_CONNECT_CLIENT_TO_FIELD;

//! Client->Field:클라이언트가 새로이 Field Server와 연결후 자신의 ID 확인 요청.
struct NET_GAME_JOIN_FIELD_IDENTITY_CF : public NET_MSG_GENERIC
{
	EMGAME_JOINTYPE		emType;
	InstanceSystem::EMAUTHORITY	typeAuthority;
	DWORD				dwSvrGroupNum;
	DWORD				dwChaNum;
	DWORD				dwGaeaID;
	MapID				previousMapID;
	CRYPT_KEY			ck;

	NET_GAME_JOIN_FIELD_IDENTITY_CF()
		:emType(EMJOINTYPE_FIRST)
		,dwSvrGroupNum(0)
		,previousMapID(SNATIVEID(false))
	{
		dwSize = sizeof(*this);
		nType = NET_MSG_JOIN_FIELD_IDENTITY;
	}
};
typedef NET_GAME_JOIN_FIELD_IDENTITY_CF* LPNET_GAME_JOIN_FIELD_IDENTITY;

// 게임참가완료
// Field->Session
struct NET_GAME_JOIN_OK_FS : public NET_MSG_GENERIC
{	
	INT					nUsrNum; // 사용자 고유번호(DB)	
	INT					nChaNum; // 캐릭터번호(DB)
	DWORD				dwGaeaID; // 가이아 ID
	CHAR				szUserID[USR_ID_LENGTH+1]; // 사용자 ID	
	CHAR				szChaName[CHR_ID_LENGTH+1]; // 캐릭터명

	NET_GAME_JOIN_OK_FS()
	{		
		dwSize = sizeof(NET_GAME_JOIN_OK_FS);
		nType = NET_MSG_GAME_JOIN_OK;
		nUsrNum = -1;
		nChaNum = -1;
		dwGaeaID = -1;
		memset(szUserID, 0, sizeof(CHAR) * (USR_ID_LENGTH+1));
		memset(szChaName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
	}
};
typedef NET_GAME_JOIN_OK_FS* LPNET_GAME_JOIN_OK;

// 관리용 채팅메시지
// 컨트롤프로그램->세션서버
struct NET_CHAT_CTRL : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;
	CHAR			szServerIP[MAX_IP_LENGTH+1];
	INT				nServicePort;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];

	NET_CHAT_CTRL()
	{		
		nType    = NET_MSG_CHAT_CTRL;
		dwSize    = sizeof(NET_CHAT_CTRL);
		emType	     = CHAT_TYPE_CTRL_GLOBAL;
		nServicePort = 0;
		memset(szServerIP, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}

    void SetMsg(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szMsg);
    }
};
typedef NET_CHAT_CTRL* LPNET_CHAT_CTRL;

// 패킷 랜덤으로 인해서 서버매니져에서 사용하는 패킷은 별도로 추가 ( 기존 패킷을 사용하는 부분이 있을수 있어서 유지 )
struct NET_CHAT_CTRL_SVRMANAGER : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;
	CHAR			szServerIP[MAX_IP_LENGTH+1];
	INT				nServicePort;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];

	NET_CHAT_CTRL_SVRMANAGER()
	{		
		nType    = NET_MSG_CHAT_CTRL_SVRMANAGER;
		dwSize    = sizeof(NET_CHAT_CTRL_SVRMANAGER);
		emType	     = CHAT_TYPE_CTRL_GLOBAL;
		nServicePort = 0;
		memset(szServerIP, 0, sizeof(CHAR) * (MAX_IP_LENGTH+1));
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}

    void SetMsg(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szMsg);
    }
};

struct NET_CHAT_CTRL2 : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;	
	int				nSvrGrp;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	

	NET_CHAT_CTRL2()
	{
		nType	= NET_MSG_CHAT_CTRL;
		dwSize	= sizeof(NET_CHAT_CTRL2);
		emType		= CHAT_TYPE_CTRL_GLOBAL2;
		nSvrGrp		= 0;
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}

    void SetMsg(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szMsg);
    }
};
typedef NET_CHAT_CTRL2* LPNET_CHAT_CTRL2;

// 패킷 랜덤으로 인해서 서버매니져에서 사용하는 패킷은 별도로 추가 ( 기존 패킷을 사용하는 부분이 있을수 있어서 유지 )
struct NET_CHAT_CTRL2_SVRMANAGER : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;	
	int				nSvrGrp;
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	

	NET_CHAT_CTRL2_SVRMANAGER()
	{
		nType	= NET_MSG_CHAT_CTRL_SVRMANAGER;
		dwSize	= sizeof(NET_CHAT_CTRL2_SVRMANAGER);
		emType		= CHAT_TYPE_CTRL_GLOBAL2;
		nSvrGrp		= 0;
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}

    void SetMsg(const char* szMsg)
    {
        if (szMsg)
            StringCchCopy(szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szMsg);
    }
};

// 관리자용 메시지 FB
struct NET_CHAT_CTRL_FB : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;						// 채팅 타입
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	// 채팅메시지

	NET_CHAT_CTRL_FB ()
	{
		nType = NET_MSG_CHAT_CTRL_FB;
		dwSize = sizeof(NET_CHAT_CTRL_FB);
		emType = CHAT_TYPE_GLOBAL;
		memset(szChatMsg, 0, sizeof(CHAR) * (GLOBAL_CHAT_MSG_SIZE+1));
	}

    void SetChatMsg(const CHAR* Msg)
    {
        if (Msg)
            StringCchCopyA(szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, Msg);
    }
};
/*
struct NET_EVENT_LOTTERY : public NET_MSG_GENERIC
{	
	CHAR szLotteryName[11];

	NET_EVENT_LOTTERY()
	{		
		nType = NET_MSG_EVENT_LOTTERY;
		dwSize = sizeof(NET_EVENT_LOTTERY);
		memset(szLotteryName, 0, sizeof(CHAR) * 11);
	}
};
typedef NET_EVENT_LOTTERY* LPNET_EVENT_LOTTERY;
*/
///////////////////////////////////////////////////////////////////////////////
// 란 선물펑펑!! 이벤트 
// 리턴값 :
// 1  1 등 P-4 PC                        (1등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 2  2 등 효리폰                        (2등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 3  3 등 SONY 디카                     (3등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 4  4 등 아이리버 MP 플레이어          (4등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 5  5 등 MAX 무비 영화 상품권 1 만원권 (5등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.)
// 6  6 등 MBC Game 일일 무료 상품권     (6등에 당첨되셨습니다. 축하드립니다. 자세한 사항은 홈페이지를 참조해 주십시오.) 
// 7  이미 복권을 입력한 사용자          (이미 복권에 응모하셨습니다.)
// 8  PC 방 IP 가 아님                   (PC 방 IP 가 아닙니다. 복권은 PC 방에서만 응모 가능합니다)
// 9  이미 사용한 복권                   (이미 사용한 복권입니다)
// 10 알수없는 오류                      (복권처리중 오류가 발생하였습니다)
/*
struct NET_EVENT_LOTTERY_FB : public NET_MSG_GENERIC
{	
	int nResult;

	NET_EVENT_LOTTERY_FB()
	{
		nType = NET_MSG_EVENT_LOTTERY_FB;
		dwSize = sizeof(NET_EVENT_LOTTERY_FB);
		nResult = 10;
	}
};
typedef NET_EVENT_LOTTERY_FB* LPNET_EVENT_LOTTERY_FB;
*/
// 출석부 이벤트
struct USER_ATTEND_INFO
{
	__time64_t	tAttendTime;
	int			nComboAttend;
	bool		bAttendReward;

	USER_ATTEND_INFO()
	{
		tAttendTime = 0;
		nComboAttend = 0;
		bAttendReward = false;
	}
};
typedef USER_ATTEND_INFO* LPUSER_ATTEND_INFO;

//! nProtect GameGuard Auth 요청 메시지
struct NET_GAMEGUARD_AUTH : public NET_MSG_GENERIC
{	
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH()
	{
		nType  = NET_MSG_GAMEGUARD_AUTH;
		dwSize = sizeof(NET_GAMEGUARD_AUTH);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		packetSize = 0;
		bPass = false;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_AUTH* LPNET_GAMEGUARD_AUTH;

//! nProtect GameGuard Auth Answer 메시지
struct NET_GAMEGUARD_ANSWER : public NET_MSG_GENERIC
{
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];

	GG_AUTH_DATA ggad;
	
	CHAR	szEnCrypt[ENCRYPT_KEY+1];


	NET_GAMEGUARD_ANSWER()
	{
		nType  = NET_MSG_GAMEGUARD_ANSWER;
		dwSize = sizeof(NET_GAMEGUARD_ANSWER);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		SecureZeroMemory(szEnCrypt, sizeof(CHAR) * (ENCRYPT_KEY+1));
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_ANSWER* LPNET_GAMEGUARD_ANSWER;

//! nProtect GameGuard Auth 요청 메시지 #1
struct NET_GAMEGUARD_AUTH_1 : public NET_MSG_GENERIC
{	
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH_1()
	{
		nType  = NET_MSG_GAMEGUARD_AUTH_1;
		dwSize = sizeof(NET_GAMEGUARD_AUTH_1);
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	

	NET_GAMEGUARD_AUTH_1(const GG_AUTH_DATA& GGData)
        : ggad(GGData)
	{
		nType  = NET_MSG_GAMEGUARD_AUTH_1;
		dwSize = sizeof(NET_GAMEGUARD_AUTH_1);
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_AUTH_1* LPNET_GAMEGUARD_AUTH_1;

//! nProtect GameGuard Auth Answer 메시지 #1
struct NET_GAMEGUARD_ANSWER_1 : public NET_MSG_GENERIC
{
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_ANSWER_1()
	{
		nType  = NET_MSG_GAMEGUARD_ANSWER_1;
		dwSize = sizeof(NET_GAMEGUARD_ANSWER_1);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_ANSWER_1* LPNET_GAMEGUARD_ANSWER_1;

//! nProtect GameGuard Auth 요청 메시지 #2
struct NET_GAMEGUARD_AUTH_2 : public NET_MSG_GENERIC
{	
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_AUTH_2()
	{
		nType  = NET_MSG_GAMEGUARD_AUTH_2;
		dwSize = sizeof(NET_GAMEGUARD_AUTH_2);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_AUTH_2* LPNET_GAMEGUARD_AUTH_2;

//! nProtect GameGuard Auth Answer 메시지 #2
struct NET_GAMEGUARD_ANSWER_2 : public NET_MSG_GENERIC
{
	BOOL bPass;
	int packetSize;
	int packetSeq;
	BYTE packet[512];
	GG_AUTH_DATA ggad;

	NET_GAMEGUARD_ANSWER_2()
	{
		nType  = NET_MSG_GAMEGUARD_ANSWER_2;
		dwSize = sizeof(NET_GAMEGUARD_ANSWER_2);
		ggad.dwIndex = 0;
		ggad.dwValue1 = 0;
		ggad.dwValue2 = 0;
		ggad.dwValue3 = 0;
		bPass = false;
		packetSize = 0;
		packetSeq = 0;
		memset(packet, 0, sizeof(BYTE)*512);
	}	
};
typedef NET_GAMEGUARD_ANSWER_2* LPNET_GAMEGUARD_ANSWER_2;

// 각각의 아이템의 최대 고유번호를 가져오기 위한 구조체
struct VIEWLOGITEMEXCHANGEMAX
{
    LONGLONG llMaxNum;
    int  nMakeType;
    WORD wItemMain;
    WORD wItemSub;

    VIEWLOGITEMEXCHANGEMAX()
    {
        llMaxNum  = 0;
        nMakeType = 0;
        wItemMain = 0;
        wItemSub  = 0;
    }
};

// AGT->Session 으로 LoginMaintenance On/Off
struct SNET_SERVER_LOGINMAINTENANCE_SESSION : public NET_MSG_GENERIC
{	
	bool	bOn;

	SNET_SERVER_LOGINMAINTENANCE_SESSION ()
	{
		nType = NET_MSG_LOGIN_MAINTENANCE_SESSION;
		dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_SESSION );
		bOn = false;
	}
};

struct SNET_SERVER_LOGINMAINTENANCE_LOGIN : public NET_MSG_GENERIC
{	
	bool	bOn;

	SNET_SERVER_LOGINMAINTENANCE_LOGIN ()
	{
		nType = NET_MSG_LOGIN_MAINTENANCE_LOGIN;
		dwSize = sizeof( SNET_SERVER_LOGINMAINTENANCE_LOGIN );
		bOn = false;
	}
};

struct NET_PROTOCOL_INFO_CA : public NET_MSG_GENERIC
{
    DWORD m_MaxClientProtocolNum;

    NET_PROTOCOL_INFO_CA(DWORD MaxClientProtocolNum)
        : m_MaxClientProtocolNum(MaxClientProtocolNum)
    {
        nType = NET_MSG_PROTOCOL_INFO_CA;
        dwSize = sizeof(NET_PROTOCOL_INFO_CA);
    }    
};

struct NET_PROTOCOL_INFO_AC : public NET_MSG_GENERIC
{
    DWORD m_MaxServerProtocolNum;

    NET_PROTOCOL_INFO_AC(DWORD MaxServerProtocolNum)
        : m_MaxServerProtocolNum(MaxServerProtocolNum)
    {
        nType = NET_MSG_PROTOCOL_INFO_AC;
        dwSize = sizeof(NET_PROTOCOL_INFO_AC);
    }    
};

struct NET_BLOCK_USER : public NET_MSG_GENERIC
{
	DWORD dwUserNum;
	bool bBlock;	// true: block, false: unblock

	NET_BLOCK_USER()
		: dwUserNum( 0 )
		, bBlock( true )
	{
		dwSize = sizeof( NET_BLOCK_USER );
		nType  = NET_MSG_BLOCK_USER;
	}
};

struct RAN_MOBILE_INFO
{
	DWORD dwChaNum;
	char szChaName[50];
	__time64_t tStartData;
	__time64_t tExpiredDate;
	__time64_t tRewardDate;
	int nRMPoint;
	int nRMJewel;
	bool bDeleted;

	RAN_MOBILE_INFO()
	{
		dwChaNum = 0;
		nRMPoint = 0;
		nRMJewel = 0;
		bDeleted = false;
		memset(szChaName, 0, sizeof(char) * 50);
	}

	void SetName(std::string strName)
	{
		if( strName.size() < 50 )
		{
			StringCchCopy(szChaName, strName.size() + 1, strName.c_str());
		}
	}
};


///////////////////////////////////////////////////////////////////////////////
// GameMoney Encrypt/Decrypt Function
#define CHAR_TEMPMONEY 64		  //   TempMoney 최대 Char 배열의 길이

/*
#if defined(TW_PARAM) || defined(_RELEASED) 
	#define GAMEMONEY_VER 100		  //   GameMoney 버젼
#elif(HK_PARAM) 
	#define GAMEMONEY_VER 100		  //   GameMoney 버젼
#else 
	#define GAMEMONEY_VER 0		  //   GameMoney 버젼
#endif
*/
#define GAMEMONEY_VER 100		  //define값이 대입은 되고 있으나, 실제 사용되고 있지는 않다.


// 2009년 10월 이전의 암호화 방식
LONGLONG DECRYPTMONEYVER100( const char* szMoneyChar );
std::string ENCRYPTMONEYVER100(const LONGLONG lnGameMoney);
// 2009년 10월 이후의 암화화 방식
LONGLONG DECRYPTMONEYVER101( const char* szMoneyChar );
std::string ENCRYPTMONEYVER101(const LONGLONG lnGameMoney);

LONGLONG DECRYPTMONEY( const char* szMoneyChar, const int ver = GAMEMONEY_VER );
std::string ENCRYPTMONEY(const LONGLONG lnGameMoney, const int ver = GAMEMONEY_VER);


/******************************************************************************
* GameGuard 에러 코드 체크 함수
* \param dwErrCode nProtect GameGuard 에러코드
*/
std::string GetGameGuardErrorString(DWORD dwErrCode);

#endif // S_NETGLOBAL_H_

//                             ,|     
//                           //|                              ,|
//                         //,/                             -~ |
//                       // / |                         _-~   /  ,
//                     /'/ / /                       _-~   _/_-~ |
//                    ( ( / /'                   _ -~     _-~ ,/'
//                     \~\/'/|             __--~~__--\ _-~  _/,
//             ,,)))))));, \/~-_     __--~~  --~~  __/~  _-~ /
//          __))))))))))))));,>/\   /        __--~~  \-~~ _-~
//         -\(((((''''(((((((( >~\/     --~~   __--~' _-~ ~|
//--==//////((''  .     `)))))), /     ___---~~  ~~\~~__--~ 
//        ))| @    ;-.     (((((/           __--~~~'~~/
//        ( `|    /  )      )))/      ~~~~~__\__---~~__--~~--_
//           |   |   |       (/      ---~~~/__-----~~  ,;::'  \         ,
//           o_);   ;        /      ----~~/           \,-~~~\  |       /|
//                 ;        (      ---~~/         `:::|      |;|      < >
//                |   _      `----~~~~'      /      `:|       \;\_____// 
//          ______/\/~    |                 /        /         ~------~
//        /~;;.____/;;'  /          ___----(   `;;;/               
//       / //  _;______;'------~~~~~    |;;/\    /          
//      //  | |                        /  |  \;;,\              
//     (<_  | ;                      /',/-----'  _>
//      \_| ||_                     //~;~~~~~~~~~ 
//          `\_|                   (,~~ 
//                                  \~\ 
//                                   ~~ 
// Fast Fast More Fast~! 
// Fly To The Moon~! 
// Fly Fly~!
// Jgkim
