#pragma once

#include "../../SigmaCore/String/MinGuid.h"

#include "../GLogicData.h"
#include "../GLUseFeatures.h"
#include "../Crow/DropCrow.h"
#include "../Crow/DropMaterial.h"
#include "../Attendance/TaskN.h"
#include "../GLCompetition.h"
#include "../MatchSystem/MatchDefine.h"
#include "../GroupChat/GroupChat.h"

#include "GLContrlBaseMsg.h"


#include "GLContrlCharJoinMsg.h"
#include "GLContrlClubMsg.h"
#include "GLContrlConflictMsg.h"
#include "GLContrlCrowMsg.h"
#include "GLContrlFriendMsg.h"
#include "GLContrlInDunMsg.h"
#include "GLContrlInvenMsg.h"
#include "GLContrlLandEventMsg.h"
#include "GLContrlNpcMsg.h"
#include "GLContrlPartyMsg.h"
#include "GLContrlPcMsg.h"
#include "GLContrlPetMsg.h"
#include "GLContrlPrivateMarket.h"
#include "GLContrlQuestMsg.h"
#include "GLContrlServerMsg.h"
#include "GLContrlSkillMsg.h"
#include "GLContrlSummonMsg.h"
#include "GLContrlTradeMsg.h"
#include "RnAttendanceMsg.h"
#include "GLContrlInstanceContentsMsg.h"

enum EMCHAT_LOUDSPEAKER
{
	EMCHAT_LOUDSPEAKER_FAIL		= 0,
	EMCHAT_LOUDSPEAKER_OK		= 1,
	EMCHAT_LOUDSPEAKER_NOITEM	= 2,
	EMCHAT_LOUDSPEAKER_BLOCK	= 3,
};

enum EMCHAT_AREA
{
	EMCHAT_AREA_FAIL		= 0,
	EMCHAT_AREA_OK			= 1,
	EMCHAT_AREA_NOMONEY		= 2,
	EMCHAT_AREA_BLOCK		= 3,
	EMCHAT_AREA_UNAPPLY		= 4,
};

enum EMCHAT_PARTY_RECRUIT
{
	EMCHAT_PARTY_RECRUIT_FAIL		= 0,
	EMCHAT_PARTY_RECRUIT_OK			= 1,
	EMCHAT_PARTY_RECRUIT_NOMONEY	= 2,
	EMCHAT_PARTY_RECRUIT_BLOCK		= 3,
	EMCHAT_PARTY_RECRUIT_TIME		= 4, // 시간 부족	
	EMCHAT_PARTY_RECRUIT_UNAPPLY	= 5,
};

enum EMCHAT_FACTION
{
	EMCHAT_FACTION_FAIL				= 0,
	EMCHAT_FACTION_OK				= 1,
	EMCHAT_FACTION_UNAPPLY			= 2,
	EMCHAT_FACTION_BLOCK			= 3,
};

enum EMCHAT_PRIVATE_FAIL
{
    EMCHAT_PRIVATE_FAIL_NOTFOUND   = 0, // 해당 유저를 찾을수 없음
    EMCHAT_PRIVATE_FAIL_BLOCK      = 1, // 블럭되었음
    EMCHAT_PRIVATE_FAIL_GM_IGNORE  = 2, // GM 캐릭터 귓속말 불가 상태
};

//
//mjeon.activity
//
enum
{
	HOW_MANY_TITLES_PER_ONE_SYNC		= 20,
	HOW_MANY_ACTVITITIES_PER_ONE_SYNC	= 50,
	HOW_MANY_CTFSTATISTICS_PER_ONE_MSG	= 15		//64 bytes each
};

enum SERVER_STOP_CODE
{
	SERVER_STOP_NOMAL			= 0, 
	SERVER_STOP_OUTOFMEMORY		= 1,	// db에서 'ado not enough storage is available to complete this operation' 에러 발생 db 관련 작업 안되는 상황, sqlite 에 저장을 한다.(CharacterBackup.h 참조)
	SERVER_STOP_JUST_RESTART	= 2,	// db 에러가 아닌 인던 생성이 안될 정도로 메모리 사용량이 많을 경우, db 상태는 괜찮을 경우 사용한다.(sqlite 저장하지 않음)
};


namespace GLMSG
{
//	#pragma pack(1)

	struct SNETPC_CHAT_BLOCK : public NET_MSG_GENERIC
	{
		__time64_t  tChatBlock;	        // 채팅 차단 해제 시간
		
		SNETPC_CHAT_BLOCK() 
			: tChatBlock(0)
		{
			dwSize = sizeof(SNETPC_CHAT_BLOCK);
			nType = NET_MSG_CHAT_BLOCK;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_BLOCK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_PRIVATE_FAIL : public NET_MSG_GENERIC
	{
        EMCHAT_PRIVATE_FAIL emFail;
		CHAR szName[CHR_ID_LENGTH+1];	// 특정 캐릭터명 or 사용자명

        SNETPC_CHAT_PRIVATE_FAIL(const EMCHAT_PRIVATE_FAIL _emFail) : emFail (_emFail)
		{
            MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_PRIVATE_FAIL) <= NET_DATA_BUFSIZE);

			dwSize = sizeof(SNETPC_CHAT_PRIVATE_FAIL);
			nType = NET_MSG_CHAT_PRIVATE_FAIL;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));			
		}

        void SetName(const char* Name)
        {
            if (Name)
                StringCchCopy(szName, CHR_ID_LENGTH+1, Name);
        }
	};

	// 채팅메시지 ( 링크용 ) 
	struct NET_CHAT_LINK //: public NET_MSG_GENERIC
	{
		//	채팅창 링크 갯수
		WORD           m_ChatType; // EMNET_MSG_CHAT 채팅 타입
        std::string    m_ChaName; //[CHR_ID_LENGTH+1];		// 특정 캐릭터명 or 사용자명
        std::string    m_ChatMsg; //[CHAT_MSG_SIZE+1];	// 채팅메시지
        std::vector<SLINK_DATA_BASIC> sLinkDataBasic; //[CHAT_LINK_MAX];

        MSGPACK_DEFINE(m_ChatType, m_ChaName, m_ChatMsg, sLinkDataBasic);

		NET_CHAT_LINK()
            : m_ChatType(CHAT_TYPE_NORMAL)
		{
			m_ChaName.clear();
			//nType = NET_MSG_CHAT_LINK;
			//dwSize = sizeof(NET_CHAT_LINK);
			//memset(m_ChaName,    0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			//memset(m_ChatMsg, 0, sizeof(CHAR) * (CHAT_MSG_SIZE+1));
			//memset(sLinkDataBasic, 0, sizeof(SLINK_DATA_BASIC) * CHAT_LINK_MAX );
            //MIN_STATIC_ASSERT(sizeof(NET_CHAT_LINK)<=NET_DATA_BUFSIZE);
		}
	};
	//typedef NET_CHAT_LINK* LPNET_CHAT_LINK;

	// 채팅메시지 링크 FB
	struct NET_CHAT_LINK_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		EMNET_MSG_CHAT	emType;						// 채팅 타입
		bool			bFactionChatting;
		CHAR			szName[CHR_ID_LENGTH+1];	// 보낸사람 캐릭터명 or 사용자명
		CHAR			szChatMsg[CHAT_MSG_SIZE+1];	// 채팅메시지
		SLINK_DATA_RECV	sLinkDataRecv[CHAT_LINK_MAX];
		DWORD			dwLinkDataSize;
		BYTE			LinkData[CHAT_LINK_DATA_SIZE];

		NET_CHAT_LINK_FB()
			: dwLinkDataSize(0)
			, bFactionChatting(false)
		{
			nType = NET_MSG_CHAT_LINK_FB;
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof(EMNET_MSG_CHAT) + sizeof(bool) + (sizeof(CHAR) * CHR_ID_LENGTH+1) +
						(sizeof(CHAR) * CHAT_MSG_SIZE+1) + (sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) + sizeof(DWORD);

			emType = CHAT_TYPE_NORMAL;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			memset(szChatMsg, 0, sizeof(CHAR) * (CHAT_MSG_SIZE+1));
			memset(sLinkDataRecv, 0, sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX );
			memset(LinkData,0, sizeof(BYTE) * CHAT_LINK_DATA_SIZE );

			MIN_STATIC_ASSERT(sizeof(NET_CHAT_LINK_FB)<=NET_DATA_BUFSIZE);
		}

		bool ADD( void* pBuffer, DWORD _dwSize )
		{
			if (dwLinkDataSize + _dwSize > CHAT_LINK_DATA_SIZE)
                return false;
			memcpy(LinkData+dwLinkDataSize, pBuffer, _dwSize);
			dwLinkDataSize += _dwSize;

			dwSize = sizeof(NET_MSG_CHARACTER) + 
                     sizeof(EMNET_MSG_CHAT) + 
					 sizeof(bool) +
                     (sizeof(CHAR) * CHR_ID_LENGTH+1) +
			         (sizeof(CHAR) * CHAT_MSG_SIZE+1) + 
                     (sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) + 
                     sizeof(DWORD) +
					 dwLinkDataSize;
			//MIN_STATIC_ASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET()
		{
			dwLinkDataSize = 0;			

			nType = NET_MSG_CHAT_LINK_FB;
			dwSize = sizeof(NET_MSG_CHARACTER) + 
                     sizeof(EMNET_MSG_CHAT) + 
					 sizeof(bool) +
                     (sizeof(CHAR) * CHR_ID_LENGTH+1) +
					 (sizeof(CHAR) * CHAT_MSG_SIZE+1) +
                     (sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) +
                     sizeof(DWORD) +
					 sizeof(bool);
			
			emType = CHAT_TYPE_NORMAL;
			bFactionChatting = false;
			memset(szName, 0, sizeof(CHAR) * (CHR_ID_LENGTH+1));
			memset(szChatMsg, 0, sizeof(CHAR) * (CHAT_MSG_SIZE+1));
			memset(sLinkDataRecv, 0, sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX );
			memset(LinkData,0, sizeof(BYTE) * CHAT_LINK_DATA_SIZE );
		}

        void SetData(
            EMNET_MSG _nType,
            EMNET_MSG_CHAT _emType,
            const CHAR* _szName,
            const CHAR* _szChatMsg)
        {
            nType = _nType;
            emType = _emType;
            SetName(_szName);
            SetChatMsg(_szChatMsg);
        }

        void SetData(
            EMNET_MSG_CHAT _emType,
            const CHAR* _szName,
            const CHAR* _szChatMsg)
        {            
            emType = _emType;
            SetName(_szName);
            SetChatMsg(_szChatMsg);
        }

        void SetData(
            EMNET_MSG_CHAT _emType,            
            const CHAR* _szChatMsg)
        {            
            emType = _emType;            
            SetChatMsg(_szChatMsg);
        }

        void SetName(const CHAR* _Name)
        {
            if (_Name)
                StringCchCopyA(szName, CHR_ID_LENGTH+1, _Name);
        }

        void SetChatMsg(const CHAR* _Msg)
        {
            if (_Msg)
                StringCchCopyA(szChatMsg, CHAT_MSG_SIZE+1, _Msg);
        }
	};
	typedef NET_CHAT_LINK_FB* LPNET_CHAT_LINK_FB;

	struct SNETPC_CHAT_LOUDSPEAKER //: public NET_MSG_GENERIC
	{
        WORD m_PosX;
		WORD m_PosY;

        std::string m_Msg; // [CHAT_MSG_SIZE+1];
        std::vector<SLINK_DATA_BASIC> m_vLinkDataBasic; //[CHAT_LINK_MAX];

        MSGPACK_DEFINE(m_PosX, m_PosY, m_Msg, m_vLinkDataBasic);

		SNETPC_CHAT_LOUDSPEAKER() 
			: m_PosX(0)
			, m_PosY(0)
		{
			//dwSize = sizeof(SNETPC_CHAT_LOUDSPEAKER);
			//nType = NET_MSG_CHAT_LOUDSPEAKER;
			//memset(m_Msg, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			//memset(m_vLinkDataBasic, 0, sizeof(SLINK_DATA_BASIC) * CHAT_LINK_MAX );
			//MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_LOUDSPEAKER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_LOUDSPEAKER_AGT : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		bool				bchannel_all;
		char				szMSG[CHAT_MSG_SIZE+1];
		SLINK_DATA_RECV		sLinkDataRecv[CHAT_LINK_MAX];

		DWORD				dwLinkDataSize;
		BYTE				LinkData[CHAT_LINK_DATA_SIZE];

		SNETPC_CHAT_LOUDSPEAKER_AGT() 
			: bchannel_all(false)
			, dwLinkDataSize(0)
		{
			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof ( bool ) + (sizeof(CHAR) * CHAT_MSG_SIZE+1) + 
						 (sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) + sizeof(DWORD);
			nType = NET_MSG_CHAT_LOUDSPEAKER_AGT;
			memset(szMSG, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			memset(sLinkDataRecv, 0, sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX );
			memset(LinkData,0, sizeof(BYTE) * CHAT_LINK_DATA_SIZE );		
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_LOUDSPEAKER_AGT)<=NET_DATA_BUFSIZE);
		}

		bool ADD( void* pBuffer, DWORD _dwSize )
		{
			if ( dwLinkDataSize + _dwSize > CHAT_LINK_DATA_SIZE ) return false;
			memcpy( LinkData+dwLinkDataSize, pBuffer, _dwSize );
			dwLinkDataSize += _dwSize;

			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof ( bool ) + (sizeof(CHAR) * CHAT_MSG_SIZE+1) + 
						(sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) + sizeof(DWORD) +	dwLinkDataSize;
			//MIN_STATIC_ASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
			return true;
		}

		void RESET()
		{
			dwLinkDataSize = 0;

			dwSize = sizeof(NET_MSG_CHARACTER) + sizeof ( bool ) + (sizeof(CHAR) * CHAT_MSG_SIZE+1) + 
						 (sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX) + sizeof(DWORD);
			nType = NET_MSG_CHAT_LOUDSPEAKER_AGT;
			memset(szMSG, 0, sizeof(char) * (CHAT_MSG_SIZE+1));
			memset(sLinkDataRecv, 0, sizeof(SLINK_DATA_RECV) * CHAT_LINK_MAX );
			memset(LinkData,0, sizeof(BYTE) * CHAT_LINK_DATA_SIZE );		
			//MIN_STATIC_ASSERT(nmg.dwSize<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_LOUDSPEAKER_FB : public NET_MSG_GENERIC
	{
		EMCHAT_LOUDSPEAKER	emFB;

		SNETPC_CHAT_LOUDSPEAKER_FB () 
			: emFB(EMCHAT_LOUDSPEAKER_FAIL)
		{
			dwSize = sizeof(SNETPC_CHAT_LOUDSPEAKER_FB);
			nType = NET_MSG_CHAT_LOUDSPEAKER_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_LOUDSPEAKER_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_AREA_FB : public NET_MSG_GENERIC
	{
		EMCHAT_AREA	emFB;

		SNETPC_CHAT_AREA_FB () 
			: emFB(EMCHAT_AREA_FAIL)
		{
			dwSize = sizeof(SNETPC_CHAT_AREA_FB);
			nType = NET_MSG_CHAT_AREA_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_AREA_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_CHAT_PARTY_RECRUIT_FB : public NET_MSG_GENERIC
	{
		EMCHAT_PARTY_RECRUIT	emFB;
		float					fPartyRecruitTime;

		SNETPC_CHAT_PARTY_RECRUIT_FB () 
			: emFB(EMCHAT_PARTY_RECRUIT_FAIL)
			, fPartyRecruitTime( 0.0f ) 
		{
			dwSize = sizeof(SNETPC_CHAT_PARTY_RECRUIT_FB);
			nType = NET_MSG_CHAT_PARTY_RECRUIT_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHAT_PARTY_RECRUIT_FB)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_CHAT_PARTY_RECRUIT_FB_AF : public NET_MSG_GENERIC
    {
        DWORD m_ChaDbNum;
        EMCHAT_PARTY_RECRUIT emFB;
        float fPartyRecruitTime;
        unsigned int Crc32;

        NET_CHAT_PARTY_RECRUIT_FB_AF(DWORD ChaDbNum)
            : m_ChaDbNum(ChaDbNum)
            , emFB(EMCHAT_PARTY_RECRUIT_FAIL)
            , fPartyRecruitTime(0.0f)
            , Crc32(0)
        {
            dwSize = sizeof(NET_CHAT_PARTY_RECRUIT_FB_AF);
            nType = NET_MSG_CHAT_PARTY_RECRUIT_FB_AF;
            MIN_STATIC_ASSERT(sizeof(NET_CHAT_PARTY_RECRUIT_FB_AF)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_CHAT_FACTION_FB : public NET_MSG_GENERIC
	{
		EMCHAT_FACTION			emFB;

		SNETPC_CHAT_FACTION_FB () 
			: emFB ( EMCHAT_FACTION_FAIL )
		{
			dwSize = sizeof( SNETPC_CHAT_FACTION_FB );
			nType = NET_MSG_CHAT_FACTION_FB;
			MIN_STATIC_ASSERT( sizeof( SNETPC_CHAT_FACTION_FB ) <= NET_DATA_BUFSIZE );
		}
	};
	
	struct SNETDROP_ITEM //: public NET_MSG_GENERIC
	{
		SDROP_CLIENT_ITEM Data;

        MSGPACK_DEFINE(Data);

		SNETDROP_ITEM()
		{
			//dwSize = sizeof(SNETDROP_ITEM);
			//nType = NET_MSG_DROP_ITEM;
			//MIN_STATIC_ASSERT(sizeof(SNETDROP_ITEM)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETDROP_MONEY //: public NET_MSG_GENERIC
	{
		float				fAge;
		DWORD				dwGlobID;
		//D3DXVECTOR3			vPos;
        se::Vector3         vPos;
		LONGLONG			lnAmount;
		BOOL				bCanGet;

        MSGPACK_DEFINE(fAge, dwGlobID, vPos, lnAmount, bCanGet);

		SNETDROP_MONEY() 
			: fAge(0)
			, dwGlobID(0)
			, vPos(0,0,0)
			, lnAmount(0)
			, bCanGet(FALSE)
		{
			//dwSize = sizeof(SNETDROP_MONEY);
			//nType = NET_MSG_DROP_MONEY;
			//MIN_STATIC_ASSERT(sizeof(SNETDROP_MONEY)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETDROP_CROW : public NET_MSG_GENERIC
	{
		SDROP_CROW Data;

		SNETDROP_CROW()
		{
			dwSize = sizeof(SNETDROP_CROW);
			nType = NET_MSG_GCTRL_DROP_CROW;
			MIN_STATIC_ASSERT(sizeof(SNETDROP_CROW)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETDROP_PC_BASE : public NET_MSG_GENERIC
	{		
        SDROP_CHAR_BASE m_Data;
        
		SNETDROP_PC_BASE()
		{
			dwSize = sizeof(SNETDROP_PC_BASE);
			nType = NET_MSG_GCTRL_DROP_PC_BASE;
			MIN_STATIC_ASSERT(sizeof(SNETDROP_PC_BASE)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETDROP_PC_STATEBLOW_SKILLFACT : public NET_MSG_GENERIC
    {
        DWORD m_ChaDbNum;

        SSTATEBLOW		sSTATEBLOWS[EMBLOW_MULTI];
        SDROP_SKILLFACT	sSKILLFACT[SKILLFACT_SIZE];

        SNETDROP_PC_STATEBLOW_SKILLFACT(DWORD ChaDbNum)
            : m_ChaDbNum(ChaDbNum)
        {
            dwSize = sizeof(SNETDROP_PC_STATEBLOW_SKILLFACT);
            nType = NET_MSG_GCTRL_DROP_PC_SKILLFACT;
            MIN_STATIC_ASSERT(sizeof(SNETDROP_PC_STATEBLOW_SKILLFACT)<=NET_DATA_BUFSIZE);
        }
    };

    struct  SNETDROP_PC_PUT_ON_ITEMS : public NET_MSG_GENERIC
    {
        DWORD m_ChaDbNum;

        SITEMCLIENT		m_PutOnItems[SLOT_NSIZE_S_2];	//	착용 Item

        SNETDROP_PC_PUT_ON_ITEMS(DWORD ChaDbNum)
            : m_ChaDbNum(ChaDbNum)
        {
            dwSize = sizeof(SNETDROP_PC_PUT_ON_ITEMS);
            nType = NET_MSG_GCTRL_DROP_PC_PUT_ON_ITEMS;
            MIN_STATIC_ASSERT(sizeof(SNETDROP_PC_PUT_ON_ITEMS)<=NET_DATA_BUFSIZE);
        }
    };

    struct SNETDROP_PC_END : public NET_MSG_GENERIC
    {
        BOOL Data;
        SNETDROP_PC_END()
            : Data(true)
        {
            dwSize = sizeof(SNETDROP_PC_END);
            nType = NET_MSG_GCTRL_DROP_PC_END;
            MIN_STATIC_ASSERT(sizeof(SNETDROP_PC_END)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNETDROP_MATERIAL : public NET_MSG_GENERIC
	{	
		SDROP_MATERIAL Data;
		SNETDROP_MATERIAL()
		{
			dwSize = sizeof(SNETDROP_MATERIAL);
			nType = NET_MSG_GCTRL_DROP_MATERIAL;
			MIN_STATIC_ASSERT(sizeof(SNETDROP_MATERIAL)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETDROP_OUT : public NET_MSG_GENERIC
	{
		enum { MAX_NUM = 100, };

		WORD				cNUM;
		WORD				cCUR;
		STARID				sOUTID[MAX_NUM];

		SNETDROP_OUT() 
			: cNUM(0)
			, cCUR(0)
		{
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD) * 2;
			nType = NET_MSG_GCTRL_DROP_OUT;
			MIN_STATIC_ASSERT(sizeof(SNETDROP_OUT)<=NET_DATA_BUFSIZE);
		}

		void RESET_CROW()
		{
			cNUM = 0;
			cCUR = 0;
		}

		const WORD GETAMOUNT()
		{
			return cNUM;
		}

		const bool ADD_CROW(const STARID &cOUT)
		{
			if ( cNUM >= MAX_NUM )
			{
				return false;
			}
			else
			{
				sOUTID[cNUM++] = cOUT;
				dwSize = sizeof(NET_MSG_GENERIC) + sizeof(WORD) * 2 + sizeof(STARID)*cNUM;
				return true;
			}
		}

		void RESET_CUR()
		{
			cCUR = 0;
		}

		const bool POP_CROW(STARID &cOUT)
		{
			if ( cNUM == cCUR )
			{
				return false;
			}
			else
			{
				cOUT = sOUTID[cCUR++];
				return true;
			}
		}
	};

	
	struct SNET_DROP_OUT_FORCED : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		//DWORD				dwCharID;
		EMDROP_OUT_FORCED	emForced;

		SNET_DROP_OUT_FORCED() 
			//: dwCharID(0)
			:emForced(EMDROPOUT_REQLOGIN)
		{
			dwSize = sizeof(SNET_DROP_OUT_FORCED);
			nType = NET_MSG_GCTRL_DROP_OUT_FORCED;
			MIN_STATIC_ASSERT(sizeof(SNET_DROP_OUT_FORCED)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_GENITEM_AGT : public NET_MSG_GENERIC
	{
		int					nCHANNEL;
		SNATIVEID			sNID;

		SNATIVEID			sMAPID;
		EMITEMGEN			emGENTYPE;
		EMGROUP				emHoldGroup;
		DWORD				dwHoldGID;
		D3DXVECTOR3			vPos;

		SNET_REQ_GENITEM_AGT () 
			: nCHANNEL(0)
			, sNID(false)
			
			, sMAPID(false)
			, emGENTYPE(EMGEN_DEFAULT)
			, emHoldGroup(EMGROUP_ONE)
			, dwHoldGID(GAEAID_NULL)
			, vPos(0,0,0)
		{
			dwSize = sizeof(SNET_REQ_GENITEM_AGT);
			nType = NET_MSG_GCTRL_REQ_GENITEM_AGT;
			MIN_STATIC_ASSERT(sizeof(SNET_REQ_GENITEM_AGT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_GENITEM_FLD_FB : public NET_MSG_GENERIC
	{
		SNATIVEID			sNID;

		SNATIVEID			sMAPID;
		EMITEMGEN			emGENTYPE;
		sc::MGUID			guid;
		EMGROUP				emHoldGroup;
		DWORD				dwHoldGID;
		D3DXVECTOR3			vPos;

		SNET_REQ_GENITEM_FLD_FB () 
			: sNID(false)
			
			, sMAPID(false)
			, emGENTYPE(EMGEN_DEFAULT)
			, emHoldGroup(EMGROUP_ONE)
			, dwHoldGID(GAEAID_NULL)
			, vPos(0,0,0)
		{
			dwSize = sizeof(SNET_REQ_GENITEM_FLD_FB);
			nType = NET_MSG_GCTRL_REQ_GENITEM_FLD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_REQ_GENITEM_FLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_SERVERSTOP : public NET_MSG_GENERIC
	{
		enum { CODE_MAGIC_NUMBER=7952, };
		DWORD dwCode;
		DWORD dwError;

		SNET_REQ_SERVERSTOP ()
			: dwCode(CODE_MAGIC_NUMBER)
			, dwError( SERVER_STOP_NOMAL )
		{
			dwSize = sizeof(SNET_REQ_SERVERSTOP);
			nType = NET_MSG_GCTRL_SERVERSTOP;
			MIN_STATIC_ASSERT(sizeof(SNET_REQ_SERVERSTOP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_REQ_SERVERSTOP_MSG : public NET_MSG_GENERIC
	{
		enum { CODE_MAGIC_NUMBER = 1977, };
		DWORD dwCode;
		DWORD dwError;

		SNET_REQ_SERVERSTOP_MSG ()
			: dwCode(CODE_MAGIC_NUMBER)
			, dwError( SERVER_STOP_JUST_RESTART )
		{
			dwSize = sizeof( SNET_REQ_SERVERSTOP_MSG );
			nType = NET_MSG_SEND_SERVER_STOP_MSG;
			MIN_STATIC_ASSERT( sizeof( SNET_REQ_SERVERSTOP_MSG ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_REQ_SERVERSTOP_FB : public NET_MSG_GENERIC
	{
		DWORD dwError;

		SNET_REQ_SERVERSTOP_FB() 
			: dwError( SERVER_STOP_NOMAL )
		{
			dwSize = sizeof(SNET_REQ_SERVERSTOP_FB);
			nType = NET_MSG_GCTRL_SERVERSTOP_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_REQ_SERVERSTOP_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_MSG_SERVERTIME_BRD : public NET_MSG_GENERIC
	{
		__time64_t			t64Time;
		int					nTimeBias;

		SNET_MSG_SERVERTIME_BRD () 
			: t64Time ( 0 )
			, nTimeBias ( 0 )
		{
			dwSize = sizeof(SNET_MSG_SERVERTIME_BRD);
			nType = NET_MSG_SERVERTIME_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_MSG_SERVERTIME_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//mjeon.Add
	//Network Messages for Post
	//

	struct SNET_POST_REQ_CHANUM : public NET_MSG_GENERIC
	{        
		int		nReqID;				// This ID is set by Client-side and used for identify 
									// matching of Post-Reqeust and Post-Response, especially ReqChaNum_FB and PostSend.
									// And same ID will be included in all related-msgs to this reuqest.
		//UINT	nUserNum;
		char	szChaName[CHR_ID_LENGTH+1];

		SNET_POST_REQ_CHANUM()
		{
			dwSize = sizeof(SNET_POST_REQ_CHANUM);
			nType = NET_MSG_POST_REQ_CHANUM;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_REQ_CHANUM)<=NET_DATA_BUFSIZE);
		}

        void SetChaName(const char* szName)
        {
            if (szName)
                StringCchCopy(szChaName, CHR_ID_LENGTH+1, szName);
        }
    };

	struct SNET_POST_REQ_CHANUM_FB : public NET_MSG_GENERIC
	{		
		int		nReqID;
		
		int		nChaNum;
		int		nUserNum;		//Added for checking equivalent account.

		SNET_POST_REQ_CHANUM_FB()
		{
			dwSize = sizeof(SNET_POST_REQ_CHANUM_FB);
			nType = NET_MSG_POST_REQ_CHANUM_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_REQ_CHANUM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_SEND : public NET_MSG_GENERIC
	{		
		int			nReqID;		// This ID is set by Client-side and used for identify 
								// matching of Post-Reqeust and Post-Response, especially ReqChaNum_FB and PostSend.
								// And same ID will be included in all related-msgs to this reuqest.
		
		BYTE		byPostState;
		BYTE		byPostType;
		BYTE		byPostClass;
		BYTE		byPayment;

		int			iSender;
		int			iRecver;
		int			iDesign;
		int			nUserNumRecver;	//Recver'sUserNum

		LONGLONG	llCharge;
		LONGLONG	llAttachMoney;	//Change the Sender's money and then put the money into DB.

		//char		SendDate[20];	//yyyy-mm-dd hh:mm:ss
		char		Title[POST_MAX_TITLE_LENGTH];
		char		Content[POST_MAX_CONTENT_LENGTH];
		
		//char		SenderName[CHR_ID_LENGTH +3];	//SenderName MUST be set at server-side.
		char		RecverName[CHR_ID_LENGTH +3];	//+3 : to avoid margin

		// Attached Item		
		WORD		xAttach01;
		WORD		yAttach01;
		WORD		nAttach01;
		SNATIVEID	idAttach01;

		SNET_POST_SEND()
		{
			dwSize = sizeof(SNET_POST_SEND);
			nType = NET_MSG_POST_SEND;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_SEND)<=NET_DATA_BUFSIZE);
		}
	};

	//
	// After POST_SEND
	//
	// SEND following msgs to the sender from the server.
	//
	// SNET_INVEN_DELETE	(for attached item)
	// SNET_UPDATE_MONEY	(for character's remain money)
	//
	// And then send POST_SEND_FB
	//

	// FieldServer -> AgentServer -> Client
	// Therefore GaeaID is required.
	struct SNET_POST_SEND_FB : public NET_MSG_GENERIC
	{
		int nReqID;


		/*
		enum EMPOST_RESULT
		{
			EMPOST_OK				= 0,	// OK
			EMPOST_RECVER_NOTEXIST	= 1,	// There does not exist the character(Recver)	
			EMPOST_NOTENOUGH_MONEY		= 2,	// Wrong amount of money
			EMPOST_WRONG_ATTACH01	= 3,	// Wrong item for attach01
			EMPOST_WRONG_CHARGE		= 4,	// Wrong item for charge
		};
		*/
		BYTE		byResut;	// EMPOST_RESULT
		int			iSender;	// Sender's ChaNum

		SNET_POST_SEND_FB()
		{
			dwSize = sizeof(SNET_POST_SEND_FB);
			nType = NET_MSG_POST_SEND_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_SEND_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_POST_TODO_AFTER_SEND_FF : public NET_MSG_GENERIC
	{
		int			nReturn;	//by DB
		SPOSTINFO	postinfo;

		SNET_POST_TODO_AFTER_SEND_FF()
			:nReturn(0)
		{
			dwSize = sizeof(SNET_POST_TODO_AFTER_SEND_FF);
			nType = NET_MSG_POST_TODO_AFTER_SEND_FF;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_TODO_AFTER_SEND_FF)<=NET_DATA_BUFSIZE);
		}
	};
	



	struct SNET_POST_NOTIFY : public NET_MSG_GENERIC
	{
        enum { POST_MAGIC_NUMBER=3379, };

		LONGLONG	llPostID;
		int			iRecver;	//Recver's ChaNum
        int         PostMagicNumber;

		SNET_POST_NOTIFY()
            : PostMagicNumber(POST_MAGIC_NUMBER)
		{
			dwSize = sizeof(SNET_POST_NOTIFY);
			nType = NET_MSG_POST_NOTIFY;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_NOTIFY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_NOTIFY_RELAY : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;
		int			iRecver;	//Recver's ChaNum

		SNET_POST_NOTIFY_RELAY()
		{
			dwSize = sizeof(SNET_POST_NOTIFY_RELAY);
			nType = NET_MSG_POST_NOTIFY_RELAY;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_NOTIFY_RELAY)<=NET_DATA_BUFSIZE);
		}
	};

	//
	// This msg will be sent when the character clicks the PostBox.
	//
	// However, the DBAction for SNET_POST_GET can be done by different two function.
	// One is the SNET_POST_GET MsgHandler and the other is the function that handles character-join. 
	// In other words, POST_GET is done when a character joins the game(into AgentServer) 
	// and clicks a postbox in the game.
	//
	// Client -> Agent
	//
	struct SNET_POST_GET : public NET_MSG_GENERIC
	{
		int		iRecver;		// Recver chaNum
		BYTE	bGetOnlyInfo;	// If this flag is set, server will send only post information but not real post data.

		SNET_POST_GET()
		{
			dwSize = sizeof(SNET_POST_GET);
			nType = NET_MSG_POST_GET;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Agent -> Client
	//
	struct SNET_POST_GET_INFO_FB : public NET_MSG_GENERIC
	{
		int		iRecver;	// Recver chaNum
		int		cntAllPost;	// The number of all Posts in DB (How many posts exist in DB?)
		int		cntNewPost;	// The number of new Posts in DB (How many posts exist in DB?)
								
		SNET_POST_GET_INFO_FB()
		{
			dwSize = sizeof(SNET_POST_GET_INFO_FB);
			nType = NET_MSG_POST_GET_INFO_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_INFO_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Agent -> Client
	//
	struct SNET_POST_GET_DATA_FB : public NET_MSG_GENERIC
	{	
		int		iRecver;		// Recver chaNum 
		int		nRowCount;		// Number rows from SELECT TOP(31) from DB (0~31) 
								// 31 means there are more than 30 posts. 
								// We don't know exactly how many posts remain. 
								// To do know that, need to receive POST_GET_INFO_FB first.

		int		nPostNum;		// Current post# (0~29)
		SAPOST	aPOST;			// A Post data

		SNET_POST_GET_DATA_FB()
		{
			dwSize = sizeof(SNET_POST_GET_DATA_FB);
			nType = NET_MSG_POST_GET_DATA_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_DATA_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //
    // Get PostState including sent posts
    //
    struct SNET_POST_GET_STATE : public NET_MSG_GENERIC
    {
        int		iRecver;		// Recver chaNum		

        SNET_POST_GET_STATE()
        {
            dwSize = sizeof(SNET_POST_GET_STATE);
            nType = NET_MSG_POST_GET_STATE;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_STATE)<=NET_DATA_BUFSIZE);
        }
    };

    //
    //Agent -> Client
    //
    struct SNET_POST_GET_STATE_FB : public NET_MSG_GENERIC
    {
        int		iRecver;	// Recver chaNum
        int		nRowCount;	// Total RowCount
        int		nPostNum;	// Current PostNum

        SAPOSTSTATE aPOSTSTATE;


        SNET_POST_GET_STATE_FB()
        {
            dwSize = sizeof(SNET_POST_GET_STATE_FB);
            nType = NET_MSG_POST_GET_STATE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_STATE_FB)<=NET_DATA_BUFSIZE);
        }
    };


    //
    // Get Sent PostState
    //
    struct SNET_POST_GET_SEND_STATE : public SNET_POST_GET_STATE
    {
        SNET_POST_GET_SEND_STATE()
        {
            dwSize = sizeof(SNET_POST_GET_SEND_STATE);
            nType = NET_MSG_POST_GET_SEND_STATE;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_SEND_STATE)<=NET_DATA_BUFSIZE);
        }
    };

    //
    //Agent -> Client
    //
    struct SNET_POST_GET_SEND_STATE_FB : public SNET_POST_GET_STATE_FB
    {
        SNET_POST_GET_SEND_STATE_FB()
        {
            dwSize = sizeof(SNET_POST_GET_SEND_STATE_FB);
            nType = NET_MSG_POST_GET_SEND_STATE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_SEND_STATE_FB)<=NET_DATA_BUFSIZE);
        }
    };


    //
    // Get Received PostState
    //
    struct SNET_POST_GET_RECV_STATE : public SNET_POST_GET_STATE
    {
        SNET_POST_GET_RECV_STATE()
        {
            dwSize = sizeof(SNET_POST_GET_RECV_STATE);
            nType = NET_MSG_POST_GET_RECV_STATE;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_RECV_STATE)<=NET_DATA_BUFSIZE);
        }
    };

    //
    //Agent -> Client
    //
    struct SNET_POST_GET_RECV_STATE_FB : public SNET_POST_GET_STATE_FB
    {
        SNET_POST_GET_RECV_STATE_FB()
        {
            dwSize = sizeof(SNET_POST_GET_RECV_STATE_FB);
            nType = NET_MSG_POST_GET_RECV_STATE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_RECV_STATE_FB)<=NET_DATA_BUFSIZE);
        }
    };


	//
	//Client -> Agent
	//
	//User just opened the post and then closed the post.
	//There was no action for attachment.
	//
	struct SNET_POST_OPEN : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;

		SNET_POST_OPEN()
		{
			dwSize = sizeof(SNET_POST_OPEN);
			nType = NET_MSG_POST_OPEN;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_OPEN)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Client -> AGENT
	//
	//User opened the post and took some attachment (item or money or both).
	//This msg should be handled in FieldServer because of item and money process.
	//
	struct SNET_POST_OPEN_TAKE : public NET_MSG_GENERIC
	{		
		LONGLONG	llPostID;
		INT			iRecver;		
		BYTE		byAttach01Accept;
		BYTE		byMoneyAccept;

		SNET_POST_OPEN_TAKE()
		{
			dwSize = sizeof(SNET_POST_OPEN_TAKE);
			nType = NET_MSG_POST_OPEN_TAKE;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_OPEN_TAKE)<=NET_DATA_BUFSIZE);
		}
	};

	//Agent -> Field
	struct SNET_POST_OPEN_TAKE_AF : public NET_MSG_GENERIC
	{
		//LONGLONG	llPostID;

		SAPOST		aPOST;				// Current status of the POST. to compare to user's reqeust.

		BYTE		byAttach01Accept;	// User command - Has user requested Attach01Accept?
		BYTE		byMoneyAccept;		// User command - Has user requested MoneyAccept?

		SNET_POST_OPEN_TAKE_AF()
		{
			dwSize = sizeof(SNET_POST_OPEN_TAKE_AF);
			nType = NET_MSG_POST_OPEN_TAKE_AF;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_OPEN_TAKE_AF)<=NET_DATA_BUFSIZE);
		}
	};

	//Field -> Agent
	struct SNET_POST_OPEN_TAKE_AF_FB : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;

		INT			iRecver;

		BYTE		byAttach01Accept;
		BYTE		byMoneyAccept;
		SHORT		result;

		SNET_POST_OPEN_TAKE_AF_FB()
		{
			dwSize = sizeof(SNET_POST_OPEN_TAKE_AF_FB);
			nType = NET_MSG_POST_OPEN_TAKE_AF_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_OPEN_TAKE_AF_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Agent -> Client
	//
	struct SNET_POST_OPEN_TAKE_FB : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;

		BYTE		byAttach01Accept;
		BYTE		byMoneyAccept;
		SHORT		result;		

		SNET_POST_OPEN_TAKE_FB()
		{
			dwSize = sizeof(SNET_POST_OPEN_TAKE_FB);
			nType = NET_MSG_POST_OPEN_TAKE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_OPEN_TAKE_FB)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_POST_RETURN : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;

		SNET_POST_RETURN()
		{
			dwSize = sizeof(SNET_POST_RETURN);
			nType = NET_MSG_POST_RETURN;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_RETURN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_RETURN_FB : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;
		INT			Result;

		SNET_POST_RETURN_FB()
		{
			dwSize = sizeof(SNET_POST_RETURN_FB);
			nType = NET_MSG_POST_RETURN_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_RETURN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_RETURN_OPEN : public NET_MSG_GENERIC
	{
		LONGLONG	llPostID;

		SNET_POST_RETURN_OPEN()
		{
			dwSize = sizeof(SNET_POST_RETURN_OPEN);
			nType = NET_MSG_POST_RETURN_OPEN;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_RETURN_OPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_RETURN_OPEN_FB : public NET_MSG_GENERIC
	{
		SNET_POST_RETURN_OPEN_FB()
		{
			dwSize = sizeof(SNET_POST_RETURN_OPEN_FB);
			nType = NET_MSG_POST_RETURN_OPEN_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_RETURN_OPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_DELETE : public NET_MSG_GENERIC
	{
		LONGLONG			llPostID;

		SNET_POST_DELETE()
		{
			dwSize = sizeof(SNET_POST_DELETE);
			nType = NET_MSG_POST_DELETE;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_DELETE_FB : public NET_MSG_GENERIC
	{
		LONGLONG llPostID;

		SNET_POST_DELETE_FB()
		{
			dwSize = sizeof(SNET_POST_DELETE_FB);
			nType = NET_MSG_POST_DELETE_FB;
            MIN_STATIC_ASSERT(sizeof(SNET_POST_DELETE_FB)<=NET_DATA_BUFSIZE);
		}
	};


	// ---------------------- SNS --------------------------
	// -----------------------------------------------------
	//mjeon.sns
	
	//
	// ----------------- Facebook -----------------
	//
    struct SNET_SNS_FB_AUTHED : public NET_MSG_GENERIC
	{
		char	SKey[SNS_MAX_SKEY_LENGTH];
		char	UID[SNS_MAX_UID_LENGTH];

		SNET_SNS_FB_AUTHED()
		{
			dwSize = sizeof(SNET_SNS_FB_AUTHED);
			nType = NET_MSG_SNS_FB_AUTHED;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_AUTHED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_FB_AUTHED_FB : public NET_MSG_GENERIC
	{
		int		result;

		SNET_SNS_FB_AUTHED_FB()
		{
			dwSize = sizeof(SNET_SNS_FB_AUTHED_FB);
			nType = NET_MSG_SNS_FB_AUTHED_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_AUTHED_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_FB_UNAUTHED : public NET_MSG_GENERIC
	{
		SNET_SNS_FB_UNAUTHED()
		{
			dwSize = sizeof(SNET_SNS_FB_UNAUTHED);
			nType = NET_MSG_SNS_FB_UNAUTHED;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_UNAUTHED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_FB_UNAUTHED_FB : public NET_MSG_GENERIC
	{
		int		result;

		SNET_SNS_FB_UNAUTHED_FB()
		{
			dwSize = sizeof(SNET_SNS_FB_UNAUTHED_FB);
			nType = NET_MSG_SNS_FB_UNAUTHED_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_UNAUTHED_FB)<=NET_DATA_BUFSIZE);
		}
	};


	//
	// AgentS -> FieldS
	//
	struct SNET_SNS_FB_UPDATE_AF : public NET_MSG_GENERIC
	{
		SFACEBOOK	AUTHFB;

		SNET_SNS_FB_UPDATE_AF()
		{
			dwSize = sizeof(SNET_SNS_FB_UPDATE_AF);
			nType = NET_MSG_SNS_FB_UPDATE_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_UPDATE_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_FB_GETUID : public NET_MSG_GENERIC
	{
		DWORD	dwTargetGaeaID;	// Field GaeaID of Target Cha

		SNET_SNS_FB_GETUID()
		{
			dwSize = sizeof(SNET_SNS_FB_GETUID);
			nType = NET_MSG_SNS_FB_GETUID;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_GETUID)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SNS_FB_GETUID_FB : public NET_MSG_GENERIC
	{
		DWORD	dwTargetGaeaID;
		int		result;

		char	TargetUID[SNS_MAX_UID_LENGTH];

		SNET_SNS_FB_GETUID_FB()
		{
			dwSize = sizeof(SNET_SNS_FB_GETUID_FB);
			nType = NET_MSG_SNS_FB_GETUID_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_GETUID_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_SNS_FB_UPDATE_BR : public NET_MSG_GENERIC
	{
		DWORD		dwGaeaID;		
		BYTE		byFB;

		SNET_SNS_FB_UPDATE_BR()
		{
			dwSize = sizeof(SNET_SNS_FB_UPDATE_BR);
			nType = NET_MSG_SNS_FB_UPDATE_BR;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_FB_UPDATE_BR)<=NET_DATA_BUFSIZE);
		}
	};



	//
	// ----------------- Twitter -----------------
	//
	struct SNET_SNS_TW_AUTHED : public NET_MSG_GENERIC
	{
		char	AToken[SNS_MAX_TOKEN_LENGTH];
		char	ATokenS[SNS_MAX_TOKEN_LENGTH];
		char	UID[SNS_MAX_UID_LENGTH];

		SNET_SNS_TW_AUTHED()
		{
			dwSize = sizeof(SNET_SNS_TW_AUTHED);
			nType = NET_MSG_SNS_TW_AUTHED;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_AUTHED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_AUTHED_FB : public NET_MSG_GENERIC
	{
		int		result;

		SNET_SNS_TW_AUTHED_FB()
		{
			dwSize = sizeof(SNET_SNS_TW_AUTHED_FB);
			nType = NET_MSG_SNS_TW_AUTHED_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_AUTHED_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_UNAUTHED : public NET_MSG_GENERIC
	{
		SNET_SNS_TW_UNAUTHED()
		{
			dwSize = sizeof(SNET_SNS_TW_UNAUTHED);
			nType = NET_MSG_SNS_TW_UNAUTHED;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_UNAUTHED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_UNAUTHED_FB : public NET_MSG_GENERIC
	{
		int		result;

		SNET_SNS_TW_UNAUTHED_FB()
		{
			dwSize = sizeof(SNET_SNS_TW_UNAUTHED_FB);
			nType = NET_MSG_SNS_TW_UNAUTHED_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_UNAUTHED_FB)<=NET_DATA_BUFSIZE);
		}
	};


	//
	// AgentS -> FieldS
	//
	struct SNET_SNS_TW_UPDATE_AF : public NET_MSG_GENERIC
	{
		STWITTER	AUTHTW;

		SNET_SNS_TW_UPDATE_AF()
		{
			dwSize = sizeof(SNET_SNS_TW_UPDATE_AF);
			nType = NET_MSG_SNS_TW_UPDATE_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_UPDATE_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_GETUID : public NET_MSG_GENERIC
	{
		DWORD	dwTargetGaeaID;	// Field GaeaID of Target Cha

		SNET_SNS_TW_GETUID()
		{
			dwSize = sizeof(SNET_SNS_TW_GETUID);
			nType = NET_MSG_SNS_TW_GETUID;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_GETUID)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_GETUID_FB : public NET_MSG_GENERIC
	{
		DWORD	dwTargetGaeaID;
		int		result;

		char	TargetUID[SNS_MAX_UID_LENGTH];

		SNET_SNS_TW_GETUID_FB()
		{
			dwSize = sizeof(SNET_SNS_TW_GETUID_FB);
			nType = NET_MSG_SNS_TW_GETUID_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_GETUID_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_TW_UPDATE_BR : public NET_MSG_GENERIC
	{
		DWORD		dwGaeaID;		
		BYTE		byTW;

		SNET_SNS_TW_UPDATE_BR()
		{
			dwSize = sizeof(SNET_SNS_TW_UPDATE_BR);
			nType = NET_MSG_SNS_TW_UPDATE_BR;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_TW_UPDATE_BR)<=NET_DATA_BUFSIZE);

		}
	};
	


	// -------------------- SNS Common --------------------


	//
	// Get my character's facebook info
	//
	struct SNET_SNS_GETINFO : public NET_MSG_GENERIC
	{
		SNET_SNS_GETINFO()
		{
			dwSize = sizeof(SNET_SNS_GETINFO);
			nType = NET_MSG_SNS_GETINFO;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_GETINFO)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_SNS_GETINFO_FB : public NET_MSG_GENERIC
	{
		int		result;

		char	FBSKey[SNS_MAX_SKEY_LENGTH];
		char	FBUID[SNS_MAX_UID_LENGTH];

		char	TWAToken[SNS_MAX_TOKEN_LENGTH];
		char	TWATokenS[SNS_MAX_TOKEN_LENGTH];
		char	TWUID[SNS_MAX_UID_LENGTH];

		SNET_SNS_GETINFO_FB()
		{
			dwSize = sizeof(SNET_SNS_GETINFO_FB);
			nType = NET_MSG_SNS_GETINFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_SNS_GETINFO_FB)<=NET_DATA_BUFSIZE);
		}
	};


	//
	//mjeon.attendance
	//
	// ------------------ Attendance ------------------
	//
	struct SNET_ATTENDANCE_SYNC_FF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_SYNC_FF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_SYNC_FF);
			nType =  NET_MSG_ATTENDANCE_SYNC_FF;			
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_SYNC_FF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_RESET_FF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_RESET_FF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_RESET_FF);
			nType =  NET_MSG_ATTENDANCE_RESET_FF;			
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_RESET_FF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_UPGRADE_FF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_UPGRADE_FF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_UPGRADE_FF);
			nType =  NET_MSG_ATTENDANCE_UPGRADE_FF;			
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_UPGRADE_FF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_SYNC_FAC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(UINT) * 3,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};

		UINT	nChaNum;	//for Relay Field-to-Agent-to-Client

		UINT	nDayN;

		UINT	nTasks;	//how many tasks exist for the attendance?

		TaskN	Tasks[MAX_TASKS_PER_ONE_DAY];

		
		BOOL SetData(UINT nDay, TaskN *arrTasks, UINT nTasks_)
		{
			if (arrTasks == NULL)
				return FALSE;

			if (nTasks_ > MAX_TASKS_PER_ONE_DAY)
				return FALSE;


			size_t size = nTasks_ * sizeof(TaskN);

			if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ATTENDANCE_SYNC_FAC %1%", size ));
				return FALSE;
			}

			nDayN	= nDay;
			nTasks	= nTasks_;
			
			for (UINT i=0; i<nTasks_; i++)
			{
				Tasks[i] = arrTasks[i];
			}

			dwSize = (DWORD)(BASIC_SIZE + nTasks_ * sizeof(TaskN));
			
			return TRUE;
		}


		SNET_ATTENDANCE_SYNC_FAC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_SYNC_FAC);
			nType =  NET_MSG_ATTENDANCE_SYNC_FAC;
			nDayN = 1;
			nTasks= 1;
			nChaNum = 0;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_SYNC_FAC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_RESET_FAC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(UINT) * 2,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};

		UINT	nChaNum;	//for Relay Field-to-Agent-to-Client

		UINT	nTasks;	//how many tasks exist for the attendance?

		TaskN	Tasks[MAX_TASKS_PER_ONE_DAY];

		BOOL SetData(TaskN *arrTasks, UINT nTasks_)
		{
			if (arrTasks == NULL)
				return FALSE;

			if (nTasks_ > MAX_TASKS_PER_ONE_DAY)
				return FALSE;


			size_t size = nTasks_ * sizeof(TaskN);

			if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ATTENDANCE_RESET_FAC %1%", size ));
				return FALSE;
			}
			
			nTasks	= nTasks_;
			
			for (UINT i=0; i<nTasks_; i++)
			{
				Tasks[i] = arrTasks[i];
			}

			dwSize = (DWORD)(BASIC_SIZE + size);
			
			return TRUE;
		}

		SNET_ATTENDANCE_RESET_FAC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_RESET_FAC);
			nType =  NET_MSG_ATTENDANCE_RESET_FAC;			
			nTasks= 1;
			nChaNum = 0;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_RESET_FAC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_UPGRADE_FAC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(UINT) * 3,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};

		UINT	nChaNum;

		UINT	nDayN;

		UINT	nTasks;	//how many tasks exist for the attendance?

		TaskN	Tasks[MAX_TASKS_PER_ONE_DAY];

		
		BOOL SetData(UINT nDay, TaskN *arrTasks, UINT nTasks_)
		{
			if (arrTasks == NULL)
				return FALSE;

			if (nTasks_ > MAX_TASKS_PER_ONE_DAY)
				return FALSE;


			size_t size = nTasks_ * sizeof(TaskN);

			if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ATTENDANCE_UPGRADE_FAC %1%", size ));
				return FALSE;
			}

			nDayN	= nDay;
			nTasks	= nTasks_;
			
			
			for (UINT i=0; i<nTasks_; i++)
			{
				Tasks[i] = arrTasks[i];
			}

			dwSize = (DWORD)(BASIC_SIZE  + size);
			
			return TRUE;
		}

		SNET_ATTENDANCE_UPGRADE_FAC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_UPGRADE_FAC);
			nType =  NET_MSG_ATTENDANCE_UPGRADE_FAC;
			nDayN = 1;
			nTasks= 1;
			nChaNum = 0;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_UPGRADE_FAC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_DONE_POINTITEM_CF : public NET_MSG_GENERIC
	{
		UINT idx;	 //Task index

		SNET_ATTENDANCE_DONE_POINTITEM_CF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_DONE_POINTITEM_CF);
			nType =  NET_MSG_ATTENDANCE_DONE_POINTITEM_CF;			
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_DONE_POINTITEM_CF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_DONE_POINTITEM_FB : public NET_MSG_GENERIC
	{
		UINT idx;	//Task index		

		SNET_ATTENDANCE_DONE_POINTITEM_FB()
		{
			dwSize = sizeof(SNET_ATTENDANCE_DONE_POINTITEM_FB);
			nType =  NET_MSG_ATTENDANCE_DONE_POINTITEM_FB;			
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_DONE_POINTITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	

	struct SNET_ATTENDANCE_TASK_UPDATE_FC : public NET_MSG_GENERIC
	{
		UINT idx;		//Task index
		UINT nValue;	//How many updated? default is 1.

		SNET_ATTENDANCE_TASK_UPDATE_FC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_TASK_UPDATE_FC);
			nType =  NET_MSG_ATTENDANCE_TASK_UPDATE_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_TASK_UPDATE_FC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_TASK_RESET_FC : public NET_MSG_GENERIC
	{
		UINT idx;	 //Task index

		SNET_ATTENDANCE_TASK_RESET_FC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_TASK_RESET_FC);
			nType =  NET_MSG_ATTENDANCE_TASK_RESET_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_TASK_RESET_FC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_TASK_COMPLETED_FC : public NET_MSG_GENERIC
	{
		UINT idx;	 //Task index	(idx == 0 -> Attendance DayN has been completed)

		SNET_ATTENDANCE_TASK_COMPLETED_FC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_TASK_COMPLETED_FC);
			nType =  NET_MSG_ATTENDANCE_TASK_COMPLETED_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_TASK_COMPLETED_FC)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Server -------------------------------------------> clients (ViewAround)
	//
	struct SNET_ATTENDANCE_POPUP_COMPLETE_FC : public SNETPC_BROAD
	{	
		SNET_ATTENDANCE_POPUP_COMPLETE_FC()
		{
			dwSize = sizeof(SNET_ATTENDANCE_POPUP_COMPLETE_FC);
			nType = NET_MSG_ATTENDANCE_POPUP_COMPLETE_FC;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_POPUP_COMPLETE_FC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_DAILYMAINTENANCE_START_AF : public NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_DAILYMAINTENANCE_START_AF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_START_AF);
			nType =  NET_MSG_ATTENDANCE_DAILYMAINTENANCE_START_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_START_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_DAILYMAINTENANCE_END_AF : public NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_DAILYMAINTENANCE_END_AF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_END_AF);
			nType =  NET_MSG_ATTENDANCE_DAILYMAINTENANCE_END_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_END_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA : public NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA()
		{
			dwSize = sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA);
			nType =  NET_MSG_ATTENDANCE_DAILYMAINTENANCE_READY_FA;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_DAILYMAINTENANCE_READY_FA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_TAKEREWARD_CF : public NET_MSG_GENERIC
	{
		UINT idxTask;

		SNET_ATTENDANCE_TAKEREWARD_CF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_TAKEREWARD_CF);
			nType =  NET_MSG_ATTENDANCE_TAKEREWARD_CF;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_TAKEREWARD_CF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ATTENDANCE_TAKEREWARD_FB : public NET_MSG_GENERIC
	{
		UINT idxTask;
		BOOL bTook;

		SNET_ATTENDANCE_TAKEREWARD_FB()
		{
			dwSize = sizeof(SNET_ATTENDANCE_TAKEREWARD_FB);
			nType =  NET_MSG_ATTENDANCE_TAKEREWARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_TAKEREWARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//
	// AgentS ------------------------------------------------> FieldS
	//
	//  Private Chatting msg has been sent to the target client successfully
	//   so that we can check the whisper attendance now.
	//
	struct SNET_ATTENDANCE_WHISPER_DONE_AF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_WHISPER_DONE_AF()
		{
			dwSize = sizeof(SNET_ATTENDANCE_WHISPER_DONE_AF);
			nType =  NET_MSG_ATTENDANCE_WHISPER_DONE_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_WHISPER_DONE_AF)<=NET_DATA_BUFSIZE);
		}
	};


	//
	// Client ------------------------------------------------> AgentS
	//
	struct SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA : public NET_MSG_GENERIC
	{
		SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA()
		{
			dwSize = sizeof(SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA);
			nType =  NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA)<=NET_DATA_BUFSIZE);
		}
	};


	//
	// AgentS ------------------------------------------------> Client
	//
	struct SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB : public NET_MSG_GENERIC
	{
		BOOL bDone;

		SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB()
		{
			dwSize = sizeof(SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB);
			nType =  NET_MSG_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB;

			bDone = FALSE;

			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_MAINTENANCE_MANUAL_START_CA_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ATTENDANCE_LOAD_END : public NET_MSG_GENERIC
	{
		DWORD	dwChaNum;

		SNET_ATTENDANCE_LOAD_END()
		{
			dwSize = sizeof(SNET_ATTENDANCE_LOAD_END);
			nType =  NET_MSG_ATTENDANCE_LOAD_END;
			MIN_STATIC_ASSERT(sizeof(SNET_ATTENDANCE_LOAD_END)<=NET_DATA_BUFSIZE);
		}
	};
	
	//
	//mjeon.CaptureTheField
	//	
	struct SNET_CTF_READY_AF : public NET_MSG_GENERIC
	{
		BOOL	bUpdate;		//CTFUpdateReady? --> FALSE: CTFReady, TRUE: CTFUpdateReady
		UINT	nRemainMinutes;

		SNET_CTF_READY_AF()
		{
			dwSize = sizeof(SNET_CTF_READY_AF);
			nType =  NET_MSG_CTF_READY_AF;

			nRemainMinutes	= 0;
			bUpdate			= FALSE;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_READY_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_READY_AC : public NET_MSG_GENERIC
	{
		BOOL	bUpdate;		//CTFUpdateReady? --> FALSE: CTFReady, TRUE: CTFUpdateReady
		UINT	nRemainMinutes;
		BOOL	bConsecutively;

		SNET_CTF_READY_AC()
		{
			dwSize = sizeof(SNET_CTF_READY_AC);
			nType =  NET_MSG_CTF_READY_AC;

			nRemainMinutes	= 0;
			bUpdate			= FALSE;
			bConsecutively  = FALSE;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_READY_AC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_WARNING4DROP_FC : public NET_MSG_GENERIC
	{
		UINT	nRemainMinutes;

		SNET_CTF_WARNING4DROP_FC()
		{
			dwSize = sizeof(SNET_CTF_WARNING4DROP_FC);
			nType =  NET_MSG_CTF_WARNING4DROP_FC;

			nRemainMinutes = 0;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_WARNING4DROP_FC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_JOIN_CF : public NET_MSG_GENERIC
	{
		BOOL bJoin;		//TRUE: Join, FALSE: Cancel Join		

		SNET_CTF_JOIN_CF()
		{
			dwSize = sizeof(SNET_CTF_JOIN_CF);
			nType =  NET_MSG_CTF_JOIN_CF;

			bJoin = TRUE;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_JOIN_CF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_JOIN_FA : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		BOOL					bJoin;				//TRUE: Join, FALSE: Cancel Join		
		WORD					wLevel;
		ENUM_CTF_JOIN_RESULT	emJoinResult;

		SNET_CTF_JOIN_FA()
			:bJoin(TRUE)			
			,wLevel(0)			
		{
			dwSize = sizeof(SNET_CTF_JOIN_FA);
			nType =  NET_MSG_CTF_JOIN_FA;			

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_JOIN_FA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_JOIN_RESULT_AC : public NET_MSG_GENERIC
	{	
		BOOL					bJoin;			//TRUE: Joined, FALSE: Canceled Join
		int						nQueued;		//0: Joined, >0: queued nQueued-th, -1: failure
		ENUM_CTF_JOIN_RESULT	emJoinResult;	//ENUM_CTF_JOIN_RESULT		

		SNET_CTF_JOIN_RESULT_AC()		
			:bJoin(TRUE)
			,nQueued(0)
			,emJoinResult(CTF_JOIN_OK)
		{
			dwSize = sizeof(SNET_CTF_JOIN_RESULT_AC);
			nType =  NET_MSG_CTF_JOIN_RESULT_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_JOIN_RESULT_AC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_UPDATE_QUEUE_AC : public NET_MSG_GENERIC
	{	
		UINT nQueued;	//0: Joined, >0: queued nQueued-th.

		SNET_CTF_UPDATE_QUEUE_AC()
		{
			dwSize = sizeof(SNET_CTF_UPDATE_QUEUE_AC);
			nType =  NET_MSG_CTF_UPDATE_QUEUE_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_UPDATE_QUEUE_AC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_PREPARE_AF : public NET_MSG_GENERIC
	{
		UINT nPlayers[SCHOOL_NUM];

		SNET_CTF_PREPARE_AF()
		{
			dwSize = sizeof(SNET_CTF_PREPARE_AF);
			nType =  NET_MSG_CTF_PREPARE_AF;

			ZeroMemory(nPlayers, sizeof(nPlayers));

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_PREPARE_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_PREPARE_AF_FB : public NET_MSG_GENERIC
	{
		UINT	nFieldSvr;	//FieldSvr number.
		BOOL	bHere;		//This FieldSvr is the ground for CTF.

		SNET_CTF_PREPARE_AF_FB()
		{
			dwSize = sizeof(SNET_CTF_PREPARE_AF_FB);
			nType =  NET_MSG_CTF_PREPARE_AF_FB;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_PREPARE_AF_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_RECALL_TO_AF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		SNATIVEID		idMap;
		DWORD			dwGateID;	//UINT_MAX: Use PosX and PosY		

		SNET_CTF_RECALL_TO_AF()
			:idMap(false)
			,dwGateID(0)
		{
			dwSize = sizeof(SNET_CTF_RECALL_TO_AF);
			nType =  NET_MSG_CTF_RECALL_TO_AF;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_RECALL_TO_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_START_AF : public NET_MSG_GENERIC
	{
		SNET_CTF_START_AF()			
		{
			dwSize = sizeof(SNET_CTF_START_AF);
			nType =  NET_MSG_CTF_START_AF;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_START_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_START_2ALL_AC : public NET_MSG_GENERIC
	{
		__time32_t	tStartTime;	//resync the start-time again.

		SNET_CTF_START_2ALL_AC()			
		{
			dwSize = sizeof(SNET_CTF_START_2ALL_AC);
			nType =  NET_MSG_CTF_START_2ALL_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_START_2ALL_AC)<=NET_DATA_BUFSIZE);
		}
	};

	
	struct SNET_CTF_START_2PLAYERS_FC : public NET_MSG_GENERIC
	{
		D3DXVECTOR3 dxPos1;	//각 인증기의 좌표 (미니맵에 나타낼 인증기의 상태 갱신용)
		D3DXVECTOR3 dxPos2;
		D3DXVECTOR3 dxPos3;

		WORD		wSchoolAuthedCert1;
		WORD		wSchoolAuthedCert2;
		WORD		wSchoolAuthedCert3;		

		SNET_CTF_START_2PLAYERS_FC()
			:dxPos1(0,0,0)
			,dxPos2(0,0,0)
			,dxPos3(0,0,0)
			,wSchoolAuthedCert1(SCHOOL_NONE)
			,wSchoolAuthedCert2(SCHOOL_NONE)
			,wSchoolAuthedCert3(SCHOOL_NONE)
		{
			dwSize = sizeof(SNET_CTF_START_2PLAYERS_FC);
			nType =  NET_MSG_CTF_START_2PLAYERS_FC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_START_2PLAYERS_FC)<=NET_DATA_BUFSIZE);
		}
	};

/*
	struct SNET_CTF_SYNC_REQ_CA : public NET_MSG_GENERIC
	{
		SNET_CTF_SYNC_REQ_CA()			
		{
			dwSize = sizeof(SNET_CTF_SYNC_REQ_CA);
			nType =  NET_MSG_CTF_SYNC_REQ_CA;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_SYNC_REQ_CA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_SYNC_REQ_CA_FB : public NET_MSG_GENERIC
	{
		UINT			state;
		UINT			nRemainMinutes;

		SNET_CTF_SYNC_REQ_CA_FB()
		{
			dwSize = sizeof(SNET_CTF_SYNC_REQ_CA_FB);
			nType =  NET_MSG_CTF_SYNC_REQ_CA_FB;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_SYNC_REQ_CA_FB)<=NET_DATA_BUFSIZE);
		}
	};
*/

	struct SNET_CTF_NEXT_READY_TIME : public NET_MSG_GENERIC
	{
		CTime ctReadyTime;

		SNET_CTF_NEXT_READY_TIME()
		{
			dwSize = sizeof( SNET_CTF_NEXT_READY_TIME );
			nType =  NET_MSG_CTF_NEXT_READY_TIME;

			MIN_STATIC_ASSERT( sizeof( SNET_CTF_NEXT_READY_TIME ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_CTF_STOP_AF : public NET_MSG_GENERIC
	{
		SNET_CTF_STOP_AF()
		{
			dwSize = sizeof(SNET_CTF_STOP_AF);
			nType =  NET_MSG_CTF_STOP_AF;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_STOP_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_STOP_AC : public NET_MSG_GENERIC
	{
		WORD		wSchoolAuthed;
		__time32_t	tNextStartTime;
		BOOL		bConsecutively;

		SNET_CTF_STOP_AC()
			:wSchoolAuthed(SCHOOL_NONE)
			,tNextStartTime(0) , bConsecutively(FALSE)
		{
			dwSize = sizeof(SNET_CTF_STOP_AC);
			nType =  NET_MSG_CTF_STOP_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_STOP_AC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_END_FA : public NET_MSG_GENERIC
	{
		WORD wSchoolAuthed;
		BOOL bConsecutively;


		SNET_CTF_END_FA()
			:wSchoolAuthed(SCHOOL_NONE) ,bConsecutively(FALSE)
		{
			dwSize = sizeof(SNET_CTF_END_FA);
			nType =  NET_MSG_CTF_END_FA;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_END_FA)<=NET_DATA_BUFSIZE);
		}
	};


	//
	// CTF time up
	//
	struct SNET_CTF_DONE_AF_AC : public NET_MSG_GENERIC
	{
		WORD wSchoolAuthed;
		BOOL bConsecutively;


		SNET_CTF_DONE_AF_AC()		
			:wSchoolAuthed(SCHOOL_NONE)	,bConsecutively(FALSE)
		{
			dwSize = sizeof(SNET_CTF_DONE_AF_AC);
			nType =  NET_MSG_CTF_DONE_AF_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_DONE_AF_AC)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_DONE_AF_FB : public NET_MSG_GENERIC
	{
		WORD wSchoolAuthed;
		BOOL bConsecutively;


		SNET_CTF_DONE_AF_FB()
			:wSchoolAuthed(SCHOOL_NONE),bConsecutively(FALSE)
		{
			dwSize = sizeof(SNET_CTF_DONE_AF_FB);
			nType =  NET_MSG_CTF_DONE_AF_FB;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_DONE_AF_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_REWARD_FC : public NET_MSG_GENERIC
	{
		SNET_CTF_REWARD_FC()			
		{
			dwSize = sizeof(SNET_CTF_REWARD_FC);
			nType =  NET_MSG_CTF_REWARD_FC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_REWARD_FC)<=NET_DATA_BUFSIZE);
		}
	};



	//
	//used in several cases need to get reward info: AF, AH, HF, AC(Sync)
	//
    struct SNET_CTF_REWARD_INFO : public NET_MSG_GENERIC
    {
        enum
        {
            BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(UINT),
            BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
        };

        WORD wSchoolAuthed;
		WORD wReserved;		//padding 2-bytes for explicit calculation of msg size such like "dwSize = (DWORD)(BASIC_SIZE  + size);" rather than sizeof(STRUCTNAME).
        UINT nHowMany;
        CTF_REWARD_BUFF	arrBuffs[CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG];


        BOOL SetData(UINT HowMany, CTF_REWARD_BUFF *_arrBuffs)
        {
			dwSize = BASIC_SIZE;

            if (_arrBuffs == NULL)
                return FALSE;

            if (HowMany > CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG)
                return FALSE;

            size_t size = HowMany * sizeof(CTF_REWARD_BUFF);

            if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_REWARD_INFO %1%", size ));
                return FALSE;
			}

            nHowMany = HowMany;
            
            for (UINT i=0; i<nHowMany; i++)
            {
                arrBuffs[i] = _arrBuffs[i];
            }

            dwSize = (DWORD)(BASIC_SIZE  + size);

            return TRUE;
        }


		BOOL SetData(std::vector<CTF_REWARD_BUFF>& vecRewardBuff)
		{
			dwSize = BASIC_SIZE;

			if (vecRewardBuff.empty())
				return FALSE;

			UINT HowMany = (UINT)vecRewardBuff.size();

            if (HowMany > CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG)
				return FALSE;			

            size_t size = HowMany * sizeof(CTF_REWARD_BUFF);

            if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_REWARD_INFO %1% (VECTOR)", size ));
               return FALSE;
			}

            nHowMany = HowMany;
            
			std::vector<CTF_REWARD_BUFF>::iterator iter = vecRewardBuff.begin();

			for (UINT i=0; iter != vecRewardBuff.end(); ++iter, ++i)
			{
				arrBuffs[i] = (*iter);
			}

            dwSize = (DWORD)(BASIC_SIZE  + size);

            return TRUE;
		}


        SNET_CTF_REWARD_INFO()
            :wSchoolAuthed(SCHOOL_NONE)
            ,nHowMany(0)
			,wReserved(0)
        {
            dwSize = sizeof(SNET_CTF_REWARD_INFO);
            nType =  NET_MSG_CTF_REWARD_INFO;

            ZeroMemory(arrBuffs, sizeof(arrBuffs));

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_REWARD_INFO)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_CTF_QUIT_CFA : public NET_MSG_CHARACTER
	{
		SNET_CTF_QUIT_CFA()
		{
			dwSize = sizeof(SNET_CTF_QUIT_CFA);
			nType =  NET_MSG_CTF_QUIT_CFA;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_QUIT_CFA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_SYNC_PLAYERNUM_AF : public NET_MSG_GENERIC
	{
		UINT arrPlayerNum[SCHOOL_NUM];

		SNET_CTF_SYNC_PLAYERNUM_AF()
		{
			dwSize = sizeof(SNET_CTF_SYNC_PLAYERNUM_AF);
			nType =  NET_MSG_CTF_SYNC_PLAYERNUM_AF;

			ZeroMemory(arrPlayerNum, sizeof(arrPlayerNum));

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_SYNC_PLAYERNUM_AF)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_CROW_AUTHED : public SNETCROW_BROAD
	{		
        SNATIVEID	sNativeID;
		DWORD		arrHP4School[SCHOOL_NUM];
		WORD		wSchoolAuthed;
		D3DXVECTOR3 dxPos;

		SNET_CTF_CROW_AUTHED()
			:sNativeID(SNATIVEID(false))
            ,wSchoolAuthed(SCHOOL_NONE)
			,dxPos(0,0,0)
		{
			dwSize = sizeof(SNET_CTF_CROW_AUTHED);
			nType = NET_MSG_CTF_CROW_AUTHED;

			ZeroMemory(arrHP4School, sizeof(arrHP4School));

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_CROW_AUTHED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_SYNC_STATUS_AC : public NET_MSG_GENERIC
	{
		DWORD		state;		//CTF 상태
		__time32_t	tStartTime;
		WORD		wRequiredLevel;
		WORD		wSchoolAuthed;
		WORD		wReserved;
		WORD		wPrevSchoolAuthed;	//History: 이전 CTF에 대한 기록 (통합UI 출력용)
		__time32_t	tPrevStartTime;		//History: 이전 CTF에 대한 기록 (통합UI 출력용)
		__time32_t	tPrevEndTime;		//History: 이전 CTF에 대한 기록 (통합UI 출력용)
		
        UINT        nPlayTimeM;
		UINT		nRewardTimeM;
		UINT		nPlayCycleH;
		DWORD		dwPvPMapID;
		DWORD		dwPvEMapID;
		DWORD		dwCertificationID1;
		DWORD		dwCertificationID2;
		DWORD		dwCertificationID3;
		DWORD		dwMaxPlayerNumPerSchool;
		BOOL		bisConsecutively;
		BOOL		bPrevConsecutively;
		BOOL		bConsecutivelyCapturel;
		DWORD		dwConsecutivelyNumber;



		SNET_CTF_SYNC_STATUS_AC()
			:state(0)
			,wRequiredLevel(198)
			,wSchoolAuthed(SCHOOL_NONE)			
			,wReserved(0)
			,wPrevSchoolAuthed(SCHOOL_NONE)
			,tStartTime(0)
			,tPrevStartTime(0)
			,tPrevEndTime(0)
            ,nPlayTimeM(0)
			,nRewardTimeM(0)
			,dwPvPMapID(UINT_MAX)
			,dwPvEMapID(UINT_MAX)
			,dwCertificationID1(UINT_MAX)
			,dwCertificationID2(UINT_MAX)
			,dwCertificationID3(UINT_MAX)
			,dwMaxPlayerNumPerSchool(50)
			,bisConsecutively(FALSE)
			,bPrevConsecutively(FALSE)
			,dwConsecutivelyNumber(0)
			,bConsecutivelyCapturel(TRUE)
			
			
		{
			dwSize = sizeof(SNET_CTF_SYNC_STATUS_AC);
			nType =  NET_MSG_CTF_SYNC_STATUS_AC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_SYNC_STATUS_AC)<=NET_DATA_BUFSIZE);
		}		
	};


	struct SNET_CTF_LOAD_REWARD_INFO_AH : public NET_MSG_GENERIC
	{
		SNET_CTF_LOAD_REWARD_INFO_AH()
		{
			dwSize = sizeof(SNET_CTF_LOAD_REWARD_INFO_AH);
			nType = NET_MSG_CTF_LOAD_REWARD_INFO_AH;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_LOAD_REWARD_INFO_AH)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_CTF_STAT_LOG_AH : public NET_MSG_GENERIC
	{
		UINT	state;						//CTF state: Start or Stop 2 시점에 각각 로그를 남기도록 한다.
		UINT	nPlayers[SCHOOL_NUM];

		SNET_CTF_STAT_LOG_AH()
			:state(0)
		{
			dwSize = sizeof(SNET_CTF_STAT_LOG_AH);
			nType =  NET_MSG_CTF_STAT_LOG_AH;

			ZeroMemory(nPlayers, sizeof(nPlayers));

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_STAT_LOG_AH)<=NET_DATA_BUFSIZE);
		}
	};
	
	
    struct SNET_CTF_QUIT_AF : public NET_MSG_GENERIC
    {
        DWORD dwChaNum;
		UINT  state;		//CTF_STATE_START ~ CTF_STATE_REWARD

        SNET_CTF_QUIT_AF()
            :dwChaNum(UINT_MAX)
			,state(2)		//CTF_STATE_START
        {
            dwSize = sizeof(SNET_CTF_QUIT_AF);
            nType =  NET_MSG_CTF_QUIT_AF;

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_QUIT_AF)<=NET_DATA_BUFSIZE);
        }
    };

/*
	struct SNET_CTF_NEW_PLAYER_HAS_COME_AF : public NET_MSG_GENERIC
    {
        DWORD dwChaNum;

        SNET_CTF_NEW_PLAYER_HAS_COME_AF()
            :dwChaNum(UINT_MAX)
        {
            dwSize = sizeof(SNET_CTF_NEW_PLAYER_HAS_COME_AF);
            nType =  NET_MSG_CTF_NEW_PLAYER_HAS_COME_AF;

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_NEW_PLAYER_HAS_COME_AF)<=NET_DATA_BUFSIZE);
        }
    };
*/

	struct SNET_CTF_UPDATE_PLAYERS_VECTOR_FC : public NET_MSG_GENERIC
    {
        enum
        {
            BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(UINT) + sizeof(UINT) + sizeof(D3DXVECTOR3),
            BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE,
			POS_USEABLE_SIZE = BUF_SIZE/sizeof(D3DXVECTOR3)
        };
        
		//  [10/2/2015 gbgim];
		// 패킷 사이즈가 1024를 넘어갈수도있기때문에 분할해서 전송한다;
		UINT		nRelativeUserIndex;			// 상대적 유저 인덱스;
        UINT		nHowMany;
		D3DXVECTOR3 dvMyPos;					// position of the receiver(a character) of this msg
        D3DXVECTOR3	arrPos[POS_USEABLE_SIZE];	// one msg for each school
												// CTF_PLAYER_NUM_LIMIT_PER_SCHOOL

        BOOL SetData(UINT HowMany, D3DXVECTOR3 *_arrPos)
        {
            if (_arrPos == NULL)
                return FALSE;

            if (HowMany > POS_USEABLE_SIZE)
                return FALSE;

            size_t size = HowMany * sizeof(D3DXVECTOR3);

            if (size > (BUF_SIZE - 1))
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_UPDATE_PLAYERS_VECTOR_FC %1%", size ));
                return FALSE;
			}

            nHowMany = HowMany;
            
            for (UINT i=0; i<nHowMany; i++)
            {
                arrPos[i] = _arrPos[i];
            }

            dwSize = (DWORD)(BASIC_SIZE  + size);

            return TRUE;
        }


		BOOL AddData(D3DXVECTOR3 _Pos)
        {
			if (nHowMany >= POS_USEABLE_SIZE)
				return FALSE;

			arrPos[nHowMany++] = _Pos;
			BOOL bPushAble(TRUE);
            if (nHowMany >= POS_USEABLE_SIZE)
                bPushAble = FALSE;

            size_t size = nHowMany * sizeof(D3DXVECTOR3);

			if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_UPDATE_PLAYERS_VECTOR_FC %1% (ADD)", size ));
				return FALSE;            
			}

            dwSize = (DWORD)(BASIC_SIZE + size);

            return bPushAble;
        }


        SNET_CTF_UPDATE_PLAYERS_VECTOR_FC()
            : nRelativeUserIndex(0)
			, nHowMany(0)
        {
            dwSize = sizeof(SNET_CTF_UPDATE_PLAYERS_VECTOR_FC);
            nType =  NET_MSG_CTF_UPDATE_PLAYERS_VECTOR_FC;

            ZeroMemory(arrPos, sizeof(arrPos));

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_UPDATE_PLAYERS_VECTOR_FC)<=NET_DATA_BUFSIZE);
        }
    };

/*
	struct SNET_CTF_UPDATE_CONTRIBUTION_POINT_FC : public NET_MSG_GENERIC
    {        
		int		nContributionPoint;

        SNET_CTF_UPDATE_CONTRIBUTION_POINT_FC()            
			:nContributionPoint(0)
        {
            dwSize = sizeof(SNET_CTF_UPDATE_CONTRIBUTION_POINT_FC);
            nType =  NET_MSG_CTF_UPDATE_CONTRIBUTION_POINT_FC;

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_UPDATE_CONTRIBUTION_POINT_FC)<=NET_DATA_BUFSIZE);
        }
    };
*/


	struct SNET_CTF_STATISTICS_FC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD),
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE//- sizeof(NET_MSG_GENERIC) - sizeof(WORD)*2 - sizeof(DWORD)*2
		};

		WORD	wSeq;		//current part-number of a large packet
		WORD	wLastSeq;	//total part-number of a large packet
							//i.e.>	wSeq = 1, wLastSeq = 4 means that there are 3 more packets remain
							//		and they must be gathered into 1 data to make it work correctly.
		
		DWORD	nHowMany;	//How many activities are included in this msg.

		CTF_STATISTICS_4_ONEPLAYER	data[HOW_MANY_CTFSTATISTICS_PER_ONE_MSG];


		SNET_CTF_STATISTICS_FC()
			:nHowMany(0)
			,wSeq(0)
			,wLastSeq(0)
		{
			dwSize = sizeof(SNET_CTF_STATISTICS_FC);
			nType = NET_MSG_CTF_STATISTICS_FC;

			MIN_STATIC_ASSERT(sizeof(SNET_CTF_STATISTICS_FC)<=NET_DATA_BUFSIZE);
		}
			
		BOOL SetData(std::vector<CTF_STATISTICS_4_ONEPLAYER>::iterator &begin, std::vector<CTF_STATISTICS_4_ONEPLAYER>::iterator &end, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nHowMany	= (DWORD)(end - begin);

			size_t size = nHowMany * sizeof(CTF_STATISTICS_4_ONEPLAYER);

			if (size > BUF_SIZE )
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_STATISTICS_FC %1%", size ));
				return FALSE;
			}
			
			std::vector<CTF_STATISTICS_4_ONEPLAYER>::iterator iter;
			UINT i =0;

			for (iter = begin; iter != end; ++iter, ++i)
			{
				data[i] = *iter;
			}

			dwSize = (DWORD)(BASIC_SIZE + nHowMany * sizeof(CTF_STATISTICS_4_ONEPLAYER));
			
			return TRUE;
		}
		
		BOOL SetData(std::vector<CTF_STATISTICS_4_ONEPLAYER> &vecSync, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nHowMany	= (DWORD)vecSync.size();

			size_t size = nHowMany * sizeof(CTF_STATISTICS_4_ONEPLAYER);

			if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_STATISTICS_FC %1%", size ));
				return FALSE;
			}
			
			for (UINT i = 0; i < nHowMany; i++)
			{
				data[i] = vecSync[i];
			}

			dwSize = (DWORD)(BASIC_SIZE + nHowMany * sizeof(CTF_STATISTICS_4_ONEPLAYER));
			
			return TRUE;
		}
		

		CTF_STATISTICS_4_ONEPLAYER* GetData()
		{
			return data;
		}
	};
	

	struct SNET_GET_CHA_EXTRA_INFO_FH : public NET_MSG_GENERIC
    {
        DWORD dwChaNum;

        SNET_GET_CHA_EXTRA_INFO_FH()
            :dwChaNum(UINT_MAX)
        {
            dwSize = sizeof(SNET_GET_CHA_EXTRA_INFO_FH);
            nType =  NET_MSG_GET_CHA_EXTRA_INFO_FH;

            MIN_STATIC_ASSERT(sizeof(SNET_GET_CHA_EXTRA_INFO_FH)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_GET_CHA_EXTRA_INFO_DH : public NET_MSG_GENERIC
    {
        DWORD	dwChaNum;
		int		nContributionPoint;

        SNET_GET_CHA_EXTRA_INFO_DH()
            :dwChaNum(UINT_MAX)
			,nContributionPoint(0)
        {
            dwSize = sizeof(SNET_GET_CHA_EXTRA_INFO_DH);
            nType =  NET_MSG_GET_CHA_EXTRA_INFO_DH;

            MIN_STATIC_ASSERT(sizeof(SNET_GET_CHA_EXTRA_INFO_DH)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_GET_CHA_EXTRA_INFO_FH_FB : public NET_MSG_GENERIC
    {
        DWORD	dwChaNum;
		int		nContributionPoint;

        SNET_GET_CHA_EXTRA_INFO_FH_FB()
            :dwChaNum(UINT_MAX)
			,nContributionPoint(0)
        {
            dwSize = sizeof(SNET_GET_CHA_EXTRA_INFO_FH_FB);
            nType =  NET_MSG_GET_CHA_EXTRA_INFO_FH_FB;

            MIN_STATIC_ASSERT(sizeof(SNET_GET_CHA_EXTRA_INFO_FH_FB)<=NET_DATA_BUFSIZE);
        }
    };

	
	struct SNET_SET_CHA_EXTRA_INFO_FH : public NET_MSG_GENERIC
    {
        DWORD	dwChaNum;
		int		nContributionPoint;

        SNET_SET_CHA_EXTRA_INFO_FH()
            :dwChaNum(UINT_MAX)
			,nContributionPoint(0)
        {
            dwSize = sizeof(SNET_SET_CHA_EXTRA_INFO_FH);
            nType =  NET_MSG_SET_CHA_EXTRA_INFO_FH;

            MIN_STATIC_ASSERT(sizeof(SNET_SET_CHA_EXTRA_INFO_FH)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_CTF_MOVESTATE_BY_GM_CA : public NET_MSG_GENERIC
    {
        SNET_CTF_MOVESTATE_BY_GM_CA()			
        {
            dwSize = sizeof(SNET_CTF_MOVESTATE_BY_GM_CA);
            nType =  NET_MSG_CTF_MOVESTATE_BY_GM_CA;

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_MOVESTATE_BY_GM_CA)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_CTF_MOVESTATE_BY_GM_CA_FB : public NET_MSG_GENERIC
    {
		UINT	stateBefore;
		UINT	stateAfter;

        SNET_CTF_MOVESTATE_BY_GM_CA_FB()
			:stateBefore(0)
			,stateAfter(0)
        {
            dwSize = sizeof(SNET_CTF_MOVESTATE_BY_GM_CA_FB);
            nType =  NET_MSG_CTF_MOVESTATE_BY_GM_CA_FB;

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_MOVESTATE_BY_GM_CA_FB)<=NET_DATA_BUFSIZE);
        }
    };


	struct SNET_CTF_RANKING_FAC : public NET_MSG_GENERIC
	{
		 enum
        {
            BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(UINT),
            BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
        };

		UINT			nHowMany;
        
		PVP::CTF_RANKER_BASIC	arrRanking[CTF_RANKERS_NUM_4_NOTIFY];	//index of the array is the ranking. [0] = 1st ranker, [1] = 2nd ranker


		BOOL SetData(std::set<PVP::CTF_RANKER> &setRanking)
        {
			dwSize = BASIC_SIZE;

			if (setRanking.empty())
				return FALSE;

			UINT HowMany = (UINT)setRanking.size();

            HowMany = min(CTF_RANKERS_NUM_4_NOTIFY, HowMany);                

            size_t size = HowMany * sizeof(PVP::CTF_RANKER_BASIC);

            if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_RANKING_FAC %1% (SET)", size ));
                return FALSE;
			}

            nHowMany = HowMany;
            
			std::set<PVP::CTF_RANKER>::iterator iter = setRanking.begin();			

			for (UINT i=0; (i < HowMany) && (iter != setRanking.end()); ++iter, ++i)
			{
				arrRanking[i] = static_cast<PVP::CTF_RANKER_BASIC>(*iter);
			}

            dwSize = (DWORD)(BASIC_SIZE + size);

            return TRUE;
        }


		BOOL SetData(std::vector<PVP::CTF_RANKER_BASIC> &vecRanking)
        {
			dwSize = BASIC_SIZE;

			if (vecRanking.empty())
				return FALSE;

			UINT HowMany = (UINT)vecRanking.size();

            HowMany = min(CTF_RANKERS_NUM_4_NOTIFY, HowMany);                

            size_t size = HowMany * sizeof(PVP::CTF_RANKER_BASIC);

            if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_CTF_RANKING_FAC %1% (VECTOR)", size ));
                return FALSE;
			}

            nHowMany = HowMany;
            
			std::vector<PVP::CTF_RANKER_BASIC>::iterator iter = vecRanking.begin();			

			for (UINT i=0; (i < HowMany) && (iter != vecRanking.end()); ++iter, ++i)
			{
				arrRanking[i] = (*iter);
			}

            dwSize = (DWORD)(BASIC_SIZE + size);

            return TRUE;
        }


        SNET_CTF_RANKING_FAC()            
			:nHowMany(0)
        {
            dwSize = sizeof(SNET_CTF_RANKING_FAC);
            nType =  NET_MSG_CTF_RANKING_FAC;

            ZeroMemory(arrRanking, sizeof(arrRanking));

            MIN_STATIC_ASSERT(sizeof(SNET_CTF_RANKING_FAC)<=NET_DATA_BUFSIZE);
        }
	};


	
	//
	//mjeon.activity	
	//
	// ------------------- Activity -------------------
	//
	struct SNET_ACTIVITY_COMPLETED_FA : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		DWORD	dwNotifyLevel;
		DWORD	dwChaNumCloser;

		SNET_ACTIVITY_COMPLETED_FA()
		{
			dwSize = sizeof(SNET_ACTIVITY_COMPLETED_FA);
			nType =  NET_MSG_ACTIVITY_COMPLETED_FA;	
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_COMPLETED_FA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ACTIVITY_CLOSED_FA : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		//DWORD	dwNotifyLevel; //Closed-notify MUST be broadcast to sync the activity.
		DWORD	dwChaNumCloser;

		SNET_ACTIVITY_CLOSED_FA()
		{
			dwSize = sizeof(SNET_ACTIVITY_CLOSED_FA);
			nType = NET_MSG_ACTIVITY_CLOSED_FA;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_CLOSED_FA)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ACTIVITY_COMPLETED_FF : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD		dwActivityID;
		DWORD		dwClass;
		DWORD		dwRewardPoint;
		char		szRewardTitleID[EM_TITLEID_MAXLENGTH];
		//std::string	strRewardTitle;
		

		SNET_ACTIVITY_COMPLETED_FF()
		{
			dwSize = sizeof(SNET_ACTIVITY_COMPLETED_FF);
			nType =  NET_MSG_ACTIVITY_COMPLETED_FF;

			ZeroMemory(szRewardTitleID, sizeof(szRewardTitleID));

			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_COMPLETED_FF)<=NET_DATA_BUFSIZE);
		}
	};

	
	//
	//Server -------------------------------------------> several Clients
	//
    struct SNET_ACTIVITY_NOTIFY_COMPLETED : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		char	szChaName[CHR_ID_LENGTH+1];

		SNET_ACTIVITY_NOTIFY_COMPLETED()
		{
			dwSize = sizeof(SNET_ACTIVITY_NOTIFY_COMPLETED);
			nType = NET_MSG_ACTIVITY_NOTIFY_COMPLETED;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_NOTIFY_COMPLETED)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_ACTIVITY_NOTIFY_CLOSED : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		char	szChaName[CHR_ID_LENGTH+1];

		SNET_ACTIVITY_NOTIFY_CLOSED()
		{
			dwSize = sizeof(SNET_ACTIVITY_NOTIFY_CLOSED);
			nType = NET_MSG_ACTIVITY_NOTIFY_CLOSED;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_NOTIFY_CLOSED)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//Server -------------------------------------------> to the client updated the activity.
	//
	struct SNET_ACTIVITY_UPDATE_VALUE : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		UINT	nValue;		

		SNET_ACTIVITY_UPDATE_VALUE()
		{
			dwSize = sizeof(SNET_ACTIVITY_UPDATE_VALUE);
			nType = NET_MSG_ACTIVITY_UPDATE_VALUE;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_UPDATE_VALUE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_ACTIVITY_UPDATE_COMPLETED : public NET_MSG_GENERIC
	{
		enum { MAXSIZE = 30, };
		DWORD	arrdwActivityID[MAXSIZE];
		DWORD	dwArrSize;

		SNET_ACTIVITY_UPDATE_COMPLETED()
		{
			dwSize = sizeof(SNET_ACTIVITY_UPDATE_COMPLETED);
			nType = NET_MSG_ACTIVITY_UPDATE_COMPLETED;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_UPDATE_COMPLETED)<=NET_DATA_BUFSIZE);
		}
	};

/*
	struct SNET_ACTIVITY_UPDATE_CLOSED : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;		

		SNET_ACTIVITY_UPDATE_CLOSED()
		{
			dwSize = sizeof(SNET_ACTIVITY_UPDATE_CLOSED);
			nType = NET_MSG_ACTIVITY_UPDATE_CLOSED;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_UPDATE_CLOSED)<=NET_DATA_BUFSIZE);
		}
	};
*/

	//
	//Server -------------------------------------------> clients (ViewAround)
	//
	struct SNET_ACTIVITY_POPUP_COMPLETE : public SNETPC_BROAD
	{
		DWORD	dwActivityID;		

		SNET_ACTIVITY_POPUP_COMPLETE()
		{
			dwSize = sizeof(SNET_ACTIVITY_POPUP_COMPLETE);
			nType = NET_MSG_ACTIVITY_POPUP_COMPLETE;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_POPUP_COMPLETE)<=NET_DATA_BUFSIZE);
		}
	};


	//
	//Client -------------------------------------------> FieldS
	//
	struct SNET_ACTIVITY_DONE_ITEMTAKE : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		DWORD	dwItemID;

		SNET_ACTIVITY_DONE_ITEMTAKE()
		{
			dwSize = sizeof(SNET_ACTIVITY_DONE_ITEMTAKE);
			nType = NET_MSG_ACTIVITY_DONE_ITEMTAKE;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_DONE_ITEMTAKE)<=NET_DATA_BUFSIZE);
		}
	};


	//
	//FieldS -------------------------------------------> Client
	//
	struct SNET_ACTIVITY_DONE_ITEMTAKE_FB : public NET_MSG_GENERIC
	{
		DWORD	dwActivityID;
		DWORD	dwItemID;
		BOOL	bUpdated;	//요청한 특별활동이 서버에서도 검증되었다면, 해당 특별활동은 update되었을 것이다.
		

		SNET_ACTIVITY_DONE_ITEMTAKE_FB()
		{
			dwSize = sizeof(SNET_ACTIVITY_DONE_ITEMTAKE_FB);
			nType = NET_MSG_ACTIVITY_DONE_ITEMTAKE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_DONE_ITEMTAKE_FB)<=NET_DATA_BUFSIZE);
		}
	};

/*
	//
	//Client -------------------------------------------> FieldS
	//
	struct SNET_ACTIVITY_SYNC_REQUEST : public NET_MSG_GENERIC
	{
		SNET_ACTIVITY_SYNC_REQUEST()
		{
			dwSize = sizeof(SNET_ACTIVITY_SYNC_REQUEST);
			nType = NET_MSG_ACTIVITY_SYNC_REQUEST;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_SYNC_REQUEST)<=NET_DATA_BUFSIZE);
		}
	};
*/

	//
	//FieldS -------------------------------------------> Client
	//

	struct SNET_ACTIVITY_SYNC : public NET_MSG_GENERIC
	{
		enum
		{
            BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE//- sizeof(NET_MSG_GENERIC) - sizeof(WORD)*2 - sizeof(DWORD)*2
		};

		WORD	wSeq;		//current part-number of a large packet
		WORD	wLastSeq;	//total part-number of a large packet
							//i.e.>	wSeq = 1, wLastSeq = 4 means that there are 3 more packets remain
							//		and they must be gathered into 1 data to make it work correctly.
		
		DWORD	nSync;		//How many activities are included in this msg.

		DWORD	nChaNum;	//SYNC msg MUST be relayed via Agent (from Field) therefore ChaNum is required.

		ACTIVITY_SYNC		dataSync[HOW_MANY_ACTVITITIES_PER_ONE_SYNC];


		SNET_ACTIVITY_SYNC()
		{
			dwSize = sizeof(SNET_ACTIVITY_SYNC);
			nType = NET_MSG_ACTIVITY_SYNC;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_SYNC)<=NET_DATA_BUFSIZE);
		}

		BOOL SetData(std::vector<ACTIVITY_SYNC>::iterator &begin, std::vector<ACTIVITY_SYNC>::iterator &end, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nSync	= (DWORD)(end - begin);

			size_t size = nSync * sizeof(ACTIVITY_SYNC);

			if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ACTIVITY_SYNC %1%", size ));
				return FALSE;
			}
			
			std::vector<ACTIVITY_SYNC>::iterator iter;
			UINT i =0;

			for (iter = begin; iter != end; ++iter, ++i)
			{
				dataSync[i] = *iter;
#ifdef _TEMP_TRACING_LOG
				sc::writeLogDebug(sc::string::format("[특별활동] idx : %1%, dwID : %2%, dwStatus : %3%, dwValue : %4%, Seq : %5%, LastSeq : %6%", i, dataSync[i].dwID, dataSync[i].dwStatus, dataSync[i].dwValue, Seq, LastSeq));
#endif
			}

			dwSize = (DWORD)(BASIC_SIZE/*sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2*/ + nSync * sizeof(ACTIVITY_SYNC));
			
			return TRUE;
		}
		
		BOOL SetData(std::vector<ACTIVITY_SYNC> &vecSync, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nSync	= (DWORD)vecSync.size();

			size_t size = nSync * sizeof(ACTIVITY_SYNC);

			if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ACTIVITY_SYNC %1%", size ));
				return FALSE;
			}
			
			for (UINT i = 0; i < nSync; i++)
			{
				dataSync[i] = vecSync[i];
#ifdef _TEMP_TRACING_LOG
				sc::writeLogDebug(sc::string::format("[특별활동] idx : %1%, dwID : %2%, dwStatus : %3%, dwValue : %4%, Seq : %5%, LastSeq : %6%", i, dataSync[i].dwID, dataSync[i].dwStatus, dataSync[i].dwValue, Seq, LastSeq));
#endif
			}

			dwSize = (DWORD)(BASIC_SIZE/*sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2*/ + nSync * sizeof(ACTIVITY_SYNC));
			
			return TRUE;
		}

		ACTIVITY_SYNC* GetData()
		{
			return dataSync;
		}
	};

	struct SNET_ACTIVITY_REWARD_POINT_SYNC : public NET_MSG_GENERIC
	{
		LONGLONG llRewardPointHigh;
		LONGLONG llRewardPointMedium;
		LONGLONG llRewardPointLow;

		DWORD	nChaNum;	//SYNC msg MUST be relayed via Agent (from Field) therefore ChaNum is required.

		SNET_ACTIVITY_REWARD_POINT_SYNC()
		{
			dwSize = sizeof(SNET_ACTIVITY_REWARD_POINT_SYNC);
			nType = NET_MSG_ACTIVITY_REWARD_POINT_SYNC;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_REWARD_POINT_SYNC)<=NET_DATA_BUFSIZE);
		}
	};

	
	//
	//FieldS -------------------------------------------> Client
	//
	struct SNET_ACTIVITY_REWARD_BADGE_SYNC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE//- sizeof(NET_MSG_GENERIC) - sizeof(WORD)*2 - sizeof(DWORD)*2
		};

		WORD	wSeq;		//current part-number of a large packet
		WORD	wLastSeq;	//total part-number of a large packet
							//i.e.>	wSeq = 1, wLastSeq = 4 means that there are 3 more packets remain
							//		and they must be gathered into 1 data to make it work correctly.
		DWORD	nSync;		//How many activities are included in this msg.

		DWORD	nChaNum;	//SYNC msg MUST be relayed via Agent (from Field) therefore ChaNum is required.

		TITLE_SYNC			dataSync[HOW_MANY_TITLES_PER_ONE_SYNC];


		SNET_ACTIVITY_REWARD_BADGE_SYNC()
		{
			dwSize = sizeof(SNET_ACTIVITY_REWARD_BADGE_SYNC);
			nType = NET_MSG_ACTIVITY_REWARD_BADGE_SYNC;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_REWARD_BADGE_SYNC)<=NET_DATA_BUFSIZE);
		}

		BOOL SetData(std::vector<TITLE_SYNC>::iterator &begin, std::vector<TITLE_SYNC>::iterator &end, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nSync	= (DWORD)(end - begin);

			size_t size = nSync * sizeof(TITLE_SYNC);

			if (size > (BUF_SIZE - 1) )
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ACTIVITY_REWARD_BADGE_SYNC %1%", size ));
				return FALSE;
			}
			
			std::vector<TITLE_SYNC>::iterator iter;
			UINT i =0;

			for (iter = begin; iter != end; ++iter, ++i)
			{
				dataSync[i] = *iter;
			}

			dwSize = (DWORD)(BASIC_SIZE/*sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2*/ + nSync * sizeof(TITLE_SYNC));
			
			return TRUE;
		}
		
		BOOL SetData(std::vector<TITLE_SYNC> &vecSync, WORD Seq, WORD LastSeq)
		{
			if(Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			nSync	= (DWORD)vecSync.size();

			size_t size = nSync * sizeof(TITLE_SYNC);

			if (size > (BUF_SIZE - 1) )
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_ACTIVITY_REWARD_BADGE_SYNC %1%", size ));
				return FALSE;
			}
			
			for (UINT i = 0; i < nSync; i++)
			{
				dataSync[i] = vecSync[i];
			}

			dwSize = (DWORD)(BASIC_SIZE/*sizeof(NET_MSG_GENERIC) + sizeof(WORD)*2 + sizeof(DWORD)*2*/ + nSync * sizeof(TITLE_SYNC));
			
			return TRUE;
		}

		TITLE_SYNC* GetData()
		{
			return dataSync;
		}
	};


	//
	//Client -------------------------------------------> FieldS
	//
	struct SNET_ACTIVITY_CHANGE_BADGE : public NET_MSG_GENERIC
	{
		int	nIndex;				//title index in TitleMananger

		SNET_ACTIVITY_CHANGE_BADGE()
		{
			dwSize = sizeof(SNET_ACTIVITY_CHANGE_BADGE);
			nType = NET_MSG_ACTIVITY_CHANGE_BADGE;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_CHANGE_BADGE)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//FieldS -------------------------------------------> Client
	//
	struct SNET_ACTIVITY_CHANGE_BADGE_FB : public NET_MSG_GENERIC
	{
		BOOL	bDone;

		SNET_ACTIVITY_CHANGE_BADGE_FB()
		{
			dwSize = sizeof(SNET_ACTIVITY_CHANGE_BADGE_FB);
			nType = NET_MSG_ACTIVITY_CHANGE_BADGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_CHANGE_BADGE_FB)<=NET_DATA_BUFSIZE);
		}
	};
	
	//
	//FieldS -------------------------------------------> Client
	//
	struct SNET_TITLE_NOTIFY_CHANGE : public SNETPC_BROAD
	{
		char	szNewTitle[EM_TITLEID_MAXLENGTH];
		UINT	nClass;

		SNET_TITLE_NOTIFY_CHANGE()
		{
			dwSize = sizeof(SNET_TITLE_NOTIFY_CHANGE);
			nType = NET_MSG_TITLE_NOTIFY_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_TITLE_NOTIFY_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//DbAction(from FieldS) --------------------------------> FieldS
	//
	struct SNET_ACTIVITY_LOAD_COMPLETED_FF : public NET_MSG_GENERIC
	{
		UINT nChaNum;

		SNET_ACTIVITY_LOAD_COMPLETED_FF()
		{
			dwSize = sizeof(SNET_ACTIVITY_LOAD_COMPLETED_FF);
			nType = NET_MSG_ACTIVITY_LOAD_COMPLETED_FF;
			nChaNum = 0;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVITY_LOAD_COMPLETED_FF)<=NET_DATA_BUFSIZE);
		}
	};


	//
	//mjeon.instance
	//	
	struct NET_MATCH_REQUEST_CA : public NET_MSG_GENERIC
	{
		DWORD	dwMapID;		// 어느 인던으로?
		DWORD	dwGateID;
		BOOL	bParty;
		BOOL	bUnmatch;		//Release인가? Match신청인가?

		NET_MATCH_REQUEST_CA()
			:dwMapID(INVALID_MAP_ID)
			,dwGateID(0)
			,bParty(FALSE)
			,bUnmatch(FALSE)
		{
			dwSize = sizeof(NET_MATCH_REQUEST_CA);
			nType = NET_MSG_MATCH_REQUEST_CA;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_REQUEST_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCH_REQUEST_CA_FB : public NET_MSG_GENERIC
	{
		DWORD	dwMapID;		// 어느 인던으로?
		DWORD	dwGateID;
		BOOL	bDone;

		NET_MATCH_REQUEST_CA_FB()
			:dwMapID(INVALID_MAP_ID)
			,dwGateID(0)
			,bDone(FALSE)
		{
			dwSize = sizeof(NET_MATCH_REQUEST_CA_FB);
			nType = NET_MSG_MATCH_REQUEST_CA_FB;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_REQUEST_CA_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCH_REQUEST_AM : public NET_MSG_UNIQUEKEY
	{
		DWORD dwMapID;		// 어느 인던으로?
		DWORD dwPartySize;	// 1 ~ MAX_PARTY_SIZE
		BOOL  bUnmatch;

		MATCH_CHAINFO arrParty[5];

		NET_MATCH_REQUEST_AM()
			:dwMapID(INVALID_MAP_ID)
			,dwPartySize(1)
			,bUnmatch(FALSE)
		{
			ZeroMemory(arrParty, sizeof(arrParty));

			dwSize = sizeof(NET_MATCH_REQUEST_AM);
			nType = NET_MSG_MATCH_REQUEST_AM;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_REQUEST_AM)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCH_REQUEST_RESULT_MAC : public NET_MSG_UNIQUEKEY
	{
		DWORD	dwMapID;		// 어느 인던으로?
		BOOL	bRelease;
		BOOL	bDone;

		NET_MATCH_REQUEST_RESULT_MAC()
			:dwMapID(INVALID_MAP_ID)			
			,bRelease(FALSE)
			,bDone(FALSE)
		{
			dwSize = sizeof(NET_MATCH_REQUEST_RESULT_MAC);
			nType = NET_MSG_MATCH_REQUEST_RESULT_MAC;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_REQUEST_RESULT_MAC)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCH_CANCEL_CAM : public NET_MSG_UNIQUEKEY
	{
		NET_MATCH_CANCEL_CAM()
		{
			dwSize = sizeof(NET_MATCH_CANCEL_CAM);
			nType = NET_MSG_MATCH_CANCEL_CAM;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_CANCEL_CAM)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_MATCH_CANCEL_CAM_FB : public NET_MSG_UNIQUEKEY
	{
		BOOL	bDone;

		NET_MATCH_CANCEL_CAM_FB()
			:bDone(TRUE)
		{
			dwSize = sizeof(NET_MATCH_CANCEL_CAM_FB);
			nType = NET_MSG_MATCH_CANCEL_CAM_FB;
			MIN_STATIC_ASSERT(sizeof(NET_MATCH_CANCEL_CAM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	//Transfer: FieldServer -> InstanceServer
	struct NET_TRANSFER_START_MA : public NET_MSG_UNIQUEKEY
	{
		DWORD	dwKey;			// InstanceKey
		DWORD	dwMapID;		// Instance의 MapID
		DWORD	dwGateID;
		DWORD	dwInstanceServerNum;	//어느 인스턴스 서버로 이전할 것인가?

		NET_TRANSFER_START_MA()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(INVALID_MAP_ID)
			,dwGateID(0)
			,dwInstanceServerNum(0)
		{
			dwSize = sizeof(NET_TRANSFER_START_MA);
			nType = NET_MSG_TRANSFER_START_MA;
			MIN_STATIC_ASSERT(sizeof(NET_TRANSFER_START_MA)<=NET_DATA_BUFSIZE);
		}
	};

	//Transfer: InstanceServer -> FieldServer
	struct NET_TRANSFER_START_MI : public NET_MSG_UNIQUEKEY
	{
		DWORD	dwKey;			// InstanceKey
		DWORD	dwMapID;		// Instance의 MapID
		DWORD	dwGateID;
		DWORD	dwInstanceServerNum;	//어느 인스턴스 서버로 이전할 것인가?

		NET_TRANSFER_START_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(INVALID_MAP_ID)
			,dwGateID(0)
			,dwInstanceServerNum(0)
		{
			dwSize = sizeof(NET_TRANSFER_START_MI);
			nType = NET_MSG_TRANSFER_START_MI;
			MIN_STATIC_ASSERT(sizeof(NET_TRANSFER_START_MI)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_TRANSFER_BACK_MI : public NET_MSG_UNIQUEKEY
	{
		DWORD	dwKey;					// 플레이 중이던 InstanceKey
		DWORD	dwInstanceServerNum;	// 어느 인스턴스 서버로부터 이전되는가?

		NET_TRANSFER_BACK_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwInstanceServerNum(0)
		{
			dwSize = sizeof(NET_TRANSFER_BACK_MI);
			nType = NET_MSG_TRANSFER_BACK_MI;
			MIN_STATIC_ASSERT(sizeof(NET_TRANSFER_BACK_MI)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_QUIT_ROOM_IM: public NET_MSG_UNIQUEKEY
	{
		DWORD	dwKey;
		DWORD	dwMapID;

		NET_QUIT_ROOM_IM()
			:dwKey(INVALID_INSTANCE_KEY)
		{
			dwSize = sizeof(NET_QUIT_ROOM_IM);
			nType = NET_MSG_QUIT_ROOM_IM;
			MIN_STATIC_ASSERT(sizeof(NET_QUIT_ROOM_IM)<=NET_DATA_BUFSIZE);
		}
	};

	struct NET_TRANSFER_REQUEST_AF : public NET_MSG_UNIQUEKEY
	{
		DWORD	dwInstanceServerNum;	//이전할(Target) InstanceServer의 번호
		DWORD	dwMapID;
		DWORD	dwGateID;
		DWORD	dwGaeaID;

		NET_TRANSFER_REQUEST_AF()
			:dwInstanceServerNum(0)
			,dwMapID(MAXDWORD)
			,dwGateID(MAXDWORD)
			,dwGaeaID(GAEAID_NULL)
		{
			dwSize = sizeof(NET_TRANSFER_REQUEST_AF);
			nType = NET_MSG_TRANSFER_REQUEST_AF;
			MIN_STATIC_ASSERT(sizeof(NET_TRANSFER_REQUEST_AF)<=NET_DATA_BUFSIZE);
		}
	};

	//
	//mjeon.instance.transfer.FAF or FAMI (M: Match, I: Instance)
	//
	struct SNET_TRANSFER_CHAR : public NET_MSG_UNIQUEKEY
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_UNIQUEKEY) + sizeof(WORD)*4 + sizeof(DWORD)*4,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};

		WORD	wSeq;		//1-base. current part-number of a large packet
		WORD	wLastSeq;	//total part-number of a large packet
							//i.e.>	wSeq = 1, wLastSeq = 4 means that there are 3 more packets remain
							//		and they must be gathered into 1 data to make it work correctly.

		WORD	wDataSize;
		WORD	wOnlye4DBSave;	//Is this transfer for the character's DB save when it closed connection in InstanceServer?

		//To InstanceServer: dwInstanceServerNum < MAXNUM_OF_INSTANCE_SERVER
		//To FieldServer:	 dwInstanceServerNum = MAXNUM_OF_INSTANCE_SERVER
		DWORD	dwInstanceServerNum;
		
		DWORD	dwMapID;		//Destination
		DWORD	dwGateID;		//Destination

		DWORD	dwReserved;		//8-bytes alignment


		BYTE	data[BUF_SIZE];

		SNET_TRANSFER_CHAR()
			:wDataSize(BUF_SIZE)
			,wOnlye4DBSave(0)
			,dwInstanceServerNum(MAXNUM_OF_INSTANCESERVER_TO_MATCH)
			,dwMapID(0)
			,dwGateID(0)
			,dwReserved(0)
		{
			dwSize = sizeof(SNET_TRANSFER_CHAR);
			nType = NET_MSG_TRANSFER_CHAR;
			MIN_STATIC_ASSERT(sizeof(SNET_TRANSFER_CHAR)<=NET_DATA_BUFSIZE);
		}
		
		//
		// calculate the pointer(position) based on Seq & LastSeq to copy memory.
		//
		BOOL SetData(BYTE bufferCompressed[MAX_SERIALIZED_CHAR_SIZE], int nTotalLen, WORD Seq, WORD LastSeq)
		{
			GASSERT(Seq > 0 && Seq <= LastSeq);
			if(Seq <= 0 || Seq > LastSeq)
				return FALSE;

			wSeq	= Seq;
			wLastSeq= LastSeq;

			if(Seq == LastSeq)
				wDataSize = nTotalLen % BUF_SIZE;
			else
				wDataSize = BUF_SIZE;

			GASSERT(wDataSize <= BUF_SIZE);
			if (wDataSize > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_TRANSFER_CHAR %1%", wDataSize ));
				return FALSE;
			}

			memcpy(data, bufferCompressed + (Seq - 1) * BUF_SIZE, wDataSize);

			dwSize = (DWORD)(BASIC_SIZE + wDataSize);

			GASSERT(dwSize <= NET_DATA_BUFSIZE);
			
			return TRUE;
		}

		BYTE* GetData()
		{
			return data;
		}

		BOOL IsToInstance()
		{
			return (dwInstanceServerNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);
		}
	};	

	struct SNET_TRANSFER_CHAR_DONE_FA : public NET_MSG_UNIQUEKEY
	{
		BOOL	bToInstance;	//현재 진행중인 Transfer의 방향 (FieldServer -> Instance or Instance -> FieldServer)

		DWORD	dwInstanceServerNum;	//Source or Target of the transfer
		DWORD	dwMapID;		//Destination
		DWORD	dwGateID;		//Destination

		SNET_TRANSFER_CHAR_DONE_FA()
			:bToInstance(FALSE)
			,dwInstanceServerNum(MAXNUM_OF_INSTANCESERVER_TO_MATCH)
			,dwMapID(MAXDWORD)
			,dwGateID(MAXDWORD)
		{
			dwSize = sizeof(SNET_TRANSFER_CHAR_DONE_FA);
			nType = NET_MSG_TRANSFER_CHAR_DONE_FA;
			MIN_STATIC_ASSERT(sizeof(SNET_TRANSFER_CHAR_DONE_FA)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNET_TRANSFERRED_CHAR_JOIN_AF : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER
	{
		DWORD dwKey;	//Instance Key
		DWORD dwGaeaID;
		DWORD dwMapID;	//Destination
        wb::EM_COUNTRY m_Country;

		SNET_TRANSFERRED_CHAR_JOIN_AF(wb::EM_COUNTRY Country)
			:dwKey(INVALID_INSTANCE_KEY)
			,dwGaeaID(GAEAID_NULL)
			,dwMapID(MAXDWORD)
            , m_Country(Country)
		{
			dwSize = sizeof(SNET_TRANSFERRED_CHAR_JOIN_AF);
			nType = NET_MSG_TRANSFERRED_CHAR_JOIN_AF;
			MIN_STATIC_ASSERT(sizeof(SNET_TRANSFERRED_CHAR_JOIN_AF)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CREATE_INSTANCE_MI : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;	//Destination

		SNET_CREATE_INSTANCE_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
		{
			dwSize = sizeof(SNET_CREATE_INSTANCE_MI);
			nType = NET_MSG_CREATE_INSTANCE_MI;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANCE_MI)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CREATE_INSTANCE_MI_FB : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;

		INSTANCE_CREATE_CODE status;

		SNET_CREATE_INSTANCE_MI_FB()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
			,status(INSTANCE_CREATE_DONE)
		{
			dwSize = sizeof(SNET_CREATE_INSTANCE_MI_FB);
			nType = NET_MSG_CREATE_INSTANCE_MI_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_CREATE_INSTANCE_MI_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_DELETE_INSTANCE_MI : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;	//Destination

		SNET_DELETE_INSTANCE_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
		{
			dwSize = sizeof(SNET_DELETE_INSTANCE_MI);
			nType = NET_MSG_DELETE_INSTANCE_MI;
			MIN_STATIC_ASSERT(sizeof(SNET_DELETE_INSTANCE_MI)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_DELETE_INSTANCE_MI_FB : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;

		INSTANCE_DELETE_CODE status;

		SNET_DELETE_INSTANCE_MI_FB()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
			,status(INSTANCE_DELETE_DONE)
		{
			dwSize = sizeof(SNET_DELETE_INSTANCE_MI_FB);
			nType = NET_MSG_DELETE_INSTANCE_MI_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_DELETE_INSTANCE_MI_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INSTANCE_NEW_MEMBER_MI : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;

		DWORD dwChaNew;		

		SNET_INSTANCE_NEW_MEMBER_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
			,dwChaNew(MAXDWORD)
		{
			dwSize = sizeof(SNET_INSTANCE_NEW_MEMBER_MI);
			nType = NET_MSG_INSTANCE_NEW_MEMBER_MI;
			MIN_STATIC_ASSERT(sizeof(SNET_INSTANCE_NEW_MEMBER_MI)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_INSTANCE_DEL_MEMBER_MI : public NET_MSG_GENERIC
	{
		DWORD dwKey;
		DWORD dwMapID;

		DWORD dwChaNew;		

		SNET_INSTANCE_DEL_MEMBER_MI()
			:dwKey(INVALID_INSTANCE_KEY)
			,dwMapID(MAXDWORD)
			,dwChaNew(MAXDWORD)
		{
			dwSize = sizeof(SNET_INSTANCE_DEL_MEMBER_MI);
			nType = NET_MSG_INSTANCE_DEL_MEMBER_MI;
			MIN_STATIC_ASSERT(sizeof(SNET_INSTANCE_DEL_MEMBER_MI)<=NET_DATA_BUFSIZE);
		}
	};

	//mjeon.groupchat
	struct SNET_GCHAT_INVITE_CAC : public NET_MSG_GENERIC
	{
		DWORD dwKey;	// == ChaNumOwner
		char  szChaNameOwner[CHR_ID_LENGTH];	//set by Server
		DWORD dwChaNumWhom;	// who will be invited?	-- set by Client

		SNET_GCHAT_INVITE_CAC()
			:dwKey(INVALID_CHAT_KEY)
			,dwChaNumWhom(INVALID_CHANUM)
		{
			ZeroMemory(szChaNameOwner, sizeof(szChaNameOwner));

			dwSize = sizeof(SNET_GCHAT_INVITE_CAC);
			nType = NET_MSG_GCHAT_INVITE_CAC;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_INVITE_CAC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GCHAT_INVITE_CAC_FB : public NET_MSG_GENERIC
	{
		DWORD dwKey;		// == ChaNumOwner
		DWORD dwChaWhom;	// == 초대받은 케릭
		BOOL bAccept;		//TRUE: Accept , FALSE: Refuse
		BOOL bAcceptDone;	//Accept가 TRUE일 지라도, Accept 처리과정에서 오류가 발생한 경우 이 값은 FALSE가 된다. 입장 불가능.

		SNET_GCHAT_INVITE_CAC_FB()
			:dwKey(INVALID_CHAT_KEY)
			,dwChaWhom(INVALID_CHANUM)
			,bAccept(FALSE)
			,bAcceptDone(FALSE)
		{
			dwSize = sizeof(SNET_GCHAT_INVITE_CAC_FB);
			nType = NET_MSG_GCHAT_INVITE_CAC_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_INVITE_CAC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GCHAT_ERRORCODE : public NET_MSG_GENERIC
	{
		DWORD dwKey;	// == ChaNumOwner
		CHAT_ERRORCODE	code;

		SNET_GCHAT_ERRORCODE()
			:dwKey(INVALID_CHAT_KEY)
			,code(CHAT_CODE_OK)
		{
			dwSize = sizeof(SNET_GCHAT_ERRORCODE);
			nType = NET_MSG_GCHAT_ERRORCODE;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_ERRORCODE)<=NET_DATA_BUFSIZE);
		}
	};

	//새로운 채팅룸에 join시에 한번만 동기화
	struct SNET_GCHAT_MEMBER_SYNC_AC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(DWORD) * 2,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};
		
		DWORD	nSync;		//How many members are included in this msg.
		DWORD	dwKey;

		CHAT_MEMBER			dataSync[HOW_MANY_CHAT_MEMBERS_PER_ONE_SYNC];


		SNET_GCHAT_MEMBER_SYNC_AC()
			:nSync(0)
			,dwKey(INVALID_CHAT_KEY)
		{
			dwSize = BASIC_SIZE;
			nType = NET_MSG_GCHAT_MEMBER_SYNC_AC;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_MEMBER_SYNC_AC)<=NET_DATA_BUFSIZE);
		}

		BOOL AddData(CHAT_MEMBER &member)
        {
			if (nSync >= HOW_MANY_CHAT_MEMBERS_PER_ONE_SYNC)
                return FALSE;

			size_t size = (nSync + 1) * sizeof(CHAT_MEMBER);

            if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_GCHAT_MEMBER_SYNC_AC %1%", size ));
                return FALSE;
			}

			dataSync[nSync++] = member;

            dwSize = (DWORD)(BASIC_SIZE + size);

            return TRUE;
        }

		void Reset()
		{
			nSync = 0;
			ZeroMemory(dataSync, sizeof(dataSync));
		}
	};


	//ooo님이 채팅에 참여(퇴장)했습니다.
	struct SNET_GCHAT_MEMBER_UPDATE_AC : public NET_MSG_GENERIC
	{
		DWORD dwKey;	// == ChaNumOwner
		BOOL bJoin;		//TRUE: Join, FALSE: Quit
		
		CHAT_MEMBER	who;

		SNET_GCHAT_MEMBER_UPDATE_AC()
			:dwKey(INVALID_CHAT_KEY)
		{
			dwSize = sizeof(SNET_GCHAT_MEMBER_UPDATE_AC);
			nType = NET_MSG_GCHAT_MEMBER_UPDATE_AC;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_MEMBER_UPDATE_AC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GCHAT_QUIT_CA : public NET_MSG_GENERIC
	{
		DWORD dwKey;	// == ChaNumOwner

		SNET_GCHAT_QUIT_CA()
			:dwKey(INVALID_CHAT_KEY)
		{
			dwSize = sizeof(SNET_GCHAT_QUIT_CA);
			nType = NET_MSG_GCHAT_QUIT_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_QUIT_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GCHAT_KICK_CA : public NET_MSG_GENERIC
	{
		DWORD dwChaNum;	//who will be kicked?

		SNET_GCHAT_KICK_CA()
			:dwChaNum(INVALID_CHANUM)
		{
			dwSize = sizeof(SNET_GCHAT_KICK_CA);
			nType = NET_MSG_GCHAT_KICK_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_KICK_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_GCHAT_MESSAGE_CAC : public NET_MSG_GENERIC
	{
		enum
		{
			BASIC_SIZE	=	sizeof(NET_MSG_GENERIC) + sizeof(DWORD) * 3 + sizeof(char) * MAX_LENGTH_OF_CHAT_MESSAGE,
			BUF_SIZE	=	NET_DATA_BUFSIZE - BASIC_SIZE
		};
		
		DWORD		dwKey;
		DWORD		dwChaSay;	//who's saying? - set by Server
		DWORD		nLink;		//How many links are included in this msg.
		char		szChat[MAX_LENGTH_OF_CHAT_MESSAGE];

		SITEMCUSTOM	itemLink[HOW_MANY_ITEM_LINK_PER_CHAT_MSG];


		SNET_GCHAT_MESSAGE_CAC()
			:dwKey(INVALID_CHAT_KEY)
			,dwChaSay(INVALID_CHANUM)
			,nLink(0)
		{
			dwSize = BASIC_SIZE;
			nType = NET_MSG_GCHAT_MESSAGE;

			ZeroMemory(szChat, sizeof(szChat));
			ZeroMemory(itemLink, sizeof(itemLink));

			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_MESSAGE_CAC)<=NET_DATA_BUFSIZE);
		}

		BOOL AddLink(SITEMCUSTOM &item)
        {
			if (nLink >= HOW_MANY_ITEM_LINK_PER_CHAT_MSG)
                return FALSE;

			size_t size = (nLink + 1) * sizeof(SITEMCUSTOM);

            if (size > BUF_SIZE)
			{
				sc::writeLogError(sc::string::format("BUF_SIZE OVER : SNET_GCHAT_MESSAGE_CAC %1%", size ));
                return FALSE;
			}

			itemLink[nLink++] = item;

            dwSize = (DWORD)(BASIC_SIZE + size);

            return TRUE;
        }
	};

	struct SNET_GCHAT_CONFIG_AC : public NET_MSG_GENERIC
	{
		DWORD dwMaxGroupChatOpen;	//최대 참여 가능한 그룹채팅의 수
		DWORD dwMaxGroupChatTO;		//하나의 그룹채팅 방에 참여가능한 최대 인원

		SNET_GCHAT_CONFIG_AC()
			:dwMaxGroupChatOpen(0)
			,dwMaxGroupChatTO(0)
		{
			dwSize = sizeof(SNET_GCHAT_CONFIG_AC);
			nType = NET_MSG_GCHAT_CONFIG_AC;
			MIN_STATIC_ASSERT(sizeof(SNET_GCHAT_CONFIG_AC)<=NET_DATA_BUFSIZE);
		}
	};

	
	
	//! 한국 셧다운 Shutdown 경고 메시지
    struct NET_KOR_SHUTDOWN_WARNING_AC : public NET_MSG_GENERIC
    {
        NET_KOR_SHUTDOWN_WARNING_AC()
        {
            dwSize = sizeof(NET_KOR_SHUTDOWN_WARNING_AC);
            nType = NET_MSG_KOR_SHUTDOWN_WARNING_AC;
            MIN_STATIC_ASSERT(sizeof(NET_KOR_SHUTDOWN_WARNING_AC)<=NET_DATA_BUFSIZE);
        }
    };

	//! 클럽 마스터 위임 ON/OFF Client->Agent
	struct SNET_CLUB_MASTER_MANDATE_CA : public NET_MSG_GENERIC
	{
		bool m_bBool;

		SNET_CLUB_MASTER_MANDATE_CA()
			: m_bBool(TRUE)
		{
			MIN_STATIC_ASSERT(sizeof(SNET_CLUB_MASTER_MANDATE_CA)<=NET_DATA_BUFSIZE);

			dwSize = sizeof(SNET_CLUB_MASTER_MANDATE_CA);
			nType  = NET_MSG_CLUB_MASTER_MANDATE_CA;
		}
	};
	
	//! User System Info
	struct SNET_USER_SYSTEM_INFO_CA : public NET_MSG_GENERIC
	{
		DWORD m_dwCpuCoreNumber;
		DWORD m_dwCpuClock;
		DWORDLONG m_dwMemorySize;
		DWORD m_dwResolutionX;
		DWORD m_dwResolutionY;
		DWORD m_dwGpuVendor;		// 0 - Etc, 1 - Intel, 2 - Nvidia, 3 - AMD
		DWORD m_dwGpuModel;
		DWORD m_dwShaderModel;
		int m_languageCode;			// SERVICENATION

		SNET_USER_SYSTEM_INFO_CA(DWORD dwCpuCoreNumber, 
								DWORD dwCpuClock, 
								DWORDLONG dwMemorySize,
								DWORD dwResolutionX,
								DWORD dwResolutionY,
								DWORD dwGpuVendor,
								DWORD dwGpuModel,
								DWORD dwShaderModel, 
								int languageCode)
			: m_dwCpuCoreNumber(dwCpuCoreNumber)
			, m_dwCpuClock(dwCpuClock)
			, m_dwMemorySize(dwMemorySize)
			, m_dwResolutionX(dwResolutionX)
			, m_dwResolutionY(dwResolutionY)
			, m_dwGpuVendor(dwGpuVendor)
			, m_dwGpuModel(dwGpuModel)
			, m_dwShaderModel(dwShaderModel)
			, m_languageCode(languageCode)
		{
			dwSize = sizeof(SNET_USER_SYSTEM_INFO_CA);
			nType = NET_MSG_USER_SYSTEM_INFO_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_USER_SYSTEM_INFO_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_GET_DATA_CHANGE : public NET_MSG_GENERIC
	{	
		DWORD dwChaDbNum;
		BOOL bPostGotData;

		SNET_POST_GET_DATA_CHANGE()
			: dwChaDbNum( 0 )
			, bPostGotData( FALSE )
		{
			dwSize = sizeof(SNET_POST_GET_DATA_CHANGE);
			nType = NET_MSG_POST_GOT_DATA_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_DATA_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_GET_STATE_CHANGE : public NET_MSG_GENERIC
	{	
		DWORD dwChaDbNum;
		BOOL bPostGotState;

		SNET_POST_GET_STATE_CHANGE()
			: dwChaDbNum( 0 )
			, bPostGotState( FALSE )
		{
			dwSize = sizeof(SNET_POST_GET_STATE_CHANGE);
			nType = NET_MSG_POST_GOT_STATE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_STATE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_GET_SEND_STATE_CHANGE : public NET_MSG_GENERIC
	{	
		DWORD dwChaDbNum;
		BOOL bPostGotSendState;

		SNET_POST_GET_SEND_STATE_CHANGE()
			: dwChaDbNum( 0 )
			, bPostGotSendState( FALSE )
		{
			dwSize = sizeof(SNET_POST_GET_SEND_STATE_CHANGE);
			nType = NET_MSG_POST_GOT_SEND_STATE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_SEND_STATE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_POST_GET_RECV_STATE_CHANGE : public NET_MSG_GENERIC
	{	
		DWORD dwChaDbNum;
		BOOL bPostGotRecvState;

		SNET_POST_GET_RECV_STATE_CHANGE()
			: dwChaDbNum( 0 )
			, bPostGotRecvState( FALSE )
		{
			dwSize = sizeof(SNET_POST_GET_RECV_STATE_CHANGE);
			nType = NET_MSG_POST_GOT_RECV_STATE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNET_POST_GET_RECV_STATE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_RELOAD_AI_FSM_CF : public NET_MSG_GENERIC
	{
		SNET_RELOAD_AI_FSM_CF()
		{
			dwSize = sizeof( SNET_RELOAD_AI_FSM_CF );
			nType = NET_MSG_GCTRL_RELOAD_AI_FSM_CF;
			MIN_STATIC_ASSERT( sizeof( SNET_RELOAD_AI_FSM_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_APPLY_SKILLEFFECT_FC : public NET_MSG_GENERIC
	{
		Faction::GLActorID sActorID;
		EMSPEC_ADDON emSpecAddon;
		bool bState;

		SNET_APPLY_SKILLEFFECT_FC( Faction::GLActorID _sActorID
			, EMSPEC_ADDON _emSpecAddon
			, bool _bState )
			: sActorID( _sActorID )
			, emSpecAddon( _emSpecAddon )
			, bState( _bState )
		{
			dwSize = sizeof( SNET_APPLY_SKILLEFFECT_FC );
			nType = NET_MSG_GCTRL_APPLY_SKILLEFFECT_FC;
			MIN_STATIC_ASSERT( sizeof( SNET_APPLY_SKILLEFFECT_FC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_SYNC_USE_FEATURES_AC : public NET_MSG_GENERIC
	{
		bool bUseFeatures[ GLUseFeatures::EMUSE_FEATURE_SIZE ];

		SNET_SYNC_USE_FEATURES_AC ( )
		{
			dwSize = sizeof( SNET_SYNC_USE_FEATURES_AC );
			nType = NET_MSG_GCTRL_SYNC_USE_FEATURES_AC;
			MIN_STATIC_ASSERT( sizeof( SNET_SYNC_USE_FEATURES_AC ) <= NET_DATA_BUFSIZE );

			ZeroMemory( bUseFeatures, sizeof( bUseFeatures ) );
		}
	};

	struct SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC : public NET_MSG_GENERIC
	{
		GLUseFeatures::EMUSE_FEATURES emUserFeatures;
		bool bUseFeatures;

		SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC ( )
		{
			dwSize = sizeof( SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC );
			nType = NET_MSG_GCTRL_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC;
			MIN_STATIC_ASSERT( sizeof( SNET_SYNC_USE_FEATURES_JOINWORLDBATTLE_AC ) <= NET_DATA_BUFSIZE );

			bUseFeatures = false;
			emUserFeatures = GLUseFeatures::EMUSE_FEATURE_SIZE;
		}
	};

	struct SNET_GM_RELEASE_CTF_BUFF : public NET_MSG_GENERIC
	{
		SNET_GM_RELEASE_CTF_BUFF()
		{
			dwSize = sizeof( SNET_GM_RELEASE_CTF_BUFF );
			nType =  NET_MSG_GM_RELEASE_CTF_BUFF;
			MIN_STATIC_ASSERT( sizeof( SNET_GM_RELEASE_CTF_BUFF ) <= NET_DATA_BUFSIZE );
		}
	};

// Revert to default structure packing
//	#pragma pack()
};

