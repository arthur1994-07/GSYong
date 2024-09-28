#include "../pch.h"
#include "./GLItemDef.h"
#include "../../SigmaCore/gassert.h"

#include "./GLItemGrindingScript.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

EMGRINDING_RS_INDEX GrindRsClassToIndex ( const EMGRINDING_RS_FLAGS emClass )
{
	switch (emClass)
	{
	case EMGRINDING_RS_FIRE:	return EMGRINDING_RS_FIRE_INDEX;
	case EMGRINDING_RS_ICE:		return EMGRINDING_RS_ICE_INDEX;
	case EMGRINDING_RS_ELECTRIC:return EMGRINDING_RS_ELECTRIC_INDEX;
	case EMGRINDING_RS_POISON:	return EMGRINDING_RS_POISON_INDEX;
	case EMGRINDING_RS_SPIRIT:	return EMGRINDING_RS_SPIRIT_INDEX;
	};

	GASSERT(0&&"�߸��� ���׾����� Ŭ�����Դϴ�.");
	return EMGRINDING_RS_FIRE_INDEX;
}

EMGRINDING_RS_FLAGS GrindIndexToRsClass ( const EMGRINDING_RS_INDEX emIndex )
{
	switch (emIndex)
	{
	case EMGRINDING_RS_FIRE_INDEX:		return EMGRINDING_RS_FIRE;
	case EMGRINDING_RS_ICE_INDEX:		return EMGRINDING_RS_ICE;
	case EMGRINDING_RS_ELECTRIC_INDEX:	return EMGRINDING_RS_ELECTRIC;
	case EMGRINDING_RS_POISON_INDEX:	return EMGRINDING_RS_POISON;
	case EMGRINDING_RS_SPIRIT_INDEX:	return EMGRINDING_RS_SPIRIT;
	};

	GASSERT(0&&"�߸��� ���׾����� Ŭ�����Դϴ�.");
	return EMGRINDING_RS_FIRE;
}

const bool SGRINDING_OPT::ISENABLELOGIC()
{
	if ( spSCRIPT && spSCRIPT->IsExistEntry() )
	{
		return true;
	}

	return false;
}

const bool SGRINDING_OPT::ISENABLELOGIC_CONDITION()
{
	if ( spSCRIPT && spSCRIPT->IsExistCondition() )
	{
		return true;
	}

	return false;
}

const bool SGRINDING_OPT::ISENABLELOGIC_CONSUME()
{
	if ( spSCRIPT && spSCRIPT->IsExistConsume() )
	{
		return true;
	}

	return false;
}

CGrindingScript* SGRINDING_OPT::GETSCRIPT()
{
	return spSCRIPT.get();
}

namespace COMMENT
{
	std::string ITEMLEVEL[EMITEM_LEVEL_NSIZE] =
	{
		"�Ϲ�",
		"������",
		"������",
		"����",
		"�鿭",
	};

	std::string ITEMTYPE[ITEM_NSIZE] =
	{
		"����,����,����",
		"ȭ��",
		"��ǰ",
		"��ų��",
		"��ȯ��",
		"������",
		"������",
		"����",
		"������",
		"��ų����Ʈ ����", 

		"��������Ʈ ����", // 10
		"��ų, ���� ����",
		"��������",
		"������ ����",
		"Ȯ����",
		"����",
		"ĳ���� �߰� ī��",
		"�κ� Ȯ�� ī��",
		"â�� Ȯ�� ī��",
		"â�� ���� ī��", 

		"�����̾� ��Ʈ", // 20
		"���λ��� �㰡��",
		"���� ������",
		"�ڽ��� �и�",
		"��Ÿ�� ����",
		"�� ����",
		"? ������",
		"CD",
		"ģ������",
		"Ŭ��ȣ��", 

		"���� �̿��", // 30
		"�̸����� ī��",
		"��Ÿ��",
		"����÷�",
		"��ȥ��",
		"�� ī��",	// PetData
		"�� ����",
		"�� �̸�����",
		"�� �÷�����",
		"�� ��Ÿ�Ϻ���", 

        "�� ��ų", // 40
        "SMS���� �߼�",
        "�� ��Ȱī��",
        "�ҹ���(�Ҹ����)",
        "���� ���",
        "Ż��",
        "Ż�� �⸧",
        "��Ʈ�� Ž�� ���� ������ ȹ��",
        "��Ʈ�� Ž�� ���� ����ġ ȹ��",
        "���� ���� ī��", 

        "������ ī��", // 50
        "��ȯ��	",
        "�� ��Ų ��",
        "�󱼽�Ÿ��",
        "�ý�ī��",
        "��������",
        "NPC��ȯ ī��",
        "�Ѿ�",
        "���ö�",
        "�� ��ų �ߺ���� ī��", 

        "��ȭ�� (��ȭ Ȯ�� ����)", // 60
        "��ȭ�� (���� �ܰ� ��ȭ)",
        "������� �÷� ����ī��",
        "������� �ν�Ʈ ���ī��",
        "��ų, ���� ����Ʈ ���� ����. (�� ������ ��)",
        "�ڽ�Ƭ �÷� ���� ī��",
        "������ ���� ī��",
        "ȯ�Ұ��� Point Card",
        "ȯ�ҺҰ� Point Card",
		"���ռ�",

		"�����ʱ�ȭ", // 70
        "���λ��� �˻�ī��",
        "�������� ����ī��",
		"����ī��",
		"�������� �н�",
		"�δ� ����Ƚ�� ����",
		"ī����� ����ī��",
        "�� ���ӸӴ�",      
        "��Ź�Ǹ� Ȯ�� ���� ī��", 
		"��� ����",
		
        "��� ����",  // 80
        "������",
        "����ڽ�",
		"�п�����ī��(��������)",
		"�п�����ī��(��������)",
		"�п�����ī��(��Ȳ����)",
		"��������ī��",
		"�ڽ�Ƭ �ɷ�ġ �ο�",
		"ȿ������ ��ǰ",
		"�ڽ�Ƭ �ɷ�ġ �ο� ����",

		"����ġ ī��", // 90
		"������ �ڽ�",	//91
		"����ġ ����", // 92
		"����ġ ĸ��",	// 93

		"�Ͻð�ȭ",

	};

    std::string ItemTypeName(EMITEM_TYPE Type)
    {
        if (Type < ITEM_NSIZE)
            return ITEMTYPE[Type];
        else
            return std::string("");
    }

	std::string ITEMSUIT[SUIT_NSIZE] =
	{
		"���ڷ�",
		"����",
		"����",
		"�尩��",
		"�Ź߷�",

		"�����",

		"�����",
		"����",

		"����",

		"�� A",	// PetData
		"�� B",
		"S.C",
		"S.C Skin",
		"S.C Parts_A",
		"S.C Parts_B",
		"S.C Parts_C",
		"S.C Parts_D",
		"S.C Parts_E",
		"S.C Parts_F",

        "�㸮��",
        "�Ͱ���",
        "��ű�",
		"���",
    };

    std::string ItemSuitName(EMSUIT Type)
    {
        if (Type < SUIT_NSIZE)
            return ITEMSUIT[Type];
        else
            return std::string("");
    }

	std::string ITEMDRUG_ER[ITEM_DRUG_ER_SIZE] =
	{
		"���� ����",
		"ȯ�� ����",
		"��������� ����"
	};

	std::string ITEMDRUG[ITEM_DRUG_SIZE] =
	{
		"NUNE",
		"ü��ȸ��",
		"���ȸ��",
		"�ٷ�ȸ��",
		"ü��+��� ȸ��",
		"ü��+Ȱ�� ȸ��",
		"ü��+���+Ȱ�� ȸ��",
		"�̻�ġ��",

		"�б���ȯ",
		"���۱�ȯ",
		"������ȯ", // 10

		"��Ȱ���",
		"ü��ȸ��+�̻�ġ��",
		"ü��+���+Ȱ��ȸ��+�̻�ġ��",

		"Ư�������̵�",
		"������ġȹ��",

		"�������� �н�",
		"�δ� ����Ƚ�� ������",
		"����ȸ��",
		"�ڽ�Ƭ �ɷ�ġ �ο� ����",
		"ȿ������",
		"�ڽ�Ƭ �ɷ�ġ �ο� ����",

		"����ġ ����(100��)",	//22
		"����ġ ����(1000��)",	//23
		"����ġ ĸ��(100��)",	//24
		"����ġ ĸ��(1000��)",	//25
		"�Ͻð�ȭ",   // sckim , ���� �߰�
		"��ũ�νð�����",
		"�Ⱓ ����",
	};

	std::string ITEMDRUG_VAR[ITEM_DRUG_SIZE] =
	{
		"�뷮",
		"�뷮",
		"�뷮",
		"�뷮",
		"�뷮",
		"�뷮",
		"�뷮",
		"�뷮",

		"�뷮",
		"�뷮",
		"�뷮", //10

		"�뷮",
		"�뷮",
		"�뷮",

		"�뷮",
		"�뷮",
		"Stage", // 16
		"Level", // 17
		"�뷮",
		"���",
		"�뷮", // 20
		"�뷮",
		"����(%)", //22
		"����(%)", //23
		"����(%)", 
		"����(%)",
		"����",
		"�뷮",
	};

	std::string ITEMPROHIBIT[PROHIBIT_ITEM_SIZE] =
	{
		"Taxi_Card_Block",
		"Teleport_Card_Block",
		"Recall_Block",
		"Cure_Block",
		"Special_Potion_Block",
	};

	//std::string ITEMSLOT[SLOT_NSIZE_S] =
	//{
	//	"���ڷ�",
	//	"��ü",
	//	"��ü",
	//	"��",
	//	"��",

	//	"������ ����",
	//	"������ ���� ����",
	//	"�޼� ����",
	//	"�޼� ���� ����",

	//	"�����",
	//	"�ո�",

	//	"������ �հ���",
	//	"�޼� �հ���"
	//};

	std::string ITEMATTACK[ITEMATT_NSIZE+1] =
	{
		"'��'����",			// 0
		"��",					// 1
		"��",					// 2
		"�ܰ�",				// 3
		"â",					// 4
		"������",			// 5
		"��",					// 6
		"Ȱ",					// 7
		"��ô",				// 8
		"��",					// 9
		"���ϰ�",			// 10
		"��Ż��",			// 11
        "�ظ�",				// 12
        "����Ǿ�",		// 13
        "���̵�",			// 14
		"�ذ��۷���",		// 15
		"����������",		// 16
		"��������",			// 17
		"ä��",				// 18
		"����",				// 19
		"��ġ",				// 20
		"���",				// 21
        "Ÿ�԰˻����",		// 22
	};

	std::string ITEMADDON[EMADD_SIZE] =
	{
		"����",
		"������",
		"ȸ����",
		"�����",
		"����",
		"ü��",
		"����",
		"���׹̳�",
		"STATS ��",
		"STATS ü��",
		"STATS ���",
		"STATS ��ø",
		"STATS ����",
		"STATS �ٷ�",
		"����ġ",
		"���ġ",
		"���ġ",

		"����",	
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������",
		"�̵��ӵ� ������",
		"���ݼӵ� ������",
		"ũ��Ƽ�� �߻�Ȯ��",
		"����Ÿ�� �߻�Ȯ��",

		"����",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������",
		"�̵��ӵ� ������",
		"���ݼӵ� ������",
		"ũ��Ƽ�� �߻���",
		"����",

		"������ �ǰ� ������ ������",
		"������ �������ط�",
		"�Ϲݸ� ���޻��� ũ��Ƽ�� �ɷ�",
		"������ ���޻��� ũ��Ƽ�� �ɷ�",
		"�ڽ��� ���޻��� ���� ����",
		"�Ϲ� �� ��� �߰� ����ġ",
		"�� ��� ���ӸӴ� �����",
		"ĳ���� �������� ���� ����",
		"ĳ���� ������� ���� ����",
		"ĳ���� �����ۿ� ���� ����",
		"�ַ� �� HP ����",
		"��Ƽ �� HP ����",
		"�ַ� �� ���ݼӵ� ���",
		"��Ƽ �� ���ݼӵ� ���",
		"����",
	};

	std::string ITEMVAR[EMVAR_SIZE] =
	{
		"����",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������"
	};

	std::string ITEMVOL[EMVAR_SIZE] = // by ���
	{
		"����",
		"HP ������",
		"MP ������",
		"SP ������",
		"HP+MP+SP ������"
	};

	std::string ITEM_QUE_TYPE[QUESTION_SIZE] =
	{
		"none",
		"Speed Up",
		"Crazy",
		"Attack Up",
		"Exp Up",
		"Exp Get",
		"Lucky",
		"Bomb!!",
		"Mob Gen",

		"Speed Up Max",
		"Madness",
		"Attack Up Max",
		"Heal",
	};

	std::string ITEM_QUE_VAR1[QUESTION_SIZE] =
	{
		"����",
		"�̵��ӵ�",
		"�̵��ӵ�",
		"Damage",
		"����ġ",
		"����",
		"�߻���",
		"Damage",
		"MID",

		"�̵��ӵ�",
		"�̵��ӵ�",
		"Damage",

		"hp"
	};

	std::string ITEM_QUE_VAR2[QUESTION_SIZE] =
	{
		"����",
		"����",
		"���ݼӵ�",
		"����",
		"����",
		"����",
		"����",
		"����",
		"SID",

		"����",
		"���ݼӵ�",
		"����",
		"����"
	};

	float ITEMVAR_SCALE[EMVAR_SIZE] =
	{
		1.0f,
		100.0f,
		100.0f,
		100.0f,
		100.0f
	};

	float ITEMADDON_VAR_SCALE[EMADD_SIZE] =
	{
		1.0f
	};

	bool IsITEMVAR_SCALE ( EMITEM_VAR emITEM_VAR )
	{
		return ITEMVAR_SCALE[emITEM_VAR]==100.0f;
	};

	std::string GRINDING_TYPE[EMGRINDING_NSIZE] =
	{
		"����",

		"���ݷ�",
		"����",

		"����(ȭ)",
		"����(��)",
		"����(��)",
		"����(��)",
		"����(��)",
	};

	std::string GRINDING_LEVEL[EMGRINDER_SIZE] =
	{
		"����",
		"����",
		"�ֻ���",
	};

	std::string GRINDING_RESIST[EMGRINDING_RS_NSIZE] =
	{
		"ȭ��",
		"����",
		"����",
		"��",
		"��",
	};

    //cwBack 2012-01-20 �����ɼ��� ���� ���Ŀ� �İ��� �ɼ��� ������ �ʴ� ����
    std::string ITEM_RANDOM_OPT[RandomOption::RANDOM_OPTION_NSIZE] =
    {
        "����",
        "���ݷ�",
        "����",

        "���߷�",
        "ȸ����",

        "HP����",
        "MP����",
        "SP����",

        "HP������",
        "MP������",
        "SP������",
        "HP+MP+SP+������",

        "���ݷ� ����",
        "���� ����",

        "���� ����",
        "SP �Ҹ�",
        "����",

        "�̵��ӵ� ����",

        "���ݷ�(����)",
        "����(����)",

        "���߷�(����)",
        "ȸ����(����)",

        "��(����)",
        "ü��(����)",
        "����(����)",
        "��ø(����)",
        "�ٷ�(����)",

        "����ġ(����)",
        "���ġ(����)", 
        "����ġ(����)",

        "����HP������(����)",
        "����MP������(����)",
        "����SP������(����)", 
        "���� ų�� CP ������(����)",

		"1��� ��ų ���ӽð�",
		"2��� ��ų ���ӽð�",
		"3��� ��ų ���ӽð�",
		"4��� ��ų ���ӽð�",
		"5��� ��ų ���ӽð�",
		"6��� ��ų ���ӽð�",
		"7��� ��ų ���ӽð�",
		"8��� ��ų ���ӽð�",
		"9��� ��ų ���ӽð�",
		"10��� ��ų ���ӽð�",
		"11��� ��ų ���ӽð�",
		"12��� ��ų ���ӽð�",
		"13��� ��ų ���ӽð�",
		"14��� ��ų ���ӽð�",
		"15��� ��ų ���ӽð�",
		"16��� ��ų ���ӽð�",
		"17��� ��ų ���ӽð�",
		"18��� ��ų ���ӽð�",
		"19��� ��ų ���ӽð�",
		"20��� ��ų ���ӽð�",
		"21��� ��ų ���ӽð�",
		"22��� ��ų ���ӽð�",
		"23��� ��ų ���ӽð�",
		"24��� ��ų ���ӽð�",
		"25��� ��ų ���ӽð�",
		"1��� ��ų ������ ������",
		"2��� ��ų ������ ������",
		"3��� ��ų ������ ������",
		"4��� ��ų ������ ������",
		"5��� ��ų ������ ������",
		"6��� ��ų ������ ������",
		"7��� ��ų ������ ������",
		"8��� ��ų ������ ������",
		"9��� ��ų ������ ������",
		"10��� ��ų ������ ������",
		"11��� ��ų ������ ������",
		"12��� ��ų ������ ������",
		"13��� ��ų ������ ������",
		"14��� ��ų ������ ������",
		"15��� ��ų ������ ������",
		"16��� ��ų ������ ������",
		"17��� ��ų ������ ������",
		"18��� ��ų ������ ������",
		"19��� ��ų ������ ������",
		"20��� ��ų ������ ������",
		"21��� ��ų ������ ������",
		"22��� ��ų ������ ������",
		"23��� ��ų ������ ������",
		"24��� ��ų ������ ������",
		"25��� ��ų ������ ������",
		"1��� ��ų ������ ����",
		"2��� ��ų ������ ����",
		"3��� ��ų ������ ����",
		"4��� ��ų ������ ����",
		"5��� ��ų ������ ����",
		"6��� ��ų ������ ����",
		"7��� ��ų ������ ����",
		"8��� ��ų ������ ����",
		"9��� ��ų ������ ����",
		"10��� ��ų ������ ����",
		"11��� ��ų ������ ����",
		"12��� ��ų ������ ����",
		"13��� ��ų ������ ����",
		"14��� ��ų ������ ����",
		"15��� ��ų ������ ����",
		"16��� ��ų ������ ����",
		"17��� ��ų ������ ����",
		"18��� ��ų ������ ����",
		"19��� ��ų ������ ����",
		"20��� ��ų ������ ����",
		"21��� ��ų ������ ����",
		"22��� ��ų ������ ����",
		"23��� ��ų ������ ����",
		"24��� ��ų ������ ����",
		"25��� ��ų ������ ����",
		"ũ��Ƽ�� ������",
		"����Ÿ�� ������",
		"������ ���뷹�� ����",
		"óġ�� HP ȸ����",
		"óġ�� MP ȸ����",
		"óġ�� SP ȸ����",
		"óġ�� HP+MP+SP ȸ����",
		"��ü MP ������ŭ ���ݷ�����",
		"��ü SP ������ŭ ü������",
		"��ü ���� ������ŭ ����ġ ����",
		"��ü ���� ������ŭ ���ġ ����",
		"��ü ���� ������ŭ ���ġ ����",
    };

	float ITEM_RANDOM_OPT_SCALE[RandomOption::RANDOM_OPTION_NSIZE] =
	{
		1.0f,
		1.0f,	//	���ݷ�.
		1.0f,	//	����.

		1.0f,	//	������.
		1.0f,	//	ȸ����.

		1.0f,	//	hp ����.
		1.0f,	//	mp ����.
		1.0f,	//	sp ����.

		100.0f,	//	hp ������.
		100.0f,	//	mp ������.
		100.0f,	//	sp ������.
		100.0f,	//	hp, mp, sp ������.

		1.0f,	//	���ݷ� ����.
		1.0f,	//	���� ����.

		1.0f,	//	���� ����.
		1.0f,	//	sp �Ҹ�.
		1.0f,	//	����.

		1.0f,	// �̵��ӵ� ����

		1.0f,	//	���ݷ�(����).
		1.0f,	//	����(����).

		1.0f,	//	������(����).
		1.0f,	//	ȸ����(����).

		1.0f,	//	��(����).
		1.0f,	//	ü��(����).
		1.0f,	//	����(����).
		1.0f,	//	��ø(����).
		1.0f,	//	�ٷ�(����).

		1.0f,	//	����ġ(����).
		1.0f,	//	���ġ(����).
		1.0f,	//	����ġ(����).

		1.0f,	//	���� hp ������(����).
		1.0f,	//	���� mp ������(����).
		1.0f,	//	���� sp ������(����).
		1.0f,	//	���� ų�� CP ������(����).
	};

	DWORD	ITEMCOLOR[EMITEM_LEVEL_NSIZE] = 
	{
		0xffff0000,
		0xff00ff00,
		0xff0000ff,
		0xffffff00,
		0xffff00ff
	};


	std::string COOLTYPE[EMCOOL_SIZE] =
	{
		"������Ÿ��",
		"������MID/SID"
	};

	std::string BIKETYPE[EMBIKE_NSIZE] =
	{
		"BT-5",
		"BT-7",
		"BT-9",
		"EV-1"
	};

	std::string BOARDTYPE[ EMBIKE_NSIZE ] =
	{
		"ȣ������",
	};

	std::string CARTYPE[EMBIKE_NSIZE] = 
	{
		"Ferrari",
		"CAR_B",
		"CAR_C",
		"CAR_D",
	};

	std::string REARCARTYPE[EMBIKE_NSIZE] =
	{
		"REARCAR_A",
		"REARCAR_B",
		"REARCAR_C",
		"REARCAR_D",
	};

	std::string SKILL_LINK_TYPE[EMITEM_SKILL_SIZE] =
	{
		"��ü",
		"�ڽ�ĳ����",
		"��ĳ����",
		"��"
	};
};

namespace db
{
	State::State()
		: m_nState( DB_NONE )
	{
	}

	void State::setState( int emState, bool bForce /*= false*/ )
	{
		if ( true == bForce )
		{
			m_nState = emState;
			return;
		}

		switch ( m_nState )
		{
		case DB_NONE:
			if ( DB_INSERT == emState )
			{
				m_nState = DB_INSERT;
			}
			else if ( DB_UPDATE == emState )
			{
				m_nState = DB_UPDATE;
			}
			else if ( DB_NONE == emState )
			{
				m_nState = DB_NONE;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_INSERT:
			if ( DB_DELETE == emState )
			{
				m_nState = DB_NONE;
			}
			else if ( DB_INSERT == emState )
			{
				m_nState = DB_INSERT;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_UPDATE:
			if ( DB_DELETE == emState )
			{
				m_nState = DB_DELETE;
			}
			else if ( DB_UPDATE == emState )
			{
				m_nState = DB_UPDATE;
			}
			else
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		case DB_DELETE:
			{
				MIN_ASSERT( 0 && "STATE Change Fail" );
			}
			break;
		default:
			break;
		}
	}

};
