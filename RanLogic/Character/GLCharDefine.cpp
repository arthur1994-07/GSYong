#include "../pch.h"

#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/File/Excel/MinExcel.h"

#include "./GLCharDefine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

WORD school2index ( const EMSCHOOLFLAG emSchool )
{
	switch ( emSchool )
	{
	case GLSCHOOL_00:	return 0;
	case GLSCHOOL_01:	return 1;
	case GLSCHOOL_02:	return 2;
	};

	return 0;
}

EMSCHOOLFLAG index2school ( const WORD wSchool )
{
	switch ( wSchool )
	{
	case 0:	return GLSCHOOL_00;
	case 1:	return GLSCHOOL_01;
	case 2:	return GLSCHOOL_02;
	};

	return GLSCHOOL_00;
}

EMCLASSINDEX CharClassToClassIndex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
	case GLCC_FIGHTER:
		return GLCL_FIGHTER;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
	case GLCC_ARMS:
		return GLCL_ARMS;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
	case GLCC_ARCHER:
		return GLCL_ARCHER;	

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
	case GLCC_SPIRIT:
		return GLCL_SPIRIT;

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
	case GLCC_EXTREME:
		return GLCL_EXTREME;

	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
	case GLCC_SCIENTIST:
		return GLCL_SCIENTIST;

    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
	case GLCC_ASSASSIN:
		return GLCL_ASSASSIN;

	case GLCC_TRICKER_M:
	case GLCC_TRICKER_W:
	case GLCC_TRICKER:
		return GLCL_TRICKER;

    case GLCC_ETC_M:
    case GLCC_ETC_W:
	case GLCC_ETC:
		return GLCL_ETC;

	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
	case GLCC_ACTOR:
		return GLCL_ACTOR;
	};

	return GLCL_FIGHTER;
}

EMCHARINDEX CharClassToIndex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCI_FIGHTER_M;
	case GLCC_ARMS_M:		return GLCI_ARMS_M;
	case GLCC_ARCHER_W:		return GLCI_ARCHER_W;
	case GLCC_SPIRIT_W:		return GLCI_SPIRIT_W;
	case GLCC_EXTREME_M:	return GLCI_EXTREME_M;
	case GLCC_EXTREME_W:	return GLCI_EXTREME_W;
	
	case GLCC_FIGHTER_W:	return GLCI_FIGHTER_W;
	case GLCC_ARMS_W:		return GLCI_ARMS_W;
	case GLCC_ARCHER_M:		return GLCI_ARCHER_M;
	case GLCC_SPIRIT_M:		return GLCI_SPIRIT_M;
	case GLCC_SCIENTIST_M:	return GLCI_SCIENTIST_M;
	case GLCC_SCIENTIST_W:	return GLCI_SCIENTIST_W;

    case GLCC_ASSASSIN_M:	return GLCI_ASSASSIN_M;
    case GLCC_ASSASSIN_W:	return GLCI_ASSASSIN_W;
	case GLCC_TRICKER_M:		return GLCI_TRICKER_M;
	case GLCC_TRICKER_W:	return GLCI_TRICKER_W;
    case GLCC_ETC_M:			return GLCI_ETC_M;
    case GLCC_ETC_W:			return GLCI_ETC_W;
	case GLCC_ACTOR_M:		return GLCI_ACTOR_M;
	case GLCC_ACTOR_W:		return GLCI_ACTOR_W;
	};


	sc::writeLogError(sc::string::format("CharClassToIndex() - �߸��� ĳ���� Ŭ�����Դϴ�.(%1%)",emClass));	
	GASSERT(0&&"�߸��� �ɸ��� Ŭ�����Դϴ�.");

	return GLCI_FIGHTER_M;
}

EMCHARCLASS CharIndexToClass ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:		return GLCC_FIGHTER_M;
	case GLCI_ARMS_M:			return GLCC_ARMS_M;
	case GLCI_ARCHER_W:		return GLCC_ARCHER_W;
	case GLCI_SPIRIT_W:		return GLCC_SPIRIT_W;

	case GLCI_EXTREME_M:	return GLCC_EXTREME_M;
	case GLCI_EXTREME_W:	return GLCC_EXTREME_W;

	case GLCI_FIGHTER_W:		return GLCC_FIGHTER_W;
	case GLCI_ARMS_W:			return GLCC_ARMS_W;
	case GLCI_ARCHER_M:		return GLCC_ARCHER_M;
	case GLCI_SPIRIT_M:		return GLCC_SPIRIT_M;
	case GLCI_SCIENTIST_M:	return GLCC_SCIENTIST_M;
	case GLCI_SCIENTIST_W:	return GLCC_SCIENTIST_W;

    case GLCI_ASSASSIN_M:	return GLCC_ASSASSIN_M;
    case GLCI_ASSASSIN_W:	return GLCC_ASSASSIN_W;
	case GLCI_TRICKER_M:		return GLCC_TRICKER_M;
	case GLCI_TRICKER_W:		return GLCC_TRICKER_W;

    case GLCI_ETC_M:			return GLCC_ETC_M;
    case GLCI_ETC_W:			return GLCC_ETC_W;

	case GLCI_ACTOR_M:		return GLCC_ACTOR_M;
	case GLCI_ACTOR_W:		return GLCC_ACTOR_W;
	};

	sc::writeLogError(sc::string::format("CharIndexToClass() - �߸��� ĳ���� �ε����Դϴ�.(%1%)",emIndex));
	GASSERT(0&&"�߸��� �ɸ��� �ε����Դϴ�.");

	return GLCC_FIGHTER_M;
}


EMCHARCLASS CharClassToSex ( const EMCHARCLASS emClass )
{
	switch (emClass)
	{
	case GLCC_FIGHTER_M:		return GLCC_FIGHTER_W;
	case GLCC_ARMS_M:		return GLCC_ARMS_W;
	case GLCC_ARCHER_W:		return GLCC_ARCHER_M;
	case GLCC_SPIRIT_W:		return GLCC_SPIRIT_M;
	case GLCC_EXTREME_M:	return GLCC_EXTREME_W;
	case GLCC_EXTREME_W:	return GLCC_EXTREME_M;
	
	case GLCC_FIGHTER_W:	return GLCC_FIGHTER_M;
	case GLCC_ARMS_W:		return GLCC_ARMS_M;
	case GLCC_ARCHER_M:		return GLCC_ARCHER_W;
	case GLCC_SPIRIT_M:		return GLCC_SPIRIT_W;
	case GLCC_SCIENTIST_M:	return GLCC_SCIENTIST_W;
	case GLCC_SCIENTIST_W:	return GLCC_SCIENTIST_M;

    case GLCC_ASSASSIN_M:	return GLCC_ASSASSIN_W;
    case GLCC_ASSASSIN_W:	return GLCC_ASSASSIN_M;
	case GLCC_TRICKER_M:		return GLCC_TRICKER_W;
	case GLCC_TRICKER_W:	return GLCC_TRICKER_M;
    case GLCC_ETC_M:			return GLCC_ETC_W;
    case GLCC_ETC_W:			return GLCC_ETC_M;
	case GLCC_ACTOR_M:		return GLCC_ACTOR_W;
	case GLCC_ACTOR_W:		return GLCC_ACTOR_M;
	};

	sc::writeLogError(sc::string::format("CharClassToSex() - �߸��� ĳ���� Ŭ�����Դϴ�.(%1%)",emClass));	
	GASSERT(0&&"�߸��� �ɸ��� Ŭ�����Դϴ�.");

	return GLCC_FIGHTER_M;
}

EMCHARINDEX CharIndexToSex ( const EMCHARINDEX emIndex )
{
	switch (emIndex)
	{
	case GLCI_FIGHTER_M:		return GLCI_FIGHTER_W;
	case GLCI_ARMS_M:			return GLCI_ARMS_W;
	case GLCI_ARCHER_W:		return GLCI_ARCHER_M;
	case GLCI_SPIRIT_W:		return GLCI_SPIRIT_M;

	case GLCI_EXTREME_M:	return GLCI_EXTREME_W;
	case GLCI_EXTREME_W:	return GLCI_EXTREME_M;

	case GLCI_FIGHTER_W:		return GLCI_FIGHTER_M;
	case GLCI_ARMS_W:			return GLCI_ARMS_M;
	case GLCI_ARCHER_M:		return GLCI_ARCHER_W;
	case GLCI_SPIRIT_M:		return GLCI_SPIRIT_W;
	case GLCI_SCIENTIST_M:	return GLCI_SCIENTIST_W;
	case GLCI_SCIENTIST_W:	return GLCI_SCIENTIST_M;

    case GLCI_ASSASSIN_M:	return GLCI_ASSASSIN_W;
    case GLCI_ASSASSIN_W:	return GLCI_ASSASSIN_M;
	case GLCI_TRICKER_M:		return GLCI_TRICKER_W;
	case GLCI_TRICKER_W:		return GLCI_TRICKER_M;
    case GLCI_ETC_M:			return GLCI_ETC_W;
    case GLCI_ETC_W:			return GLCI_ETC_M;
	case GLCI_ACTOR_M:		return GLCI_ACTOR_W;
	case GLCI_ACTOR_W:		return GLCI_ACTOR_M;
	};
	
	sc::writeLogError(sc::string::format("CharIndexToSex() - �߸��� ĳ���� �ε����Դϴ�.(%1%)",emIndex));
	GASSERT(0&&"�߸��� �ɸ��� �ε����Դϴ�.");

	return GLCI_FIGHTER_M;
}

EMCHARCLASS CharSexandClassIndexToCharClass( const int nSex, const EMCLASSINDEX emclassIndex )
{
	switch (emclassIndex)
	{
	case GLCL_FIGHTER: // ������
		{
			if ( nSex == 1 )
				return GLCC_FIGHTER_M;
			else if (nSex == 2)
				return GLCC_FIGHTER_W;
			else
				return GLCC_FIGHTER;
		}
		break;
	case GLCL_ARMS: // �˵���
		{
			if ( nSex == 1 )
				return GLCC_ARMS_M;
			else if (nSex == 2)
				return GLCC_ARMS_W;
			else
				return GLCC_ARMS;
		}
		break;
	case GLCL_ARCHER: // ��ú�
		{
			if ( nSex == 1 )
				return GLCC_ARCHER_M;
			else if (nSex == 2)
				return GLCC_ARCHER_W;
			else
				return GLCC_ARCHER;
		}
		break;
	case GLCL_SPIRIT: // �⿹��
		{
			if ( nSex == 1 )
				return GLCC_SPIRIT_M;
			else if (nSex == 2)
				return GLCC_SPIRIT_W;
			else
				return GLCC_SPIRIT;
		}
		break;
	case GLCL_EXTREME: // �ذ���
		{
			if ( nSex == 1 )
				return GLCC_EXTREME_M;
			else if (nSex == 2)
				return GLCC_EXTREME_W;
			else
				return GLCC_EXTREME;
		}
		break;
	case GLCL_SCIENTIST: // ���к�
		{
			if ( nSex == 1 )
				return GLCC_SCIENTIST_M;
			else if (nSex == 2)
				return GLCC_SCIENTIST_W;
			else
				return GLCC_SCIENTIST;
		}
		break;
	case GLCL_ASSASSIN: // �μ���
		{
			if ( nSex == 1 )
				return GLCC_ASSASSIN_M;
			else if (nSex == 2)
				return GLCC_ASSASSIN_W;
			else
				return GLCC_ASSASSIN;
		}
		break;
	case GLCL_TRICKER: // ������;
		{
			if ( nSex == 1 )
				return GLCC_TRICKER_M;
			else if (nSex == 2)
				return GLCC_TRICKER_W;
			else
				return GLCC_TRICKER;
		}
		break;
	case GLCL_ACTOR:
		{
			if ( nSex == 1 )
				return GLCC_ACTOR_M;
			else if (nSex == 2)
				return GLCC_ACTOR_W;
			else
				return GLCC_ACTOR;
		}
		break;
	default:
		{
			if (nSex == 1)
				return GLCC_MAN;
			else if (nSex == 2)
				return GLCC_WOMAN;
			else
				return GLCC_ALL_NEWSEX;
		}
		break;
	};

		return GLCC_ALL_NEWSEX;
}

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex )
{
	switch ( emClass )
	{
	case GLCC_FIGHTER_M:
	case GLCC_FIGHTER_W:
		{
			wBeginIndex = EMSKILL_FIGHTER_01;
			wEndIndex = EMSKILL_FIGHTER_04;
		}
		break;

	case GLCC_ARMS_M:
	case GLCC_ARMS_W:
		{
			wBeginIndex = EMSKILL_ARMS_01;
			wEndIndex = EMSKILL_ARMS_04;
		}
		break;

	case GLCC_ARCHER_M:
	case GLCC_ARCHER_W:
		{
			wBeginIndex = EMSKILL_ARCHER_01;
			wEndIndex = EMSKILL_ARCHER_04;
		}
		break;

	case GLCC_SPIRIT_M:
	case GLCC_SPIRIT_W:
		{
			wBeginIndex = EMSKILL_SPIRIT_01;
			wEndIndex = EMSKILL_SPIRIT_04;
		}
		break;	

	case GLCC_EXTREME_M:
	case GLCC_EXTREME_W:
		{
			wBeginIndex = EMSKILL_EXTREME_01;
			wEndIndex = EMSKILL_EXTREME_04;
		}
		break;

	case GLCC_SCIENTIST_M:
	case GLCC_SCIENTIST_W:
		{
			wBeginIndex = EMSKILL_SCIENTIST_01;
			wEndIndex = EMSKILL_SCIENTIST_04;			
		}
		break;

    case GLCC_ASSASSIN_M:
    case GLCC_ASSASSIN_W:
        {
            wBeginIndex = EMSKILL_ASSASSIN_01;
            wEndIndex = EMSKILL_ASSASSIN_04;
        }
        break;

    case GLCC_TRICKER_M:
    case GLCC_TRICKER_W:
        {
            wBeginIndex = EMSKILL_TRICKER_01;
            wEndIndex = EMSKILL_TRICKER_04;
        }
        break;
	case GLCC_ACTOR_M:
	case GLCC_ACTOR_W:
		{
			// TODO : �ӽ��׽�Ʈ
			//wBeginIndex = EMSKILL_ACTOR_01;
			//wEndIndex = EMSKILL_ACTOR_04;
			wBeginIndex = EMSKILL_EXTREME_01;
			wEndIndex = EMSKILL_EXTREME_04;
		}
		break;
	default:
		
		sc::writeLogError(sc::string::format("GetCharSkillClassIndex() - �߸��� ĳ���� Ŭ�����Դϴ�.(%1%)",emClass));		
		GASSERT(0&&"�߸��� �ɸ��� Ŭ�����Դϴ�.");

		break;
	};
}

namespace COMMENT
{
	std::string ACTIONTYPE[GLAT_SIZE] =
	{
		"GLAT_IDLE",
		"GLAT_MOVE",
		"GLAT_ATTACK",
		"GLAT_SKILL",
		"GLAT_SHOCK",
		"GLAT_PUSHPULL",
		"GLAT_GATHERING",
		"GLAT_TALK",
		"GLAT_CONFT_END",
		"GLAT_MOTION",

		"GLAT_AUTHED",
		"GLAT_FALLING",
		"GLAT_DIE",
	};

	std::string TRIBE[TRIBE_NSIZE] = 
	{
		"�ΰ�",
		"����",
		"õ��",
		"�Ǹ�",
		"����",
		"����",
		"�巡��"
	};

	std::string MOVETYPE[MOVETYPE_NSIZE] = 
	{
		"���� �̵�",
		"���� �̵�",
		"���� �̵�"
	};

	std::string ELEMENT[EMELEMENT_MAXNUM2] = 
	{
		"����",
		"��",
		"����",
		"����",
		"��",
		"��ȭ",
		"����",
		"����",
		"����",
		"������ ����"
	};

	std::string BRIGHT[BRIGHT_SIZE] =
	{
		"{��}",
		"{���}",
		"[��+���]"
	};

	std::string CHARCLASS[GLCI_NUM_ACTOR] =
	{
		"�����γ�",
		"�˵��γ�",
		"��úο�",
		"�⿹�ο�",
		"�ذ��γ�",
		"�ذ��ο�",
		"�����ο�",
		"�˵��ο�",
		"��úγ�",
		"�⿹�γ�",
		"���кγ�",
		"���кο�",
        "�μ��γ�",
        "�μ��ο�",
		"�����γ�",
		"�����ο�",
        "��Ÿ��",
        "��Ÿ��",
		"���غγ�",
		"���غο�",
	};	
	
	std::string EMSCHOOL[GLSCHOOL_NUM] =
	{
		"school 1",
		"school 2",
		"school 3",
	};

	std::string CHARCOMMENT[GLCI_NUM_ACTOR] = 
	{
		"", // ������ ��;
		"", // �˵��� ��;
		"", // ��ú� ��;
		"", // �⿹�� ��;
		"", // �ذ��� ��;
		"", // �ذ��� ��;
		"", // ������ ��;
		"", // �˵��� ��;
		"", // ��ú� ��;
		"", // �⿹�� ��;
		"", // ���к� ��;
		"", // ���к� ��;
        "", // �μ��� ��;
        "", // �μ��� ��;
        "", // ������ ��;
        "", // ������ ��;
		"", // ��Ÿ ��;
		"", // ��Ÿ ��;
		"", // ���غ� ��
		"", // ���غ� ��
	};

	std::string TRANSFORM_SET[EMTRANSFORM_SET_MAXSIZE] =
	{
		"",
	};

	std::string TFCHAR_SET[EMTRANSFORM_CHAR_SET_MAXSIZE] =
	{
		"",
	};

	std::string BLOW[EMBLOW_SIZE] =
	{
		"����",	// 0
		"����",	// 1
		"����",	// 2
		"��ȭ",	// 3
		"ȭ��",	// 4
		"�õ�",	// 5

		"����",	// 6
		"�ߵ�",	// 7
		"����",	// 8
	};

	std::string BLOW_VAR1[EMBLOW_SIZE] =
	{
		"����",
		"�̼Ӻ�ȭ��",	//"����"
		"����",			//"����"
		"�̼Ӻ�ȭ��",	//"��ȭ"
		"����",			//"ȭ��"
		"�̼Ӻ�ȭ��",	//"�õ�"

		"����",			//"����"
		"����",			//"�ߵ�"
		"����",			//"����"
	};

	float BLOW_VAR1_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,			//"����"
		1.0f,			//"����"
		100.0f,			//"��ȭ"
		1.0f,			//"ȭ��"
		100.0f,			//"�õ�"

		1.0f,			//"����"
		1.0f,			//"�ߵ�"
		1.0f,			//"����"
	};

	std::string BLOW_VAR2[EMBLOW_SIZE] =
	{
		"����",
		"�����̺�ȭ��",		//"����"
		"����",				//"����"
		"����",				//"��ȭ"
		"����Ÿ��ġ",		//"ȭ��"
		"Ÿ�ݰ�����",		//"�õ�"

		"����",				//"����"
		"����Ÿ��ġ",		//"�ߵ�"
		"����Ÿ��ġ",		//"����"
	};

	float BLOW_VAR2_SCALE[EMBLOW_SIZE] =
	{
		1.0f,
		100.0f,				//"����"
		1.0f,				//"����"
		1.0f,				//"��ȭ"
		1.0f,				//"ȭ��"
		100.0f,				//"�õ�"

		1.0f,				//"����"
		1.0f,				//"�ߵ�"
		1.0f,				//"����"
	};

	std::string IMPACT_ADDON[EIMPACTA_SIZE] =
	{
		"����",
		"������ ��ȭ��",
		"ȸ���� ��ȭ��",
	
		"����ġ ��ȭ��",
		"���ġ ��ȭ��",

		"HP ��ȭ��",
		"MP ��ȭ��",
		"SP ��ȭ��",
		"HP+MP+SP ��ȭ��",

		"����ġ ��ȭ��",
		"���ġ ��ȭ��",

		"����ġ",
		"���ġ",
		"���ġ",

		"HP ������",
		"MP ������",
		"SP ������",
		"����ġ ��ȭ",
		"�ɷ�ġ ��ȭ",

        "HP ȸ���� ����",
        "MP ȸ���� ����",
        "SP ȸ���� ����",
        "CP ȸ���� ����",
	};

    // 1.0f     �̸� �Ǽ� ���� ex) 1,2,3,4
    // 100.0f  �̸� %����      ex) 1%, 2%, 3%
	float IMPACT_ADDON_SCALE[EIMPACTA_SIZE] =
	{
		1.0f,
		1.0f,
		1.0f,
	
		1.0f,
		1.0f,

		100.0f,
		100.0f,
		100.0f,
		100.0f,

		100.0f,
		100.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,
		1.0f,

		1.0f,
		1.0f,

        100.0f,
        100.0f,
        100.0f,
        100.0f,
        1.0f,
	};

	std::string IMPACT_TAR[EMIMPACT_TARGET_NSIZE] =
	{
		"�ڽ�",
		"Ÿ��",
		"�ڽź��� Ÿ�ٱ���",
		"������ġ�� �ֺ� Ÿ��",
		"������ġ"
	};

	std::string IMPACT_REALM[EMIMPACT_REALM_NSIZE] =
	{
		"'��ǥ' �ڽ�",
		"'��ǥ' ����",
		"'��ǥ' ���� ����",
		"'��ǥ'�� ���� ��ä��"
	};

	std::string IMPACT_SIDE[EMIMPACT_SIDE_NSIZE] =
	{
		"�ڱ�����",
		"������",
		"��ο���"
	};

	std::string SPEC_ADDON[EMSPECA_NSIZE] =
	{
		"��� ����",						// 0.��ɾ���.
		"����/�б�",					// 1.����/�б�.
		"����� �ݻ�",				   // 2.������ݻ�.
		"��Ȱ",							   // 3.��Ȱ.
		"HP ������",						// 4.HP ������.
		"MP ������",					   // 5.MP ������.
		"SP ������",						// 6.SP ������.
		"HP ����",                          // 7.HP ����.
		"MP ����",							// 8.MP ����.
		"SP ����",                          // 9.SP ����.
		"�����̻� ����",                // 10.�����̻� ����.
		"�����̻� ȸ��",                // 11.�����̻� ȸ��.
		"��������",                        // 12.��������.
		"�����Ÿ�",                        // 13.�����Ÿ�.
		"�̵��ӵ�(�Ǽ�)",                        // 14.�̵��ӵ�.
		"����",							   // 15.����
		"����",							   // 16.����
		"���� ����",					   // 17.���� ����
		"���ݼӵ�",                        // 18.������.
		"��ų ������",				   // 19.��ų ������ ��ȭ��
		"����Ÿ��",                        // 20.��Ÿ		
		"���� ������ �����",        // 21.���� ������ �����
		"���� ������ �����",        // 22.���� ������ �����
		"���� ������ �ݻ���",        // 23.���� ������ �ݻ���
		"���� ������ �ݻ���",        // 24.���� ������ �ݻ���
		"��ų ȿ�� ����",			   // 25.��ųȿ�� ����
		"����",								// 26.����
		"�ߵ��� ��ų",				   // 27.�ߵ��� ��ų
		"�������",                        // 28.�������
		"�о��",                        // 29.�о��
		"���ӵ�����",						// 30.���ӵ�����
		"����",							   // 31.����
		"��ų�����Ÿ� ��ȭ",			// 32.�����Ÿ�
		"��ų���뿵�� ��ȭ",			// 33.���� ����
		"���������",					// 34.���� ��� ����
		"��ų������",					// 35.��ų ��� ����
		"������ ���",				   // 36.������ ���
		"�����۵����",					// 37.������ �����
		"�ݾ׵����",						// 38.�ݾ� �����
		"����ġ������",					// 39.����ġ ������
		"Ư�� NPC ��ȭ",				   // 40.Ư�� NPC ��ȭ
		"Ư�� ������ ���",		   // 41.Ư�� ������ ���
		"�����̵�",                        // 42.���� �̵�
		"��ġ����",                        // 43.��ġ ����
		"����",							   // 44.����
		"���",							   // 45.���
        "����",							   // 46.����
		"���ӽð�����",					// 47.���ӽð� ����
		"����",								// 48.����
		"ȯ��",							   // 49.ȯ��
		"�ǰݽ� ȿ�� ����",		   // 50. �ǰݽ� ��ųȿ�� ����
		"��",							   // 51. ��
		"����",							   // 52. ����
		"�鿪",							   // 53. �鿪
		"��������",                        // 54. ��������
		"�������� ����"                 // 55. �������� ����
		"����",							   // 56. ����		
		"����",							   // 57. ����
		"��������ġ������",			// 58. ���� ����ġ ������;
		"��ų ��ũ",					// 59. ��ų ��ũ;
		"���ݷ�(����)",					//60. ���ݷ� ���� 
		"���ݷ�(�Ǽ�, ��ų ������)",	//61. ���ݷ� % (�ٰŸ�, ���Ÿ� ������)
		"����(����)",					//62. ���� ����
		"����(�Ǽ�)",					//63. ���� %
		"������(����)",					//64. ������ ����
		"������(�Ǽ�)",					//65. ������ %
		"ȸ����(����)",					//66. ȸ���� ����
		"ȸ����(�Ǽ�)",					//67. ȸ���� %
		"HP ����",						//68. HP����
		"MP ����",						//69. MP����
		"SP ����",						//70. SP����
		"CP ����(�̻��)",				//71. CP����
		"HP ȸ����",					//72. HPȸ����
		"MP ȸ����",					//73. MPȸ����
		"SP ȸ����",					//74. SPȸ����
		"HP,MP,SP ȸ����",				//75. HP,MP,SP ȸ����
		"���� ����",					//76. ���� ����
		"SP �Ҹ�",					//77. SP �Ҹ�
		"����",							//78. ����
		"��",							//79. ��
		"ü��",							//80. ü��
		"����",							//81. ����
		"��ø",							//82. ��ø
		"�ٷ�",							//83. �ٷ�
		"����ġ",						//84. ����ġ
		"���ġ",						//85. ���ġ
		"����ġ",						//86. ����ġ
		"���� HP ������",				//87. ���� HP ������
		"���� MP ������",				//88. ���� MP ������
		"���� SP ������",				//89. ���� SP ������
		"���� ų�� CP ������",		//90. ���� ų�� CP ������
		"�̵��ӵ�(����)"				//91. �̵��ӵ� ���� ������(Ż�Ͱ� ������ ��ġ)
		"������(�Ǽ�, ��ü������)",		//92. ������ (�ٰŸ�, ��� ����ġ ���� ���� ���� ��ġ���� ������)
 
	};

	std::string SPEC_ADDON_VAR1[EMSPECA_NSIZE] =
	{
		"����",				// 0.��ɾ���.
		"�Ÿ�",				// 1.����/�б�.
		"����",				// 2.������ݻ�.
		"ȸ����",			// 3.��Ȱ.
		"�����",			// 4.HP ������.
		"�����",			// 5.MP ������.
		"�����",			// 6.SP ������.
		"����",				// 7.HP ����.
		"����",				// 8.MP ����.
		"����",				// 9.SP ����.
		"����",				// 10.�����̻� ����.
		"����",				// 11.�����̻� ȸ��.
		"��������",			// 12.��������.
		"���ݰŸ�",			// 13.�����Ÿ�.
		"�̼���",			// 14.�̵��ӵ�.
		"�Ÿ�",				// 15.����
		"���� ����",		// 16.����
		"���� ����",		// 17.���� ����
		"������",			// 18.������.
		"��ȭ��",			// 19.��ų ������ ��ȭ��
		"�Ÿ�",				// 20.��Ÿ		
		"�����",			// 21.���� ������ �����
		"�����",			// 22.���� ������ �����
		"�ݻ���",			// 23.���� ������ �ݻ���
		"�ݻ���",			// 24.���� ������ �ݻ���
		"����",				// 25.��ųȿ�� ����
		"����",				// 26.����
		"Ȯ��",				// 27.�ߵ��� ��ų
		"Ȯ��",				// 28.�������
		"Ȯ��",				// 29.�о��
		"�ð�",				// 30.���ӵ�����
        "�ݻ���",			// 31.����
        "�Ÿ�",				// 32.�����Ÿ�
        "�Ÿ�",				// 33.���� ����
        "����",				// 34.���� ��� ����
        "����",				// 35.��ų ��� ����
        "Ƚ��",				// 36.������ ���
        "������",			// 37.������ �����
        "������",			// 38.�ݾ� �����
        "������",			// 39.����ġ ������
        "����",				// 40.Ư�� NPC ��ȭ
        "����",				// 41.Ư�� ������ ���
        "����",				// 42.���� �̵�
        "Ȯ��",				// 43.��ġ ����
		"��ŸȮ��",			// 44.��ȭ
        "����",				// 45.���
        "Ȯ��",				// 46.����
		"��������1",		// 47.���ӽð� ����
		"�����Ÿ�",			// 48.����
		"����ID",			// 49.ȯ��
		"Ȯ��",				// 50. �ǰݽ� ��ųȿ�� ����
		"Ȯ��",				// 51. ��
		"Ȯ��",				// 52. ����
		"����Ÿ��",			// 53. �鿪
		"Ȯ��",				// 54. �������� ȿ��
		"����",				// 55. �������� ����
		"Ȯ��",				// 56. ����		
		"Ȯ��",				// 57. ����
		"����",				// 58. ���� ����ġ ������;
		"Ȯ��",				// 59. ��ų ��ũ;
		"��",					// 60. ���ݷ�(����);
		"��",					// 61. ���ݷ�(����);
		"��",					// 62. ����(����);
		"��",					// 63. ����(����);
		"��",					// 64. ���߷�(����);
		"��",					// 65. ���߷�(����);
		"��",					// 66. ȸ����(����);
		"��",					// 67. ȸ����(����);
		"��",					// 68. hp����;
		"��",					// 69. mp����;
		"��",					// 70. sp����;
		"��",					// 71. cp����;
		"��",					// 72. hpȸ����;
		"��",					// 73. mpȸ����;
		"��",					// 74. spȸ����;
		"��",					// 75. hp, mp, sp ȸ����;
		"��",					// 76. ���� ����;
		"��",					// 77. sp �Ҹ�;
		"��",					// 78. ����;
		"��",					// 79. ��(����);
		"��",					// 80. ü��(����);
		"��",					// 81. ����(����);
		"��",					// 82. ��ø(����);
		"��",					// 83. �ٷ�(����);
		"��",					// 84. ����ġ(����);
		"��",					// 85. ���ġ(����);
		"��",					// 86. ����ġ(����);
		"��",					// 87. ���� hp����(����);
		"��",					// 88. ���� mp����(����);
		"��",					// 89. ���� sp����(����);
		"��",					// 90. cp����(����);
		"��",					// 91. �̵��ӵ�;
		"��",					// 92. ������(����);
		"��",					// 93. �þ� ����(����);
	};

    // 1.0f     �̸� �Ǽ� ���� ex) 1,2,3,4
    // 100.0f  �̸� %����      ex) 1%, 2%, 3%
	float SPEC_ADDON_VAR1_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,			// 0.��ɾ���.
		1.0f,			// 1.����/�б�.
		1.0f,			// 2.������ݻ�.
		100.0f,		// 3.��Ȱ.
		100.0f,		// 4.HP ������.
		100.0f,		// 5.MP ������.
		100.0f,		// 6.SP ������.
		1.0f,			// 7.HP ����.
		1.0f,			// 8.MP ����.
		1.0f,			// 9.SP ����.
		1.0f,			// 10.�����̻� ����.
		1.0f,			// 11.�����̻� ȸ��.
		1.0f,			// 12.��������.
		1.0f,			// 13.�����Ÿ�.
		100.0f,		// 14.�̵��ӵ�.
		100.0f,		// 15.����
		1.0f,			// 16.����
		80.0f,			// 17.���� ����
		100.0f,		// 18.������.
		1.0f,			// 19.��ų ������ ��ȭ��
		100.0f,		// 20.��Ÿ		
		100.0f,		// 21.���� ������ �����
		100.0f,		// 22.���� ������ �����
		100.0f,		// 23.���� ������ �ݻ���
		100.0f,		// 24.���� ������ �ݻ���
		1.0f,			// 25.�̷ο� ����ȿ�� ����
		1.0f,			// 26.����
		100.0f,		// 27.�ߵ��� ��ų
		100.0f,		// 28.�������
		100.0f,		// 29.�о��
		1.0f,			// 30.���ӵ�����        
        1.0f,			// 31.����
        1.0f,			// 32.�����Ÿ�
        1.0f,			// 33.���� ����
        1.0f,			// 34.���� ��� ����
        1.0f,			// 35.��ų ��� ����
        1.0f,			// 36.������ ���
        1.0f,			// 37.������ �����
        1.0f,			// 38.�ݾ� �����
        1.0f,			// 39.����ġ ������
        1.0f,			// 40.Ư�� NPC ��ȭ
        1.0f,			// 41.Ư�� ������ ���
        1.0f,			// 42.���� �̵�
        1.0f,			// 43.��ġ ����
		100.0f,			// 44.��ȭ
        1.0f,			// 45.���
		1.0f,			// 46.����
		1.0f,			// 47.���ӽð� ����
		0.0f,			// 48.����
		0.0f,			// 49.ȯ��
		0.0f,			// 50. �ǰݽ� ��ųȿ�� ����
		0.0f,			// 51. ��
		0.0f,			// 52. ����
		0.0f,			// 53. �鿪
		0.0f,			// 54. �������� ȿ��
		0.0f,			// 55. �������� ����
		1.0f,			// 56. ����		
		1.0f,			// 57. ����	
		1.0f,			// 58. ���� ����ġ ������;
		0.0f,			// 59. ��ų ��ũ;
		0.0f,			// 60. ���ݷ�(����);
		1.0f,			// 61. ���ݷ�(����);
		0.0f,			// 62. ����(����);
		1.0f,			// 63. ����(����);
		0.0f,			// 64. ���߷�(����);
		1.0f,			// 65. ���߷�(����);
		0.0f,			// 66. ȸ����(����);
		1.0f,			// 67. ȸ����(����);
		0.0f,			// 68. hp����;
		0.0f,			// 69. mp����;
		0.0f,			// 70. sp����;
		0.0f,			// 71. cp����;
		1.0f,			// 72. hpȸ����;
		1.0f,			// 73. mpȸ����;
		1.0f,			// 74. spȸ����;
		1.0f,			// 75. hp, mp, sp ȸ����;
		0.0f,			// 76. ���� ����;
		0.0f,			// 77. sp �Ҹ�;
		0.0f,			// 78. ����;
		0.0f,			// 79. ��(����);
		0.0f,			// 80. ü��(����);
		0.0f,			// 81. ����(����);
		0.0f,			// 82. ��ø(����);
		0.0f,			// 83. �ٷ�(����);
		0.0f,			// 84. ����ġ(����);
		0.0f,			// 85. ���ġ(����);
		0.0f,			// 86. ����ġ(����);
		0.0f,			// 87. ���� hp����(����);
		0.0f,			// 88. ���� mp����(����);
		0.0f,			// 89. ���� sp����(����);
		0.0f,			// 90. cp����(����);
		0.0f,			// 91. �̵��ӵ�;
		1.0f,			// 92. ������(����);
		1.0f,			// 93. �þ� ����(����);
	};

	std::string SPEC_ADDON_VAR2[EMSPECA_NSIZE] =
	{
		"����",				// 0.��ɾ���.
		"Ȯ��",				// 1.����/�б�.
		"����",				// 2.������ݻ�.
		"����",				// 3.��Ȱ.
		"����",				// 4.HP ������.
		"����",				// 5.MP ������.
		"����",				// 6.SP ������.
		"����",				// 7.HP ����.
		"����",				// 8.MP ����.
		"����",				// 9.SP ����.
		"����",				// 10.�����̻� ����.
		"����",				// 11.�����̻� ȸ��.
		"����",				// 12.��������.
		"����",				// 13.�����Ÿ�.
		"����",				// 14.�̵��ӵ�.
		"�ӵ�",				// 15.����
		"�ִϸ��̼�",		// 16.����
		"���� ����",		// 17.���� ����
		"����",				// 18.������.
		"����",				// 19.��ų ������ ��ȭ��
		"����",				// 20.��Ÿ		
		"����",				// 21.���� ������ �����
		"Ȯ��",				// 22.���� ������ �����
		"Ȯ��",				// 23.���� ������ �ݻ���
		"����",				// 24.���� ������ �ݻ���
		"����",				// 25.�̷ο� ����ȿ�� ����
		"����",				// 26.����
		"����",				// 27.�ߵ��� ��ų
		"�̵��ӵ�",			// 28.�������
		"�Ÿ�",				// 29.�о��
        "������",		   // 30.���ӵ�����        
        "����",			   // 31.����
        "����",			   // 32.�����Ÿ�
        "����",			   // 33.���� ����
        "����",			   // 34.���� ��� ����
        "����",			   // 35.��ų ��� ����
        "�����",			// 36.������ ���
        "����",			   // 37.������ �����
        "����",			   // 38.�ݾ� �����
        "����",			   // 39.����ġ ������
        "����",			   // 40.Ư�� NPC ��ȭ
        "����",			   // 41.Ư�� ������ ���
        "����",			   // 42.���� �̵�
        "�̵� �ӵ�",		// 43.��ġ ����
        "��������",			// 44.��ȭ
		"����",				// 45.���
		"���ӿ���",			// 46.����
		"��������2",		// 47.���ӽð� ����
		"�������",			// 48.����
		"�̸�Ƽ��ID",		// 49.ȯ��
		"�ൿŸ��",			// 50. �ǰݽ� ��ųȿ�� ����
		"����",				// 51. ��
		"����",				// 52. ����
		"�ൿŸ��",			// 53. �鿪
		"���ӽð�",			// 54. �������� ȿ��
		"����",				// 55. �������� ����
		"��ȿ�Ÿ�",			// 56. ����		
		"�������",			// 57. ����
		"��øȸ��",			// 58. ���� ����ġ ������;
		"����",				// 59. ��ų ��ũ;
		"����",				// 60. ���ݷ�(����);
		"����",				// 61. ���ݷ�(����);
		"����",				// 62. ����(����);
		"����",				// 63. ����(����);
		"����",				// 64. ���߷�(����);
		"����",				// 65. ���߷�(����);
		"����",				// 66. ȸ����(����);
		"����",				// 67. ȸ����(����);
		"����",				// 68. hp����;
		"����",				// 69. mp����;
		"����",				// 70. sp����;
		"����",				// 71. cp����;
		"����",				// 72. hpȸ����;
		"����",				// 73. mpȸ����;
		"����",				// 74. spȸ����;
		"����",				// 75. hp, mp, sp ȸ����;
		"����",				// 76. ���� ����;
		"����",				// 77. sp �Ҹ�;
		"����",				// 78. ����;
		"����",				// 79. ��(����);
		"����",				// 80. ü��(����);
		"����",				// 81. ����(����);
		"����",				// 82. ��ø(����);
		"����",				// 83. �ٷ�(����);
		"����",				// 84. ����ġ(����);
		"����",				// 85. ���ġ(����);
		"����",				// 86. ����ġ(����);
		"����",				// 87. ���� hp����(����);
		"����",				// 88. ���� mp����(����);
		"����",				// 89. ���� sp����(����);
		"����",				// 90. cp����(����);
		"����",				// 91. �̵��ӵ�;
		"����",				// 92. ������(����);
		"����",				// 93. �þ� ����(����);
	};

    // 1.0f     �̸� �Ǽ� ���� ex) 1,2,3,4
    // 100.0f  �̸� %����      ex) 1%, 2%, 3%
	float SPEC_ADDON_VAR2_SCALE[EMSPECA_NSIZE] =
	{
		1.0f,			// 0.��ɾ���.
		100.0f,		// 1.����/�б�.
		1.0f,			// 2.������ݻ�.
		1.0f,			// 3.��Ȱ.
		1.0f,			// 4.HP ������.
		1.0f,			// 5.MP ������.
		1.0f,			// 6.SP ������.
		1.0f,			// 7.HP ����.
		1.0f,			// 8.MP ����.
		1.0f,			// 9.SP ����.
		1.0f,			// 10. �����̻� ����.
		1.0f,			// 11. �����̻� ȸ��.
		1.0f,			// 12. ��������.
		1.0f,			// 13. �����Ÿ�.
		1.0f,			// 14. �̵��ӵ�.
		1.0f,			// 15. ����
		0.0f,			// 16. ����
		1.0f,			// 17. ���� ����
		1.0f,			// 18. ������.
		1.0f,			// 19. ��ų ������ ��ȭ��
		1.0f,			// 20. ��Ÿ		
		1.0f,			// 21. ���� ������ �����
		100.0f,		// 22. ���� ������ �����
		100.0f,		// 23. ���� ������ �ݻ���
		100.0f,		// 24. ���� ������ �ݻ���
		1.0f,			// 25. �̷ο� ����ȿ�� ����
		1.0f,			// 26. ����
		1.0f,			// 27. �ߵ��� ��ų
		1.0f,			// 28. �������
		1.0f,			// 29. �о��
        1.0f,			// 30. ���ӵ�����        
        1.0f,			// 31. ����
        1.0f,			// 32. �����Ÿ�
        1.0f,			// 33. ���� ����
        1.0f,			// 34. ���� ��� ����
        1.0f,			// 35. ��ų ��� ����
        1.0f,			// 36. ������ ���
        1.0f,			// 37. ������ �����
        1.0f,			// 38. �ݾ� �����
        1.0f,			// 39. ����ġ ������
        1.0f,			// 40. Ư�� NPC ��ȭ
        1.0f,			// 41. Ư�� ������ ���
        1.0f,			// 42. ���� �̵�
        1.0f,			// 43. ��ġ ����
        100.0f,		// 44. ��ȭ
        1.0f,			// 45. ���
		1.0f,			// 46. ����
		1.0f,			// 47. ���ӽð� ����
		1.0f,			// 48. ����
		1.0f,			// 49. ȯ��
		1.0f,			// 50. �ǰݽ� ��ųȿ�� ����
		1.0f,			// 51. ��
		1.0f,			// 52. ����
		1.0f,			// 53. �鿪
		1.0f,			// 54. �������� ȿ��
		1.0f,			// 55. �������� ����
		1.0f,			// 56. ����		
		10.0f,			// 57. ����
		1.0f,			// 58. ���� ����ġ ������;
		0.0f,			// 59. ��ų ��ũ;
		0.0f,			// 60. ���ݷ�(����);
		0.0f,			// 61. ���ݷ�(����);
		0.0f,			// 62. ����(����);
		0.0f,			// 63. ����(����);
		0.0f,			// 64. ���߷�(����);
		0.0f,			// 65. ���߷�(����);
		0.0f,			// 66. ȸ����(����);
		0.0f,			// 67. ȸ����(����);
		0.0f,			// 68. hp����;
		0.0f,			// 69. mp����;
		0.0f,			// 70. sp����;
		0.0f,			// 71. cp����;
		0.0f,			// 72. hpȸ����;
		0.0f,			// 73. mpȸ����;
		0.0f,			// 74. spȸ����;
		0.0f,			// 75. hp, mp, sp ȸ����;
		0.0f,			// 76. ���� ����;
		0.0f,			// 77. sp �Ҹ�;
		0.0f,			// 78. ����;
		0.0f,			// 79. ��(����);
		0.0f,			// 80. ü��(����);
		0.0f,			// 81. ����(����);
		0.0f,			// 82. ��ø(����);
		0.0f,			// 83. �ٷ�(����);
		0.0f,			// 84. ����ġ(����);
		0.0f,			// 85. ���ġ(����);
		0.0f,			// 86. ����ġ(����);
		0.0f,			// 87. ���� hp����(����);
		0.0f,			// 88. ���� mp����(����);
		0.0f,			// 89. ���� sp����(����);
		0.0f,			// 90. cp����(����);
		0.0f,			// 91. �̵��ӵ�;
		0.0f,			// 92. ������(����);
		0.0f,			// 93. �þ� ����(����);

	};

	std::string SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_NSIZE] =
	{
		"�Ϲݰ��� ����",
		"��ų���� ����",
		"����",
		"�����",
		"����Ʈ",
	};

	std::string SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_NSIZE] =
	{
		"����",
		"�����̸�Ƽ��",
		"Ÿ���� �Ұ�"		
	};

	std::string SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_NSIZE] =
	{
		"�ڽ�",
		"������",
		"������",
	};

    std::string SPEC_ASIGN_TYPE[EMSPEC_SKILL_ASING_NSIZE] =
    {
        "���� ��",
        "���� ��"        
    };

    std::string SPEC_INC_TYPE[EMSPEC_INC_TYPE_NSIZE] =
    {
        "����",
        "����"
    };

	std::string SPEC_INVISIBLE_USE_TYPE = "���������� ���";

    std::string SPEC_SKILL_CONTINUE = "����";

	std::string SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_NSIZE] =
	{
		"����",
		"�ȱ�",
		"�ٱ�",
	};

	std::string SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_NSIZE] = 
	{
		"����",
		"�ڽ�",
		"Ÿ��",
	};

	std::string SKILLCLASS[EMSKILLCLASS_NSIZE] =
	{
		"������-class1",
		"������-class2",
		"������-class3",
		"������-class4",

		"�˵���-class1",
		"�˵���-class2",
		"�˵���-class3",
		"�˵���-class4",

		"��ú�-class1",
		"��ú�-class2",
		"��ú�-class3",
		"��ú�-class4",

		"�⿹��-class1",
		"�⿹��-class2",
		"�⿹��-class3",
		"�⿹��-class4",

		"NPC_01",
		"NPC_02",
		"NPC_03",
		"NPC_04",
		"NPC_05",
		"NPC_06",
		"NPC_07",
		"NPC_08",
		"NPC_09",
		"NPC_10",
		"NPC_11",
		"NPC_12",
		"NPC_13",
		"NPC_14",
		"�ذ���-class1",
		"�ذ���-class2",
		"�ذ���-class3",
		"�ذ���-class4",
		"NPC_19",
		"NPC_20"

		"���к�-class1",
		"���к�-class2",
		"���к�-class3",
		"���к�-class4",

		"����(���ö�)",

		"��ȯ��",
        "BIKE",

        "�μ���-class1",
        "�μ���-class2",
        "�μ���-class3",
        "�μ���-class4",

        "��Ÿ-class1",
        "��Ÿ-class2",
        "��Ÿ-class3",
        "��Ÿ-class4",

        "������",
		"�̺�Ʈ",
	};

	std::string LANDEFFECT_TYPE[EMLANDEFFECT_SIZE] =
	{
		"���ݼӵ�",
		"�̵��ӵ�",
		"HP ������",
		"MP ������",
		"ȸ����",
		"����ġ ��ȭ��",
		"���ġ ��ȭ��",
		"���׼�ġ",
		"��� �������"
	};

	int SPEC_CA[EMSPECA_CAK_MAX] = 
	{
		0,
	};
};

void SaveEmCharClassExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);

    int Row = 0;
    int Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "Type");               Xls.SetCell(SheetNum, Row, Col++, "Value"); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_NONE");          Xls.SetCell(SheetNum, Row, Col++, GLCC_NONE); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER_M");     Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS_M");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER_W");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT_W");      Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME_M");     Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME_W");     Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER_W");     Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS_W");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS_W); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER_M");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT_M");      Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT_M); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST_M");   Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST_W");   Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN_M");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN_W");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN_W); Row++; Col = 0;

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER_M");    Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER_M); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER_W");    Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC_M");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC_M); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC_W");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC_W); Row++; Col = 0;

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR_M");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR_M); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR_W");         Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR_W); Row++; Col = 0;

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_FIGHTER");       Xls.SetCell(SheetNum, Row, Col++, GLCC_FIGHTER); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARMS");          Xls.SetCell(SheetNum, Row, Col++, GLCC_ARMS); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ARCHER");        Xls.SetCell(SheetNum, Row, Col++, GLCC_ARCHER); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SPIRIT");        Xls.SetCell(SheetNum, Row, Col++, GLCC_SPIRIT); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_EXTREME");       Xls.SetCell(SheetNum, Row, Col++, GLCC_EXTREME); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_SCIENTIST");     Xls.SetCell(SheetNum, Row, Col++, GLCC_SCIENTIST); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ASSASSIN");      Xls.SetCell(SheetNum, Row, Col++, GLCC_ASSASSIN); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_TRICKER");      Xls.SetCell(SheetNum, Row, Col++, GLCC_TRICKER); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ETC");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ETC); Row++; Col = 0;      

	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ACTOR");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ACTOR); Row++; Col = 0;
	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL");           Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL); Row++; Col = 0;		
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_OLD_EX");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_OLD_EX); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_NEWSEX");    Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_NEWSEX); Row++; Col = 0;	
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_SCIENTIST"); Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_SCIENTIST); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_ASSASSIN");  Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_ASSASSIN); Row++; Col = 0; 
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_TRICKER");   Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_TRICKER); Row++; Col = 0; 
	Xls.SetCell(SheetNum, Row, Col++, "GLCC_ALL_ACTOR");     Xls.SetCell(SheetNum, Row, Col++, GLCC_ALL_ACTOR); Row++; Col = 0; 

    Xls.SetCell(SheetNum, Row, Col++, "GLCC_MAN");           Xls.SetCell(SheetNum, Row, Col++, GLCC_MAN); Row++; Col = 0;		
    Xls.SetCell(SheetNum, Row, Col++, "GLCC_WOMAN");         Xls.SetCell(SheetNum, Row, Col++, GLCC_WOMAN); Row++; Col = 0;		
}

void SaveEmSchoolFlagExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName)
{
    Xls.RenameWorksheet(SheetNum, SheetName);
    int Row = 0;
    int Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "School");             Xls.SetCell(SheetNum, Row, Col++, "Value"); Row++; Col = 0;
	// �Ʒ��� COMMENT�� �����Ϳ��� �о�����ʴ´�;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[0]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_00); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[1]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_01); Row++; Col = 0;
	Xls.SetCell(SheetNum, Row, Col++, COMMENT::EMSCHOOL[2]); Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_02); Row++; Col = 0;
    Xls.SetCell(SheetNum, Row, Col++, "All");                Xls.SetCell(SheetNum, Row, Col++, GLSCHOOL_ALL); Row++; Col = 0;
}

SSEARCHITEMESULT::SSEARCHITEMESULT()
    : nSearchItemID(false)
    , llPRICE(0)
    , vShopPos(0.0f, 0.0f, 0.0f)
{
    memset(szShopName, 0, sizeof(char) * (MAP_NAME_LENGTH));
}

void SSEARCHITEMESULT::SetShopName(const std::string& Name)
{
    StringCchCopy(szShopName, MAP_NAME_LENGTH, Name.c_str());
}

