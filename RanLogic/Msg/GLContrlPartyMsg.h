#pragma once

#include "GLContrlBaseMsg.h"

enum EMPARTY_LINK_JOIN_FB
{
	EMPARTY_LINK_JOIN_FB_OK			= 0,	//	성공.
	EMPARTY_LINK_JOIN_FB_FAIL		= 1,	//	실패.
	EMPARTY_LINK_JOIN_FB_FULL		= 2,	//	가득 찼다.
	EMPARTY_LINK_JOIN_FB_NOCHAR		= 3,	//	캐릭터가 없습니다.
	EMPARTY_LINK_JOIN_FB_REJOIN		= 4,	//	이미 다른 파티에 가입 
	EMPARTY_LINK_JOIN_FB_NOMASTER	= 5,	//	해당 캐릭터가 마스터가 아님
	EMPARTY_LINK_JOIN_FB_OTHERSCHOOL= 6,	//	다른 학교라서 안됌
	EMPARTY_LINK_JOIN_FB_CLUBBATTLE	= 7,	//	클럽 배틀중에는 불가
	EMPARTY_LINK_JOIN_FB_CHANNEL	= 8,	//	채널이 달라서 불가함
	EMPARTY_LINK_JOIN_FB_CTF	= 9, // CTF 중에는 다른 학원과 불가;
};

enum EMPARTY_LINK_LURE_FB
{
	EMPARTY_LINK_LURE_FB_OK			= 0,	//	성공.
	EMPARTY_LINK_LURE_FB_FAIL		= 1,	//	실패.
	EMPARTY_LINK_LURE_FB_FULL		= 2,	//	가득 찼다.
	EMPARTY_LINK_LURE_FB_NOCHAR		= 3,	//	캐릭터가 없습니다.
	EMPARTY_LINK_LURE_FB_REJOIN		= 4,	//	이미 다른 파티에 가입 
	EMPARTY_LINK_LURE_FB_NOMASTER	= 5,	//	해당 캐릭터가 마스터가 아님
	EMPARTY_LINK_LURE_FB_OTHERSCHOOL= 6,	//	다른 학교라서 안됌
	EMPARTY_LINK_LURE_FB_CLUBBATTLE	= 7,	//	클럽 배틀중에는 불가
	EMPARTY_LINK_LURE_FB_CHANNEL	= 8,	//	채널이 달라서 불가함
    EMPARTY_LINK_LURE_FB_OPT		= 9,	//	파티 옵션 오류.
};

enum EMPARTY_REFRESH_REQ
{
	EMPARTY_REFRESH_REQ_PARTY_INFORMATION, // 파티/원정대의 정보 요청;
	EMPARTY_REFRESH_REQ_MEMBER_INFORMATION, // 파티/원정대 멤버들의 상세 정보 요청;
};

enum EMPARTY_ACTION_TYPE {
	PARTY_ACTION_NON_TYPE    = 0,
	PARTY_ACTION_BAN_TYPE    = 1,
	PARTY_ACTION_SECEDE_TYPE = 2,
};

namespace GLMSG
{
	#pragma pack(1)

	struct NET_PARTY_MESSAGE : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;

		NET_PARTY_MESSAGE(const GLPartyID& _PartyID)
			: PartyID(_PartyID)
		{
		}
	};
	//---------------------------------------------------------------------------NET
    struct SNET_PARTY_LURE : public NET_MSG_GENERIC
    {
        DWORD				dwGaeaID;
        SPartyOption			sOption;

        SNET_PARTY_LURE( DWORD _dwGaeaID, SPartyOption _sOption ) 
            : dwGaeaID(_dwGaeaID)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNET_PARTY_LURE);
			nType = NET_MSG_GCTRL_PARTY_LURE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE)<=NET_DATA_BUFSIZE);
		}
	};
	
    //---------------------------------------------------------------------------NET
    struct SNET_PARTY_LURE_WITH_CHARNUM : public NET_MSG_CHARACTER
    {
        SPartyOption			sOption;

        SNET_PARTY_LURE_WITH_CHARNUM( SPartyOption _sOption ) 
            : sOption(_sOption)
        {
            dwSize = sizeof(SNET_PARTY_LURE_WITH_CHARNUM);
            nType = NET_MSG_GCTRL_PARTY_LURE_WITH_CHARNUM;
            MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_WITH_CHARNUM)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_LURE_FB : public NET_MSG_GENERIC
	{
		const DWORD					dwGaeaID;
		const EMPARTY_LURE_FB	emAnswer;

        SNET_PARTY_LURE_FB(const DWORD _dwGaeaID, const EMPARTY_LURE_FB _emAnswer)
			: dwGaeaID(_dwGaeaID)
			, emAnswer(_emAnswer)
		{
			dwSize = sizeof(SNET_PARTY_LURE_FB);
			nType = NET_MSG_GCTRL_PARTY_LURE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNET_PARTY_LURE_FB_WITH_CHARNAME : public NET_MSG_GENERIC
    {
        char szCharName[CHAR_SZNAME];
        EMPARTY_LURE_FB emAnswer;

        SNET_PARTY_LURE_FB_WITH_CHARNAME( std::string& _strName, EMPARTY_LURE_FB _emAnswer ) 
            : emAnswer(_emAnswer)
        {
            StringCchCopy(szCharName, CHAR_SZNAME, _strName.c_str());

            dwSize = sizeof(SNET_PARTY_LURE_FB_WITH_CHARNAME);
            nType = NET_MSG_GCTRL_PARTY_LURE_FB_WITH_CHARNAME;
            MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_FB_WITH_CHARNAME)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_LURE_TAR : public NET_MSG_GENERIC
	{
		GLPartyID		PartyID;
		DWORD			dwMasterID;
		SPartyOption	sOption;
		bool				bExpedition;

        SNET_PARTY_LURE_TAR ( GLPartyID _PartyID, DWORD _dwMasterID, SPartyOption _sOption ) 
			: PartyID(_PartyID)
			, dwMasterID(_dwMasterID)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNET_PARTY_LURE_TAR);
			nType = NET_MSG_GCTRL_PARTY_LURE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_TAR)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNET_PARTY_LURE_TAR_WITH_CHARNAME : public NET_MSG_GENERIC
    {
        GLPartyID partyID;
		DWORD dwHashKey;
        DWORD dwMasterID;
		DWORD dwTimeKey;
		SPartyOption partyOption;
        char szMasterName[CHAR_SZNAME];        
		bool bExpedition;

        SNET_PARTY_LURE_TAR_WITH_CHARNAME(const GLPartyID& _partyID, const DWORD _dwHashKey, const DWORD _dwMasterID, const DWORD _dwTimeLimit, const std::string& _strMasterName, const SPartyOption& _partyOption) 
            : partyID(_partyID)
			, dwHashKey(_dwHashKey)
            , dwMasterID(_dwMasterID)
			, dwTimeKey(_dwTimeLimit)
            , partyOption(_partyOption)
        {
            StringCchCopy(szMasterName, CHAR_SZNAME, _strMasterName.c_str());

            dwSize = sizeof(SNET_PARTY_LURE_TAR_WITH_CHARNAME);
            nType = NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME;
            MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_TAR_WITH_CHARNAME)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET	
	struct SNET_PARTY_LURE_TAR_ANS : public NET_MSG_GENERIC
	{
		EMPARTY_LURE_FB	emAnswer;
		GLPartyID				partyID;
		DWORD					dwHashKey;
		DWORD					dwMasterID;
		DWORD					dwTimeKey;
		SPartyOption			partyOption;

        SNET_PARTY_LURE_TAR_ANS(const EMPARTY_LURE_FB _emAnswer, const GLPartyID _partyID, const DWORD _dwHashKey, const  DWORD _dwMasterID, const DWORD _dwTimeKey, const SPartyOption& _partyOption = SPartyOption()) 
			: emAnswer(_emAnswer)
			, partyID(_partyID)
			, dwHashKey(_dwHashKey)
			, dwMasterID(_dwMasterID)
			, dwTimeKey(_dwTimeKey)
            , partyOption(_partyOption)
		{
			dwSize = sizeof(SNET_PARTY_LURE_TAR_ANS);
			nType = NET_MSG_GCTRL_PARTY_LURE_TAR_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LURE_TAR_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_SECEDE : public NET_MSG_GENERIC
	{		
		EMPARTY_ACTION_TYPE	eType;
		GLPartyID			PartyID;
		DWORD				dwGaeaID;

        SNET_PARTY_SECEDE ( GLPartyID _PartyID, DWORD _dwGaeaID, EMPARTY_ACTION_TYPE eActionType = PARTY_ACTION_NON_TYPE ) 
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID) , eType(eActionType)
		{
			dwSize = sizeof(SNET_PARTY_SECEDE);
			nType = NET_MSG_GCTRL_PARTY_SECEDE;
            MIN_STATIC_ASSERT(sizeof(SNET_PARTY_SECEDE)<=NET_DATA_BUFSIZE);
		}
	};
	
	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_AUTHORITY : public NET_PARTY_MESSAGE
	{
		const DWORD dwNewMasterGaeaID;
		const DWORD dwOldMasterGaeaID;
		const DWORD dwPartyIndex;

		char szCharName[CHAR_SZNAME];

        SNET_PARTY_AUTHORITY(const GLPartyID& _PartyID, const DWORD _dwNewMasterGaeaID, const DWORD _dwOldMasterGaeaID, const DWORD _dwPartyIndex = MASTER_PARTY) 
			: NET_PARTY_MESSAGE(_PartyID)
			, dwNewMasterGaeaID(_dwNewMasterGaeaID)
			, dwOldMasterGaeaID(_dwOldMasterGaeaID)
			, dwPartyIndex(_dwPartyIndex)
		{
			memset( szCharName, 0, sizeof(char)*CHAR_SZNAME );
			dwSize = sizeof( SNET_PARTY_AUTHORITY );
			nType = NET_MSG_GCTRL_PARTY_AUTHORITY;
            MIN_STATIC_ASSERT(sizeof(SNET_PARTY_AUTHORITY)<=NET_DATA_BUFSIZE);
		}
	};


	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_FNEW : public NET_PARTY_MESSAGE
	{
		DWORD				dwMaster;
		SPartyOption		sOption;
		DWORD				nPartyIndex;		

		DWORD				nMember;		
		GLPARTY_FNET	sPartyInfo[MAXPARTY];

		void ADDMEMBER(const GLPARTY_FNET& sParty )
		{
			if (nMember >= MAXPARTY )
				return;

			sPartyInfo[nMember] = sParty;
			++nMember;
			dwSize += sizeof(GLPARTY_FNET);
		}		

        SNET_PARTY_FNEW(const GLPartyID& PartyID, const DWORD _dwMaster, const SPartyOption& _sOption, const DWORD _nPartyIndex = MASTER_PARTY)
            :   NET_PARTY_MESSAGE(PartyID)
              , dwMaster(_dwMaster)
              , sOption(_sOption)
			  , nPartyIndex(_nPartyIndex)
              , nMember(0)
		{
			dwSize = sizeof(SNET_PARTY_FNEW) - sizeof(GLPARTY_FNET) * MAXPARTY;
			nType = NET_MSG_GCTRL_PARTY_FNEW;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_FNEW)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_FNEW_UPDATE : public NET_PARTY_MESSAGE
	{
		DWORD				dwGaeID;	
		DWORD				dwMaster;
		SPartyOption		sOption;
		DWORD				nPartyIndex;		

		DWORD				nMember;		
		GLPARTY_FNET	sPartyInfo[MAXPARTY];

		void ADDMEMBER(const GLPARTY_FNET& sParty )
		{
			if (nMember >= MAXPARTY )
				return;

			sPartyInfo[nMember] = sParty;
			++nMember;
			dwSize += sizeof(GLPARTY_FNET);
		}		

		SNET_PARTY_FNEW_UPDATE(const GLPartyID& PartyID, const DWORD _dwGaeID, const DWORD _dwMaster, const SPartyOption& _sOption, const DWORD _nPartyIndex = MASTER_PARTY)
			:   NET_PARTY_MESSAGE(PartyID)
			, dwMaster(_dwMaster)
			, sOption(_sOption)
			, nPartyIndex(_nPartyIndex)
			, nMember(0)
			, dwGaeID(_dwGaeID)
		{
			dwSize = sizeof(SNET_PARTY_FNEW_UPDATE) - sizeof(GLPARTY_FNET) * MAXPARTY;
			nType = NET_MSG_GCTRL_PARTY_FNEW_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_FNEW_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_MBR_DETAIL : public NET_PARTY_MESSAGE
	{		
		const DWORD dwGaeaID;
		const D3DXVECTOR3 vPos;
		const GLPADATA sHP;
		const int Level;

        SNET_PARTY_MBR_DETAIL(
			const GLPartyID& _PartyID,
			const DWORD _dwGaeaID,
			const D3DXVECTOR3& _vPos,
            const GLPADATA& _sHP, const int _level)
            : NET_PARTY_MESSAGE(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, vPos(_vPos)
			, sHP(_sHP)
			, Level(_level)
		{
			dwSize = sizeof(SNET_PARTY_MBR_DETAIL);
			nType = NET_MSG_GCTRL_PARTY_MBR_DETAIL;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MBR_DETAIL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_MBR_DETAIL_PARTY : public NET_PARTY_MESSAGE
	{
		const DWORD dwMyGaeaID;
		DWORD nMember;

		struct MEMBER_DETAIL_INFORMATION
		{
			DWORD				dwGaeaID;
			GLPADATA			sHP;
			int						Level;
			D3DXVECTOR3		vPos;
		};
		MEMBER_DETAIL_INFORMATION MemberDetail[MAXPARTY];

		SNET_PARTY_MBR_DETAIL_PARTY(const GLPartyID& _PartyID, const DWORD _dwMyGaeaID)
			: NET_PARTY_MESSAGE(_PartyID)
			, dwMyGaeaID(_dwMyGaeaID)
			, nMember(0)
		{
			dwSize = sizeof(SNET_PARTY_MBR_DETAIL_PARTY) - sizeof(MEMBER_DETAIL_INFORMATION) * MAXPARTY;
			nType = NET_MSG_GCTRL_PARTY_MBR_DETAIL_PARTY;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MBR_DETAIL_PARTY)<=NET_DATA_BUFSIZE);			
		}
		void AddMemberDetail(
			const DWORD dwGaeaID, const GLPADATA& sHP, 
			const int Level, const D3DXVECTOR3& vPos)
		{
			if ( nMember > (MAXPARTY - 1) )
				return;

			MEMBER_DETAIL_INFORMATION& Member = MemberDetail[nMember];

			Member.dwGaeaID = dwGaeaID;
			Member.sHP = sHP;
			Member.Level = Level;
			Member.vPos = vPos;			

			dwSize += sizeof(MEMBER_DETAIL_INFORMATION);
			++nMember;			
		}
	};

	struct SNET_PARTY_MBR_LEVEL_UP : public NET_MSG_GENERIC
	{
		DWORD	dwGaeaID;
		int			Level;
		DWORD	dwPartyIndex;

		SNET_PARTY_MBR_LEVEL_UP( DWORD _dwGaeaID, int _level, DWORD _dwPartyIndex = 0 )
			: dwGaeaID(_dwGaeaID)
			, Level(_level)
			, dwPartyIndex(_dwPartyIndex)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_MBR_LEVELUP;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MBR_LEVEL_UP)<=NET_DATA_BUFSIZE);
		}
	};


	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_DISSOLVE : public NET_PARTY_MESSAGE
	{
		SNET_PARTY_DISSOLVE(const GLPartyID& _PartyID ) 
			: NET_PARTY_MESSAGE(_PartyID)
		{
			dwSize = sizeof(SNET_PARTY_DISSOLVE);
			nType = NET_MSG_GCTRL_PARTY_DISSOLVE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_DISSOLVE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_ADD : public NET_PARTY_MESSAGE
	{		
		GLPARTY_FNET	sPartyInfo;

        SNET_PARTY_ADD(const GLPartyID& _PartyID, const GLPARTY_FNET& _sPartyInfo)
            : NET_PARTY_MESSAGE(_PartyID)
            , sPartyInfo(_sPartyInfo)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_ADD;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_ADD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_DEL : public NET_PARTY_MESSAGE
	{
		DWORD				delMemberGaeaID;
		DWORD				delMemberDbNum;
		SNATIVEID			currentMapID;

		EMPARTY_ACTION_TYPE eType;

        SNET_PARTY_DEL(const GLPartyID& _PartyID, const DWORD _delMemberGaeaID, const DWORD _delMemberDbNum, const SNATIVEID& _currentMapID, EMPARTY_ACTION_TYPE eActionType = PARTY_ACTION_NON_TYPE)
            : NET_PARTY_MESSAGE(_PartyID)
            , delMemberGaeaID(_delMemberGaeaID)
			, delMemberDbNum(_delMemberDbNum)
			, currentMapID(_currentMapID)
			, eType(eActionType)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_DEL;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_MBR_MOVEMAP : public NET_PARTY_MESSAGE
	{
		const DWORD		dwGaeaID;
		const MapID		mapID;

        SNET_PARTY_MBR_MOVEMAP(const GLPartyID& _PartyID, const DWORD& _dwGaeaID, const MapID& _mapID)
            : NET_PARTY_MESSAGE(_PartyID)
            , dwGaeaID(_dwGaeaID)
            , mapID(_mapID)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_MBR_MOVEMAP;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MBR_MOVEMAP)<=NET_DATA_BUFSIZE);
		}
	};
	
	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_MBR_UPDATE : public NET_PARTY_MESSAGE
	{
		const DWORD dwGaeaID;
		const D3DXVECTOR3 vPos;
		const GLDWDATA sHP;
		const WORD wLevel;

        SNET_PARTY_MBR_UPDATE(const GLPartyID& _PartyID, const DWORD _dwGaeaID, const D3DXVECTOR3& _vPos, const GLDWDATA& _sHP, const WORD& _wLevel)
            : NET_PARTY_MESSAGE(_PartyID)
			, dwGaeaID(_dwGaeaID)
            , vPos(_vPos)
            , sHP(_sHP)
			, wLevel(_wLevel)
		{
			dwSize = sizeof(SNET_PARTY_MBR_UPDATE);
			nType = NET_MSG_GCTRL_PARTY_MBR_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MBR_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_PARTY_MBR_PICKUP_BRD : public NET_PARTY_MESSAGE
    {
		DWORD dwGaeaID;
        SNATIVEID	sNID_ITEM;

		SNETPC_PARTY_MBR_PICKUP_BRD(const GLPartyID& PartyID)
			: NET_PARTY_MESSAGE(PartyID)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PARTY_MBR_PICKUP_BRD)<=NET_DATA_BUFSIZE);
		}
	};

    //---------------------------------------------------------------------------NET
    struct SNETPC_PARTY_MBR_PICKUP_LINK_BRD : public NET_PARTY_MESSAGE
    {
		DWORD dwGaeaID;
        SITEMCUSTOM		sItem;

        SNETPC_PARTY_MBR_PICKUP_LINK_BRD(const GLPartyID& PartyID)
			: NET_PARTY_MESSAGE(PartyID)
        {
            dwSize = sizeof(*this);
            nType = NET_MSG_GCTRL_PARTY_MBR_PICKUP_LINK_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPC_PARTY_MBR_PICKUP_LINK_BRD)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------NET
	struct SNET_PARTY_BRD : public SNETPC_BROAD
	{
		GLPartyID PartyID;
		DWORD dwPMasterID;
		
		SNET_PARTY_BRD () :			
			dwPMasterID(GAEAID_NULL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_PARTY_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PARTY_MBR_PICKUP_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CONFT_SPTY_EXP_FLD : public NET_MSG_GENERIC
	{
		WORD				wSCHOOL;
		float				fEXP_RATE;

		SNET_CONFT_SPTY_EXP_FLD () :
			wSCHOOL(0),
			fEXP_RATE(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_CONFRONTSPTY_EXP_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_CONFT_SPTY_EXP_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_MEMBER_RENAME : public NET_PARTY_MESSAGE
	{
		DWORD dwGaeaID;
		char  szName[CHAR_SZNAME];

        SNET_PARTY_MEMBER_RENAME(const GLPartyID& _PartyID, const DWORD _dwGaeaID)
            : NET_PARTY_MESSAGE(_PartyID)
            , dwGaeaID(_dwGaeaID)
		{
			memset ( szName, 0, CHAR_SZNAME );
			dwSize = sizeof(SNET_PARTY_MEMBER_RENAME);
			nType = NET_MSG_GCTRL_PARTY_MBR_RENAME;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_MEMBER_RENAME)<=NET_DATA_BUFSIZE);
		}

        void SetName(const char* Name)
        {
            if (Name)
                StringCchCopy(szName, CHAR_SZNAME, Name);
        }
	};

	struct SNET_PARTY_LINK_JOIN : public NET_MSG_GENERIC
	{
		DWORD				dwMasterID;

		SNET_PARTY_LINK_JOIN ( DWORD _dwMasterID ) 
			: dwMasterID(_dwMasterID)
		{
			dwSize = sizeof(SNET_PARTY_LINK_JOIN);
			nType = NET_MSG_GCTRL_PARTY_LINK_JOIN;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_JOIN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_LINK_JOIN_FB : public NET_MSG_GENERIC
	{
		EMPARTY_LINK_JOIN_FB	emFB;

		SNET_PARTY_LINK_JOIN_FB () 
			: emFB(EMPARTY_LINK_JOIN_FB_FAIL)
		{
			dwSize = sizeof(SNET_PARTY_LINK_JOIN);
			nType = NET_MSG_GCTRL_PARTY_LINK_JOIN_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_JOIN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_LINK_JOIN_ANS : public NET_MSG_GENERIC
	{
		DWORD					dwGaeaID;
		DWORD					dwHashKey;		
		DWORD					dwTimeKey;

		EMCHARCLASS			emClass;
		CHAR						szName[CHAR_SZNAME];

			
        SNET_PARTY_LINK_JOIN_ANS(const DWORD _dwHashKey, const DWORD _dwGaeaID, const DWORD _dwTimeKey, EMCHARCLASS _emClass ) 
			: dwHashKey(_dwHashKey)
			, dwGaeaID(_dwGaeaID)
			, dwTimeKey(_dwTimeKey)
			, emClass( _emClass )
		{
			memset ( szName, 0, CHAR_SZNAME );
			dwSize = sizeof(SNET_PARTY_LINK_JOIN_ANS);
			nType = NET_MSG_GCTRL_PARTY_LINK_JOIN_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_JOIN_ANS)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_PARTY_LINK_LURE : public NET_MSG_GENERIC
	{
		DWORD					dwHashKey;
		DWORD					dwGaeaID;
		DWORD					dwTimeKey;

		SPartyOption			sOption;

        SNET_PARTY_LINK_LURE(const DWORD _dwHashKey, const DWORD _dwGaeaID, const DWORD _dwTimeKey, SPartyOption _sOption ) 
			: dwHashKey(_dwHashKey)
			, dwGaeaID(_dwGaeaID)
			, dwTimeKey(_dwTimeKey)
            , sOption(_sOption)
		{
			dwSize = sizeof(SNET_PARTY_LINK_LURE);
			nType = NET_MSG_GCTRL_PARTY_LINK_LURE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_LURE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_LINK_LURE_FB : public NET_MSG_GENERIC
	{
		EMPARTY_LINK_LURE_FB	emFB;

			
		SNET_PARTY_LINK_LURE_FB () 
			: emFB(EMPARTY_LINK_LURE_FB_FAIL)
		{
			dwSize = sizeof(SNET_PARTY_LINK_LURE_FB);
			nType = NET_MSG_GCTRL_PARTY_LINK_LURE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_LURE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_LINK_REFUSE : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;

		SNET_PARTY_LINK_REFUSE ( DWORD _dwGaeaID ) 
			: dwGaeaID(_dwGaeaID)
		{
			dwSize = sizeof(SNET_PARTY_LINK_REFUSE);
			nType = NET_MSG_GCTRL_PARTY_LINK_REFUSE;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_REFUSE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_LINK_REFUSE_FB : public NET_MSG_GENERIC
	{
		CHAR				szName[CHAR_SZNAME];

		SNET_PARTY_LINK_REFUSE_FB () 			
		{
			memset ( szName, 0, CHAR_SZNAME );
			dwSize = sizeof(SNET_PARTY_LINK_REFUSE_FB);
			nType = NET_MSG_GCTRL_PARTY_LINK_REFUSE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_LINK_REFUSE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_NEW_CA : public NET_MSG_GENERIC
	{
		GLPartyID PartyID;

		SNET_EXPEDITION_NEW_CA(const GLPartyID& _PartyID)
			: PartyID(_PartyID)
		{			
			dwSize = sizeof(SNET_EXPEDITION_NEW_CA);
			nType = NET_MSG_GCTRL_EXPEDITION_FNEW_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_EXPEDITION_NEW_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_NEW_FB : public NET_MSG_GENERIC
	{		
		GLPartyID PartyIDs[MAX_PARTYGROUP];
		GLPARTY_FNET MasterInfo;
		DWORD dwMasterGaeaID;
		DWORD dwMyPartyIndex;
		SNET_EXPEDITION_NEW_FB()
			: dwMyPartyIndex(0)
		{			
			dwSize = sizeof(SNET_EXPEDITION_NEW_FB);
			nType = NET_MSG_GCTRL_EXPEDITION_FNEW_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_EXPEDITION_NEW_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_ERROR : public NET_MSG_GENERIC
	{
		EMEXPEDITION_ERROR emError;

		SNET_EXPEDITION_ERROR()
			: emError( EMEXPEDITION_ERROR_NONE )
		{			
			dwSize = sizeof( SNET_EXPEDITION_ERROR );
			nType = NET_MSG_GCTRL_EXPEDITION_ERROR;
			MIN_STATIC_ASSERT( sizeof( SNET_EXPEDITION_ERROR ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_PARTY_CHANGE_OPTION : public NET_MSG_GENERIC
	{
		DWORD dwMasterGaeaID;
		GLPartyID PartyID;
		SPartyOption PartyOption;

		SNET_PARTY_CHANGE_OPTION(const DWORD _dwMasterGaeaID, const GLPartyID& _PartyID, const SPartyOption& _PartyOption)
			: dwMasterGaeaID(_dwMasterGaeaID)
			, PartyID(_PartyID)
			, PartyOption(_PartyOption)
		{			
			dwSize = sizeof(SNET_PARTY_CHANGE_OPTION);
			nType = NET_MSG_GCTRL_PARTY_CHANGE_OPTION;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_CHANGE_OPTION)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_ADD_TENDER_ITEM : public NET_MSG_GENERIC
	{
		const MapID mapID;
		const SITEMCUSTOM ItemCustom;
		const D3DXVECTOR3 vGeneratePosition;
		const DWORD dwItemID; // 아이템의 실제 아이디가 아닌 GLPartyTenderItemManager에서 관리하는 ID;
		const float fInsertTime;

		SNET_PARTY_ADD_TENDER_ITEM(const GLPartyID& _PartyID, const MapID& _mapID, const SITEMCUSTOM& _ItemCustom, const DWORD _dwItemID, const D3DXVECTOR3& _vGeneratePosition, const float _fInsertTime)
			: mapID(_mapID)
			, ItemCustom(_ItemCustom)
			, vGeneratePosition(_vGeneratePosition)
			, dwItemID(_dwItemID)
			, fInsertTime(_fInsertTime)
		{
			dwSize = sizeof(SNET_PARTY_ADD_TENDER_ITEM);
			nType = NET_MSG_GCTRL_PARTY_ADD_TENDER_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_ADD_TENDER_ITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_TENDER_ITEM : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwGaeaID;		
		DWORD nItem;
		DWORD dwItemIDs[MAX_TENDERABLE_ITEM]; // 아이템의 실제 ID가 아닌 입찰 메니저에서 관리하는 별도의 ID;

		void AddItem(const DWORD dwItemID)
		{
			if ( !(nItem < MAX_TENDERABLE_ITEM) )
				return;

			dwItemIDs[nItem++] = dwItemID;
			dwSize += sizeof(DWORD);
		}

		SNET_PARTY_TENDER_ITEM(const GLPartyID& _PartyID, const DWORD _dwGaeaID)
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, nItem(0)
		{
			dwSize = sizeof(SNET_PARTY_TENDER_ITEM) - sizeof(DWORD) * MAX_TENDERABLE_ITEM;
			nType = NET_MSG_GCTRL_PARTY_TENDER_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_TENDER_ITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_TENDER_ITEM_FB : NET_MSG_GENERIC
	{
		struct SDiceResult
		{
			DWORD dwGaeaID;
			BYTE btResult;
		};

		const GLPartyID PartyID;
		const DWORD dwItemID;
		char cItemName[ITEM_SZNAME];
		DWORD dwOwnerGaeaID;		
		DWORD nMember;	
		SDiceResult sDiceResult[MAXPARTY * MAX_PARTYGROUP];

		void AddMember(const DWORD dwGaeaID, const BYTE btResult)
		{
			if ( !(nMember < MAXPARTY * MAX_PARTYGROUP) )
				return;

			sDiceResult[nMember].dwGaeaID = dwGaeaID;
			sDiceResult[nMember++].btResult = btResult;

			dwSize += sizeof(SDiceResult);
		}

		SNET_PARTY_TENDER_ITEM_FB(const GLPartyID& _PartyID, const DWORD _dwItemID, const char* const _cItemName)
			: PartyID(_PartyID)
			, dwItemID(_dwItemID)
			, nMember(0)
			, dwOwnerGaeaID(GAEAID_NULL)
		{
			StringCchCopy(cItemName, ITEM_SZNAME + 1, _cItemName);
			dwSize = sizeof(SNET_PARTY_TENDER_ITEM_FB) - sizeof(SDiceResult) * MAXPARTY * MAX_PARTYGROUP;
			nType = NET_MSG_GCTRL_PARTY_TENDER_ITEM_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_TENDER_ITEM_FB)<=NET_DATA_BUFSIZE);
		}			
	};

	struct SNET_PARTY_JUNK_ITEM : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwGaeaID;
		const DWORD dwItemID;
		char cItemName[ITEM_SZNAME];

		void SetName(const char* _cItemName)
		{
			StringCchCopy(cItemName, ITEM_SZNAME + 1, _cItemName);
		}		

		SNET_PARTY_JUNK_ITEM(const GLPartyID& _PartyID, const DWORD _dwGaeaID, const DWORD _dwItemID)
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, dwItemID(_dwItemID)
		{
			dwSize = sizeof(SNET_PARTY_JUNK_ITEM);
			nType = NET_MSG_GCTRL_PARTY_JUNK_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_JUNK_ITEM) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_TRANSFER_ITEM : public NET_MSG_GENERIC
	{
		struct MemberInfo
		{			
			DWORD dwGaeaID;
			DWORD dwPartyID;
		};
		const GLPartyID PartyID;
		const DWORD dwGaeaID;
		const DWORD dwItemID;
		DWORD nMember;
		MemberInfo MemberInfos[MAXPARTY * MAX_PARTYGROUP];

		void AddMember(const DWORD dwGaeaID, const DWORD dwPartyID)
		{
			if ( !(nMember < MAXPARTY * MAX_PARTYGROUP) )
				return;

			MemberInfos[nMember].dwGaeaID = dwGaeaID;
			MemberInfos[nMember++].dwPartyID = dwPartyID;

			dwSize += sizeof(MemberInfo);
		}

		SNET_PARTY_TRANSFER_ITEM(const GLPartyID& _PartyID, const DWORD _dwGaeaID, const DWORD _dwItemID)
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, dwItemID(_dwItemID)
			, nMember(0)
		{
			dwSize = sizeof(SNET_PARTY_TRANSFER_ITEM) - sizeof(MemberInfo) * MAXPARTY * MAX_PARTYGROUP;
			nType = NET_MSG_GCTRL_PARTY_TRANSFER_ITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_TRANSFER_ITEM) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_CHECK_READY_REQUEST_CA : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwMasterGaeaID;
		
		SNET_EXPEDITION_CHECK_READY_REQUEST_CA(const GLPartyID& _PartyID, const DWORD _dwMasterGaeaID)
			: PartyID(_PartyID)
			, dwMasterGaeaID(_dwMasterGaeaID)
		{
			dwSize = sizeof(SNET_EXPEDITION_CHECK_READY_REQUEST_CA);
			nType = NET_MSG_GCTRL_EXPEDITION_CHECK_READY_REQUEST_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_EXPEDITION_CHECK_READY_REQUEST_CA) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_CHECK_READY_ANSWER_CA : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwMasterGaeaID;
		const DWORD dwPartySlot; // 몇번째 파티인지;
		const DWORD dwGaeaID;
		const bool bAnswer;

		SNET_EXPEDITION_CHECK_READY_ANSWER_CA(const GLPartyID& _PartyID, const DWORD _dwMasterGaeaID, const DWORD _dwGaeaID, const DWORD dwPartySlot = 0, const bool _bAnswer = false)
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, dwMasterGaeaID(_dwMasterGaeaID)
			, dwPartySlot(dwPartySlot)
			, bAnswer(_bAnswer)
		{
			dwSize = sizeof(SNET_EXPEDITION_CHECK_READY_ANSWER_CA);
			nType = NET_MSG_GCTRL_EXPEDITION_CHECK_READY_ANSWER_CA;
			MIN_STATIC_ASSERT(sizeof(SNET_EXPEDITION_CHECK_READY_ANSWER_CA) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_EXPEDITION_SWAP_SLOT : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwMasterGaeaID; // Master Client -> Agent -> FieldServer/PartyClient;
		const GLSwapSlotInformation SwapSlotFrom;
		const GLSwapSlotInformation SwapSlotTo;

		SNET_EXPEDITION_SWAP_SLOT(const GLPartyID& _PartyID, const DWORD _dwMasterGaeaID
			, const GLSwapSlotInformation& _SwapSlotFrom
			, const GLSwapSlotInformation& _SwapSlotTo)
			: PartyID(_PartyID)
			, dwMasterGaeaID(_dwMasterGaeaID)
			, SwapSlotFrom(_SwapSlotFrom)
			, SwapSlotTo(_SwapSlotTo)
		{
			dwSize = sizeof(SNET_EXPEDITION_SWAP_SLOT);
			nType = NET_MSG_GCTRL_EXPEDITION_SWAP_SLOT;
			MIN_STATIC_ASSERT(sizeof(SNET_EXPEDITION_SWAP_SLOT) <= NET_DATA_BUFSIZE);
		}
	};	

	struct SNET_QBOX_OPTION_MEMBER : public NET_PARTY_MESSAGE
	{
		const bool bQBoxEnable;

		SNET_QBOX_OPTION_MEMBER(const GLPartyID& _PartyID, const bool _bEnable)
			: NET_PARTY_MESSAGE(_PartyID)
			,  bQBoxEnable(_bEnable) 
		{
			dwSize = sizeof(SNET_QBOX_OPTION_MEMBER);
			nType = NET_QBOX_OPTION_MEMBER;
			MIN_STATIC_ASSERT(sizeof(SNET_QBOX_OPTION_MEMBER)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_REFRESH_REQ : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwMyGaeaID;
		EMPARTY_REFRESH_REQ emPartyRefreshRequest;

		SNET_PARTY_REFRESH_REQ(const GLPartyID& _PartyID, const DWORD _dwMyGaeaID)
			: PartyID(_PartyID)
			, dwMyGaeaID(_dwMyGaeaID)
			, emPartyRefreshRequest(EMPARTY_REFRESH_REQ_MEMBER_INFORMATION)
		{
			dwSize = sizeof(SNET_PARTY_REFRESH_REQ);
			nType = NET_MSG_GCTRL_PARTY_REFRESH_REQ;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_REFRESH_REQ) <= NET_DATA_BUFSIZE);
		}
	};

	struct SNET_PARTY_ERROR_TO_CLIENT : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		EMPARTY_ERROR_TYPE emPartyErrorCode;
		char szCharName[CHAR_SZNAME];

		SNET_PARTY_ERROR_TO_CLIENT(const GLPartyID& _PartyID)
			: PartyID(_PartyID)
		{
			dwSize = sizeof(SNET_PARTY_ERROR_TO_CLIENT) - sizeof(szCharName);
			nType = NET_MSG_GCTRL_PARTY_ERROR;
			MIN_STATIC_ASSERT(sizeof(SNET_PARTY_ERROR_TO_CLIENT) <= NET_DATA_BUFSIZE);
		}

		void addCharName(const char* const _szCharName)
		{
			StringCchCopy(szCharName, CHAR_SZNAME, _szCharName);
			dwSize = sizeof(SNET_PARTY_ERROR_TO_CLIENT);
		}
	};

	// Revert to default structure packing
	#pragma pack()
};