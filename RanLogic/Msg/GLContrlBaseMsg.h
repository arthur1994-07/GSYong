#pragma once

#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../../EngineLib/G-Logic/GLPeriod.h"
#include "../../SigmaCore/gassert.h"

//#include "../s_NetGlobal.h"
#include "../Character/GLCharData.h"
#include "../Crow/GLCrowData.h"
#include "../Item/GLItem.h"
#include "../Item/GLMoney.h"
#include "../Club/GLClubDefine.h"
#include "../Party/GLPartyDefine.h"
#include "../GLCDMDefine.h"
#include "../GLChatLinkDef.h"
#include "../Market/MarketDefine.h"


//	#include "GLContrlMsg.h"
//	메세지 정보의 크기를 (NET_DATA_BUFSIZE) byte 이내로 제한 하여 설계.
//
#define NET_DELAY		(0.2f)
#define NET_MSGDELAY	(0.3f)

#define MAX_SEARCH_RESULT 10

inline float APPLY_MSGDELAY ( float &fDelay )
{
	fDelay -= NET_MSGDELAY;
	if ( fDelay < 0.0f )	fDelay = 0.0f;
	return fDelay;
}

enum EMGLMSG
{
	EMGLMSG_SKILLMAX	= 100,
	EMGLMSG_COOLTIMEMAX	= 30,
};

enum EMREQUEST_MSG
{
	EMREQMSG_OBJ_DROP	= 0,
};

enum EMTAKE_FB
{
	EMTAKE_FB_OFF						= 0,	//	아이탬이 사라진 경우.
	EMTAKE_FB_RESERVED				= 1,	//	예약 소유자가 지정되어 있을 경우.
	EMTAKE_FB_INVEN_ERROR			= 2,	//	인벤토리에 넣을수 없음.
	EMTAKE_FB_TOO_EVENT_ITEM	= 3,	//	이미 이벤트 아이템을 가지고 있습니다.
	EMTAKE_FB_DISTANCE				= 4,	//	거리가 너무 멉니다.
	EMTAKE_FB_TENDERING				= 5,	// 입찰이 진행중입니다.
};

enum EMHOLD_FB
{
	EMHOLD_FB_OFF				= 0,	//	동일한 아이탬이 존제하지 않습니다.
	EMHOLD_FB_OK				= 1,	//	아이탬을 HOLD합니다.
	EMHOLD_FB_NONKEEPSTORAGE	= 2,	//	락커에 물품을 넣을 수 없음.
};

enum EMCHAR_JOIN_FB
{
	EMCJOIN_FB_ERROR		= 0,
	EMCJOIN_FB_NOWLOGIN		= 1,	//	현제 접속중인 상태라서 접속이 안됩니다.
	EMCJOIN_FB_PKPOINT		= 2,	//	pk 성향이 높아서 해당 채널에 접속 불가.
	EMCJOIN_FB_SAVE_CHA		= 3,	//	아직 저장중인 캐릭터가 있어서 접속 할 수 없다.
	EMCJOIN_FB_WAIT			= 4,	//  1.5초 이내로 연속적으로 로그인 메세지 보내는 경우 // 2차비번
	EMCJOIN_FB_SAVE_CHA2	= 5,	//	필드서버에 ado 문제가 생겨 sqlite 캐릭터 정보를 저장중이거나, 아직 그 문제 있던 필드서버가 아직 정상이 되지 않아 접속 불가
	EMCJOIN_FB_OVERSTAT_RANDOMOPTION	= 6,	//	랜덤옵션이 문제가 있는 캐릭터이다.
	EMCJOIN_FB_BLOCK_CLASS = 7, // GS 전용, 변술부 사전생성만 가능하고 게임에 접속할 수 없도록
};

enum EMDROP_OUT_FORCED
{
	EMDROPOUT_ERROR			= 0,	//	오류로 인한 접속 종료.
	EMDROPOUT_REQLOGIN		= 1,	//	동일 유저의 접속 시도로 인한 접속 종료.
	// 아래 값은 사용안함, 판별용으로 나둠;
	EMDROPOUT_DISCONNECTED	= 2,	//	서버랑 연결이 끊어짐;
};

enum EM2ND_PASS_FAIL
{
	EMPASS_SUCCESS = 0,
	EMPASS_FAIL = -2,
	EMPASS_BLOCK = -3,
};

enum EMCHAR_GATEOUT_FB
{
	EMCHAR_GATEOUT_OK		= 0,
	EMCHAR_GATEOUT_FAIL		= 1,
	EMCHAR_GATEOUT_MAPID	= 2,
	EMCHAR_GATEOUT_GATEID	= 3,
	EMCHAR_GATEOUT_TARMAPID	= 4,
	EMCHAR_GATEOUT_TARGATEID= 5,
	EMCHAR_GATEOUT_FIELDSVR	= 6,
	EMCHAR_GATEOUT_HOLD		= 7,
	EMCHAR_GATEOUT_CONDITION = 8,
	EMCHAR_GATEOUT_CLUBBATTLE = 9,			//	선도전맵시 클럽배틀
    EMCHAR_GATEOUT_CLUBBATTLE2 = 10,		//	CDM맵시 클럽배틀
    EMCHAR_GATEOUT_CTF = 11,				//	CTF 진행중일때 진입 불가
    EMCHAR_GATEOUT_CDM_MEMS_LIMITED = 12,	// CDM 인원 초과시 진입 불가.
	EMCHAR_GATEOUT_PARTYMAP = 13,			// 파티맵 진입 불가
};

enum EMSKILL_VALIDATION
{
	EMSKILL_VALIDATION_OK					= 0,
	EMSKILL_VALIDATION_NONE_SKILL			= 1,
	EMSKILL_VALIDATION_NONE_LEARN			= 2,
	EMSKILL_VALIDATION_INVALID_SKILLLEVEL	= 3,
	EMSKILL_VALIDATION_INVALID_CLASS		= 4,
	EMSKILL_VALIDATION_INVALID_BRIGHT		= 5,
	EMSKILL_VALIDATION_INVALID_LEVEL		= 6,
	EMSKILL_VALIDATION_INVALID_NEEDSKILL	= 7,
};

enum EMSKILL_LEARNCHECK
{
	EMSKILL_LEARN_OK		= 0,	//	배우기 성공.
	EMSKILL_LEARN_MAX		= 1,	//	더 이상 렙업을 할 수 없음.
	EMSKILL_LEARN_NOTTERM	= 2,	//	요구 조건이 충족되지 않아서 렙업을 할수 없음.
	EMSKILL_LEARN_NOTCLASS	= 3,	//	부서가 틀려서 배울수 없습니다.
	EMSKILL_LEARN_NOTBRIGHT	= 4,	//	빛/암 조건이 충족되지 않습니다.
	EMSKILL_LEARN_ALREADY	= 5,	//	이미 배운 스킬입니다.

	EMSKILL_LEARN_UNKNOWN	= 999
};

enum EMLUNCHBOX_FB
{
	EMLUNCHBOX_FB_OK		= 0,	//	사용 성공.
	EMLUNCHBOX_FB_FAILED 	= 1,	//	사용 실패.

	EMLUNCHBOX_FB_UNKNOWN	= 999
};


enum EMPARTY_LURE_FB
{
	EMPARTY_LURE_OK,					//	파티 모집에 수락.
	EMPARTY_LURE_REFUSE,			//	파티 모집에 거부.	
	EMPARTY_LURE_FULL,				//	파티가 가득차서 더이상 오을수 없음.
	EMPARTY_LURE_OTHER,				//	다른 파티에 가입되어 있어서 거절됨.
	EMPARTY_LURE_NOCHAR,			// 존재 하지 않는 케릭터.
	EMPARTY_LURE_OPT,				//	파티 옵션 오류.
	EMPARTY_LURE_OTHERSCHOOL,	//	다른 학원생과의 파티 불가능.
	EMPARTY_LURE_CLUBBATTLE,		//	클럽배틀 중일때는 파티 불가
	EMPARTY_LURE_CTF,				//	CTF 중일때는 파티 불가
	EMPARTY_LURE_TIME_OVER,		// 제한 시간 초과;
	EMPARTY_LURE_CREATE_FAIL,
};

enum EMTRADE_ANS
{
	EMTRADE_OK				= 0,
	EMTRADE_CANCEL			= 1,
};

enum EMTRADE_AGREE_FB
{
	EMTRADE_AGREE_OK		= 0,
	EMTRADE_AGREE_CANCEL	= 1,
	EMTRADE_AGREE_TIME		= 2,
};

enum EMTRADE_CANCEL_TAR
{
	EMTRADE_CANCEL_NORMAL	= 0,	//	일반적인 취소.
	EMTRADE_CANCEL_OTHER	= 1,	//	상대편이 취소함.
	EMTRADE_CANCEL_MYINVEN	= 2,	//	개래 도중 인밴 부족.
	EMTRADE_CANCEL_TARINVEN	= 3,	//	개래 도중 인밴 부족.
};

enum EMGRINDING_FB
{
	EMGRINDING_FAIL			= 0,
	EMGRINDING_SUCCEED		= 1
};

enum EMREGEN_GATE_FB
{
	EMREGEN_GATE_FAIL		= 0,
	EMREGEN_GATE_SUCCEED	= 1,
};

enum EMREGEN_CURE_FB
{
	EMREGEN_CURE_FAIL		= 0,
	EMREGEN_CURE_SUCCEED	= 1,
};

enum EMREGEN_CHARRESET_FB
{
	EMREGEN_CHARRESET_SUCCEED	= 0,
	EMREGEN_CHARRESET_FAIL		= 1,
	EMREGEN_CHARRESET_ITEM_FAIL	= 2,
};

enum EMNPC_ITEM_TRADE_FB
{
	EMNPC_ITEM_TRADE_SUCCEED		= 0,
	EMNPC_ITEM_TRADE_FAIL			= 1,
	EMNPC_ITEM_TRADE_ITEM_FAIL		= 2,
	EMNPC_ITEM_TRADE_INSERT_FAIL	= 3,
};

enum EMCONFRONT_FB
{
	EMCONFRONT_FAIL				= 0,	//	일반 오류;
	EMCONFRONT_AGREE			= 1,	//	대련 수락(ans용);
	EMCONFRONT_REFUSE			= 2,	//	대련 거부;
	EMCONFRONT_OPTION			= 3,	//	옵션 오류;
	EMCONFRONT_PEACE			= 4,	//	평화 지역 대련 불가;
	EMCONFRONT_MAX				= 5,	//	n 번 이상 연속 대련으로 time 이후 대련 가능;
	EMCONFRONT_ELAP				= 6,	//	종전 대련후 time 이후 대련 가능;
	EMCONFRONT_ALREADY_ME	= 7,	//	자신이 이미 대련중;
	EMCONFRONT_ALREADY_TAR	= 8,	//	상대방이 이미 대련중;
	EMCONFRONT_PARTY			= 9,	//	같은 파티원 끼리는 대련 불가;
	EMCONFRONT_DIE_YOU			= 10,	//	상대 캐릭터가 죽어있어서 대련 불가;
	EMCONFRONT_DIE_ME			= 11,	//	자신의 캐릭터가 죽어있어서 대련 불가;

	EMCONFRONT_MENOTPARTY	= 12,	//	파티원이 아니여서 파티 대련이 불가능;
	EMCONFRONT_MENOTMASTER	= 13,	//	파티장이 아니여서 [파티대련]을 신청하실수 없습니다;
	EMCONFRONT_TARMASTER		= 14,	//	파티장이 아니여서 [파티대련]을 신청하실수 없습니다;
	EMCONFRONT_VEHICLE			= 15,	//	탈것을 타고 있어서 대련 불가;
	EMCONFRONT_EXPEDITION		= 16, // 원정대 결성중에는 대련 불가;

	EMCONFRONT_NONCONFRONTMODE	= 17,	//  대련 불가능 모드가 켜져있음 or 대련이 불가능한 맵임
};

enum EMCONFRONT_END
{
	EMCONFRONT_END_FAIL		= 0,	//	일반 오류.
	EMCONFRONT_END_WIN		= 1,	//	승리.
	EMCONFRONT_END_LOSS		= 2,	//	패배.
	EMCONFRONT_END_TIME		= 3,	//	시간 초과.
	EMCONFRONT_END_MOVEMAP	= 4,	//	맵 이동.
	EMCONFRONT_END_DISWIN	= 5,	//	거리가 너무 떨어짐.
	EMCONFRONT_END_DISLOSS	= 6,	//	거리가 너무 떨어짐.

	EMCONFRONT_END_PARTY	= 7,	//	대련 도중 서로 파티원이 되어서 종료됨.

	EMCONFRONT_END_NOTWIN	= 10,	//	승리에 기여하지 못함.
	EMCONFRONT_END_PWIN		= 11,	//	파티대련에 승리.
	EMCONFRONT_END_PLOSS	= 12,	//	파티대련에 패배.
	EMCONFRONT_END_PTIME	= 13,	//	파티대련 시간초과.

	EMCONFRONT_END_CWIN		= 14,	//	클럽대련에 승리.
	EMCONFRONT_END_CLOSS	= 15,	//	클럽대련에 패배.
	EMCONFRONT_END_CTIME	= 16,	//	클럽대련 시간초과.
};

enum EMGRINDINGRESULT
{
	EMGRINDINGRESULT_FAILED					   = 0x0000, //실패
	EMGRINDINGRESULT_SUCCESS	               = 0x0001, //성공
	EMGRINDINGRESULT_ANTIDISAPPEAR             = 0x0002, //소방주 사용
	EMGRINDINGRESULT_INC_GRINDINGRATE		   = 0x0004, //강화주 사용 
	EMGRINDINGRESULT_ANTIRESET			       = 0x0008, //완화주 사용 
	EMGRINDINGRESULT_RESET					   = 0x0010, //초기화 됨 
	EMGRINDINGRESULT_TERMINATE				   = 0x0020, //파괴 됨
};

enum EMBUYREBUYITEM
{
    EMBUYREBUYITEM_FAILED = 0,              // 일반적 오류.
    EMBUYREBUYITEM_NOT_ENOUGH_MONEY = 1,    // 돈 부족.
    EMBUYREBUYITEM_CAN_NOT_BUY_ITEM = 2,    // 살 수 없는 아이템.
    EMBUYREBUYITEM_NOT_ENOUGH_INVEN = 3,    // 인벤 공간 부족.
    EMBUYREBUYITEM_OVER_DURATION = 4,       // 유효기간 초과.
    EMBUYREBUYITEM_SUCCESS = 5,             // 성공.
};

// 캐릭터 로비 디스플레이용 데이터
struct SCHARINFO_LOBBY
{
	DWORD				m_dwCharID;
    DWORD               m_ClubDbNum; //! 클럽 번호

	//EMCHARCLASS			m_emClass;					// 직업. (고정)
    DWORD			    m_emClass;					// 직업. (고정) EMCHARCLASS
	WORD				m_wSchool;					// 학원.
	WORD				m_wRebornNum;					// 학원.
	WORD				m_wHair;					// 헤어스타일
	WORD				m_wFace;					// 얼굴모양
	WORD                m_wSex;                     // 성별 (추가:2005-12-29 Jgkim)
	WORD                m_wHairColor;               // 머리 색깔 (추가:2005-12-29 Jgkim)

	GLPADATA			m_sHP;						//	생명량. ( 현제/최대량 )
	GLLLDATA			m_sExperience;				//	경험치. ( 현제/다음레밸도달값 )
	int					m_nBright;					//	속성.

	WORD				m_wLevel;					//	레벨.
	SCHARSTATS			m_sStats;					//	Stats.

	//SITEM_LOBY			m_PutOnItems[SLOT_TSIZE];	//	형상 정보.
    std::vector<SITEM_LOBY> m_PutOnItems;

	SNATIVEID			m_sSaveMapID;				//	종료 맵.
    
    DWORD               m_ClubRank;                 //! 클럽 랭크
    std::string			m_ChaName; //[CHAR_SZNAME];		// 이름. (고정)
    std::string         m_ClubName; //[CLUB_NAME_LENGTH]; //! 클럽이름

    bool                m_Lock;                     //! 잠겨있는가? (선물기능), 잠겨있는 캐릭터는 플레이 불가. 잠금 표시를 한다.

	bool				m_bRanMobile;

    MSGPACK_DEFINE(
        m_dwCharID, m_ClubDbNum,
        m_emClass, m_wSchool, m_wHair, m_wFace, m_wSex, m_wHairColor,
        m_sHP, m_sExperience, m_nBright,
        m_wLevel, m_sStats,
        m_PutOnItems,
        m_sSaveMapID,
        m_ClubRank, m_ChaName, m_ClubName,
        m_Lock, m_bRanMobile);

	SCHARINFO_LOBBY()
		: m_ClubDbNum(0)
        , m_dwCharID(0)
		, m_emClass(GLCC_ARMS_M)
		, m_wLevel(1)
		, m_wSchool(0)
		, m_wHair(0)
		, m_wFace(0)
		, m_wSex(0)
		, m_wHairColor(0)
		, m_nBright(0)
        , m_Lock(false)
        , m_ClubRank(0)
		, m_bRanMobile(false)
	{
		//memset(m_ChaName, 0, sizeof(char) * CHAR_SZNAME);		
        //memset(m_ClubName, 0, sizeof(char) * CLUB_NAME_LENGTH);

        // 오류를 방지하기 위해서 미리 SLOT_TSIZE 만큼 넣어둔다.
        m_PutOnItems.reserve(SLOT_TSIZE);
        SITEM_LOBY LobyItem;
        for (size_t i=0; i<SLOT_TSIZE; ++i)
            m_PutOnItems.push_back(LobyItem);
	}

    void SetName(const std::string& ChaName)
    {
        m_ChaName = ChaName;
    }

    //! 잠겨있는가? (선물기능)
    inline bool IsLock() const { return m_Lock; }
    inline DWORD ChaDbNum() const { return m_dwCharID; }

    //! 클럽 랭크
    inline DWORD GetClubRank() const { return m_ClubRank; }
    inline const char* GetClubName() const { return m_ClubName.c_str(); }
    inline void SetClubName(const char* szName)
    {
        if (szName)
            m_ClubName = szName;
    }
};

//! Agent->Cache:캐릭터 정보 요청
struct NET_LOBBY_CHAR_INFO_AH : public NET_MSG_GENERIC
{
    int m_UserDbNum;
    int m_ChaDbNum;    
    DWORD m_AgentSlot;
    char m_IpAddress[MAX_IP_LENGTH+1];
    USHORT m_Port;

    NET_LOBBY_CHAR_INFO_AH(int UserDbNum, int ChaDbNum, DWORD AgentSlot, const char* szIpAddress, USHORT Port)
        : m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
    {        
        dwSize = sizeof(NET_LOBBY_CHAR_INFO_AH);
        nType = NET_MSG_LOBBY_CHAR_INFO_AH;
        StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_AH)<=NET_DATA_BUFSIZE);
    }
};

struct NET_LOBBY_CHAR_INFO_AH_END : public NET_MSG_GENERIC
{
    int m_UserDbNum;
    DWORD m_AgentSlot;
    char m_IpAddress[MAX_IP_LENGTH+1];
    USHORT m_Port;

    NET_LOBBY_CHAR_INFO_AH_END(int UserDbNum, DWORD AgentSlot, const char* szIpAddress, USHORT Port)
        : m_UserDbNum(UserDbNum)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
    {        
        dwSize = sizeof(NET_LOBBY_CHAR_INFO_AH_END);
        nType = NET_MSG_LOBBY_CHAR_INFO_AH_END;
        StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_AH_END)<=NET_DATA_BUFSIZE);
    }
};


//! Database->Cache:Lobby 캐릭터 정보
struct NET_LOBBY_CHAR_INFO_DH //: public NET_MSG_GENERIC
{
    int m_UserDbNum;
    int m_ChaDbNum;
    DWORD m_CacheSlot;
    DWORD m_AgentSlot;
    std::string m_IpAddress; //[MAX_IP_LENGTH+1];
    USHORT m_Port;
    SCHARINFO_LOBBY m_ChaData;

    MSGPACK_DEFINE(
        m_UserDbNum, 
        m_ChaDbNum, 
        m_CacheSlot,
        m_AgentSlot,
        m_IpAddress,
        m_Port,
        m_ChaData);

    NET_LOBBY_CHAR_INFO_DH()
        : m_UserDbNum(0)
        , m_ChaDbNum(0)
        , m_CacheSlot(0)
        , m_AgentSlot(0)    
        , m_Port(0)        
    {
    }

    NET_LOBBY_CHAR_INFO_DH(
        int UserDbNum,
        int ChaDbNum,
        DWORD CacheSlot,
        DWORD AgentSlot,
        const std::string& IpAddress,
        USHORT Port)
        : m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
		, m_CacheSlot(CacheSlot)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
        , m_IpAddress(IpAddress)
    {        
        //dwSize = sizeof(NET_LOBBY_CHAR_INFO_DH);
        //nType = NET_MSG_LOBBY_CHAR_INFO_DH;        
        //MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_DH)<=NET_DATA_BUFSIZE);
    }
};

//! Database->Cache:Lobby 캐릭터 정보
struct NET_LOBBY_CHAR_INFO_NEW_DH //: public NET_MSG_GENERIC
{
    int m_UserDbNum;
    int m_ChaDbNum;
    DWORD m_CacheSlot;
    DWORD m_AgentSlot;
    std::string m_IpAddress; //[MAX_IP_LENGTH+1];
    USHORT m_Port;
    SCHARINFO_LOBBY m_ChaData;

    MSGPACK_DEFINE(
        m_UserDbNum,
        m_ChaDbNum,
        m_CacheSlot,
        m_AgentSlot,
        m_IpAddress,
        m_Port,
        m_ChaData);

    NET_LOBBY_CHAR_INFO_NEW_DH()
        : m_UserDbNum(0)
        , m_ChaDbNum(0)
        , m_CacheSlot(0)
        , m_AgentSlot(0)        
        , m_Port(0)
    {
    }

    NET_LOBBY_CHAR_INFO_NEW_DH(
        int UserDbNum,
        int ChaDbNum,
        DWORD CacheSlot,
        DWORD AgentSlot,
        const char* szIpAddress,
        USHORT Port)
        : m_UserDbNum(UserDbNum)
        , m_ChaDbNum(ChaDbNum)
        , m_CacheSlot(CacheSlot)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
    {        
        //dwSize = sizeof(NET_LOBBY_CHAR_INFO_NEW_DH);
        //nType = NET_MSG_LOBBY_NEW_CHAR_INFO_DH;
        //StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        //MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_NEW_DH)<=NET_DATA_BUFSIZE);
    }
};

//! Cache->Agent:Lobby 캐릭터 정보
struct NET_LOBBY_CHAR_INFO_HA //: public NET_MSG_GENERIC
{
    int m_UserDbNum;
    DWORD m_AgentSlot;
    std::string m_IpAddress; //[MAX_IP_LENGTH+1];
    USHORT m_Port;
    SCHARINFO_LOBBY m_ChaData;

    MSGPACK_DEFINE(m_UserDbNum, m_AgentSlot, m_IpAddress, m_Port, m_ChaData);

    NET_LOBBY_CHAR_INFO_HA()
        : m_UserDbNum(0)
        , m_AgentSlot(0)
        , m_Port(0)
    {
    }

    NET_LOBBY_CHAR_INFO_HA(
        int UserDbNum,
        DWORD AgentSlot,
        const std::string& IpAddress,
        USHORT Port,
        const SCHARINFO_LOBBY& ChaData)
        : m_UserDbNum(UserDbNum)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
        , m_ChaData(ChaData)
        , m_IpAddress(IpAddress)
    {        
        //dwSize = sizeof(NET_LOBBY_CHAR_INFO_HA);
        //nType = NET_MSG_LOBBY_CHAR_INFO_HA;
        //StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        //MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_HA)<=NET_DATA_BUFSIZE);
    }
};

struct NET_LOBBY_CHAR_INFO_HA_END : public NET_MSG_GENERIC
{
    int m_UserDbNum;
    DWORD m_AgentSlot;
    char m_IpAddress[MAX_IP_LENGTH+1];
    USHORT m_Port;

    NET_LOBBY_CHAR_INFO_HA_END(int UserDbNum,DWORD AgentSlot, const char* szIpAddress, USHORT Port)
        : m_UserDbNum(UserDbNum)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
    {        
        dwSize = sizeof(NET_LOBBY_CHAR_INFO_HA_END);
        nType = NET_MSG_LOBBY_CHAR_INFO_HA_END;
        StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_HA_END)<=NET_DATA_BUFSIZE);
    }
};


//! Database->Cache:Lobby 유저 정보
struct NET_LOBBY_USER_INFO_DH : public NET_MSG_GENERIC
{
    int m_UserDbNum;    
    DWORD m_CacheSlot;
    DWORD m_AgentSlot;
    char m_IpAddress[MAX_IP_LENGTH+1];
    USHORT m_Port;
    
    NET_LOBBY_USER_INFO_DH(
        int UserDbNum,
        DWORD CacheSlot,
        DWORD AgentSlot,
        const char* szIpAddress,
        USHORT Port)
        : m_UserDbNum(UserDbNum)        
        , m_CacheSlot(CacheSlot)
        , m_AgentSlot(AgentSlot)
        , m_Port(Port)
    {        
        dwSize = sizeof(NET_LOBBY_USER_INFO_DH);
        nType = NET_MSG_LOBBY_USER_INFO_DH;
        StringCchCopy(m_IpAddress, MAX_IP_LENGTH+1, szIpAddress);
        MIN_STATIC_ASSERT(sizeof(NET_LOBBY_USER_INFO_DH)<=NET_DATA_BUFSIZE);
    }
};

//! Database->Cache:Lobby 캐릭터 정보
struct NET_LOBBY_CHAR_INFO_DH2 //: public NET_MSG_GENERIC
{
    int m_UserDbNum;
    SCHARINFO_LOBBY m_ChaData;

    MSGPACK_DEFINE(m_UserDbNum, m_ChaData);

    NET_LOBBY_CHAR_INFO_DH2()
        : m_UserDbNum(0)
    {
    }

    NET_LOBBY_CHAR_INFO_DH2(
        int UserDbNum,
        const SCHARINFO_LOBBY& ChaData)
        : m_UserDbNum(UserDbNum)
        , m_ChaData(ChaData)
    {        
        //dwSize = sizeof(NET_LOBBY_CHAR_INFO_DH2);
        //nType = NET_MSG_LOBBY_CHAR_INFO_DH2;
        //MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHAR_INFO_DH2)<=NET_DATA_BUFSIZE);
    }
};

//! Database->Cache:Lobby 유저 정보 완료
struct NET_LOBBY_USER_INFO_END_DH : public NET_MSG_GENERIC
{
    int m_UserDbNum;

    NET_LOBBY_USER_INFO_END_DH(
        int UserDbNum)
        : m_UserDbNum(UserDbNum)
    {        
        dwSize = sizeof(NET_LOBBY_USER_INFO_END_DH);
        nType = NET_MSG_LOBBY_USER_INFO_END_DH;        
        MIN_STATIC_ASSERT(sizeof(NET_LOBBY_USER_INFO_END_DH)<=NET_DATA_BUFSIZE);
    }
};

namespace GLMSG
{
	//#pragma pack(1)

	struct SNETPC_BROAD : public NET_MSG_GENERIC
	{
        DWORD dwGaeaID;

		SNETPC_BROAD(DWORD GaeaID=0)
			: dwGaeaID(GaeaID)
		{
		}
	};

	struct SNETCROW_BROAD : public NET_MSG_GENERIC
	{
		EMCROW emCrow;
		DWORD dwID;

        SNETCROW_BROAD(EMCROW Crow=CROW_PC, DWORD GaeaId=0)
			: emCrow(Crow)
			, dwID(GaeaId)
		{
		}
	};

	struct SNETPET_BROAD : public NET_MSG_GENERIC
	{
        DWORD dwGUID;

		SNETPET_BROAD(DWORD Guid=UINT_MAX)
            : dwGUID(Guid)
		{
		}
	};

	struct SNET_COMBINE : public NET_MSG_GENERIC
	{
		enum
		{
			//				최대전송량    - ( 메시지 헤드. )
			EMHEAD_SIZE = ( sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 ),
			EMMAX_SIZE = NET_DATA_BUFSIZE - EMHEAD_SIZE,
		};

		WORD wMSG_SIZE;
		WORD wMSG_CUR;
		BYTE arrayMSG[EMMAX_SIZE];

		SNET_COMBINE() 
			: wMSG_SIZE(0)
			, wMSG_CUR(0)
		{
			dwSize = sizeof(SNET_COMBINE);
			nType = NET_MSG_COMBINE;
            memset(arrayMSG, 0, sizeof(BYTE) * EMMAX_SIZE);
			MIN_STATIC_ASSERT(sizeof(SNET_COMBINE)<=NET_DATA_BUFSIZE);			
		}

		bool IsVALID()
		{
			return wMSG_SIZE!=0;
		}

		void RESET()
		{
			wMSG_SIZE = 0;
			wMSG_CUR = 0;
			memset(arrayMSG, 0, sizeof(BYTE) * EMMAX_SIZE);
		}

		void RESET_CUR ()
		{
			wMSG_CUR = 0;
		}

		bool ADDMSG(LPVOID _pMsg)
		{
			NET_MSG_GENERIC* pMsg = (NET_MSG_GENERIC*) _pMsg;
			
			// 아직 들어갈 공간이 있는지 점검.
			WORD wSIZE = (WORD) pMsg->Size();
			if ((wSIZE+wMSG_SIZE) >= EMMAX_SIZE)
			{
				return false;
			}
			else
			{
				// 메시지를 뒤에 넣는다.
				memcpy ( arrayMSG+wMSG_SIZE, _pMsg, wSIZE );
				// 메시지가 차지한 공간을 계산.
				wMSG_SIZE += wSIZE;
				// 메시지 사이즈 재 설정.
				dwSize = EMHEAD_SIZE + wMSG_SIZE;	
				return true;
			}
		}

		bool POPMSG(LPNET_MSG_GENERIC& pMsg)
		{
			if (wMSG_SIZE<=wMSG_CUR)
                return false;

			pMsg = (NET_MSG_GENERIC*) (arrayMSG + (wMSG_CUR));
			wMSG_CUR += (WORD) pMsg->Size();

			GASSERT(wMSG_SIZE>=wMSG_CUR);
			if (wMSG_SIZE<wMSG_CUR)
                return false;
            else
			    return true;
		}
	};

	// Revert to default structure packing
	//#pragma pack()
};
