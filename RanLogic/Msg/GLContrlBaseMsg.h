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
//	�޼��� ������ ũ�⸦ (NET_DATA_BUFSIZE) byte �̳��� ���� �Ͽ� ����.
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
	EMTAKE_FB_OFF						= 0,	//	�������� ����� ���.
	EMTAKE_FB_RESERVED				= 1,	//	���� �����ڰ� �����Ǿ� ���� ���.
	EMTAKE_FB_INVEN_ERROR			= 2,	//	�κ��丮�� ������ ����.
	EMTAKE_FB_TOO_EVENT_ITEM	= 3,	//	�̹� �̺�Ʈ �������� ������ �ֽ��ϴ�.
	EMTAKE_FB_DISTANCE				= 4,	//	�Ÿ��� �ʹ� �ٴϴ�.
	EMTAKE_FB_TENDERING				= 5,	// ������ �������Դϴ�.
};

enum EMHOLD_FB
{
	EMHOLD_FB_OFF				= 0,	//	������ �������� �������� �ʽ��ϴ�.
	EMHOLD_FB_OK				= 1,	//	�������� HOLD�մϴ�.
	EMHOLD_FB_NONKEEPSTORAGE	= 2,	//	��Ŀ�� ��ǰ�� ���� �� ����.
};

enum EMCHAR_JOIN_FB
{
	EMCJOIN_FB_ERROR		= 0,
	EMCJOIN_FB_NOWLOGIN		= 1,	//	���� �������� ���¶� ������ �ȵ˴ϴ�.
	EMCJOIN_FB_PKPOINT		= 2,	//	pk ������ ���Ƽ� �ش� ä�ο� ���� �Ұ�.
	EMCJOIN_FB_SAVE_CHA		= 3,	//	���� �������� ĳ���Ͱ� �־ ���� �� �� ����.
	EMCJOIN_FB_WAIT			= 4,	//  1.5�� �̳��� ���������� �α��� �޼��� ������ ��� // 2�����
	EMCJOIN_FB_SAVE_CHA2	= 5,	//	�ʵ弭���� ado ������ ���� sqlite ĳ���� ������ �������̰ų�, ���� �� ���� �ִ� �ʵ弭���� ���� ������ ���� �ʾ� ���� �Ұ�
	EMCJOIN_FB_OVERSTAT_RANDOMOPTION	= 6,	//	�����ɼ��� ������ �ִ� ĳ�����̴�.
	EMCJOIN_FB_BLOCK_CLASS = 7, // GS ����, ������ ���������� �����ϰ� ���ӿ� ������ �� ������
};

enum EMDROP_OUT_FORCED
{
	EMDROPOUT_ERROR			= 0,	//	������ ���� ���� ����.
	EMDROPOUT_REQLOGIN		= 1,	//	���� ������ ���� �õ��� ���� ���� ����.
	// �Ʒ� ���� ������, �Ǻ������� ����;
	EMDROPOUT_DISCONNECTED	= 2,	//	������ ������ ������;
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
	EMCHAR_GATEOUT_CLUBBATTLE = 9,			//	�������ʽ� Ŭ����Ʋ
    EMCHAR_GATEOUT_CLUBBATTLE2 = 10,		//	CDM�ʽ� Ŭ����Ʋ
    EMCHAR_GATEOUT_CTF = 11,				//	CTF �������϶� ���� �Ұ�
    EMCHAR_GATEOUT_CDM_MEMS_LIMITED = 12,	// CDM �ο� �ʰ��� ���� �Ұ�.
	EMCHAR_GATEOUT_PARTYMAP = 13,			// ��Ƽ�� ���� �Ұ�
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
	EMSKILL_LEARN_OK		= 0,	//	���� ����.
	EMSKILL_LEARN_MAX		= 1,	//	�� �̻� ������ �� �� ����.
	EMSKILL_LEARN_NOTTERM	= 2,	//	�䱸 ������ �������� �ʾƼ� ������ �Ҽ� ����.
	EMSKILL_LEARN_NOTCLASS	= 3,	//	�μ��� Ʋ���� ���� �����ϴ�.
	EMSKILL_LEARN_NOTBRIGHT	= 4,	//	��/�� ������ �������� �ʽ��ϴ�.
	EMSKILL_LEARN_ALREADY	= 5,	//	�̹� ��� ��ų�Դϴ�.

	EMSKILL_LEARN_UNKNOWN	= 999
};

enum EMLUNCHBOX_FB
{
	EMLUNCHBOX_FB_OK		= 0,	//	��� ����.
	EMLUNCHBOX_FB_FAILED 	= 1,	//	��� ����.

	EMLUNCHBOX_FB_UNKNOWN	= 999
};


enum EMPARTY_LURE_FB
{
	EMPARTY_LURE_OK,					//	��Ƽ ������ ����.
	EMPARTY_LURE_REFUSE,			//	��Ƽ ������ �ź�.	
	EMPARTY_LURE_FULL,				//	��Ƽ�� �������� ���̻� ������ ����.
	EMPARTY_LURE_OTHER,				//	�ٸ� ��Ƽ�� ���ԵǾ� �־ ������.
	EMPARTY_LURE_NOCHAR,			// ���� ���� �ʴ� �ɸ���.
	EMPARTY_LURE_OPT,				//	��Ƽ �ɼ� ����.
	EMPARTY_LURE_OTHERSCHOOL,	//	�ٸ� �п������� ��Ƽ �Ұ���.
	EMPARTY_LURE_CLUBBATTLE,		//	Ŭ����Ʋ ���϶��� ��Ƽ �Ұ�
	EMPARTY_LURE_CTF,				//	CTF ���϶��� ��Ƽ �Ұ�
	EMPARTY_LURE_TIME_OVER,		// ���� �ð� �ʰ�;
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
	EMTRADE_CANCEL_NORMAL	= 0,	//	�Ϲ����� ���.
	EMTRADE_CANCEL_OTHER	= 1,	//	������� �����.
	EMTRADE_CANCEL_MYINVEN	= 2,	//	���� ���� �ι� ����.
	EMTRADE_CANCEL_TARINVEN	= 3,	//	���� ���� �ι� ����.
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
	EMCONFRONT_FAIL				= 0,	//	�Ϲ� ����;
	EMCONFRONT_AGREE			= 1,	//	��� ����(ans��);
	EMCONFRONT_REFUSE			= 2,	//	��� �ź�;
	EMCONFRONT_OPTION			= 3,	//	�ɼ� ����;
	EMCONFRONT_PEACE			= 4,	//	��ȭ ���� ��� �Ұ�;
	EMCONFRONT_MAX				= 5,	//	n �� �̻� ���� ������� time ���� ��� ����;
	EMCONFRONT_ELAP				= 6,	//	���� ����� time ���� ��� ����;
	EMCONFRONT_ALREADY_ME	= 7,	//	�ڽ��� �̹� �����;
	EMCONFRONT_ALREADY_TAR	= 8,	//	������ �̹� �����;
	EMCONFRONT_PARTY			= 9,	//	���� ��Ƽ�� ������ ��� �Ұ�;
	EMCONFRONT_DIE_YOU			= 10,	//	��� ĳ���Ͱ� �׾��־ ��� �Ұ�;
	EMCONFRONT_DIE_ME			= 11,	//	�ڽ��� ĳ���Ͱ� �׾��־ ��� �Ұ�;

	EMCONFRONT_MENOTPARTY	= 12,	//	��Ƽ���� �ƴϿ��� ��Ƽ ����� �Ұ���;
	EMCONFRONT_MENOTMASTER	= 13,	//	��Ƽ���� �ƴϿ��� [��Ƽ���]�� ��û�ϽǼ� �����ϴ�;
	EMCONFRONT_TARMASTER		= 14,	//	��Ƽ���� �ƴϿ��� [��Ƽ���]�� ��û�ϽǼ� �����ϴ�;
	EMCONFRONT_VEHICLE			= 15,	//	Ż���� Ÿ�� �־ ��� �Ұ�;
	EMCONFRONT_EXPEDITION		= 16, // ������ �Ἲ�߿��� ��� �Ұ�;

	EMCONFRONT_NONCONFRONTMODE	= 17,	//  ��� �Ұ��� ��尡 �������� or ����� �Ұ����� ����
};

enum EMCONFRONT_END
{
	EMCONFRONT_END_FAIL		= 0,	//	�Ϲ� ����.
	EMCONFRONT_END_WIN		= 1,	//	�¸�.
	EMCONFRONT_END_LOSS		= 2,	//	�й�.
	EMCONFRONT_END_TIME		= 3,	//	�ð� �ʰ�.
	EMCONFRONT_END_MOVEMAP	= 4,	//	�� �̵�.
	EMCONFRONT_END_DISWIN	= 5,	//	�Ÿ��� �ʹ� ������.
	EMCONFRONT_END_DISLOSS	= 6,	//	�Ÿ��� �ʹ� ������.

	EMCONFRONT_END_PARTY	= 7,	//	��� ���� ���� ��Ƽ���� �Ǿ �����.

	EMCONFRONT_END_NOTWIN	= 10,	//	�¸��� �⿩���� ����.
	EMCONFRONT_END_PWIN		= 11,	//	��Ƽ��ÿ� �¸�.
	EMCONFRONT_END_PLOSS	= 12,	//	��Ƽ��ÿ� �й�.
	EMCONFRONT_END_PTIME	= 13,	//	��Ƽ��� �ð��ʰ�.

	EMCONFRONT_END_CWIN		= 14,	//	Ŭ����ÿ� �¸�.
	EMCONFRONT_END_CLOSS	= 15,	//	Ŭ����ÿ� �й�.
	EMCONFRONT_END_CTIME	= 16,	//	Ŭ����� �ð��ʰ�.
};

enum EMGRINDINGRESULT
{
	EMGRINDINGRESULT_FAILED					   = 0x0000, //����
	EMGRINDINGRESULT_SUCCESS	               = 0x0001, //����
	EMGRINDINGRESULT_ANTIDISAPPEAR             = 0x0002, //�ҹ��� ���
	EMGRINDINGRESULT_INC_GRINDINGRATE		   = 0x0004, //��ȭ�� ��� 
	EMGRINDINGRESULT_ANTIRESET			       = 0x0008, //��ȭ�� ��� 
	EMGRINDINGRESULT_RESET					   = 0x0010, //�ʱ�ȭ �� 
	EMGRINDINGRESULT_TERMINATE				   = 0x0020, //�ı� ��
};

enum EMBUYREBUYITEM
{
    EMBUYREBUYITEM_FAILED = 0,              // �Ϲ��� ����.
    EMBUYREBUYITEM_NOT_ENOUGH_MONEY = 1,    // �� ����.
    EMBUYREBUYITEM_CAN_NOT_BUY_ITEM = 2,    // �� �� ���� ������.
    EMBUYREBUYITEM_NOT_ENOUGH_INVEN = 3,    // �κ� ���� ����.
    EMBUYREBUYITEM_OVER_DURATION = 4,       // ��ȿ�Ⱓ �ʰ�.
    EMBUYREBUYITEM_SUCCESS = 5,             // ����.
};

// ĳ���� �κ� ���÷��̿� ������
struct SCHARINFO_LOBBY
{
	DWORD				m_dwCharID;
    DWORD               m_ClubDbNum; //! Ŭ�� ��ȣ

	//EMCHARCLASS			m_emClass;					// ����. (����)
    DWORD			    m_emClass;					// ����. (����) EMCHARCLASS
	WORD				m_wSchool;					// �п�.
	WORD				m_wRebornNum;					// �п�.
	WORD				m_wHair;					// ��Ÿ��
	WORD				m_wFace;					// �󱼸��
	WORD                m_wSex;                     // ���� (�߰�:2005-12-29 Jgkim)
	WORD                m_wHairColor;               // �Ӹ� ���� (�߰�:2005-12-29 Jgkim)

	GLPADATA			m_sHP;						//	������. ( ����/�ִ뷮 )
	GLLLDATA			m_sExperience;				//	����ġ. ( ����/�������뵵�ް� )
	int					m_nBright;					//	�Ӽ�.

	WORD				m_wLevel;					//	����.
	SCHARSTATS			m_sStats;					//	Stats.

	//SITEM_LOBY			m_PutOnItems[SLOT_TSIZE];	//	���� ����.
    std::vector<SITEM_LOBY> m_PutOnItems;

	SNATIVEID			m_sSaveMapID;				//	���� ��.
    
    DWORD               m_ClubRank;                 //! Ŭ�� ��ũ
    std::string			m_ChaName; //[CHAR_SZNAME];		// �̸�. (����)
    std::string         m_ClubName; //[CLUB_NAME_LENGTH]; //! Ŭ���̸�

    bool                m_Lock;                     //! ����ִ°�? (�������), ����ִ� ĳ���ʹ� �÷��� �Ұ�. ��� ǥ�ø� �Ѵ�.

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

        // ������ �����ϱ� ���ؼ� �̸� SLOT_TSIZE ��ŭ �־�д�.
        m_PutOnItems.reserve(SLOT_TSIZE);
        SITEM_LOBY LobyItem;
        for (size_t i=0; i<SLOT_TSIZE; ++i)
            m_PutOnItems.push_back(LobyItem);
	}

    void SetName(const std::string& ChaName)
    {
        m_ChaName = ChaName;
    }

    //! ����ִ°�? (�������)
    inline bool IsLock() const { return m_Lock; }
    inline DWORD ChaDbNum() const { return m_dwCharID; }

    //! Ŭ�� ��ũ
    inline DWORD GetClubRank() const { return m_ClubRank; }
    inline const char* GetClubName() const { return m_ClubName.c_str(); }
    inline void SetClubName(const char* szName)
    {
        if (szName)
            m_ClubName = szName;
    }
};

//! Agent->Cache:ĳ���� ���� ��û
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


//! Database->Cache:Lobby ĳ���� ����
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

//! Database->Cache:Lobby ĳ���� ����
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

//! Cache->Agent:Lobby ĳ���� ����
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


//! Database->Cache:Lobby ���� ����
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

//! Database->Cache:Lobby ĳ���� ����
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

//! Database->Cache:Lobby ���� ���� �Ϸ�
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
			//				�ִ����۷�    - ( �޽��� ���. )
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
			
			// ���� �� ������ �ִ��� ����.
			WORD wSIZE = (WORD) pMsg->Size();
			if ((wSIZE+wMSG_SIZE) >= EMMAX_SIZE)
			{
				return false;
			}
			else
			{
				// �޽����� �ڿ� �ִ´�.
				memcpy ( arrayMSG+wMSG_SIZE, _pMsg, wSIZE );
				// �޽����� ������ ������ ���.
				wMSG_SIZE += wSIZE;
				// �޽��� ������ �� ����.
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