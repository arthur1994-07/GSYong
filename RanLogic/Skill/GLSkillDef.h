
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
	// Csv �ε��� �� ���
	extern int iCsvCur;

	enum
	{
		MAX_SZNAME			= 32,
		MAX_SZFILE			= 68,
		MAX_SZCOMMENT		= 128,

		MAX_LEVEL_BEFORE	= 9,
		MAX_LEVEL			= 24,
		MAX_ADDON			= 5,	//	Addon �ִ��� 
		MAX_EFFTARGETPOS	= 10,
		MAX_GRADE			= 25,
	};

	enum EMTYPES
	{
		EMFOR_HP				= 0,	//	ü��ġ ��ȭ.
		EMFOR_MP				= 1,	//	���ġ ��ȭ.
		EMFOR_SP				= 2,	//	�ٷ�ġ ��ȭ.

		EMFOR_VARHP				= 3,	//	ä��ġ ��ȭ��.
		EMFOR_VARMP				= 4,	//	���ġ ��ȭ��.
		EMFOR_VARSP				= 5,	//	�ٷ�ġ ��ȭ��.

		EMFOR_DEFENSE			= 6,	//	���ġ ��ȭ.
		EMFOR_CURE				= 7,	//	�ص�. ( �ص� ���� �ʿ� )

		EMFOR_HITRATE			= 8,	//	������.
		EMFOR_AVOIDRATE			= 9,	//	ȸ����.

		EMFOR_VARAP				= 10,	//	��� (ü��,���,�ٷ�) ��ȭ��.

		EMFOR_VARDAMAGE			= 11,	//	����ġ ��ȭ��.
		EMFOR_VARDEFENSE		= 12,	//	���ġ ��ȭ��.

		EMFOR_PA				= 13,	//	����ġ.
		EMFOR_SA				= 14,	//	���ġ.
		EMFOR_MA				= 15,	//	���ġ.

		EMFOR_HP_RATE			= 16,	//	HP ������.
		EMFOR_MP_RATE			= 17,	//	MP ������.
		EMFOR_SP_RATE			= 18,	//	SP ������.

		EMFOR_PET_GETALLITEMS	= 19,	//	��� ������ �ݱ�	// PetData
		EMFOR_PET_GETRAREITEMS	= 20,	//	���� �����۸� �ݱ�
		EMFOR_PET_GETPOTIONS	= 21,	//	��ǰ�� �����۸� �ݱ�
		EMFOR_PET_GETMONEY		= 22,	//	���� �ݱ�
		EMFOR_PET_GETSTONE		= 23,	//	������ �����۸� �ݱ�
		EMFOR_PET_HEAL			= 24,	//	������ HP ȸ�� �ӵ� ���
		EMFOR_PET_SUPPROT		= 25,	//	������ HP, MP, SP ȸ���� ���
		EMFOR_PET_BACKUP_ATK	= 26,	//	������ ���ݷ�(��) ���
		EMFOR_PET_BACKUP_DEF	= 27,	//	������ ����(��) ���
		EMFOR_PET_AUTOMEDISON	= 28,	//	�ڵ� ��ǰ ���
		EMFOR_PET_PROTECTITEMS	= 29,	//	������ ������ �Ҿ������ �ʵ��� ��ȣ

		EMFOR_RESIST			= 30,	//	���� ��ȭ��

		EMFOR_SUMMON_ACTIVETIME	= 31,	//	��ȯ�ð� ����
		EMFOR_SUMMON_HIRE		= 32,	//	��ȯ�� ��� ���� ����

		EMFOR_PET_GETQBOX		= 33,	// Q�ڽ��� �ݱ�;
		EMFOR_PET_GETMATERIALS	= 34,	// ��Ḹ �ݱ�;

		FOR_TYPE_SIZE
	};
	enum EMROLE
	{
		EMROLE_NORMAL	= 0,		//	���.
		EMROLE_PASSIVE	= 1,		//	��ú� �迭. ( ����ϴ� ���� �ƴϰ� ������� �ٸ� ���� ���� )
		EMROLE_NSIZE	= 2,
	};

	enum EMAPPLY	//	������ ����� ȿ��.
	{
		EMAPPLY_MELEE	= 0,	//	����;
		EMAPPLY_RANGE	= 1,	//	���;
		EMAPPLY_MAGIC	= 2,	//	����;

		EMAPPLY_NSIZE		= 3
	};

	enum EMAPPLY_DWORD
	{
		EMAPPLY_DWORD_NULL		= 0x0,
		EMAPPLY_DWORD_MELEE	= (1 << EMAPPLY_MELEE),		// ����;
		EMAPPLY_DWORD_RANGE	= (1 << EMAPPLY_RANGE),		// ���;
		EMAPPLY_DWORD_MAGIC	= (1 << EMAPPLY_MAGIC),		// ����;
	};


	enum EMACTION_TYPE	//	�ൿŸ��
	{
		EMACTION_TYPE_NORMAL = 0,	// �Ϲ�
		EMACTION_TYPE_BUFF	 = 1,	// ����
		EMACTION_TYPE_DEBUFF = 2,	// �����
		EMACTION_TYPE_LIMIT	 = 3,	// ����Ʈ

		EMACTION_TYPE_NSIZE  = 4,	
	};

	enum EMACTION_TYPE_DWORD // �ൿ Ÿ�� ��Ʈ �÷���
	{
		EMACTION_TYPE_DWORD_NULL    = 0x00,
		EMACTION_TYPE_DWORD_NORMAL	= (1 << EMACTION_TYPE_NORMAL),  // �Ϲ�
		EMACTION_TYPE_DWORD_BUFF    = (1 << EMACTION_TYPE_BUFF),    // ����
		EMACTION_TYPE_DWORD_DEBUFF	= (1 << EMACTION_TYPE_DEBUFF),	// �����
		EMACTION_TYPE_DWORD_LIMIT   = (1 << EMACTION_TYPE_LIMIT),	// ����Ʈ
	};

	enum EMEFFECT_TYPE // �ΰ� ȿ�� Ÿ��;
	{
		EMEFFECT_TYPE_NONE = 0,		// ����;
		EMEFFECT_TYPE_IMPACTA = 1,	// �ΰ�ȿ��;
		EMEFFECT_TYPE_SPECA = 2,		// Ư��ȿ��;
		EMEFFECT_TYPE_STYPE = 3,		// Ư���ɷ�;
		EMEFFECT_TYPE_NSIZE = 4,
	};	

	enum EMKEYWORD
	{
		EMKEYWORD_PILEUP,	// ��ø ���;
		EMKEYWORD_NSIZE,
	};

	enum EMEFFECTIME
	{
		EMTIME_FIRST	= 0,		//	��ų ���۰� ���ÿ�.
		EMTIME_IMPACT	= 1,		//	��ų Ÿ�� ����.
		EMTIME_TARG_OVR	= 2,		//	��ų Ÿ�� ����Ʈ�� ������ ����.
		EMTIME_PASSIVE	= 3,		//	Ÿ�ٿ� ������ ��ų��.
		EMTIME_REV01	= 4,
		EMTIME_EFFECT	= 5,		//  ȿ�������
		EMTIME_NSIZE	= 6,
	};

	enum EMEFFECTPOS
	{
		EMPOS_GROUND	= 0,		//	���� ��ġ.
		EMPOS_STRIKE	= 1,		//	��Ʈ����ũ ��ġ. ( ����, �տ� ������ ������(�߻��ϴ�) ��ġ. )

		EMPOS_NSIZE		= 2,
	};

	/// Ư�� �ߵ� ��ų;
	enum EMSPECIALSKILLTYPE
	{
		EMSSTYPE_NONE					= 0,		// ������;
		EMSSTYPE_TRANSFORM				= 1,		// ����;
		EMSSTYPE_SUMMONS				= 2,		// ��ȯ;
		EMSSTYPE_NSIZE
	};

	enum EMTRANSFORM_PART
	{
		EMTRANSFORM_HEAD	= 0,		    // �۸�
		EMTRANSFORM_UPPER	= 1,			// ��ü
		EMTRANSFORM_LOWER	= 2,			// ��ü
		EMTRANSFORM_HAND	= 3,			// ��
		EMTRANSFORM_LHAND	= 4,			// �޼� ���
		EMTRANSFORM_RHAND	= 5,			// ������ ���
		EMTRANSFORM_FOOT	= 6,			// �ٸ� ���
		EMTRANSFORM_FACE	= 7,			// ��		

		EMTRANSFORM_NSIZE	= 8,				

	};

	enum GLSKILL_ATT    // �ظ�, ����Ǿ�, ���̵� �߰�.
	{
		SKILLATT_NOTHING	= 0,	//	���빫�����.

		SKILLATT_SWORD	= 1,	//	��.
		SKILLATT_SABER		= 2,	//	��.
		SKILLATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

		SKILLATT_DAGGER	= 4,	//	�ܰ�.
		SKILLATT_SPEAR		= 5,	//	â.
		SKILLATT_STICK		= 6,	//	������.
		SKILLATT_GWON		= 7,	//	��.
		SKILLATT_BOW		= 8,	//	Ȱ.
		SKILLATT_THROW	= 9,	//	��ô.

		SKILLATT_PISTOL			= 10,	//	����
		SKILLATT_RAILGUN		= 11,	//	����1
		SKILLATT_PORTALGUN	= 12,	//	����2

		SKILLATT_GUN				= 13,	//	�� ��ü
		SKILLATT_RIFLE			= 14,	//	������ü

		SKILLATT_SIDE				= 15,   // ������
		SKILLATT_DUALSPEAR			= 16,   // ��� ���Ǿ�
		SKILLATT_THROWING_KNIFE		= 17,   // ������

		SKILLATT_EXTREME_GLOVE	= 18,	// �ذ� �۷���

		SKILLATT_TRICK_STICK	= 19, // ���� ������;
		SKILLATT_TRICK_BOX		= 20, // ���� ����;
		SKILLATT_TRICK_WHIP		= 21, // ���� ä��;

		SKILLATT_ACTOR_SHIELD	= 22, // ���� ����
		SKILLATT_ACTOR_HAMMER	= 23, // ���� ��ġ
		SKILLATT_ACTOR_UMBRELLA	= 24, // ���� ���

		SKILLATT_NOCARE			= 25,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		SKILLATT_NSIZE			= 25,	//	��������.

		SKILLATT_NEAR			= 7,	//	���� ����.
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