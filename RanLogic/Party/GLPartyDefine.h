#pragma once

#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../Item/GLItemCustom.h"
#include "../Character/GLCharDefine.h"

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH	33
#endif

// 에러 발생시 로그 출력 켬/끔; 주석 처리하면 꺼짐;
// RanLogicServer/Party/GGLPartyManagerAgent/SendErrorMsg()에서 사용;
#define DEFPARTY_DEBUG_LOG_ON 

#define SMALL_CHAR_SZNAME 16 // 실제로 입력가능한 캐릭터명이다;
#define DEFPARTY_NULL (0xffffffff)
#define DEFSLOT_NULL (0xffffffff)
#define MAXPARTY 8
#define MAX_PARTYGROUP 5 // 원정대당 최대 파티 수;
#define MASTER_PARTY 0 // 파티내에서 파티장의 인덱스;

#define TENDER_ITEM_REFRESH_TIME 30.0f
#define MAX_TENDER_ITEM 4096 // 파티/원정대 당 입찰 아이템 등록 가능한 갯수;
#define MAX_TENDERABLE_ITEM 32 // 한번에 입찰 가능한 갯수;

// 필드서버에서 위치, 체력등 빠른 갱신이 이루어지는 변수들에 대한 갱신 시간;
#define DEFPARTY_INFORM_REFRESH_TIME 500 


// 파티 아이템/돈 분배 방식;
enum EMPARTY_ITEM_OPTION
{
	EMPARTY_ITEM_DEFAULT,			// 기여자 우선;
	EMPARTY_ITEM_FREE,				// 자유획득;
	EMPARTY_ITEM_LEADER,			// 파티/원정대장(획득);
	EMPARTY_ITEM_ROUND,			// 순차습득;

	// 파티 리뉴얼에 새로 추가된 옵션 두개;
	// 기존 파티에 대한 작업은 EMPARTY_ITEM_NSIZE 대신 EMPARTY_ITEM_ROUND 사용;
	EMPARTY_ITEM_LEADER_SHARE,	// 파티/원정대장(분배);
	EMPARTY_ITEM_DICE,				// 주사위;	
	EMPARTY_ITEM_NSIZE,
};

enum EMPARTY_MONEY_OPTION
{
	EMPARTY_MONEY_DEFAULT	= 0,
	EMPARTY_MONEY_FREE			= 1, // 자유획득;
	EMPARTY_MONEY_LEADER		= 2, // 파티 리더 우선권;
	EMPARTY_MONEY_EQUAL		= 3, // 돈, 균등분배;
	EMPARTY_MONEY_NSIZE		= 4
};

enum EMPARTY_MODE_TYPE
{
	EMPARTY_MODE_PARTY = 0, // 파티;
	EMPARTY_MODE_EXPEDITION, // 원정대;
	EMPARTY_MODE_NSIZE,
};

enum EMPARTY_AUTHORITY_FLAG
{
	EMPARTY_AUTHORITY_NULL = 0x00,
	EMPARTY_AUTHORITY_PARTY_MASTER = 0x01, // 파티장;	
	EMPARTY_AUTHORITY_EXPEDITION_MASTER = 0x02, // 원정대장;
	EMPARTY_AUTHORITY_EXPEDITION_SUB_MASTER = 0x04, // 원정대 부장;	
	EMPARTY_AUTHORITY_ALL = 0xffffffff
};

enum EMPARTY_RENEWAL_WORD
{
	EMPARTY_RENEWAL_WORD_ITEM = 0, // 아이템;
	EMPARTY_RENEWAL_WORD_ITEM_FILTER = 1, // 아이템 필터;
	EMPARTY_RENEWAL_WORD_MONEY = 2, // 게임머니;
	EMPARTY_RENEWAL_WORD_DISBAND_PARTY = 3, // 파티 해산;
	EMPARTY_RENEWAL_WORD_SHOWBUFF = 4, // 구성원 버프 보기;
	EMPARTY_RENEWAL_WORD_EXPULSION = 5, // 추방;
	EMPARTY_RENEWAL_WORD_MANDATE = 6, // 위임;
	EMPARTY_RENEWAL_WORD_CONSTRUCT_EXPEDITION =  7, // 원정대 구성;
	EMPARTY_RENEWAL_WORD_SECEDE = 8,  // 탈퇴;
	EMPARTY_RENEWAL_WORD_SHOOL = 9, // 구성 학원;
	EMPARTY_RENEWAL_WORD_INVITE = 10, // 초대;
	EMPARTY_RENEWAL_WORD_DISBAND_EXPEDITION = 11, // 원정대 해산;
	EMPARTY_RENEWAL_WORD_SHOWINFO = 12,
	EMPARTY_RENEWAL_WORD_NSIZE,	
};

enum EMPARTY_ITEM_FILTER_OPTION
{
	EMPARTY_ITEM_FILTER_ALL = 0,			//전체;는 사용되는곳이 없지만 아이템 등급과 비교됨으로 지우면안된다;
	EMPARTY_ITEM_FILTER_MIDTOHIGH = 1,		//영웅;
	EMPARTY_ITEM_FILTER_HIGH = 2,			//고대;
	EMPARTY_ITEM_FILTER_HIGHEST = 3,		//전설;
	EMPARTY_ITEM_FILTER_NSIZE,
};

enum EMEXPEDITION_SCHOOL_OPTION
{
//  	EMEXPEDITION_SCHOOL_OPTION_ALL = 0, // 전체;	
//  	EMEXPEDITION_SCHOOL_OPTION_SM = 1, // 성문;
//  	EMEXPEDITION_SCHOOL_OPTION_HA = 2, // 현암;
//  	EMEXPEDITION_SCHOOL_OPTION_BH = 3, // 봉황;
//  	EMEXPEDITION_SCHOOL_OPTION_NSIZE = 4,
	//  [1/10/2013 gbgim];
	// - 파티할 때 학원 검사를 해야되는 경우가 있는데 옵션은 1부터 학원이고;
	//   타겟의 학원 정보(school)은 0부터 학원이기 때문에 비교시에 학원 정보에;
	//   +1하던 단점이 있어서 다음과 같이 수정,( GameWord.xml Element:EXPEDITION_INFO_SCHOOL_TEXT 수정);
 	EMEXPEDITION_SCHOOL_OPTION_SM = 0, // 성문;
 	EMEXPEDITION_SCHOOL_OPTION_HA = 1, // 현암;
 	EMEXPEDITION_SCHOOL_OPTION_BH = 2, // 봉황;
	EMEXPEDITION_SCHOOL_OPTION_ALL = 3, // 전체;	
 	EMEXPEDITION_SCHOOL_OPTION_NSIZE = 4,
};

enum EMPARTY_TENDER_STATE_TYPE
{
	EMPARTY_TENDER_STATE_TENDER, // 입찰 완료;
	EMPARTY_TENDER_STATE_NONEAVAILABLE, // 입찰 불가능;
	EMPARTY_TENDER_STATE_FREE, // 입찰 가능;
	EMPARTY_TENDER_STATE_NSIZE
};

//  [1/11/2013 gbgim]
// - 파티가입 경로중 링크를 통한 파티가입과 파티장이 직접 초대해서 가입하는 방식;
//   2가지가 있는데 기존에 두가지 타입을 둘다 둬서 문제가 생기면 둘다 수정해줘야되는;
//   부분이 있어서 같은 부분은 하나로 묶어서 처리하기위해 따로 만듬;
enum EMPARTY_ERROR_TYPE
{
	EMPARTY_ERROR_OK, // 성공;
	EMPARTY_ERROR_NOCHAR, // 존재 하지 않는 케릭터; 해킹
	EMPARTY_ERROR_NOPARTY, // 존재 하지 않는 파티;
	EMPARTY_ERROR_NOEXPEDITION, // 존재 하지 않는 원정대;
	EMPARTY_ERROR_NOMEMBER, // 해당 파티/원정대원이 아님;

	//  [1/10/2013 gbgim]
	EMPARTY_ERROR_NOMASTER,				//! 존재하지 않는 마스터(해킹);
	EMPARTY_ERROR_LINK_FULLPARTY,		//! 파티 인원이 가득참;
	EMPARTY_ERROR_LURE_FULLPARTY,
	EMPARTY_ERROR_LINK_REJOIN,			//! 이미 다른 파티에 가입되어 있음;
	EMPARTY_ERROR_LURE_REJOIN,
	EMPARTY_ERROR_LINK_CLUBBATTLE,		//! 클럽 배틀 중이라서 가입 안됨;
	EMPARTY_ERROR_LURE_CLUBBATTLE,
	EMPARTY_ERROR_LINK_CTF,				//! 점령전 중이라서 파티 안됨;
	EMPARTY_ERROR_LURE_CTF,
	EMPARTY_ERROR_OTHERSCHOOL,			//! 다른 학원이라서 파티 안됨;
	EMPARTY_ERROR_PRISON,

};

enum EMPARTY_MEMBER_STATE_TYPE
{
	EMPARTY_MEMBER_STATE_NULL = 0x0000,
	EMPARTY_MEMBER_STATE_CONFRONT = 0x0001, // 대련중;
	EMPARTY_MEMBER_STATE_SELECTED = 0x0002, // 선택됨;
	EMPARTY_MEMBER_STATE_READY = 0x0004, // 준비 확인;
	EMPARTY_MEMBER_STATE_TAKEITEM = 0x0008, // 아이템 획득;	
};

enum EMEXPEDITION_ERROR
{
	EMEXPEDITION_ERROR_NONE,
	EMEXPEDITION_ERROR_NEW_ALREADY,		// 원정대 생성 오류(이미 원정대임);
	EMEXPEDITION_ERROR_NEW_PARTYMAP,	// 원정대 생성 오류(파티맵);
	EMEXPEDITION_ERROR_INVALID_LAND,		// 원정대 생성 오류(잘못된 지역);
};

class SPartyOption
{
public:
	const bool isValidOption(void) const; // 유효한 옵션인지 확인;	

	const EMPARTY_ITEM_OPTION GetItemOption(void) const;
	const EMPARTY_MONEY_OPTION GetMoneyOption(void) const;	
	const EMPARTY_ITEM_FILTER_OPTION GetItemFilter(void) const;
	void SetItemOption(const EMPARTY_ITEM_OPTION _emItemOption);
	void SetMoneyOption(const EMPARTY_MONEY_OPTION _emMoneyOption);	
	void SetItemFilter(const EMPARTY_ITEM_FILTER_OPTION _emItemFilter);
	void Reset(void);

public:
	const bool operator != (const SPartyOption& sPartyOption) const;
	const bool operator == (const SPartyOption& sPartyOption) const;
	SPartyOption(void);
	SPartyOption(const DWORD _emItemOption, const DWORD _emMoneyOption);
	SPartyOption(const EMPARTY_ITEM_OPTION _emItemOption, const EMPARTY_MONEY_OPTION _emMoneyOption);

private:
	EMPARTY_ITEM_OPTION emItemOption;				// 아이템 습득 옵션;
	EMPARTY_MONEY_OPTION emMoneyOption;				// 게임머니 습득 옵션;
	EMPARTY_ITEM_FILTER_OPTION emItemFilterLevel;	// 아이템 필터 레벨;
};

class GLPartyID
{	
public:
	enum { INVALID = 0 };
public:
	//	MSGPACK_DEFINE(dwId);
	void Reset(void);	
	const bool isValidExpedition(void) const; // 원정대에 속해 있는가;
	const bool isValidParty(void) const; // 파티에 속해 있는가;	

	const DWORD GetID(void) const;
	const DWORD GetPartyID(void) const;
	const DWORD GetExpeditionID(void) const;	

	void SetPartyID(const WORD _wPartyID);
	void SetExpeditionID(const WORD _wExpeditionID);
	void SetPartyID(const DWORD dwPartyID);
	void SetExpeditionID(const DWORD dwExpeditionID);

public:
	const GLPartyID& operator =(const DWORD& dwPartyID);	
	const bool operator <(const GLPartyID& PartyID) const;
	const bool operator >(const GLPartyID& PartyID) const;
	const bool operator !=(const GLPartyID& PartyID) const;
	const bool operator ==(const GLPartyID& PartyID) const;
	const bool operator !=(const DWORD& dwPartyID) const;
	const bool operator ==(const DWORD& dwPartyID) const;
	GLPartyID(const GLPartyID& PartyID);
	GLPartyID(const DWORD& dwPartyID, const DWORD& dwExpeditionID);	
	GLPartyID(const DWORD& dwPartyID);		
	GLPartyID(void);

public:  // 메세지팩 이용을 위해 public으로 해두었지만, 절대 직접 접근 하지 말자;		
		WORD wPartyID; // 파티 ID;
		WORD wExpeditionID; // 원정대에 속한 경우 원정대 ID;			
	
	MSGPACK_DEFINE(wPartyID, wExpeditionID);
	// 파티/원정대에 속하지 않은 경우 wPartyID = INVALID, wExpeditionID = INVALID;
	// 파티에만 속한 경우 : wPartyID = 임의 값, wExpeditionID = INVALID;
	// 원정대에 속한 경우 : wPartyID = 임의 값, wExpeditionID = 임의 값;
};

//! 파티모집 링크에 사용할 데이터;
struct GLPARTY_LINK_MEM
{		
    DWORD	dwGaeaID;
    DWORD	emClass;						// EMCHARCLASS

	WORD	wLevel;	
    char	m_Name[SMALL_CHAR_SZNAME+1];

    //MSGPACK_DEFINE(dwGaeaID, emClass, m_Name);

    GLPARTY_LINK_MEM()
        : dwGaeaID(UINT_MAX)
        , emClass(GLCC_FIGHTER_M)
		, wLevel( 0 )
    {
        memset(m_Name, 0, sizeof(char) * (SMALL_CHAR_SZNAME+1));
    }

    void RESET()
    {
        dwGaeaID = UINT_MAX;
        emClass = GLCC_NONE;        
		wLevel = 0;
        memset(m_Name, 0, sizeof(char) * (SMALL_CHAR_SZNAME+1));
    }

    BOOL VALID() const
    {
        if (dwGaeaID != GAEAID_NULL)
            return TRUE;
        return FALSE;
    }

    GLPARTY_LINK_MEM& operator = (const GLPARTY_LINK_MEM& rhs)
    {
        dwGaeaID = rhs.dwGaeaID;
        emClass  = rhs.emClass;
		wLevel = rhs.wLevel;
        StringCchCopy(m_Name, SMALL_CHAR_SZNAME+1, rhs.m_Name);
        return *this;
    }

    void SetName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_Name, SMALL_CHAR_SZNAME+1, szName);
    }

    void SetName(const std::string& Name)
    {        
        StringCchCopy(m_Name, SMALL_CHAR_SZNAME+1, Name.c_str());
    }
};

struct GLPARTY_LINK
{
    SNATIVEID		sMapID;	
    GLPARTY_LINK_MEM sPartyMem[MAXPARTY];
    BOOL        bOnce;
	DWORD			dwHashKey;
	DWORD			dwTimeKey;

    GLPARTY_LINK()
        : sMapID   (false)		
        , bOnce    (FALSE)
		, dwHashKey(0)
		, dwTimeKey(0)
    {
    }
};

struct GLPARTY_FNET
{
    DWORD			m_dwGaeaID;
	DWORD			m_dwDbNum;
    EMCHARCLASS	m_emClass;	
	WORD			m_wSchool;
	WORD			m_wLevel;

    char			m_szName[CHAR_SZNAME];

    bool			m_bConfront;
    MapID		m_sMapID;

	const bool operator ==(const GLPARTY_FNET& rhs)
	{
		return m_dwGaeaID == rhs.m_dwGaeaID;
	}
	GLPARTY_FNET& operator =(const GLPARTY_FNET& rhs)
	{
		m_dwGaeaID = rhs.m_dwGaeaID;
		m_dwDbNum = rhs.m_dwDbNum;
		m_emClass = rhs.m_emClass;
		m_wSchool = rhs.m_wSchool;
		m_bConfront = rhs.m_bConfront;
		m_sMapID = rhs.m_sMapID;		
		m_wLevel = rhs.m_wLevel;	
		::StringCchCopy( m_szName, CHAR_SZNAME, rhs.m_szName );
		return *this;
	}

    GLPARTY_FNET(void) 
        : m_dwGaeaID(GAEAID_NULL)
		, m_dwDbNum(DBNUM_NULL)
        , m_emClass(GLCC_FIGHTER_M)
		, m_wSchool(0)
		, m_wLevel(0)
        , m_bConfront(true)
    {
        memset(m_szName, 0, sizeof(char) * (CHAR_SZNAME));
    }

    GLPARTY_FNET(const DWORD dwGaeaID, const DWORD dwDbNum, const char* const szName, const WORD wSchool, const WORD wLevel, const EMCHARCLASS emClass, const MapID& sMapID )
		: m_dwGaeaID(dwGaeaID)
		, m_dwDbNum(dwDbNum)
		, m_emClass(emClass)
		, m_wSchool(wSchool)
		, m_wLevel(wLevel)
		, m_sMapID(sMapID)
    {
        ::StringCchCopy( m_szName, CHAR_SZNAME, szName );
    }

	const DWORD GetGaeaID(void) const
	{
		return m_dwGaeaID;
	}
	const DWORD& GetChaDbNum(void) const
	{
		return m_dwDbNum;
	}
	const DWORD GetClass(void) const
	{
		return m_emClass;
	}
	const char* const GetName(void) const
	{
		return m_szName;
	}	
	void SetGaeaID(const DWORD dwGaeaID)
	{
		m_dwGaeaID = dwGaeaID;	
	}
	void SetChaDbNum(const DWORD& dwDbNum)
	{
		m_dwDbNum = dwDbNum;
	}
	void SetClass(const DWORD emClass)
	{
		m_emClass = EMCHARCLASS(emClass);
	}
	void SetName(const char* const szName)
	{
		::StringCchCopy( m_szName, CHAR_SZNAME, szName );
	}
};

class GLExpedition
{
public:
	const bool isMaster(const DWORD dwGaeaID) const;
	const bool isSubMaster(const DWORD dwGaeaID) const;
	const DWORD GetMasterGaeaID(void) const;
	const DWORD GetMasterGaeaID(const DWORD dwPartyIndex) const;
	const DWORD GetPartySlotIndex(const GLPartyID& PartyID) const;
	const DWORD GetExpeditionID(void) const;
	const GLPartyID& GetPartyID(const DWORD dwIndex) const;

	void SetMasterGaeaID(const DWORD dwGaeaID, const DWORD dwPartySlotIndex);
	void SetExpeditionID(const DWORD dwExpeditionID);
	void SetPartyID(const GLPartyID PartyIDs[MAX_PARTYGROUP]);

	void Reset(void);

public:
	GLExpedition(void);
	virtual ~GLExpedition(void);

private:		
	GLPartyID m_PartyIDs[MAX_PARTYGROUP];
	DWORD m_dwMasterGaeaIDs[MAX_PARTYGROUP];
	DWORD m_dwExpeditionID;
};

struct GLTenderItemCustom
{
public:
	GLTenderItemCustom(const MapID& mapID, const float fInsertTime, const D3DXVECTOR3& vGeneratePosition);

	const MapID m_mapID;
	const D3DXVECTOR3 m_vGeneratePosition;	
	const float m_fInsertTime;	
};

struct GLSwapSlotInformation
{
	DWORD dwGaeaID;
	DWORD nPartyIndex;
	GLSwapSlotInformation(void)
	{
	}
	GLSwapSlotInformation(const DWORD _dwGaeaID, const DWORD _dwPartyIndex)
		: dwGaeaID(_dwGaeaID)
		, nPartyIndex(_dwPartyIndex)
	{
	}
};

class GLPartyMemberGaeaIDs
{
public:
	DWORD dwGaeaIDs[MAXPARTY * MAX_PARTYGROUP];
	DWORD nMember;
};
