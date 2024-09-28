#ifndef _GL_ITEM_LIMIT_DEFINE_H_
#define _GL_ITEM_LIMIT_DEFINE_H_

#pragma once

namespace gamelog
{
    enum EMIDTYPE
    {
        ID_NONE         = 0,
        ID_USER			= 1, //! 유저
        ID_CHAR			= 2, //! 캐릭터
        ID_CLUB			= 3, //! 클럽
        ID_MOB			= 4, //! 몬스터
		ID_SCHOOL		= 5, //! 학원
    };

    //! 행동 로그
    enum EMACTION
    {
        ACT_DIE				   = 1,
        ACT_KILL			   = 2,
        ACT_HOSTILE			   = 3,
        ACT_LEV_UP			   = 4,
        ACT_RESTORE			   = 5,

        ACT_CLUB_MAKE		   = 11,
        ACT_CLUB_DELETE		   = 12,
        ACT_CLUB_LEV_UP		   = 13,
        ACT_CLUB_JOIN_MEMBER   = 14,
        ACT_CLUB_SECEDE_MEMBER = 15,
        ACT_CLUB_KICK_MEMBER   = 16,	
        ACT_CLUB_DEPUTY_MEMBER = 17,

        ACT_RECOVERY		   = 18,
        ACT_RECOVERY_NPC	   = 19,
        ACT_PET_SKILL		   = 20,

        ACT_FRIEND_ADD		   = 21,
        ACT_FRIEND_DEL		   = 22,
        ACT_FRIEND_BLOCK	   = 23,

        ACT_CLUB_AUTHORITY	   = 24,
        ACT_CLUB_COMMISSION_CHANGE = 25, //! 수수료 변경
        ACT_CLUB_GUIDANCE = 26, //! 선도클럽이 됨
		ACT_CLUB_AUTO_JOIN_MEMBER   = 27,
		ACT_JUMPING_LEV_UP		   = 28,
		ACT_CHANGE_SCHOOL		= 29,
		ACT_EXP_COMPRESSOR		= 30, // 경험치 압축 사용
    };
} // namespace log

//! Item 생성 타입
enum EMITEMGEN
{
    EMGEN_DEFAULT	= 0,
    EMGEN_INIT		= 1,
    EMGEN_SHOP		= 2,
    EMGEN_NPC		= 3,
    EMGEN_QUEST		= 4,
    EMGEN_MOB		= 5,
    EMGEN_GMEDIT	= 6,
    EMGEN_OLD		= 7,
    EMGEN_BILLING	= 8,
    EMGEN_GMEDIT2   = 9,
    EMGEN_ITEMMIX	= 10,
    EMGEN_GATHERING	= 11,
    EMGEN_SYSTEM	= 12,
    EMGEN_POINTSHOP = 13, //! Point Shop
    EMGEN_CHEAT     = 14,
    EMGEN_REBUY     = 15, // Rebuy Item.
	EMGEN_DECOMPOSE	= 16, // 분해;

    EMGEN_ALL		= -1,
};

enum
{
	EMMONEY_LOG				= 10000,			// 로그를 남길만한 금액변화.
	EMCLUBINCOMEMONEY_LOG	= 100000000,	// 클럽 수입 로그
};

//! 아이템의 교환타입
enum EMITEM_ROUTE
{
	ITEM_ROUTE_CHAR		    =  0, //! 캐릭터간 거래
	ITEM_ROUTE_GROUND		=  1, //! 바닥에 떨어지거나 바닥에서 골드를 입수하였다.
	ITEM_ROUTE_SHOP		    =  2, //! 상점에 판매,구입
	ITEM_ROUTE_USER_INVEN	=  3, //! 사용자 락커에 넣기, 빼기
	ITEM_ROUTE_DELETE		=  4, //! 아이템 삭제
	ITEM_ROUTE_CLUB		    =  5, //! 클럽 락커에 넣기, 빼기
	MONEY_ROUTE_CLUB_INCOME	=  6, //! 선도클럽 세금 수입
	MONEY_ROUTE_NPC_REMAKE  =  7, //! 돈을 지불하고 NPC 에게서 아이템 랜덤수치 개조
	ITEM_ROUTE_PET_CARD	    =  8, //! 팻카드의 사용여부
	ITEM_ROUTE_ODD_EVEN	    =  9, //! 홀짝게임
	ITEM_ROUTE_VEHICLE	    = 10, //! 보드 생성 로그
	ITEM_ROUTE_VNINVEN	    = 11, //! 베트남 탐닉 방지 인벤토리에서 돈을 꺼냈을 경우
	ITEM_ROUTE_GARBAGE	    = 12, //! 휴지통으로 아이템 삭제시
	ITEM_ROUTE_NPCCOME	    = 13, //! NPC 소환으로 생긴 수수료
	ITEM_ROUTE_ITEM_MIX	    = 14, //! 아이템 조합
	ITEM_ROUTE_GATHERING	= 15, //! 채집
	ITEM_ROUTE_SYSTEM		= 16, //! 시스템으로 일어난 로그
	ITEM_ROUTE_PK_DROP		= 17, //! PK로 인한 아이템 드랍
    MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY = 18, //! Char Inven Total Money
    MONEY_ROUTE_USER_INVEN_TOTAL_MONEY = 19, //! User Inven Total Money
    MONEY_ROUTE_CLUB_TOTAL_MONEY       = 20, //! Club Total Money
    MONEY_RECOVERY_CONSUME             = 21, //! 캐릭터 부활에 사용된 게임머니
    MONEY_USE_ITEM_REPAIR              = 22, //! 아이템 수리에 상용된 게임머니
	MONEY_ROUTE_BUY_TEXASHOLDEM_CHIP   = 23, //! 텍사스홀덤 칩구매
    ITEM_ROUTE_CONSIGMENT_SALE         = 24, //! 아이템을 위탁판매 하였다.
    ITEM_ROUTE_CONSIGMENT_SENDBACK     = 25, //! 아이템을 위탁판매 돌려받음( 판매 금액 + 아이템 ).
	ITEM_ROUTE_POST			= 26, //! 우편
	ITEM_ROUTE_EXCHANGE		= 27, //! NPC 아이템 교환
	ITEM_ROUTE_DECOMPOSE	= 28, //! 분해로 인한 아이템 생성;
	ITEM_ROUTE_COSTUME_STATE_INSERT = 29, // 코스튬에 능력치를 적용
	ITEM_ROUTE_COSTUME_STATE_ERASE = 30, // 코스튬에 능력치를 적용
	ITEM_ROUTE_COSTUME_STATE_REINSERT = 31, 
	ITEM_ROUTE_NORMAL_GRINDING = 32, // 일반 연마
	ITEM_ROUTE_ANTI_DISAPPEAR_GRINDING = 33, // 소방주 사용 연마 // 사용 금지 ITEM_ROUTE_NORMAL_GRINDING 로 통일하고 상세 정보로 구분
	ITEM_ROUTE_INC_GRINDINGRATE_GRINDING = 34, // 강화주 사용 연마 // 사용 금지 ITEM_ROUTE_NORMAL_GRINDING 로 통일하고 상세 정보로 구분
	ITEM_ROUTE_ANTI_RESET_GRINDING = 35, // 완화주 사용 연마 // 사용 금지 ITEM_ROUTE_NORMAL_GRINDING 로 통일하고 상세 정보로 구분
	ITEM_ROUTE_COSTUME_STATE_DELETE = 36,
	ITEM_ROUTE_TEMP_ENCHANT			= 37,
	ITEM_ROUTE_TEMP_ENCHANT_ERASE   = 38, // 사용 금지 ITEM_ROUTE_TEMP_ENCHANT 로 통일하고 상세 정보로 구분
	MONEY_ROUTE_BUY_LOTTOSYSTEM_TICKET = 39, //! 로또시스템 티켓 구매
	MONEY_ROUTE_RESTORE_LOTTOSYSTEM_TICKET = 40, //! 로또시스템 티켓 구매 실패 복구
	MONEY_ROUTE_WIN_LOTTOSYSTEM = 41, //! 로또시스템 당첨금액 지급
    ITEM_ROUTE_SKILL_RESET =  42, //! 망각의 약을 사용해서 게임머니를 돌려받았다.
	ITEM_ROUTE_CONSIGMENT_SALE_SERVERGROUP_DIFF = 43, //! 아이템을 위탁판매 하였다.(판매자, 구매자 서버다름)
};
enum EMITEM_ROUTE_DETAIL
{
	ITEM_ROUTE_DETAIL_UNKNOWN							= 0,		// 
	LOG_RANPOINT_INFO_ITEM_PRICE_BY_CONSIGEMENT			= 1000,		// 위탁판매에 아이템을 등록했으며, 판매가격은(1개당 포인트) 얼마이다.
	LOG_RANPOINT_INFO_POST_BY_CONSIGEMENT				= 1001,		// 
	LOG_RANPOINT_PLUS_BY_CONSIGEMENT					= 1002,		// 
	LOG_RANPOINT_MINUS_BY_CONSIGEMENT					= 1003,		// 위탁판매에 등록된 아이템을 구매하고 구매비용은(포인트) 얼마이다.
	LOG_RANPOINT_MINUS_FEE_BY_CONSIGEMENT				= 1004,		// 위탁판매에 아이템을 등록했으며, 등록 수수료는(포인트) 얼마이다.
	LOG_RANPOINT_MINUS_BY_POINTSHOP						= 1005,		// 
	LOG_MONEY_CONSIGNMENT_SALE_GAMEMONEY_SELL			= 2000,		// 위탁판매에 게임머니를 판매 등록하면서 게임머니를 소모하였다.
	LOG_MONEY_INFO_POST_BY_CONSIGEMENT					= 2001,		// 
	LOG_MONEY_CURRENT_INVEN_BY_CONSIGEMENT				= 2002,		// 위탁판매 관련 현재 인벤토리 게임머니
	LOG_MONEY_MINUS_FEE_BY_CONSIGEMENT					= 2003,		//
	LOG_MONEY_PLUS_BY_CONSIGEMENT						= 2004,		//
	LOG_MONEY_MINUS_BY_CONSIGEMENT						= 2005,		//
	LOG_MONEY_TRADE										= 2006,		// 
	LOG_MONEY_SELL_TO_NPC								= 2007,		//
	LOG_MONEY_PRIVATE_MARKET_BUY_ITEM					= 2008,		//
	LOG_MONEY_ADD_FROM_PARTY							= 2009,		//
	LOG_MONEY_ADD_FROM_LAND								= 2010,		//
	LOG_MONEY_DROP_INVNE_TO_FIELD						= 2011,		//
	LOG_MONEY_LEARN_SKILL								= 2012,		//
	LOG_MONEY_MOVE_STORAGE_TO_INVEN						= 2013,		//
	LOG_MONEY_MOVE_INVEN_TO_STORAGE						= 2014,		//
	LOG_MONEY_CREATE_CLUB_CHARGE						= 2015,		//
	LOG_MONEY_BUY_FROM_NPC								= 2016,		//
	LOG_MONEY_REBUY										= 2017,		// 
	LOG_MONEY_GARBAGE									= 2018,		//
	LOG_MONEY_QUEST_START								= 2019,		//
	LOG_MONEY_CLUB_RANK_UPGREAD							= 2020,		//
	LOG_MONEY_DECOMPOSE									= 2021,		//
	LOG_MONEY_CLUB_STORAGE_DRAW							= 2022,		//
	LOG_MONEY_CLUB_STORAGE_SAVE							= 2023,		//
	LOG_MONEY_CLUB_GUIDANCE_COMMISSION					= 2024,		//
	LOG_MONEY_REMODEL_CHARGE							= 2025,		//
	LOG_MONEY_ODD_EVEN_CHARGE							= 2026,		//
	LOG_MONEY_NPC_COMMISSION							= 2027,		//
	LOG_MONEY_MIX_CHARGE								= 2028,		//
	LOG_MONEY_RESURRECTION								= 2029,		//
	LOG_MONEY_ITEM_REPAIR								= 2030,		//
	LOG_MONEY_TEXASHOLDEM_BUY							= 2031,		//
	LOG_MONEY_POST_ATTACHEDMONEY						= 2032,		// 
	LOG_MONEY_POST_CHARGEDMONEY							= 2033,		// 
	LOG_MONEY_CHAR_INVEN_CURRENT						= 2034,		// 상황에 맞게 추가 변환필요
	LOG_MONEY_CHAR_STORAGE_CURRENT						= 2035,		// 상황에 맞게 추가 변환필요
	LOG_MONEY_CLUB_STORAGE_CURRENT						= 2036,		// 상황에 맞게 추가 변환필요
	LOG_MONEY_RANMOBILE_CONNECTION						= 2037,		//
	LOG_MONEY_RANMOBILE_EXTENDTIME						= 2038,		//
	LOG_MONEY_LOTTOSYSTEM_TICKET_BUY					= 2039,		//
	LOG_MONEY_LOTTOSYSTEM_TICKET_RESTORE				= 2040,		//
	LOG_MONEY_LOTTOSYSTEM_WIN							= 2041,		//
	LOG_MONEY_SKILL_RESET								= 2042,		// 
	LOG_ITEM_INFO_REGIST_ITEM_BY_CONSIGEMENT			= 3000,		// 위탁판매에 아이템을 등록하였다.
	LOG_ITEM_ADD_ITEM_BY_POINTSHOP						= 3001,		// 포인트샵에서 아이템을 구매했다.
	LOG_ITEM_DEL_POINT_CARD								= 3002,		//
	LOG_ITEM_CHAR_STORAGE_CURRENT						= 3003,		//
	LOG_ITEM_ADD_ITEM_BY_PREMIUMSET						= 3004,		//
	LOG_ITEM_CHAR_INVEN_CURRENT							= 3005,		//
	LOG_ITEM_CHAR_PUTON_CURRENT							= 3006,		//
	LOG_ITEM_CHAR_SLOT_CURRENT							= 3007,		//
	LOG_ITEM_TRADE										= 3008,		// 
	LOG_ITEM_RESTORE_CUSTOME_BY_TIMEOUT					= 3009,		//
	LOG_ITEM_CLUB_STORAGE_MOVE_CLUB_TO_CHAR				= 3010,		//
	LOG_ITEM_ADD_ITEM_TO_INVEN							= 3011,		//
	LOG_ITEM_ROUTING_ITEM_TO_INVEN						= 3012,		//
	LOG_ITEM_PICKUP_FIELD_ITEM							= 3013,		//
	LOG_ITEM_EXCHANGE_STORAGE_TO_INVEN					= 3014,		//
	LOG_ITEM_CHANGE_INVEN_TO_INVEN						= 3015,		//
	LOG_ITEM_MOVE_STORAGE_TO_INVEN						= 3016,		//
	LOG_ITEM_BUY_ITEM_FROM_NPC							= 3017,		//
	LOG_ITEM_ADD_ITEM_BY_BOX							= 3018,		//
	LOG_ITEM_ADD_ITEM_BY_LOCKED_BOX						= 3019,		//
	LOG_ITEM_ADD_ITEM_BY_DISJUNCTION					= 3020,		//
	LOG_ITEM_ADD_ITEM_BY_SKILL_STAT_RESET				= 3021,		//
	LOG_ITEM_PRIVATE_MARKET_BUY_ITEM					= 3022,		//
	LOG_ITEM_ADD_ITEM_BY_QUEST_REWARD					= 3023,		//
	LOG_ITEM_ADD_ITEM_BY_GM								= 3024,		//
	LOG_ITEM_ADD_ITEM_BY_PURCHASE						= 3025,		//
	LOG_ITEM_ROUTING_FROM_FIELD							= 3026,		// 오타 수정 필요
	LOG_ITEM_DROP_INVEN_TO_FIELD						= 3027,		//
	LOG_ITEM_DROP_SLOT_TO_FIELD							= 3028,		//
	LOG_ITEM_DROP_COSTUME_TO_FIELD_BY_ERROR				= 3029,		//
	LOG_ITEM_DEL_ITEM_BY_SELL_TO_NPC					= 3030,		//
	LOG_ITEM_REBUY										= 3031,		// 
	LOG_ITEM_MOVE_INVEN_TO_STORAGE						= 3032,		//
	LOG_ITEM_EXCHANGE_INVEN_WITH_STORAGE				= 3033,		//
	LOG_ITEM_DEL_ITEM_BY_EXCHANGE_INVEN_WITH_STORAGE	= 3034,		// 
	LOG_ITEM_DEL_ITEM_BY_TIMEOUT						= 3035,		//
	LOG_ITEM_DEL_ITEM_BY_GUIDANCE_CERTIFICATION			= 3036,		//
	LOG_ITEM_DEL_ITEM_BY_INVEN_USE						= 3037,		//
	LOG_ITEM_DEL_INSTANCE_ITEM_BY_SLOT_USE				= 3038,		//
	LOG_ITEM_DEL_ITEM_BY_SLOT_USE						= 3039,		//
	LOG_ITEM_DEL_ITEM_BY_NPC_EXCHANGE					= 3040,		//
	LOG_ITEM_DEL_BOX									= 3041,		// 
	LOG_ITEM_DEL_RANDOMBOX								= 3042,		//
	LOG_ITEM_ADD_ITEM_BY_RANDOMBOX						= 3043,		//
	LOG_ITEM_DEL_SELECTBOX								= 3044,		//
	LOG_ITEM_DEL_COSTUME_ON_ITEM						= 3045,		//
	LOG_ITEM_DEL_CLEANSER_BY_USE						= 3046,		//
	LOG_ITEM_DEL_COSTUME_BY_CLEANSER					= 3047,		//
	LOG_ITEM_DEL_LOCKED_BOX								= 3048,		//
	LOG_ITEM_DEL_LOCKED_BOX_FAIL						= 3049,		//
	LOG_ITEM_DEL_WRAPPING								= 3050,		//
	LOG_ITEM_DEL_REMODEL_FAIL							= 3051,		//
	LOG_ITEM_DEL_COSTUME_STATE							= 3052,		//
	LOG_ITEM_DEL_ITEM_BY_NPC_TRADE						= 3053,		//
	LOG_ITEM_DEL_STORAGE_ITEM							= 3054,		//
	LOG_ITEM_DEL_INVEN_ITEM								= 3055,		//
	LOG_ITEM_DEL_PET_REVIVE								= 3056,		//
	LOG_ITEM_DEL_DECOMPOSE								= 3057,		//
	LOG_ITEM_DEL_ITEM_BY_TEXASHOLDEM					= 3058,		//
	LOG_ITEM_DEL_ITEM_BY_VALIDATION						= 3059,		//
	LOG_ITEM_CLUB_STORAGE_EXCHANGE_CHAR_TO_CLUB			= 3060,		//
	LOG_ITEM_CLUB_STORAGE_EXCHANGE_CLUB_TO_CHAR			= 3061,		//
	LOG_ITEM_CLUB_STORAGE_ADD_CHAR_TO_CLUB				= 3062,		//
	LOG_ITEM_REMODEL_RESULT								= 3063,		//
	LOG_ITEM_PETCARD_CREATE_PET							= 3064,		//
	LOG_ITEM_VEHICLE_CREATE_VEHICLE						= 3065,		//
	LOG_ITEM_DEL_ITEM_TO_GARBAGE						= 3066,		//
	LOG_ITEM_MIX_MIX_RESULT								= 3067,		//
	LOG_ITEM_MIX_DEL_ITEM								= 3068,		// 
	LOG_ITEM_ADD_ITEM_BY_GATHERING						= 3069,		// 
	LOG_ITEM_ADD_ITEM_BY_LUA							= 3070,		// 
	LOG_ITEM_DEL_ITEM_BY_LUA							= 3071,		// 
	LOG_ITEM_CDM_REWARD_TO_CHAR_INVEN					= 3072,		// 
	LOG_ITEM_UNWRAPPING									= 3073,		// 
	LOG_ITEM_WRAPPING_BY_ITEM							= 3074,		// 
	LOG_ITEM_COUSUME_ADD_BY_ITEM						= 3075,		// 
	LOG_ITEM_COSTUME_DEL_BY_DISJUNCTION					= 3076,		// 
	LOG_ITEM_COSTUME_DEL_BY_CLEANSER					= 3077,		// 
	LOG_ITEM_COSTUME_DEL_BY_TIMEOUT						= 3078,		// 
	LOG_ITEM_DROP_ITEM_BY_PK							= 3079,		// 
	LOG_ITEM_CONSIGEMENT_SENDBACK						= 3080,		// 
	LOG_ITEM_CONSIGEMENT_BUY_RECV_BY_POST				= 3081,		// 
	LOG_ITEM_POST_ATTACHEDITEM							= 3082,		// 
	LOG_ITEM_GET_ITEM_BY_NPC							= 3083,		// 
	LOG_ITEM_ADD_ITEM_BY_DECOMPOSE						= 3084,		// 
	LOG_ITEM_COSTUME_STATE_NEW							= 3085,		// 
	LOG_ITEM_COSTUME_STATE_OVERWRITE					= 3086,		// 
	LOG_ITEM_COSTUME_STATE_DEL_BY_TIMEOUT				= 3087,		// 
	LOG_ITEM_COSTUME_STATE_DEL							= 3088,		// 
	LOG_ITEM_NORMAL_GRINDING							= 3089,		// 
	LOG_ITEM_ANTI_DISAPPEAR_GRINDING					= 3090,		// 
	LOG_ITEM_INC_GRINDINGRATE_GRINDING					= 3091,		// 
	LOG_ITEM_ANTI_RESET_GRINDING						= 3092,		// 
	LOG_ITEM_TEMP_ENCHANT								= 3093,		// 
	LOG_ITEM_TEMP_ENCHANT_ERASE							= 3094,		// 
	LOG_ITEM_ADD_ITEM_RANMOBILE_BUY						= 3095,		// 
	LOG_MONEY_CONSIGEMENT_SALE_MONEY_BUY_SENT_POST		= 3096,		// 위탁판매에 등록되어 있는 게임머니를 구매하였고, 그 게임머니가 우편으로 전송되었다.(우편 첨부 게임머니를 찾아야 실제 게임머니가 증가한다.)
	LOG_ITEM_REMODEL_SELECT_RESULT						= 3097,		// 변경 전 유지 선택 상태에서 결과값 선택함
};


enum EMVEHICLE_ACTION
{
	EMVEHICLE_ACTION_BATTERY_BEFORE	= 0, // 탈것 베터리 전
	EMVEHICLE_ACTION_BATTERY_AFTER	= 1, // 탈것 베터리 후
};

namespace pet
{
    //! 펫 액션 타입
    enum EM_ACTION
    {
	    EM_ACTION_FOOD_BEFORE	   =  0, //! 펫 먹이전
	    EM_ACTION_FOOD_AFTER	   =  1, //! 펫 먹이후
	    EM_ACTION_RENAME		   =  2, //! 펫 이름변경
	    EM_ACTION_COLOR			   =  3, //! 펫 컬러변경
	    EM_ACTION_STYLE			   =  4, //! 펫 스타일변경
	    EM_ACTION_SKILL_LEARN	   =  5, //! 펫 스킬 배움
	    EM_ACTION_REVIVE		   =  6, //! 펫 부활카드
	    EM_ACTION_PETSKINPACK	   =  7, //! 펫 스킨 팩
	    EM_ACTION_PET_DUALSKILL	   =  8, //! 펫 중복 스킬 카드
        EM_ACTION_SUMMONS          =  9, //! 펫 소환
        EM_ACTION_UNSUMMONS        = 10, //! 펫 회수
        EM_ACTION_SKILL_CHANGE     = 11, //! 펫 스킬 변경
        EM_ACTION_SKILL_CHANGE_A   = 12, //! 펫 스킬 변경 A 슬롯
        EM_ACTION_SKILL_CHANGE_B   = 13, //! 펫 스킬 변경 B 슬롯
    };
} // namespace pet

struct SITEMLMT
{
	enum { VERSION = 0x001, };

	LONGLONG lnDEFAULT;
	LONGLONG lnINIT;
	LONGLONG lnSHOP;
	LONGLONG lnNPC;
	LONGLONG lnQUEST;
	LONGLONG lnGM;
	LONGLONG lnMOB;
	LONGLONG lnOLD;
	LONGLONG lnBILLING;
	LONGLONG lnGather;
	LONGLONG lnSystem;

	SITEMLMT()
        : lnDEFAULT(0)
        , lnINIT(0)
        , lnSHOP(0)
        , lnNPC(0)
        , lnQUEST(0)
        , lnMOB(0)
        , lnGM(0)
        , lnOLD(0)
        , lnBILLING(0)
        , lnGather(0)
        , lnSystem(0)
	{
	}

	LONGLONG GETTOTAL () const
	{
		return lnDEFAULT+lnINIT+lnSHOP+lnNPC+lnQUEST+lnGM+lnMOB+lnOLD+lnBILLING+lnGather+lnSystem;
	}
};

#endif // _GL_ITEM_LIMIT_DEFINE_H_
