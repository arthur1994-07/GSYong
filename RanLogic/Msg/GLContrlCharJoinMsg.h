#pragma once
#include <map>
#include <string>

#include "../WorldBattle/WorldBattleDefine.h"
#include "../InstanceSystem/GLInstanceSystem.h"
#include "../Move/MoveSystemDefine.h"
#include "GLContrlBaseMsg.h"

enum EMREQ_MOVEMAP_RESULT_AFB
{
	EMREQ_MOVEMAP_RESULT_AFB_OK				= 0,	//	성공.
	EMREQ_MOVEMAP_RESULT_AFB_FAIL			= 1,	//	실패.
	EMREQ_MOVEMAP_RESULT_AFB_ITEM			= 2,	//	아이템 불충족.
	EMREQ_MOVEMAP_RESULT_AFB_CONDITION		= 3,	//	여건이 안됨. ( 케릭 상태 이상, 대련, 거래 ).
	EMREQ_MOVEMAP_RESULT_AFB_NOTLASTCALL	= 4,	//	직전귀환 위치가 저장되어 있지 않음.
	EMREQ_MOVEMAP_RESULT_AFB_PK_LEVEL		= 5,	//	pk 성향이 낮아서 귀환 카드 사용 불가능.
	EMREQ_MOVEMAP_RESULT_AFB_JOINCON		= 6,	// 진입조건 불충분 ( 레벨, 스킬, 퀘스트, 아이템, 성향, 생활점수 )
	EMREQ_MOVEMAP_RESULT_AFB_IMMOVABLE		= 7,	// 진입불가 ( 이벤트 지역 )
	EMREQ_MOVEMAP_RESULT_AFB_INSTANTMAP		= 8,	//	직전귀환 위치가 인스턴트맵일경우
	EMREQ_MOVEMAP_RESULT_AFB_LOADING		= 9,	// 클라이언트에 로딩 시작을 알림
};

enum EMREQ_TELEPORT_FB
{
	EMREQ_TELEPORT_FB_OK			= 0,	//	성공.
	EMREQ_TELEPORT_FB_FAIL			= 1,	//	실패.
	EMREQ_TELEPORT_FB_ITEM			= 2,	//	아이템 불충족.
	EMREQ_TELEPORT_FB_CONDITION		= 3,	//	여건이 안됨. ( 케릭 상태 이상, 대련, 거래 ).
	EMREQ_TELEPORT_FB_PK_LEVEL		= 4,	//	pk 성향이 낮아서 귀환 카드 사용 불가능.
	EMREQ_TELEPORT_FB_JOINCON		= 5,	//  진입조건 불충분 ( 레벨, 스킬, 퀘스트, 아이템, 성향, 생활점수 )
	EMREQ_TELEPORT_FB_IMMOVABLE		= 6,	//  진입불가 ( 이벤트 지역 )
};

enum EMREQ_MUST_LEAVE_MAP_FB
{
	EMREQ_MUST_LEAVE_MAP_FB_OK			= 0,	//	성공.
	EMREQ_MUST_LEAVE_MAP_FB_FAIL			= 1,	//	실패.
};

enum EMREQ_MUST_LEAVE_REASON
{
	EMREQ_MUST_LEAVE_REASON_LEVEL, // 레벨;
	EMREQ_MUST_LEAVE_REASON_EXPEDITION, // 원정대;
};

enum EMFRIEND_FLAGS
{
	EMFRIEND_BLOCK	= -1,	//	블럭 ( 귓말 차단 상태. )
	EMFRIEND_OFF	= 0,	//	친구가 아닌 상태.
	EMFRIEND_VALID	= 1,	//	서로 유효한 친구.
	EMFRIEND_BETRAY = 2,	//	상대가 나를 차단하거나 삭제한 상태.
};

enum EMSAVECHARPOS_FB
{
	EMSAVECHARPOS_FB_FAIL				= 0,	//	일반 오류.
	EMSAVECHARPOS_FB_OK					= 1,	//	성공.
};

struct SFRIEND
{
public:
	SFRIEND(WORD Group) 
		: m_ChaDbNum(0)
		, m_Flag(EMFRIEND_OFF) // 
		, m_Online(false)
		, m_Channel(0)
        , m_Group(Group)
        , m_ChaClass(0)
        , m_ChaLevel(1)
        , m_PosX(0)
        , m_PosY(0)
	{
		//memset(m_ChaName, 0, sizeof(m_ChaName));
		//memset(szPhoneNumber, 0, sizeof(szPhoneNumber));
	}

    SFRIEND() 
        : m_ChaDbNum(0)
        , m_Flag(EMFRIEND_OFF)
        , m_Online(false)
        , m_Channel(0)
        , m_Group(0)
        , m_ChaClass(0)
        , m_ChaLevel(1)
        , m_PosX(0)
        , m_PosY(0)
    {
        //memset(m_ChaName, 0, sizeof(m_ChaName));
        //memset(szPhoneNumber, 0, sizeof(szPhoneNumber));
    }

    SFRIEND(const SFRIEND& rhs)
    {
        *this = rhs;
    }

    SFRIEND& SFRIEND::operator = (const SFRIEND& rhs)
    {
        if (this == &rhs)
            return *this;

        m_ChaDbNum = rhs.m_ChaDbNum;
        m_Flag     = rhs.m_Flag;
        m_Online   = rhs.m_Online;
        m_Channel  = rhs.m_Channel;
        m_Group    = rhs.m_Group;

        m_ChaName = rhs.m_ChaName;
        //StringCbCopy( szPhoneNumber, SMS_RECEIVER, cData.szPhoneNumber );

        m_ChaClass = rhs.m_ChaClass;
        m_ChaLevel = rhs.m_ChaLevel;
        m_ChaMap   = rhs.m_ChaMap;
        m_PartyID    = rhs.m_PartyID;		

        m_PosX = rhs.m_PosX;
        m_PosY = rhs.m_PosY;

        return *this;
    }

public:
    int m_ChaDbNum; //! Character DB Number
    int m_Flag; //! 친구 상태 EMFRIEND_FLAGS
    WORD m_Group;
    std::string m_ChaName; //CHR_ID_LENGTH+1;
    DWORD m_ChaClass; // EMCHARCLASS
    WORD  m_ChaLevel;
    MapID m_ChaMap;
    int m_PosX;
    int m_PosY;
    //TCHAR szPhoneNumber[SMS_RECEIVER];

    BYTE m_Channel;
    bool m_Online;
	GLPartyID m_PartyID;    

    MSGPACK_DEFINE(m_ChaDbNum, m_Flag, m_Group, m_ChaName, m_Channel, m_Online, m_ChaClass, m_ChaLevel, m_ChaMap, m_PartyID, m_PosX, m_PosY);

public:
    inline bool IsVALID() const { return m_Flag==EMFRIEND_VALID; }
    inline bool IsBLOCK() const { return m_Flag==EMFRIEND_BLOCK; }
    inline bool IsOFF() const   { return m_Flag==EMFRIEND_OFF;	}
    inline bool IsBETRAY() const { return m_Flag==EMFRIEND_BETRAY;}
    inline WORD GetGroup() const { return m_Group; }
    inline void SetGroup(WORD GroupNum) { m_Group = GroupNum; }
    inline int GetChaDbNum() const { return m_ChaDbNum; }
    void SetFriendName(const std::string& FriendName)
    {
        m_ChaName = FriendName;
    }

    std::string GetName() { return m_ChaName; }
    const std::string& GetName() const { return m_ChaName; }

    //! 친구 상태 EMFRIEND_FLAGS
    EMFRIEND_FLAGS GetFlag() const { return static_cast<EMFRIEND_FLAGS> (m_Flag); }
};

struct SCHARDATA2;

namespace GLMSG
{
	#pragma pack(1)	

    //! DB->Field:캐릭터 Join
	//! 내부 메시지용.
	struct NET_CHAR_JOIN_DF : public NET_MSG_GENERIC
	{
		struct SINFO
		{
			EMGAME_JOINTYPE		m_emType;
			MapID				m_sStartMap;
			DWORD				m_dwStartGate;
			D3DXVECTOR3			m_vStartPos;
			DWORD				m_dwActState;
			BOOL				m_bUseArmSub;
			EMIP_BONUS_CLASS	m_emIPBonus;
			SChinaTime			m_sChinaTime;
			SVietnamGainSystem	m_sVietnamSystem;
			SEventTime			m_sEventTime;
		};

		//mjeon.메시지 구조체 전체에 대한 8-byte alignment를 방지하기 위해 내부 구조체를 구성
		struct STIME
		{
			//8-bytes alignment
			__time64_t			m_tPremiumDate;
			__time64_t			m_tChatBlock;
		};
        
        DWORD				m_dwClientID;
		SCHARDATA2*			m_pCharData;		
		int					m_dwTutorial;

		//mjeon.AF		
		DWORD				m_dwGaeaID;
		GLPartyID			m_PartyID;

		SINFO				m_sINFO;

		STIME				m_sTIME;	//mjeon.AF

        wb::EM_COUNTRY m_Country; //! 국가설정

		DWORD				m_dwCountryID;

        bool bChangeField; // 출석부 준비 여부.

		float fMagicNum;

		CRYPT_KEY cKey;

		bool bResetCharacter;	// 스텟, 스킬(망각의 약 먹은 효과 적용 해야하는가?)

		int CharacterOption;

		enum
		{
			EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(SCHARDATA2*)+sizeof(INT)+sizeof(UINT)+sizeof(DWORD)+sizeof(SINFO)+sizeof(STIME)+sizeof(wb::EM_COUNTRY)+sizeof(DWORD)+sizeof(bool)+sizeof(float)+sizeof(CRYPT_KEY)+sizeof(bool)+sizeof(int),
		};

		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		//위의 항목 추가

		NET_CHAR_JOIN_DF()
			: m_dwClientID(0)
			, m_pCharData(0)			
			, m_dwGaeaID(GAEAID_NULL)
            , m_Country(wb::UNKNOWN_COUNTRY)
			, m_dwCountryID(0)
            , bChangeField(false)
			, fMagicNum( 0.0f )
			, bResetCharacter(false)
			, CharacterOption(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_FIELD_REQ_JOIN;

			//ZeroMemory(&m_sTIME, sizeof(m_sTIME));

			MIN_STATIC_ASSERT(sizeof(NET_CHAR_JOIN_DF)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNETLOBBY_REQ_GAME_JOIN : public NET_MSG_GENERIC
	{
		SNETLOBBY_REQ_GAME_JOIN ()
		{
			dwSize = sizeof(SNETLOBBY_REQ_GAME_JOIN);
			nType = NET_MSG_LOBBY_REQ_GAME_JOIN;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_REQ_GAME_JOIN)<=NET_DATA_BUFSIZE);
		}
	};

	//! Agent->Client:Lobby Display 용 캐릭터 정보
	struct NET_LOBBY_CHARINFO_AC //: public NET_MSG_GENERIC
	{
		SCHARINFO_LOBBY Data;

        MSGPACK_DEFINE(Data);

        NET_LOBBY_CHARINFO_AC()
        {
        }

		NET_LOBBY_CHARINFO_AC(const SCHARINFO_LOBBY& CharData)
            : Data(CharData)
		{
			//dwSize = sizeof(NET_LOBBY_CHARINFO_AC);
			//nType = NET_MSG_LOBBY_CHARINFO_AC;
			//MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHARINFO_AC)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_LOBBY_CHARINFO_AC_END : public NET_MSG_GENERIC
	{
 		NET_LOBBY_CHARINFO_AC_END()
		{
			dwSize = sizeof(NET_LOBBY_CHARINFO_AC_END);
			nType = NET_MSG_LOBBY_CHARINFO_AC_END;
			MIN_STATIC_ASSERT(sizeof(NET_LOBBY_CHARINFO_AC_END)<=NET_DATA_BUFSIZE);
		}
	};


	//---------------------------------------------------------------------------NET
    struct SNETLOBBY_CHARJOIN : public NET_MSG_CHARACTER
	{
		char				szUserID[USR_ID_LENGTH+1];

		DWORD				dwClientID;
		DWORD				dwGaeaID;

		MapID				sMapID;								//	맵 ID.
		D3DXVECTOR3		vPos;									//	맵 상의 위치.
		bool					bCullByObjectMap;				//	오브젝트에 의한 컬링 on/off;

		SCHARDATA			Data;								//	기본 데이터.
		
		WORD				wSKILLQUICK_ACT;					//	스킬 퀵슬롯중 액티브된 스킬.
		SNATIVEID			sSKILLQUICK[EMSKILLQUICK_SIZE];		//	스킬 퀵슬롯.
		SACTION_SLOT		sACTIONQUICK[EMACTIONQUICK_SIZE];	//	엑션 퀵슬롯.

		DWORD				dwNumInvenItems;					//	인밴에 들어있는 아이템 갯수.
		DWORD				dwNumVNInvenItems;					//	베트남 인밴에 들어있는 아이템 갯수.
		DWORD				dwNumSkill;							//	습득한 스킬의 갯수.
		DWORD				dwNumQuestProc;
		DWORD				dwNumQuestEnd;
		DWORD				dwChargeItem;
		DWORD				dwItemCoolTime;
		DWORD				dwCountryID;

		bool				bBIGHEAD;
		bool				bBIGHAND;
		float				fBIGHEAD;
		float				fBIGHAND;

		SNATIVEID			sStartMapID;				//	초기 시작 맵.
		DWORD				dwStartGate;				//	초기 시작 개이트.

		SNATIVEID			sLastCallMapID;				//	직전귀환 맵.
		D3DXVECTOR3			vLastCallPos;				//	직전귀환 위치.

		bool				bTracingChar;				//  추적 대상인지 아닌지

		EMIP_BONUS_CLASS	emIPBonus;			//  태국 사이버 까페

		SNETLOBBY_CHARJOIN () :
			dwClientID(0),
			dwGaeaID(0),

			wSKILLQUICK_ACT(0),

			dwNumInvenItems(0),
			dwNumVNInvenItems(0),
			dwNumSkill(0),

			dwNumQuestProc(0),
			dwNumQuestEnd(0),

			dwChargeItem(0),
			dwItemCoolTime(0),
			
			dwCountryID(0),

			bBIGHEAD(false),
			bBIGHAND(false),
			fBIGHEAD(1),
			fBIGHAND(1),

			sStartMapID(false),
			dwStartGate(0),
			sLastCallMapID(false),
			vLastCallPos(0,0,0),
			bTracingChar(FALSE),
			emIPBonus(EMIP_BONUS_NONE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_CHAR_JOIN;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARJOIN)<=NET_DATA_BUFSIZE);
			memset ( szUserID, 0, sizeof( char ) * ( USR_ID_LENGTH+1) );
		}
	};

	//---------------------------------------------------------------------------NET
//     struct SNETLOBBY_CHARPUTON : public NET_MSG_GENERIC
// 	{
// 		SITEMCUSTOM			PutOnItems[SLOT_TSIZE];				//	착용 아이템.
// 
// 		SNETLOBBY_CHARPUTON ()
// 		{
// 			dwSize = sizeof(*this);
// 			nType = NET_MSG_LOBBY_CHAR_PUTON;
// 			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARPUTON)<=NET_DATA_BUFSIZE);
// 		}
// 	};

	//---------------------------------------------------------------------------NET
	struct SNETENTRY_FAILED : public NET_MSG_GENERIC
	{
		SNETENTRY_FAILED () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_ETNRY_FAILED;
			MIN_STATIC_ASSERT(sizeof(SNETENTRY_FAILED)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLIMITTIME_OVER : public NET_MSG_GENERIC
	{
		int					nRemainTime;

		SNETLIMITTIME_OVER () :
				nRemainTime( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_LIMITTIME_OVER;
			MIN_STATIC_ASSERT(sizeof(SNETLIMITTIME_OVER)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETLOBBY_CHARPUTON_EX : public NET_MSG_CHARACTER
	{
		int					nSlot;
		SITEMCUSTOM			PutOnItem;							//	착용 아이템.

		SNETLOBBY_CHARPUTON_EX ()
		: nSlot ( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_CHAR_PUTON_EX;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARPUTON_EX)<=NET_DATA_BUFSIZE);
		}
	};	

    struct CHARPUTON_EX_PACK_DATA
    {
        int					nSlot;
        SITEMCUSTOM			PutOnItem;							//	착용 아이템.

        CHARPUTON_EX_PACK_DATA()
            : nSlot(0)
        {}

        MSGPACK_DEFINE(  nSlot, PutOnItem );
    };

	struct SNETLOBBY_CHARSKILL : public NET_MSG_CHARACTER
	{
		DWORD				wNum;
		SCHARSKILL			sSKILL[EMGLMSG_SKILLMAX];

		SNETLOBBY_CHARSKILL () :
			wNum(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_CHAR_SKILL;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARSKILL)<=NET_DATA_BUFSIZE);
		}

		BOOL ADD ( const SCHARSKILL &sSkill )
		{
			if ( wNum == EMGLMSG_SKILLMAX )
				return FALSE;

			sSKILL[wNum] = sSkill;
			wNum++;

			dwSize = sizeof(*this) - sizeof(SCHARSKILL) * ( EMGLMSG_SKILLMAX - wNum );
			return TRUE;
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_INVENITEM : public NET_MSG_CHARACTER
	{
		SINVENITEM			Data;

		SNETLOBBY_INVENITEM ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_CHAR_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_INVENITEM)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETLOBBY_INVENITEM_PACK
    {
        SINVENITEM			Data;
        // NET_MSG_LOBBY_CHAR_ITEM

        MSGPACK_DEFINE( Data );
    };

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_VIETNAM_INVENITEM : public NET_MSG_CHARACTER
	{
		SINVENITEM			Data;

		SNETLOBBY_VIETNAM_INVENITEM ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_CHAR_VIETNAM_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_VIETNAM_INVENITEM)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_QUEST_PROG : public NET_MSG_CHARACTER
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_CHARACTER)-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD)-sizeof(BOOL), };
		
		DWORD				dwQID;
		BOOL				bLastSeq;
		DWORD				dwSeq;
		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNETLOBBY_QUEST_PROG (DWORD _dwQID) :
			dwQID(_dwQID),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL);
			nType = NET_MSG_LOBBY_QUEST_PROG;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_QUEST_PROG)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_QUEST_PROG)<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNETPC_QUEST_PROG_INVEN_INSERT2 : public NET_MSG_CHARACTER
	{
		DWORD				dwQID;
		SINVENITEM			Data;

		SNETPC_QUEST_PROG_INVEN_INSERT2( DWORD _dwQID, SINVENITEM& _Data )
			: dwQID( _dwQID )
			, Data( _Data )

		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT2;
			MIN_STATIC_ASSERT( sizeof( SNETPC_QUEST_PROG_INVEN_INSERT2 ) <= NET_DATA_BUFSIZE );
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETLOBBY_QUEST_END : public NET_MSG_CHARACTER
	{
		enum { EMSIZE_MAX = NET_DATA_BUFSIZE-sizeof(NET_MSG_CHARACTER)-sizeof(DWORD)-sizeof(DWORD)-sizeof(DWORD)-sizeof(BOOL), };
		
		DWORD				dwQID;
		BOOL				bLastSeq;
		DWORD				dwSeq;

		DWORD				dwLengthStream;
		BYTE				arraySTREAM[EMSIZE_MAX];

		SNETLOBBY_QUEST_END (DWORD _dwQID) :
			dwQID(_dwQID),
			bLastSeq(FALSE),
			dwSeq(0),
			dwLengthStream(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL);
			nType = NET_MSG_LOBBY_QUEST_END;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_QUEST_END)<=NET_DATA_BUFSIZE);
		}

		bool SETSTREAM ( BYTE *pSTream, DWORD dwLength )
		{
			if ( EMSIZE_MAX <= dwLength )		return false;

			dwLengthStream = dwLength;
			memcpy ( arraySTREAM, pSTream, sizeof(BYTE)*dwLengthStream );

			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BOOL) + sizeof(BYTE)*dwLengthStream;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_QUEST_END)<=NET_DATA_BUFSIZE);

			return true;
		}
	};

	struct SNETLOBBY_CHARGE_ITEM : public NET_MSG_GENERIC
	{
		char				szPurKey[PURKEY_LENGTH+1];
		SNATIVEID			nidITEM;

		SNETLOBBY_CHARGE_ITEM () 
			: nidITEM(false)
		{
			dwSize = sizeof(SNETLOBBY_CHARGE_ITEM);
			nType = NET_MSG_LOBBY_CHARGE_ITEM;
			memset(szPurKey, 0, sizeof(char) * (PURKEY_LENGTH+1));
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARGE_ITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_ITEM_COOLTIME : public NET_MSG_CHARACTER
	{
		DWORD				dwNum;
		EMCOOL_TYPE			emCoolType;
		ITEM_COOLTIME		sItemCoolTime[EMGLMSG_COOLTIMEMAX];

		SNETLOBBY_ITEM_COOLTIME ()
			: dwNum(0)
			, emCoolType( EMCOOL_ITEMID )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_LOBBY_ITEM_COOLTIME;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_ITEM_COOLTIME)<=NET_DATA_BUFSIZE);
		}

		BOOL ADD ( const ITEM_COOLTIME &sCoolTime )
		{
			if ( dwNum == EMGLMSG_COOLTIMEMAX )	return FALSE;

			sItemCoolTime[dwNum] = sCoolTime;
			dwNum++;

			dwSize = sizeof(*this) - sizeof(ITEM_COOLTIME) * ( EMGLMSG_COOLTIMEMAX - dwNum );
			return TRUE;
		}
	};	

	struct SNETLOBBY_INSTANCE_LIST : public NET_MSG_CHARACTER
	{
		enum
		{
			INSTANCE_NSIZE = (NET_DATA_BUFSIZE - sizeof(NET_MSG_CHARACTER) - sizeof(DWORD)) / sizeof(InstanceSystem::InstanceInformationClient),
			HEADER_NSIZE = sizeof(NET_MSG_CHARACTER) + sizeof(DWORD),
		};
		DWORD nInstanceInformation;
		InstanceSystem::InstanceInformationClient instanceInformation[INSTANCE_NSIZE];

		const bool IsAddable ()
		{
			if ( nInstanceInformation >= INSTANCE_NSIZE )
				return false;

			return true;
		}

		const bool add(const InstanceSystem::InstanceInformationClient& _instanceInformation)
		{
			if ( nInstanceInformation >= INSTANCE_NSIZE )
				return false;

			instanceInformation[nInstanceInformation++] = _instanceInformation;
			dwSize += sizeof(InstanceSystem::InstanceInformationClient);
			return true;
		}

		void reset(void)
		{
			nInstanceInformation = 0;
			dwSize = HEADER_NSIZE;
		}

		SNETLOBBY_INSTANCE_LIST(void)
			: nInstanceInformation(0)
		{
			nType = NET_MSG_LOBBY_INSTANCE_LIST;
			dwSize = HEADER_NSIZE;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_INSTANCE_LIST) <= NET_DATA_BUFSIZE);
		}

	};

	struct SNETLOBBY_CHAR_DEFAULT: public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		float fInvisiblePerceiveAngle;
		float fInvisiblePerceiveRadius;
		float fInvisiblePerceiveLevel;
		float fInvisibleUpdateTime;

		SNETLOBBY_CHAR_DEFAULT(void)
		{
			dwSize = sizeof(SNETLOBBY_CHAR_DEFAULT);
			nType = NET_MSG_LOBBY_CHAR_DEFAULT;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHAR_DEFAULT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETLOBBY_CHARJOIN_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCHAR_JOIN_FB emCharJoinFB;

		SNETLOBBY_CHARJOIN_FB(EMCHAR_JOIN_FB _emCharJoinFB=EMCJOIN_FB_NOWLOGIN)
			: emCharJoinFB(_emCharJoinFB)
		{
			dwSize = sizeof(SNETLOBBY_CHARJOIN_FB);
			nType = NET_MSG_LOBBY_CHAR_JOIN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARJOIN_FB)<=NET_DATA_BUFSIZE);
		}
	};	

	// 2차비번
	struct SNETLOBBY_CHARJOIN_2NDPASS : public NET_MSG_GENERIC
	{
		int	nResult;
		CTime tBlockExpiredTime;

		SNETLOBBY_CHARJOIN_2NDPASS () 
			: nResult(0)
		{
			dwSize = sizeof(SNETLOBBY_CHARJOIN_2NDPASS);
			nType = NET_MSG_LOBBY_CHARJOIN_2NDPASS;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARJOIN_2NDPASS)<=NET_DATA_BUFSIZE);
		}
	};
	/////////////////////////////////////////////

	struct SNETLOBBY_CHARJOIN_CHINA_ERROR : public NET_MSG_GENERIC
	{
		SNETLOBBY_CHARJOIN_CHINA_ERROR () 
		{
			dwSize = sizeof(SNETLOBBY_CHARJOIN_CHINA_ERROR);
			nType = NET_MSG_LOBBY_CHINA_ERROR;
			MIN_STATIC_ASSERT(sizeof(SNETLOBBY_CHARJOIN_CHINA_ERROR)<=NET_DATA_BUFSIZE);
		}
	};
	
	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_CHARCHK : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		DWORD				dwClientID;

		DWORD				dwUserID;
		DWORD				dwCharID;
		CHAR				szName[CHR_ID_LENGTH+1];

		SNETPC_FIELDSVR_CHARCHK	() 
			: dwGaeaID(0)
			, dwClientID(0)
			
			, dwUserID(0)
			, dwCharID(0)
		{
			dwSize = sizeof(SNETPC_FIELDSVR_CHARCHK);
			nType = NET_MSG_FIELDSVR_CHARCHK;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			MIN_STATIC_ASSERT(sizeof(SNETPC_FIELDSVR_CHARCHK)<=NET_DATA_BUFSIZE);			
		}

        void SetName(const char* _Name)
        {
            if (_Name)
                StringCchCopy(szName, CHR_ID_LENGTH+1, _Name);
        }
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_FIELDSVR_CHARCHK_FB : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		CHAR				szName[CHR_ID_LENGTH+1];

		int					nChannel;
		DWORD				dwFIELDID;
		bool				bExist;


		SNETPC_FIELDSVR_CHARCHK_FB () 
			: dwGaeaID(0)
			, nChannel(0)
			, dwFIELDID(0)
			, bExist(false)
		{
			dwSize = sizeof(SNETPC_FIELDSVR_CHARCHK_FB);
			nType = NET_MSG_FIELDSVR_CHARCHK_FB;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			MIN_STATIC_ASSERT(sizeof(SNETPC_FIELDSVR_CHARCHK_FB)<=NET_DATA_BUFSIZE);			
		}
	};

	//AF
	struct SNETPC_MOVEMAP_AF : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		MoveSystem::EMMOVE_TYPE emMoveType;
		BOOL	            bTransfer;		//Serialization을 이용한 캐릭터 이전 중인가? DB Save/Load를 이용하는 일반적인 Recall의 경우는 FALSE
		BOOL	            bToInstance;	//Instance로의 Transfer인가?
		BOOL				bSameFieldServer;
		MapID				targetMapID;
		DWORD				dwGateID;
		D3DXVECTOR3			vPosition;

        EMGAME_JOINTYPE		emType;

		SNETPC_MOVEMAP_AF ()
			: emMoveType( MoveSystem::EMMOVE_TYPE_DEFAULT )
			, bTransfer(FALSE)
			, bToInstance(FALSE)
			, bSameFieldServer(FALSE)
            , emType(EMJOINTYPE_MOVEMAP)
			, dwGateID( UINT_MAX )
			, vPosition( 0.f, 0.f, 0.f )
		{
			dwSize = sizeof(SNETPC_MOVEMAP_AF);
			nType = NET_MSG_MOVEMAP_AF;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MOVEMAP_AF)<=NET_DATA_BUFSIZE);
		}
	};

	//FA
	struct SNETPC_FIELDSVR_OUT_FB : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		BOOL	            bTransfer;		//Serialization을 이용한 캐릭터 이전 중인가? DB Save/Load를 이용하는 일반적인 Recall의 경우는 FALSE
		BOOL	            bToInstance;	//Instance로의 Transfer인가
        EMGAME_JOINTYPE		emType;
		D3DXVECTOR3	positionPrevious;

        bool                bChangeField; // 출석부 준비 여부.
		SSUMMONABLE			sSummonable;

		SNETPC_FIELDSVR_OUT_FB ()
			:bTransfer(FALSE)
			,bToInstance(FALSE)
            ,emType(EMJOINTYPE_MOVEMAP)
            ,bChangeField(false)
		{
			dwSize = sizeof(SNETPC_FIELDSVR_OUT_FB);
			nType = NET_MSG_FIELDSVR_OUT_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_FIELDSVR_OUT_FB)<=NET_DATA_BUFSIZE);
		}
	};	

	//---------------------------------------------------------------------------NET
	struct SNETREQ_INSTANTMAP_MOVEOUT_REQ : public NET_MSG_GENERIC
	{
		SNETREQ_INSTANTMAP_MOVEOUT_REQ ()
		{
			dwSize = sizeof(SNETREQ_INSTANTMAP_MOVEOUT_REQ);
			nType = NET_MSG_GCTRL_INSTANTMAP_MOVEOUT_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_INSTANTMAP_MOVEOUT_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETREQ_GATEOUT_REQ : public NET_MSG_GENERIC
	{
		DWORD				dwGateID;
		DWORD				dwGateToIndex;

		SNETREQ_GATEOUT_REQ () 
			: dwGateID(0)
			, dwGateToIndex(0)
		{
			dwSize = sizeof(SNETREQ_GATEOUT_REQ);
			nType = NET_MSG_GCTRL_REQ_GATEOUT_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_GATEOUT_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_GATEOUT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwGateID;
		DWORD				dwGaeaID;
		DWORD				dwGateToIndex;

		SNETREQ_GATEOUT () 
			: dwGateID(0)
			, dwGaeaID(0)			
			, dwGateToIndex(0)
		{
			dwSize = sizeof(SNETREQ_GATEOUT);
			nType = NET_MSG_GCTRL_REQ_GATEOUT;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_GATEOUT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_GATEOUT_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMCHAR_GATEOUT_FB	emFB;

		MapID			   sMapID;
		D3DXVECTOR3         vPos;
		bool                bCullByObjectMap;

		SNETREQ_GATEOUT_FB () 
			: emFB(EMCHAR_GATEOUT_OK)
			, vPos(0,0,0)
		{
			dwSize = sizeof(SNETREQ_GATEOUT_FB);
			nType = NET_MSG_GCTRL_REQ_GATEOUT_FB;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_GATEOUT_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_LANDIN : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		__time64_t tLoginTime;

		// RequestLandIn Call this function after receive view around message already. 버그 찾기 위한 Data
		int m_nDoActWaitTypePrev;
		int m_nDoActWaitType;

		SNETREQ_LANDIN( int nDoActWaitTypePrev, int nDoActWaitType )
			: m_nDoActWaitTypePrev(nDoActWaitTypePrev)
			, m_nDoActWaitType(nDoActWaitType)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_LANDIN;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_LANDIN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETREQ_READY : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNETREQ_READY ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_READY;
			MIN_STATIC_ASSERT(sizeof(SNETREQ_READY)<=NET_DATA_BUFSIZE);
		}
	};	

	//---------------------------------------------------------------------------NET
    struct NET_CHAR_RESURRECT_INFO_FC : public NET_MSG_GENERIC  // Field -> Client : 부활 관련 정보 전송;
    {
        const unsigned int sResurrectAbleState;
        float fResurrectTime;
        float fResurrectRestore;

        NET_CHAR_RESURRECT_INFO_FC(const unsigned int _ResurrectState)
            : sResurrectAbleState(_ResurrectState)
            , fResurrectTime(0.0f)
            , fResurrectRestore(0.0f)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_RESURRECT_STATE;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_RESURRECT_INFO_FC)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CHAR_REQ_RESURRECT_CF : public NET_MSG_GENERIC  // Client -> Field : 부활 요청;
    {
        const unsigned int resurrectType;

        NET_CHAR_REQ_RESURRECT_CF(const unsigned int _resurrectType)
            : resurrectType(_resurrectType)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_REQ_RESURRECT;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_REQ_RESURRECT_CF)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_CHAR_REQ_RESURRECT_FB : public NET_MSG_CHARACTER  // Field -> Client : 부활 요청의 따른 결과 전송;
    {
        const unsigned int resurrectType;
        const unsigned int emResult;

        __int64 lnRestorableEXP;
        __int64 lnRestoreExpCost;

        NET_CHAR_REQ_RESURRECT_FB(const unsigned int _resurrectType, const unsigned int _emResult)
            : resurrectType(_resurrectType)
            , emResult(_emResult)
            
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_REQ_RESURRECT_FB;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_REQ_RESURRECT_FB)<=NET_DATA_BUFSIZE);            
        }
    };

    struct NET_CHAR_RESURRECT_FAFC : public NET_MSG_GENERIC
    {
        const MapID sMapID;
        D3DXVECTOR3 vPos;

        const int nNowHP;
        const int nNowMP;
        const int nNowSP; 

        const bool bCullByObjectMap;

		DWORD dwChaNum;

        NET_CHAR_RESURRECT_FAFC(const MapID& _sMapID
			, const D3DXVECTOR3& _vPos
			, const int _nHP
			, const int _nMP
			, const int _nSP
			, const bool bCullMap
			, DWORD _dwChaNum)
            : sMapID(_sMapID)
            , vPos(_vPos)
            , nNowHP(_nHP)
            , nNowMP(_nMP)
            , nNowSP(_nSP)
            , bCullByObjectMap(bCullMap)
			, dwChaNum(_dwChaNum)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_RESURRECT_FAFC;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_RESURRECT_FAFC)<=NET_DATA_BUFSIZE);
        }
    };

    /*struct NET_CHAR_RESURRECT_MOVEMAP_FA : public NET_MSG_CHARACTER  // Field -> Agent : 타 필드 서버로 필드서버 이동이 필요한 부활시 Agent에 요청;
    {
        const SNATIVEID sMapID;
        const DWORD dwGateID;
        const float fRestore;

        NET_CHAR_RESURRECT_MOVEMAP_FA(const DWORD _dwChaNum, const SNATIVEID& _sMapID, const DWORD _dwGateID, const float _fRestore)
            : sMapID(_sMapID)
            , dwGateID(_dwGateID)
            , fRestore(_fRestore)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_RESURRECT_MOVEMAP_FA;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_RESURRECT_MOVEMAP_FA)<=NET_DATA_BUFSIZE);

            dwChaNum = _dwChaNum;
        }
    };*/

    struct NET_CHAR_RESURRECT_MOVEMAP_AF : public NET_MSG_CHARACTER  // Agent -> Field : 부활에 의해 필드서버 이동 되어질때 해당 필드에 통보;
    {
        const SNATIVEID sMapID;
        const DWORD dwGateID;
        const float fRestore;

        NET_CHAR_RESURRECT_MOVEMAP_AF(const DWORD _dwChaNum, const SNATIVEID& _sMapID, const DWORD _dwGateID, const float _fRestore)
            : sMapID(_sMapID)
            , dwGateID(_dwGateID)
            , fRestore(_fRestore)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_RESURRECT_MOVEMAP_AF;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_RESURRECT_MOVEMAP_AF)<=NET_DATA_BUFSIZE);

            dwChaNum = _dwChaNum;
        }
    };

    /*struct NET_CHAR_RESURRECT_MOVEMAP_FB : public NET_MSG_CHARACTER  // Field DB -> Agent : 필드서버이동 부활시 부활 후 DB액션 후 agent를 통해 필드서버 이동을 요청;
    {
        const SNATIVEID sMapID;
        const DWORD dwGateID;

        NET_CHAR_RESURRECT_MOVEMAP_FB(const DWORD _dwChaNum, const SNATIVEID& _sMapID, const DWORD _dwGateID)
            : sMapID(_sMapID)
            , dwGateID(_dwGateID)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_RESURRECT_MOVEMAP_FB;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_RESURRECT_MOVEMAP_FB)<=NET_DATA_BUFSIZE);

            dwChaNum = _dwChaNum;
        }
    };*/

	/************************************************************************/
	/* Field 를 이동해야 함을 Agent 에 알린다;                            */
	/************************************************************************/
	struct SNETPC_REQ_MOVEMAP_FA : public NET_MSG_CHARACTER
	{
		MoveSystem::EMMOVE_TYPE	emMoveType;
		DWORD					dwFieldServerIDTo;
		DWORD					dwChannelTo;

		MapID					mapIDTo;
		DWORD					dwGATEID;
		D3DXVECTOR3				vPosition;

		DWORD					dwRetryCount;

		SNETPC_REQ_MOVEMAP_FA () 
			: emMoveType ( MoveSystem::EMMOVE_TYPE_DEFAULT )
			, dwFieldServerIDTo ( FIELDSERVER_MAX )
			, dwChannelTo ( MAX_CHANNEL_NUMBER )
			, dwGATEID ( UINT_MAX )
			, vPosition ( 0.f, 0.f, 0.f )
			, dwRetryCount ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GCTRL_REQ_MOVEMAP_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_MOVEMAP_FA ) <= NET_DATA_BUFSIZE );
		}
	};

	/************************************************************************/
	/* 이동 시의 성공 & 실패 상태를 Field 또는 Client 에 알린다;        */
	/************************************************************************/
	struct SNETPC_NOTICE_MOVEMAP_RESULT_AFB : public NET_MSG_CHARACTER
	{
		EMREQ_MOVEMAP_RESULT_AFB	emFB;

		MoveSystem::EMMOVE_TYPE		emMoveType;

		MapID						sMAPID;
		DWORD						dwGATEID;
		D3DXVECTOR3					vPOS;

		DWORD						dwRetryCount;

		bool						bCullByObjectMap;

		SNETPC_NOTICE_MOVEMAP_RESULT_AFB () 
			: emFB ( EMREQ_MOVEMAP_RESULT_AFB_OK )
			, emMoveType ( MoveSystem::EMMOVE_TYPE_DEFAULT )
			, dwGATEID ( UINT_MAX )
			, vPOS ( 0.f, 0.f, 0.f )
			, dwRetryCount ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GCTRL_NOTICE_MOVEMAP_RESULT_AFB;
			MIN_STATIC_ASSERT( sizeof( SNETPC_NOTICE_MOVEMAP_RESULT_AFB ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP_AG : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		MapID				sMAPID;
		DWORD				dwGATEID;
		D3DXVECTOR3		vPOS;		

		SNETPC_REQ_MUST_LEAVE_MAP_AG () 
			: dwGATEID(UINT_MAX)
			, vPOS(0,0,0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_REQ_MUST_LEAVE_MAP_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_MUST_LEAVE_MAP_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP_FB : public NET_MSG_GENERIC
	{
		EMREQ_MUST_LEAVE_MAP_FB		emFB;
		MapID				sMAPID;
		D3DXVECTOR3		vPOS;
		bool					bCullByObjectMap;

		SNETPC_REQ_MUST_LEAVE_MAP_FB () 
			: emFB(EMREQ_MUST_LEAVE_MAP_FB_FAIL)
			, vPOS(0,0,0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_REQ_MUST_LEAVE_MAP_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_MUST_LEAVE_MAP_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MUST_LEAVE_MAP : public NET_MSG_GENERIC
	{
		SNATIVEID			sMAPID;
		DWORD				dwGATEID;
		EMREQ_MUST_LEAVE_REASON emReason;

		SNETPC_REQ_MUST_LEAVE_MAP () 
			: dwGATEID(0)			
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_REQ_MUST_LEAVE_MAP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_MUST_LEAVE_MAP)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_REQ_TELEPORT_AG : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		MapID					sMAPID;
		D3DXVECTOR3			vPOS;

		SNETPC_REQ_TELEPORT_AG () :
			sMAPID(false),
			vPOS(0,0,0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_TELEPORT_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TELEPORT_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TELEPORT_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMREQ_TELEPORT_FB	emFB;
		MapID				sMAPID;
		D3DXVECTOR3		vPOS;
		bool					bCullByObjectMap;

		SNETPC_REQ_TELEPORT_FB () :
			emFB(EMREQ_TELEPORT_FB_OK),
			sMAPID(false),
			vPOS(0,0,0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_TELEPORT_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TELEPORT_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_LAND_INFO : public NET_MSG_GENERIC
	{
		MapID			nidMAP;
		bool				bClubBattle;
		bool				bClubBattleHall;
		bool				bClubDeathMatch;
		bool				bClubDeathMatchHall;
		bool				bPK;
		bool				bLunchBoxForbid;
		bool				bClubBattleInProgress;	// 선도클럽결정전 진행중

		DWORD				dwGuidClub;
		DWORD				dwGuidClubMarkVer;
		char				szGuidClubName[CLUB_NAME_LENGTH];
		float				fCommission;

		DWORD				dwClubMapID;	// 선도 및 CDM ID

		SNETPC_LAND_INFO () 
			: bClubBattle(false)
			, bClubBattleHall(false)
			, bPK(false)
			, bClubDeathMatch(false)
			, bClubDeathMatchHall(false)
			, dwGuidClub(CLUB_NULL)
			, dwGuidClubMarkVer(0)
			, fCommission(0.0f)
			, bLunchBoxForbid(false)
			, bClubBattleInProgress( false )
			, dwClubMapID( 0 )
		{
			dwSize = sizeof(SNETPC_LAND_INFO);
			nType = NET_MSG_GCTRL_LAND_INFO;
			memset(szGuidClubName, 0, sizeof(char) * (CLUB_NAME_LENGTH));
			MIN_STATIC_ASSERT(sizeof(SNETPC_LAND_INFO)<=NET_DATA_BUFSIZE);			
		}
	};


	struct SNETPC_SERVER_PKEVENT_INFO : public NET_MSG_GENERIC
	{
		bool				bSCHOOL_FREEPK;
		bool				bBRIGHTEVENT;
		bool				bPKServer;

		SNETPC_SERVER_PKEVENT_INFO () :
			bPKServer(false),
			bSCHOOL_FREEPK(false),
			bBRIGHTEVENT(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SERVER_PKEVENT_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_PKEVENT_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_INFO : public NET_MSG_GENERIC
	{
		bool				bSCHOOL_FREEPK;

		SNETPC_SERVER_INFO () 
			: bSCHOOL_FREEPK(false)
		{
			dwSize = sizeof(SNETPC_SERVER_INFO);
			nType = NET_MSG_GCTRL_SERVER_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_BRIGHTEVENT_INFO : public NET_MSG_GENERIC
	{
		bool				bBRIGHTEVENT;

		SNETPC_SERVER_BRIGHTEVENT_INFO () :
			bBRIGHTEVENT(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_BRIGHTEVENT_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_GUID_BATTLE_INFO : public NET_MSG_GENERIC
	{
		bool				bClubBattle;

		SNETPC_SERVER_CLUB_GUID_BATTLE_INFO () :
			bClubBattle(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SERVER_CLUB_GUID_BATTLE_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_CLUB_GUID_BATTLE_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;

		SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG ()
			: dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SERVER_CLUB_GUID_BATTLE_REMAIN_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_CLUB_GUID_BATTLE_REMAIN_AG)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_SERVER_CLUB_DEATHMATCH_INFO : public NET_MSG_GENERIC
	{
		bool				bClubDeathMatch;

		SNETPC_SERVER_CLUB_DEATHMATCH_INFO () :
			bClubDeathMatch(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_CLUB_DEATHMATCH_INFO)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;

		SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG ()
			: dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_REQ_SERVER_CLUB_DEATHMATCH_REMAIN_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SERVER_CLUB_DEATHMATCH_REMAIN_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SCHOOLFREEPK_FLD : public NET_MSG_GENERIC
	{
		bool				bSCHOOL_FREEPK;

		SNETPC_SCHOOLFREEPK_FLD () :
			bSCHOOL_FREEPK(false)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_SCHOOLFREEPK_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SCHOOLFREEPK_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SAVECHARPOSRST_FROMDB : public NET_MSG_GENERIC
	{
		DWORD				dwCharID;
		DWORD				dwFieldServer;
		EMSAVECHARPOS_FB	emFB;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)*3 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_SAVECHARPOSRST_FROMDB ()
			: emFB(EMSAVECHARPOS_FB_FAIL)
			, dwCharID(0)
			, dwFieldServer(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_CHARPOS_FROMDB2AGT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SAVECHARPOSRST_FROMDB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MARKETCHECK : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;

		SNETPC_MARKETCHECK ()
			: dwGaeaID(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_MARKET_CHECKTIME;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MARKETCHECK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_MOVE_MAP : public NET_MSG_CHARACTER
	{
		SNATIVEID	sMapID;
		DWORD		dwGata;
		D3DXVECTOR3 vPosition;

		SNETPC_REQ_MOVE_MAP()
			: sMapID( NATIVEID_NULL() )
			, dwGata( 0 )
			, vPosition( D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQ_MOVE_MAP;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_MOVE_MAP ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_PREMIUM_MAP_EXIT : public NET_MSG_GENERIC
	{
		DWORD dwRemainTime;

		SNETPC_PREMIUM_MAP_EXIT()
			: dwRemainTime( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_PREMIUM_MAP_EXIT;
			MIN_STATIC_ASSERT( sizeof( SNETPC_PREMIUM_MAP_EXIT ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_PRISON_AG : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNETPC_PRISON_AG () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PRISON_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PRISON_AG)<=NET_DATA_BUFSIZE);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};
