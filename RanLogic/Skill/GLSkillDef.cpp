#include "pch.h"
#include "GLSkillDef.h"

SKILLID::SKILLID()
: sSKILLID(SNATIVEID(false))
, wLEVEL(0)
{
}

SKILLID::SKILLID( const SNATIVEID& _sSKILLID )
: sSKILLID(_sSKILLID)
, wLEVEL(0)
{
}

SKILLID::SKILLID( const SNATIVEID& _sSKILLID, const WORD _LEVEL )
: sSKILLID(_sSKILLID)
, wLEVEL(_LEVEL)
{
}

void SKILLID::RESET()
{
	(*this) = SKILLID_NULL;
}

const bool SKILLID::operator == ( const SKILLID& rhs ) const
{
	if ( sSKILLID != rhs.sSKILLID )
	{
		return false;
	}

	if ( wLEVEL != rhs.wLEVEL )
	{
		return false;
	}

	return true;
}

const bool SKILLID::operator != ( const SKILLID& rhs ) const
{
	return !(sSKILLID == rhs.sSKILLID);
}

namespace SKILL
{
	int iCsvCur = 0;

	GLSKILL_ATT assign_skill_att ( glold::GLITEM_ATT_102 emOld )
	{
		GLSKILL_ATT emAttack;
		switch ( emOld )
		{
		case glold::ITEMATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;
		case glold::ITEMATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;
		case glold::ITEMATT_REV00:
			emAttack = SKILLATT_SWORD;	//	sword 로 강제 변환.
			break;
		case glold::ITEMATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;
		case glold::ITEMATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;
		case glold::ITEMATT_BOW:
			emAttack = SKILLATT_BOW; 
			break;
		case glold::ITEMATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold::ITEMATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att ( glold_103::GLITEM_ATT_103 emOld )
	{
		GLSKILL_ATT emAttack;
		switch ( emOld )
		{
		case glold_103::ITEMATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_103::ITEMATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;
		case glold_103::ITEMATT_SABER:
			emAttack = SKILLATT_SABER;
			break;
		case glold_103::ITEMATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;
		case glold_103::ITEMATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_103::ITEMATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_103::ITEMATT_BOW:
			emAttack = SKILLATT_BOW; 
			break;
		case glold_103::ITEMATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_103::ITEMATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att ( glold_104::GLITEM_ATT_104 emOld )
	{
		GLSKILL_ATT emAttack;
		switch ( emOld )
		{
		case glold_104::ITEMATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_104::ITEMATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;
		case glold_104::ITEMATT_SABER:
			emAttack = SKILLATT_SABER;
			break;

		case glold_104::ITEMATT_SWORDSABER:
			emAttack = SKILLATT_SWORDSABER;
			break;		

		case glold_104::ITEMATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;
		case glold_104::ITEMATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_104::ITEMATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_104::ITEMATT_GWON:
			emAttack = SKILLATT_GWON;
			break;

		case glold_104::ITEMATT_BOW:
			emAttack = SKILLATT_BOW; 
			break;

		case glold_104::ITEMATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_104::ITEMATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att ( glold_105::GLSKILL_ATT_105 emOld )
	{
		GLSKILL_ATT emAttack;

		switch ( emOld )
		{
		case glold_105::SKILLATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_105::SKILLATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;

		case glold_105::SKILLATT_SABER:
			emAttack = SKILLATT_SABER;
			break;

		case glold_105::SKILLATT_SWORDSABER:
			emAttack = SKILLATT_SWORDSABER;
			break;

		case glold_105::SKILLATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;

		case glold_105::SKILLATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_105::SKILLATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_105::SKILLATT_GWON:
			emAttack = SKILLATT_GWON;
			break;

		case glold_105::SKILLATT_BOW:
			emAttack = SKILLATT_BOW;
			break;

		case glold_105::SKILLATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_105::SKILLATT_PISTOL:
			emAttack = SKILLATT_PISTOL;
			break;

		case glold_105::SKILLATT_RAILGUN:
			emAttack = SKILLATT_RAILGUN;
			break;

		case glold_105::SKILLATT_PORTALGUN:
			emAttack = SKILLATT_PORTALGUN;
			break;

		case glold_105::SKILLATT_GUN:
			emAttack = SKILLATT_GUN;
			break;

		case glold_105::SKILLATT_RIFLE:
			emAttack = SKILLATT_RIFLE;
			break;

		case glold_105::SKILLATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att ( glold_106::GLSKILL_ATT_106 emOld )
	{
		GLSKILL_ATT emAttack;

		switch ( emOld )
		{
		case glold_106::SKILLATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_106::SKILLATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;

		case glold_106::SKILLATT_SABER:
			emAttack = SKILLATT_SABER;
			break;

		case glold_106::SKILLATT_SWORDSABER:
			emAttack = SKILLATT_SWORDSABER;
			break;

		case glold_106::SKILLATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;

		case glold_106::SKILLATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_106::SKILLATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_106::SKILLATT_GWON:
			emAttack = SKILLATT_GWON;
			break;

		case glold_106::SKILLATT_BOW:
			emAttack = SKILLATT_BOW;
			break;

		case glold_106::SKILLATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_106::SKILLATT_PISTOL:
			emAttack = SKILLATT_PISTOL;
			break;

		case glold_106::SKILLATT_RAILGUN:
			emAttack = SKILLATT_RAILGUN;
			break;

		case glold_106::SKILLATT_PORTALGUN:
			emAttack = SKILLATT_PORTALGUN;
			break;

		case glold_106::SKILLATT_GUN:
			emAttack = SKILLATT_GUN;
			break;

		case glold_106::SKILLATT_RIFLE:
			emAttack = SKILLATT_RIFLE;
			break;

		case glold_106::SKILLATT_HAMMER:
			emAttack = SKILLATT_SIDE;
			break;

		case glold_106::SKILLATT_DUALSPEAR:
			emAttack = SKILLATT_DUALSPEAR;
			break;

		case glold_106::SKILLATT_SIDE:
			emAttack = SKILLATT_THROWING_KNIFE;
			break;

		case glold_106::SKILLATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att( glold_107::GLSKILL_ATT_107 emOld )
	{
		GLSKILL_ATT emAttack;

		switch ( emOld )
		{
		case glold_107::SKILLATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_107::SKILLATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;

		case glold_107::SKILLATT_SABER:
			emAttack = SKILLATT_SABER;
			break;

		case glold_107::SKILLATT_SWORDSABER:
			emAttack = SKILLATT_SWORDSABER;
			break;

		case glold_107::SKILLATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;

		case glold_107::SKILLATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_107::SKILLATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_107::SKILLATT_GWON:
			emAttack = SKILLATT_GWON;
			break;

		case glold_107::SKILLATT_BOW:
			emAttack = SKILLATT_BOW;
			break;

		case glold_107::SKILLATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_107::SKILLATT_PISTOL:
			emAttack = SKILLATT_PISTOL;
			break;

		case glold_107::SKILLATT_RAILGUN:
			emAttack = SKILLATT_RAILGUN;
			break;

		case glold_107::SKILLATT_PORTALGUN:
			emAttack = SKILLATT_PORTALGUN;
			break;

		case glold_107::SKILLATT_GUN:
			emAttack = SKILLATT_GUN;
			break;

		case glold_107::SKILLATT_RIFLE:
			emAttack = SKILLATT_RIFLE;
			break;

		case glold_107::SKILLATT_HAMMER:
			emAttack = SKILLATT_SIDE;
			break;

		case glold_107::SKILLATT_DUALSPEAR:
			emAttack = SKILLATT_DUALSPEAR;
			break;

		case glold_107::SKILLATT_SIDE:
			emAttack = SKILLATT_THROWING_KNIFE;
			break;

		case glold_107::SKILLATT_EXTREME_GLOVE:
			emAttack = SKILLATT_EXTREME_GLOVE;
			break;

		case glold_107::SKILLATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

	GLSKILL_ATT assign_skill_att( glold_108::GLSKILL_ATT_108 emOld )
	{
		GLSKILL_ATT emAttack;

		switch ( emOld )
		{
		case glold_108::SKILLATT_NOTHING:
			emAttack = SKILLATT_NOTHING;
			break;

		case glold_108::SKILLATT_SWORD:
			emAttack = SKILLATT_SWORD;
			break;

		case glold_108::SKILLATT_SABER:
			emAttack = SKILLATT_SABER;
			break;

		case glold_108::SKILLATT_SWORDSABER:
			emAttack = SKILLATT_SWORDSABER;
			break;

		case glold_108::SKILLATT_DAGGER:
			emAttack = SKILLATT_DAGGER;
			break;

		case glold_108::SKILLATT_SPEAR:
			emAttack = SKILLATT_SPEAR;
			break;

		case glold_108::SKILLATT_STICK:
			emAttack = SKILLATT_STICK;
			break;

		case glold_108::SKILLATT_GWON:
			emAttack = SKILLATT_GWON;
			break;

		case glold_108::SKILLATT_BOW:
			emAttack = SKILLATT_BOW;
			break;

		case glold_108::SKILLATT_THROW:
			emAttack = SKILLATT_THROW;
			break;

		case glold_108::SKILLATT_PISTOL:
			emAttack = SKILLATT_PISTOL;
			break;

		case glold_108::SKILLATT_RAILGUN:
			emAttack = SKILLATT_RAILGUN;
			break;

		case glold_108::SKILLATT_PORTALGUN:
			emAttack = SKILLATT_PORTALGUN;
			break;

		case glold_108::SKILLATT_GUN:
			emAttack = SKILLATT_GUN;
			break;

		case glold_108::SKILLATT_RIFLE:
			emAttack = SKILLATT_RIFLE;
			break;

		case glold_108::SKILLATT_HAMMER:
			emAttack = SKILLATT_SIDE;
			break;

		case glold_108::SKILLATT_DUALSPEAR:
			emAttack = SKILLATT_DUALSPEAR;
			break;

		case glold_108::SKILLATT_SIDE:
			emAttack = SKILLATT_THROWING_KNIFE;
			break;

		case glold_108::SKILLATT_EXTREME_GLOVE:
			emAttack = SKILLATT_EXTREME_GLOVE;
			break;

		case glold_108::SKILLATT_TRICK_STICK :
			emAttack = SKILLATT_TRICK_STICK;
			break;
		case glold_108::SKILLATT_TRICK_BOX   :
			emAttack = SKILLATT_TRICK_BOX;
			break;
		case glold_108::SKILLATT_TRICK_WHIP  :
			emAttack = SKILLATT_TRICK_WHIP;
			break;
		case glold_108::SKILLATT_NOCARE:
			emAttack = SKILLATT_NOCARE;
			break;

		default:
			emAttack = SKILLATT_NOTHING;
			break;
		};

		return emAttack;
	}

};

namespace COMMENT
{
	std::string SKILL_ROLE[SKILL::EMROLE_NSIZE] =
	{
		"기본형",
		"패시브형"
	};   

	std::string SKILL_APPLY[SKILL::EMAPPLY_NSIZE] =
	{
		"물리 타격형",
		"물리 사격형",
		"마법 작용형"
	};

	std::string SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NSIZE] = 
	{
		"일반",
		"버프",
		"디버프",
		"리미트",
	};

	std::string SKILL_EFFECT_TYPE[SKILL::EMEFFECT_TYPE_NSIZE] = 
	{
		"없음",
		"부가효과",
		"특수효과",
		"특수능력",
	};

	std::string SKILL_KEYWORD[SKILL::EMKEYWORD_NSIZE] =
	{
		"중첩 허용",
	};

	std::string SKILL_TYPES[SKILL::FOR_TYPE_SIZE] = 
	{
		"HP 변화",
		"MP 변화",
		"SP 변화",

		"HP 변화율",
		"MP 변화율",
		"SP 변화율",

		"방어치 변화",
		"해독",
		"명중율 변화",
		"회피율 변화",
		"HP+MP+SP 변화율",

		"공격치 변화량",
		"방어치 변화량",

		"격투치",
		"사격치",
		"마력치",

		"HP 증폭율",
		"MP 증폭율",
		"SP 증폭율",

		"펫 모든 아이템",	// PetData
		"펫 레어 아이템",
		"펫 약품류 아이템",
		"펫 돈",
		"펫 연마재 아이템",
		"펫 주인 HP 회복 속도",
		"펫 주인 HP,MP,SP 회복률",
		"펫 주인 공격력",
		"펫 주인 방어력",
		"펫 자동 약품 사용",
		"펫 주인 아이템 보호",
		"저항치 변화",

		"소환시간 증가",
		"소환수 고용가능",
		"펫 Q박스",
		"펫 재료",
	};

	float SKILL_TYPES_SCALE[SKILL::FOR_TYPE_SIZE] =
	{
		1.0f,
		1.0f,
		1.0f,

		100.0f,
		100.0f,
		100.0f,

		1.0f,
		1.0f,
		1.0f,
		1.0f,
		100.0f,

		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,	// PetData
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,
		1.0f,

		1.0f,

		1.0f,
	};

	std::string SKILL_LEVEL[SKILL::MAX_LEVEL] =
	{
		"레벨 01",
		"레벨 02",
		"레벨 03",
		"레벨 04",
		"레벨 05",
		"레벨 06",
		"레벨 07",
		"레벨 08",
		"레벨 09",
		"레벨 10",
		"레벨 11",
		"레벨 12",
		"레벨 13",
		"레벨 14",
		"레벨 15",
		"레벨 16",
		"레벨 17",
		"레벨 18",
		"레벨 19",
		"레벨 20",
		"레벨 21",
		"레벨 22",
		"레벨 23",
		"레벨 24",
	};

	std::string SKILL_EFFTIME[SKILL::EMTIME_NSIZE] =
	{
		"발동시",
		"타격시",
		"지향효과종료시",
		"타겟부착",
		"NULL",
		"효과적용시",
	};

	std::string SKILL_EFFPOS[SKILL::EMPOS_NSIZE] =
	{
		"지면위치",
		"타격위치",
	};

	std::string SPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE] =
	{
		"사용안함",
		"변신",
		"소환"
	};

	std::string SKILLATTACK[SKILL::SKILLATT_NSIZE+1] = 
	{
		"'비'착용",
		"검",
		"도",
		"검/도",
		"단검",
		"창",
		"몽둥이",
		"권",
		"활",
		"투척",
		"건",
		"레일건",
		"포탈건",
		"총",
		"레일건/포탈건",
		"해머",
		"듀얼스피어",
		"사이드",
		"극강글러브",
		"마술지팡이",
		"마술상자",
		"채찍",
		"방패",
		"망치",
		"우산",
		"타입검사안함",
	};

	std::string SKILL_EFFTARGETPOS[SKILL::MAX_EFFTARGETPOS] =
	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
	};
};