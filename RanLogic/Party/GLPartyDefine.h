#pragma once

#include "../../EngineLib/DxLand/DxLandDef.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../Item/GLItemCustom.h"
#include "../Character/GLCharDefine.h"

#ifndef CHR_ID_LENGTH
#define CHR_ID_LENGTH	33
#endif

// ���� �߻��� �α� ��� ��/��; �ּ� ó���ϸ� ����;
// RanLogicServer/Party/GGLPartyManagerAgent/SendErrorMsg()���� ���;
#define DEFPARTY_DEBUG_LOG_ON 

#define SMALL_CHAR_SZNAME 16 // ������ �Է°����� ĳ���͸��̴�;
#define DEFPARTY_NULL (0xffffffff)
#define DEFSLOT_NULL (0xffffffff)
#define MAXPARTY 8
#define MAX_PARTYGROUP 5 // ������� �ִ� ��Ƽ ��;
#define MASTER_PARTY 0 // ��Ƽ������ ��Ƽ���� �ε���;

#define TENDER_ITEM_REFRESH_TIME 30.0f
#define MAX_TENDER_ITEM 4096 // ��Ƽ/������ �� ���� ������ ��� ������ ����;
#define MAX_TENDERABLE_ITEM 32 // �ѹ��� ���� ������ ����;

// �ʵ弭������ ��ġ, ü�µ� ���� ������ �̷������ �����鿡 ���� ���� �ð�;
#define DEFPARTY_INFORM_REFRESH_TIME 500 


// ��Ƽ ������/�� �й� ���;
enum EMPARTY_ITEM_OPTION
{
	EMPARTY_ITEM_DEFAULT,			// �⿩�� �켱;
	EMPARTY_ITEM_FREE,				// ����ȹ��;
	EMPARTY_ITEM_LEADER,			// ��Ƽ/��������(ȹ��);
	EMPARTY_ITEM_ROUND,			// ��������;

	// ��Ƽ ������ ���� �߰��� �ɼ� �ΰ�;
	// ���� ��Ƽ�� ���� �۾��� EMPARTY_ITEM_NSIZE ��� EMPARTY_ITEM_ROUND ���;
	EMPARTY_ITEM_LEADER_SHARE,	// ��Ƽ/��������(�й�);
	EMPARTY_ITEM_DICE,				// �ֻ���;	
	EMPARTY_ITEM_NSIZE,
};

enum EMPARTY_MONEY_OPTION
{
	EMPARTY_MONEY_DEFAULT	= 0,
	EMPARTY_MONEY_FREE			= 1, // ����ȹ��;
	EMPARTY_MONEY_LEADER		= 2, // ��Ƽ ���� �켱��;
	EMPARTY_MONEY_EQUAL		= 3, // ��, �յ�й�;
	EMPARTY_MONEY_NSIZE		= 4
};

enum EMPARTY_MODE_TYPE
{
	EMPARTY_MODE_PARTY = 0, // ��Ƽ;
	EMPARTY_MODE_EXPEDITION, // ������;
	EMPARTY_MODE_NSIZE,
};

enum EMPARTY_AUTHORITY_FLAG
{
	EMPARTY_AUTHORITY_NULL = 0x00,
	EMPARTY_AUTHORITY_PARTY_MASTER = 0x01, // ��Ƽ��;	
	EMPARTY_AUTHORITY_EXPEDITION_MASTER = 0x02, // ��������;
	EMPARTY_AUTHORITY_EXPEDITION_SUB_MASTER = 0x04, // ������ ����;	
	EMPARTY_AUTHORITY_ALL = 0xffffffff
};

enum EMPARTY_RENEWAL_WORD
{
	EMPARTY_RENEWAL_WORD_ITEM = 0, // ������;
	EMPARTY_RENEWAL_WORD_ITEM_FILTER = 1, // ������ ����;
	EMPARTY_RENEWAL_WORD_MONEY = 2, // ���ӸӴ�;
	EMPARTY_RENEWAL_WORD_DISBAND_PARTY = 3, // ��Ƽ �ػ�;
	EMPARTY_RENEWAL_WORD_SHOWBUFF = 4, // ������ ���� ����;
	EMPARTY_RENEWAL_WORD_EXPULSION = 5, // �߹�;
	EMPARTY_RENEWAL_WORD_MANDATE = 6, // ����;
	EMPARTY_RENEWAL_WORD_CONSTRUCT_EXPEDITION =  7, // ������ ����;
	EMPARTY_RENEWAL_WORD_SECEDE = 8,  // Ż��;
	EMPARTY_RENEWAL_WORD_SHOOL = 9, // ���� �п�;
	EMPARTY_RENEWAL_WORD_INVITE = 10, // �ʴ�;
	EMPARTY_RENEWAL_WORD_DISBAND_EXPEDITION = 11, // ������ �ػ�;
	EMPARTY_RENEWAL_WORD_SHOWINFO = 12,
	EMPARTY_RENEWAL_WORD_NSIZE,	
};

enum EMPARTY_ITEM_FILTER_OPTION
{
	EMPARTY_ITEM_FILTER_ALL = 0,			//��ü;�� ���Ǵ°��� ������ ������ ��ް� �񱳵����� �����ȵȴ�;
	EMPARTY_ITEM_FILTER_MIDTOHIGH = 1,		//����;
	EMPARTY_ITEM_FILTER_HIGH = 2,			//���;
	EMPARTY_ITEM_FILTER_HIGHEST = 3,		//����;
	EMPARTY_ITEM_FILTER_NSIZE,
};

enum EMEXPEDITION_SCHOOL_OPTION
{
//  	EMEXPEDITION_SCHOOL_OPTION_ALL = 0, // ��ü;	
//  	EMEXPEDITION_SCHOOL_OPTION_SM = 1, // ����;
//  	EMEXPEDITION_SCHOOL_OPTION_HA = 2, // ����;
//  	EMEXPEDITION_SCHOOL_OPTION_BH = 3, // ��Ȳ;
//  	EMEXPEDITION_SCHOOL_OPTION_NSIZE = 4,
	//  [1/10/2013 gbgim];
	// - ��Ƽ�� �� �п� �˻縦 �ؾߵǴ� ��찡 �ִµ� �ɼ��� 1���� �п��̰�;
	//   Ÿ���� �п� ����(school)�� 0���� �п��̱� ������ �񱳽ÿ� �п� ������;
	//   +1�ϴ� ������ �־ ������ ���� ����,( GameWord.xml Element:EXPEDITION_INFO_SCHOOL_TEXT ����);
 	EMEXPEDITION_SCHOOL_OPTION_SM = 0, // ����;
 	EMEXPEDITION_SCHOOL_OPTION_HA = 1, // ����;
 	EMEXPEDITION_SCHOOL_OPTION_BH = 2, // ��Ȳ;
	EMEXPEDITION_SCHOOL_OPTION_ALL = 3, // ��ü;	
 	EMEXPEDITION_SCHOOL_OPTION_NSIZE = 4,
};

enum EMPARTY_TENDER_STATE_TYPE
{
	EMPARTY_TENDER_STATE_TENDER, // ���� �Ϸ�;
	EMPARTY_TENDER_STATE_NONEAVAILABLE, // ���� �Ұ���;
	EMPARTY_TENDER_STATE_FREE, // ���� ����;
	EMPARTY_TENDER_STATE_NSIZE
};

//  [1/11/2013 gbgim]
// - ��Ƽ���� ����� ��ũ�� ���� ��Ƽ���԰� ��Ƽ���� ���� �ʴ��ؼ� �����ϴ� ���;
//   2������ �ִµ� ������ �ΰ��� Ÿ���� �Ѵ� �ּ� ������ ����� �Ѵ� ��������ߵǴ�;
//   �κ��� �־ ���� �κ��� �ϳ��� ��� ó���ϱ����� ���� ����;
enum EMPARTY_ERROR_TYPE
{
	EMPARTY_ERROR_OK, // ����;
	EMPARTY_ERROR_NOCHAR, // ���� ���� �ʴ� �ɸ���; ��ŷ
	EMPARTY_ERROR_NOPARTY, // ���� ���� �ʴ� ��Ƽ;
	EMPARTY_ERROR_NOEXPEDITION, // ���� ���� �ʴ� ������;
	EMPARTY_ERROR_NOMEMBER, // �ش� ��Ƽ/��������� �ƴ�;

	//  [1/10/2013 gbgim]
	EMPARTY_ERROR_NOMASTER,				//! �������� �ʴ� ������(��ŷ);
	EMPARTY_ERROR_LINK_FULLPARTY,		//! ��Ƽ �ο��� ������;
	EMPARTY_ERROR_LURE_FULLPARTY,
	EMPARTY_ERROR_LINK_REJOIN,			//! �̹� �ٸ� ��Ƽ�� ���ԵǾ� ����;
	EMPARTY_ERROR_LURE_REJOIN,
	EMPARTY_ERROR_LINK_CLUBBATTLE,		//! Ŭ�� ��Ʋ ���̶� ���� �ȵ�;
	EMPARTY_ERROR_LURE_CLUBBATTLE,
	EMPARTY_ERROR_LINK_CTF,				//! ������ ���̶� ��Ƽ �ȵ�;
	EMPARTY_ERROR_LURE_CTF,
	EMPARTY_ERROR_OTHERSCHOOL,			//! �ٸ� �п��̶� ��Ƽ �ȵ�;
	EMPARTY_ERROR_PRISON,

};

enum EMPARTY_MEMBER_STATE_TYPE
{
	EMPARTY_MEMBER_STATE_NULL = 0x0000,
	EMPARTY_MEMBER_STATE_CONFRONT = 0x0001, // �����;
	EMPARTY_MEMBER_STATE_SELECTED = 0x0002, // ���õ�;
	EMPARTY_MEMBER_STATE_READY = 0x0004, // �غ� Ȯ��;
	EMPARTY_MEMBER_STATE_TAKEITEM = 0x0008, // ������ ȹ��;	
};

enum EMEXPEDITION_ERROR
{
	EMEXPEDITION_ERROR_NONE,
	EMEXPEDITION_ERROR_NEW_ALREADY,		// ������ ���� ����(�̹� ��������);
	EMEXPEDITION_ERROR_NEW_PARTYMAP,	// ������ ���� ����(��Ƽ��);
	EMEXPEDITION_ERROR_INVALID_LAND,		// ������ ���� ����(�߸��� ����);
};

class SPartyOption
{
public:
	const bool isValidOption(void) const; // ��ȿ�� �ɼ����� Ȯ��;	

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
	EMPARTY_ITEM_OPTION emItemOption;				// ������ ���� �ɼ�;
	EMPARTY_MONEY_OPTION emMoneyOption;				// ���ӸӴ� ���� �ɼ�;
	EMPARTY_ITEM_FILTER_OPTION emItemFilterLevel;	// ������ ���� ����;
};

class GLPartyID
{	
public:
	enum { INVALID = 0 };
public:
	//	MSGPACK_DEFINE(dwId);
	void Reset(void);	
	const bool isValidExpedition(void) const; // �����뿡 ���� �ִ°�;
	const bool isValidParty(void) const; // ��Ƽ�� ���� �ִ°�;	

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

public:  // �޼����� �̿��� ���� public���� �صξ�����, ���� ���� ���� ���� ����;		
		WORD wPartyID; // ��Ƽ ID;
		WORD wExpeditionID; // �����뿡 ���� ��� ������ ID;			
	
	MSGPACK_DEFINE(wPartyID, wExpeditionID);
	// ��Ƽ/�����뿡 ������ ���� ��� wPartyID = INVALID, wExpeditionID = INVALID;
	// ��Ƽ���� ���� ��� : wPartyID = ���� ��, wExpeditionID = INVALID;
	// �����뿡 ���� ��� : wPartyID = ���� ��, wExpeditionID = ���� ��;
};

//! ��Ƽ���� ��ũ�� ����� ������;
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
