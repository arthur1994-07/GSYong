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
	#pragma comment(lib, "../=nProtect/ggsrvlib25_32_MD.lib") // ������Ʈ-�Ӽ�-��Ŀ-�Է�-�߰����Ӽ�����  "$(SolutionDir)=nProtect/ggsrvlib25_32_MD.lib"  �����ϰ� �ҽ����� �߰��ϵ��� ����
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
	NET_MSG_VERSION_OK			= 100, //! ������ ��ġ��
	NET_MSG_CLIENT_VERSION_CS	= 110, //! ���� ������ ����
    NET_MSG_VERSION_LS          = 111, //! Login->Session:���� ������ ����
    NET_MSG_VERSION_SS          = 112, //! Session->Server:���� ������ ����
    NET_MSG_CLIENT_VERSION_SC	= 113, //! Login->Client ���� ������ ����
    NET_MSG_CLIENT_VERSION_AC   = 114, //! Agent->Client ���� ������ ����
	NET_MSG_VERSION_REQ			= 120, //! ���� ������ ��û��
	NET_MSG_REQ_CRYT_KEY		= 130, //! Ŭ���̾�Ʈ->���Ӽ��� : ��ȣŰ ��û

    NET_MSG_CS_AES_REQ_SERVER_PUB_KEY = 131, //! Aes

	NET_MSG_SND_CRYT_KEY		= 140, //! ���Ӽ���->Ŭ���̾�Ʈ : ��ȣŰ ����	
	NET_MSG_RANDOM_NUM          = 141, //! Agent->Client : �����ѹ�
//	NET_MSG_PING_REQUEST		= 150, //! �������� ping �� ��û�Ѵ�.
//	NET_MSG_PING_ANSWER		    = 151, //! �������� ping �� �����Ѵ�.
	
    NET_MSG_HEARTBEAT_SERVER_REQ= 153, //! SERVER->SERVER Server HeartBeat Request
    NET_MSG_HEARTBEAT_SERVER_ANS= 154, //! SERVER->SERVER Server HeartBeat Answer
    NET_MSG_SVR_DOWN            = 155, //! Server System Down!
	NET_MSG_SVR_CLOSECLIENT     = 156, //! Server Close Client	
	NET_MSG_HEARTBEAT_CLIENT_REQ= 160, //! SERVER->CLIENT Client HeartBeat Request
	NET_MSG_HEARTBEAT_CLIENT_ANS= 161, //! CLIENT->SERVER Client HeartBeat Answer
	NET_MSG_COMPRESS            = 170, //! Compressed message	
    
	///////////////////////////////////////////////////////////////////////////	
	// Server info message

	//================================== ������ ==================================
	NET_MSG_LGIN				= 171, //! 171 
	//============================================================================
	
	NET_MSG_REQ_GAME_SVR		= 172, //! Ŭ���̾�Ʈ�� -> �α��μ��� : ���� ������ ������ ��û
	NET_MSG_SND_GAME_SVR		= 173, //! �α��μ���   -> Ŭ���̾�Ʈ : ���� ������ ������ ����
	NET_MSG_SND_GAME_SVR_END	= 174, //! �α��μ���   -> Ŭ���̾�Ʈ : ���� ���� ���� ���� ��
	NET_MSG_REQ_FULL_SVR_INFO	= 175, //! ���Ǽ��� -> �α��μ���, ���Ӽ��� : ���� ��ü ������ ��û�Ѵ�.
	NET_MSG_SND_FULL_SVR_INFO	= 176, //! �α��μ���, ���Ӽ��� -> ���Ǽ��� : ���� ��ü ������ Session ������ �����Ѵ�.
	NET_MSG_REQ_CUR_STATE		= 177, //! ���Ǽ��� -> �α��μ���, ���Ӽ��� : ���� ��������= (������),�� ��û�Ѵ�.
	NET_MSG_SND_CUR_STATE		= 178, //! �α��μ���, ���Ӽ��� -> ���Ǽ��� : ���� ��������= (������),�� �����Ѵ�.
	NET_MSG_SND_CHANNEL_STATE	= 179, //! Agent->Session : ���� ä�λ��¸� �����Ѵ�.
	NET_MSG_REQ_SVR_INFO		= 180, //! �α��μ��� -> ���Ǽ��� : ���Ӽ����� ���������� ��û�Ѵ�.
	NET_MSG_SND_SVR_INFO		= 181, //! ���Ǽ��� -> �α��μ��� : ���Ӽ����� ���������� �����Ѵ�.
	NET_MSG_SND_SVR_INFO_RESET	= 182, //! ���Ǽ��� -> �α��μ��� : ���Ӽ����� ���������� �����϶�� ���.
	NET_MSG_REQ_ALL_SVR_INFO	= 183, //! �������α׷�->���Ǽ��� : ��� ������ ���������� ��û�Ѵ�.
	NET_MSG_SND_ALL_SVR_INFO	= 184, //! ���Ǽ���->�������α׷� : ��� ������ ���������� �����Ѵ�.
	NET_MSG_REQ_AUTH_SVR_INFO	= 185, 
	NET_MSG_SND_AUTH_SVR_INFO	= 186,
	NET_MSG_SND_ALL_SVR_INFO_S	= 187, //! ���Ǽ���->�������α׷� : ������������ ����
	NET_MSG_SND_ALL_SVR_INFO_E	= 188, //! ���Ǽ���->�������α׷� : ���������� ��� �������� �˸���.
	NET_MSG_RECONNECT_FIELD		= 189, //! ���Ǽ���->������Ʈ : �ʵ�� �ٽ� �����϶�.
	NET_MSG_I_AM_AGENT			= 190, //! Agent->(Field or Match): �ڽ��� ������ſ� �������� �˸�	
	NET_MSG_NEW_INSTANCE		= 191, //! Instance->Match: �ν��Ͻ� ���� ���
	NET_MSG_RECONNECT_MATCH		= 192, //! ���Ǽ���->������Ʈ : Match�� �ٽ� �����϶�.

	NET_MSG_LOGOUT_G_S			= 193, //! Agent->Session : �α׾ƿ� ����	
	NET_MSG_LOGOUT_G_S_S		= 194, //! Agent->Session : �α׾ƿ� ����. ����� ID �� ����
	DAUM_NET_MSG_LOGOUT_G_S		= 195, //! Agent->Session : Daum �α׾ƿ� ����

	//! User Information message
	NET_MSG_REQ_USER_INFO		= 196, //! ����� ������ ��û�Ѵ�.
	NET_MSG_USER_INFO			= 197, //! ����� ������ �����Ѵ�.

	//! Server control message	
	NET_MSG_SVR_CMD				= 198,
	//NET_MSG_SVR_PAUSE, //! ������ ��� �����.
	//NET_MSG_SVR_RESUME, //! ���߾��� ������ �ٽ� ������Ų��.	
	NET_MSG_SVR_RESTART			= 199, //! ������ ������ ���߰� ���Ӱ� ������Ų��.
	NET_MSG_SVR_STOP			= 200, //! ������ ������ �����.
	NET_MSG_SND_CHANNEL_FULL_STATE	= 201, //! ������ ä�� FULL ���¸� LoginServer�� ������.

    NET_MSG_CLOSE_CLIENT_SESSION	= 202, //! Session:Client socket closed

	
	// ============ �������� �޼��� ������ �Ʒ��� �������� ��ȣ�� �ݵ�� ���� ================
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
	// �߰��Ǵ� �������ݵ��� �ݵ�� �Ʒ� ������ ���ķ� �߰��ؾ���
	//////////////////////////////////////////////////////////////////////////////////////////
	//================================== ������ ==================================
	//
	NET_MSG_LGIN_MAX	= 213,
	NET_MSG_LOBBY		= 214,
	
	//============================================================================
	// ��Ŷ Enum ���� ���� ������� ���ؼ� �����Ŵ������� ����ϴ� ��Ŷ�߿��� ���Ӱ��� ��Ŷ�� �������� �߰� ( ������Ŷ�� ���Ӽ������� ����Ҽ� �ֱ⶧���� �״�� ���� ) - 2014.12.18
	NET_MSG_INSPECTION_REWARD_SET_REQ				= 215,	//�Ⱓ�� ������ �ð� ����
	NET_MSG_INSPECTION_REWARD_SET_ACK				= 216,
	NET_MSG_CHAT_CTRL_SVRMANAGER					= 217,
	NET_MSG_EVENT_EXP_MS							= 218,
	NET_MSG_EVENT_EXP_END_MS						= 219,
	NET_MSG_UPDATE_TRACING_ALL						= 220,
	NET_MSG_UPDATE_TRACING_CHARACTER				= 221,
	

	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ���ݺ��� �Ʒ��� �߰��Ǵ� ��Ŷ�� Enum ���� ���帶�� �������� �����	!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//!-----------------------------------------------------------------------------------------------------[LOBBY]	
	// NET_MSG_LOGIN, //! �α��� 
	NET_MSG_LOGIN_2, //! �α���
	NET_MSG_LOGIN_FB, //! �α��� ���	
    NET_MSG_LOGIN_FEEDBACK_DATA2, //! 
	NET_MSG_LOGIN_FB_SUB_OK_FIELD, //! �α��μ�������� �ʵ弭���� �����ؼ� �����ϰ� �Ѵ�.		

	
	DAUM_NET_MSG_LOGIN, //! Daum �α���
	DAUM_NET_MSG_LOGIN_FB, //! Daum �α��� ��� Agent->Client	

	MET_MSG_SVR_FULL, //! ���� Full

	CHINA_NET_MSG_LOGIN, //! China �α���
	CHINA_NET_MSG_LOGIN_FB, //! China �α��� ��� Agent->Client

	GSP_NET_MSG_LOGIN, //! GSP �α���
	GSP_NET_MSG_LOGIN_FB, //! GSP �α��� ��� AGent->Client

	IDN_NET_MSG_LOGIN, //mjeon.indonesia
	IDN_NET_MSG_LOGIN_FB,

	TERRA_NET_MSG_LOGIN, //! Terra �α���
	TERRA_NET_MSG_LOGIN_FB, //! Terra �α��� ��� Agent->Client

	TERRA_NET_MSG_PASSCHECK, //! Terra Password Check
	DAUM_NET_MSG_PASSCHECK, //! Daum Password Check
	NET_MSG_PASSCHECK_FB, //! Password Check ��� Agent->Client
	EXCITE_NET_MSG_PASSCHECK, //! Excite Password Check

	EXCITE_NET_MSG_LOGIN, //! Excite �α���
	EXCITE_NET_MSG_LOGIN_FB, //! Excite �α��� ��� Agent->Client

	JAPAN_NET_MSG_LOGIN, //! �Ϻ�(Gonzo) �α���
	JAPAN_NET_MSG_LOGIN_FB, //! �Ϻ�(Gonzo) �α��� ��� Agent->Client
	JAPAN_NET_MSG_UUID, //! �Ϻ�(Gonzo) �α��� UUID

	GS_NET_MSG_WB_LOGIN, // GS �α������� WB ���� ���θ� Ȯ���ϱ� ���ؼ� ����
	GS_NET_MSG_WB_LOGIN_AW, // GS �α������� WB ���� ���θ� Ȯ���ϱ� ���ؼ� ����
	GS_NET_MSG_WB_LOGIN_WA, // GS �α������� WB ���� ���θ� Ȯ���ϱ� ���ؼ� ����
	GS_NET_MSG_WB_LOGIN_FB, //! �α��� ���	
	GS_NET_MSG_LOGIN, //! GS �α���
	GS_NET_MSG_LOGIN_FB, //! GS �α��� ��� Agent->Client


	THAI_NET_MSG_LOGIN, //! Thailand �α���
	THAI_NET_MSG_LOGIN_FB, //! Thailand �α��� ��� Agent->Client

	GM_NET_MSG_LOGIN, //! GM �α���
	GM_NET_MSG_LOGIN_FB, //! GM �α��� ��� Agent->Client

	US_NET_MSG_LOGIN, //! US �α���
	US_NET_MSG_LOGIN_FB, //! US �α��� ��� Agent->Client

	NET_MSG_REQ_RAND_KEY, //! ����Ű ��û Client->Agent
	
	NET_MSG_SND_ENCRYPT_KEY, //! �α��� ����Ű ���� Login->Session

    NET_MSG_CLOSE_CLIENT, //! Ŭ���̾�Ʈ ���� ���� �����Ŵ;

	//! Character create message
	//NET_MSG_CHA_INFO, //! ĳ���� ����
	NET_MSG_CHA_NEW, //! ���ο� ĳ���� ����
	NET_MSG_CHA_NEW_FB, //! ĳ���� ���� ����/���� �޽���
	
	NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	NET_MSG_CHA_DEL_FB, //! Agent->Client : �ɸ��� ���� ���
	NET_MSG_CHA_DEL_FB_OK, //! Agent->Client : �ɸ��� ���� ����
	NET_MSG_CHA_DEL_FB_ERROR, //! Agent->Client : �ɸ��� ���� ����
	DAUM_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
    NET_MSG_CHA_DEL_FB_CLUB, //! Agent->Client : �ɸ��� ���� ����(Ŭ���������̱� ������ ������ ���� �ʴ´�)
	TERRA_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	EXCITE_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	GSP_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	JAPAN_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	GS_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����
	IDN_NET_MSG_CHA_DEL, //! Client->Agent : �ɸ��� ����

	NET_MSG_CHA_DECREASE, //! Agent->Session : ĳ���� ����, ���� ����
	NET_MSG_TEST_CHA_DECREASE, //! Agent->Session : ĳ���� ����, ���� ����
	DAUM_NET_MSG_CHA_DECREASE, //! DAUM : Agent->Session : ĳ���� ����, ���� ����
	DAUM_NET_MSG_TEST_CHA_DECREASE, //! DAUM : Agent->Session : ĳ���� ����, ���� ����

	NET_MSG_CHA_INCREASE, //! Agent->Session : ĳ���� ����, ���� ����
	NET_MSG_TEST_CHA_INCREASE, //! Agent->Session : ĳ���� ����, ���� ����
	DAUM_NET_MSG_CHA_INCREASE, //! DAUM : Agent->Session : ĳ���� ����, ���� ����
	DAUM_NET_MSG_TEST_CHA_INCREASE,//! DAUM : Agent->Session : ĳ���� ����, ���� ����

	//NET_MSG_REQ_CHA_ALL_INFO, //! ��� ĳ������ ������ ��û�Ѵ�.
	//NET_MSG_REQ_CHA_INFO, //! Ư�� ĳ������ ������ ��û�Ѵ�.
	NET_MSG_REQ_CHA_BINFO_CA, //! Ư�� ĳ������ �⺻= (ĳ���ͼ���ȭ��),���� ��û�Ѵ�. // 279
	//NET_MSG_REQ_CHA_FEEDBACK, //! ĳ���� ���� �ǵ�鵥����
	//NET_MSG_CHA_INFO_LOGIN, //! �α��μ������� Ŭ���̾�Ʈ���� �Ѱ��ִ� ĳ���� ���� 
	NET_MSG_REQ_CHA_BAINFO, //! Ŭ���̾�Ʈ->���Ӽ��� : ĳ���� �������� ��û // 280
	NET_MSG_CHA_BAINFO_AC, //! Agent->Client : ����� ĳ���� DB ��ȣ ���� // 281
    NET_MSG_CHA_BAINFO_DH, //! Database->Cache : ����� ĳ���� DB ��ȣ ����
    NET_MSG_CHA_BAINFO_HA, //! Cache->Agent : ����� ĳ���� DB ��ȣ ����
	NET_MSG_CHA_BAINFO_DA, //! DB->Agent : ������ ��ȯ�� ���ؼ� ��� �޽��� �����̸� �Ѵ�.
    NET_MSG_LOBBY_CHAR_INFO_AH, //! Agent->Cache : ĳ���� Lobby Display �� ���� ��û
	NET_MSG_LOBBY_CHAR_INFO_AH_END, 
    NET_MSG_LOBBY_CHAR_INFO_DH, //! Database->Cache : ĳ���� Lobby Display �� ����
    NET_MSG_LOBBY_CHAR_INFO_HA, //! Cache->Agent : ĳ���� Lobby Display �� ����
	NET_MSG_LOBBY_CHAR_INFO_HA_END, //! Cache->Agent : ĳ���� Lobby Display �� ����
    NET_MSG_LOBBY_USER_INFO_DH, //! Database->Cache : ���� Lobby Display �� ����
    NET_MSG_LOBBY_CHAR_INFO_DH2, //! Database->Cache : ĳ���� Lobby Display �� ����
    NET_MSG_LOBBY_USER_INFO_END_DH, //! Database->Cache : ���� Lobby Display �� ĳ���� ���� ��
    NET_MSG_LOBBY_NEW_CHAR_INFO_DH, //! Database->Cache : ĳ���� Lobby Display �� ����
	

	NET_MSG_LOBBY_CHARINFO_AC,  //! (AC) ĳ���� ���ÿ� ����. // 291
	NET_MSG_LOBBY_CHARINFO_AC_END,  //! (AC) ĳ���� ���ÿ� ����. // 291
	NET_MSG_LOBBY_CHAR_JOIN,    //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ���� ������ ����.
	NET_MSG_LOBBY_CHAR_ITEM,    //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ���� �κ��丮 �������� ����.
	NET_MSG_LOBBY_CHAR_DEFAULT, //! �ʵ弭��->Ŭ���̾�Ʈ : ������ ����.
	NET_MSG_LOBBY_CHAR_JOIN_FB, //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ������ �������� ���� �޽���.
	NET_MSG_LOBBY_CHAR_SKILL,   //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ���Ͱ� ��� ��ų ���� ����.
	NET_MSG_LOBBY_QUEST_END,    //! ����Ʈ �Ϸ� ����.
	NET_MSG_LOBBY_QUEST_PROG,   //! ����Ʈ ���� ����.
	NET_MSG_LOBBY_CHARGE_ITEM,  //! ���� ������ ����.
	//NET_MSG_LOBBY_CHAR_PUTON, //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ���� ���� �������� ����.
	
	NET_MSG_CHARPOS_FROMDB2AGT, //!	ĳ���� ��ġ DB ���� ���
	
	NET_MSG_LOBBY_ITEM_COOLTIME, //! ������ ��Ÿ��

	NET_MSG_LOBBY_INSTANCE_LIST, //! �δ� ����Ʈ;
	
	NET_MSG_LOBBY_CHAR_JOIN_CA,    //! �κ񿡼� ������ ĳ���ͷ� ��������	// 303
	NET_MSG_LOBBY_GAME_COMPLETE,   //! Ŭ���̾�Ʈ->Ŭ���̾�Ʈ : ���ӿ� �� ������ ��� �޾Ƽ� ���ӿ� ��.
	NET_MSG_GAME_JOIN_OK,          //! �ʵ弭��    ->���Ǽ��� : �ش� ĳ���Ͱ� ���μ���
	MET_MSG_GAME_JOIN_FIELDSVR,    //! ������Ʈ����->�ʵ弭�� : �ش�ĳ���Ͱ� ���ӿ� ���ο�û
	MET_MSG_GAME_JOIN_FIELDSVR_FB, //! �ʵ弭��->������Ʈ���� : ���õ� ĳ���ͷ� ������	
	MET_MSG_JOIN_INSTANCESVR_INFO,

	NET_MSG_CONNECT_CLIENT_FIELD, //! ������Ʈ����->Ŭ���̾�Ʈ   : �ش��ʵ弭���� ���� ���
	NET_MSG_JOIN_FIELD_IDENTITY,  //! Ŭ���̾�Ʈ  ->�ʵ弭��     : ���ο� Field ������ ������ �ڽ��� �ν��� ����.

	NET_MSG_LOBBY_REQ_GAME_JOIN,  //! Ŭ���̾�Ʈ  ->Ŭ���̾�Ʈ   : �������̽��� ���õ� �ɸ����� ������ ����ϱ� ���ؼ�.
	
	NET_MSG_AGENT_REQ_JOIN, //! ������ƮDB ->������Ʈ   : db���� ���� char�� ���ӽõ�.
	NET_MSG_FIELD_REQ_JOIN, //! �ʵ�DB		->�ʵ弭��   : db���� ���� char�� ���ӽõ�.

	//NET_MSG_EVENT_LOTTERY, //! Ŭ���̾�Ʈ  ->������Ʈ   : �����Է�
	//NET_MSG_EVENT_LOTTERY_FB, //! ������Ʈ    ->Ŭ���̾�Ʈ : ���ǰ��

	NET_MSG_LOBBY_CHARJOIN_2NDPASS, // 2����� // ĳ���� ���ý� 2������� ���� �����µ� ����� ���� �Ǵ� �����ΰ��
	NET_IN_MSG_CHAJOIN_2NDPASSWORD, // 2�����


	NET_MSG_LOBBY_CHINA_ERROR, //! �߱� �α��ν� �÷��� �����ð��� 5�ð��� �Ѱ� �������� �����ð��� 5�ð��� ���� �ʾ��� ��� ���� �޽���

	NET_MSG_LOBBY_CHAR_VIETNAM_ITEM, //! ��Ʈ�� �κ��丮 ������ ���� �޽���

	NET_MSG_LOBBY_CHAR_PUTON_EX, //! �ʵ弭��->Ŭ���̾�Ʈ : ���õ� ĳ���� ���� �������� ����. ( �Ѱ��� ���� )

    NET_MSG_LOGIN_WB_CA, //! World battle login

	// User System Info
	NET_MSG_USER_SYSTEM_INFO_CA,					// �������� System ������ �����Ѵ�. // 323

	/// Country - ��Ŷ ����͸��� ���Ͽ� ��� �Ѱ��� ����;
	NET_MSG_GCTRL_COUNTRY_CHANGE_CA,
	NET_MSG_GCTRL_COUNTRY_CHANGE_AFC,
	/// ---------------------------------------------------------------- ///

	//================================== ������ ==================================
	NET_MSG_LOBBY_MAX,
	NET_MSG_GCTRL,
	//============================================================================

	//!-----------------------------------------------------------------------------------------------------[GAME CONTRL]
	
	NET_MSG_COMBINE,  //! ���� �޽���. ( �ʵ� ���� -> Ŭ���̾�Ʈ )
    NET_MSG_JSON,  //! Json message

	///////////////////////////////////////////////////////////////////////////
	// nProtect
	NET_MSG_GAMEGUARD_AUTH, //! nProtect GameGuard Auth ��û �޽���
	NET_MSG_GAMEGUARD_ANSWER, //! nProtect GameGuard Auth Answer �޽���
	NET_MSG_GAMEGUARD_AUTH_1, //! nProtect GameGuard Auth ��û �޽���
	NET_MSG_GAMEGUARD_ANSWER_1, //! nProtect GameGuard Auth Answer �޽���
	NET_MSG_GAMEGUARD_AUTH_2, //! nProtect GameGuard Auth ��û �޽���
	NET_MSG_GAMEGUARD_ANSWER_2, //! nProtect GameGuard Auth Answer �޽���

    NET_MSG_KOR_SHUTDOWN_WARNING_AC, //! �ѱ� �˴ٿ� Shutdown

	// Apex ����
	NET_MSG_APEX_DATA, //! Apex Data �޼���
	NET_MSG_APEX_ANSWER, //! Apex Answer �޼���
//	NET_MSG_APEX_RSAKEY, //! Apex RSA Key �޼���
	// Apex ����( ȫ�� )
	NET_MSG_APEX_RETURN, //! Apex Client Return 	

	NET_MSG_REQ_FRIENDLIST,     //! ģ�� ��� ���� ��û.
	NET_MSG_REQ_FRIENDADD,      //! ģ�� �߰� ��û.
	NET_MSG_REQ_FRIENDADD_LURE, //! ģ�� �߰� ����.
	NET_MSG_REQ_FRIENDADD_ANS,  //! ģ�� �߰� ���� ���� ȸ��.
	NET_MSG_REQ_FRIENDADD_FB,   //! ģ�� �߰� ��û ���.
	NET_MSG_REQ_FRIENDDEL,      //! ģ�� ���� ��û.
	NET_MSG_REQ_FRIENDDEL_FB,   //! ģ�� ���� ���.

	NET_MSG_FRIENDINFO, //! ģ�� ����.
	NET_MSG_FRIENDSTATE, //! ģ�� ����.

	NET_MSG_REQ_FRIENDBLOCK, //! �ӼӸ� �� ��û.
	NET_MSG_REQ_FRIENDBLOCK_FB, //! �ӼӸ� �� ��û ���.

	NET_MSG_CHAT_CTRL, //! ä�ø޽��� ���� ( Ŭ���̾�Ʈ->���Ӽ��� )

//	NET_MSG_CHAT, //! ä�ø޽��� ���� ( Ŭ���̾�Ʈ->���Ӽ��� )
//	NET_MSG_CHAT_FB, //! ä�ø޽��� ���� ( ���Ӽ���->Ŭ���̾�Ʈ )

	NET_MSG_CHAT_PRIVATE_FAIL, //! ä��, �ӼӸ� �����⿡ �����Ͽ��� ���. ( ���Ӽ���->Ŭ���̾�Ʈ )	

	NET_MSG_CHAT_LOUDSPEAKER, //! Ȯ���� �޽ý�.
	NET_MSG_CHAT_LOUDSPEAKER_AGT, //! Ȯ���� �޽��� agt �� ����.
	NET_MSG_CHAT_LOUDSPEAKER_FB, //! Ȯ���� �޽��� ó�� FB.

	NET_MSG_CHAT_BLOCK, //! ä�� ���� ������ ���. ( ���Ӽ��� -> Ŭ���̾�Ʈ )

	NET_MSG_CHAT_LINK, //! ä�ø޽��� ���� ( Ŭ���̾�Ʈ->�ʵ弭�� )
	NET_MSG_CHAT_LINK_FB, //! ä�ø޽��� ���� ( �ʵ弭��->Ŭ���̾�Ʈ )

	NET_MSG_FIELDSVR_CHARCHK, //! �ʵ弭���� �ɸ��� �������� ���� CHAR ���� ���� �޽��� ����.
	NET_MSG_FIELDSVR_CHARCHK_FB, //! �ʵ弭���� �ɸ��� �������� ���� CHAR ���� ���� �޽��� ����.

	NET_MSG_MOVEMAP_AF, //! �ʵ弭������ ������ ���� ���� ó���� ��Ź. ( �ʵ弭�� �̵���. )
	NET_MSG_FIELDSVR_OUT_FB, //! �ʵ弭������ ������ ���� ���� ó�� ���. ( �ʵ弭�� �̵���. )

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

	NET_MSG_GCTRL_ACTION_BRD,	//!	�ൿ ��ȭ.
	NET_MSG_GCTRL_UPDATE_STATE,	//!	���� ��ȭ ������Ʈ.
	NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE,	//! ��� ��ȯ�� GaeaID ������Ʈ;
	NET_MSG_GCTRL_UPDATE_EXP,	//!	����ġ ��ȭ�� �߻�.
	NET_MSG_CHAR_UPDATE_MONEY_FC,	//!	����ġ ��ȭ�� �߻�.

	NET_MSG_GCTRL_UPDATE_SP,	//!	SP ��ȭ�� �߻�.
	NET_MSG_GCTRL_UPDATE_LP,	//!	Living Point ��ȭ�� �߻�.
	NET_MSG_GCTRL_UPDATE_SKP,	//!	��ų ����Ʈ ��ȭ�� �߻�.

	NET_MSG_GCTRL_UPDATE_STATE_BRD,	//!	���� ��ȭ ������Ʈ.
	NET_MSG_GCTRL_UPDATE_PASSIVE_BRD,	//!	passive skill ���� ��ȭ ������Ʈ.

	NET_MSG_GCTRL_UPDATE_BRIGHT,	//!	bright(�Ӽ�)ġ ������Ʈ.
	NET_MSG_GCTRL_UPDATE_BRIGHT_BRD,	//!	bright(�Ӽ�)ġ ������Ʈ.
	NET_MSG_GCTRL_UPDATE_STATS,	//!	stats ������Ʈ.

	NET_MSG_GCTRL_UPDATE_FLAGS,	//!	flags ������Ʈ.	

	NET_MSG_GCTRL_PUSHPULL_BRD,	//!	�аų� ���� �׼� �߻���.
	NET_MSG_GCTRL_JUMP_POS_BRD,	//!	�ɸ����� ��ġ�� ���������� ����Ǿ��� ���. Ŭ���̾�Ʈ�� �ݿ�.

	NET_MSG_GCTRL_DOMINATE_MOVETO_CF,	// ���� ��� �̵� ��û.
	NET_MSG_GCTRL_DOMINATE_MOVETO_CF_FB,	// ���� ��� �̵� ��û ����.
	NET_MSG_GCTRL_DOMINATE_MOVETO_FC,	// ���� ��� �̵�.	

	NET_MSG_GCTRL_SELECT_TARGET,	//!	����Ÿ�ټ���.

	NET_MSG_GCTRL_INVEN_INSERT,	//!	�ι꿡 ������ ���Ե�.
	NET_MSG_GCTRL_INVEN_DELETE,	//!	�ι�   ������ ����.
	NET_MSG_GCTRL_INVEN_DEL_INSERT,	//!	�ι꿡 �ִ� ������ ������ ������ ����.

	NET_MSG_GCTRL_PUTON_RELEASE,	//!	������ �������� ������.
	NET_MSG_GCTRL_PUTON_RELEASE_BRD,	//!	������ �������� ������.
	NET_MSG_GCTRL_PUTON_UPDATE,	//!	���Կ� ������ ����.
	NET_MSG_GCTRL_PUTON_UPDATE_BRD,	//!	[���] ��� �������� �����.

	NET_MSG_GCTRL_INVEN_DRUG_UPDATE,	//!	�Ҹ��� ������ �Ҹ���� ������Ʈ.
	NET_MSG_GCTRL_PUTON_DRUG_UPDATE,	//!	�Ҹ��� ������ �Ҹ���� ������Ʈ.

	NET_MSG_GCTRL_INVEN_ITEM_UPDATE,	//!	�ι� ������ ��ġ ������Ʈ.

	NET_MSG_GCTRL_PICKUP_MONEY,	//!	�� ����.
	NET_MSG_GCTRL_PICKUP_ITEM,	//!	������ ���� �˸�.

	NET_MSG_REFER_CHAR_PUBLIC,	// ĳ���� �������� ����/�����
	NET_MSG_REFER_CHAR_REQ,	// ĳ���� �������� ��û
	NET_MSG_REFER_CHAR_FB,	// ĳ���� �������� ��û ����
	NET_MSG_REFER_CHAR_PUTON,	// ĳ���� �������� ���� ������
	NET_MSG_REFER_CHAR_SKILL,	// ĳ���� �������� ��ų
	NET_MSG_REFER_CHAR_INFO,	// ĳ���� �������� �ɷ�ġ

	NET_MSG_GCTRL_2_FRIEND_REQ,	//!	ģ���� ���� ��û.
	NET_MSG_GCTRL_2_FRIEND_CK,	//!	ģ���� ���� ģ���� �ִ��� �˻��� ��ġ ��ȯ.
	NET_MSG_GCTRL_2_FRIEND_AG,	//!	ģ���� ���� AG�� ȸ�� ( ģ�� ��ġ ���� ).
	NET_MSG_GCTRL_2_FRIEND_FLD,	//!	ģ���� ���� FLD�� �̵� ���.
	NET_MSG_GCTRL_2_FRIEND_FB,	//!	ģ���� ���� ó�� ��� �뺸.

	NET_MSG_GCTRL_REQ_TAKE_FB,	//!	�ݱ� �õ� ���.
	NET_MSG_GCTRL_REQ_FIELD_TO_INVEN,	//!	������,�� �ݱ� �õ�.
	NET_MSG_GCTRL_REQ_FIELD_TO_HOLD,	//!	Field�� �ִ� �������� ��� �õ�.

	NET_MSG_GCTRL_REQ_INVEN_TO_HOLD,	//!	�ι꿡 �ִ� �������� ��� �õ�.
	NET_MSG_GCTRL_REQ_INVEN_EX_INVEN,	//!	�ι꿡 �ִ� �����۰� �κ� ������ ��ȯ.
    NET_MSG_GCTRL_REQ_STORAGE_EX_INVEN,	//!	�ι꿡 �ִ� �����۰� â�� ������ ��ȯ.
    NET_MSG_GCTRL_REQ_SLOT_EX_INVEN,	//!	�ι꿡 �ִ� �����۰� ���� ������ ��ȯ.

	NET_MSG_GCTRL_REQ_SLOT_TO_HOLD,	//!	������, ���Կ��� ������.

	NET_MSG_GCTRL_REQ_INVEN_TO_FIELD,	//!	�������� �ٴڿ� ����Ʈ��.
    NET_MSG_GCTRL_REQ_HOLD_TO_INVEN,	//!	�κ����� �κ����� ������ �ű�.
	NET_MSG_GCTRL_REQ_INVEN_TO_INVEN,	//!	�κ����� �κ����� ������ �ű�.
    NET_MSG_GCTRL_REQ_STORAGE_TO_INVEN,	//!	â���� �κ����� ������ �ű�.
	NET_MSG_GCTRL_REQ_HOLD_TO_SLOT,	//!	������, �տ��� ��������.

	NET_MSG_GCTRL_REQ_HOLD_FB,	//!	�ι꿡 �ִ� �������� ��� �õ��� ��� �ǵ��.
	NET_MSG_GCTRL_REQ_INVEN_TO_SLOT,	//!	�ι꿡 �ִ� �������� ���Կ� �ֱ�.

	NET_MSG_GCTRL_REQ_MONEY_TO_FIELD,	//!	���� �ʵ忡 ������.
	NET_MSG_GCTRL_REQ_INVEN_SPLIT,	//!	�κ��� �ִ� ��ħ ������ �и�.

	NET_MSG_GCTRL_PUTON_CHANGE,	//!	���� ����.
	NET_MSG_GCTRL_PUTON_CHANGE_BRD,	//!	[���] ���� ���� �˸�.
	NET_MSG_GCTRL_PUTON_CHANGE_AG,	//!	Agent�� ���� ���� �˸�.
	NET_MSG_GCTRL_REQ_SLOT_CHANGE,	//!	���� ���� ü����.

    NET_MSG_GCTRL_REQ_SLOT_TO_INVEN,	//!	���Կ� �ִ� �������� �κ�����

	NET_MSG_GCTRL_REQ_BUY_FROM_NPC,	//!	NPC���� ��ǰ �Ǹ� �õ�.
	NET_MSG_GCTRL_REQ_SALE_TO_NPC,	//!	NPC���� ��ǰ �Ǹ� �õ�.

    NET_MSG_GCTRL_RESURRECT_STATE,  // ��Ȱ ����;
    NET_MSG_GCTRL_RESURRECT_FAFC,  // ��Ȱ �뺸;
    NET_MSG_GCTRL_REQ_RESURRECT, // ��Ȱ ��û;
    NET_MSG_GCTRL_REQ_RESURRECT_FB,  // ��Ȱ ��û�� ���� ����;
    NET_MSG_RESURRECT_MOVEMAP_FA, // ��Ȱ�� ���� �ʵ弭�� �̵�;
    NET_MSG_RESURRECT_MOVEMAP_AF, // ��Ȱ�� ���� �ʵ弭�� �̵�;
    NET_MSG_RESURRECT_MOVEMAP_FB, // ��Ȱ�� ���� �ʵ弭�� �Ƶ�;

	NET_MSG_GCTRL_REQ_REBIRTH,	//!	��Ȱ ��û.
	NET_MSG_GCTRL_REQ_REBIRTH_FB,	//!	��Ȱ ��û ����.

    NET_MSG_CHAR_LEVEL_UP_FA,	//!	Field->Agent Character level up
	NET_MSG_GCTRL_REQ_LEVELUP,	//!	���� ��û.
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP,
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_CA,
	NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_FB,
	NET_MSG_GCTRL_REQ_JUMPING_LEVLEUP_BYEXP,

	NET_MSG_GCTRL_REQ_LEVELUP_FB,	//!	���� ��û ����.
	NET_MSG_GCTRL_REQ_LEVELUP_BRD,	//!	���� ȿ��.

	NET_MSG_GCTRL_REQ_STATSUP,	//!	STATS UP
	NET_MSG_GCTRL_REQ_STATSUP_FB,	//!	STATS UP ����.

	NET_MSG_GCTRL_REQ_STATSUP_EX,	//!	STATS_EX UP
	NET_MSG_GCTRL_REQ_STATSUP_EX_FB,	//!	STATS_EX UP ����.

    
	NET_MSG_GCTRL_REQ_INVENDRUG,	//!	�ι� ��ǰ �Ա�.
	NET_MSG_GCTRL_REQ_LEARNSKILL,	//!	�ι� ��ų ���� ��û.
	NET_MSG_GCTRL_REQ_LEARNSKILL_FB,	//!	�ι� ��ų ���� ����.
	NET_MSG_GCTRL_REQ_SKILLUP,	//!	��ų ���׷��̵� ��û.
    NET_MSG_GCTRL_REQ_RNSKILLUP,    //! ���ο� ��ų ������(����) : 2012-05-09 cwback
	NET_MSG_GCTRL_REQ_SKILLUP_FB,	//!	��ų ���׷��̵� ����.
	NET_MSG_GCTRL_REQ_RNSKILLUP_COMPLETE,	//!	���ο� ��ų ������ �Ϸ�

	NET_MSG_GCTRL_REQ_RECALL,	//!	��ȯ ������ ��� ��û.
	NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB,	//!	��ȯ ������ ��� ����.
	NET_MSG_GCTRL_REQ_MOVEMAP_FA,	//!	��ȯ�� ������Ʈ ������ ��û.

	NET_MSG_GCTRL_REQ_BUS,	//!	���� ���� ��û.
	NET_MSG_GCTRL_REQ_BUS_FB,	//!	���� ���� ��û ����.

	NET_MSG_GCTRL_REQ_SKILL,	//!	��ų �ߵ� ��û.
	NET_MSG_GCTRL_REQ_SKILL_FB,	//!	��ų �ߵ� ��û ����.
	NET_MSG_GCTRL_REQ_SKILL_BRD,	//!	��ų �ߵ�.
	NET_MSG_GCTRL_SKILLFACT_BRD,	//!	��ų ����� ü�� ��ȭ.
	NET_MSG_GCTRL_SKILLHOLD_BRD,	//!	��ų ����� ���� ȿ��.
	NET_MSG_GCTRL_SKILLCONSUME_FB,	//!	��ų �ߵ��� �Ҹ�Ǵ� ��ġ��.
	NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION,
	NET_MSG_GCTRL_REQ_SKILL_DEALY_ACTION_BRD,

	NET_MSG_GCTRL_SKILL_CANCEL,	//!	��ų ���.
	NET_MSG_GCTRL_SKILL_CANCEL_BRD,	//!	��ų ��� Ŭ���̾�Ʈ�� �ݿ�.

	NET_MSG_GCTRL_REQ_SKILLHOLD_RS,	//!	��ų ���� ȿ�� ���� ��û.
	NET_MSG_GCTRL_REQ_SKILLHOLD_RS_FB,	//!	��ų ���� ȿ�� ���� ��û ����.
	NET_MSG_GCTRL_SKILLHOLD_RS_BRD,	//!	��ų ���� ȿ�� ����.

	NET_MSG_GCTRL_SAVESKILLFACT_BRD,	//!	����� ��ų ���� ȿ�� INFO

	NET_MSG_GCTRL_STATEBLOW_BRD,	//!	�����̻�.
	NET_MSG_GCTRL_CURESTATEBLOW_BRD,	//!	�����̻�.
	NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD,	//! ������ ���õ� ���� �̻� ������Ʈ

	NET_MSG_GCTRL_REQ_SKILLQ_ACTIVE,	//!	��ų �������� ��Ƽ�� ��ų.
	NET_MSG_GCTRL_REQ_SKILLQ_SET,	//!	��ų ������ ���� ��û.
	NET_MSG_GCTRL_REQ_SKILLQ_RESET,	//!	��ų ������ ���� ��û.
	NET_MSG_GCTRL_REQ_SKILLQ_FB,	//!	��ų ������ ��û�� ���� ������� Ŭ���̾�Ʈ�� ����.

	NET_MSG_GCTRL_REQ_ACTIONQ_SET,	//!	���ǽ��� ���� ��û.
	NET_MSG_GCTRL_REQ_ACTIONQ_RESET,	//!	���ǽ��� ���� ��û.
	NET_MSG_GCTRL_REQ_ACTIONQ_FB,	//!	���ǽ��� ��û�� ���� ������� Ŭ���̾�Ʈ�� ����.

	//! â���� Ư�� ������ ���� ��������specific
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM,	//!	â�� Ư�� ���� ��������.
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_COUNT_FB,	//!	â�� Ư�� ���� ��������.
	NET_MSG_GCTRL_REQ_GETSTORAGE_SPECIFIC_ITEM_FB,	//!	â�� Ư�� ���� ��������.

	NET_MSG_GCTRL_REQ_GETSTORAGE,	//!	â�� ���� ��������.
	NET_MSG_GCTRL_REQ_GETSTORAGE_FB,	//!	â�� ���� ��������.
	NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM,	//!	â�� ���� ��������.

	NET_MSG_GCTRL_REQ_STORAGEDRUG,	//!	â�� ��ǰ �Ա�.
	NET_MSG_GCTRL_REQ_STORAGESKILL,	//!	â�� ��ų �Ա�.

	NET_MSG_GCTRL_REQ_STORAGE_TO_HOLD,	//!	â�� �ִ� �������� ��� �õ�.
	NET_MSG_GCTRL_REQ_INVEN_EX_STORAGE,	//!	â�� �ִ� �����ư� �κ� ������ ��ȯ.
    NET_MSG_GCTRL_REQ_STORAGE_EX_STORAGE, //! â�� �ִ� ������ ��ȯ.
	NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE,	//!	�������� â�� ����.
    NET_MSG_GCTRL_REQ_STORAGE_TO_STORAGE, //! â���� â��� ������ �ű�.
    NET_MSG_GCTRL_REQ_INVEN_TO_STORAGE_FB, //! â�� ������ �ֱ� �ǵ��.

	NET_MSG_GCTRL_STORAGE_INSERT,	//!	â�� ������ ���Ե�.
	NET_MSG_GCTRL_STORAGE_DELETE,	//!	â�� ������ ����.
	NET_MSG_GCTRL_STORAGE_DEL_INSERT,	//!	â�� ������ ���� �� ����.
	NET_MSG_GCTRL_STORAGE_DRUG_UPDATE,	//!	â�� �Ҹ�ǰ ���Ƚ�� �ľ�.

	NET_MSG_GCTRL_STORAGE_SAVE_MONEY,	//!	â�� ���� ����.
	NET_MSG_GCTRL_STORAGE_DRAW_MONEY,	//!	â���� ���� ����.
	NET_MSG_GCTRL_STORAGE_UPDATE_MONEY,	//!	â���� �� �׼� ��ȭ.

	NET_MSG_GCTRL_REQ_STORAGE_SPLIT,	//!	â�� �ִ� ��ħ ������ �и�.
	NET_MSG_GCTRL_STORAGE_STATE,	//!	â�� ��밡�� �Ӽ�.
	NET_MSG_GCTRL_PREMIUM_STATE,	//!	�����̾� �Ӽ�.

	NET_MSG_GCTRL_PARTY_LURE,	//!	��Ƽ ���� ��û.			Ŭ���̾�Ʈ -> ����.
    NET_MSG_GCTRL_PARTY_LURE_WITH_CHARNUM,	//!	��Ƽ ���� ��û.			Ŭ���̾�Ʈ -> ����.
	NET_MSG_GCTRL_PARTY_LURE_FB,	//!	��Ƽ ���� ��û�� ���.	Ŭ���̾�Ʈ -> ����.
    NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME,	//!	��Ƽ ���� ��û�� ���.	Ŭ���̾�Ʈ -> ����.
	NET_MSG_GCTRL_PARTY_LURE_TAR,	//!	��Ƽ ���� ��û.			���� -> Ŭ���̾�Ʈ.
    NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME,	//!	��Ƽ ���� ��û.			���� -> Ŭ���̾�Ʈ.
	NET_MSG_GCTRL_PARTY_LURE_TAR_ANS,	//!	��Ƽ ���� ��û ����.	Ŭ���̾�Ʈ -> ����.

	NET_MSG_GCTRL_PARTY_SECEDE,	//!	��Ƽ Ż�� ��û.

	NET_MSG_GCTRL_PARTY_ADD,	//!	��Ƽ�� �߰�.
	NET_MSG_GCTRL_PARTY_ADD_FB,	//!	��Ƽ�� �߰� FB.

	NET_MSG_GCTRL_PARTY_DEL,	//!	��Ƽ�� ����.
	NET_MSG_GCTRL_PARTY_DEL_FB,	//!	��Ƽ�� ���� FB
	NET_MSG_GCTRL_PARTY_AUTHORITY,	//! ��Ƽ�� ����
	NET_MSG_GCTRL_PARTY_DISSOLVE,	//!	��Ƽ ��ü.
	NET_MSG_GCTRL_PARTY_REFRESH_REQ, //! ��Ƽ ���� ���� ��û;
	NET_MSG_GCTRL_PARTY_ERROR, //! ��Ƽ ���� �޼��� ����;
	
	NET_MSG_GCTRL_PARTY_FNEW,	//!	��Ƽ ����. (field).
	NET_MSG_GCTRL_PARTY_MBR_DETAIL,	//!	���Կ��� ���� ����.
	NET_MSG_GCTRL_PARTY_MBR_DETAIL_PARTY, //! ���� ��Ƽ���� ���� ����;
	NET_MSG_GCTRL_PARTY_MBR_MOVEMAP,	//!	��Ƽ���� map �̵�.
	NET_MSG_GCTRL_PARTY_MBR_UPDATE, //! ��Ƽ���� ���� ��ȭ.
    NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD,	//!	��Ƽ���� ������ ������ �˸�.
    NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD,	//!	��Ƽ���� ������ ������ �˸�. (������ ��ũ)
	NET_MSG_GCTRL_PARTY_MBR_LEVELUP,	

	NET_MSG_GCTRL_PARTY_BRD,	//!	�ɸ����� ��Ƽ �Ҽ� ��ȭ�� �ֺ� Ŭ���̾�Ʈ�鿡 �˸�.

	NET_MSG_GCTRL_EXPEDITION_FNEW_CA, // ������ ����;	
	NET_MSG_GCTRL_EXPEDITION_FNEW_FB,
	NET_MSG_GCTRL_PARTY_CHANGE_OPTION, // ��Ƽ/������ �ɼ� ����;
	NET_MSG_GCTRL_EXPEDITION_DEL_AF, // ������ ����;
	NET_MSG_GCTRL_EXPEDITION_DEL_AC, // ������ ����;
	NET_MSG_GCTRL_EXPEDITION_ERROR,	 // ������ ���� ����

	NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA, // �غ� Ȯ�� ��û;
	NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA, // �غ� Ȯ�� ��û�� ���� ����;

	NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT, // �ڸ� ��ü;	

	NET_MSG_GCTRL_PARTY_ADD_TENDER_ITEM, // �ֻ��� ���� ������ ���;
	NET_MSG_GCTRL_PARTY_DELETE_TENDER_ITEM, // �ֻ��� ���� ������ ��� �ð� ����;
	NET_MSG_GCTRL_PARTY_TENDER_ITEM, // �ֻ��� ������ ����;
	NET_MSG_GCTRL_PARTY_TENDER_ITEM_FB, // �ֻ��� ������ ������ ���� ��� �˸�;
	NET_MSG_GCTRL_PARTY_JUNK_ITEM, // ������ �й�/������ ������;
	NET_MSG_GCTRL_PARTY_JUNK_ITEM_FB, // ������ �й�/������ �����⿡ ���� ��� �˸�;
	NET_MSG_GCTRL_PARTY_TRANSFER_ITEM, // ������ �й�/������ �絵;
	NET_MSG_GCTRL_PARTY_TRANSFER_ITEM_FB, // ������ �й�/������ �絵�� ���� ��� �˸�;		

	NET_MSG_GCTRL_TRADE,	//!	Ʈ���̵� ��û. ( Ŭ���̾�Ʈ1 -> ���� )
	NET_MSG_GCTRL_TRADE_FB,	//!	Ʈ���̵� ��û ����. ( ���� -> Ŭ���̾�Ʈ1 )
	NET_MSG_GCTRL_TRADE_TAR,	//!	Ʈ���̵� ��뿡�� ��û. ( ���� -> Ŭ���̾�Ʈ2 )
	NET_MSG_GCTRL_TRADE_TAR_ANS,	//!	Ʈ���̵� ����� ����. ( Ŭ���̾�Ʈ2 -> ���� )

	NET_MSG_GCTRL_TRADE_AGREE,	//!	Ʈ���̵� ���� ����.
	
	NET_MSG_GCTRL_TRADE_AGREE_TAR,	//!	Ʈ���̵� ���� ���� ���濡�� �˸�. ( ���濡��. )

	NET_MSG_GCTRL_TRADE_ITEM_REGIST,	//!	Ʈ���̵��� ������ ���.
	NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR,	//!	Ʈ���̵��� ������ ���. ( ���濡��. )
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE,	//!	Ʈ���̵��� ������ ��Ͽ��� ����.
	NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR,	//!	Ʈ���̵��� ������ ��Ͽ��� ����. ( ���濡��. )

	NET_MSG_GCTRL_TRADE_MONEY,	//!	Ʈ���̵��� �ݾ�.
	NET_MSG_GCTRL_TRADE_MONEY_TAR,	//!	Ʈ���̵��� �ݾ��� ���濡�� �˸�.

	NET_MSG_GCTRL_TRADE_COMPLETE_TAR,	//!	Ʈ���̵� �Ϸ�.

	NET_MSG_GCTRL_TRADE_CANCEL,	//!	Ʈ���̵� ���.
	NET_MSG_GCTRL_TRADE_CANCEL_TAR,	//!	Ʈ���̵� ���. ( ���濡��. )

	NET_MSG_GCTRL_INVEN_BOXOPEN,	//!	box open ��û.
	NET_MSG_GCTRL_INVEN_BOXOPEN_FB,	//!	box open ��û FB.

	NET_MSG_GCTRL_INVEN_DISGUISE,	//!	���忡 �ڽ����� ������.
	NET_MSG_GCTRL_INVEN_DISGUISE_FB,	//!	���忡 �ڽ����� ������ FB.

    NET_MSG_GCTRL_INVEN_BOX_WRAPPING,	//!	�����ۿ� �������� ����.;
    NET_MSG_GCTRL_INVEN_BOX_WRAPPING_FB,	//!	�����ۿ� �������� ���� FB.;

    NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING,	//!	������ �ڽ��� ǯ.;
    NET_MSG_GCTRL_INVEN_BOX_UNWRAPPING_FB,	//!	������ �ڽ��� ǯ FB.;

	NET_MSG_GCTRL_INVEN_GRINDING,	//!	INVEN ������ ���� �õ�.
	NET_MSG_GCTRL_INVEN_LOCKBOX_UNLOCK,	//! ��� ���� ���� �õ�,
	NET_MSG_GCTRL_INVEN_GRINDING_FB,	//!	INVEN ������ ���� �õ�.

	NET_MSG_GCTRL_INVEN_PERIODEXTEND,	//!	������ �Ⱓ ���� �õ�;
	NET_MSG_GCTRL_INVEN_PERIODEXTEND_FB,

	NET_MSG_GCTRL_REGEN_GATE,	//!	��Ȱ ��ġ ����.
	NET_MSG_GCTRL_REGEN_GATE_FB,	//!	��Ȱ ��ġ ���� FB.

	NET_MSG_GCTRL_CURE,	//!	ġ�� ��û.
	NET_MSG_GCTRL_CURE_FB,	//!	ġ�� ��û FB.

	NET_MSG_GCTRL_CHARRESET,	//!	�ɸ��� ���� ���� ��û. ( stats, skill )
	NET_MSG_GCTRL_CHARRESET_FB,	//!	�ɸ��� ���� ���� ��û FB. ( stats, skill )
	
	NET_MSG_GCTRL_NPC_ITEM_TRADE,	//!	������ ��ȯ ��û.
	NET_MSG_GCTRL_NPC_ITEM_TRADE_FB,	//!	������ ��ȯ ��û FB.

	NET_MSG_GCTRL_CONFRONT,	//!	��� ��û to (����)
	NET_MSG_GCTRL_CONFRONT_TAR,	//!	��� ��û to (����)
	NET_MSG_GCTRL_CONFRONT_ANS,	//!	��� ��û ���� to (����)
	NET_MSG_GCTRL_CONFRONT_FB,	//!	��� ��û ���� to (��û��)

	NET_MSG_GCTRL_CONFRONT_START2_FLD,	//!	��� ���� to (�ʵ�)
	NET_MSG_GCTRL_CONFRONT_START2_CLT,	//!	��� ���� to (Ŭ���̾�Ʈ)
	NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT,	//!	��� FIGHT to (Ŭ���̾�Ʈ)

	NET_MSG_GCTRL_CONFRONT_END2_FLD,	//!	��� ���� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONT_END2_CLT,	//!	��� ���� to (Ŭ���̾�Ʈ).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD,	//!	��� ���� to (�ֺ� Ŭ���̾�Ʈ).
	NET_MSG_GCTRL_CONFRONT_END2_AGT,	//!	��� ���� to (������Ʈ).
	NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR,	//!	��� ���� to (��� ����� Ŭ���̾�Ʈ).

	NET_MSG_GCTRL_CONFRONT_RECOVE,	//!	��ÿɼ� ȸ���� ��� Ƚ�� to (Ŭ���̾�Ʈ)

	NET_MSG_GCTRL_CONFRONTPTY_START2_FLD,	//!	(��Ƽ) ��� ���� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTPTY_START2_CLT,	//!	(��Ƽ) ��� ���� to (Ŭ���̾�Ʈ)

	NET_MSG_GCTRL_CONFRONTPTY_END2_FLD,	//!	(��Ƽ) ��� ���� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTPTY_END2_CLT,	//!	(��Ƽ) ��� ���� to (Ŭ���̾�Ʈ).

	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_FLD,	//!	(��Ƽ) ��ÿ� ������ ��� �˻� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTPTY_CHECKMBR_AGT,	//!	(��Ƽ) ��ÿ� ������ ��� to (������Ʈ).

	NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD,	//!	�п� ��Ƽ ��� �¸��� ����ġ ��ü ������ exp �ο�.

	NET_MSG_GCTRL_CONFRONTCLB_START2_FLD,	//!	(Ŭ��) ��� ���� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTCLB_START2_CLT,	//!	(Ŭ��) ��� ���� to (Ŭ���̾�Ʈ)

	NET_MSG_GCTRL_CONFRONTCLB_END2_FLD,	//!	(Ŭ��) ��� ���� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTCLB_END2_CLT,	//!	(Ŭ��) ��� ���� to (Ŭ���̾�Ʈ).

	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_FLD,	//!	(Ŭ��) ��ÿ� ������ ��� �˻� to (�ʵ�).
	NET_MSG_GCTRL_CONFRONTCLB_CHECKMBR_AGT,	//!	(Ŭ��) ��ÿ� ������ ��� to (������Ʈ).

	NET_MSG_GCTRL_SERVERSTOP,	// Purpose for field server test
	NET_MSG_GCTRL_SERVERSTOP_FB,

	NET_MSG_GCTRL_REQ_GENITEM_AGT,	//!	������ ���� ���� to (������Ʈ).
	NET_MSG_GCTRL_REQ_GENITEM_FLD_FB,	//!	������ ���� �亯 to (�ʵ�).

	NET_MSG_GCTRL_REQ_GESTURE,	//!	������ ����.
	NET_MSG_GCTRL_REQ_GESTURE_BRD,	//!	������ ���� BRD.

	NET_MSG_GCTRL_QITEMFACT_BRD,	//!	?������ ��ġ BRD.
	NET_MSG_GCTRL_QITEMFACT_END_BRD,	//!	?������ ���� BRD.

	NET_MSG_GCTRL_EVENTFACT_BRD,	//!	?������ �̺�Ʈ ���� BRD.
	NET_MSG_GCTRL_EVENTFACT_END_BRD,	//!	?������ �̺�Ʈ ���� BRD.
	NET_MSG_GCTRL_EVENTFACT_INFO,	//!	?������ �̺�Ʈ BRD.

    NET_MSG_GCTRL_IPEVENT_BRD, //! IP Event ����.

	NET_MSG_GCTRL_INVEN_CLEANSER,	//!	������ ������ ����Ͽ� ��ȯ�� ������ �ǵ���.
	NET_MSG_GCTRL_INVEN_CLEANSER_FB,	//!	������ ������ ����Ͽ� ��ȯ�� ������ �ǵ��� FB.

	NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT,	//!	���� �ð� ����� ������ ����.

	NET_MSG_GCTRL_INVEN_RESET_SKST,	//!	���� ��ų ���� ��û.
	NET_MSG_GCTRL_INVEN_RESET_SKST_FB,	//!	���� ��ų ���� ����.

	NET_MSG_GCTRL_INVEN_CHARCARD,	//!	�ɸ��� �߰� ī�� ��� ��û.
	NET_MSG_GCTRL_INVEN_CHARCARD_FB,	//!	�ɸ��� �߰� ī�� ��� ��û FB.

	NET_MSG_GCTRL_INVEN_INVENLINE,	//!	�κ����� �߰� ī�� ��� ��û.
	NET_MSG_GCTRL_INVEN_INVENLINE_FB,	//!	�κ����� �߰� ī�� ��� ��û FB.

	NET_MSG_GCTRL_INVEN_STORAGECARD,	//!	â�� �߰� ī�� ��� ��û.
	NET_MSG_GCTRL_INVEN_STORAGECARD_FB,	//!	â�� �߰� ī�� ��� ��û FB.

	NET_MSG_GCTRL_INVEN_STORAGEOPEN,	//!	��� â�� �̿� ī�� ��� ��û.
	NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB,	//!	��� â�� �̿� ī�� ��� ��û FB.
	NET_MSG_GCTRL_INVEN_STORAGECLOSE,	//!	��� â�� �̿� ī�� ��� �Ϸ�.

	NET_MSG_GCTRL_INVEN_PREMIUMSET,	//!	�����̾��� ��� ��û.
	NET_MSG_GCTRL_INVEN_PREMIUMSET_FB,	//!	�����̾��� ��� ��û FB.

	NET_MSG_GCTRL_FIRECRACKER,	//!	���� ��� ��û.
	NET_MSG_GCTRL_FIRECRACKER_FB,	//!	���� ��� ��û FB.
	NET_MSG_GCTRL_FIRECRACKER_BRD,	//!	���� ���.

	NET_MSG_GCTRL_PLAYERKILLING_ADD,	//!	player killing ����� �߰�.
	NET_MSG_GCTRL_PLAYERKILLING_DEL,	//!	player killing ����� ����.

	NET_MSG_GET_CHARGED_ITEM_CF,        //! ������ ������ ���� DB���� �������� ��û
	NET_MSG_GET_CHARGED_ITEM_FC,       //! ������ ������ ���� DB���� �������� ��û FB
	NET_MSG_GCTRL_CHARGED_ITEM_GET,	   //! ������ ������ �������� ��û.
	NET_MSG_GCTRL_CHARGED_ITEM_GET_FB, //! ������ ������ �������� ��û FB.
	NET_MSG_GCTRL_CHARGED_ITEM_DEL,	//!	������ ������ ����Ʈ���� ����.
	NET_MSG_GCTRL_CHARGED_ITEM2_INVEN,	//!	������ ������ �κ��� �ֱ� ��û.

	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN,	//!	random box open ��û.
	NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB,	//!	random box open ��û FB.
	NET_MSG_GCTRL_INVEN_LOCKBOXOPEN,	//! ��� ���� ���� ��û;
	NET_MSG_GCTRL_INVEN_LOCKBOX_FB,		//! ��� ���� ���� ��û ���;
	NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN,	// ���û��� ����;
	NET_MSG_GCTRL_INVEN_SELFORMBOX_SELECTOPEN_FB,

	NET_MSG_GCTRL_INVEN_DISJUNCTION,	//!	�ڽ��� �и� ��û.
	NET_MSG_GCTRL_INVEN_DISJUNCTION_FB,	//!	�ڽ��� �и� ��û FB.

	NET_MSG_GCTRL_INVEN_HAIR_CHANGE,	//!	��Ÿ�� ���� ��û.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB,	//!	��Ÿ�� ���� ��� FB.
	NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD,	//!	��Ÿ�� ���� ��� BRD.

	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE,	//!	��Ÿ�� ���� ��û.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_FB,	//!	��Ÿ�� ���� ��� FB.
	NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_BRD,	//!	��Ÿ�� ���� ��� BRD.

	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE,	//!	����÷� ���� ��û.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB,	//!	����÷� ���� ��� FB.
	NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD,	//!	����÷� ���� ��� BRD.

	NET_MSG_GCTRL_INVEN_FACE_CHANGE,	//!	�󱼽�Ÿ�� ���� ��û.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB,	//!	�󱼽�Ÿ�� ���� ��� FB.
	NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD,	//!	�󱼽�Ÿ�� ���� ��� BRD.

	NET_MSG_GCTRL_INVEN_REMODELOPEN,	//!	���� ��� �̿� ī�� ��� ��û.
	NET_MSG_GCTRL_INVEN_REMODELOPEN_FB,	//!	���� ��� �̿� ī�� ��� ��û FB.

	NET_MSG_GCTRL_INVEN_GARBAGEOPEN,	//!	������ ��� �̿� ī�� ��� ��û
	NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB,	//!	������ ��� �̿� ī�� ��� ��û FB.
	NET_MSG_GCTRL_GARBAGE_RESULT,	//!	������ ���� ��û
	NET_MSG_GCTRL_GARBAGE_RESULT_FB,	//!	������ ���� ��û FB.

	NET_MSG_CROW_MOVETO,
	NET_MSG_CROW_ATTACK,
	NET_MSG_CROW_AVOID,
	NET_MSG_CROW_DAMAGE,
    NET_MSG_CROW_SKILL,
	NET_MSG_CROW_SKILL_DEALY_ACTION,
	NET_MSG_CROW_END_SKILL,
    NET_MSG_CROW_UPDATE_STATE,

	NET_MSG_GCTRL_INVEN_RENAME,	//!	�̸� ���� ��û.
	NET_MSG_GCTRL_INVEN_RENAME_FB,	//!	�̸� ���� ��� FB.
	NET_MSG_GCTRL_INVEN_RENAME_FROM_DB,	//!	DB ���� ��� FB.
	NET_MSG_GCTRL_INVEN_RENAME_AGTBRD,	//!	Agent�� ���ä�� BRD ��û.
	NET_MSG_GCTRL_INVEN_RENAME_BRD,	//!	�̸� ���� ��� BRD.

	NET_MSG_GCTRL_PARTY_MBR_RENAME,	//! �̸� ������ ��Ƽ������� �˸�.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT,	//! �̸� ������ Ŭ��������� �˸�.
	NET_MSG_GCTRL_FRIEND_RENAME_CLT,	//! �̸� ������ ģ���鿡�� �˸�.
	//NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT,	//! ����ȣ ������ ģ���鿡�� �˸�.
	NET_MSG_GCTRL_CLUB_MBR_RENAME_FLD,	//! �̸� ������ FieldServer�� �˸�.
	NET_MSG_GCTRL_FRIEND_RENAME_FLD,	//! �̸� ������ FieldServer�� �˸�.
	NET_MSG_GCTRL_CHANGE_NAMEMAP	,	//! GLLandMan �� NameMap ����

	NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN, // NPC ��ȭ ��û
	NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN_FB, // NPC ��ȭ ��û FB
	NET_MSG_GCTRL_REQ_QUEST_START,	//!	����Ʈ ������ ��û.
	NET_MSG_GCTRL_REQ_QUEST_START_WITHOUT_NPC,
	NET_MSG_GCTRL_REQ_QUEST_START_FB,	//!	����Ʈ ������ ��û FB.
	NET_MSG_GCTRL_REQ_QUEST_TALK,	//!	����Ʈ ��ȭ ���� ��û.
	NET_MSG_GCTRL_REQ_QUEST_TALK_FB,	//!	����Ʈ ��ȭ ���� ��û FB.

	NET_MSG_GCTRL_QUEST_PROG_GIVEUP,	//!	����Ʈ ���� ���� ��û.
	NET_MSG_GCTRL_QUEST_PROG_DEL,	//!	����Ʈ ���� ����Ʈ���� ����.
	NET_MSG_GCTRL_QUEST_END_DEL,	//!	����Ʈ �Ϸ� ����Ʈ���� ����.
	NET_MSG_GCTRL_QUEST_END_STREAM,	//!	����Ʈ �Ϸ� ����.

	NET_MSG_GCTRL_QUEST_PROG_STREAM,	//!	����Ʈ ���� ����.
	NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM,	//!	����Ʈ ���� step.
	NET_MSG_GCTRL_QUEST_PROG_INVEN,	//!	����Ʈ INVENTORY.

	NET_MSG_GCTRL_QUEST_PROG_NPCTALK,	//!	����Ʈ ���� npc ��ȭ.
	NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB,	//!	����Ʈ ���� npc ��ȭ.

	NET_MSG_GCTRL_QUEST_PROG_MOBKILL,	//!	����Ʈ ���� mob ����.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL,	//!	����Ʈ ���� mob ����.( ��Ƽ����Ʈ �Ϸ�� )
	NET_MSG_GCTRL_QUEST_PROG_QITEM,	//!	����Ʈ ����Ʈ ������ ����.
	NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM,	//!	����Ʈ ���� ����Ʈ ������ ����.( ��Ƽ����Ʈ �Ϸ�� )
	NET_MSG_GCTRL_QUEST_PROG_ITEMUSE,	//!����Ʈ ���� ����Ʈ ������ ���
	NET_MSG_GCTRL_QUEST_PROG_ITEMGRINDING,//!����Ʈ ���� ���� ����Ʈ ������ ����
	NET_MSG_GCTRL_QUEST_PROG_CONFTING,//!����Ʈ ���� ���
	NET_MSG_GCTRL_QUEST_PROG_REACHZONE,	//!	����Ʈ ���� ��ġ ����.
	NET_MSG_GCTRL_QUEST_PROG_READ,	//!	����Ʈ �б� ��û ����.
	NET_MSG_GCTRL_QUEST_PROG_TIME,	//!	����Ʈ ��� �ð�.
	NET_MSG_GCTRL_QUEST_PROG_TIMEOVER,	//!	����Ʈ �ð� ���� �ʰ���.
	NET_MSG_GCTRL_QUEST_PROG_NONDIE,	//!	����Ʈ ��� ����.
	NET_MSG_GCTRL_QUEST_PROG_LEVEL,	//!	����Ʈ ���� ����.
	NET_MSG_GCTRL_QUEST_PROG_LEVEL_UP,//!����Ʈ ���� ��

	NET_MSG_GCTRL_QUEST_COMPLETE_FB,	//!	����Ʈ�� �Ϸ�Ǿ����� ���� ��ǰ�� ���� ���ѻ���.
	NET_MSG_GCTRL_REQ_QUEST_COMPLETE,	//!	����Ʈ �Ϸ� ��û ( ����ǰ ���� ).

	NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN,	//!	����Ʈ �ι� ���� ����.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP,	//!	����Ʈ �ι�� ������ ����.

	NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT,	//!	����Ʈ �ι� ������ ����.
	NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE,	//!	����Ʈ �ι� ������ ����.

	
	NET_MSG_GCTRL_PMARKET_TITLE,	//!	���� ������ Ÿ��Ʋ ����.
	NET_MSG_GCTRL_PMARKET_TITLE_FB,	//!	���� ������ Ÿ��Ʋ ����.

	NET_MSG_GCTRL_PMARKET_REGITEM,	//!	���� ������ �Ǹ� ������ ���.
	NET_MSG_GCTRL_PMARKET_REGITEM_FB,	//!	���� ������ �Ǹ� ������ ��� FB.

	NET_MSG_GCTRL_PMARKET_DISITEM,	//!	���� �������� ������ ����.
	NET_MSG_GCTRL_PMARKET_DISITEM_FB,	//!	���� �������� ������ ���� FB.
	NET_MSG_GCTRL_PMARKET_OPEN,	//!	���� ���� ����.
	NET_MSG_GCTRL_PMARKET_OPEN_FB,	//!	���� ���� ���� FB.
	NET_MSG_GCTRL_PMARKET_OPEN_BRD,	//!	���� ���� ���� BRD.

	NET_MSG_GCTRL_PMARKET_CLOSE,	//!	���� ���� ����.
	NET_MSG_GCTRL_PMARKET_CLOSE_FB,	//!	���� ���� ���� FB.
	NET_MSG_GCTRL_PMARKET_CLOSE_BRD,	//!	���� ���� ���� BRD.
	
	NET_MSG_GCTRL_PMARKET_BUY,	//!	���� �������� ����.
	NET_MSG_GCTRL_PMARKET_BUY_FB,	//!	���� �������� ���� FB.

	NET_MSG_GCTRL_PMARKET_ITEM_INFO,	//!	���� ������ �Ǹ� ������ ���� ��û.
	NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD,	//!	���� ������ �Ǹ� ������ BRD.
	NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD,	//!	���� ������ �Ǹ� ������ Update BRD.

	//! ���λ��� �˻�

	NET_MSG_GCTRL_PMARKET_REGITEM_FH,	//!	���� ������ �Ǹ� ������ ���.
	NET_MSG_GCTRL_PMARKET_DISITEM_FH,	//!	���� �������� ������ ����.
	NET_MSG_GCTRL_PMARKET_SEARCH_REQ_CAH,//! ���� �������� ������ �˻� ��û
	NET_MSG_GCTRL_PMARKET_SEARCH_RESULT_HAC,//!���� ���� ������ �˻� ���

    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_CF,	//!	���λ��� �˻� ī�� ��� ��û.    ( Ŭ�� -> �ʵ弭�� );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FA,	//!	���λ��� �˻� ī�� ��� �˸�.    ( �ʵ弭�� -> ������Ʈ���� );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_AF,	//!	���λ��� �˻� ī�� ��� �˸�����.( ������Ʈ���� -> �ʵ弭�� );
    NET_MSG_GCTRL_PMARKET_SEARCH_OPEN_FB,	//!	���λ��� �˻� ī�� ��� ��� FB. ( �ʵ�or������Ʈ���� -> Ŭ�� );
    NET_MSG_GCTRL_PMARKET_SEARCH_CLOSE,	    //!	��� â�� �̿� ī�� ��� �Ϸ�.    ( Ŭ�� -> ������Ʈ���� );

	//NET_MSG_GCTRL_PMARKET_BUY,	//!	���� �������� ����.
	//NET_MSG_GCTRL_PMARKET_BUY_FB,	//!	���� �������� ���� FB.

	//NET_MSG_GCTRL_PMARKET_ITEM_INFO,	//!	���� ������ �Ǹ� ������ ���� ��û.
	//NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD,	//!	���� ������ �Ǹ� ������ BRD.
	//NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD,	//!	���� ������ �Ǹ� ������ Update BRD.
	//----------------

	NET_MSG_GCTRL_LEVEL_EVENT_END_FLD,	//!	���� �̺�Ʈ ����.
	NET_MSG_GCTRL_LEVEL_EVENT_WARNING_FLD,	//! ���� �̺�Ʈ ��� �޼���.
	NET_MSG_GCTRL_LEVEL_EVENT_COUNTDOWN_FLD,	//! ���� �̺�Ʈ ī��Ʈ �޼���.

	NET_MSG_SERVER_CLOSE_FIELD, //! �ʵ� ���� ���� ����;
	NET_MSG_SERVER_CLOSE_AGENT, //! Agent Server ���� ���� ( Agent Server���� ����Ǵ� �ٸ� Agent Server�� �ǹ��� );

	NET_MSG_GM_EVENT_EX,	//!	EX�̺�Ʈ ����.
	NET_MSG_GM_EVENT_EX_END,	//!	EX�̺�Ʈ ����.

    //NET_MSG_EVENT_EXP_MS,       //! ����ġ �̺�Ʈ ServerManager->Session // ��Ŷ ���� ���� ���ؼ� ������� �̵� 2014.12.18
    NET_MSG_EVENT_EXP_SA,       //! ����ġ �̺�Ʈ Session->Agent
	NET_MSG_GM_EVENT_EXP_CAF,	//!	����ġ �̺�Ʈ ����.
    //NET_MSG_EVENT_EXP_END_MS,   //! ����ġ �̺�Ʈ ���� ServerManager->Session // ��Ŷ ���� ���� ���ؼ� ������� �̵� 2014.12.18
    NET_MSG_EVENT_EXP_END_SA,   //! ����ġ �̺�Ʈ ���� Session->Agent
	NET_MSG_GM_EVENT_EXP_END_CAF,	//!	����ġ �̺�Ʈ ����.
	NET_MSG_GM_EVENT_EXP_END_FB,	//!	����ġ �̺�Ʈ ���� FB.

	NET_MSG_GM_EVENT_ITEM_GEN,	//!	������ �߻��� �̺�Ʈ.
	NET_MSG_GM_EVENT_ITEM_GEN_FB,	//!	������ �߻��� �̺�Ʈ FB.

	NET_MSG_GM_EVENT_ITEM_GEN_END,	//!	������ �߻��� �̺�Ʈ ����.
	NET_MSG_GM_EVENT_ITEM_GEN_END_FB,	//!	������ �߻��� �̺�Ʈ ���� FB.

	NET_MSG_GM_EVENT_MONEY_GEN,	//!	�� �߻��� �̺�Ʈ.
	NET_MSG_GM_EVENT_MONEY_GEN_END,	//!	�� �߻��� �̺�Ʈ ����.

	NET_MSG_GM_EVENT_GRADE,	//!	���� Ȯ�� �̺�Ʈ
	NET_MSG_GM_EVENT_GRADE_FB,	//!	���� Ȯ�� �̺�Ʈ FB.

	NET_MSG_GM_EVENT_GRADE_END,	//!	���� Ȯ�� �̺�Ʈ ����
	NET_MSG_GM_EVENT_GRADE_END_FB,	//!	���� Ȯ�� �̺�Ʈ ���� FB.

	NET_MSG_GCTRL_PERIOD,
	NET_MSG_GCTRL_WEATHER,
	NET_MSG_GCTRL_WHIMSICAL,
	NET_MSG_GCTRL_FIELDINFO_RESET,

	NET_MSG_SERVER_CTRL_WEATHER,	//!	������ ���� ����.
	NET_MSG_SERVER_CTRL_TIME,	//!	������ �ð� ����.
	NET_MSG_SERVER_CTRL_MONTH,	//!	������ �� ����.
	NET_MSG_SERVER_CTRL_GENITEMHOLD,	//!	������ ������ ������ ����.
	NET_MSG_SERVER_PLAYERKILLING_MODE,	//!	player killing on/off.

	NET_MSG_GM_MOVE2GATE,	//!	�ɸ��͸� gate no �� �̵�.
	NET_MSG_GM_MOVE2GATE_FLD,	//!	�ɸ��͸� gate no �� �̵� Agent -> Field
	NET_MSG_GM_MOVE2GATE_FB,	//!	�ɸ��͸� gate no �� �̵�.
	NET_MSG_GM_KICK_USER,	//!	Ư�� �÷��̾ ������.
	NET_MSG_GM_KICK_USER_PROC,	//!	Ư�� �÷��̾ ������.
	NET_MSG_GM_KICK_USER_PROC_FLD,    //!  Ư�� �÷��̾� ���� �������� Agent -> Field
	NET_MSG_GM_MOVE2MAPPOS,	//!	ĳ���͸� Ư������ Ư����ġ�� �̵�.
	NET_MSG_GM_MOVE2MAPPOS_FLD,	//!	ĳ���͸� Ư������ Ư����ġ�� �̵�. Agent -> Field

	NET_MSG_BLOCK_DETECTED,	//!	�ɸ��͸� gate no �� �̵�.
	NET_MSG_SERVER_GENERALCHAT,	//!	���� �˸� �޽��� �߻���.

	NET_MSG_GM_WHERE_NPC,	//!	NPC�� ��� ��ġ�� �ִ��� �˻�.
	NET_MSG_GM_WHERE_NPC_FLD,	//!	NPC�� ��� ��ġ�� �ִ��� �˻�. Agent -> Field
	NET_MSG_GM_WHERE_NPC_FB,	//!	NPC�� ��� ��ġ�� �ִ��� �˻�.

	NET_MSG_GM_WHERE_NPC_ALL,	//!	�ʿ� ��� NPC ��ġ �˻�
	NET_MSG_GM_WHERE_NPC_ALL_FLD,	//!	NPC�� ��� ��ġ�� �ִ��� �˻�. Agent -> Field
	NET_MSG_GM_WHERE_NPC_ALL_FB,	//!	NPC�� ��� ��ġ�� �ִ��� �˻�.

	NET_MSG_GM_WHERE_PC_MAP,	//!	PC�� ��� �㿡 �ִ��� �˻�.
	NET_MSG_GM_WHERE_PC_MAP_FB,	//!	PC�� ��� �㿡 �ִ��� �˻� FB.

	NET_MSG_GM_WHERE_PC_POS,	//!	PC�� ��� ��ġ�� �ִ��� �˻�.
	NET_MSG_GM_WHERE_PC_POS_FB,	//!	PC�� ��� ��ġ�� �ִ��� �˻� FB.

	NET_MSG_GM_MOVE2CHAR,	//!	ĳ���͸� Ư��ĳ���ͷ� �̵� ��û.
	NET_MSG_GM_MOVE2CHAR_POS,	//!	ĳ���͸� Ư��ĳ���ͷ� �̵� ��ġ Ȯ��.
	NET_MSG_GM_MOVE2CHAR_AG,	//!	ĳ���͸� Ư��ĳ���ͷ� �̵� ��ġ Ȯ�� ����.
	NET_MSG_GM_MOVE2CHAR_FLD,	//!	ĳ���͸� Ư��ĳ���ͷ� �̵� ��û ����.
	NET_MSG_GM_MOVE2CHAR_FB,	//!	ĳ���͸� Ư��ĳ���ͷ� �̵� ���� ���.

	NET_MSG_GM_BIGHEAD,	//!	BIG HEAD ��� on/off.
	NET_MSG_GM_BIGHEAD_BRD,	//!	BIG HEAD ��� on/off.

	NET_MSG_GM_BIGHAND,	//!	BIG HAND ��� on/off.
	NET_MSG_GM_BIGHAND_BRD,	//!	BIG HAND ��� on/off.

	NET_MSG_GM_MOB_GEN,	//!	�� �߻�.
	NET_MSG_GM_MOB_GEN_FLD,	//!	�� �߻�. Agent->Field
	NET_MSG_GM_MOB_DEL,	//!	�� ����.
	NET_MSG_GM_MOB_DEL_FLD,	//!	�� ����. Agent->Field
	NET_MSG_GM_MOB_LEVEL,	//!	��(LEVEL FILE) �߻�.
	NET_MSG_GM_MOB_LEVEL_LAYER,	//!	���� ������ �� ���̾� ����
	NET_MSG_GM_MOB_LEVEL_CLEAR,	//!	��(LEVEL FILE) �߻� ����.
	NET_MSG_GM_MOB_LEVEL_LIST, //! Layer list �˸� _ agent -> field;
	NET_MSG_GM_MOB_LEVEL_LIST_FC, //! Layer List �˸� _ field -> client;

	NET_MSG_GM_CHAT_BLOCK_UACCOUNT,	//!	ä�� ��� (�����̸�)
	NET_MSG_GM_CHAT_BLOCK_CHARNAME,	//!	ä�� ��� (ĳ���̸�)
	NET_MSG_GM_CHAT_BLOCK_CHARID,	//!	ä�� ��� (ĳ����ȣ)
	NET_MSG_GM_CHAT_BLOCK_FB,	//!	ä�� ��� FB
	NET_MSG_GM_CHAT_BLOCK_FLD,	//!	ä�� ��� FLD
	NET_MSG_GM_CHAT_BLOCK_NOTIFY, //! ä�� ��� ���� �˸�

	NET_MSG_USER_CHAR_INFO_4NAME,	//! ĳ������ ���� ��� (������ɾ�)
	NET_MSG_USER_CHAR_INFO_AGT_FB,	//!	ĳ������ ���� ��� agt FB.
	NET_MSG_USER_CHAR_INFO_FLD_FB,	//!	ĳ������ ���� ��� fld FB.
	NET_MSG_USER_CHAR_INFO_FLD,	//!	ĳ������ ���� ��� FLD.

	NET_MSG_GM_CHAR_INFO_4NAME,	//!	ĳ������ ���� ��� (ĳ���̸�).
	NET_MSG_GM_CHAR_INFO_4CHARID,	//!	ĳ������ ���� ��� (ĳ����ȣ).
	NET_MSG_GM_CHAR_INFO_AGT_FB,	//!	ĳ������ ���� ��� agt FB.
	NET_MSG_GM_CHAR_INFO_FLD_FB,	//!	ĳ������ ���� ��� fld FB.
	NET_MSG_GM_CHAR_INFO_FLD,	//!	ĳ������ ���� ��� FLD.

   
//-----------------------------------------------------------------------------------------------------------

	NET_MSG_GM_FREEPK,	//!	���� ������
	NET_MSG_GM_FREEPK_BRD,	//!	���� ������

	NET_FREEPK_GM_COMMAND_CA, //! GM��ɾ� ���� PK ON/OFF

	NET_MSG_GM_WARNING_MSG,	//!	���� ������
	NET_MSG_GM_WARNING_MSG_FLD,	//!	���� ������
	NET_MSG_GM_WARNING_MSG_BRD,	//!	���� ������

	NET_MSG_GM_VIEWALLPLAYER,	//!  ���� ��� �÷��̾� ǥ��
	NET_MSG_GM_VIEWALLPLAYER_FLD_REQ,	//!  �ʵ弭���� ��û
	NET_MSG_GM_VIEWALLPLAYER_FLD_FB ,	//!  �ʵ弭�� ó����� ȸ��

	NET_MSG_GM_GENCHAR,	//!  �ɸ��� ��ȯ ��û
	NET_MSG_GM_GENCHAR_POS,	//!  �ɸ��� ��ȯ ��û
	NET_MSG_GM_GENCHAR_AG,	//!  �ɸ��� ��ȯ ��û
	NET_MSG_GM_GENCHAR_FLD,	//!  �ɸ��� ��ȯ ��û
	NET_MSG_GM_GENCHAR_FB,	//!  �ɸ��� ��ȯ ��û

	NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO,	// �����̺�Ʈ

	NET_MSG_GCTRL_UPDATE_LASTCALL,	// ���� ��ȯ ��ġ Ŭ���̾�Ʈ�� ����
	NET_MSG_GCTRL_UPDATE_STARTCALL,	// ���� ��ȯ ��ġ Ŭ���̾�Ʈ�� ����

	NET_MSG_GM_WHYSOSERIOUS_EMULATOR_CF,	
    NET_MSG_GM_SHOWMETHEMONEY_EMULATOR_CF,	
    NET_MSG_GM_COUNTDOWN_MSG_BRD,	// �ڵ����� ī��Ʈ �޼���
	NET_MSG_GM_SHOWMETHEMONEY,	//!  ĳ���ͷ� �� ����
	NET_MSG_GM_SHOWMETHEMONEY_FLD,	//!  ĳ���ͷ� �� ����
    NET_MSG_GM_WHYSOSERIOUS,	//!  ĳ���ͷ� ������ ����
    NET_MSG_GM_WHYSOSERIOUS_FLD,	//!  ĳ���ͷ� ������ ����
	NET_MSG_GM_WHYSOSERIOUS_FB_FAC, //! ������ ���� ���;

	NET_MSG_GM_HITRATESCRIPT_MSG_CF, // ���ݼ����� ��ũ��Ʈ ���ε� ��û;

	NET_MSG_GM_FLYCAMERACONTROL_CF,
	NET_MSG_GM_FLYCAMERACONTROL_FC,
	NET_MSG_GM_FLYCAMERA_SETVIEWSIGHT_CF,
	NET_MSG_GM_FLYCAMERACONTROL_CA,	//! C->A, GM ī�޶� ���� ����;
	NET_MSG_GM_FLYCAMERACONTROL_FB,	//! A->C, GM ī�޶� ���� ����;
	NET_MSG_GM_FLYCAMERACONTROL_MOVE_CF,

    NET_MSG_GM_LOG_TO_DB_CAF,   //!  �ؽ�Ʈ�� �������� �α׸� ������ DB �� �ø��� ��

	NET_MSG_GM_AUTO_LEVEL, // autoLevel Control �޼��� _ client -> agent;
	NET_MSG_GM_AUTO_LEVEL_LIST, // autoLevel Control �޼��� _ agent -> client;
	NET_MSG_GM_AUTO_LEVEL_RESULT, // autoLevel Control �޼��� _ agent -> client;
	NET_MSG_GM_AUTO_LEVEL_INFORMATION, // autoLevel Control �޼��� _ agent -> client;

	NET_MSG_GM_LAND, // Land Control Message _ client -> server;
	NET_MSG_GM_LAND_INFORMATION, // Land Information Lotify Message _ field -> client;

	NET_MSG_GM_MAP_LAYER, // Map Layer Control Message _ client -> agent;
	NET_MSG_GM_MAP_LAYER_NEW_NOTIFY, // Map Layer Notify Message _ agent -> client;
	NET_MSG_GM_MAP_LAYER_DEL_NOTIFY, // Map Layer Notify Message _ agent -> client;
	NET_MSG_GM_MAP_LAYER_ERROR_NOTIFY, // Map Layer Notify Message _ agent -> client;

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_REBUILD_RESULT,	// ������ ���� - ����, ���� ���	// ITEMREBUILD_MARK
	NET_MSG_REBUILD_MOVE_ITEM,	// ������ ���� - ������ �̵�
	NET_MSG_REBUILD_COST_MONEY,	// ������ ���� - ���� ���
	NET_MSG_REBUILD_INPUT_MONEY,	// ������ ���� - ���� �Է� �ݾ�

//-----------------------------------------------------------------------------------------------------------

	NET_MSG_PET_DROPPET,	// ANYPET ����
	NET_MSG_CREATE_ANYPET,	// ANYPET ����
	NET_MSG_PET_REQ_USECARD,	// ��ī�� ���
	NET_MSG_PET_REQ_USECARD_FB,
	NET_MSG_PET_REQ_UNUSECARD,	// �� �Ҹ�
	NET_MSG_PET_REQ_UNUSECARD_FB,
	NET_MSG_PET_REQ_DISAPPEAR,	// ������ �Ҹ�(���̵���, ���������...)

	NET_MSG_CREATE_PET_FROMDB_FB,	// �ֻ���(DB)
	NET_MSG_GET_PET_FROMDB_FB,	// �ֻ���(DB)
	NET_MSG_GET_PET_FROMDB_ERROR,	// �ֻ���(DB) ����

	NET_MSG_PET_REQ_UPDATE_MOVE_STATE,  // ������ ���� ����
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB,
	NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD,

	NET_MSG_PET_REQ_SKILL_CHANGE_CF,	// ��ų����
	NET_MSG_PET_REQ_SKILLCHANGE_FB,	// ��ų����
	NET_MSG_PET_REQ_SKILL_CHANGE_FC,	// ��ų����

	NET_MSG_PET_ADD_SKILLFACT,	// SkillFact �߰�
	NET_MSG_PET_REMOVE_SKILLFACT,	// SkillFact �߰�

	NET_MSG_PET_REQ_GOTO,	// �̵�ó��
	NET_MSG_PET_REQ_STOP,	// �̵�ó��
	NET_MSG_PET_REQ_GOTO_FB,	// �̵�ó��
	NET_MSG_PET_GOTO_BRD,	// �̵�ó��
	NET_MSG_PET_STOP_BRD,	// �̵�ó��

	NET_MSG_PET_REQ_GIVEFOOD,	// ����ֱ�
	NET_MSG_PET_REQ_GIVEFOOD_FB,	// ����ֱ�
	NET_MSG_PET_REQ_GETFULL_FROMDB_FB,	// ����ֱ�

	NET_MSG_PET_GETRIGHTOFITEM,	// ���������� �ִ� ���������� ��û
	NET_MSG_PET_GETRIGHTOFITEM_FB,	// ���������� �ִ� ���������� ��û
	NET_MSG_PET_NOTENOUGHINVEN,	// �κ��丮�� �����Ͽ� ���� ������� ��

	NET_MSG_PET_REQ_RENAME,	// �̸�����
	NET_MSG_PET_REQ_RENAME_FB,	// �̸�����
	NET_MSG_PET_REQ_RENAME_BRD,	// �̸�����
	NET_MSG_PET_REQ_RENAME_FROMDB_FB,	// �̸�����

	NET_MSG_PET_REQ_CHANGE_COLOR,	// �÷�����
	NET_MSG_PET_REQ_CHANGE_COLOR_FB,	// �÷�����
	NET_MSG_PET_REQ_CHANGE_COLOR_BRD,	// �÷�����

	NET_MSG_PET_REQ_CHANGE_STYLE,	// ��Ÿ�Ϻ���
	NET_MSG_PET_REQ_CHANGE_STYLE_FB,	// ��Ÿ�Ϻ���
	NET_MSG_PET_REQ_CHANGE_STYLE_BRD,	// ��Ÿ�Ϻ���

	// Ŭ���̾�Ʈ ���� ���� (Field -> Client)
	NET_MSG_PET_UPDATECLIENT_FULL,	// �������� ����

	NET_MSG_PET_REQ_INVEN_EX_SLOT,	// ������ Ż��(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_FB,	// ������ Ż��(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD,	// ������ Ż��(SLOT<->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_INVEN,	// ������ Ż��(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_FB,	// ������ Ż��(SLOT->HOLD)
	NET_MSG_PET_REQ_SLOT_TO_HOLD_BRD,	// ������ Ż��(SLOT->HOLD)
	NET_MSG_PET_REQ_INVEN_TO_SLOT,	// ������ Ż��(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_FB,	// ������ Ż��(HOLD->SLOT)
	NET_MSG_PET_REQ_HOLD_TO_SLOT_BRD,	// ������ Ż��(HOLD->SLOT)

	NET_MSG_PET_REQ_LEARNSKILL,	// �ֽ�ų ����
	NET_MSG_PET_REQ_LEARNSKILL_FB,	// �ֽ�ų ����
	NET_MSG_PET_REQ_LEARNSKILL_BRD,	// �ֽ�ų ����

	NET_MSG_PET_REQ_FUNNY,	// Funny Gesture
	NET_MSG_PET_REQ_FUNNY_BRD,	// Funny Gesture

	NET_MSG_PET_REMOVE_SLOTITEM,	// �ֽ��Ծ����� ����
	NET_MSG_PET_REMOVE_SLOTITEM_FB,	// �ֽ��Ծ����� ����
	NET_MSG_PET_REMOVE_SLOTITEM_BRD,	// �ֽ��Ծ����� ����

	NET_MSG_PET_ATTACK,	// ���ݸ��
	NET_MSG_PET_ATTACK_BRD,	// ���ݸ��

	NET_MSG_PET_SAD,	// Sad���
	NET_MSG_PET_SAD_BRD,	// Sad���

	NET_MSG_PET_REQ_PETCARDINFO,	// ��ī�� ����
	NET_MSG_PET_REQ_PETCARDINFO_FB,	// ��ī�� ����

	NET_MSG_PET_REQ_PETREVIVEINFO,	// �ֺ�Ȱ ����
	NET_MSG_PET_REQ_PETREVIVEINFO_FB,	// �ֺ�Ȱ ����

	NET_MSG_PET_REQ_REVIVE,	// ������ �� ��Ȱ
	NET_MSG_PET_REQ_REVIVE_FB,	// ������ �� ��Ȱ
	NET_MSG_PET_REQ_REVIVE_FROMDB_FB,	// ������ �� ��Ȱ

	NET_MSG_PET_ACCESSORY_DELETE,	// �־Ǽ��縮 ����
	NET_MSG_PET_ACCESSORY_DELETE_BRD,	// �־Ǽ��縮 ����

	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION,	// �� ��ų Ȯ�� ����
	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_FB,	// �� ��ų Ȯ�� ����
	NET_MSG_PET_REQ_SKILLSLOT_EXPANSION_BRD,	// �� ��ų Ȯ�� ����

	//NET_MSG_SMS_PHONE_NUMBER,	// ĳ���� ����ȣ ����/������Ʈ ��û
	//NET_MSG_SMS_PHONE_NUMBER_FB,	//!
	//NET_MSG_SMS_PHONE_NUMBER_FROM_DB,	//!	DB ���� ��� FB.
	//NET_MSG_SMS_PHONE_NUMBER_AGTBRD,	//!	Agent�� ���ä�� BRD ��û.

	NET_MSG_SMS_SEND,
	NET_MSG_SMS_SEND_FB,
	NET_MSG_SMS_SEND_FROM_DB,

	NET_MSG_GM_MOB_GEN_EX,   // ����ȯ(Ȯ�����߰�)
	NET_MSG_GM_MOB_GEN_EX_FLD,   // ����ȯ(Ȯ�����߰�)
    NET_MSG_GM_MOB_DEL_EX,   // ������(Ȯ�����߰�)
	NET_MSG_GM_MOB_DEL_EX_FLD,   // ������(Ȯ�����߰�)	

    NET_MSG_ODD_EVEN_LOG_FH, //! Ȧ¦���� ��� �α� Field->Cache
	NET_MSG_SERVERTIME_BRD,	// ���� ����ð� BroadCasting

	NET_MSG_GCTRL_MINIGAME, // �̴ϰ���;

	NET_MSG_GCTRL_EVENT, // �̺�Ʈ;

	// Instance System; 
	NET_MSG_GCTRL_INSTANCE_SYSTEM, // �ν��Ͻ� ���� �ý���;
    NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, // �ν��Ͻ� ���� �ý��� (MSGPACK��);
    NET_MSG_GCTRL_INSTANCE_CONTENTS, // �ν��Ͻ� �����ý����� ����� �������� �޽���;
    NET_MSG_GCTRL_INSTANCE_CONTENTS_MSGPACK, // �ν��Ͻ� �����ý����� ����� �������� �޽���(MSGPACK��);

	NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_FB,	// Faction �絵��;
	NET_MSG_GCTRL_INSTANCE_RETRY_FACTION_CF,	// Faction �絵�� ����;
	NET_MSG_GCTRL_INSTANCE_SHOW_TIME_UI_FB,		// Show Time UI;
	NET_MSG_GCTRL_INSTANCE_SHOW_ENTRANCE_STATE_UI_FB,	// Show Entrance State UI;
	NET_MSG_GCTRL_INSTANCE_SET_RETURNMAP_INFORMATION_FA,	// ReturnMap Information ����;


	// Matching System
	NET_MSG_GCTRL_MATCHING_SYSTEM, // ��Ī �ý���
	NET_MSG_GCTRL_MATCHING_MSGPACK_SYSTEM, // ��Ī �ý���

	NET_MSG_GCTRL_MATCHING_UPDATE_JOIN_COUNT_AC,	// Join Count ����;

    // Faction System;
    NET_MSG_FACTION_SYSTEM,  // ���� ���� �޽���;

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


	NET_MSG_GCTRL_SET_SERVER_DELAYTIME,	// ���� ���1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FB,	// ���� ���1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_AGT,	// ���� ���1
	NET_MSG_GCTRL_SET_SERVER_DELAYTIME_FLD,	// ���� ���1
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET,	// ���� ���2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FB,	// ���� ���2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_AGT,	// ���� ���2
	NET_MSG_GCTRL_SET_SERVER_SKIPPACKET_FLD,	// ���� ���2
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME,	// ���� ���3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FB,	// ���� ���3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_AGT,	// ���� ���3
	NET_MSG_GCTRL_SET_SERVER_CRASHTIME_FLD,	// ���� ���3


	//
	//mjeon.Add
	//Messages for Ping process
	//
	NET_MSG_GM_PING,	//!  Ping ��û
	NET_MSG_GM_PING_FB,	//!  Ping ����



	NET_MSG_GCTRL_SERVER_LOGINMAINTENANCE_AGT,	//�α��μ��� MainTenance��� On/Off 

	NET_MSG_GCTRL_STORAGE_ITEM_UPDATE,	//! ��Ŀ ������ ������Ʈ
	//NET_MSG_REQ_FRIEND_CLUB_OPEN,	//! ģ��,Ŭ��â ����(Ŭ���̾�Ʈ->������Ʈ)

	
	//
	//mjeon.Add
	//Messages for Post
	//

	NET_MSG_POST_REQ_CHANUM,	// ChaNum ��û to Agent
	NET_MSG_POST_REQ_CHANUM_FB,	// ChaNum ��û�� ���� ����


	// Sender --------------------> FieldS --------------------> Recver (DB Create)
	NET_MSG_POST_SEND,	// ���� ������ ��û
	NET_MSG_POST_SEND_FB,	// ���� ������ ����


	// Recver <-------------------- AgentS <-------------------- FieldS
	NET_MSG_POST_NOTIFY,	// ���� ���� �˸� (Field->Agent )
	NET_MSG_POST_NOTIFY_RELAY,	// ���� ���� �˸� (Agent->Client)


	// Recver <-------------------> AgentS
	NET_MSG_POST_GET,	// ���� ��� ��û
	NET_MSG_POST_GET_INFO_FB,	// ���� ���� �� ���� ����
	NET_MSG_POST_GET_DATA_FB,	// ���� ���� ������ ����


	// Recver�� ���� ����/Ȯ��
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_OPEN,	// ���� �б�(����)
	NET_MSG_POST_OPEN_TAKE,	// ���� �б� & ÷�� ���� (Client->Agent)
	NET_MSG_POST_OPEN_TAKE_AF,	// ���� �б� & ÷�� ���� (Agent->Field)
	NET_MSG_POST_OPEN_TAKE_AF_FB,	// ���� �б� & ÷�� ���� (Field->Agent)
	NET_MSG_POST_OPEN_TAKE_FB,	// ���� �б� & ÷�� ���� (Field->Agent)


	// Recver�� ���� �ݼ�
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_RETURN,	// ���� �ݼ� ��û
	NET_MSG_POST_RETURN_FB,	// ���� �ݼ� ����


	// �ݼ� �ÿ��� Sender���� POST_NOTIFY ���


	// Sender�� �ݼ� ���� Ȯ��
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update
	NET_MSG_POST_RETURN_OPEN,	// �ݼ۵� ���� �б�(�ޱ�)
	NET_MSG_POST_RETURN_OPEN_FB,	// �ݼ۵� ���� �б�(�ޱ�) ����


	// ���� ����
	// Recver --------------------> SERVER --------------------> DB (of Recver) Update	
	NET_MSG_POST_DELETE,	// ���� ����
	NET_MSG_POST_DELETE_FB,	// ���� ���� ����

	
	// Recver <-------------------> AgentS
    NET_MSG_POST_GET_STATE,	// ���� ��Ȳ ��û
    NET_MSG_POST_GET_STATE_FB,	// ���� ��Ȳ ���� �� ���� ����
    NET_MSG_POST_GET_SEND_STATE,	// ���� ���� ��Ȳ ��û
    NET_MSG_POST_GET_SEND_STATE_FB,	// ���� ���� ��Ȳ ���� �� ���� ����
    NET_MSG_POST_GET_RECV_STATE,	// ���� ���� ��Ȳ ��û
    NET_MSG_POST_GET_RECV_STATE_FB,	// ���� ���� ��Ȳ ���� �� ���� ����

	NET_MSG_POST_TODO_AFTER_SEND_FF,	//���� �����⸦ �� �Ŀ�, GaeaServer���� ó������ ���׵�



	//
	//mjeon.sns
	//

	//
	// --------------------------- FACEBOOK -------------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_FB_AUTHED,	// Facebook �������� ����
	NET_MSG_SNS_FB_AUTHED_FB,	// Facebook �������� ���� ����
	NET_MSG_SNS_FB_UNAUTHED,	// Facebook �������� ����
	NET_MSG_SNS_FB_UNAUTHED_FB,	// Facebook �������� ���� ����	
	NET_MSG_SNS_FB_GETUID,	// Facebook UID ��û	
	NET_MSG_SNS_FB_GETUID_FB,	// Facebook UID ����

	// AgentS <-------------------> FieldS ---------------------> DB
	NET_MSG_SNS_FB_UPDATE_AF,	// FieldServer�� FB ���� ���� - �ܹ���

	// FieldS <---------- Broadcasting to ViewAround -----------> Client(s)
	NET_MSG_SNS_FB_UPDATE_BR,	// FieldServer -> Clients - �ܹ���



	//
	// ---------------------------- TWITTER -------------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_TW_AUTHED,	// Twitter �������� ����
	NET_MSG_SNS_TW_AUTHED_FB,	// Twitter �������� ���� ����
	NET_MSG_SNS_TW_UNAUTHED,	// Twitter �������� ����
	NET_MSG_SNS_TW_UNAUTHED_FB,	// Twitter �������� ���� ����
	NET_MSG_SNS_TW_GETUID,	// Twitter UID ��û	
	NET_MSG_SNS_TW_GETUID_FB,	// Twitter UID ����
	
	// AgentS <-------------------> FieldS ---------------------> DB
	NET_MSG_SNS_TW_UPDATE_AF,	// FieldServer�� TW ���� ���� - �ܹ���

	// FieldS <---------- Broadcasting to ViewAround -----------> Client(s)
	NET_MSG_SNS_TW_UPDATE_BR,	// FieldServer -> Clients - �ܹ���

	//
	// --------------------------- SNS Common -----------------------------
	//

	// Client <-------------------> AgentS ---------------------> DB
	NET_MSG_SNS_GETINFO,	// SNS ���� ��û
	NET_MSG_SNS_GETINFO_FB,	// SNS ���� ����
	

	NET_MSG_GM_GETWHISPERMSG,	//!	ĳ������ �ӼӸ��� ����´�.
	NET_MSG_CHINA_GAINTYPE  ,	//!	�߱� ��޺� ����

	NET_MSG_GM_LIMIT_EVENT_BEGIN,	//!	�̺�Ʈ ����.
	NET_MSG_GM_LIMIT_EVENT_END,	//!	�̺�Ʈ ����.

	NET_MSG_GM_LIMIT_EVENT_RESTART,	//!	�̺�Ʈ �ٽ� ����.

	NET_MSG_GM_LIMIT_EVENT_APPLY_START,	//!	Ŭ���̾�Ʈ�� �̺�Ʈ ���� ���������� �˸�
	NET_MSG_GM_LIMIT_EVENT_APPLY_END,	//!	Ŭ���̾�Ʈ�� �̺�Ʈ ���� �������� �˸�

	NET_MSG_GM_LIMIT_EVENT_BEGIN_FB,   //!	Ŭ���̾�Ʈ�� �̺�Ʈ�� ���� �Ǿ����� �˸�
	NET_MSG_GM_LIMIT_EVENT_END_FB,   //!	Ŭ���̾�Ʈ�� �̺�Ʈ�� ���� �Ǿ����� �˸�

	NET_MSG_GM_LIMIT_EVENT_TIME_REQ,   //!	Ŭ���̾�Ʈ�� ������ �̺�Ʈ ������ ��û
	NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB,   //!	������ Ŭ���̾�Ʈ�� �̺�Ʈ ������ ����
	NET_MSG_GM_LIMIT_EVENT_TIME_RESET,   //!	AGENT������ FIELD�������� �α��� �ð��� RESET�϶�� ����

	NET_MSG_GM_PRINT_CROWLIST,   //!	�ڽ��� �ִ� ���� �� ����Ʈ�� ����Ʈ��.
	NET_MSG_GM_PRINT_CROWLIST_FLD,   //!	�ڽ��� �ִ� ���� �� ����Ʈ�� ����Ʈ��.
	NET_MSG_GM_PRINT_CROWLIST_FB,   //!	�ڽ��� �ִ� ���� �� ����Ʈ�� ����Ʈ��.

	NET_MSG_SERVER_CTRL_WEATHER2,	//!	������ ���� ����. ���ο� �޽���
	NET_MSG_GCTRL_MAPWEATHER,	
	NET_MSG_GCTRL_MAPWHIMSICAL,

	NET_MSG_CHAR_ACTIVE_VEHICLE_CF,	//! Ŭ���̾�Ʈ�� Ż���� Ȱ��/��Ȱ�� ��û
	NET_MSG_CHAR_ACTIVE_VEHICLE_FC,	//! Ŭ���̾�Ʈ�� Ż���� Ȱ��/��Ȱ�� �˸�
	NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD,	//! ���� Ŭ���̾�Ʈ�� Ż���� Ȱ��/��Ȱ�� �˸�

	NET_MSG_GCTRL_GET_VEHICLE,	//! Ŭ���̾�Ʈ�� Ż���� ���� ��û
	NET_MSG_GCTRL_GET_VEHICLE_FB,	//! Ŭ���̾�Ʈ�� Ż���� ���� ����
	NET_MSG_GCTRL_GET_VEHICLE_BRD,	//! ���� Ŭ���̾�Ʈ�� Ż�� ���� ����
	NET_MSG_GCTRL_UNGET_VEHICLE_FB,	//! Ż�� ���� ��Ȱ��ȭ
	NET_MSG_GCTRL_UNGET_VEHICLE_BRD,	//! Ż�� ���� ��Ȱ��ȭ

	NET_MSG_VEHICLE_CREATE_FROMDB_FB,	//! ���� Ż�� ���� ( DB ) 
	NET_MSG_VEHICLE_GET_FROMDB_FB,	//! Ż�� ���� ( DB )
	NET_MSG_VEHICLE_GET_FROMDB_ERROR,	//! Ż�� ���� ( DB���� )
	
	NET_MSG_VEHICLE_ACCESSORY_DELETE,	//! Ż���� �Ǽ����� ����
	NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD,	// Ż���� �Ǽ��縮 ���� �˸�
	
	NET_MSG_VEHICLE_REQ_INVEN_EX_SLOT,	// Ż�� slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB,	// Ż�� slot ex hold
	NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD,	// Ż�� slot ex hold
	
	NET_MSG_VEHICLE_REQ_INVEN_TO_SLOT,	// Ż�� hold to slot
	NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB,	// Ż�� hold to slot
	
	NET_MSG_VEHICLE_REQ_SLOT_TO_INVEN,	// Ż�� slot to hold
	NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB,	// Ż�� slot to hold
	
	NET_MSG_VEHICLE_REMOVE_SLOTITEM,	// item slot ����
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB,	// item slot ����
	NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD,	// item slot ����

	NET_MSG_VEHICLE_REQ_GIVE_BATTERY,	// ���͸� ����
	NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB,	// ���͸� ����
	NET_MSG_VEHICLE_REQ_GETFULL_FROMDB_FB,	// ���͸� ����
	NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY,	// ���͸� ����
	NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB,	// ���͸� �������� ( ���� )

	NET_MSG_VEHICLE_REQ_ITEM_INFO,	// Ż�;����� ����
	NET_MSG_VEHICLE_REQ_ITEM_INFO_FB,	// Ż�;����� ����

	NET_MSG_VIETNAM_GAINTYPE,	// ��Ʈ�� Ž�� ���� �ý��� ���� ���� Ÿ��
	NET_MSG_VIETNAM_TIME_REQ,	// ��Ʈ�� Ž�� ���� �ý��� �ð� ��û
	NET_MSG_VIETNAM_TIME_REQ_FB,	// ��Ʈ�� Ž�� ���� �ý��� �ð� ��û
	NET_MSG_VIETNAM_GAIN_EXP,	// ��Ʈ�� Ž�� ���� �ý��� ����ġ
	NET_MSG_VIETNAM_GAIN_MONEY,	// ��Ʈ�� Ž�� ���� �ý��� ��
	NET_MSG_GCTRL_REQ_VNGAIN_TO_HOLD,   //!	Ž�й��� �ι꿡 �ִ� �������� ��� �õ�.
	NET_MSG_GCTRL_REQ_VNGAIN_EX_HOLD,	//!	Ž�й��� �ι꿡 �ִ� �������� ��� �õ�.
	NET_MSG_GCTRL_REQ_HOLD_TO_VNGAIN,	//!	�������� Ž�й��� �ι꿡 ����
	NET_MSG_GCTRL_REQ_VNGAIN_INVEN_RESET,	//! Ž�� ���� �ι� ��ü �ʱ�ȭ
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN,	//! ������ ��ư���� Ž�� ���� �ι꿡�� �ι����� ������ �ű涧
	NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB,	//! ������ ��ư���� Ž�� ���� �ι꿡�� �ι����� ������ �ű涧
	NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET,	//!	��Ʈ�� Ž�� ���� ������ ���� �޽���
	NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB,	//!	��Ʈ�� Ž�� ���� ������ ���� �޽���
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB,	//!	��Ʈ�� Ž�� ���� ������ ���� �޽���
	NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE,	//!	��Ʈ�� Ž�� ���� ������ ���� �޽��� 
	NET_MSG_VIETNAM_ALLINITTIME,	//!	��Ʈ�� Ž�� �����ý��� 12�ð� ������ �ʱ�ȭ �Ǿ�����
	
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE,	// ���� ����
	NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB,	// ���� ����

	NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE,	// �п� ����
	NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE_FB,	// �п� ����

	NET_MSG_MARKET_CHECKTIME,	// ������ �������� �� 10�и��� �޽����� ���� ƨ�� ������ �����Ѵ�.

	NET_MSG_CHAT_CTRL_FB, //! �����ڿ� ä�ø޽��� ���� ( ���Ӽ���->Ŭ���̾�Ʈ )

	NET_QBOX_OPTION_REQ_AG, //! QBox On/Off �ɼ� ������ -> ������Ʈ ����
	NET_QBOX_OPTION_REQ_FLD, //! QBox On/Off �ɼ� ������Ʈ ���� -> �ʵ弭��
	NET_QBOX_OPTION_MEMBER, //! QBox On/Off �ɼ� �ʵ� ���� -> �������..

	//NET_MSG_UPDATE_TRACING_ALL, //! ������ ĳ���͸� ���� ������Ʈ�Ѵ�. // ��Ŷ ���� ���� ���ؼ� ������� �̵� 2014.12.18
	//NET_MSG_UPDATE_TRACING_CHARACTER, //! ������ ĳ���͸� Ư�� ĳ���͸� ������Ʈ�Ѵ�. // ��Ŷ ���� ���� ���ؼ� ������� �̵� 2014.12.18
	NET_MSG_LOG_UPDATE_TRACING_CHARACTER, //! // �������� ������ �α׸� agent������ ������.

	NET_MSG_TRACING_CHARACTER, //! �ʵ�->Ŭ���̾�Ʈ ���� �������� �ƴ����� ������.

	NET_MSG_GCTRL_REQ_TELEPORT,	//!	��ȯ�� ������ ��� ��û.
	NET_MSG_GCTRL_REQ_TELEPORT_FB,	//!	��ȯ�� ������ ��� ����.
	NET_MSG_GCTRL_REQ_TELEPORT_AG,	//!	��ȯ�� ������Ʈ ������ ��û.

	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC,	// ����ġ ȸ�� ������ ��û (NPC)
	NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB,	// ����ġ ȸ�� ������ ��û FB (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC,	// ����ġ ȸ�� ��� (NPC)
	NET_MSG_GCTRL_RECOVERY_NPC_FB,	// ����ġ ȸ�� ��� FB (NPC)
	NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP,	//!	����Ʈ ���̵� ����.
	NET_MSG_GM_CLASS_EVENT,	//!	Ŭ������ �̺�Ʈ

	NET_MSG_VIETNAM_TIME_RESET,	// ��Ʈ�� �ð� ���� //vietnamtest%%% && vietnamtest2

	NET_MSG_GCTRL_ITEMSHOPOPEN,	//!	ItemShopOpen ( Japan ) 
	NET_MSG_GCTRL_ITEMSHOPOPEN_BRD,	//!	ItemShopOpen ( Japan ) 

	NET_MSG_GM_VIEWWORKEVENT,	//! ���� agent������ ����� ��� �̺�Ʈ ����
	NET_MSG_GM_VIEWWORKEVENT_FB,	//! ���� agent������ ����� ��� �̺�Ʈ ���� ȸ��

	NET_MSG_PET_PETSKINPACKOPEN,	//! �� ��Ų �� ������ ���
	NET_MSG_PET_PETSKINPACKOPEN_FB,	//! �� ��Ų �� ������ ���
	NET_MSG_PET_PETSKINPACKOPEN_BRD,	//! �� ��Ų �� ������ ���

	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM,	//! ���λ����� ������ �˻�
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT,	//! ���λ����� ������ �˻� ���
	NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT_REQ,	//! ���λ����� ������ �˻� ��� ��û
	
	NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE,	//! �ǰݽ� ��ų �ߵ�

	NET_MSG_GCTRL_BEGIN_SKILL_FC, // ��ų ���� �� ��� �Ϸ� �Ǹ� ����->Ŭ�� �޼��� �۽�;
	NET_MSG_GCTRL_END_SKILL_FC, // ��ų�� ���� �Ǿ��� ��� ����->Ŭ�� �޼��� �۽�;
	NET_MSG_GCTRL_END_SKILL_BRD, // ��ų�� ���� �Ǿ��� ��� ����->�ֺ�Ŭ�� �޼��� �۽�;
	
	NET_MSG_REQ_ATTENDLIST,	//! �⼮���� ����Ʈ ��û
	NET_MSG_REQ_ATTENDLIST_FB,	//! �⼮���� ����Ʈ ���
	NET_MSG_REQ_ATTENDANCE,	//! �⼮ üũ
	NET_MSG_REQ_ATTENDANCE_FB,	//! �⼮ üũ �亯
//	NET_MSG_REQ_ATTEND_REWARD_FLD,	//! �⼮ üũ �̺�Ʈ ������ ���� ( �ʵ�� )	
	NET_MSG_REQ_ATTEND_REWARD_CLT,	//! �⼮ üũ �̺�Ʈ ������ ���� ( Ŭ��� )

	NET_MSG_REQ_USE_SUMMON,	//! ��ȯ�� ��û
	NET_MSG_REQ_USE_SUMMON_EVENT_SKILL,	//! ��ȯ�� �̺�Ʈ ��ų;
	NET_MSG_HIRE_BASICSUMMON_CF,	//! ��ȯ�� ���;
	NET_MSG_HIRE_BASICSUMMON_FC,	//! ��ȯ�� ��� ����;
	NET_MSG_REQ_USE_SUMMON_FB,	//! ��ȯ�� ��û ����
	NET_MSG_GCTRL_DROP_SUMMON,	//! ��ȯ�� ����
	NET_MSG_GCTRL_FAILED_DROP_SUMMON,	//! ��ȯ ����;
	NET_MSG_GCTRL_SUMMON_GOTO,	//! ��ȯ�� �̵�
	NET_MSG_GCTRL_SUMMON_ATTACK,	//! ��ȯ�� ����
	NET_MSG_GCTRL_SUMMON_ATTACK_AVOID,	//! ��ȯ�� ���� ȸ��
	NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE,	//! ��ȯ�� ���� ������
	NET_MSG_GCTRL_SUMMON_SKILL,	//! ��ȯ�� ����
	NET_MSG_GCTRL_SUMMON_END_SKILL,	//! ��ȯ�� ��ų ����
	NET_MSG_GCTRL_SUMMON_EFFECT,	//! ��ȯ�� ����Ʈ Render;
	NET_MSG_GCTRL_SUMMON_UPDATE_CUR_SKILLPAGE,	//! ��ȯ�� ��ų ������ ������Ʈ;
	NET_MSG_GCTRL_SUMMON_UPDATE_STATE,	//! ��ȯ�� ���� ������Ʈ
	NET_MSG_GCTRL_SUMMON_LANDEFFECT_UPDATE,	//! ��ȯ�� ���� ȿ�� ������Ʈ

	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE,	//! �� ��Ÿ�� ���� ��û
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_FB,	//! �� ��Ÿ�� ���� ���
	NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_BRD,	//! �� ��Ÿ�� ���� ���

	NET_MSG_REQ_MUST_LEAVE_MAP,	//! �������� ������ �����ʾ� ���� ������ �� ��
	NET_MSG_REQ_MUST_LEAVE_MAP_FB,	//! �������� ������ �����ʾ� ���� ������ �� ��
	NET_MSG_REQ_MUST_LEAVE_MAP_AG,	//! �������� ������ �����ʾ� ���� ������ �� ��

	NET_MSG_REQ_EXPEDITION_LEAVE_MAP, //! ������ ������ ���� �ʾ� ���� ������ �� ��
	NET_MSG_REQ_EXPEDITION_LEAVE_MAP_FB, //! ������ ������ ���� �ʾ� ���� ������ �� ��
	NET_MSG_REQ_EXPEDITION_LEAVE_MAP_AG, //! ������ ������ ���� �ʾ� ���� ������ �� ��

	NET_MSG_CYBERCAFECLASS_UPDATE,	//! �±� ���̹� ī�� ��� ������Ʈ
	
	NET_MSG_GCTRL_REQ_TAXI,	//! �ý� ž�� ��û
	NET_MSG_GCTRL_REQ_TAXI_FB,	//! �ý� ž�� ��û ���
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS,	//! �ý� ž�½� NPC��ǥ ��û
	NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB,	//! �ý� ž�½� NPC��ǥ ��û ���
	
	NET_MSG_GCTRL_ETNRY_FAILED,	//! ���� ���н� �޽���
	
	NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE,	//! ��Ÿ�� ������Ʈ
	NET_MSG_GCTRL_ITEM_COOLTIME_ERROR,	//! ��Ÿ�ӿ� �ɷ��� ������

	NET_MSG_GCTRL_NPC_RECALL,	//! NPC ȣ��.
	NET_MSG_GCTRL_NPC_RECALL_FB,	//! NPC ȣ�� ���.

	NET_MSG_GCTRL_NPC_COMMISSION,	//! NPC ȣ�� COMMISION ������Ʈ ��û ( Field->Agent ) 
	NET_MSG_GCTRL_NPC_COMMISSION_FB,	//! NPC ȣ�� COMMISION ��� ( Agent -> Field ) 

	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FLD,	//! �ν��Ͻ� �� ���� AGENT -> FIELD
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB,	//! �ν��Ͻ� �� ���� ��û �ǵ��
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_REQ,	//! �ν��Ͻ� �� ��û CLIENT -> FIELD -> AGENT
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_NOGATE_REQ, //! �ν��Ͻ� �� ��û CLIENT -> FIELD -> AGENT
	NET_MSG_GCTRL_CREATE_INSTANT_MAP_DEL,	//! �ν��Ͻ� �� ���� 
	//NET_MSG_GM_TESTCREATE_INSTANT_MAP_REQ,	//! �ν��Ͻ� �� ���� �׽�Ʈ ��û CLIENT -> FIELD -> AGENT

	NET_MSG_GM_SHOP_INFO_REQ,	//! ������ ���� ��û(GM��ɾ�)
	NET_MSG_GM_SHOP_INFO_FB,	//! ������ ���� ����(GM��ɾ�)

	NET_MSG_GCTRL_INVEN_ITEM_MIX,	//! ������ ���� ��û
	NET_MSG_INVEN_ITEM_MIX_FC,	//! ������ ���� ��û ���

	NET_MSG_GCTRL_LIMITTIME_OVER,	//! ���� ���н� �޽���

	NET_MSG_LOGIN_MAINTENANCE_SESSION,	// �������� LoginMaintenance ���� On/Off
	NET_MSG_LOGIN_MAINTENANCE_LOGIN,	// �α������� LoginMaintenance ���� On/Off
	
	NET_MSG_REQ_GATHERING,	//! ä�� ��û
	NET_MSG_REQ_GATHERING_FB,	//! ä�� ��û ���
	NET_MSG_REQ_GATHERING_BRD,	//! ä�� ��û ���
	NET_MSG_REQ_GATHERING_RESULT,	//! ä�� ���
	NET_MSG_REQ_GATHERING_RESULT_BRD,	//! ä�� ���
	NET_MSG_REQ_GATHERING_CANCEL,	//! ä���� ����
	NET_MSG_REQ_GATHERING_CANCEL_BRD,	//! ä���� ����

	NET_MSG_GCTRL_UPDATE_CP,	//!	CP ������Ʈ 

	NET_MSG_GCTRL_INSTANTMAP_MOVEOUT_REQ,	//!	�ν���Ʈ�� ������ ���� �̵�

	NET_MSG_INSTANT_MAP_REENTRY_CHECK_REQ,	//!	�ν���Ʈ�� ������ �ð� ��û
	NET_MSG_INSTANT_MAP_REENTRY_CHECK_FB,	//!	�ν���Ʈ�� ������ �ð� �˸�

	NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD,	//!	CDM �����ð� �˸� BRD
	
	NET_MSG_GCTRL_REQ_NPCTALK_BASIC,	//!	NPC Talk ������û
	NET_MSG_GCTRL_REQ_NPCTALK_BASIC_FB,	//!	NPC Talk ������û FB

    NET_MSG_CHAT_PARTY_RECRUIT_FB_AF,	// ��Ƽ����ä�� �޼��� FB
	NET_MSG_CHAT_AREA_FB,	// ����ä�� �޼��� FB
	NET_MSG_CHAT_PARTY_RECRUIT_FB,	// ��Ƽ����ä�� �޼��� FB
	NET_MSG_CHAT_FACTION_FB,	// ����ä�� �޼��� FB
	NET_MSG_GCTRL_PARTY_LINK_JOIN,	// ��Ƽ����ä�� ���� ��û
	NET_MSG_GCTRL_PARTY_LINK_JOIN_FB,	// ��Ƽ����ä�� ���� ��û FB
	NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS,	// ��Ƽ����ä�� ���� ��û ���� ( ���� -> ������ ) 
	NET_MSG_GCTRL_PARTY_LINK_LURE,	// ��Ƽ����ä�� ����
	NET_MSG_GCTRL_PARTY_LINK_LURE_FB,	// ��Ƽ����ä�� ���� FB
	NET_MSG_GCTRL_PARTY_LINK_REFUSE,	// ��Ƽ����ä�� ����
	NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB,	// ��Ƽ����ä�� ���� ���� ( ���� -> ��û�� ) 	


	NET_MSG_GM_CHECKINSTANTNUM,	// �δ� ���� ���� üũ �޽���
	NET_MSG_GM_CHECKINSTANTNUM_FB,	// �δ� ���� ���� üũ �޽��� FB

    NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE, // ��ȯ�� ���� �̺�Ʈ ���� (1635 ~ 1644 ���� �ӽ�)
    NET_MSG_GCTRL_LANDEVENT_REMOVE, // �����̺�Ʈ ����

	NET_MSG_GCTRL_ATTACK_BLOCKING, // ���� ���ŷ �޼���
	NET_MSG_GCTRL_ATTACK_BLOCKING_BRD, // ���� ���ŷ �޼��� �˸�
	NET_MSG_GCTRL_CROW_ATTACK_BLOCKING, // �� ���ŷ �޼��� �˸�
	NET_MSG_GCTRL_SUMMON_ATTACK_BLOCKING, // ��ȯ�� ���ŷ �޼��� �˸�


	NET_MSG_GCTRL_WITH_VEHICLE,   //! ������� ����Ÿ��
	NET_MSG_GCTRL_WITH_VEHICLE_TAR,   //! ������� ����Ÿ�� �����
	NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS,   //! ������� ����Ÿ�� ANS
	NET_MSG_GCTRL_WITH_VEHICLE_FB,   //! ������� ����Ÿ�� FB
	NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_FB,	//! ���ڸ� ž�� ���� �޼���
	NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_BRD,	//! ���� Ŭ���̾�Ʈ�� Ż���� Ȱ��/��Ȱ�� �˸�
	NET_MSG_GCTRL_WITH_VEHICLE_MOVE_MAP,	//! ���̵��� ž�� ĳ���� �� �̵� ��û
	NET_MSG_GCTRL_VEHICLE_BRD,	// ����ũ���� DropChar���� �и�
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE,	// Ż�� �÷� ����
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_FB,	// Ż�� �÷� ���� FB
	NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_BRD,	// Ż�� �÷� ���� BRD
	NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE,	// ����ũ ������
	NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE_FB,	// ����ũ ������
	NET_MSG_GCTRL_LEARN_BIKE_BOOST,	// ����ũ �ν�Ʈ ����
	NET_MSG_GCTRL_LEARN_BIKE_BOOST_FB,	// ����ũ �ν�Ʈ ���� FB
	NET_MSG_GCTRL_LEARN_BIKE_BOOST_BRD,	// ����ũ �ν�Ʈ ���� BRD
	NET_MSG_GCTRL_USE_BIKE_BOOST,	// ����ũ �ν�Ʈ ���
	NET_MSG_GCTRL_USE_BIKE_BOOST_FB,	// ����ũ �ν�Ʈ ��� FB
	NET_MSG_GCTRL_USE_BIKE_BOOST_BRD,	// ����ũ �ν�Ʈ ��� BRD
	NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_FB,	// ����ũ �ν�Ʈ ���� FB
	NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_BRD,	// ����ũ �ν�Ʈ ���� BRD

	NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX, // �κ� ���ö� ���
	NET_MSG_GCTRL_REQ_INVEN_LUNCHBOX_FB, // �κ� ���ö� ��� FB
	NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_CF,	// ������ ���� ��û;
	NET_MSG_GCTRL_REQ_INVEN_DECOMPOSE_FB,	// ������ ���� ���;
	
	NET_MSG_GCTRL_TRADE_LOCK,	//! Ʈ���̵� ��.
	NET_MSG_GCTRL_TRADE_LOCK_TAR,	//!Ʈ���̵� ��. field -> agent.

	NET_MSG_GM_NONCONFRONT_MODE,	// ��� �Ұ��� ��� GM ��ɾ�
	NET_MSG_GM_NONCONFRONT_MODE_VIEW_REQ,	// ��� �Ұ��� ��� �� ���� ��û
	NET_MSG_GM_NONCONFRONT_MODE_VIEW_FB,	// ��� �Ұ��� ��� �� ���� ����

	NET_MSG_GCTRL_POSTBOX_OPEN, // ������ ���� ��û
	NET_MSG_GCTRL_POSTBOX_OPEN_FB, // ������ ���� ��û FB
	NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD, // ������ ���� ��û (ī��)
	NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD_FB, // ������ ���� ��û FB (ī��)
	NET_MSG_GCTRL_POSTBOX_CLOSE, // ������ �ݱ� �˸�

    NET_MSG_GM_GAME_DATA_UPDATE_CA,
    NET_MSG_GM_GAME_DATA_UPDATE_AS,
    NET_MSG_GM_GAME_DATA_UPDATE_SS,

	// �ڽ�Ƭ ���� ����. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE,	// �ڽ�Ƭ �÷� ����. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_FB,	// �ڽ�Ƭ �÷� ���� FB. by luxes.
	NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_BRD,	// ����ũ�÷� ���� BRD


	NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD,	// ��ȿ�� BRD
	NET_MSG_GCTRL_SUMMON_MAPEFFECT_UPDATE,	// ��ȿ�� BRD

    NET_MSG_GM_LOG_ITEM_RELOAD_CA,
    NET_MSG_GM_LOG_ITEM_RELOAD_AS,
    NET_MSG_GM_LOG_ITEM_RELOAD_SS,

    NET_MSG_GCTRL_REQ_ITEM_SORT,   // Cleint -> Filed�� ������ ���� ��û.
	NET_MSG_GCTRL_REQ_ITEM_SORT_START,   // Cleint -> Filed�� ������ ���� ��û ����. ������ NET_MSG_GCTRL_REQ_ITEM_SORT �� �и�
	NET_MSG_GCTRL_REQ_ITEM_SORT_DATA,   // Cleint -> Filed�� ������ ���� ��û ����. ������ NET_MSG_GCTRL_REQ_ITEM_SORT �� �и�
	NET_MSG_GCTRL_REQ_ITEM_SORT_END,	// Cleint -> Filed�� ������ ���� ��û ����. ������ NET_MSG_GCTRL_REQ_ITEM_SORT �� �и�
    NET_MSG_GCTRL_ITEM_SORT_FAIL,   // Filed -> Client�� ������ ���� ���� �˸�.

    NET_MSG_GM_WHISPER_STATE,   // Client -> Agent�� �ӼӸ� ��� ���� ���� ��û.
    NET_MSG_GM_WHISPER_STATE_FB,   // Agent  -> Client�� �ӼӸ� ��� ���� ���� �˸�.

    NET_MSG_GCTRL_REQ_SKILL_REVIVE,   // ��Ȱ����� ��Ȱ ��û


	//PVE ������ ���
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_CF, // �������� �н� ��� CF
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF, // �������� �н� ��� CF
	NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_FB, // �������� �н� ��� FB

	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_CF, // �δ� ����Ƚ�� ������
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FA,
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_AF,
	NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_FC,

    NET_MSG_GCTRL_INVEN_CONSIGNEMENT_SLOT_EXPIRATION_REQ, // ��Ź�Ǹ� ���� Ȯ�� ��û(ī�������)


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
	NET_MSG_ATTENDANCE_DONE_POINTITEM_CF,	// PointShop ��ٱ��Ͽ� �⼮�� �������� �䱸�� ��ŭ�� �������� �־���.
	NET_MSG_ATTENDANCE_DONE_POINTITEM_FB,	// PointShop ��ٱ��Ͽ� �⼮�� �������� �䱸�� ��ŭ�� �������� �־���.�� ���� ����
	NET_MSG_ATTENDANCE_WHISPER_DONE_AF,	// Whisper msg has been sent to the target client via Agent
	NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA,	// GM requests to do daily-maintenance manually
	NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB,	// Response of Manual AttendanceMaintenance
	NET_MSG_ATTENDANCE_LOAD_END,

    
	// Messages for rebuy list.
    // AgentS
    NET_MSG_ADD_REBUY_ITEM_FA, // ����� ����Ʈ�� �߰�. field -> agent.
    NET_MSG_ADD_REBUY_ITEM_AC, // ����� ����Ʈ�� �߰� ���. agent -> client.
    NET_MSG_BUY_REBUY_ITEM_CA, // ����� ����Ʈ���� ������ ����. client -> agent.
    NET_MSG_BUY_REBUY_ITEM_AF, // ����� ����Ʈ���� ������ ����. agent -> field.
    NET_MSG_BUY_REBUY_ITEM_FC, // ����� ����Ʈ���� ������ ���� ���. field -> client.
    NET_MSG_BUY_REBUY_ITEM_FA, // ����� ����Ʈ���� ������ ���� ���. field -> agent.
    NET_MSG_CHECK_REBUY_SUPPLIES, // NPC ����� ui���� �Ҹ�ǰ ������ üũ.
	

	//
	//mjeon.Activity
	//Messages for Activities
	//
	NET_MSG_ACTIVITY_COMPLETED_FA,		// Activity �Ϸ� �뺸(Agent������)
	NET_MSG_ACTIVITY_CLOSED_FA,			// Activity �Ϸ� �Ұ��� ����(������ ����)
	NET_MSG_ACTIVITY_COMPLETED_FF,		// Activity �Ϸ� �뺸(DbAction���� Field��)
	NET_MSG_ACTIVITY_NOTIFY_COMPLETED,	// Activity �Ϸ� (to ���� clients)
	NET_MSG_ACTIVITY_NOTIFY_CLOSED,		// Activity �Ϸ� �Ұ��� ����(������ ����) (to ���� clients)
	NET_MSG_ACTIVITY_UPDATE_VALUE,		// Activity �����Ȳ ������Ʈ (to ƯȰ�� ������ ����)
	NET_MSG_ACTIVITY_UPDATE_COMPLETED,	// Activity �Ϸ���·� ������Ʈ (to ƯȰ�� ������ ����)
	NET_MSG_ACTIVITY_POPUP_COMPLETE,	// Activity �Ϸ� �˾� (to viewaround)
	NET_MSG_ACTIVITY_DONE_ITEMTAKE,		// Activity ������ȹ�� ���� �޼���û
	NET_MSG_ACTIVITY_DONE_ITEMTAKE_FB,	// Activity ������ȹ�� ���� �޼�����
	NET_MSG_ACTIVITY_SYNC,				// Activity ��Ȳ�� DB(Server)�� ����ȭ (���� ���ӽ�)
	NET_MSG_ACTIVITY_REWARD_POINT_SYNC,	// Activity point ����ȭ
	NET_MSG_ACTIVITY_REWARD_BADGE_SYNC,	// Activity badge ����ȭ
	NET_MSG_ACTIVITY_CHANGE_BADGE,		// Activity badge ����
	NET_MSG_ACTIVITY_CHANGE_BADGE_FB,	// Activity badge ��������
	NET_MSG_ACTIVITY_LOAD_COMPLETED_FF,	// ActivityLoad() completed
	NET_MSG_TITLE_NOTIFY_CHANGE,	// Title ���� �˸� (to viewaround)


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

	NET_MSG_GM_RANMOBILE_COMMAND_REQUEST, // ������� �׽�Ʈ ��ɾ�
	NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE, // ������� �׽�Ʈ ��ɾ�

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
	NET_MSG_GCHAT_ERRORCODE,		//���� GroupChat���� errorcode ���޿�
	NET_MSG_GCHAT_QUIT_CA,
	NET_MSG_GCHAT_KICK_CA,			//Kick out a player from the chat
	NET_MSG_GCHAT_MEMBER_SYNC_AC,	//Agent -> All members in the room
	NET_MSG_GCHAT_MEMBER_UPDATE_AC,	//Agent -> All members in the room (Join or Quit)
	NET_MSG_GCHAT_MESSAGE,			//chat messages	

	NET_MSG_GCHAT_CONFIG_AC,		//groupchat ���� ���� �� Client�� ���޹��� ���������� �����Ѵ�



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

    NET_MSG_GAME_JOIN_AH, //! ���� ����
    NET_MSG_CHAR_CLOSE_AH, //! ���� ���� ����

    NET_MSG_CHA_LIST_AH, //! Agent->Cache:ĳ���� ����Ʈ ��û

	// ����ġ ���� / ĸ��
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

    //! Friend Group/ģ�� �׷� -----------------------------------------------
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

    //! Notify/�˸� -----------------------------------------------------------
    NET_MSG_NOTIFY_REQ_CA,
    NET_MSG_NOTIFY_DATA_AC,
    NET_MSG_NOTIFY_DATA_AC_END,

    //! Club ------------------------------------------------------------------
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ,     //! Ŭ�������� ���� ����
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK, //! Ŭ�������� ���� ���� ����
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ANS, //! Ŭ�������� ���� ���� �亯
    NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB,  //! Ŭ�������� ���� ���� ���
    NET_MSG_GCTRL_CLUB_AUTHORITY_CLT,	  //! Ŭ�������� ���� ����
    NET_MSG_GCTRL_CLUB_AUTHORITY_BRD,	  //! Ŭ�������� ���� ����
    NET_MSG_CLUB_MASTER_CHANGE_AF,	      //! Ŭ�������� ���� ���� (�ʵ�)

    NET_MSG_GCTRL_DUPLICATE_CLOSEGAME,	//!	�ߺ��α������� ������ �����ؾ� �ϴ� ���

    NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD,	//!	Ŭ��������ġ �����ð�.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_READY_FLD,	//!	Ŭ��������ġ �غ�.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_START_FLD,	//!	Ŭ��������ġ ����.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD,	//!	Ŭ��������ġ ����.

    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_FLD,	//!	Ŭ��������ġ ����.
    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_AGT,	//!	Ŭ��������ġ ����. ( Fiedl ��� -> AGT )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD,	//!	Ŭ��������ġ ����.

    NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO,	//!	Ŭ��������ġ ���� ���� ( ���ӽ� ) 
    NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG,	//!	Ŭ��������ġ ���� ���� ( Agent�� �����ð� ��û )

    NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE,	//!	Ŭ��������ġ Kill/Death ��Ȳ ( �ڱ⸸ )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE,	//!	Ŭ��������ġ Kill/Death ��Ȳ ( �ڱ⸸ )
    NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE,	//!	Ŭ��������ġ ��ŷ ��Ȳ
    NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_REQ,	//!	Ŭ��������ġ ��Ȳ ��û
	NET_MSG_GCTRL_CLUB_DEATHMATCH_ON_OFF_CA,	//! Ŭ��������ġ ON OFF ���� ���

    NET_MSG_WB_CLUB_BATTLE_SET_FLAG_FAC_BRD, //! �������� Flag ����

    NET_MSG_CLUB_COUNTRY_COUNT_CA,
    NET_MSG_CLUB_COUNTRY_COUNT_START_AC,
    NET_MSG_CLUB_COUNTRY_COUNT_AC,
    NET_MSG_CLUB_COUNTRY_COUNT_END_AC,
    NET_MSG_CLUB_COUNTRY_CA,
    //NET_MSG_CLUB_CHAR_LEVEL_CHANGE_AC,
    NET_MSG_REQ_DROP_CHAR_CLUB_NAME_CA,
    NET_MSG_UPDATE_DROP_CHAR_CLUB_NAME_AC,

    NET_MSG_LOBBY_CLUB_INFO,    //! Ŭ�� ����
    NET_MSG_LOBBY_CLUB_INFO_EX, //! Ŭ�� ����
    //NET_MSG_LOBBY_CLUB_MEMBER,  //! Ŭ�� ���
    NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK, //!	Ŭ�� ���.
    NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK_END,
    NET_MSG_LOBBY_CLUB_ALLIANCE, //!	Ŭ�� ����.
    NET_MSG_LOBBY_CLUB_BATTLE,  //!	Ŭ�� ��Ʋ.
    NET_MSG_LOBBY_CLUB_INFO_END, 

    NET_MSG_GCTRL_CLUB_NEW,	//!	Ŭ�� ����.
    NET_MSG_CLUB_NEW_LOBBY_CA, //! Ŭ�� ���� Lobby
    NET_MSG_GCTRL_CLUB_NEW_2AGT,	//!	Ŭ�� ���� to AGENT.
    NET_MSG_GCTRL_CLUB_NEW_DB2AGT,	//!	Ŭ�� ���� DB to AGENT.
    NET_MSG_GCTRL_CLUB_NEW_2FLD,	//!	Ŭ�� ���� to FIELD.
    NET_MSG_GCTRL_CLUB_NEW_FB,	//!	Ŭ�� ���� FB.

    NET_MSG_GCTRL_CLUB_DISSOLUTION,	//!	Ŭ�� ��ü ��û.
    NET_MSG_GCTRL_CLUB_DISSOLUTION_2FLD,	//!	Ŭ�� ��ü ��û.
    NET_MSG_GCTRL_CLUB_DISSOLUTION_FB,	//!	Ŭ�� ��ü ��û FB.
    NET_MSG_GCTRL_CLUB_DEL_2FLD,	//!	Ŭ�� ��ü to FIELD.
    NET_MSG_GCTRL_CLUB_DEL_2CLT,	//!	Ŭ�� ���� to CLIENT. ( ��ü �� Ż�� )
    NET_MSG_GCTRL_CLUB_DEL_BRD,	//!	Ŭ�� ���� BRD. ( ��ü �� Ż�� )

    NET_MSG_GCTRL_CLUB_INFO_2FLD,	//!	Ŭ�� ���� to FLIED.
    NET_MSG_GCTRL_CLUB_INFO_2CLT,	//!	Ŭ�� ���� to CLIENT.
    NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION,	//!	Ŭ�� ��ü���� ���� to CLIENT.
    NET_MSG_GCTRL_CLUB_MEMBER_2CLT,	//!	Ŭ�� ��� to CLIENT.

    NET_MSG_GCTRL_CLUB_INFO_BRD,	//!	Ŭ�� ���� BRD.
    NET_MSG_GCTRL_CLUB_INFO_MARK_BRD,	//!	Ŭ�� ��ũ ���� BRD.
    NET_MSG_GCTRL_CLUB_INFO_NICK_BRD,	//!	Ŭ�� �г��� ���� BRD.

    NET_MSG_GCTRL_CLUB_MARK_INFO,	//!	Ŭ�� ��ũ ��û.
    NET_MSG_GCTRL_CLUB_MARK_INFO_FB,	//!	Ŭ�� ��ũ ��ȯ.

    NET_MSG_GCTRL_CLUB_MARK_CHANGE,      //! Ŭ�� ��ũ ���� ��û
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_2FLD, //! Ŭ�� ��ũ ���� ��û
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT, //! Ŭ�� ��ũ ���� ��û
    NET_MSG_GCTRL_CLUB_MARK_CHANGE_FB,   //! Ŭ�� ��ũ ���� FB

    NET_MSG_CLUB_RANK_UP_CF,        //! Ŭ�� ��ũ ����
    NET_MSG_CLUB_RANK_CHANGE_FA,    //! Ŭ�� ��ũ ����
    NET_MSG_CLUB_RANK_CHANGE_AF,    //! Ŭ�� ��ũ ����
    NET_MSG_CLUB_RANK_CHANGE_FC,    //! Ŭ�� ��ũ ����
    NET_MSG_GCTRL_CLUB_RANK_FB,     //! Ŭ�� ��ũ ���� FB

    NET_MSG_CLUB_MENBER_LOBBY_CA,	//!	Ŭ�� ���� ��û, Lobby
    NET_MSG_CLUB_MEMBER_REQ_CA, 	//!	Ŭ�� ���� ��û.
    NET_MSG_CLUB_MEMBER_REQ_ASK_AC,	//!	Ŭ�� ���� ����
    NET_MSG_CLUB_MEMBER_REQ_ANS_CA,	//!	Ŭ�� ���� �亯
    NET_MSG_GCTRL_CLUB_MEMBER_REQ_2AGT,	//!	Ŭ�� ���� �亯
    NET_MSG_CLUB_MEMBER_REQ_FB_AC,	//!	Ŭ�� ���� ��û FB

    NET_MSG_GCTRL_CLUB_MEMBER_ADD_2FLD,	//!	Ŭ�� ��� �߰� to FIELD.

    NET_MSG_CLUB_MEMBER_DEL_CA,         //! Ŭ�� ��� ���� ��û.
    NET_MSG_CLUB_MEMBER_DEL_AF,         //! Ŭ�� ��� ���� ��û to FIELD.
    NET_MSG_CLUB_MEMBER_DEL_FC,         //! Ŭ�� ��� ���� ��û ��� to CLIENT.
    NET_MSG_CLUB_MEMBER_DEL_AC,         //! Ŭ�� ��� ���� ��û ��� FB.

    NET_MSG_GCTRL_CLUB_MEMBER_NICK,	//!	Ŭ�� ��� ����.
    NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB,	//!	Ŭ�� ��� ���� FB.

    //NET_MSG_GCTRL_CLUB_MEMBER_STATE,	//!	Ŭ�� ��� ����.
    //NET_MSG_GCTRL_CLUB_MEMBER_POS,	//!	Ŭ�� ��� ��ġ.

    NET_MSG_GCTRL_CLUB_MEMBER_SECEDE,	//!	Ŭ�� ��� Ż��.
    NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB,	//!	Ŭ�� ��� Ż�� FB.

    NET_MSG_GCTRL_CLUB_MEMBER_DB2DEL,	//!	Ŭ�� ��� ���� ��û.

    //������ ����
    NET_MSG_GCTRL_SERVER_CLUB_GUID_BATTLE_INFO,         // ������ ���� ����
    NET_MSG_GCTRL_REQ_SERVER_CLUB_GUID_BATTLE_REMAIN_AG,// ������ �����ð� ��û(Field->Agent->Client)
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_REMAIN_BRD,	        // ������ �����ð� ����
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_FLD,	        // ���� Ŭ�� ������ ����.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_START_BRD,	        // ���� Ŭ�� ������ ����.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_FLD,	            // ���� Ŭ�� ������ ����.
    NET_MSG_GCTRL_CLUB_GUID_BATTLE_END_BRD,	            // ���� Ŭ�� ������ ����.

    NET_MSG_GCTRL_CLUB_CD_CERTIFY,	       //! cd ����
    NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB,	   //! cd FB
    NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD,	   //! cd ���� BRD
    NET_MSG_CLUB_CD_CERTIFY_ING_BRD, //! cd ���� ��� BRD

    NET_MSG_CLUB_CERTIFIED_FA,	//!	cd ���� �Ϸ� ( agent�� �˸� )
    NET_MSG_CLUB_CERTIFIED_AF,	//!	cd ���� �Ϸ� ( field�� �˸� )
    //NET_MSG_GCTRL_CLUB_CERTIFIED_CLT,	//!	cd ���� �Ϸ� ( field�� �˸� )

    NET_MSG_GCTRL_CLUB_COMMISSION,	//!	���� ���� ������ ����.
    NET_MSG_GCTRL_CLUB_COMMISSION_FB,	//!	���� ���� ������ ���� FB.
    NET_MSG_GCTRL_CLUB_COMMISSION_FLD,	//!	���� ���� ������ ���� FLD.
    NET_MSG_GCTRL_CLUB_COMMISSION_BRD,	//!	���� ���� ������ ���� BRD.
    NET_MSG_GCTRL_CLUB_INCOME_MONEY_AGT,	//!	Ŭ���� ���� �߻� AGT.

    NET_MSG_CLUB_GETSTORAGE_CA,	        //! â�� ���� ��û.
    NET_MSG_GCTRL_CLUB_GETSTORAGE,      //! â�� ���� ��û.
    NET_MSG_GCTRL_CLUB_GETSTORAGE_FB,	//!	â�� ���� ��û.
    NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM,	//!	â�� ���� ȸ��.

    NET_MSG_CLUB_STORAGE_TO_INVEN_CA,	//!	â�� ������ ���ø���.
    NET_MSG_CLUB_STORAGE_TO_INVEN_AF,	//!	â�� ������ ���ø���.
    NET_MSG_CLUB_STORAGE_RESTORE_FA,	//!	â�� ������ ����
    NET_MSG_CLUB_ITEM_EXCHANGE_TO_INVEN_CA,	//! â�� �����۰� �κ��� ����ִ� ������ ��ȯ
    NET_MSG_CLUB_STORAGE_EX_CLUB_STORAGE_CA,	//! â�� �����۰� â�� ����ִ� ������ ��ȯ
    NET_MSG_CLUB_STORAGE_TO_CLUB_STORAGE_CA,	//! â�� �������� â��� �ű�.
    NET_MSG_CLUB_STORAGE_EX_INVEN_AF,	//!	â�� �����۰� �տ� �鸰 ������ ��ȯ.
    NET_MSG_CLUB_STORAGE_EX_INVEN_FA,	//!	â�� �����۰� �տ� �鸰 ������ ��ȯ.
    NET_MSG_CLUB_INVEN_TO_STORAGE_CF,	//!	�տ� �鸰 ������ â�� �ֱ�.
    NET_MSG_CLUB_INVEN_TO_STORAGE_FA,	//!	�տ� �鸰 ������ â�� �ֱ�.
    NET_MSG_CLUB_HOLD_TO_STORAGE_AF,	//!	�տ� �鸰 ������ â�� �ֱ� ����/����
    //NET_MSG_GCTRL_CLUB_STORAGE_SPLIT,	//!	â���� ������ �����ϱ�.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_CF,	//!	���� ����.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_FA,	//!	���� ����.
    NET_MSG_CLUB_STORAGE_SAVE_MONEY_AF,	//!	���� ����.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_CA,	//!	���� ����.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_AF,	//!	���� ����.
    NET_MSG_CLUB_STORAGE_DRAW_MONEY_FA,	//!	���� ����.

    NET_MSG_GCTRL_CLUB_STORAGE_INSERT,	//!	â�� �ֱ�.
    NET_MSG_GCTRL_CLUB_STORAGE_DELETE,	//!	â���� ����.
    NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS,	//!	������ �ֱ� ���ÿ� �ϱ�.
    NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM,	//!	â�� �ִ� �������� ���� ����.
    NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY,	//!	â���� ����ݾ� ����.

    NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD,	//!	���� ���� ������ ���� ���� BRD.

    NET_MSG_GCTRL_CLUB_STORAGE_RESET,	//!	Ŭ�� â�� ������ ����.
    NET_MSG_CLUB_STORAGE_SEND_END,	//!	Ŭ�� â�� ����
    NET_MSG_GCTRL_CLUB_STORAGE_GET_DB,	//!	Ŭ�� â�� ������ db���� ����. ������ ���� ����
	NET_MSG_GCTRL_CLUB_STORAGE_GET_DB_END,	//!	Ŭ�� â�� ������ db���� ����. ��Ÿ ����(��) ����, �� ������ ������ ���� �Ϸ� �޽���

    NET_MSG_CLUB_INCOME_RENEW_CA,	//!	Ŭ�� ���ͱ� ���� ��û ( client->field ).
    //NET_MSG_GCTRL_CLUB_INCOME_UP,	//!	Ŭ�� ���ͱ� ���� ��û ( field->agent ).
    //NET_MSG_GCTRL_CLUB_INCOME_DN,	//!	Ŭ�� ���ͱ� ���� ȸ�� ( agent->field ).
    NET_MSG_GCTRL_CLUB_INCOME_FB,	//!	Ŭ�� ���ͱ� ���� ��� ( field->agent ).

    NET_MSG_GCTRL_CLUB_NOTICE_REQ,	//!	Ŭ�� ���� ���� ��û. ( client->agent )
    NET_MSG_GCTRL_CLUB_NOTICE_FB,	//!	Ŭ�� ���� ���� ���. ( agent->client )
    NET_MSG_GCTRL_CLUB_NOTICE_FLD,	//!	Ŭ�� ���� �ʵ忡 �˸�. ( agent->field )
    NET_MSG_GCTRL_CLUB_NOTICE_CLT,	//!	Ŭ�� ���� Ŭ���̾�Ʈ�� �˸�. ( field->client )

    //NET_MSG_GCTRL_CLUB_SUBMASTER,	     //! Ŭ�� �θ� ����.
    NET_MSG_GCTRL_CLUB_SUBMASTER_FB,	 //! Ŭ�� �θ� ���� FB.
    //NET_MSG_GCTRL_CLUB_SUBMASTER_BRD,	 //! Ŭ�� �θ� ���� �˸� CLT.
    //NET_MSG_GCTRL_CLUB_SUBMASTER_FLD,	 //! Ŭ�� �θ� ���� �˸� FLD.
    NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AC, //! Agent->Client CD ������ ����
    NET_MSG_CLUB_CD_CERTIFIER_CHANGE_AF, //! Agent->Field  CD ������ ����
    NET_MSG_CLUB_MASTER_CHNAGE_AC,       //! Agent->Client Club ������ ����

    NET_MSG_CLUB_ALLIANCE_REQ_CA,        //! Ŭ�� ���� ��û.
    NET_MSG_CLUB_ALLIANCE_REQ_ASK_AC,    //! Ŭ�� ���� ��û ASK.
    NET_MSG_CLUB_ALLIANCE_REQ_ANS_CA,    //! Ŭ�� ���� ��û ANS.
    NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB,	 //! Ŭ�� ���� ��û ��� FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_FLD, //! Ŭ�� ���� Ŭ�� ADD FLD.
    NET_MSG_CLUB_ALLIANCE_ADD_AC,        //! Ŭ�� ���� Ŭ�� ADD CLT.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_REQ, //! Ŭ�� ���� Ŭ�� ���� ��û.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB,	 //! Ŭ�� ���� Ŭ�� ���� ��û FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_REQ, //! Ŭ�� ���� Ż�� ��û.
    NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB,	 //! Ŭ�� ���� Ż�� ��û FB.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FLD, //! Ŭ�� ���� Ŭ�� DEL FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT, //! Ŭ�� ���� Ŭ�� DEL CLT.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_REQ, //! Ŭ�� ���� ��ü FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB,  //! Ŭ�� ���� ��ü FLD.

    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FLD, //! Ŭ�� ���� ��ü FLD.
    NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT, //! Ŭ�� ���� ��ü CLT.


    NET_MSG_GCTRL_CLUB_BATTLE_REQ,      //! Ŭ�� ��Ʋ ��û 
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK,	//! Ŭ�� ��Ʋ ��û ASK.
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_ANS,	//! Ŭ�� ��Ʋ ��û ANS.
    NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB,	//! Ŭ�� ��Ʋ ��û ��� FB.

    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_FLD,  //! Ŭ�� ��Ʋ ���� FLD.
    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT,  //! Ŭ�� ��Ʋ ���� CLT(���).
    NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2, //! Ŭ�� ��Ʋ ���� CLT2(��¥����).
    NET_MSG_GCTRL_CLUB_BATTLE_OVER_FLD,	  //! Ŭ�� ��Ʋ ���� FLD.
    NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT,	  //! Ŭ�� ��Ʋ ���� CLT.

    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ,     //! Ŭ�� ��Ʋ ���� ��û.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK, //! Ŭ�� ��Ʋ ���� ��û ASK.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ANS, //! Ŭ�� ��Ʋ ���� ��û ANS.
    NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB,	 //! Ŭ�� ��Ʋ ���� ��û ��� FB.

    NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ,    //! Ŭ�� ��Ʋ �׺� ��û.
    NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB, //! Ŭ�� ��Ʋ �׺� ��û ��� FB.

    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_AGT,	//! Ŭ�� ��Ʋ ų�� UPDATE Field -> Agent
    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE_FLD,	//! Ŭ�� ��Ʋ ų�� UPDATE Agent -> Field
    NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE,      //! Ŭ�� ��Ʋ ų�� UPDATE Ŭ���̾�Ʈ

    NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE,	//!	Ŭ�� ��Ʋ ų/������ ����

    NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_FLD,	//!	Ŭ�� ��Ʋ ����� Field���� ���� Agent -> Field
    NET_MSG_GCTRL_CLUB_BATTLE_LAST_KILL_UPDATE_AGT,	//!	Ŭ�� ��Ʋ ����� Field���� ���� ��� Field -> Agent


    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ,	//!	���� ��Ʋ ��û 
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK,	//!	���� ��Ʋ ��û ASK.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ANS,	//!	���� ��Ʋ ��û ANS.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB,	//!	���� ��Ʋ ��û ��� FB.

    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ,	//!	���� ��Ʋ ���� ��û.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK,	//!	���� ��Ʋ ���� ��û ASK.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ANS,	//!	���� ��Ʋ ���� ��û ANS.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB,	//!	���� ��Ʋ ���� ��û ��� FB.

    NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ,	//!	���� ��Ʋ �׺� ��û.
    NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB,	//!	���� ��Ʋ �׺� ��û ��� FB.

    NET_MSG_CLUB_AUTH_AF,
    NET_MSG_CLUB_AUTH_FC,

	NET_MSG_CLUB_MASTER_MANDATE_CA, //! Ŭ�� ������ ���� ON/OFF

    NET_MSG_CLUB_GRADE_FLAG_CHANGE_CA, //! Ŭ�� ���� Flag ���� Client->Agent
    NET_MSG_CLUB_GRADE_FLAG_CHANGE_AF, //! Ŭ�� ���� Flag ���� Agent->Client

    NET_MSG_CLUB_PULIC_FLAG_CHANGE_CA, //! Ŭ�� ���� Flag ���� Client->Agent
    NET_MSG_CLUB_PULIC_FLAG_CHANGE_AC, //! Ŭ�� ���� Flag ���� Agent->Client
    NET_MSG_CLUB_PULIC_FLAG_CHANGE_AF, //! Ŭ�� ���� Flag ���� Agent->Field
    NET_MSG_CLUB_AUTO_KICK_CA, //! Ŭ�� �ڵ� Ż�� ���� ����
    NET_MSG_CLUB_AUTO_KICK_AF, //! Ŭ�� �ڵ� Ż�� ���� ����
    NET_MSG_CLUB_AUTO_KICK_AC, //! Ŭ�� �ڵ� Ż�� ���� ����
    NET_MSG_CLUB_NEWBIE_NOTICE_CA, //! Ŭ�� ���Ի� �Ұ���
    NET_MSG_CLUB_NEWBIE_NOTICE_AF, //! Ŭ�� ���Ի� �Ұ���
    NET_MSG_CLUB_NEWBIE_NOTICE_AC, //! Ŭ�� ���Ի� �Ұ���
    
    NET_MSG_CLUB_LOG,        //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_CA,     //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_AH,     //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_HA,     //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_HA_END, //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_AC,     //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_AC_END, //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_DH,     //! Ŭ�� �α�
    NET_MSG_CLUB_LOG_DH_END, //! Ŭ�� �α�

    NET_MSG_CLUB_GRADE_NAME_CHANGE_CA, //! Ŭ�� ��� �̸�
    NET_MSG_CLUB_GRADE_NAME_CHANGE_AF, //! Ŭ�� ��� �̸�
    NET_MSG_CLUB_GRADE_NAME_CHANGE_AC, //! Ŭ�� ��� �̸�
    NET_MSG_CLUB_GRADE_NAME_CHANGE_RESULT_AC, //! Ŭ�� ��� �̸�

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

    NET_MSG_CHAR_POS_FA, //! Field->Agent ĳ���� ���� ��ġ �˸�

    NET_MSG_CHAR_ONLINE_AC, //! ĳ���� ���� �˸� online
    NET_MSG_CHAR_OFFLINE_AC, //! ĳ���� ���� �˸� offline
    NET_MSG_CHAR_POSITION_AC, //! ĳ���� ��ġ �˸�

    NET_MSG_PARTY_INFO_REQ_CA, //! ��Ƽ ���� ��û
    NET_MSG_PARTY_INFO_REQ_AC,

    NET_MSG_CHAR_DB_NUM_REQ_CA, //! ĳ���� �̸����� ĳ���� DB ��ȣ�� ��û
    NET_MSG_CHAR_DB_NUM_REQ_AC, //! ĳ���� �̸����� ĳ���� DB ��ȣ�� ��û ���
    NET_MSG_CHAR_DB_NUM_REQ_DA,  //! ĳ���� �̸����� ĳ���� DB ��ȣ�� ��û ���

    NET_MSG_FRIEND_STATE_REQ_CA, //! ĳ���� ģ������ ��û
    NET_MSG_FRIEND_STATE_REQ_AC, //! ĳ���� ģ������ ��û
    NET_MSG_FRIEND_STATE_REQ_DA, //! ĳ���� ģ������ ��û

    NET_MSG_GUIDANCE_MAP_INFO_CA, //! ����Ŭ�� �� ���� ��û.
    NET_MSG_GUIDANCE_MAP_INFO_AC,
    NET_MSG_GM_GUIDANCE_INFO_CA, //! GM�� ����Ŭ�� ���� ��û.
    NET_MSG_GM_GUIDANCE_INFO_AC,
    NET_MSG_GM_GUIDANCE_CHANGE_CA, //! ����Ŭ�� ���� ����
	NET_MSG_GM_GUIDANCE_ON_OFF_CA, //! ������ ON/OFF

    NET_MSG_OTHER_CHAR_PARTY_CHANGE_AC, //! ��Ƽ ���� ����

    NET_LOG_ATTENDANCE_DS, //! �⼮�� �α� DB->Server
    NET_LOG_ATTENDANCE_SH, //! �⼮�� �α� Server->Cache
    NET_LOG_ATTENDANCE_TASK_DS, //! �⼮�� �α� DB->Server
    NET_LOG_ATTENDANCE_TASK_SH, //! �⼮�� �α� DB->Server

	NET_MSG_CHA_SAVE_DF,	//! ĳ���� ���� �� ó�� DB->Field


	//! ���λ��� Private Market ----------------------------------------------------------

	NET_MSG_GCTRL_PMARKET_SELLER_REGISTER_FH,	//! ���λ��� �Ǹ��� ���� ���(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_REGISTER_FH,	//! ���λ��� �Ǹ� ������ ���(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_CF,	//! ���λ��� ������ �˻�(�⺻) ��û(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_BASIC_FH,	//! ���λ��� ������ �˻�(�⺻) ��û(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_NUM_HFC,	//! ���λ��� �˻� ��� ������ ����(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_HFC,	//! ���λ��� �˻� ���(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_END_HFC,	//! ���λ��� �˻� ��� ��(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_CF,	//! ���λ��� ������ ���� ��û(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_FH,	//! ���λ��� ������ ���� ��û(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_HF,	//! ���λ��� ������ ��� ����(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_FH,	//! ���λ��� ������ ��� ���� ���(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_PAY_MONEY_ROLLBACK_HF,	//! ���λ��� ������ ���Ŵ�� �ѹ�(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_HF,	//! ���λ��� ������ ���� ó��(Cache->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_ROLLBACK_FH,	//! ���λ��� ������ ���� ó�� �ѹ�(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_BUY_RESULT_FH,	//! ���λ��� �������� �������� �� ���� �ִ� ������ ���� ����(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_FH,	//! ���λ��� Ȧ����������� ����(Field->Cache)
	NET_MSG_GCTRL_PMARKET_ITEM_IS_HOLD_HF,	//! ���λ��� Ȧ����������� ���� ����(Cache->Field)
	NET_MSG_GCTRL_PMARKET_COMMON_FB,	//! ���λ��� �Ϲ� FB (Server->Client)
	NET_MSG_GCTRL_PMARKET_LOG_CF,	//! ���λ��� �ŷ����� ��û(Client->field)
	NET_MSG_GCTRL_PMARKET_LOG_FH,	//! ���λ��� �ŷ����� ��û(field->Cache)
	NET_MSG_GCTRL_PMARKET_LOG_NUM_HFC,	//! ���λ��� �ŷ����� ����(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_LOG_HFC,	//! ���λ��� �ŷ�����(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_LOG_END_HFC,	//! ���λ��� �ŷ����� ��(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_STATE_CF,	//! ���λ��� ���� ���� ���� ��û(Client->field)
	NET_MSG_GCTRL_PMARKET_STATE_FH,	//! ���λ��� ���� ���� ���� ��û(field->Cache)
	NET_MSG_GCTRL_PMARKET_STATE_HFC,	//! ���λ��� ���� ���� ���� ����(Cache->field->Client)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_CF,	//! ���λ��� �˻� ��û(������ �̸�����)(Client->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_SEARCH_NAME_AH,	//! ���λ��� �˻� ��û(������ �̸�����)(Agent->Cache)
	NET_MSG_GCTRL_PMARKET_OPTION_CA,	//! ���λ��� ���� ���� ���� ��û(Client->Agent)
	NET_MSG_GCTRL_PMARKET_OPTION_FH,	//! ���λ��� ���� ���� ���� ��û(Field->Cache)
	NET_MSG_GCTRL_PMARKET_OPTION_AF,	//! ���λ��� ���� ��û(Agent->Field)
	NET_MSG_GCTRL_PMARKET_ITEM_NUM_TOTAL_HFC,	//! ���λ��� �˻� ��� �� ������ ����(Cache->Field->Client)
	NET_MSG_GCTRL_PMARKET_CLOSE_FH,	//! ���λ��� �ݴ´�.(Field->Cache)
	NET_MSG_GCTRL_PMARKET_POST_SEND_HF,	//! ���λ��� ������ ���� ���� ��û, �������� �ʵ�� ���� ���� ���� �Ѵ�.(Cache->Field)

    // Product (����) Ÿ��
    // ���� �޼���
    NET_MSG_REQ_LEARN_PRODUCTBOOK,	    //! ���ռ� ���� ��û
    NET_MSG_REQ_LEARN_PRODUCTBOOK_FB,	//! ���ռ� ���� ��û ��� �뺸
    NET_MSG_PRODUCT_REQ,	            //! ������ ���� ��û
    NET_MSG_PRODUCT_FB,	                //! ������ ���� ��û ���
	NET_MSG_PRODUCT_FA,					//! ������ ���� ��û ��� ( FA );
	NET_MSG_PRODUCT_EXCHANGE_LOG_FA,	//! ������ Exchange Log;
    
    NET_MSG_PRODUCTBOOK,                //! ���ռ� ���� ��� ����
    // cwback
    //! REQ = Request     (��û)     
    //! ACK = Acknowledge (����)
    //�ű� �޼���
    NET_MSG_REQ_PRODUCTBOOK,            //! ���ռ� ���� ��� ��û

    NET_MSG_PRODUCT_TYPE_LIST_REQ,      //! ���� �о� ���, ���պо� ����Ʈ ��û
    NET_MSG_PRODUCT_TYPE_LIST_ACK,      //! ���� �о� ���, ���պо� ����Ʈ ����

    NET_MSG_PRODUCT_TYPE_ADD_REQ,       //! ���� �о� �߰� ��û
    NET_MSG_PRODUCT_TYPE_ADD_ACK,       //! ���� �о� �߰� ����

    NET_MSG_PRODUCT_TYPE_DEL_REQ,       //! ���� �о� ���� ��û
    NET_MSG_PRODUCT_TYPE_DEL_ACK,       //! ���� �о� ���� ��û

    NET_MSG_PRODUCT_ISGETEXP_LIST_REQ,  //! �������� ���õ��� ������ ��� ��û
    NET_MSG_PRODUCT_ISGETEXP_LIST_ACK,  //! �������� ���õ��� ������ ��� ����

	NET_MSG_CATCH_DELAY,	// �ʸ��ɶ�Ŀ���� ����(���� �ڵ� ����) Server->Client
	NET_MSG_GM_MSG_DELAY_MODIFY,	// ������ �޽��� ����

	NET_MSG_CHA_SAVE_FA, // �ʵ弭���� ĳ���� �����Ѵٰ� ������Ʈ�� �˸�(������ ���� ���)
	NET_MSG_CHA_SAVE_END_FA, // �ʵ弭���� ĳ���� ���峡���ٰ� ������Ʈ�� �˸�(������ ���� ���)

	NET_MSG_CHA_VALIDATION_FA,

    //! ������ ����
    NET_MSG_ITEM_REPAIR_REQ,            //! ������ ����
    NET_MSG_ITEM_REPAIR_ACK,            //!
    NET_MSG_ITEM_REPAIR_CARD_REQ,       //! ������ ���� â ����
    NET_MSG_ITEM_REPAIR_CARD_ACK,       //! 
    

    // PVE �޽���
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

    // Char ���� �޽���
    NET_MSG_CHAR_STATE_FC,

	// reference count ���� ��û(GM �޽���)
	NET_MSG_REFERENCE_COUNT_CAF,

    // ipevent ����.(GM �޼���)
    NET_MSG_GM_LOAD_IPEVENT,
    NET_MSG_GM_SET_IPEVENT,

    // ����ġ ��� ����.
    NET_MSG_GM_ACQUIRE_EXP,
    NET_MSG_GM_ACQUIRE_EXP_ANS,
    NET_MSG_GCTRL_ACQUIRE_EXP,

	//RNATTENDANCE RN �⼮�� �޽���
	NET_MSG_RNATTENDANCE_QUEST_LIST_FC,
	NET_MSG_GCTRL_REQ_REQ_RNATTENDANCE_QUEST_START,

	/// Lotto System Message - ��Ŷ ����͸��� ���Ͽ� ��� �Ѱ��� ����;
	// Client -> Field -> Agent;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_CF,	// �ζ� ���� ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA,	// �ζ� ���� ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_CF,	// Ȯ�ξȵ� ���Ÿ���Ʈ Ȯ�� ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA,	// Ȯ�ξȵ� ���Ÿ���Ʈ Ȯ�� ��û;


	// Client -> Agent;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA,	// ���� ���� Ȯ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA,		// ���� ȸ�� Ȯ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA,	// ȸ���� ���� ��÷�� ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA,			// �ζ� ĳ���� �� ���� ����Ʈ ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA,		// ȸ�� �� ��÷ ��ȣ ��û;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA,	// ȸ�� �� ��÷�� ��û;

	// Client -> Anget ( GM );
	NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA,			// ���� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA,				// ����ڵ��� ������� ������ ���� ��ȣ�� ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA,					// �ζ� ��÷ ���� �� ��ȣ�� ��ȣ�� ��� ��� �ִ��� Ȯ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA,				// �ζ��� �� ��ȣ�� �ϳ������� �ִ� ���·� �ʱ�ȭ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA,				// Ư�� ��ȣ�� ������ �߰�;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA,				// Ư�� ��ȣ�� ������ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SET_INVENT_WIN_NUM_CA,	// ���� ��÷ ��ȣ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_RESET_INVENT_WIN_NUM_CA,	// ���� ��÷ ��ȣ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_CA,		// ���� Lock ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_CA,		// �� ���� Lock ����;

	// Agent -> Field;
	NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_AF,	// ��÷�� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_POST_WIN_MONEY_FC,	// ��÷�� ����;

	// Agent -> Field -> Client;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_AF,	// �ζ� ���� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FC,	// �ζ� ���� ����;

	// Agent -> Client;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_GLOBAL_VALUE_AC,// Ŭ���̾�Ʈ ����ȭ Global Value ��Ŷ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_STATUS_AC,		// Ŭ���̾�Ʈ ����ȭ ��Ŷ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NEXT_DRAWING_DATE_AC,// ���� ��÷ ���� ���� ��Ŷ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_AC,	// ���� �ζ� �ý����� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_AC,		// ���� ȸ�� ����ȭ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_AC,	// ȸ���� ���� ��÷�� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_AC,			// �ζ� ĳ���� �� ���� ����Ʈ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_COUNT_AC,	// �ζ� ����Ʈ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_AC,		// ȸ�� �� ��÷ ��ȣ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_AC,	// ȸ�� �� ��÷�� ����;

	// Agent -> Client ( GM );
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_AC,			// ����ڵ��� ������� ������ ���� ��ȣ�� ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_AC,				// �ζ� ��÷ ���� �� ��ȣ�� ��ȣ�� ��� ��� �ִ��� Ȯ��;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_AC,			// �ζ��� �� ��ȣ�� �ϳ������� �ִ� ���·� �ʱ�ȭ;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_AC,			// Ư�� ��ȣ�� ������ �߰�;
	NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_AC,			// Ư�� ��ȣ�� ������ ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_INVENT_NUM_DSP_AC,	// ���� ���� �� DSP;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_AC,	// ���� Lock ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_AC,	// �� ���� Lock ����;

	// Agent Myself;
	NET_MSG_GCTRL_LOTTO_SYSTEM_SYNC_AGENT_MYSELF,	// ����ȭ;

	// Agent Broadcast Message;
	NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_AB,		// ���� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_GO_NEXT_TURN_AB,		// ȸ�� ����;
	NET_MSG_GCTRL_LOTTO_SYSTEM_DRAWING_NUM_AB,		// ��÷ ��ȣ;
	/// ---------------------------------------------------------------- ///

	/// Reloading AI FSM - ��Ŷ ����͸��� ���Ͽ� ��� �Ѱ��� ����;
	NET_MSG_GCTRL_RELOAD_AI_FSM_CF,
	/// ---------------------------------------------------------------- ///

	/// Country - ��Ŷ ����͸��� ���Ͽ� ��� �Ѱ��� ����;
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

	/// Country - ��Ŷ ����͸��� ���Ͽ� ��� �Ѱ��� ����;
	NET_MSG_GCTRL_SELECT_PROVIDE_REWARD_DF,
	/// ---------------------------------------------------------------- ///

	// Synchronize Use Features
	NET_MSG_GCTRL_SYNC_USE_FEATURES_AC,
	NET_MSG_GCTRL_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC,

	// ��ų ȿ�� ����;
	NET_MSG_GCTRL_APPLY_SKILLEFFECT_FC,

	// �� �̵� ��û
	NET_MSG_REQ_MOVE_MAP,
	// �����̾� �� ��������
	NET_MSG_PREMIUM_MAP_EXIT,

	NET_MSG_FIELD_STATE, //! Agent�� Field ������ ���¸� ����;



    // ConsignmentSale ( ��Ź�Ǹ� )
    // cwback
    //! REQ = Request     (��û)     
    //! ACK = Acknowledge (����)
    NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_REQ,     // ������ ��� ��û
    NET_MSG_CONSIGNMENT_SALE_ITEM_LIST_ACK,     // ������ ��� ����

    NET_MSG_CONSIGNMENT_ADD_ITEM_REQ,           // ������ ��� ��û
    NET_MSG_CONSIGNMENT_ADD_ITEM_ACK,           // ������ ��� ����

    NET_MSG_CONSIGNMENT_ITEM_REGIST_REQ,        // ���λ��� �˻� ������ ��� ��û(Field->Cache, DB->Cache)

    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_REQ, // ������ ��ȯ ��û
    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK, // ������ ��ȯ ����
    NET_MSG_CONSIGNMENT_SALE_ITEM_SENDBACK_ACK_DB, // ������ ��ȯ ����( Field DB-> Field )
   
    NET_MSG_CONSIGNMENT_SALE_PURCHASE_REQ,      //! ���λ��� ������ ��� ���� �� ����(Cache->Field)
    NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK,      //! ������ ���ſ�û�� ���� DB����(DB->Field)

    NET_MSG_CONSIGNMENT_SALE_SLOT_EXPIRATION_DATE_ACK, //! ���λ��� ���� ���� �ð�

	NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_REQ,      //! ���λ��� ������ ��� ����(Cache->Field)
	NET_MSG_CONSIGNMENT_SALE_PAY_MONEY_ACK,      //! ���λ��� ������ ��� ���� ���(Field->Cache)
	NET_MSG_CONSIGNMENT_SALE_UPDATE_ITEM_ACK,    //! ���λ��� ������ ������ ó�� ���(DB->Cache)
	NET_MSG_CONSIGNMENT_SALE_SEND_POST_AND_LOG_REQ, //! ���λ��� ���� �߼�(Cache->Field)
	NET_MSG_CONSIGNMENT_SALE_PURCHASE_ACK_DB, // ������ ���� ó�� ����( Field DB-> Field )
	NET_MSG_GCTRL_PMARKET_SEND_DATA_FF, // ���λ��� ������ ĳ�ÿ� �����ֱ� ���ؼ� �޽��� ó�� ������� �ʵ尡 �ٽ� �޾Ƽ� ó��( Field-> Field )

    //////////////////////////////////////////////////////////////////////////


	// �ػ罺Ȧ�� �޽���
	NET_MSG_GCTRL_TEXAS_HOLD_EM, 
	NET_MSG_BLOCK_USER,

	NET_MSG_POST_GOT_DATA_CHANGE, 
	NET_MSG_POST_GOT_STATE_CHANGE, 
	NET_MSG_POST_GOT_SEND_STATE_CHANGE, 
	NET_MSG_POST_GOT_RECV_STATE_CHANGE, 

	NET_MSG_CHAR_MONEY_NOTIFY_FA, 

	//////////////////////////////////////////////////////////////////////////
	// ���� ����
	// cwback
	NET_MSG_POST_RENEW_LIST_REQ,	// ���� ���
	NET_MSG_POST_RENEW_LIST_ACK,
	NET_MSG_POST_RENEW_SEND_REQ,	// ���� ����
	NET_MSG_POST_RENEW_SEND_ACK,
	NET_MSG_POST_RENEW_CHA_CHECK_ACK,// ĳ���� üũ( �ʵ弭�� ��� )
	NET_MSG_POST_RENEW_DEL_REQ,		// ���� ����
	NET_MSG_POST_RENEW_DEL_ACK,
	NET_MSG_POST_RENEW_ATTACH_REQ,	// ���� �ޱ�
	NET_MSG_POST_RENEW_ATTACH_ACK,
	NET_MSG_POST_RENEW_READ_REQ,	// ���� �б�
	NET_MSG_POST_RENEW_ATTACH_ROLLBACK_REQ,	// ���� ÷�� �ѹ�
	NET_MSG_POST_RENEW_RETURN_REQ,	// ���� �ݼ�
	NET_MSG_POST_RENEW_RETURN_ACK,
	NET_MSG_POST_RENEW_NEW_POST,	// �� ���� ������

	// ��ų ��Ÿ�� ���� ȿ�� ���� ���� ( Ŭ��� ����ȭ )
	NET_MSG_GCTRL_SKILLDELAY,		// ��ų ������(��Ÿ��,������) ���� 
	
	NET_MSG_COUNTRECORDER_INFO_REQ,
	NET_MSG_COUNTRECORDER_INFO_FB,

	NET_MSG_GM_NOTIFY_MONEY_CAF,
	NET_MSG_GCTRL_REQ_EXCHANGE_BUY_FROM_NPC, 

	NET_MSG_GCTRL_CLUB_STORAGE_USER_RESET,
	NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_RESET, 
	NET_MSG_GCTRL_CLUB_STORAGE_USER_FB,
	NET_MSG_GCTRL_CLUB_STORAGE_MONEY_USER_FB,

	NET_MSG_GCTRL_INVEN_COSTUME_STATS,		//!	�ڽ�Ƭ �ɷ�ġ �ο�
	NET_MSG_GCTRL_INVEN_COSTUME_STATS_FB,	//!	�ڽ�Ƭ �ɷ�ġ �ο� FB.
	NET_MSG_LOG_INVEN_COSTUME_STATS,		//! �ڽ�Ƭ �ɷ�ġ �α� 

	NET_MSG_DRUG_FB,

	NET_MSG_DEFAULT_GAME_OPTION,

	NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE,	//! �ڽ�Ƭ �ɷ�ġ �ο� ����
	NET_MSG_GCTRL_INVEN_COSTUEM_RELEASE_FB,	//! �ڽ�Ƭ �ɷ�ġ �ο� ���� FB

	NET_MSG_GCTRL_PARTY_FNEW_UPDATE,	//!	��Ƽ ����. (field).

	NET_MSG_DISCONNECT_CHARACTER, 
	NET_MSG_CTF_NEXT_READY_TIME, 
	NET_MSG_SEND_SERVER_STOP_MSG, 

	NET_MSG_GCTRL_INVEN_TL_GRINDING,	//!	INVEN ������ �Ͻð�ȭ �õ�.

	NET_MSG_GCTRL_LEVEL_LAYER_FC,		//! Level �� Layer �� Ȱ�� Ȥ�� ��Ȱ�� �Ǿ��ٰ� Client ���� �˸�.

	NET_MSG_MACROMODE_ONOFF_CF,		// ��ũ�� Ȱ��ȭ;
	NET_MSG_MACROMODE_ONOFF_FB,		// �� �޼��� �亯;
	NET_MSG_MACROMODE_CLOSE_FC,		// ��ũ�ΰ� ������ ���� ����ɽ� ���޵� �޼���;
	NET_MSG_MACROMODE_STATE_FC,		// �������� ���� ��ȭ�� �˸�;
	NET_MSG_MACROMODE_REQNEARESTCROWINFO_CF,
	NET_MSG_MACROMODE_NEARESTCROWINFO_FC,

	NET_MSG_GCTRL_PRISON_AG,
	NET_MSG_GCTRL_FORCERESET_SKILL_STAT_FC,

    /*
     * redmine : #1161 Randombox Ȯ������ �ý��� ó��
     * created : sckim, 2015.12.11, �űԵ��
     * modified : 
     */
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_CA, // Randombox Ȯ������ ������ ��� ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_AC,  // Randombox Ȯ������ ������ ��� ��� Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH, // Randombox Ȯ������ ������ ��� ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA,  // Randombox Ȯ������ ������ ��� ��� Feedback

     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_CA,     // Randombox Ȯ������ ������ ��ȸ ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_AC,      // Randombox Ȯ������ ������ ��ȸ ��� Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH,     // Randombox Ȯ������ ������ ��ȸ ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA,      // Randombox Ȯ������ ������ ��ȸ ��� Feedback

     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_CA,   // Randombox Ȯ������ ������ ���� ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_AC,    // Randombox Ȯ������ ������ ���� ��� Feedback
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH,   // Randombox Ȯ������ ������ ���� ��û
     NET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA,    // Randombox Ȯ������ ������ ���� ��� Feedback

	 //NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN,	//!	random box open ��û. 
	 //NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB,	//!	random box open ��û FB.
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH, // Randombox Open�� Ȯ������ ���뿩�� ��û
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF,  // Randombox Open�� Ȯ������ ���뿩�� ��� Feedback
     NET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH, // Randombox Open�� Ȯ������ �������� ���θ� CacheServer�� ����

     NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_FA,   // Randombox Open�� ��÷Ȯ���� 5%�̸��� ��� ���� �޽����� ����
     NET_MSG_GCTL_RANDOMBOX_WINNER_NOTIFY_AC,   // Randombox Open�� ��÷Ȯ���� 5%�̸��� ��� ���� �޽����� ����

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

    //! Ȯ��/������ ����Ʈ ���� ��� : sckim, Redmine #4544
    NET_MSG_GM_RANDOMBOX_NOTIFY_ITEM_RELOAD_CAF,   //! �����ڽ� ��÷�� ���� Ȯ�� Reload ��û

	NET_MSG_GCTRL_CLUB_RENAME_CF, 
	NET_MSG_GCTRL_CLUB_RENAME_DF, 
	NET_MSG_GCTRL_CLUB_RENAME_FA, 
	NET_MSG_GCTRL_CLUB_RENAME_AF, 
	NET_MSG_GCTRL_CLUB_RENAME_CLT, 
	NET_MSG_GCTRL_CLUB_RENAME_BRD, 
	NET_MSG_GCTRL_CLUB_RENAME_2ALLIANCE, 
	NET_MSG_GCTRL_CLUB_RENAME_FB, 

	// ĳ���� ����;
	NET_MSG_REQUEST_LOCK_CHARSLOT_CF,
	NET_MSG_REQUEST_UNLOCK_CHARSLOT_CF,
	NET_MSG_REQUEST_CHANGE_CHARSLOT_CF,
	NET_MSG_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF,
	NET_MSG_RESPONSE_LOCK_CHARSLOT_FC,
	NET_MSG_RESPONSE_UNLOCK_CHARSLOT_FC,
	NET_MSG_RESPONSE_CHANGE_CHARSLOT_FC,
	NET_MSG_UPDATE_CHARSLOT_FC,

	// ������ ��ų ����;
	NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_ITEMSKILL_UIOPEN_FB,

	// ������ �⺻�ɷ�ġ ����;
	NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_BASICSTATS_UIOPEN_FB,

	// ������ ����ɷ�ġ ����;
	NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_CF,
	NET_MSG_INVEN_CHANGE_ADDSTATS_UIOPEN_FB,

	NET_MSG_SKILL_RESET_CF, 

	//================================== ������ ==================================
	// !!!!!!!!!! ���� !!!!!!!!!!
	// ��Ʈ��ũ �������Ϸ��� ����Ǳ� ������ ��Ŷ Define�� �ִ� ũ��� 5000 �� �Ѿ�ٸ� CPROFILE ���� �ִ� ũ�⸦ �����ؾ��Ѵ�.
	// �������� NET_MSG_GCTRL_MAX �� ���������� �ҷ��������� ���� ��Ʈ��ũ�ܰ� ������ �����ʿ䰡 ��� �������� ������. 
	// ���� �ִ��� ���ϸ� ���̱� ���ؼ� �ʵ��� �����̳ʸ� ������� �ʰ� �迭�� ����ؼ� ���� �����ϵ��� ��.
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
    // ChaNum�� �ʿ� ���� ���
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

    // ChaDbNum�� �ʿ��� ��
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
	// �޽��� ���� ���
	// RanLogic\Msg\MessageEnumMixer.h ����
	
public:
// ����� �ڵ�󿡼� off ��Ű����	
// _USE_MESSAGE_SHUFFLE_ ������ �ּ��Ұ�
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
// FA, AF Ȥ�� �̸� �����ϴ� CAF, FAC ������ ��� �޽����� �̰��� ��ӹ޾ƶ�.
struct NET_MSG_CHARACTER : public NET_MSG_GENERIC	
{
	NET_MSG_CHARACTER()
		: dwChaNum(0)
	{
	}

	DWORD	dwChaNum;	// � ĳ���Ϳ� ���� �޽������� ��Ȯ�� ���, �� ���� ����ؼ� Relay���� �Ѵ�.
};
typedef NET_MSG_CHARACTER* LPNET_MSG_CHARACTER;


/*
mjeon.instance

 Agent -> Match -> Instance(������ �� Ư�� �ν��Ͻ�) ����
	or
 Instance -> Match -> Agent(Ư�� ������) ����
   ������ ���޿� ���
*/
struct NET_MSG_UNIQUEKEY : public NET_MSG_CHARACTER
{
	NET_MSG_UNIQUEKEY()
		:dwSvrGroupNum(0)
	{
	}

	DWORD	dwSvrGroupNum;	// (�ν��Ͻ�����)Ư�� �������� Ư�� ĳ���Ϳ� ���� �޽������� ��Ȯ�� ���, �� ���� ����ؼ� Relay���� �Ѵ�.
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
// NET_MSG_SVR_PAUSE, // ������ ��� �����.
// NET_MSG_SVR_RESUME, // ���߾��� ������ �ٽ� ������Ų��.	
// NET_MSG_SVR_RESTART, // ������ ������ ���߰� ���Ӱ� ������Ų��.
// NET_MSG_SVR_STOP, // ������ ������ �����.
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

//! Login->Session:���� ����
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
	int	 nIndex;	 //! �ɸ��� ����/class
	WORD wSchool;	 //! �б�
	WORD wFace;		 //! ��
	WORD wHair;		 //! ��� ��Ÿ��
	WORD wHairColor; //! ��� �÷�
	WORD wSex;		 //! ����
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
	bool				bExtreme; // �ذ��� ���� ���� ����;
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

// �κ񿡼� ���÷��� �� ĳ������ ���������� ��û�Ѵ�.
struct NET_CHA_REQ_BA_INFO : public NET_MSG_GENERIC
{
	// int             nChannel; // ����ڰ� �����Ϸ��� ä��
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

// �ش� ĳ������ ������ ��û�Ѵ�.
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
	INT	 m_ChaDbNum; // ������ ĳ���� ��ȣ
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
	INT m_ChaDbNum; //! ������ ĳ���� ��ȣ
	char m_szPass2[USR_PASS_LENGTH+1];
	
	DAUM_NET_CHA_DEL(int ChaDbNum, const char* szPass2)
        : m_ChaDbNum(ChaDbNum)
	{
		nType	= DAUM_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
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
	INT					nChaNum; // ������ ĳ���� ��ȣ
	char				sz2ndPass[MD5_MAX_LENGTH+1];
	
	IDN_NET_CHA_DEL(int ChaDbNum)
        :nChaNum(ChaDbNum)
	{
		nType	= IDN_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
		dwSize = sizeof(IDN_NET_CHA_DEL);
		
		ZeroMemory(sz2ndPass, sizeof(sz2ndPass));
	}
};
typedef IDN_NET_CHA_DEL* LPIDN_NET_CHA_DEL;



/**
 * ĳ���� �������� Client->Agent
 * \return 
 */
struct TERRA_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // ������ ĳ���� ��ȣ
	
	TERRA_NET_CHA_DEL()
	{
		nType	= TERRA_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
		dwSize = sizeof(TERRA_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef TERRA_NET_CHA_DEL* LPTERRA_NET_CHA_DEL;

/**
 * ĳ���� �������� Client->Agent
 * \return 
 */
struct GSP_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // ������ ĳ���� ��ȣ
	
	GSP_NET_CHA_DEL()
	{
		dwSize = sizeof(GSP_NET_CHA_DEL);
		nType = GSP_NET_MSG_CHA_DEL;
		nChaNum = 0;
	}
};
typedef GSP_NET_CHA_DEL* LPGSP_NET_CHA_DEL;

/**
 * ĳ���� �������� Client->Agent
 * \return 
 */
struct EXCITE_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // ������ ĳ���� ��ȣ
	
	EXCITE_NET_CHA_DEL()
	{
		nType	= EXCITE_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
		dwSize = sizeof(EXCITE_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef EXCITE_NET_CHA_DEL* LPEXCITE_NET_CHA_DEL;

//! ĳ���� �������� Client->Agent
struct JAPAN_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT					nChaNum; // ������ ĳ���� ��ȣ
	
	JAPAN_NET_CHA_DEL()
	{
		nType	= JAPAN_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
		dwSize = sizeof(JAPAN_NET_CHA_DEL);
		nChaNum = 0;
	}
};
typedef JAPAN_NET_CHA_DEL* LPJAPAN_NET_CHA_DEL;

//! ĳ���� �������� Client->Agent
struct GS_NET_CHA_DEL : public NET_MSG_GENERIC
{
	INT	 m_ChaDbNum; // ������ ĳ���� ��ȣ
	char m_szPass2[GS_USER_PASS+1];

	GS_NET_CHA_DEL(int ChaDbNum, const char* szPass2)
        : m_ChaDbNum(ChaDbNum)
	{	
		nType	= GS_NET_MSG_CHA_DEL; // Ŭ���̾�Ʈ->���Ӽ��� : �ɸ��� ����
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
	INT					nChaNum;	// ������ ĳ���� ��ȣ
	BOOL				bExtreme;
	BOOL				bActor;
    WORD                wChaRemain; // ���� ĳ���� ���� ����
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

// 2�����
struct NET_LOBBY_CHAR_JOIN_CA : public NET_MSG_GENERIC
{
	INT m_ChaDbNum; //! ���� ������ ĳ���� ��ȣ
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

// 2����� ���� �޼���
//! �������� �޽���
//! Client -> Agent Server
//struct NET_LOBBY_CHAR_JOIN_CA : public NET_MSG_GENERIC
//{
//	INT m_ChaDbNum; //! ���� ������ ĳ���� ��ȣ
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
	EMJOINTYPE_FIRST	= 0,	// �ű� ����
	EMJOINTYPE_MOVEMAP	= 1,	// ���̵��� ���� �ʵ弭�� ����
	EMJOINTYPE_REBIRTH	= 2,	// ��Ȱ�� ���� �ʴ����� ����
	EMJOINTYPE_TRANSFER = 3,	// Serialization
    EMJOINTYPE_PVEINDUN = 4,    // PVE �δ� �̵��� ���� �ʵ弭�� ����
	EMJOINTYPE_GATE		= 5,	// ����Ʈ �̵�;
};

// �������� �޽���
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

    bool bChangeField; // �⼮�� �غ� ����.

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


//! Agent->Client:�ش� Field ������ �����϶�
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

//! Client->Field:Ŭ���̾�Ʈ�� ������ Field Server�� ������ �ڽ��� ID Ȯ�� ��û.
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

// ���������Ϸ�
// Field->Session
struct NET_GAME_JOIN_OK_FS : public NET_MSG_GENERIC
{	
	INT					nUsrNum; // ����� ������ȣ(DB)	
	INT					nChaNum; // ĳ���͹�ȣ(DB)
	DWORD				dwGaeaID; // ���̾� ID
	CHAR				szUserID[USR_ID_LENGTH+1]; // ����� ID	
	CHAR				szChaName[CHR_ID_LENGTH+1]; // ĳ���͸�

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

// ������ ä�ø޽���
// ��Ʈ�����α׷�->���Ǽ���
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

// ��Ŷ �������� ���ؼ� �����Ŵ������� ����ϴ� ��Ŷ�� ������ �߰� ( ���� ��Ŷ�� ����ϴ� �κ��� ������ �־ ���� )
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

// ��Ŷ �������� ���ؼ� �����Ŵ������� ����ϴ� ��Ŷ�� ������ �߰� ( ���� ��Ŷ�� ����ϴ� �κ��� ������ �־ ���� )
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

// �����ڿ� �޽��� FB
struct NET_CHAT_CTRL_FB : public NET_MSG_GENERIC
{	
	EMNET_MSG_CHAT	emType;						// ä�� Ÿ��
	CHAR			szChatMsg[GLOBAL_CHAT_MSG_SIZE+1];	// ä�ø޽���

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
// �� ��������!! �̺�Ʈ 
// ���ϰ� :
// 1  1 �� P-4 PC                        (1� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.)
// 2  2 �� ȿ����                        (2� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.)
// 3  3 �� SONY ��ī                     (3� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.)
// 4  4 �� ���̸��� MP �÷��̾�          (4� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.)
// 5  5 �� MAX ���� ��ȭ ��ǰ�� 1 ������ (5� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.)
// 6  6 �� MBC Game ���� ���� ��ǰ��     (6� ��÷�Ǽ̽��ϴ�. ���ϵ帳�ϴ�. �ڼ��� ������ Ȩ�������� ������ �ֽʽÿ�.) 
// 7  �̹� ������ �Է��� �����          (�̹� ���ǿ� �����ϼ̽��ϴ�.)
// 8  PC �� IP �� �ƴ�                   (PC �� IP �� �ƴմϴ�. ������ PC �濡���� ���� �����մϴ�)
// 9  �̹� ����� ����                   (�̹� ����� �����Դϴ�)
// 10 �˼����� ����                      (����ó���� ������ �߻��Ͽ����ϴ�)
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
// �⼮�� �̺�Ʈ
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

//! nProtect GameGuard Auth ��û �޽���
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

//! nProtect GameGuard Auth Answer �޽���
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

//! nProtect GameGuard Auth ��û �޽��� #1
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

//! nProtect GameGuard Auth Answer �޽��� #1
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

//! nProtect GameGuard Auth ��û �޽��� #2
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

//! nProtect GameGuard Auth Answer �޽��� #2
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

// ������ �������� �ִ� ������ȣ�� �������� ���� ����ü
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

// AGT->Session ���� LoginMaintenance On/Off
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
#define CHAR_TEMPMONEY 64		  //   TempMoney �ִ� Char �迭�� ����

/*
#if defined(TW_PARAM) || defined(_RELEASED) 
	#define GAMEMONEY_VER 100		  //   GameMoney ����
#elif(HK_PARAM) 
	#define GAMEMONEY_VER 100		  //   GameMoney ����
#else 
	#define GAMEMONEY_VER 0		  //   GameMoney ����
#endif
*/
#define GAMEMONEY_VER 100		  //define���� ������ �ǰ� ������, ���� ���ǰ� ������ �ʴ�.


// 2009�� 10�� ������ ��ȣȭ ���
LONGLONG DECRYPTMONEYVER100( const char* szMoneyChar );
std::string ENCRYPTMONEYVER100(const LONGLONG lnGameMoney);
// 2009�� 10�� ������ ��ȭȭ ���
LONGLONG DECRYPTMONEYVER101( const char* szMoneyChar );
std::string ENCRYPTMONEYVER101(const LONGLONG lnGameMoney);

LONGLONG DECRYPTMONEY( const char* szMoneyChar, const int ver = GAMEMONEY_VER );
std::string ENCRYPTMONEY(const LONGLONG lnGameMoney, const int ver = GAMEMONEY_VER);


/******************************************************************************
* GameGuard ���� �ڵ� üũ �Լ�
* \param dwErrCode nProtect GameGuard �����ڵ�
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
