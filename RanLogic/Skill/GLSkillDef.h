
#pragma once

#include "../Item/GLItemDef.h"

#define SKILLID_NULL SKILLID()

struct SKILLID
{
	SNATIVEID sSKILLID;
	WORD	  wLEVEL;

	SKILLID();
	explicit SKILLID( const SNATIVEID& _sSKILLID );
	SKILLID( const SNATIVEID& _sSKILLID, const WORD _LEVEL );
	
	void RESET();
	const bool operator == ( const SKILLID& rhs ) const;
	const bool operator != ( const SKILLID& rhs ) const;

	const WORD mid() const  { return sSKILLID.Mid(); }
	const WORD sid() const  { return sSKILLID.Sid(); }
};

namespace SKILL
{
	// Csv 로딩할 때 사용
	extern int iCsvCur;

	enum
	{
		MAX_SZNAME			= 32,
		MAX_SZFILE			= 68,
		MAX_SZCOMMENT		= 128,

		MAX_LEVEL_BEFORE	= 9,
		MAX_LEVEL			= 24,
		MAX_ADDON			= 5,	//	Addon 최대지 
		MAX_EFFTARGETPOS	= 10,
		MAX_GRADE			= 25,
	};

	enum EMTYPES
	{
		EMFOR_HP				= 0,	//	체력치 변화.
		EMFOR_MP				= 1,	//	기력치 변화.
		EMFOR_SP				= 2,	//	근력치 변화.

		EMFOR_VARHP				= 3,	//	채력치 변화율.
		EMFOR_VARMP				= 4,	//	기력치 변화율.
		EMFOR_VARSP				= 5,	//	근력치 변화율.

		EMFOR_DEFENSE			= 6,	//	방어치 변화.
		EMFOR_CURE				= 7,	//	해독. ( 해독 종류 필요 )

		EMFOR_HITRATE			= 8,	//	명중율.
		EMFOR_AVOIDRATE			= 9,	//	회피율.

		EMFOR_VARAP				= 10,	//	모든 (체력,기력,근력) 변화율.

		EMFOR_VARDAMAGE			= 11,	//	공격치 변화량.
		EMFOR_VARDEFENSE		= 12,	//	방어치 변화량.

		EMFOR_PA				= 13,	//	격투치.
		EMFOR_SA				= 14,	//	사격치.
		EMFOR_MA				= 15,	//	방어치.

		EMFOR_HP_RATE			= 16,	//	HP 증폭율.
		EMFOR_MP_RATE			= 17,	//	MP 증폭율.
		EMFOR_SP_RATE			= 18,	//	SP 증폭율.

		EMFOR_PET_GETALLITEMS	= 19,	//	모든 아이템 줍기	// PetData
		EMFOR_PET_GETRAREITEMS	= 20,	//	레어 아이템만 줍기
		EMFOR_PET_GETPOTIONS	= 21,	//	약품류 아이템만 줍기
		EMFOR_PET_GETMONEY		= 22,	//	돈만 줍기
		EMFOR_PET_GETSTONE		= 23,	//	연마재 아이템만 줍기
		EMFOR_PET_HEAL			= 24,	//	주인의 HP 회복 속도 상승
		EMFOR_PET_SUPPROT		= 25,	//	주인의 HP, MP, SP 회복률 상승
		EMFOR_PET_BACKUP_ATK	= 26,	//	주인의 공격력(율) 상승
		EMFOR_PET_BACKUP_DEF	= 27,	//	주인의 방어력(율) 상승
		EMFOR_PET_AUTOMEDISON	= 28,	//	자동 약품 사용
		EMFOR_PET_PROTECTITEMS	= 29,	//	주인이 아이템 잃어버리지 않도록 보호

		EMFOR_RESIST			= 30,	//	저항 변화량

		EMFOR_SUMMON_ACTIVETIME	= 31,	//	소환시간 증가
		EMFOR_SUMMON_HIRE		= 32,	//	소환수 고용 가능 여부

		EMFOR_PET_GETQBOX		= 33,	// Q박스만 줍기;
		EMFOR_PET_GETMATERIALS	= 34,	// 재료만 줍기;

		FOR_TYPE_SIZE
	};
	enum EMROLE
	{
		EMROLE_NORMAL	= 0,		//	즉시.
		EMROLE_PASSIVE	= 1,		//	페시브 계열. ( 사용하는 것은 아니고 배웠을때 다른 값에 영향 )
		EMROLE_NSIZE	= 2,
	};

	enum EMAPPLY	//	물리력 가산시 효능.
	{
		EMAPPLY_MELEE	= 0,	//	격투;
		EMAPPLY_RANGE	= 1,	//	사격;
		EMAPPLY_MAGIC	= 2,	//	마력;

		EMAPPLY_NSIZE		= 3
	};

	enum EMAPPLY_DWORD
	{
		EMAPPLY_DWORD_NULL		= 0x0,
		EMAPPLY_DWORD_MELEE	= (1 << EMAPPLY_MELEE),		// 격투;
		EMAPPLY_DWORD_RANGE	= (1 << EMAPPLY_RANGE),		// 사격;
		EMAPPLY_DWORD_MAGIC	= (1 << EMAPPLY_MAGIC),		// 마력;
	};


	enum EMACTION_TYPE	//	행동타입
	{
		EMACTION_TYPE_NORMAL = 0,	// 일반
		EMACTION_TYPE_BUFF	 = 1,	// 버프
		EMACTION_TYPE_DEBUFF = 2,	// 디버프
		EMACTION_TYPE_LIMIT	 = 3,	// 리미트

		EMACTION_TYPE_NSIZE  = 4,	
	};

	enum EMACTION_TYPE_DWORD // 행동 타입 비트 플래그
	{
		EMACTION_TYPE_DWORD_NULL    = 0x00,
		EMACTION_TYPE_DWORD_NORMAL	= (1 << EMACTION_TYPE_NORMAL),  // 일반
		EMACTION_TYPE_DWORD_BUFF    = (1 << EMACTION_TYPE_BUFF),    // 버프
		EMACTION_TYPE_DWORD_DEBUFF	= (1 << EMACTION_TYPE_DEBUFF),	// 디버프
		EMACTION_TYPE_DWORD_LIMIT   = (1 << EMACTION_TYPE_LIMIT),	// 리미트
	};

	enum EMEFFECT_TYPE // 부가 효과 타입;
	{
		EMEFFECT_TYPE_NONE = 0,		// 없음;
		EMEFFECT_TYPE_IMPACTA = 1,	// 부가효과;
		EMEFFECT_TYPE_SPECA = 2,		// 특수효과;
		EMEFFECT_TYPE_STYPE = 3,		// 특수능력;
		EMEFFECT_TYPE_NSIZE = 4,
	};	

	enum EMKEYWORD
	{
		EMKEYWORD_PILEUP,	// 중첩 허용;
		EMKEYWORD_NSIZE,
	};

	enum EMEFFECTIME
	{
		EMTIME_FIRST	= 0,		//	스킬 시작과 동시에.
		EMTIME_IMPACT	= 1,		//	스킬 타격 지점.
		EMTIME_TARG_OVR	= 2,		//	스킬 타격 이팩트가 끝나는 지점.
		EMTIME_PASSIVE	= 3,		//	타겟에 부착된 스킬용.
		EMTIME_REV01	= 4,
		EMTIME_EFFECT	= 5,		//  효과적용시
		EMTIME_NSIZE	= 6,
	};

	enum EMEFFECTPOS
	{
		EMPOS_GROUND	= 0,		//	지면 위치.
		EMPOS_STRIKE	= 1,		//	스트라이크 위치. ( 무기, 손에 지정된 때리는(발생하는) 위치. )

		EMPOS_NSIZE		= 2,
	};

	/// 특수 발동 스킬;
	enum EMSPECIALSKILLTYPE
	{
		EMSSTYPE_NONE					= 0,		// 사용안함;
		EMSSTYPE_TRANSFORM				= 1,		// 변신;
		EMSSTYPE_SUMMONS				= 2,		// 소환;
		EMSSTYPE_NSIZE
	};

	enum EMTRANSFORM_PART
	{
		EMTRANSFORM_HEAD	= 0,		    // 핼멧
		EMTRANSFORM_UPPER	= 1,			// 상체
		EMTRANSFORM_LOWER	= 2,			// 하체
		EMTRANSFORM_HAND	= 3,			// 손
		EMTRANSFORM_LHAND	= 4,			// 왼손 장비
		EMTRANSFORM_RHAND	= 5,			// 오른손 장비
		EMTRANSFORM_FOOT	= 6,			// 다리 장비
		EMTRANSFORM_FACE	= 7,			// 얼굴		

		EMTRANSFORM_NSIZE	= 8,				

	};

	enum GLSKILL_ATT    // 해머, 듀얼스피어, 사이드 추가.
	{
		SKILLATT_NOTHING	= 0,	//	착용무기없음.

		SKILLATT_SWORD	= 1,	//	검.
		SKILLATT_SABER		= 2,	//	도.
		SKILLATT_SWORDSABER	= 3,	//	검/도. "검/도" 는 스킬에서 사용무기 확인시에만 사용됨.

		SKILLATT_DAGGER	= 4,	//	단검.
		SKILLATT_SPEAR		= 5,	//	창.
		SKILLATT_STICK		= 6,	//	몽둥이.
		SKILLATT_GWON		= 7,	//	권.
		SKILLATT_BOW		= 8,	//	활.
		SKILLATT_THROW	= 9,	//	투척.

		SKILLATT_PISTOL			= 10,	//	권총
		SKILLATT_RAILGUN		= 11,	//	소총1
		SKILLATT_PORTALGUN	= 12,	//	소총2

		SKILLATT_GUN				= 13,	//	총 전체
		SKILLATT_RIFLE			= 14,	//	소총전체

		SKILLATT_SIDE				= 15,   // 갈고리낫
		SKILLATT_DUALSPEAR			= 16,   // 듀얼 스피어
		SKILLATT_THROWING_KNIFE		= 17,   // 수리검

		SKILLATT_EXTREME_GLOVE	= 18,	// 극강 글러브

		SKILLATT_TRICK_STICK	= 19, // 마술 지팡이;
		SKILLATT_TRICK_BOX		= 20, // 마술 상자;
		SKILLATT_TRICK_WHIP		= 21, // 마술 채찍;

		SKILLATT_ACTOR_SHIELD	= 22, // 연극 방패
		SKILLATT_ACTOR_HAMMER	= 23, // 연극 망치
		SKILLATT_ACTOR_UMBRELLA	= 24, // 연극 우산

		SKILLATT_NOCARE			= 25,	//	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
		SKILLATT_NSIZE			= 25,	//	종류갯수.

		SKILLATT_NEAR			= 7,	//	근접 공격.
	};

	GLSKILL_ATT assign_skill_att( glold::GLITEM_ATT_102 emOld );
	GLSKILL_ATT assign_skill_att( glold_103::GLITEM_ATT_103 emOld );
	GLSKILL_ATT assign_skill_att( glold_104::GLITEM_ATT_104 emOld );
	GLSKILL_ATT assign_skill_att( glold_105::GLSKILL_ATT_105 emOld );
	GLSKILL_ATT assign_skill_att( glold_106::GLSKILL_ATT_106 emOld );
	GLSKILL_ATT assign_skill_att( glold_107::GLSKILL_ATT_107 emOld );
	GLSKILL_ATT assign_skill_att( glold_108::GLSKILL_ATT_108 emOld );
};

namespace COMMENT
{
	enum { SKILL_ICON_SIZE=256 };

	extern std::string SKILL_ROLE[SKILL::EMROLE_NSIZE];
	extern std::string SKILL_APPLY[SKILL::EMAPPLY_NSIZE];
	extern std::string SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NSIZE];
	extern std::string SKILL_EFFECT_TYPE[SKILL::EMEFFECT_TYPE_NSIZE];	

	extern std::string SKILL_KEYWORD[SKILL::EMKEYWORD_NSIZE];

	extern std::string SKILL_TYPES[SKILL::FOR_TYPE_SIZE];
	extern float SKILL_TYPES_SCALE[SKILL::FOR_TYPE_SIZE];
	inline bool IsSKILL_PER ( SKILL::EMTYPES emTYPE )			{ return SKILL_TYPES_SCALE[emTYPE]==100.0f; }

	extern std::string SKILL_LEVEL[SKILL::MAX_LEVEL];

	extern std::string SKILL_EFFTIME[SKILL::EMTIME_NSIZE];
	extern std::string SKILL_EFFPOS[SKILL::EMPOS_NSIZE];
	extern std::string SPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE];

	extern std::string SKILLATTACK[SKILL::SKILLATT_NSIZE+1];

	extern std::string SKILL_EFFTARGETPOS[SKILL::MAX_EFFTARGETPOS];
};