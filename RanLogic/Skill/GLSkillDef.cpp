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
			emAttack = SKILLATT_SWORD;	//	sword �� ���� ��ȯ.
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
		"�⺻��",
		"�нú���"
	};   

	std::string SKILL_APPLY[SKILL::EMAPPLY_NSIZE] =
	{
		"���� Ÿ����",
		"���� �����",
		"���� �ۿ���"
	};

	std::string SKILL_ACTION_TYPE[SKILL::EMACTION_TYPE_NSIZE] = 
	{
		"�Ϲ�",
		"����",
		"�����",
		"����Ʈ",
	};

	std::string SKILL_EFFECT_TYPE[SKILL::EMEFFECT_TYPE_NSIZE] = 
	{
		"����",
		"�ΰ�ȿ��",
		"Ư��ȿ��",
		"Ư���ɷ�",
	};

	std::string SKILL_KEYWORD[SKILL::EMKEYWORD_NSIZE] =
	{
		"��ø ���",
	};

	std::string SKILL_TYPES[SKILL::FOR_TYPE_SIZE] = 
	{
		"HP ��ȭ",
		"MP ��ȭ",
		"SP ��ȭ",

		"HP ��ȭ��",
		"MP ��ȭ��",
		"SP ��ȭ��",

		"���ġ ��ȭ",
		"�ص�",
		"������ ��ȭ",
		"ȸ���� ��ȭ",
		"HP+MP+SP ��ȭ��",

		"����ġ ��ȭ��",
		"���ġ ��ȭ��",

		"����ġ",
		"���ġ",
		"����ġ",

		"HP ������",
		"MP ������",
		"SP ������",

		"�� ��� ������",	// PetData
		"�� ���� ������",
		"�� ��ǰ�� ������",
		"�� ��",
		"�� ������ ������",
		"�� ���� HP ȸ�� �ӵ�",
		"�� ���� HP,MP,SP ȸ����",
		"�� ���� ���ݷ�",
		"�� ���� ����",
		"�� �ڵ� ��ǰ ���",
		"�� ���� ������ ��ȣ",
		"����ġ ��ȭ",

		"��ȯ�ð� ����",
		"��ȯ�� ��밡��",
		"�� Q�ڽ�",
		"�� ���",
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
		"���� 01",
		"���� 02",
		"���� 03",
		"���� 04",
		"���� 05",
		"���� 06",
		"���� 07",
		"���� 08",
		"���� 09",
		"���� 10",
		"���� 11",
		"���� 12",
		"���� 13",
		"���� 14",
		"���� 15",
		"���� 16",
		"���� 17",
		"���� 18",
		"���� 19",
		"���� 20",
		"���� 21",
		"���� 22",
		"���� 23",
		"���� 24",
	};

	std::string SKILL_EFFTIME[SKILL::EMTIME_NSIZE] =
	{
		"�ߵ���",
		"Ÿ�ݽ�",
		"����ȿ�������",
		"Ÿ�ٺ���",
		"NULL",
		"ȿ�������",
	};

	std::string SKILL_EFFPOS[SKILL::EMPOS_NSIZE] =
	{
		"������ġ",
		"Ÿ����ġ",
	};

	std::string SPECIAL_SKILL_TYPE[SKILL::EMSSTYPE_NSIZE] =
	{
		"������",
		"����",
		"��ȯ"
	};

	std::string SKILLATTACK[SKILL::SKILLATT_NSIZE+1] = 
	{
		"'��'����",
		"��",
		"��",
		"��/��",
		"�ܰ�",
		"â",
		"������",
		"��",
		"Ȱ",
		"��ô",
		"��",
		"���ϰ�",
		"��Ż��",
		"��",
		"���ϰ�/��Ż��",
		"�ظ�",
		"����Ǿ�",
		"���̵�",
		"�ذ��۷���",
		"����������",
		"��������",
		"ä��",
		"����",
		"��ġ",
		"���",
		"Ÿ�԰˻����",
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