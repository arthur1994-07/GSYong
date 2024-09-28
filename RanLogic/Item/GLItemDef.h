#pragma once

#include <string>
#include <boost/tr1/memory.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/export.hpp>

#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"
#include "../FSM/TMiniFSM.h"
#include "../Character/GLCharDefine.h"
#include "../RandomOption/RandomOption.h"

namespace glold
{
	enum GLITEM_ATT_102
	{
		ITEMATT_NOTHING		= 0,	//	착용무기없음.

		ITEMATT_SWORD		= 1,	//	검류.
		ITEMATT_REV00		= 2,	//	REV00
		ITEMATT_DAGGER		= 3,	//	단검류.
		ITEMATT_SPEAR		= 4,	//	창류.
		ITEMATT_BOW			= 5,	//	활류.
		ITEMATT_THROW		= 6,	//	투척류.

		ITEMATT_NOCARE		= 7,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		ITEMATT_NSIZE		= 7,	//	종류갯수.
	};
}

namespace glold_103
{
	enum GLITEM_ATT_103
	{
		ITEMATT_NOTHING		= 0,	//	착용무기없음.

		ITEMATT_SWORD		= 1,	//	검.
		ITEMATT_SABER		= 2,	//	도.
		ITEMATT_DAGGER		= 3,	//	단검.
		ITEMATT_SPEAR		= 4,	//	창.
		ITEMATT_STICK		= 5,	//	몽둥이.
		ITEMATT_BOW			= 6,	//	활.
		ITEMATT_THROW		= 7,	//	투척.

		ITEMATT_NOCARE		= 8,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		ITEMATT_NSIZE		= 8,	//	종류갯수.
	};
}

namespace glold_104
{
	enum GLITEM_ATT_104
	{
		ITEMATT_NOTHING		= 0,	//	착용무기없음.

		ITEMATT_SWORD		= 1,	//	검.
		ITEMATT_SABER		= 2,	//	도.
		ITEMATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

		ITEMATT_DAGGER		= 4,	//	단검.
		ITEMATT_SPEAR		= 5,	//	창.
		ITEMATT_STICK		= 6,	//	몽둥이.
		ITEMATT_GWON		= 7,	//	권.
		ITEMATT_BOW			= 8,	//	활.
		ITEMATT_THROW		= 9,	//	투척.
		
		ITEMATT_NOCARE		= 10,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		ITEMATT_NSIZE		= 10,	//	종류갯수.
	};
}

namespace glold_105
{
	enum GLITEM_ATT_105
	{
		ITEMATT_NOTHING		= 0,	//	착용무기없음.

		ITEMATT_SWORD		= 1,	//	검.
		ITEMATT_SABER		= 2,	//	도.

		ITEMATT_DAGGER		= 3,	//	단검.
		ITEMATT_SPEAR		= 4,	//	창.
		ITEMATT_STICK		= 5,	//	몽둥이.
		ITEMATT_GWON		= 6,	//	권.
		ITEMATT_BOW			= 7,	//	활.
		ITEMATT_THROW		= 8,	//	투척.

		ITEMATT_PISTOL		= 9,	//	권총
		ITEMATT_RAILGUN		= 10,	//	소총1
		ITEMATT_PORTALGUN	= 11,	//	소총2

		ITEMATT_HAMMER      = 12,   // 해머.
		ITEMATT_DUALSPEAR   = 13,   // 듀얼 스피어.
		ITEMATT_SIDE        = 14,   // 사이드.

		ITEMATT_NOCARE		= 15,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		ITEMATT_NSIZE		= 15,	//	종류갯수.
	};

    enum GLSKILL_ATT_105
    {
        SKILLATT_NOTHING	= 0,	//	착용무기없음.

        SKILLATT_SWORD		= 1,	//	검.
        SKILLATT_SABER		= 2,	//	도.
        SKILLATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

        SKILLATT_DAGGER		= 4,	//	단검.
        SKILLATT_SPEAR		= 5,	//	창.
        SKILLATT_STICK		= 6,	//	몽둥이.
        SKILLATT_GWON		= 7,	//	권.
        SKILLATT_BOW		= 8,	//	활.
        SKILLATT_THROW		= 9,	//	투척.

        SKILLATT_PISTOL		= 10,	//	권총
        SKILLATT_RAILGUN	= 11,	//	소총1
        SKILLATT_PORTALGUN	= 12,	//	소총2

        SKILLATT_GUN		= 13,	//	총 전체
        SKILLATT_RIFLE		= 14,	//	소총전체

        SKILLATT_NOCARE		= 15,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
        SKILLATT_NSIZE		= 15,	//	종류갯수.

        SKILLATT_NEAR		= 7,	//	근접 공격.
    };
}

namespace glold_106
{
	enum GLSKILL_ATT_106	// 해머, 듀얼스피어, 사이드 추가.
	{
		SKILLATT_NOTHING	= 0,	//	착용무기없음.

		SKILLATT_SWORD		= 1,	//	검.
		SKILLATT_SABER		= 2,	//	도.
		SKILLATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

		SKILLATT_DAGGER		= 4,	//	단검.
		SKILLATT_SPEAR		= 5,	//	창.
		SKILLATT_STICK		= 6,	//	몽둥이.
		SKILLATT_GWON		= 7,	//	권.
		SKILLATT_BOW		= 8,	//	활.
		SKILLATT_THROW		= 9,	//	투척.

		SKILLATT_PISTOL		= 10,	//	권총
		SKILLATT_RAILGUN	= 11,	//	소총1
		SKILLATT_PORTALGUN	= 12,	//	소총2

		SKILLATT_GUN		= 13,	//	총 전체
		SKILLATT_RIFLE		= 14,	//	소총전체

		SKILLATT_HAMMER     = 15,   //  해머
		SKILLATT_DUALSPEAR  = 16,   //  듀얼 스피어
		SKILLATT_SIDE       = 17,   //  사이드

		SKILLATT_NOCARE		= 18,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		SKILLATT_NSIZE		= 18,	//	종류갯수.

		SKILLATT_NEAR		= 7,	//	근접 공격.
	};
}

namespace glold_107
{
	enum GLSKILL_ATT_107	// 극강 글러브;
	{
		SKILLATT_NOTHING	= 0,	//	착용무기없음.

		SKILLATT_SWORD		= 1,	//	검.
		SKILLATT_SABER		= 2,	//	도.
		SKILLATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

		SKILLATT_DAGGER		= 4,	//	단검.
		SKILLATT_SPEAR		= 5,	//	창.
		SKILLATT_STICK		= 6,	//	몽둥이.
		SKILLATT_GWON		= 7,	//	권.
		SKILLATT_BOW		= 8,	//	활.
		SKILLATT_THROW		= 9,	//	투척.

		SKILLATT_PISTOL		= 10,	//	권총
		SKILLATT_RAILGUN	= 11,	//	소총1
		SKILLATT_PORTALGUN	= 12,	//	소총2

		SKILLATT_GUN		= 13,	//	총 전체
		SKILLATT_RIFLE		= 14,	//	소총전체

		SKILLATT_HAMMER     = 15,   //  해머
		SKILLATT_DUALSPEAR  = 16,   //  듀얼 스피어
		SKILLATT_SIDE       = 17,   //  사이드

		SKILLATT_EXTREME_GLOVE	= 18,	// 극강 글러브;

		SKILLATT_NOCARE		= 19,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		SKILLATT_NSIZE		= 19,	//	종류갯수.

		SKILLATT_NEAR		= 7,	//	근접 공격.
	};
}

namespace glold_108
{
	enum GLITEM_ATT_108
	{
		ITEMATT_NOTHING	= 0,	//	착용무기없음.

		ITEMATT_SWORD		= 1,	//	검.
		ITEMATT_SABER		= 2,	//	도.

		ITEMATT_DAGGER	= 3,	//	단검.
		ITEMATT_SPEAR		= 4,	//	창.
		ITEMATT_STICK		= 5,	//	몽둥이.
		ITEMATT_GWON		= 6,	//	권.
		ITEMATT_BOW			= 7,	//	활.
		ITEMATT_THROW		= 8,	//	투척.

		ITEMATT_PISTOL			= 9,	//	권총
		ITEMATT_RAILGUN		= 10,	//	소총1
		ITEMATT_PORTALGUN	= 11,	//	소총2

		ITEMATT_HAMMER		= 12,   // 해머.
		ITEMATT_DUALSPEAR	= 13,   // 듀얼 스피어.
		ITEMATT_SIDE				= 14,   // 사이드.

		ITEMATT_EXTREME_GLOVE	= 15,	// 극강 글러브.

		ITEMATT_TRICK_STICK	= 16, // 마술 지팡이;
		ITEMATT_TRICK_BOX	= 17, // 마술 상자;
		ITEMATT_TRICK_WHIP	= 18,	// 마술 채찍;

		ITEMATT_NOCARE		= 19,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		ITEMATT_NSIZE		= 19,	//	종류갯수.
	};

	enum GLSKILL_ATT_108
	{
		SKILLATT_NOTHING	= 0,	//	착용무기없음.

		SKILLATT_SWORD		= 1,	//	검.
		SKILLATT_SABER		= 2,	//	도.
		SKILLATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

		SKILLATT_DAGGER		= 4,	//	단검.
		SKILLATT_SPEAR		= 5,	//	창.
		SKILLATT_STICK		= 6,	//	몽둥이.
		SKILLATT_GWON		= 7,	//	권.
		SKILLATT_BOW		= 8,	//	활.
		SKILLATT_THROW		= 9,	//	투척.

		SKILLATT_PISTOL		= 10,	//	권총
		SKILLATT_RAILGUN	= 11,	//	소총1
		SKILLATT_PORTALGUN	= 12,	//	소총2

		SKILLATT_GUN		= 13,	//	총 전체
		SKILLATT_RIFLE		= 14,	//	소총전체

		SKILLATT_HAMMER     = 15,   //  해머
		SKILLATT_DUALSPEAR  = 16,   //  듀얼 스피어
		SKILLATT_SIDE       = 17,   //  사이드

		SKILLATT_EXTREME_GLOVE	= 18,	// 극강 글러브;

		SKILLATT_TRICK_STICK	= 19, // 마술 지팡이;
		SKILLATT_TRICK_BOX		= 20, // 마술 상자;
		SKILLATT_TRICK_WHIP		= 21, // 마술 채찍;

		SKILLATT_NOCARE		= 22,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		SKILLATT_NSIZE		= 22,	//	종류갯수.

		SKILLATT_NEAR		= 7,	//	근접 공격.
	};
}

enum EMITEMLEVEL
{
	EMITEM_LEVEL_NORMAL		= 0,	// 일반;
	EMITEM_LEVEL_HIGH		= 1,	// 고급;
	EMITEM_LEVEL_RARE		= 2,	// 희귀;
	EMITEM_LEVEL_UNIQUE		= 3,	// 영웅;
	EMITEM_LEVEL_EPIC		= 4,	// 고대;
	EMITEM_LEVEL_LEGENDARY	= 5,	// 전설;
	
	EMITEM_LEVEL_NSIZE		= 6,
};

enum EMITEM_FLAG
{
	TRADE_SALE			= 0x001,	//	상점에 판매 가능.
	TRADE_EXCHANGE		= 0x002,	//	개인간 교환 가능.
	TRADE_THROW			= 0x004,	//	필드에 버리기 가능.

	TRADE_EVENT_SGL		= 0x008,	//	여러 이벤트 아이템중 한개 이상 못 줍게 하기 위한.

	ITEM_DISGUISE		= 0x010,	//	코스툼.
	ITEM_TIMELMT		= 0x020,	//	시효성.
	
	ITEM_CHANNEL_ALL	= 0x040, //	모든 체널.
	TRADE_GARBAGE		= 0x080, //	파쇄 가능

	ITEM_RESTRICT		= 0x100, //	기능 제한. by luxes.

    ITEM_BOX_WRAPPABLE  = 0x200, // 포장지로 포장 가능 여부;
	ITEM_SPLIT			= 0x400, // 아이템 분해 가능 여부;

	LIMIT_LOCKER		= 0x800, // 창고에 넣을수 없는지 여부;

	TRADE_ALL		= TRADE_SALE|TRADE_EXCHANGE|TRADE_THROW,
};

enum EMITEM_HAND
{
	HAND_RIGHT		= 0x0001,	//	오른손 용.
	HAND_LEFT		= 0x0002,	//	왼손 용.
	HAND_BOTH		= 0x0003,	//	왼손, 오른손 모두 가능.
};

//	Note : 캐릭터 에니메이션 제어와 관련이 있음.
//		캐릭터 에니메이션 선택할때 착용 아이탬에 따라 선택을 함.
//
enum GLITEM_ATT
{
	ITEMATT_NOTHING	= 0,	//	착용무기없음.

	ITEMATT_SWORD		= 1,	//	검.
	ITEMATT_SABER		= 2,	//	도.

	ITEMATT_DAGGER	= 3,	//	단검.
	ITEMATT_SPEAR		= 4,	//	창.
	ITEMATT_STICK		= 5,	//	몽둥이.
	ITEMATT_GWON		= 6,	//	권.
	ITEMATT_BOW			= 7,	//	활.
	ITEMATT_THROW		= 8,	//	투척.
	
	ITEMATT_PISTOL			= 9,	//	권총
	ITEMATT_RAILGUN		= 10,	//	소총1
	ITEMATT_PORTALGUN	= 11,	//	소총2

    ITEMATT_SIDE			= 12,   // 갈고리 낫
    ITEMATT_DUALSPEAR		= 13,   // 듀얼 스피어
    ITEMATT_THROWING_KNIFE	= 14,   // 수리검

	ITEMATT_EXTREME_GLOVE	= 15,	// 극강 글러브.

	ITEMATT_TRICK_STICK	= 16, // 마술 지팡이;
	ITEMATT_TRICK_BOX	= 17, // 마술 상자;
	ITEMATT_TRICK_WHIP	= 18,	// 마술 채찍;

	ITEMATT_ACTOR_SHIELD = 19, // 방패
	ITEMATT_ACTOR_HAMMER = 20, // 망치
	ITEMATT_ACTOR_UMBRELLA = 21, // 우산

	ITEMATT_WEAPON_STR = 22,		// 힘무기;
	ITEMATT_WEAPON_DEX = 23,		// 민첩무기;
	ITEMATT_WEAPON_SPI = 24,		// 정신무기;

	ITEMATT_NOCARE		= 25,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
	ITEMATT_NSIZE		= 25,	//	종류갯수.
};

// 각 무기 타입별 공격거리;
enum EMITEM_ATTACK_DISTANCE
{
	EMITEM_ATTACK_DISTANCE_NOTHING = 2,				// 착용무기없음;
	EMITEM_ATTACK_DISTANCE_SWORD = 5,				// 검;
	EMITEM_ATTACK_DISTANCE_SABER = 5,				// 도;
	EMITEM_ATTACK_DISTANCE_DAGGER = 1,				// 단검;
	EMITEM_ATTACK_DISTANCE_SPEAR = 7,				// 창;
	EMITEM_ATTACK_DISTANCE_STICK = 5,				// 몽둥이;
	EMITEM_ATTACK_DISTANCE_GWON = 1,				// 권;
	EMITEM_ATTACK_DISTANCE_BOW = 75,				// 활;
	EMITEM_ATTACK_DISTANCE_THROW = 30,				// 투척;
	EMITEM_ATTACK_DISTANCE_PISTOL = 75,				// 건;
	EMITEM_ATTACK_DISTANCE_RAILGUN = 90,			// 레일건;
	EMITEM_ATTACK_DISTANCE_PORTALGUN = 90,			// 포탈건;
	EMITEM_ATTACK_DISTANCE_SIDE = 10,				// 갈고리낫;
	EMITEM_ATTACK_DISTANCE_DUALSPEAR = 5,			// 듀얼 스피어;
	EMITEM_ATTACK_DISTANCE_THROWING_KNIFE = 20,		// 수리검;
	EMITEM_ATTACK_DISTANCE_EXTREME_GLOVE = 2,		// 극강 글러브;
	EMITEM_ATTACK_DISTANCE_TRICK_STICK = 70,		// 마술 지팡이;
	EMITEM_ATTACK_DISTANCE_TRICK_BOX = 60,			// 마술 상자;
	EMITEM_ATTACK_DISTANCE_TRICK_WHIP = 60,			// 마술 채찍;
	EMITEM_ATTACK_DISTANCE_ACTOR_SHIELD = 12,		// 방패;
	EMITEM_ATTACK_DISTANCE_ACTOR_HAMMER = 10,		// 망치;
	EMITEM_ATTACK_DISTANCE_ACTOR_UMBRELLA = 15,		// 우산;
};

inline const WORD GET_ATTACKDISTANCE_USING_ITEMATT ( GLITEM_ATT _emItemAtt )
{
	switch ( _emItemAtt )
	{
	default:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_NOTHING );

	case ITEMATT_SWORD:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_SWORD );

	case ITEMATT_SABER:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_SABER );

	case ITEMATT_DAGGER:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_DAGGER );

	case ITEMATT_SPEAR:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_SPEAR );

	case ITEMATT_STICK:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_STICK );

	case ITEMATT_GWON:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_GWON );

	case ITEMATT_BOW:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_BOW );

	case ITEMATT_THROW:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_THROW );

	case ITEMATT_PISTOL:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_PISTOL );

	case ITEMATT_RAILGUN:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_RAILGUN );

	case ITEMATT_PORTALGUN:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_PORTALGUN );

	case ITEMATT_SIDE:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_SIDE );

	case ITEMATT_DUALSPEAR:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_DUALSPEAR );

	case ITEMATT_THROWING_KNIFE:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_THROWING_KNIFE );

	case ITEMATT_EXTREME_GLOVE:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_EXTREME_GLOVE );

	case ITEMATT_TRICK_STICK:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_TRICK_STICK );

	case ITEMATT_TRICK_BOX:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_TRICK_BOX );

	case ITEMATT_TRICK_WHIP:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_TRICK_WHIP );

	case ITEMATT_ACTOR_SHIELD:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_ACTOR_SHIELD );

	case ITEMATT_ACTOR_HAMMER:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_ACTOR_HAMMER );

	case ITEMATT_ACTOR_UMBRELLA:
		return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_ACTOR_UMBRELLA );
	}

	return static_cast< WORD >( EMITEM_ATTACK_DISTANCE_NOTHING );
}

enum GLITEM_CONST
{
	ITEM_SZNAME		= 65,	//	item 이름 길이 제한. (NULL) 문자 포함 길이.
	ITEM_SZCOMMENT	= 256,	//	item "간략설명" 길이 제한. (NULL) 문자 포함 길이.
	ITEM_SKILLS		= 4,	//	"스킬 포인트 증가치" 가 붙을수 있는 갯수.
};

enum EMSUIT
{
	SUIT_HEADGEAR		= 0,
	SUIT_UPPER			= 1,
	SUIT_LOWER			= 2,
	SUIT_HAND			= 3,
	SUIT_FOOT			= 4,

	SUIT_HANDHELD		= 5,

	SUIT_NECK			= 6,
	SUIT_WRIST			= 7,

	SUIT_FINGER			= 8, // 반지;

	SUIT_PET_A			= 9,	// PetData
	SUIT_PET_B			= 10,
	SUIT_VEHICLE		= 11,	// 탈것
	
	SUIT_VEHICLE_SKIN		= 12,	// 탈것 중 보드 스킨
	SUIT_VEHICLE_PARTS_A	= 13,	// 탈것 중 보드 스킨
	SUIT_VEHICLE_PARTS_B	= 14,	// 탈것 중 보드 파츠
	SUIT_VEHICLE_PARTS_C	= 15,
	SUIT_VEHICLE_PARTS_D	= 16,
	SUIT_VEHICLE_PARTS_E	= 17,
	SUIT_VEHICLE_PARTS_F	= 18,
	SUIT_VEHICLE_PARTS_SIZE = 6,
    
    SUIT_WAISTBAND	= 19,   //허리띠
    SUIT_EARRING			= 20,   //귀걸이
    SUIT_ACCESSORIES	= 21,   //장신구
	SUIT_DECORATION	= 22, // 장식;

	SUIT_NSIZE			= 23,
};

enum ITEM_VALUE
{
	ITEM_VAlUE_SUIT_REMODELNUM,
	ITEM_VAlUE_NSIZE,
};
////	Note : 착용위치.
//enum EMSLOT
//{
//	SLOT_HEADGEAR	    = 0,	//	머리 쓸것
//	SLOT_UPPER		    = 1,	//	상체
//	SLOT_LOWER		    = 2,	//	하체
//	SLOT_HAND		    = 3,	//	손
//	SLOT_FOOT		    = 4,	//	발
//
//	SLOT_RHAND		    = 5,	//	오른손 도구
//	SLOT_LHAND		    = 6,	//	왼손 도구
//	
//	SLOT_NECK		    = 7,	//	목걸이
//	SLOT_WRIST		    = 8,	//	손목
//
//	SLOT_RFINGER	    = 9,	//	오른손 손가락
//	SLOT_LFINGER	    = 10,	//	왼손 손가락
//
//	SLOT_RHAND_S	    = 11,	//	오른손 도구, 극강부
//	SLOT_LHAND_S	    = 12,	//	왼손 도구, 극강부 
// 
//	SLOT_VEHICLE	    = 13,	//  탈것 
//
//	SLOT_NSIZE		    = 11,   // 일반부서 아이템 q( - 탈것 )
//	SLOT_NSIZE_2	    = 12,   // 일반부서 아이템 ( + 탈것 )
//	SLOT_NSIZE_S	    = 13,	// 극강부 아이템 ( - 탈것 )
//	SLOT_NSIZE_S_2	    = 14,	// 극강부 아이템 ( + 탈것 )
//
//	SLOT_HOLD		    = 14,	//	손에 들고 있는 것.
//
//	SLOT_TSIZE		    = 15,
//};

enum EMSLOTCHECK_RESULT
{
	EMSLOTCHECK_OK = 0,					// 성공;
	EMSLOTCHECK_FAILED_UNIQUE,			// 고유 아이템은 하나만 장착가능하다;
	EMSLOTCHECK_FAILED,					// 실패;
};

//	Note : 착용위치.
enum EMSLOT
{
    SLOT_HEADGEAR	    = 0,//	머리 쓸것
    SLOT_UPPER		    = 1,	//	상체
    SLOT_LOWER		    = 2,	//	하체
    SLOT_HAND		    = 3,	//	손
    SLOT_FOOT		    = 4,	//	발

    SLOT_RHAND		    = 5,	//	오른손 도구
    SLOT_LHAND		    = 6,	//	왼손 도구

    SLOT_NECK		    = 7,	//	목걸이
    SLOT_WRIST		    = 8,	//	손목

    SLOT_RFINGER	    = 9,	//	오른손 손가락
    SLOT_LFINGER	    = 10,	//	왼손 손가락    

    SLOT_RHAND_S	    = 11,	//	오른손 도구, 극강부
    SLOT_LHAND_S	    = 12,	//	왼손 도구, 극강부 

    SLOT_VEHICLE	    = 13,	//  탈것     

    SLOT_WAISTBAND      = 14,   // 허리띠
    SLOT_DECORATION       = 15,   // 장식;
    SLOT_EARRINGS       = 16,   // 귀걸이
    SLOT_RACCESSORIES   = 17,   // 오른쪽 장신구
    SLOT_LACCESSORIES   = 18,   // 왼쪽 장신구

    SLOT_NSIZE		    = SLOT_RHAND_S,   // 일반부서 아이템 q( - 탈것 )
    SLOT_NSIZE_2	    = SLOT_LHAND_S,   // 일반부서 아이템 ( + 탈것 ) //아이템 미리보기에서 사용
    SLOT_NSIZE_S	    = SLOT_VEHICLE,	// 극강부 아이템 ( - 탈것 )     //강화주 완화주 착용검사
    SLOT_NSIZE_S_2	    = 19,	// 극강부 아이템 ( + 탈것 )    
	SLOT_NSIZE_S_3		= 6,    // 아이템 스킬 링크 가능슬롯 개수

	//  [1/16/2013 gbgim]
	// - 예전 홀드 슬롯 쓸때 사용됫음. 
	//   예전에는 아이템을 들면 서버에 들엇다 패킷 전송 하고 홀드 슬롯에 아이템 넣는 방식
	//   현재는 클라에서 든 아이템의 위치만 가지고 있고 이미지만 따라다니게 되어있음.
	//   소스 정리 필요함!
    SLOT_HOLD		    = SLOT_NSIZE_S_2,	//	손에 들고 있는 것.(마우스로 들고있는 아이템)    

    SLOT_TSIZE		    = 20,
};

inline EMSUIT SLOT_2_SUIT ( EMSLOT emSlot )
{
	switch( emSlot )
	{
	case SLOT_HEADGEAR:
        return SUIT_HEADGEAR;

	case SLOT_UPPER:
        return SUIT_UPPER;

	case SLOT_LOWER:
        return SUIT_LOWER;

	case SLOT_HAND:
        return SUIT_HAND;

	case SLOT_FOOT:
        return SUIT_FOOT;

	case SLOT_RHAND:	
	case SLOT_RHAND_S:
	case SLOT_LHAND:	
	case SLOT_LHAND_S:
        return SUIT_HANDHELD;

	case SLOT_NECK:
        return SUIT_NECK;

	case SLOT_WRIST:
        return SUIT_WRIST;

	case SLOT_RFINGER:
	case SLOT_LFINGER:
        return SUIT_FINGER;

	case SLOT_VEHICLE:
        return SUIT_VEHICLE;

    case SLOT_WAISTBAND:    
        return SUIT_WAISTBAND;

    case SLOT_DECORATION:
		return SUIT_DECORATION;
    case SLOT_EARRINGS:
        return SUIT_EARRING;

    case SLOT_RACCESSORIES:
    case SLOT_LACCESSORIES:
        return SUIT_ACCESSORIES;

	default:
        return SUIT_NSIZE;
	}
}

inline EMPIECECHAR SLOT_2_PIECE ( EMSLOT emSlot )
{
	switch( emSlot )
	{
	case SLOT_HEADGEAR:
        return PIECE_HEADGEAR;

	case SLOT_UPPER:
        return PIECE_UPBODY;

	case SLOT_LOWER:
        return PIECE_LOBODY;

	case SLOT_HAND:
        return PIECE_GLOVE;

	case SLOT_FOOT:
        return PIECE_FOOT;

	case SLOT_RHAND:	
	case SLOT_RHAND_S:
        return PIECE_RHAND;

	case SLOT_LHAND:	
	case SLOT_LHAND_S:
        return PIECE_LHAND;

	case SLOT_VEHICLE:
        return PIECE_VEHICLE;

	case SLOT_NECK:
	case SLOT_WRIST:
	case SLOT_RFINGER:
	case SLOT_LFINGER:
    case SLOT_WAISTBAND:
    case SLOT_DECORATION:
    case SLOT_EARRINGS:
    case SLOT_RACCESSORIES:
    case SLOT_LACCESSORIES:
        return PIECE_NONE;

	default:
        return PIECE_NONE;
	}
}

inline EMSLOT PIECE_2_SLOT ( EMPIECECHAR emPiece )
{
	switch( emPiece )
	{
	case PIECE_HEAD:
        return SLOT_HEADGEAR;

	case PIECE_UPBODY:
        return SLOT_UPPER;

	case PIECE_LOBODY:
        return SLOT_LOWER;

	case PIECE_GLOVE:
        return SLOT_HAND;

	case PIECE_RHAND:
        return SLOT_RHAND;

	case PIECE_LHAND:
        return SLOT_LHAND;

	case PIECE_FOOT:
        return SLOT_FOOT;

	case PIECE_VEHICLE:
        return SLOT_VEHICLE;

	case PIECE_HAIR:
	case PIECE_HEADGEAR:
        return SLOT_TSIZE;

	default:
        return SLOT_TSIZE;
	}
}

//! 아이템 종류
enum EMITEM_TYPE
{
	ITEM_SUIT				= 0,
	ITEM_ARROW				= 1,	//! 화살.
	ITEM_CURE				= 2,	//! 약품.
	ITEM_SKILL				= 3,	//! 스킬 습득 서적.
	ITEM_RECALL				= 4,	//! 귀환서.
	ITEM_KEY				= 5,	//! 인증서 (열쇠)
	ITEM_GRINDING			= 6,	//! 연마제
	ITEM_CHARM				= 7,	//! 부적.
	ITEM_TICKET				= 8,	//! 승차권.
	ITEM_SKP_RESET			= 9,	//! 스킬 포인트 리셋. (미사용)

	ITEM_STAT_RESET			= 10,	//! 스텟 포인트 리셋. (미사용)
	ITEM_SKP_STAT_RESET_A	= 11,	//! 스킬, 스텟 포인트 균등 리셋. (구 망각의 약)
	ITEM_BOX				= 12,	//! 선물 상자.
	ITEM_CLEANSER			= 13,	//! 세탁제.
	ITEM_LOUDSPEAKER		= 14,	//! 확성기.
	ITEM_FIRECRACKER		= 15,	//! 폭죽.
	ITEM_CHARACTER_CARD		= 16,	//! 캐릭터 추가 카드.
	ITEM_INVEN_CARD			= 17,	//! 인벤 확장 카드.
	ITEM_STORAGE_CARD		= 18,	//! 창고 확장 카드.
	ITEM_STORAGE_CONNECT	= 19,	//! 창고 연결 카드.

	ITEM_PREMIUMSET			= 20,	//! 프리미엄 세트.
	ITEM_PRIVATEMARKET		= 21,	//! 개인상점 개설 권리.
	ITEM_RANDOMITEM			= 22,	//! 랜덤 아이탬.
	ITEM_DISJUNCTION		= 23,	//! 코스툼 분리.
	ITEM_HAIR				= 24,	//! 헤어스타일 변경.
	ITEM_FACE				= 25,	//! 얼굴 변경.
	ITEM_QITEM				= 26,	//! Question 아이템.
	ITEM_CD					= 27,	//! CD.
	ITEM_2FRIEND			= 28,	//! 친구에게.
	ITEM_CLUBCALL			= 29,	//! 클럽호출.

	ITEM_HAIRSHOP			= 30,	//! 헤어샾 이용권.
	ITEM_RENAME				= 31,   //! 이름변경 카드.
	ITEM_HAIR_STYLE			= 32,	//! 헤어스타일
	ITEM_HAIR_COLOR			= 33,   //! 헤어컬러
	ITEM_REVIVE				= 34,	//! 귀혼주
	ITEM_PET_CARD			= 35,	//! 펫 카드	// PetData
	ITEM_PET_FOOD			= 36,	//! 펫 먹이
	ITEM_PET_RENAME			= 37,	//! 팻 이름변경
	ITEM_PET_COLOR			= 38,	//! 팻 컬러변경
	ITEM_PET_STYLE			= 39,	//! 팻 스타일변경

	ITEM_PET_SKILL			= 40,	//! 팻 스킬
	ITEM_SMS				= 41,	//! SMS문자 발송
	ITEM_PET_REVIVE			= 42,	//! 팻 부활카드
	ITEM_ANTI_DISAPPEAR 	= 43,	//! 소방주(소멸방지)
	ITEM_REMODEL			= 44,	//! 개조 기능
	ITEM_VEHICLE			= 45,	//! 탈것
	ITEM_VEHICLE_OIL		= 46,	//! 탈것 기름
	ITEM_VIETNAM_ITEMGET	= 47,	//! 베트남 탐닉 방지 아이템 획득
	ITEM_VIETNAM_EXPGET		= 48,	//! 베트남 탐닉 방지 경험치 획득	
	ITEM_GENDER_CHANGE		= 49,	//! 성별 변경 카드 

	ITEM_GARBAGE_CARD		= 50,	//! 휴지통 카드	
	ITEM_TELEPORT_CARD		= 51,	//! 소환서	
	ITEM_PET_SKIN_PACK  	= 52,	//! 펫 스킨 팩
	ITEM_FACE_STYLE			= 53,	//! 얼굴스타일
	ITEM_TAXI_CARD			= 54,	//! 택시카드
	ITEM_MATERIALS			= 55,	//! 재료아이템
	ITEM_NPC_RECALL			= 56,	//! NPC소환 카드
	ITEM_BULLET				= 57,	//! 총알
	ITEM_LUNCHBOX			= 58,	//! 도시락
	ITEM_PET_DUALSKILL      = 59,	//! 펫 스킬 중복사용 카드

	ITEM_INC_GRINDINGRATE   = 60,	//! 강화주 (강화 확률 증가)
	ITEM_ANTI_RESET		    = 61,	//! 완화주 (리셋 단계 완화)
	ITEM_VEHICLE_COLOR		= 62,	//! 탈것 컬러 변경카드
	ITEM_BIKE_BOOST_CARD	= 63,	//! 오토바이 부스트 기능카드
	ITEM_SKP_STAT_RESET_B	= 64,	//! 스킬, 스텟 포인트 선택 리셋. (신 망각의 약)
	ITEM_COSTUM_COLOR_CH_CD	= 65,	//! 코스튬 컬러 변경 카드.
	ITEM_POSTBOX_CARD		= 66,	//! 우편함 연결 카드
    ITEM_POINT_CARD_REFUND     = 67, //! 환불가능 Point Card
    ITEM_POINT_CARD_NOT_REFUND = 68, //! 환불불가 Point Card
	ITEM_PRODUCT_BOOK		= 69,	//! 조합서
	
    ITEM_PRODUCT_RESET		= 70,	//! 제작 초기화
    ITEM_PMARKET_SEARCH_CARD = 71,  //! 개인상점 검색 카드
    ITEM_REPAIR_CARD		= 72,	//! 수리도구 연결카드
	ITEM_STAMP_CARD		= 73, //! 인장 카드;
	ITEM_STAGE_PASS			= 74,	//!스테이지 패스
	ITEM_ENTER_NUM_CHARGE	= 75,	//!인던 입장횟수 중전권
	ITEM_CARDGAME_CARD	= 76, //! 카드게임 연결카드
    ITEM_CONSIGMENT_SALE_GAME_MONEY = 77,  //! 게임머니 아이템
    ITEM_CONSIGMENT_SALE_EXTEND_CARD = 78,  //! 위탁판매 확장슬롯
	ITEM_LOCK_BOX = 79,				//! 잠긴 상자;
	ITEM_LOCK_BOX_KEY = 80,						//! 선물 상자 열쇠;
    ITEM_BOX_WRAPPER = 81,	//! 박스 포장지;
    ITEM_WRAPPED_BOX = 82,	//! 포장된 박스;
	ITEM_SCHOOL_CHANGE_0 = 83,	//! 학원 변경 카드 
	ITEM_SCHOOL_CHANGE_1 = 84,	//! 학원 변경 카드 
	ITEM_SCHOOL_CHANGE_2 = 85,	//! 학원 변경 카드 
	ITEM_INITIALIZE_COUNTRY = 86,	//! 국가 초기화 아이템;
	ITEM_COSTUME_STATS   = 87,		//! 코스튬 능력치 부여
	ITEM_EFFECT_RELEASE_CURE = 88,  //! 효과해제 약품
	ITEM_COSTUME_EFFECT_RELEASE = 89, // !코스튬 능력치 해제
	ITEM_EXP_CARD = 90,				//! 경험치 카드 ( 점핑 이벤트를 위한 아이템 )
	ITEM_SELECTIVEFORM_BOX = 91,	//! 선택형 박스;
	ITEM_EXP_COMPERSSOR = 92,			//! 경험치 압축    
	ITEM_EXP_CAPSULE = 93,			//! 경험치 컙슐
    ITEM_TEMP_ENCHANT = 94,
	ITEM_PERIODEXTANSION = 95,		//! 기간연장;

	ITEM_RANMOBILE_CONNECTION = 96, // 모바일 연동권
	ITEM_RANMOBILE_JEWEL = 97,

	ITEM_CHANGE_ACTORS_DISGUISE_INFO = 98,		// 변술부 변장아이템;

	ITEM_CLUBRENAME = 99,	//! 클럽이름변경 카드.

	ITEM_CHANGE_ITEMSKILL_CARD = 100,		// 아이템 스킬 변경 카드;
	ITEM_CHANGE_BASICSTATS_CARD = 101,		// 아이템 기본 능력치 변경 카드;
	ITEM_CHANGE_ADDSTATS_CARD = 102,		// 아이템 가산 능력치 변경 카드;
	ITEM_UNLOCK_CHARACTER_SLOT = 103,		// 캐릭터 슬롯 락 해제;

	ITEM_NSIZE,						//! 종류갯수.
	
};

static const int PROHIBIT_ITEM_LIST[] = 
{
	ITEM_TAXI_CARD,
	ITEM_TELEPORT_CARD,
	ITEM_RECALL,
	ITEM_CURE,
	ITEM_EFFECT_RELEASE_CURE,
};

static const int PROHIBIT_ITEM_SIZE = sizeof(PROHIBIT_ITEM_LIST) / sizeof(PROHIBIT_ITEM_LIST[0]);

// 효과해제 약품의 해제리스트( ER. EFFECT_RELEASE )
enum EMITEM_DRUG_ER
{
	ITEM_DRUG_ER_STUN		 = 0, // 스턴 해제
	ITEM_DRUG_ER_HALLUCINATE = 1, // 환독 해제
	ITEM_DRUG_ER_NON_DRUG    = 2, // 물약사용금지 해제
	ITEM_DRUG_ER_SIZE,
	ITEM_DRUG_ER_MAX = 30		  // 값(비트자리) 최대 DWORD 
};

enum EMITEM_DRUG
{
	ITEM_DRUG_NUNE			= 0,
	ITEM_DRUG_HP			= 1,	//	체력
	ITEM_DRUG_MP			= 2,	//	기력
	ITEM_DRUG_SP			= 3,	//	활력

	ITEM_DRUG_HP_MP			= 4,	//	체력+기력
	ITEM_DRUG_MP_SP			= 5,	//	체력+활력
	ITEM_DRUG_HP_MP_SP		= 6,	//	체력+기력+활력
	
	ITEM_DRUG_CURE			= 7,	//	상태 이상 치료.

	ITEM_DRUG_CALL_SCHOOL	= 8,	//	학교귀환.
	ITEM_DRUG_CALL_REGEN	= 9,	//	시작귀환.
	ITEM_DRUG_CALL_LASTCALL	= 10,	//	직전귀환.
	ITEM_DRUG_CALL_REVIVE	= 11,	//	부활기능.

	ITEM_DRUG_HP_CURE		= 12,	// 체력+이상치료
	ITEM_DRUG_HP_MP_SP_CURE	= 13,	// 체력+기력+활력+이상치료
	ITEM_DRUG_CALL_TELEPORT	= 14,	// 특정공간이동

	ITEM_DRUG_CP			= 15,	// 전투 수치 획득

	ITEM_DRUG_STAGE_PASS	= 16,	// 스테이지 패스
	ITEM_DRUG_ENTER_NUM_CHARGE	= 17,	// 인던 입장횟수 중전권
	ITEM_DRUG_TENDENCY_RECOVERY = 18,	// 성향회복
	ITEM_DRUG_COSTUME_STATS = 19,
	ITEM_DRUG_EFFECT_RELEASE = 20, 
	ITEM_DRUG_COSTUME_EFF_RELEASE = 21,
	

	ITEM_DRUG_EXP_COMPERSSOR_TYPE1 = 22, // 경험치 압축
	ITEM_DRUG_EXP_COMPERSSOR_TYPE2 = 23, // 경험치 압축
	ITEM_DRUG_EXP_CAPSULE_TYPE1	= 24, // 경험치 캡슐
	ITEM_DRUG_EXP_CAPSULE_TYPE2	= 25, // 경험치 캡슐

    ITEM_DRUG_TEMP_ENCHANT = 26,	// 일시강화
	ITEM_DRUG_MACROTIME = 27,		// 매크로 시간 충전;
	ITEM_DRUG_PERIODEXTENTION = 28, // 기간 연장;
	
	ITEM_DRUG_SIZE,
};

// 열거형들 사이에 추가되는일이 없어야한다;
enum EMITEM_ADDON
{
	EMADD_NOTINIT	= MAXWORD,
	EMADD_NONE		= 0,
	EMADD_HITRATE	= 1,	//	명중율
	EMADD_AVOIDRATE	= 2,	//	회피율.

	EMADD_DAMAGE	= 3,	//	공격력.
	EMADD_DEFENSE	= 4,	//	방어력.

	EMADD_HP		= 5,	//	체력 증가.
	EMADD_MP		= 6,	//	마나 증가.
	EMADD_SP		= 7,	//	스테미나 증가.

	EMADD_STATS_POW	= 8,	//	힘.
	EMADD_STATS_STR	= 9,	//	체력.
	EMADD_STATS_SPI	= 10,	//	정신.
	EMADD_STATS_DEX	= 11,	//	민첩.
	EMADD_STATS_INT	= 12,	//	지성.
	EMADD_STATS_STA	= 13,	//	근력.

	EMADD_PA		= 14,	//	격투치.
	EMADD_SA		= 15,	//	사격치.
	EMADD_MA		= 16,	//	마력치.

	// 여기서 부터 기존의 EMITEM_VAR;
	// 네이밍을 EMADDEX_로하는데 EMADD하나로 가기에는 기존 ADDON과 겹치는 옵션이 많다;
	// 추후 추가되는 열거형은 EX를 추가하지않아도 된다;
	EMADDEX_INCR_NONE			= 17,
	EMADDEX_INCR_HP				= 18,
	EMADDEX_INCR_MP				= 19,
	EMADDEX_INCR_SP				= 20,
	EMADDEX_INCR_AP				= 21,
	EMADDEX_INCR_MOVE_SPEED		= 22,
	EMADDEX_INCR_ATTACK_SPEED	= 23,
	EMADDEX_INCR_CRITICAL_RATE	= 24,
	EMADDEX_INCR_CRUSHING_BLOW	= 25,
	EMADDEX_INC_NONE			= 26,
	EMADDEX_INC_HP				= 27,
	EMADDEX_INC_MP				= 28,
	EMADDEX_INC_SP				= 29,
	EMADDEX_INC_AP				= 30,
	EMADDEX_INC_MOVE_SPEED		= 31,
	EMADDEX_INC_ATTACK_SPEED	= 32,
	EMADDEX_INC_CRITICAL_RATE	= 33,
	EMADDEX_INC_CRUSHING_BLOW	= 34,
	EMADDEX_INCR_IGNORE_BOSSDAMAGE		= 35,		// 보스몹에게서 피격될시 일정 데미지 무시;
	EMADDEX_BOSSFIXEDDAMAGE				= 36,			// 보스몹에게 고정데미지;
	EMADDEX_INCR_CRITICAL_EMERGENCYMOB	= 37,	// 위급 상태의 몹에게 크리티컬 발생률 증가;
	EMADDEX_INCR_CRITICAL_EMERGENCYBOSS = 38,	// 위급 상태의 보스에게 크리티컬 발생률 증가;
	EMADDEX_DEC_DAMAGE_EMERGENCYSTATE	= 39, 	// 위급 상태일 경우 데미지 감소;
	EMADDEX_INCR_EXPADDEFFECT			= 40,	// 일반 몹 사살 경험치 증가;
	EMADDEX_INCR_DROPGENMONEY			= 41,	// 몹 사냥시 드랍되는 돈 증가율;
	EMADDEX_DECR_DAMAGE_MELEESKILL		= 42,	// 물리 근접 스킬 피격시 경우 데미지 감소;
	EMADDEX_DECR_DAMAGE_RANGESKILL		= 43,	// 사격 스킬 피격시 경우 데미지 감소;
	EMADDEX_DECR_DAMAGE_MAGICSKILL		= 44,	// 마법 스킬 피격시 경우 데미지 감소;
	EMADDEX_INC_HP_SOLOPLAY				= 45,		// 솔로플레이시 HP증가 (파티가 아닐시);
	EMADDEX_INC_HP_PARTYPLAY			= 46,	// 파티플레이시 HP증가;
	EMADDEX_INCR_ATKSPEED_SOLOPLAY		= 47,	// 솔로플레이시 공속 증가 (파티가 아닐시);
	EMADDEX_INCR_ATKSPEED_PARTYPLAY		= 48,	// 파티플레이시 공속 증가;

	// 이걸로 인해 구조는 복잡해질것임 [10/24/2016 gbgim];
	// 기획 당시 제대로 파악이 안됬으며 이대로 진행 안하면 안된단다;
	// 해당 옵션은 스크립트에 의해 결정되며 SSUIT보다는 SITEMCUSTOM에 종속적이다;
	// 해당 값으로 세팅된 실 데이터는 SVAR, SVOL, SADDON중 하나이다;
	EMADD_RANDOM	= 49,	//	랜덤;

	EMADD_SIZE		= 50,
};

namespace ITEM
{
	// SADDON, SVAR, SVOL을 SADDON 하나로 합친다;
	struct SADDON
	{
		// 적용 타입;
		EMITEM_ADDON	emTYPE;
		// 사용처는 ITEM::bAddonValueInteger 참고하면됨;
		union
		{
			int				nVALUE;
			float			fVALUE; 
		};

		SADDON () : emTYPE(EMADD_NONE), nVALUE(0)
		{
		}

		SADDON (EMITEM_ADDON _type) : emTYPE(_type), nVALUE(0)
		{
		}

		const bool IsNoneType() const { return (emTYPE == EMADD_NONE) || ( emTYPE == EMADDEX_INCR_NONE) || ( emTYPE == EMADDEX_INC_NONE)? true : false; }
	};

	const bool bAddonValueInteger[EMADD_SIZE] =
	{
		true, // EMADD_NONE		= 0,
		true, // EMADD_HITRATE	= 1,	//	명중율
		true, // EMADD_AVOIDRATE	= 2,	//	회피율.
		true, // EMADD_DAMAGE	= 3,	//	공격력.
		true, // EMADD_DEFENSE	= 4,	//	방어력.
		true, // EMADD_HP		= 5,	//	체력 증가.
		true, // EMADD_MP		= 6,	//	마나 증가.
		true, // EMADD_SP		= 7,	//	스테미나 증가.
		true, // EMADD_STATS_POW	= 8,	//	힘.
		true, // EMADD_STATS_STR	= 9,	//	체력.
		true, // EMADD_STATS_SPI	= 10,	//	정신.
		true, // EMADD_STATS_DEX	= 11,	//	민첩.
		true, // EMADD_STATS_INT	= 12,	//	지성.
		true, // EMADD_STATS_STA	= 13,	//	근력.
		true, // EMADD_PA		= 14,	//	격투치.
		true, // EMADD_SA		= 15,	//	사격치.
		true, // EMADD_MA		= 16,	//	마력치.
		true, // EMADDEX_NONE			= 17,
		false, // EMADDEX_INCR_HP				= 18,
		false, // EMADDEX_INCR_MP				= 19,
		false, // EMADDEX_INCR_SP				= 20,
		false, // EMADDEX_INCR_AP				= 21,
		false, // EMADDEX_INCR_MOVE_SPEED		= 22,
		false, // EMADDEX_INCR_ATTACK_SPEED	= 23,
		false, // EMADDEX_INCR_CRITICAL_RATE	= 24,
		false, // EMADDEX_INCR_CRUSHING_BLOW	= 25,
		true, // EMADDEX_INC_NONE			= 26,
		true, // EMADDEX_INC_HP				= 27,
		true, // EMADDEX_INC_MP				= 28,
		true, // EMADDEX_INC_SP				= 29,
		true, // EMADDEX_INC_AP				= 30,
		true, // EMADDEX_INC_MOVE_SPEED		= 31,
		false, // EMADDEX_INC_ATTACK_SPEED	= 32,
		false, // EMADDEX_INC_CRITICAL_RATE	= 33,
		true, // EMADDEX_INC_CRUSHING_BLOW	= 34,
		false, // EMADDEX_INCR_IGNORE_BOSSDAMAGE			= 35,		// 보스몹에게서 피격될시 일정 데미지 무
		true, // EMADDEX_BOSSFIXEDDAMAGE				= 36,			// 보스몹에게 고정데미지;
		false, // EMADDEX_INCR_CRITICAL_EMERGENCYMOB	= 37,	// 위급 상태의 몹에게 크리티컬 발생률 증가;
		false, // EMADDEX_INCR_CRITICAL_EMERGENCYBOSS = 38,	// 위급 상태의 보스에게 크리티컬 발생률 증가
		false, // EMADDEX_DEC_DAMAGE_EMERGENCYSTATE	= 39, 	// 위급 상태일 경우 데미지 감소;
		false, // EMADDEX_INCR_EXPADDEFFECT			= 40,	// '경험치 증가율 효과'를 증가시킴;
		false, // EMADDEX_INCR_DROPGENMONEY			= 41,	// 몹 사냥시 드랍되는 돈 증가율;
		false, // EMADDEX_DECR_DAMAGE_MELEESKILL		= 42,	// 물리 근접 스킬 피격시 경우 데미지 감소;
		false, // EMADDEX_DECR_DAMAGE_RANGESKILL		= 43,	// 사격 스킬 피격시 경우 데미지 감소;
		false, // EMADDEX_DECR_DAMAGE_MAGICSKILL		= 44,	// 마법 스킬 피격시 경우 데미지 감소;
		true, // EMADDEX_INC_HP_SOLOPLAY				= 45,		// 솔로플레이시 HP증가 (파티가 아닐시);
		true,// EMADDEX_INC_HP_PARTYPLAY			= 46,	// 파티플레이시 HP증가;
		false,// EMADDEX_INCR_ATKSPEED_SOLOPLAY		= 47,	// 솔로플레이시 공속 증가 (파티가 아닐시);
		true,// EMADDEX_INCR_ATKSPEED_PARTYPLAY		= 48,	// 파티플레이시 공속 증가;
		// EMADD_RANDOM	= 49,	//	랜덤;
	};

}; // namespace ITEM

// 구시대 유물이다. 사용하면안된다;
enum EMITEM_VAR
{
	EMVAR_NONE			= 0,
	EMVAR_HP			= 1,
	EMVAR_MP			= 2,
	EMVAR_SP			= 3,
	EMVAR_AP			= 4,
	EMVAR_MOVE_SPEED	= 5,
	EMVAR_ATTACK_SPEED	= 6,
	EMVAR_CRITICAL_RATE	= 7,
	EMVAR_CRUSHING_BLOW	= 8,
	
	EMVAR_IGNORE_BOSSDAMAGE	= 9,		// 보스몹에게서 피격될시 일정 데미지 무시;
	EMVAR_BOSSFIXEDDAMAGE = 10,			// 보스몹에게 고정데미지;
	EMVAR_INCR_CRITICAL_EMERGENCYMOB = 11,	// 위급 상태의 몹에게 크리티컬 발생률 증가;
	EMVAR_INCR_CRITICAL_EMERGENCYBOSS = 12,	// 위급 상태의 보스에게 크리티컬 발생률 증가;
	EMVAR_DEC_DAMAGE_EMERGENCYSTATE = 13, 	// 위급 상태일 경우 데미지 감소;
	EMVAR_INCR_EXPADDEFFECT = 14,	// '경험치 증가율 효과'를 증가시킴;
	EMVAR_INCR_DROPGENMONEY = 15,	// 몹 사냥시 드랍되는 돈 증가율;
	EMVAR_DECR_DAMAGE_MELEESKILL = 16,	// 물리 근접 스킬 피격시 경우 데미지 감소;
	EMVAR_DECR_DAMAGE_RANGESKILL = 17,	// 사격 스킬 피격시 경우 데미지 감소;
	EMVAR_DECR_DAMAGE_MAGICSKILL = 18,	// 마법 스킬 피격시 경우 데미지 감소;
	EMVAR_INC_HP_SOLOPLAY = 19,		// 솔로플레이시 HP증가 (파티가 아닐시);
	EMVAR_INC_HP_PARTYPLAY = 20,	// 파티플레이시 HP증가;
	EMVAR_INCR_ATKSPEED_SOLOPLAY = 21,	// 솔로플레이시 공속 증가 (파티가 아닐시);
	EMVAR_INCR_ATKSPEED_PARTYPLAY = 22,	// 파티플레이시 공속 증가;

	EMVAR_SIZE = 23,
};

enum EMITEM_QUESTION
{
	QUESTION_NONE			= 0,
	QUESTION_SPEED_UP		= 1,
	QUESTION_CRAZY			= 2,
	QUESTION_ATTACK_UP		= 3,
	QUESTION_EXP_UP			= 4,
	QUESTION_EXP_GET		= 5,
	QUESTION_LUCKY			= 6,
	QUESTION_BOMB			= 7,
	QUESTION_MOBGEN			= 8,

	QUESTION_SPEED_UP_M		= 9,
	QUESTION_MADNESS		= 10,
	QUESTION_ATTACK_UP_M	= 11,
	QUESTION_HEAL			= 12,

	QUESTION_SIZE			= 13,
};

// enum EMRANDOM_OPT
// {
// 	EMR_OPT_NULL			= 0,
// 	EMR_OPT_DAMAGE			= 1,	//	공격력.
// 	EMR_OPT_DEFENSE			= 2,	//	방어력.
// 
// 	EMR_OPT_HITRATE			= 3,	//	명중율.
// 	EMR_OPT_AVOIDRATE		= 4,	//	회피율.
// 
// 	EMR_OPT_HP				= 5,	//	hp 증가.
// 	EMR_OPT_MP				= 6,	//	mp 증가.
// 	EMR_OPT_SP				= 7,	//	sp 증가.
// 
// 	EMR_OPT_HP_INC			= 8,	//	hp 증가율.
// 	EMR_OPT_MP_INC			= 9,	//	mp 증가율.
// 	EMR_OPT_SP_INC			= 10,	//	sp 증가율.
// 	EMR_OPT_HMS_INC			= 11,	//	hp, mp, sp 증가율.
// 
// 	EMR_OPT_GRIND_DAMAGE	= 12,	//	공격력 연마.
// 	EMR_OPT_GRIND_DEFENSE	= 13,	//	방어력 연마.
// 
// 	EMR_OPT_RANGE			= 14,	//	공격 영역.
// 	EMR_OPT_DIS_SP			= 15,	//	sp 소모량.
// 	EMR_OPT_RESIST			= 16,	//	저항.
// 
// 	EMR_OPT_MOVE_SPEED		= 17,	// 이동속도 증가
// 
// 	EMR_OPT_DAMAGE_INT		= 18,	//	공격력(정수).
// 	EMR_OPT_DEFENSE_INT		= 19,	//	방어력(정수).
// 
// 	EMR_OPT_HITRATE_INT		= 20,	//	명중율(정수).
// 	EMR_OPT_AVOIDRATE_INT	= 21,	//	회피율(정수).
// 
// 	EMR_OPT_STATS_POW_INT	= 22,	//	힘(정수).
// 	EMR_OPT_STATS_STR_INT	= 23,	//	체력(정수).
// 	EMR_OPT_STATS_SPI_INT	= 24,	//	정신(정수).
// 	EMR_OPT_STATS_DEX_INT	= 25,	//	민첩(정수).
// 	EMR_OPT_STATS_STA_INT	= 26,	//	근력(정수).
// 
// 	EMR_OPT_PA_INT				= 27,	//	격투치(정수).
// 	EMR_OPT_SA_INT				= 28,	//	사격치(정수).
// 	EMR_OPT_MA_INT				= 29,	//	마력치(정수).
// 
// 	EMR_OPT_HP_POTION_INC_INT	= 30,	//	포션 hp 증가량(정수).
// 	EMR_OPT_MP_POTION_INC_INT	= 31,	//	포션 mp 증가량(정수).
// 	EMR_OPT_SP_POTION_INC_INT	= 32,	//	포션 sp 증가량(정수).
// 	EMR_OPT_CP_INC_INT			= 33,	//	몬스터 킬시 CP 증가량(정수).
// 
// 	EMR_OPT_SIZE				= 34,
// };

enum EMCOOL_TYPE
{
	EMCOOL_ITEMID	= 0,	// MID/SID
	EMCOOL_ITEMTYPE	= 1,	// 아이템 타입
	EMCOOL_SIZE		= 2,	// 사이즈
};

enum EMPARTS_TYPE
{
	EMBIKE_BT5	= 0,	// 바이크 종류
	EMBIKE_BT7	= 1,	
	EMBIKE_BT9	= 2,
	EMBIKE_EV1	= 3,
	EMBIKE_SIZE	= 4,	// 바이크 현재 사이즈
	EMBIKE_NSIZE = 10,	// 최대 사이즈
	
	EMBOARD_HOVER = 0,	// 보드 종류	
	EMHOVER_SIZE = 1,	// 보드 현재 사이즈

	// Car
	EMCAR_FERRARI = 0, // EMCAR_A
	EMCAR_B = 1,
	EMCAR_C = 2,
	EMCAR_D = 3,
	EMCAR_SIZE = 4,

	// RearCar
	EMREARCAR_A = 0,
	EMREARCAR_B = 1,
	EMREARCAR_C = 2,
	EMREARCAR_D = 3,
	EMREARCAR_SIZE = 4,
};

enum EMITEM_SKILL_LINK
{
	EMITEM_SKILL_ALL    = 0,
	EMITEM_SKILL_SELF   = 1,
	EMITEM_SKILL_ENERMY = 2,
	EMITEM_SKILL_MOB    = 3,
	EMITEM_SKILL_SIZE
};

struct SRANDOM_OPT
{
	WORD	wTYPE;
	short	nVALUE;

	SRANDOM_OPT()
		: wTYPE( RandomOption::RANDOM_OPTION_NULL )
		, nVALUE( 0 )
	{
	}

	float get_float()	{ return nVALUE * 0.01f; }
	float get_int()		{ return nVALUE; }
};


enum EMGRINDING_CLASS
{
	EMGRINDING_CLASS_ARM	= 0,
	EMGRINDING_CLASS_CLOTH	= 1,

	EMGRINDING_CLASS_SIZE	= 2,
};

enum EMGRINDING_TYPE
{
	EMGRINDING_NUNE				= 0,

	EMGRINDING_DAMAGE			= 1,
	EMGRINDING_DEFENSE			= 2,

	EMGRINDING_RESIST_FIRE		= 3,
	EMGRINDING_RESIST_ICE		= 4,
	EMGRINDING_RESIST_ELEC		= 5,
	EMGRINDING_RESIST_POISON	= 6,
	EMGRINDING_RESIST_SPIRIT	= 7,
	
	EMGRINDING_NSIZE			= 8
};

enum EMGRINDING_ATTRIBUTE
{
	EMGRINDING_ATTRIBUTE_NULL			  = 0x0000,
	EMGRINDING_ATTRIBUTE_RANDOM			  = 0x0001, // 범위 랜덤 연마
	EMGRINDING_ATTRIBUTE_NODEGRATION	  = 0x0002, // 연마등급이 감소되지 않음
	EMGRINDING_ATTRIBUTE_IGNOREPROTECTED  = 0x0004, // 연마보호(완화주,소방주 등)를 무시
	EMGRINDING_ATTRIBUTE_IGNORERATEINC	  = 0x0008, // 확률증가(강화주,이벤트 등)를 무시
	EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ = 0x0010, // 기본 요구사항 무시 (연마제 등급 체크를 무시하거나, 연마제 소모개수를 무조건 한개로 통일한다)
	EMGRINDING_ATTRIBUTE_ADDED			  = 0x0020, // 결과 연마수치를 무조건 더함 (단독으로 사용시 아무효과 없음 RANDOM과 같이 사용해야함)
};

enum EMGRINDER_TYPE
{
	EMGRINDER_NORMAL			= 0,
	EMGRINDER_HIGH				= 1,
	EMGRINDER_TOP				= 2,

	EMGRINDER_SIZE				= 3,
};

enum EMGRINDING_RS_FLAGS
{
	EMGRINDING_RS_FIRE		= 0x01,
	EMGRINDING_RS_ICE		= 0x02,
	EMGRINDING_RS_ELECTRIC	= 0x04,
	EMGRINDING_RS_POISON	= 0x08,
	EMGRINDING_RS_SPIRIT	= 0x10,

	EMGRINDING_RS_NSIZE		= 5,
	EMGRINDING_NO			= 2,
	EMGRINDING_RS_ALL		= (EMGRINDING_RS_FIRE|EMGRINDING_RS_ICE|EMGRINDING_RS_ELECTRIC|EMGRINDING_RS_POISON|EMGRINDING_RS_SPIRIT)
};

enum EMGRINDING_RS_INDEX
{
	EMGRINDING_RS_FIRE_INDEX		= 0,
	EMGRINDING_RS_ICE_INDEX			= 1,
	EMGRINDING_RS_ELECTRIC_INDEX	= 2,
	EMGRINDING_RS_POISON_INDEX		= 3,
	EMGRINDING_RS_SPIRIT_INDEX		= 4,

	EMGRINDING_RS_INDEX_NSIZE		= 5
};

enum EMGRINDINGOPTION
{
	EMGRINDINGOPTION_NONE			       = 0x0000, // 옵션 없음
	EMGRINDINGOPTION_ANTIDISAPPEAR	       = 0x0001, // 소방주 착용 (소멸 금지)
	EMGRINDINGOPTION_INC_GRINDINGRATE      = 0x0002, // 강화주 착용 (강화 확률 증가)
	EMGRINDINGOPTION_ANTIRESET			   = 0x0004, // 완화주 사용 (리셋 단계 감소)
};

enum EMGRINDINGCONDITION
{
	EMGRINDINGCONDITION_FAILED		= 0, // 일반 실패
	EMGRINDINGCONDITION_SUCCESS		= 1, // 성공
	EMGRINDINGCONDITION_NOITEM		= 2, // 아이템이 없음
	EMGRINDINGCONDITION_MAX			= 3, // 연마 최대치 도달
	EMGRINDINGCONDITION_NOTBEST		= 4, // 연마제가 최상급이 아님
	EMGRINDINGCONDITION_NOTHIGH		= 5, // 연마제가 상급이 아님
	EMGRINDINGCONDITION_NOTBESTITEM	= 6, // 최상급 이상으로 연마할 수 없는 아이템
	EMGRINDINGCONDITION_NOTNUM		= 7, // 연마제 갯수 부족
	EMGRINDINGCONDITION_DEFCLASS	= 8, // 아이템과 연마제가 타입이 다름
	EMGRINDINGCONDITION_MIN			= 9,  // 최소 연마가능 수치 미만인 아이템에 사용할 경우
	EMGRINDINGCONDITION_RANDOM_MAX	= 10, // 현재 아이템의 연마수치보다, 범위 연마제의 최대 연마 수치가 같거나 작을 경우,
	EMGRINDINGCONDITION_TIME_LIMIT  = 11, // 일시강화에서 사용 - 기간제일 경우 불가
	EMGRINDINGCONDITION_RE_ENCHANT  = 12, // 일시강화에서 사용 - 재강화
	EMGRINDINGCONDITION_TLGRIND_ENCHANT  = 13, // 일시강화에서 사용 - 일시강화가 부여되어 있다.
};

enum EMREMODEL_TYPE
{
	EMREMODEL_TYPE_NONE = 0,
	EMREMODEL_TYPE_DAMAGE = 1,
	EMREMODEL_TYPE_DEFENCEANDRESIST = 2,
};

enum EMREBUILD_FIXED_BASICSTATS
{
	EMREBUILD_FIXED_BASICSTATS_NONE				= 0x0000,
	EMREBUILD_FIXED_BASICSTATS_ATTACKDAMAGE		= 0x0001,
	EMREBUILD_FIXED_BASICSTATS_DEFENCE			= 0x0002,
};

EMGRINDING_RS_INDEX	GrindRsClassToIndex ( const EMGRINDING_RS_FLAGS emClass );
EMGRINDING_RS_FLAGS	GrindIndexToRsClass ( const EMGRINDING_RS_INDEX	emIndex );

struct SGRIND_OLD
{
	EMGRINDING_TYPE	emTYPE;

	DWORD			dwRESIST;
	int				nMin;
	int				nMax;

	SGRIND_OLD () :
		emTYPE(EMGRINDING_NUNE),

		dwRESIST(NULL),
		nMin(0),
		nMax(0)
	{
	}
};

class CGrindingScript;

struct SGRINDING_OPT
{
	typedef std::map<WORD,float>	MAP_CHANCE;
	typedef MAP_CHANCE::iterator	MAP_CHANCE_ITER;

	typedef std::map<WORD,float>	MAP_RESET;
	typedef MAP_RESET::iterator		MAP_RESET_ITER;
	
	typedef std::map<WORD,float>	MAP_TERMINATE;
	typedef MAP_TERMINATE::iterator MAP_TERMINATE_ITER;

	EMGRINDING_TYPE	 emTYPE;
	EMGRINDER_TYPE	 emLEVEL;
	EMGRINDING_CLASS emPURPOSE;
	DWORD			 dwATTRIBUTE;
	WORD			 wCONSUME;
	MAP_CHANCE		 mapCHANCE;
	MAP_RESET		 mapRESET;
	MAP_TERMINATE	 mapTERMINATE;
	WORD			 wENCHANT_MAX;
	WORD			 wENCHANT_MIN;
	WORD			 wRANGE_MAX;
	WORD			 wRANGE_MIN;

	std::tr1::shared_ptr<CGrindingScript> spSCRIPT;

	SGRINDING_OPT()
		: emTYPE(EMGRINDING_NSIZE)
		, emLEVEL(EMGRINDER_SIZE)
		, emPURPOSE(EMGRINDING_CLASS_SIZE)
		, dwATTRIBUTE(0)
		, wCONSUME(0)
		, wRANGE_MIN(USHRT_MAX)
		, wRANGE_MAX(0)
		, wENCHANT_MIN(USHRT_MAX)
		, wENCHANT_MAX(0)
	{
	}

	const bool ISTYPENULL()			 { return (emTYPE == EMGRINDING_NSIZE); }
	const bool ISLEVELNULL()		 { return (emLEVEL == EMGRINDER_SIZE); }
	const bool ISPURPOSENULL()		 { return (emPURPOSE == EMGRINDING_CLASS_SIZE); }
	const bool ISCONSUMENULL()		 { return (wCONSUME == 0); }

	const bool ISENCHANTMAXNULL()	 { return (wENCHANT_MAX == 0); }
	const bool ISENCHANTMINNULL()	 { return (wENCHANT_MIN == USHRT_MAX); }

	const bool ISRANGEMAXNULL()		 { return (wRANGE_MAX == 0); }
	const bool ISRANGEMINNULL()		 { return (wRANGE_MIN == USHRT_MAX); }

	const bool ISCHANCELISTNULL()	 { return (mapCHANCE.empty()); }
	const bool ISRESETLISTNULL()	 { return (mapRESET.empty()); }
	const bool ISTERMINATELISTNULL() { return (mapTERMINATE.empty()); }
	const bool ISENABLELOGIC();
	const bool ISENABLELOGIC_CONDITION();
	const bool ISENABLELOGIC_CONSUME();

	CGrindingScript* GETSCRIPT();
};

struct SGRINDING_DATA
{
	std::string		strNAME;
	SGRINDING_OPT*	pSET;

	SGRINDING_DATA()
		: pSET( NULL )
	{
	}

	~SGRINDING_DATA()
	{
	}
};

struct SGRINDING_DATA_OP
{
	bool operator() ( const SGRINDING_DATA &lvalue, const SGRINDING_DATA &rvalue )
	{
		return lvalue.strNAME < rvalue.strNAME;
	}

	bool operator() ( const SGRINDING_DATA &lvalue, const std::string strNAME )
	{
		return lvalue.strNAME < strNAME;
	}

	bool operator() ( const std::string strNAME, const SGRINDING_DATA &rvalue )
	{
		return strNAME < rvalue.strNAME;
	}
};

struct ITEM_COOLTIME
{
	enum { VERSION = 0x0001 };

	DWORD		dwID;		//	사용한 아이템 ID
	DWORD		dwCoolID;	//	쿨타임 ID ( 타입에 따라 다름 ) 
	__time64_t	tUseTime;	//	사용시간
	__time64_t	tCoolTime;	//	사용가능한 시간

	ITEM_COOLTIME()
		: dwID(0)
		, dwCoolID(0)
		, tUseTime(0)
		, tCoolTime(0)
	{
	}

    ITEM_COOLTIME& ITEM_COOLTIME::operator = (const ITEM_COOLTIME& rhs)
    {
        if (this != &rhs)
        {
            dwID = rhs.dwID;
            dwCoolID = rhs.dwCoolID;
            tUseTime = rhs.tUseTime;
            tCoolTime = rhs.tCoolTime;
        }
        return *this;
    }

    bool IsZeroTime() const 
    {
        if (tUseTime == 0 && tCoolTime == 0)
            return true;
        else
            return false;
    }

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & dwID;
		ar & dwCoolID;
		ar & tUseTime;
		ar & tCoolTime;
	}
};

enum ACCESSORYTYPE
{
    ACCETYPEA		= 0, //! 머리, 몸통
    ACCETYPEB		= 1, //! 팔, 다리, 꼬리
    ACCETYPESIZE	= 2,
};

namespace ITEM
{
	struct SVAR
	{
		EMITEM_VAR		emTYPE;		//	적용 타입.
		float			fVariate;	//	적용 값.

		SVAR ()
			: emTYPE(EMVAR_NONE)
			, fVariate(0.0f)
		{
		}
	};

	struct SVOL
	{
		EMITEM_VAR	emTYPE;		//	적용 타입.
		float		fVolume;	//	적용 값.

		SVOL ()
			: emTYPE(EMVAR_NONE)
			, fVolume(0)
		{
		}
	};
} // namespace ITEM

enum EM_ITEM_MIX
{
    ITEMMIX_ITEMNUM = 5,
};

enum EM_ITEM_DURABILITY
{
    DURABILITY_RATE_COUNT = 8,
};

enum EM_ITEM_DURABILITY_APPLY
{
	DURABILITY_APPLY_BASIC,
	DURABILITY_APPLY_RANDOM_OPTION,
	DURABILITY_APPLY_ADD,
	DURABILITY_APPLY_EMPTY1,
	DURABILITY_APPLY_EMPTY2,
	DURABILITY_APPLY_EMPTY3,
	DURABILITY_APPLY_COUNT = 6,
};

enum EMITEM_SUIT_STATE
{
	EMITEM_SUIT_NULL					= 0x00000000,
	EMITEM_SUIT_CHANGE_COLOR	= 0x00000001, // 염색 가능;
	EMITEM_SUIT_UNIQUE				= 0x00000002, // 고유 착용(동시 착용 금지);
};

namespace COMMENT
{
	extern std::string ITEMLEVEL[EMITEM_LEVEL_NSIZE];
	
    //! 아이템 타입
    extern std::string ITEMTYPE[ITEM_NSIZE];
    std::string ItemTypeName(EMITEM_TYPE Type);

	extern std::string ITEMSUIT[SUIT_NSIZE];
    std::string ItemSuitName(EMSUIT Type);

	//extern std::string ITEMSLOT[SLOT_NSIZE_S]; // 사용안함.
	extern std::string ITEMATTACK[ITEMATT_NSIZE+1];

	extern std::string ITEMDRUG_ER[ITEM_DRUG_ER_SIZE];
	extern std::string ITEMDRUG[ITEM_DRUG_SIZE];
	extern std::string ITEMDRUG_VAR[ITEM_DRUG_SIZE];

	extern std::string ITEMPROHIBIT[PROHIBIT_ITEM_SIZE];

	extern std::string ITEMADDON[EMADD_SIZE];
	
	extern std::string ITEMVAR[EMVAR_SIZE];
	extern float ITEMADDON_VAR_SCALE[EMADD_SIZE];

	extern std::string ITEMVOL[EMVAR_SIZE]; // by 경대
	extern std::string ITEM_QUE_TYPE[QUESTION_SIZE]; // Question Item
	extern std::string ITEM_QUE_VAR1[QUESTION_SIZE];
	extern std::string ITEM_QUE_VAR2[QUESTION_SIZE];

	extern float ITEMVAR_SCALE[EMVAR_SIZE];
	bool IsITEMVAR_SCALE ( EMITEM_VAR emITEM_VAR );

	extern std::string GRINDING_TYPE[EMGRINDING_NSIZE];
	extern std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE];
	extern std::string GRINDING_LEVEL[EMGRINDER_SIZE];

	extern std::string ITEM_RANDOM_OPT[RandomOption::RANDOM_OPTION_NSIZE];
	extern float ITEM_RANDOM_OPT_SCALE[RandomOption::RANDOM_OPTION_NSIZE];

	extern DWORD	ITEMCOLOR[EMITEM_LEVEL_NSIZE];

	extern std::string COOLTYPE[EMCOOL_SIZE];

	extern std::string BIKETYPE[EMBIKE_NSIZE];
	extern std::string BOARDTYPE[EMBIKE_NSIZE];
	extern std::string CARTYPE[EMBIKE_NSIZE];
	extern std::string REARCARTYPE[EMBIKE_NSIZE];
	extern std::string SKILL_LINK_TYPE[EMITEM_SKILL_SIZE];
}


// 코스튬에 적용되는 아이템 속성 타입.
enum COSTUME_STATS {
	E_NONE			  = 0,
	// 일반 속성
	E_EXPMULTIPLE     = 1,	// 경험치(-327.00% ~ +327.00%)치
	//E_HITRATE,				// 명중률률
	//E_AVOIDRATE,			// 회피율율
	//E_DAMAGE,				// 공격력(Low, High 같이 사용)력
	E_DEFENCE,				// 방어력력
	//E_RESIST_FIRE,			// 저항(불)불)
	//E_RESIST_ICE,			// 저항(얼음)
	//E_RESIST_ELECTRIC,		// 저항(전기)
	//E_RESIST_POISON,		// 저항(독)
	//E_RESIST_SPIRIT,		// 저항(정기)
	E_RESIST,
	E_EMPTY,

	E_GENERAL_MAX     = E_EMPTY,

	// 가산 효과			
	E_ADD_HITRATE	  = 5,	// 명중률
	E_ADD_AVOIDRATE,		// 회피율
	E_ADD_DAMAGE,			// 공격력
	E_ADD_DEFENSE,			// 방어력
	E_ADD_HP,				// 체력(HP)
	//E_ADD_MANA,				// 마나
	//E_ADD_STEMINA,			// 스테미너
	E_ADD_STATS_POW,		// Stats 힘
	E_ADD_STATS_STA,		// Stats 체력
	E_ADD_STATS_SPI,		// Stats 정신
	E_ADD_STATS_DEX,		// Stats 민첩
	//E_ADD_STATS_INT,		// Stats 지력
	//E_ADD_STATS_STR,		// Stats 근력
	E_ADD_ENERGY,			// 기력치
	E_ADD_SHOOTING,			// 사격치
	E_ADD_MELEE,			// 격투치
	E_ADD_EMPTY1,
	E_ADD_EMPTY2,
	E_ADD_EMPTY3,

	E_ADDED_MAX		  = E_ADD_EMPTY3 - E_EMPTY,
	// 변화율				
	E_R_HP_INC		  = 20,	// HP 증가율
	E_R_MP_INC,				// MP 증가율
	E_R_SP_INC,				// SP 증가율
	E_R_HMSP_INC,			// HP+MP+SP 증가율
	E_R_MOVESPEED,			// 이동속도 증가율
	E_R_ATTACKSPEED,		// 공격속도 증가율
	E_R_CRITICALHIT,		// 크리티컬 발생확률
	E_R_STRIKEHIT,			// 강한타격 발생확률
	E_R_EMPTY,
	
	E_VARRATE_MAX		  = E_R_EMPTY - E_ADD_EMPTY3,
	
	// 변화량				
	//E_A_HP_INC		  = 35,	// HP 증가량
	//E_A_MP_INC,				// MP 증가량
	//E_A_SP_INC,				// SP 증가량
	//E_A_HMSP_INC,			// HP+MP+SP 증가량
	//E_A_MOVESPEED,			// 이동속도 증가량
	//E_A_ATTACKSPEED,		// 공격속도 증가량
	//E_A_CRITICALHIT,		// 크리티컬 발생량
};	

#define MAX_GEN		E_GENERAL_MAX - 1
#define MAX_ADDED   E_ADDED_MAX   - 3
#define MAX_VAR     E_VARRATE_MAX - 1


#define ID_STAT(id, result) if( E_EXPMULTIPLE <= id && id <= E_EMPTY ) { result = id - E_EXPMULTIPLE; } \
							else if ( E_ADD_HITRATE <= id && id <= E_ADD_EMPTY3 ) { result = id - E_ADD_HITRATE; } \
							else if ( E_R_HP_INC    <= id && id <= E_R_EMPTY ) { result = id - E_R_HP_INC; }
							

//! 아이템 인벤토리 타입
//! db 에서 사용하므로 변경하면 안됨
enum INVEN_TYPE
{
	INVEN_DELETED		= 0,	//! 지워진 아이템(필드에 버리거나 그런 경우이다.)
	INVEN_PUTON			= 1,	//! 착용아이템
	INVEN_INVEN			= 2,	//! 일반 인벤토리
	INVEN_QUEST_PROC	= 3,	//! 진행중인 Quest Item
	INVEN_QUEST_END		= 4,	//! 완료된 Quest Item
	INVEN_POST			= 5,	//! 우편으로 보내지고 있는 아이템
	INVEN_LOCKER		= 6,	//! 캐릭터 락커
	INVEN_CLUB_LOCKER	= 7,	//! 클럽 락커
	INVEN_VEHICLE		= 8,	//! Vehicle
	INVEN_PET			= 9,	//! Pet
	INVEN_DELETED2		= 10,	//! 지워진 아이템(실제로 상점에 팔거나 시간이 다되어 삭제되거나 정말 삭제된 경우이다.)
};

enum CALL_TYPE
{
	INSERT_PUTON_ITEM, 
	INSERT_INVEN_ITEM, 
	INSERT_USER_LOCKER_ITEM, 
	INSERT_QUEST_ITEM, 
	INSERT_QUEST_END_ITEM, 
	INSERT_PET_ITEM, 
	INSERT_VEHICLE_ITEM, 
	INSERT_CLUB_LOCKER_ITEM, 
	INSERT_CLUB_LOCKER_ITEM_FOR_TRANSFER, 
	INSERT_USER_LOCKER_ITEM_FOR_TRANSFER, 
	INSERT_USER_LOCKER_ITEM_FOR_TRANSFER_ALL, 
	INSERT_PUTON_ITEM_FOR_TRANSFER_ALL, 
	INSERT_INVEN_ITEM_FOR_TRANSFER_ALL, 
	INSERT_QUEST_ITEM_FOR_TRANSFER_ALL, 
	INSERT_QUEST_END_ITEM_FOR_TRANSFER_ALL, 
	INSERT_PET_ITEM_FOR_TRANSFER_ALL, 
	INSERT_VEHICLE_ITEM_FOR_TRANSFER_ALL, 
	INSERT_PUTON_ITEM_FOR_TRANSFER, 
	INSERT_INVEN_ITEM_FOR_TRANSFER, 
	INSERT_QUEST_ITEM_FOR_TRANSFER, 
	INSERT_QUEST_END_ITEM_FOR_TRANSFER, 
	INSERT_PET_ITEM_FOR_TRANSFER, 
	INSERT_VEHICLE_ITEM_FOR_TRANSFER, 
	UPDATE_PUTON_ITEM, 
	UPDATE_INVEN_ITEM, 
	UPDATE_USER_LOCKER_ITEM, 
	UPDATE_QUEST_ITEM, 
	UPDATE_QUEST_END_ITEM, 
	UPDATE_PET_ITEM, 
	UPDATE_VEHICLE_ITEM, 
	UPDATE_CLUB_LOCKER_ITEM, 
	DELETE_QUEST_ITEM_LOAD, 
	DELETE_PUTON_ITEM, 
	DELETE_INVEN_ITEM, 
	DELETE_USER_LOCKER_ITEM, 
	DELETE_QUEST_ITEM, 
	DELETE_QUEST_END_ITEM, 
	DELETE_PET_ITEM, 
	DELETE_VEHICLE_ITEM, 
	DELETE_CLUB_LOCKER_ITEM, 
	INSERT_INVEN_ITEM_CHARGED_ITEM, 
	CONSUME_INVEN_ITEM, 
	INSERTTOINVEN_PDROPITEM, 
	DELETE_GARBAGE_RESULT, 
	DELETE_PUTON_ITEM_RELEASE_SLOT, 
	VEHICLE_PUTON_ITEM_RESET, 
	INSERT_INVEN_PILE_ITEM, 
	INSERT_INVEN_BUY_FROM_NPC_NOT_PILEITEM, 
	INSERT_INVEN_REBUY_ITEM, 
	DELETE_INVEN_ITEM_PMARKET_SELL, 
	INSERT_INVEN_ITEM_PMARKET_BUY, 
	DELETE_INVEN_ITEM_PILEITEM, 
	DELETE_INVEN_ITEM_SELL_TO_NPC, 
	DELLTE_PET_INVEN_TIMELIMIT_ITEM, 
	DELETE_INVEN_ITEM_RESET_TIMELMT_ITEM, 
	INSERT_PUTON_ITEM_SLOT_ITEM_SERVER, 
	UPDATE_PUTON_ITEM_SLOT_ITEM_SERVER, 
	INSERT_INVEN_DELETED, 
	UPDATE_INVEN_DELETED, 
	INSERT_INVEN_ITEM_RESET_TIMELMT_ITEM_RESTORE_COSTUME, 
	DELETE_USER_LOCKER_ITEM_TIMELIMIT,
	INSERT_USER_LOCKER_ITEM_TIMELIMIT_ITEMDELETE_AND_RESTORE_COSTUME, 
	INSERTTOINVEN_PDROPITEM_EX, 
	DELETE_ITEM_CHECKITEMDURATION, 
	INSERT_ITEM_CHECKITEMDURATION, 
	INSERT_INVEN_ITEM_IVEN_DISGUISE, 
	DELETE_ITEM_GRINDING_TERMINATE,  
	INSERT_ITEM_GRINDING_TERMINATE_RESTORE_COSTUME, 
	INVEN_COUNT_DELETE_ITEM, 
	GM_MAKE_ITEM, 
	MSG_INVEN_EX_INVEN_ERROR, 
	MSG_INVEN_EX_INVEN_ERROR2, 
	MSG_INVEN_EX_INVEN_INVENITEM_INSERT, 
	MSG_INVEN_EX_INVEN_HOLDITEM_INSERT, 
	MSG_STORAGE_EX_INVEN_ERROR, 
	INVEN_TO_STORAGE, 
	MSG_REQINVENTOFIELD, 
	PET_PUTON_ITEM_RESET, 
	DELLTE_VEHICLE_INVEN_TIMELIMIT_ITEM, 
	UPDATE_PUTON_ITEM_SLOT_UPDATE_ITEM, 
	INSERT_PUTON_ITEM_NEW_CHA, 
	INSERT_INVEN_ITEM_NEW_CHA, 
	INSERT_QUEST_ITEM_NEW_CHA, 
	INSERT_QUEST_END_ITEM_NEW_CHA, 
	PERIOD_EXTENSION_ITEM,
};

namespace db
{
	enum ITEM_DB_STATE
	{
		DB_NONE   = 0, 
		DB_INSERT = 1, 
		DB_UPDATE = 2, 
		DB_DELETE = 3, 
	};

	class State
	{
	public:
		State();
		~State() {}

	protected:
		int m_nState;

	public:
		MSGPACK_DEFINE( m_nState );

		const int getState() const { return m_nState; }
		void setState( int emState, bool bForce = false );

	private:
		friend class boost::serialization::access;
		template <typename Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & m_nState;
		}
	};
}
