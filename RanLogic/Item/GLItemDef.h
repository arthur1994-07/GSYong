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
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	�˷�.
		ITEMATT_REV00		= 2,	//	REV00
		ITEMATT_DAGGER		= 3,	//	�ܰ˷�.
		ITEMATT_SPEAR		= 4,	//	â��.
		ITEMATT_BOW			= 5,	//	Ȱ��.
		ITEMATT_THROW		= 6,	//	��ô��.

		ITEMATT_NOCARE		= 7,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 7,	//	��������.
	};
}

namespace glold_103
{
	enum GLITEM_ATT_103
	{
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	��.
		ITEMATT_SABER		= 2,	//	��.
		ITEMATT_DAGGER		= 3,	//	�ܰ�.
		ITEMATT_SPEAR		= 4,	//	â.
		ITEMATT_STICK		= 5,	//	������.
		ITEMATT_BOW			= 6,	//	Ȱ.
		ITEMATT_THROW		= 7,	//	��ô.

		ITEMATT_NOCARE		= 8,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 8,	//	��������.
	};
}

namespace glold_104
{
	enum GLITEM_ATT_104
	{
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	��.
		ITEMATT_SABER		= 2,	//	��.
		ITEMATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

		ITEMATT_DAGGER		= 4,	//	�ܰ�.
		ITEMATT_SPEAR		= 5,	//	â.
		ITEMATT_STICK		= 6,	//	������.
		ITEMATT_GWON		= 7,	//	��.
		ITEMATT_BOW			= 8,	//	Ȱ.
		ITEMATT_THROW		= 9,	//	��ô.
		
		ITEMATT_NOCARE		= 10,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 10,	//	��������.
	};
}

namespace glold_105
{
	enum GLITEM_ATT_105
	{
		ITEMATT_NOTHING		= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	��.
		ITEMATT_SABER		= 2,	//	��.

		ITEMATT_DAGGER		= 3,	//	�ܰ�.
		ITEMATT_SPEAR		= 4,	//	â.
		ITEMATT_STICK		= 5,	//	������.
		ITEMATT_GWON		= 6,	//	��.
		ITEMATT_BOW			= 7,	//	Ȱ.
		ITEMATT_THROW		= 8,	//	��ô.

		ITEMATT_PISTOL		= 9,	//	����
		ITEMATT_RAILGUN		= 10,	//	����1
		ITEMATT_PORTALGUN	= 11,	//	����2

		ITEMATT_HAMMER      = 12,   // �ظ�.
		ITEMATT_DUALSPEAR   = 13,   // ��� ���Ǿ�.
		ITEMATT_SIDE        = 14,   // ���̵�.

		ITEMATT_NOCARE		= 15,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 15,	//	��������.
	};

    enum GLSKILL_ATT_105
    {
        SKILLATT_NOTHING	= 0,	//	���빫�����.

        SKILLATT_SWORD		= 1,	//	��.
        SKILLATT_SABER		= 2,	//	��.
        SKILLATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

        SKILLATT_DAGGER		= 4,	//	�ܰ�.
        SKILLATT_SPEAR		= 5,	//	â.
        SKILLATT_STICK		= 6,	//	������.
        SKILLATT_GWON		= 7,	//	��.
        SKILLATT_BOW		= 8,	//	Ȱ.
        SKILLATT_THROW		= 9,	//	��ô.

        SKILLATT_PISTOL		= 10,	//	����
        SKILLATT_RAILGUN	= 11,	//	����1
        SKILLATT_PORTALGUN	= 12,	//	����2

        SKILLATT_GUN		= 13,	//	�� ��ü
        SKILLATT_RIFLE		= 14,	//	������ü

        SKILLATT_NOCARE		= 15,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
        SKILLATT_NSIZE		= 15,	//	��������.

        SKILLATT_NEAR		= 7,	//	���� ����.
    };
}

namespace glold_106
{
	enum GLSKILL_ATT_106	// �ظ�, ����Ǿ�, ���̵� �߰�.
	{
		SKILLATT_NOTHING	= 0,	//	���빫�����.

		SKILLATT_SWORD		= 1,	//	��.
		SKILLATT_SABER		= 2,	//	��.
		SKILLATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

		SKILLATT_DAGGER		= 4,	//	�ܰ�.
		SKILLATT_SPEAR		= 5,	//	â.
		SKILLATT_STICK		= 6,	//	������.
		SKILLATT_GWON		= 7,	//	��.
		SKILLATT_BOW		= 8,	//	Ȱ.
		SKILLATT_THROW		= 9,	//	��ô.

		SKILLATT_PISTOL		= 10,	//	����
		SKILLATT_RAILGUN	= 11,	//	����1
		SKILLATT_PORTALGUN	= 12,	//	����2

		SKILLATT_GUN		= 13,	//	�� ��ü
		SKILLATT_RIFLE		= 14,	//	������ü

		SKILLATT_HAMMER     = 15,   //  �ظ�
		SKILLATT_DUALSPEAR  = 16,   //  ��� ���Ǿ�
		SKILLATT_SIDE       = 17,   //  ���̵�

		SKILLATT_NOCARE		= 18,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		SKILLATT_NSIZE		= 18,	//	��������.

		SKILLATT_NEAR		= 7,	//	���� ����.
	};
}

namespace glold_107
{
	enum GLSKILL_ATT_107	// �ذ� �۷���;
	{
		SKILLATT_NOTHING	= 0,	//	���빫�����.

		SKILLATT_SWORD		= 1,	//	��.
		SKILLATT_SABER		= 2,	//	��.
		SKILLATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

		SKILLATT_DAGGER		= 4,	//	�ܰ�.
		SKILLATT_SPEAR		= 5,	//	â.
		SKILLATT_STICK		= 6,	//	������.
		SKILLATT_GWON		= 7,	//	��.
		SKILLATT_BOW		= 8,	//	Ȱ.
		SKILLATT_THROW		= 9,	//	��ô.

		SKILLATT_PISTOL		= 10,	//	����
		SKILLATT_RAILGUN	= 11,	//	����1
		SKILLATT_PORTALGUN	= 12,	//	����2

		SKILLATT_GUN		= 13,	//	�� ��ü
		SKILLATT_RIFLE		= 14,	//	������ü

		SKILLATT_HAMMER     = 15,   //  �ظ�
		SKILLATT_DUALSPEAR  = 16,   //  ��� ���Ǿ�
		SKILLATT_SIDE       = 17,   //  ���̵�

		SKILLATT_EXTREME_GLOVE	= 18,	// �ذ� �۷���;

		SKILLATT_NOCARE		= 19,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		SKILLATT_NSIZE		= 19,	//	��������.

		SKILLATT_NEAR		= 7,	//	���� ����.
	};
}

namespace glold_108
{
	enum GLITEM_ATT_108
	{
		ITEMATT_NOTHING	= 0,	//	���빫�����.

		ITEMATT_SWORD		= 1,	//	��.
		ITEMATT_SABER		= 2,	//	��.

		ITEMATT_DAGGER	= 3,	//	�ܰ�.
		ITEMATT_SPEAR		= 4,	//	â.
		ITEMATT_STICK		= 5,	//	������.
		ITEMATT_GWON		= 6,	//	��.
		ITEMATT_BOW			= 7,	//	Ȱ.
		ITEMATT_THROW		= 8,	//	��ô.

		ITEMATT_PISTOL			= 9,	//	����
		ITEMATT_RAILGUN		= 10,	//	����1
		ITEMATT_PORTALGUN	= 11,	//	����2

		ITEMATT_HAMMER		= 12,   // �ظ�.
		ITEMATT_DUALSPEAR	= 13,   // ��� ���Ǿ�.
		ITEMATT_SIDE				= 14,   // ���̵�.

		ITEMATT_EXTREME_GLOVE	= 15,	// �ذ� �۷���.

		ITEMATT_TRICK_STICK	= 16, // ���� ������;
		ITEMATT_TRICK_BOX	= 17, // ���� ����;
		ITEMATT_TRICK_WHIP	= 18,	// ���� ä��;

		ITEMATT_NOCARE		= 19,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		ITEMATT_NSIZE		= 19,	//	��������.
	};

	enum GLSKILL_ATT_108
	{
		SKILLATT_NOTHING	= 0,	//	���빫�����.

		SKILLATT_SWORD		= 1,	//	��.
		SKILLATT_SABER		= 2,	//	��.
		SKILLATT_SWORDSABER	= 3,	//	��/��. "��/��" �� ��ų���� ��빫�� Ȯ�νÿ��� ����.

		SKILLATT_DAGGER		= 4,	//	�ܰ�.
		SKILLATT_SPEAR		= 5,	//	â.
		SKILLATT_STICK		= 6,	//	������.
		SKILLATT_GWON		= 7,	//	��.
		SKILLATT_BOW		= 8,	//	Ȱ.
		SKILLATT_THROW		= 9,	//	��ô.

		SKILLATT_PISTOL		= 10,	//	����
		SKILLATT_RAILGUN	= 11,	//	����1
		SKILLATT_PORTALGUN	= 12,	//	����2

		SKILLATT_GUN		= 13,	//	�� ��ü
		SKILLATT_RIFLE		= 14,	//	������ü

		SKILLATT_HAMMER     = 15,   //  �ظ�
		SKILLATT_DUALSPEAR  = 16,   //  ��� ���Ǿ�
		SKILLATT_SIDE       = 17,   //  ���̵�

		SKILLATT_EXTREME_GLOVE	= 18,	// �ذ� �۷���;

		SKILLATT_TRICK_STICK	= 19, // ���� ������;
		SKILLATT_TRICK_BOX		= 20, // ���� ����;
		SKILLATT_TRICK_WHIP		= 21, // ���� ä��;

		SKILLATT_NOCARE		= 22,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
		SKILLATT_NSIZE		= 22,	//	��������.

		SKILLATT_NEAR		= 7,	//	���� ����.
	};
}

enum EMITEMLEVEL
{
	EMITEM_LEVEL_NORMAL		= 0,	// �Ϲ�;
	EMITEM_LEVEL_HIGH		= 1,	// ���;
	EMITEM_LEVEL_RARE		= 2,	// ���;
	EMITEM_LEVEL_UNIQUE		= 3,	// ����;
	EMITEM_LEVEL_EPIC		= 4,	// ���;
	EMITEM_LEVEL_LEGENDARY	= 5,	// ����;
	
	EMITEM_LEVEL_NSIZE		= 6,
};

enum EMITEM_FLAG
{
	TRADE_SALE			= 0x001,	//	������ �Ǹ� ����.
	TRADE_EXCHANGE		= 0x002,	//	���ΰ� ��ȯ ����.
	TRADE_THROW			= 0x004,	//	�ʵ忡 ������ ����.

	TRADE_EVENT_SGL		= 0x008,	//	���� �̺�Ʈ �������� �Ѱ� �̻� �� �ݰ� �ϱ� ����.

	ITEM_DISGUISE		= 0x010,	//	�ڽ���.
	ITEM_TIMELMT		= 0x020,	//	��ȿ��.
	
	ITEM_CHANNEL_ALL	= 0x040, //	��� ü��.
	TRADE_GARBAGE		= 0x080, //	�ļ� ����

	ITEM_RESTRICT		= 0x100, //	��� ����. by luxes.

    ITEM_BOX_WRAPPABLE  = 0x200, // �������� ���� ���� ����;
	ITEM_SPLIT			= 0x400, // ������ ���� ���� ����;

	LIMIT_LOCKER		= 0x800, // â�� ������ ������ ����;

	TRADE_ALL		= TRADE_SALE|TRADE_EXCHANGE|TRADE_THROW,
};

enum EMITEM_HAND
{
	HAND_RIGHT		= 0x0001,	//	������ ��.
	HAND_LEFT		= 0x0002,	//	�޼� ��.
	HAND_BOTH		= 0x0003,	//	�޼�, ������ ��� ����.
};

//	Note : ĳ���� ���ϸ��̼� ����� ������ ����.
//		ĳ���� ���ϸ��̼� �����Ҷ� ���� �����ƿ� ���� ������ ��.
//
enum GLITEM_ATT
{
	ITEMATT_NOTHING	= 0,	//	���빫�����.

	ITEMATT_SWORD		= 1,	//	��.
	ITEMATT_SABER		= 2,	//	��.

	ITEMATT_DAGGER	= 3,	//	�ܰ�.
	ITEMATT_SPEAR		= 4,	//	â.
	ITEMATT_STICK		= 5,	//	������.
	ITEMATT_GWON		= 6,	//	��.
	ITEMATT_BOW			= 7,	//	Ȱ.
	ITEMATT_THROW		= 8,	//	��ô.
	
	ITEMATT_PISTOL			= 9,	//	����
	ITEMATT_RAILGUN		= 10,	//	����1
	ITEMATT_PORTALGUN	= 11,	//	����2

    ITEMATT_SIDE			= 12,   // ���� ��
    ITEMATT_DUALSPEAR		= 13,   // ��� ���Ǿ�
    ITEMATT_THROWING_KNIFE	= 14,   // ������

	ITEMATT_EXTREME_GLOVE	= 15,	// �ذ� �۷���.

	ITEMATT_TRICK_STICK	= 16, // ���� ������;
	ITEMATT_TRICK_BOX	= 17, // ���� ����;
	ITEMATT_TRICK_WHIP	= 18,	// ���� ä��;

	ITEMATT_ACTOR_SHIELD = 19, // ����
	ITEMATT_ACTOR_HAMMER = 20, // ��ġ
	ITEMATT_ACTOR_UMBRELLA = 21, // ���

	ITEMATT_WEAPON_STR = 22,		// ������;
	ITEMATT_WEAPON_DEX = 23,		// ��ø����;
	ITEMATT_WEAPON_SPI = 24,		// ���Ź���;

	ITEMATT_NOCARE		= 25,	//	Ÿ�� ��� ����.	( "GLSKILL" ���� Hand ���� ���� �Ӽ����� ����. )
	ITEMATT_NSIZE		= 25,	//	��������.
};

// �� ���� Ÿ�Ժ� ���ݰŸ�;
enum EMITEM_ATTACK_DISTANCE
{
	EMITEM_ATTACK_DISTANCE_NOTHING = 2,				// ���빫�����;
	EMITEM_ATTACK_DISTANCE_SWORD = 5,				// ��;
	EMITEM_ATTACK_DISTANCE_SABER = 5,				// ��;
	EMITEM_ATTACK_DISTANCE_DAGGER = 1,				// �ܰ�;
	EMITEM_ATTACK_DISTANCE_SPEAR = 7,				// â;
	EMITEM_ATTACK_DISTANCE_STICK = 5,				// ������;
	EMITEM_ATTACK_DISTANCE_GWON = 1,				// ��;
	EMITEM_ATTACK_DISTANCE_BOW = 75,				// Ȱ;
	EMITEM_ATTACK_DISTANCE_THROW = 30,				// ��ô;
	EMITEM_ATTACK_DISTANCE_PISTOL = 75,				// ��;
	EMITEM_ATTACK_DISTANCE_RAILGUN = 90,			// ���ϰ�;
	EMITEM_ATTACK_DISTANCE_PORTALGUN = 90,			// ��Ż��;
	EMITEM_ATTACK_DISTANCE_SIDE = 10,				// ������;
	EMITEM_ATTACK_DISTANCE_DUALSPEAR = 5,			// ��� ���Ǿ�;
	EMITEM_ATTACK_DISTANCE_THROWING_KNIFE = 20,		// ������;
	EMITEM_ATTACK_DISTANCE_EXTREME_GLOVE = 2,		// �ذ� �۷���;
	EMITEM_ATTACK_DISTANCE_TRICK_STICK = 70,		// ���� ������;
	EMITEM_ATTACK_DISTANCE_TRICK_BOX = 60,			// ���� ����;
	EMITEM_ATTACK_DISTANCE_TRICK_WHIP = 60,			// ���� ä��;
	EMITEM_ATTACK_DISTANCE_ACTOR_SHIELD = 12,		// ����;
	EMITEM_ATTACK_DISTANCE_ACTOR_HAMMER = 10,		// ��ġ;
	EMITEM_ATTACK_DISTANCE_ACTOR_UMBRELLA = 15,		// ���;
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
	ITEM_SZNAME		= 65,	//	item �̸� ���� ����. (NULL) ���� ���� ����.
	ITEM_SZCOMMENT	= 256,	//	item "��������" ���� ����. (NULL) ���� ���� ����.
	ITEM_SKILLS		= 4,	//	"��ų ����Ʈ ����ġ" �� ������ �ִ� ����.
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

	SUIT_FINGER			= 8, // ����;

	SUIT_PET_A			= 9,	// PetData
	SUIT_PET_B			= 10,
	SUIT_VEHICLE		= 11,	// Ż��
	
	SUIT_VEHICLE_SKIN		= 12,	// Ż�� �� ���� ��Ų
	SUIT_VEHICLE_PARTS_A	= 13,	// Ż�� �� ���� ��Ų
	SUIT_VEHICLE_PARTS_B	= 14,	// Ż�� �� ���� ����
	SUIT_VEHICLE_PARTS_C	= 15,
	SUIT_VEHICLE_PARTS_D	= 16,
	SUIT_VEHICLE_PARTS_E	= 17,
	SUIT_VEHICLE_PARTS_F	= 18,
	SUIT_VEHICLE_PARTS_SIZE = 6,
    
    SUIT_WAISTBAND	= 19,   //�㸮��
    SUIT_EARRING			= 20,   //�Ͱ���
    SUIT_ACCESSORIES	= 21,   //��ű�
	SUIT_DECORATION	= 22, // ���;

	SUIT_NSIZE			= 23,
};

enum ITEM_VALUE
{
	ITEM_VAlUE_SUIT_REMODELNUM,
	ITEM_VAlUE_NSIZE,
};
////	Note : ������ġ.
//enum EMSLOT
//{
//	SLOT_HEADGEAR	    = 0,	//	�Ӹ� ����
//	SLOT_UPPER		    = 1,	//	��ü
//	SLOT_LOWER		    = 2,	//	��ü
//	SLOT_HAND		    = 3,	//	��
//	SLOT_FOOT		    = 4,	//	��
//
//	SLOT_RHAND		    = 5,	//	������ ����
//	SLOT_LHAND		    = 6,	//	�޼� ����
//	
//	SLOT_NECK		    = 7,	//	�����
//	SLOT_WRIST		    = 8,	//	�ո�
//
//	SLOT_RFINGER	    = 9,	//	������ �հ���
//	SLOT_LFINGER	    = 10,	//	�޼� �հ���
//
//	SLOT_RHAND_S	    = 11,	//	������ ����, �ذ���
//	SLOT_LHAND_S	    = 12,	//	�޼� ����, �ذ��� 
// 
//	SLOT_VEHICLE	    = 13,	//  Ż�� 
//
//	SLOT_NSIZE		    = 11,   // �Ϲݺμ� ������ q( - Ż�� )
//	SLOT_NSIZE_2	    = 12,   // �Ϲݺμ� ������ ( + Ż�� )
//	SLOT_NSIZE_S	    = 13,	// �ذ��� ������ ( - Ż�� )
//	SLOT_NSIZE_S_2	    = 14,	// �ذ��� ������ ( + Ż�� )
//
//	SLOT_HOLD		    = 14,	//	�տ� ��� �ִ� ��.
//
//	SLOT_TSIZE		    = 15,
//};

enum EMSLOTCHECK_RESULT
{
	EMSLOTCHECK_OK = 0,					// ����;
	EMSLOTCHECK_FAILED_UNIQUE,			// ���� �������� �ϳ��� ���������ϴ�;
	EMSLOTCHECK_FAILED,					// ����;
};

//	Note : ������ġ.
enum EMSLOT
{
    SLOT_HEADGEAR	    = 0,//	�Ӹ� ����
    SLOT_UPPER		    = 1,	//	��ü
    SLOT_LOWER		    = 2,	//	��ü
    SLOT_HAND		    = 3,	//	��
    SLOT_FOOT		    = 4,	//	��

    SLOT_RHAND		    = 5,	//	������ ����
    SLOT_LHAND		    = 6,	//	�޼� ����

    SLOT_NECK		    = 7,	//	�����
    SLOT_WRIST		    = 8,	//	�ո�

    SLOT_RFINGER	    = 9,	//	������ �հ���
    SLOT_LFINGER	    = 10,	//	�޼� �հ���    

    SLOT_RHAND_S	    = 11,	//	������ ����, �ذ���
    SLOT_LHAND_S	    = 12,	//	�޼� ����, �ذ��� 

    SLOT_VEHICLE	    = 13,	//  Ż��     

    SLOT_WAISTBAND      = 14,   // �㸮��
    SLOT_DECORATION       = 15,   // ���;
    SLOT_EARRINGS       = 16,   // �Ͱ���
    SLOT_RACCESSORIES   = 17,   // ������ ��ű�
    SLOT_LACCESSORIES   = 18,   // ���� ��ű�

    SLOT_NSIZE		    = SLOT_RHAND_S,   // �Ϲݺμ� ������ q( - Ż�� )
    SLOT_NSIZE_2	    = SLOT_LHAND_S,   // �Ϲݺμ� ������ ( + Ż�� ) //������ �̸����⿡�� ���
    SLOT_NSIZE_S	    = SLOT_VEHICLE,	// �ذ��� ������ ( - Ż�� )     //��ȭ�� ��ȭ�� ����˻�
    SLOT_NSIZE_S_2	    = 19,	// �ذ��� ������ ( + Ż�� )    
	SLOT_NSIZE_S_3		= 6,    // ������ ��ų ��ũ ���ɽ��� ����

	//  [1/16/2013 gbgim]
	// - ���� Ȧ�� ���� ���� ������. 
	//   �������� �������� ��� ������ ����� ��Ŷ ���� �ϰ� Ȧ�� ���Կ� ������ �ִ� ���
	//   ����� Ŭ�󿡼� �� �������� ��ġ�� ������ �ְ� �̹����� ����ٴϰ� �Ǿ�����.
	//   �ҽ� ���� �ʿ���!
    SLOT_HOLD		    = SLOT_NSIZE_S_2,	//	�տ� ��� �ִ� ��.(���콺�� ����ִ� ������)    

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

//! ������ ����
enum EMITEM_TYPE
{
	ITEM_SUIT				= 0,
	ITEM_ARROW				= 1,	//! ȭ��.
	ITEM_CURE				= 2,	//! ��ǰ.
	ITEM_SKILL				= 3,	//! ��ų ���� ����.
	ITEM_RECALL				= 4,	//! ��ȯ��.
	ITEM_KEY				= 5,	//! ������ (����)
	ITEM_GRINDING			= 6,	//! ������
	ITEM_CHARM				= 7,	//! ����.
	ITEM_TICKET				= 8,	//! ������.
	ITEM_SKP_RESET			= 9,	//! ��ų ����Ʈ ����. (�̻��)

	ITEM_STAT_RESET			= 10,	//! ���� ����Ʈ ����. (�̻��)
	ITEM_SKP_STAT_RESET_A	= 11,	//! ��ų, ���� ����Ʈ �յ� ����. (�� ������ ��)
	ITEM_BOX				= 12,	//! ���� ����.
	ITEM_CLEANSER			= 13,	//! ��Ź��.
	ITEM_LOUDSPEAKER		= 14,	//! Ȯ����.
	ITEM_FIRECRACKER		= 15,	//! ����.
	ITEM_CHARACTER_CARD		= 16,	//! ĳ���� �߰� ī��.
	ITEM_INVEN_CARD			= 17,	//! �κ� Ȯ�� ī��.
	ITEM_STORAGE_CARD		= 18,	//! â�� Ȯ�� ī��.
	ITEM_STORAGE_CONNECT	= 19,	//! â�� ���� ī��.

	ITEM_PREMIUMSET			= 20,	//! �����̾� ��Ʈ.
	ITEM_PRIVATEMARKET		= 21,	//! ���λ��� ���� �Ǹ�.
	ITEM_RANDOMITEM			= 22,	//! ���� ������.
	ITEM_DISJUNCTION		= 23,	//! �ڽ��� �и�.
	ITEM_HAIR				= 24,	//! ��Ÿ�� ����.
	ITEM_FACE				= 25,	//! �� ����.
	ITEM_QITEM				= 26,	//! Question ������.
	ITEM_CD					= 27,	//! CD.
	ITEM_2FRIEND			= 28,	//! ģ������.
	ITEM_CLUBCALL			= 29,	//! Ŭ��ȣ��.

	ITEM_HAIRSHOP			= 30,	//! ���� �̿��.
	ITEM_RENAME				= 31,   //! �̸����� ī��.
	ITEM_HAIR_STYLE			= 32,	//! ��Ÿ��
	ITEM_HAIR_COLOR			= 33,   //! ����÷�
	ITEM_REVIVE				= 34,	//! ��ȥ��
	ITEM_PET_CARD			= 35,	//! �� ī��	// PetData
	ITEM_PET_FOOD			= 36,	//! �� ����
	ITEM_PET_RENAME			= 37,	//! �� �̸�����
	ITEM_PET_COLOR			= 38,	//! �� �÷�����
	ITEM_PET_STYLE			= 39,	//! �� ��Ÿ�Ϻ���

	ITEM_PET_SKILL			= 40,	//! �� ��ų
	ITEM_SMS				= 41,	//! SMS���� �߼�
	ITEM_PET_REVIVE			= 42,	//! �� ��Ȱī��
	ITEM_ANTI_DISAPPEAR 	= 43,	//! �ҹ���(�Ҹ����)
	ITEM_REMODEL			= 44,	//! ���� ���
	ITEM_VEHICLE			= 45,	//! Ż��
	ITEM_VEHICLE_OIL		= 46,	//! Ż�� �⸧
	ITEM_VIETNAM_ITEMGET	= 47,	//! ��Ʈ�� Ž�� ���� ������ ȹ��
	ITEM_VIETNAM_EXPGET		= 48,	//! ��Ʈ�� Ž�� ���� ����ġ ȹ��	
	ITEM_GENDER_CHANGE		= 49,	//! ���� ���� ī�� 

	ITEM_GARBAGE_CARD		= 50,	//! ������ ī��	
	ITEM_TELEPORT_CARD		= 51,	//! ��ȯ��	
	ITEM_PET_SKIN_PACK  	= 52,	//! �� ��Ų ��
	ITEM_FACE_STYLE			= 53,	//! �󱼽�Ÿ��
	ITEM_TAXI_CARD			= 54,	//! �ý�ī��
	ITEM_MATERIALS			= 55,	//! ��������
	ITEM_NPC_RECALL			= 56,	//! NPC��ȯ ī��
	ITEM_BULLET				= 57,	//! �Ѿ�
	ITEM_LUNCHBOX			= 58,	//! ���ö�
	ITEM_PET_DUALSKILL      = 59,	//! �� ��ų �ߺ���� ī��

	ITEM_INC_GRINDINGRATE   = 60,	//! ��ȭ�� (��ȭ Ȯ�� ����)
	ITEM_ANTI_RESET		    = 61,	//! ��ȭ�� (���� �ܰ� ��ȭ)
	ITEM_VEHICLE_COLOR		= 62,	//! Ż�� �÷� ����ī��
	ITEM_BIKE_BOOST_CARD	= 63,	//! ������� �ν�Ʈ ���ī��
	ITEM_SKP_STAT_RESET_B	= 64,	//! ��ų, ���� ����Ʈ ���� ����. (�� ������ ��)
	ITEM_COSTUM_COLOR_CH_CD	= 65,	//! �ڽ�Ƭ �÷� ���� ī��.
	ITEM_POSTBOX_CARD		= 66,	//! ������ ���� ī��
    ITEM_POINT_CARD_REFUND     = 67, //! ȯ�Ұ��� Point Card
    ITEM_POINT_CARD_NOT_REFUND = 68, //! ȯ�ҺҰ� Point Card
	ITEM_PRODUCT_BOOK		= 69,	//! ���ռ�
	
    ITEM_PRODUCT_RESET		= 70,	//! ���� �ʱ�ȭ
    ITEM_PMARKET_SEARCH_CARD = 71,  //! ���λ��� �˻� ī��
    ITEM_REPAIR_CARD		= 72,	//! �������� ����ī��
	ITEM_STAMP_CARD		= 73, //! ���� ī��;
	ITEM_STAGE_PASS			= 74,	//!�������� �н�
	ITEM_ENTER_NUM_CHARGE	= 75,	//!�δ� ����Ƚ�� ������
	ITEM_CARDGAME_CARD	= 76, //! ī����� ����ī��
    ITEM_CONSIGMENT_SALE_GAME_MONEY = 77,  //! ���ӸӴ� ������
    ITEM_CONSIGMENT_SALE_EXTEND_CARD = 78,  //! ��Ź�Ǹ� Ȯ�彽��
	ITEM_LOCK_BOX = 79,				//! ��� ����;
	ITEM_LOCK_BOX_KEY = 80,						//! ���� ���� ����;
    ITEM_BOX_WRAPPER = 81,	//! �ڽ� ������;
    ITEM_WRAPPED_BOX = 82,	//! ����� �ڽ�;
	ITEM_SCHOOL_CHANGE_0 = 83,	//! �п� ���� ī�� 
	ITEM_SCHOOL_CHANGE_1 = 84,	//! �п� ���� ī�� 
	ITEM_SCHOOL_CHANGE_2 = 85,	//! �п� ���� ī�� 
	ITEM_INITIALIZE_COUNTRY = 86,	//! ���� �ʱ�ȭ ������;
	ITEM_COSTUME_STATS   = 87,		//! �ڽ�Ƭ �ɷ�ġ �ο�
	ITEM_EFFECT_RELEASE_CURE = 88,  //! ȿ������ ��ǰ
	ITEM_COSTUME_EFFECT_RELEASE = 89, // !�ڽ�Ƭ �ɷ�ġ ����
	ITEM_EXP_CARD = 90,				//! ����ġ ī�� ( ���� �̺�Ʈ�� ���� ������ )
	ITEM_SELECTIVEFORM_BOX = 91,	//! ������ �ڽ�;
	ITEM_EXP_COMPERSSOR = 92,			//! ����ġ ����    
	ITEM_EXP_CAPSULE = 93,			//! ����ġ ����
    ITEM_TEMP_ENCHANT = 94,
	ITEM_PERIODEXTANSION = 95,		//! �Ⱓ����;

	ITEM_RANMOBILE_CONNECTION = 96, // ����� ������
	ITEM_RANMOBILE_JEWEL = 97,

	ITEM_CHANGE_ACTORS_DISGUISE_INFO = 98,		// ������ ���������;

	ITEM_CLUBRENAME = 99,	//! Ŭ���̸����� ī��.

	ITEM_CHANGE_ITEMSKILL_CARD = 100,		// ������ ��ų ���� ī��;
	ITEM_CHANGE_BASICSTATS_CARD = 101,		// ������ �⺻ �ɷ�ġ ���� ī��;
	ITEM_CHANGE_ADDSTATS_CARD = 102,		// ������ ���� �ɷ�ġ ���� ī��;
	ITEM_UNLOCK_CHARACTER_SLOT = 103,		// ĳ���� ���� �� ����;

	ITEM_NSIZE,						//! ��������.
	
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

// ȿ������ ��ǰ�� ��������Ʈ( ER. EFFECT_RELEASE )
enum EMITEM_DRUG_ER
{
	ITEM_DRUG_ER_STUN		 = 0, // ���� ����
	ITEM_DRUG_ER_HALLUCINATE = 1, // ȯ�� ����
	ITEM_DRUG_ER_NON_DRUG    = 2, // ��������� ����
	ITEM_DRUG_ER_SIZE,
	ITEM_DRUG_ER_MAX = 30		  // ��(��Ʈ�ڸ�) �ִ� DWORD 
};

enum EMITEM_DRUG
{
	ITEM_DRUG_NUNE			= 0,
	ITEM_DRUG_HP			= 1,	//	ü��
	ITEM_DRUG_MP			= 2,	//	���
	ITEM_DRUG_SP			= 3,	//	Ȱ��

	ITEM_DRUG_HP_MP			= 4,	//	ü��+���
	ITEM_DRUG_MP_SP			= 5,	//	ü��+Ȱ��
	ITEM_DRUG_HP_MP_SP		= 6,	//	ü��+���+Ȱ��
	
	ITEM_DRUG_CURE			= 7,	//	���� �̻� ġ��.

	ITEM_DRUG_CALL_SCHOOL	= 8,	//	�б���ȯ.
	ITEM_DRUG_CALL_REGEN	= 9,	//	���۱�ȯ.
	ITEM_DRUG_CALL_LASTCALL	= 10,	//	������ȯ.
	ITEM_DRUG_CALL_REVIVE	= 11,	//	��Ȱ���.

	ITEM_DRUG_HP_CURE		= 12,	// ü��+�̻�ġ��
	ITEM_DRUG_HP_MP_SP_CURE	= 13,	// ü��+���+Ȱ��+�̻�ġ��
	ITEM_DRUG_CALL_TELEPORT	= 14,	// Ư�������̵�

	ITEM_DRUG_CP			= 15,	// ���� ��ġ ȹ��

	ITEM_DRUG_STAGE_PASS	= 16,	// �������� �н�
	ITEM_DRUG_ENTER_NUM_CHARGE	= 17,	// �δ� ����Ƚ�� ������
	ITEM_DRUG_TENDENCY_RECOVERY = 18,	// ����ȸ��
	ITEM_DRUG_COSTUME_STATS = 19,
	ITEM_DRUG_EFFECT_RELEASE = 20, 
	ITEM_DRUG_COSTUME_EFF_RELEASE = 21,
	

	ITEM_DRUG_EXP_COMPERSSOR_TYPE1 = 22, // ����ġ ����
	ITEM_DRUG_EXP_COMPERSSOR_TYPE2 = 23, // ����ġ ����
	ITEM_DRUG_EXP_CAPSULE_TYPE1	= 24, // ����ġ ĸ��
	ITEM_DRUG_EXP_CAPSULE_TYPE2	= 25, // ����ġ ĸ��

    ITEM_DRUG_TEMP_ENCHANT = 26,	// �Ͻð�ȭ
	ITEM_DRUG_MACROTIME = 27,		// ��ũ�� �ð� ����;
	ITEM_DRUG_PERIODEXTENTION = 28, // �Ⱓ ����;
	
	ITEM_DRUG_SIZE,
};

// �������� ���̿� �߰��Ǵ����� ������Ѵ�;
enum EMITEM_ADDON
{
	EMADD_NOTINIT	= MAXWORD,
	EMADD_NONE		= 0,
	EMADD_HITRATE	= 1,	//	������
	EMADD_AVOIDRATE	= 2,	//	ȸ����.

	EMADD_DAMAGE	= 3,	//	���ݷ�.
	EMADD_DEFENSE	= 4,	//	����.

	EMADD_HP		= 5,	//	ü�� ����.
	EMADD_MP		= 6,	//	���� ����.
	EMADD_SP		= 7,	//	���׹̳� ����.

	EMADD_STATS_POW	= 8,	//	��.
	EMADD_STATS_STR	= 9,	//	ü��.
	EMADD_STATS_SPI	= 10,	//	����.
	EMADD_STATS_DEX	= 11,	//	��ø.
	EMADD_STATS_INT	= 12,	//	����.
	EMADD_STATS_STA	= 13,	//	�ٷ�.

	EMADD_PA		= 14,	//	����ġ.
	EMADD_SA		= 15,	//	���ġ.
	EMADD_MA		= 16,	//	����ġ.

	// ���⼭ ���� ������ EMITEM_VAR;
	// ���̹��� EMADDEX_���ϴµ� EMADD�ϳ��� ���⿡�� ���� ADDON�� ��ġ�� �ɼ��� ����;
	// ���� �߰��Ǵ� �������� EX�� �߰������ʾƵ� �ȴ�;
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
	EMADDEX_INCR_IGNORE_BOSSDAMAGE		= 35,		// ���������Լ� �ǰݵɽ� ���� ������ ����;
	EMADDEX_BOSSFIXEDDAMAGE				= 36,			// ���������� ����������;
	EMADDEX_INCR_CRITICAL_EMERGENCYMOB	= 37,	// ���� ������ ������ ũ��Ƽ�� �߻��� ����;
	EMADDEX_INCR_CRITICAL_EMERGENCYBOSS = 38,	// ���� ������ �������� ũ��Ƽ�� �߻��� ����;
	EMADDEX_DEC_DAMAGE_EMERGENCYSTATE	= 39, 	// ���� ������ ��� ������ ����;
	EMADDEX_INCR_EXPADDEFFECT			= 40,	// �Ϲ� �� ��� ����ġ ����;
	EMADDEX_INCR_DROPGENMONEY			= 41,	// �� ��ɽ� ����Ǵ� �� ������;
	EMADDEX_DECR_DAMAGE_MELEESKILL		= 42,	// ���� ���� ��ų �ǰݽ� ��� ������ ����;
	EMADDEX_DECR_DAMAGE_RANGESKILL		= 43,	// ��� ��ų �ǰݽ� ��� ������ ����;
	EMADDEX_DECR_DAMAGE_MAGICSKILL		= 44,	// ���� ��ų �ǰݽ� ��� ������ ����;
	EMADDEX_INC_HP_SOLOPLAY				= 45,		// �ַ��÷��̽� HP���� (��Ƽ�� �ƴҽ�);
	EMADDEX_INC_HP_PARTYPLAY			= 46,	// ��Ƽ�÷��̽� HP����;
	EMADDEX_INCR_ATKSPEED_SOLOPLAY		= 47,	// �ַ��÷��̽� ���� ���� (��Ƽ�� �ƴҽ�);
	EMADDEX_INCR_ATKSPEED_PARTYPLAY		= 48,	// ��Ƽ�÷��̽� ���� ����;

	// �̰ɷ� ���� ������ ������������ [10/24/2016 gbgim];
	// ��ȹ ��� ����� �ľ��� �ȉ����� �̴�� ���� ���ϸ� �ȵȴܴ�;
	// �ش� �ɼ��� ��ũ��Ʈ�� ���� �����Ǹ� SSUIT���ٴ� SITEMCUSTOM�� �������̴�;
	// �ش� ������ ���õ� �� �����ʹ� SVAR, SVOL, SADDON�� �ϳ��̴�;
	EMADD_RANDOM	= 49,	//	����;

	EMADD_SIZE		= 50,
};

namespace ITEM
{
	// SADDON, SVAR, SVOL�� SADDON �ϳ��� ��ģ��;
	struct SADDON
	{
		// ���� Ÿ��;
		EMITEM_ADDON	emTYPE;
		// ���ó�� ITEM::bAddonValueInteger �����ϸ��;
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
		true, // EMADD_HITRATE	= 1,	//	������
		true, // EMADD_AVOIDRATE	= 2,	//	ȸ����.
		true, // EMADD_DAMAGE	= 3,	//	���ݷ�.
		true, // EMADD_DEFENSE	= 4,	//	����.
		true, // EMADD_HP		= 5,	//	ü�� ����.
		true, // EMADD_MP		= 6,	//	���� ����.
		true, // EMADD_SP		= 7,	//	���׹̳� ����.
		true, // EMADD_STATS_POW	= 8,	//	��.
		true, // EMADD_STATS_STR	= 9,	//	ü��.
		true, // EMADD_STATS_SPI	= 10,	//	����.
		true, // EMADD_STATS_DEX	= 11,	//	��ø.
		true, // EMADD_STATS_INT	= 12,	//	����.
		true, // EMADD_STATS_STA	= 13,	//	�ٷ�.
		true, // EMADD_PA		= 14,	//	����ġ.
		true, // EMADD_SA		= 15,	//	���ġ.
		true, // EMADD_MA		= 16,	//	����ġ.
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
		false, // EMADDEX_INCR_IGNORE_BOSSDAMAGE			= 35,		// ���������Լ� �ǰݵɽ� ���� ������ ��
		true, // EMADDEX_BOSSFIXEDDAMAGE				= 36,			// ���������� ����������;
		false, // EMADDEX_INCR_CRITICAL_EMERGENCYMOB	= 37,	// ���� ������ ������ ũ��Ƽ�� �߻��� ����;
		false, // EMADDEX_INCR_CRITICAL_EMERGENCYBOSS = 38,	// ���� ������ �������� ũ��Ƽ�� �߻��� ����
		false, // EMADDEX_DEC_DAMAGE_EMERGENCYSTATE	= 39, 	// ���� ������ ��� ������ ����;
		false, // EMADDEX_INCR_EXPADDEFFECT			= 40,	// '����ġ ������ ȿ��'�� ������Ŵ;
		false, // EMADDEX_INCR_DROPGENMONEY			= 41,	// �� ��ɽ� ����Ǵ� �� ������;
		false, // EMADDEX_DECR_DAMAGE_MELEESKILL		= 42,	// ���� ���� ��ų �ǰݽ� ��� ������ ����;
		false, // EMADDEX_DECR_DAMAGE_RANGESKILL		= 43,	// ��� ��ų �ǰݽ� ��� ������ ����;
		false, // EMADDEX_DECR_DAMAGE_MAGICSKILL		= 44,	// ���� ��ų �ǰݽ� ��� ������ ����;
		true, // EMADDEX_INC_HP_SOLOPLAY				= 45,		// �ַ��÷��̽� HP���� (��Ƽ�� �ƴҽ�);
		true,// EMADDEX_INC_HP_PARTYPLAY			= 46,	// ��Ƽ�÷��̽� HP����;
		false,// EMADDEX_INCR_ATKSPEED_SOLOPLAY		= 47,	// �ַ��÷��̽� ���� ���� (��Ƽ�� �ƴҽ�);
		true,// EMADDEX_INCR_ATKSPEED_PARTYPLAY		= 48,	// ��Ƽ�÷��̽� ���� ����;
		// EMADD_RANDOM	= 49,	//	����;
	};

}; // namespace ITEM

// ���ô� �����̴�. ����ϸ�ȵȴ�;
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
	
	EMVAR_IGNORE_BOSSDAMAGE	= 9,		// ���������Լ� �ǰݵɽ� ���� ������ ����;
	EMVAR_BOSSFIXEDDAMAGE = 10,			// ���������� ����������;
	EMVAR_INCR_CRITICAL_EMERGENCYMOB = 11,	// ���� ������ ������ ũ��Ƽ�� �߻��� ����;
	EMVAR_INCR_CRITICAL_EMERGENCYBOSS = 12,	// ���� ������ �������� ũ��Ƽ�� �߻��� ����;
	EMVAR_DEC_DAMAGE_EMERGENCYSTATE = 13, 	// ���� ������ ��� ������ ����;
	EMVAR_INCR_EXPADDEFFECT = 14,	// '����ġ ������ ȿ��'�� ������Ŵ;
	EMVAR_INCR_DROPGENMONEY = 15,	// �� ��ɽ� ����Ǵ� �� ������;
	EMVAR_DECR_DAMAGE_MELEESKILL = 16,	// ���� ���� ��ų �ǰݽ� ��� ������ ����;
	EMVAR_DECR_DAMAGE_RANGESKILL = 17,	// ��� ��ų �ǰݽ� ��� ������ ����;
	EMVAR_DECR_DAMAGE_MAGICSKILL = 18,	// ���� ��ų �ǰݽ� ��� ������ ����;
	EMVAR_INC_HP_SOLOPLAY = 19,		// �ַ��÷��̽� HP���� (��Ƽ�� �ƴҽ�);
	EMVAR_INC_HP_PARTYPLAY = 20,	// ��Ƽ�÷��̽� HP����;
	EMVAR_INCR_ATKSPEED_SOLOPLAY = 21,	// �ַ��÷��̽� ���� ���� (��Ƽ�� �ƴҽ�);
	EMVAR_INCR_ATKSPEED_PARTYPLAY = 22,	// ��Ƽ�÷��̽� ���� ����;

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
// 	EMR_OPT_DAMAGE			= 1,	//	���ݷ�.
// 	EMR_OPT_DEFENSE			= 2,	//	����.
// 
// 	EMR_OPT_HITRATE			= 3,	//	������.
// 	EMR_OPT_AVOIDRATE		= 4,	//	ȸ����.
// 
// 	EMR_OPT_HP				= 5,	//	hp ����.
// 	EMR_OPT_MP				= 6,	//	mp ����.
// 	EMR_OPT_SP				= 7,	//	sp ����.
// 
// 	EMR_OPT_HP_INC			= 8,	//	hp ������.
// 	EMR_OPT_MP_INC			= 9,	//	mp ������.
// 	EMR_OPT_SP_INC			= 10,	//	sp ������.
// 	EMR_OPT_HMS_INC			= 11,	//	hp, mp, sp ������.
// 
// 	EMR_OPT_GRIND_DAMAGE	= 12,	//	���ݷ� ����.
// 	EMR_OPT_GRIND_DEFENSE	= 13,	//	���� ����.
// 
// 	EMR_OPT_RANGE			= 14,	//	���� ����.
// 	EMR_OPT_DIS_SP			= 15,	//	sp �Ҹ�.
// 	EMR_OPT_RESIST			= 16,	//	����.
// 
// 	EMR_OPT_MOVE_SPEED		= 17,	// �̵��ӵ� ����
// 
// 	EMR_OPT_DAMAGE_INT		= 18,	//	���ݷ�(����).
// 	EMR_OPT_DEFENSE_INT		= 19,	//	����(����).
// 
// 	EMR_OPT_HITRATE_INT		= 20,	//	������(����).
// 	EMR_OPT_AVOIDRATE_INT	= 21,	//	ȸ����(����).
// 
// 	EMR_OPT_STATS_POW_INT	= 22,	//	��(����).
// 	EMR_OPT_STATS_STR_INT	= 23,	//	ü��(����).
// 	EMR_OPT_STATS_SPI_INT	= 24,	//	����(����).
// 	EMR_OPT_STATS_DEX_INT	= 25,	//	��ø(����).
// 	EMR_OPT_STATS_STA_INT	= 26,	//	�ٷ�(����).
// 
// 	EMR_OPT_PA_INT				= 27,	//	����ġ(����).
// 	EMR_OPT_SA_INT				= 28,	//	���ġ(����).
// 	EMR_OPT_MA_INT				= 29,	//	����ġ(����).
// 
// 	EMR_OPT_HP_POTION_INC_INT	= 30,	//	���� hp ������(����).
// 	EMR_OPT_MP_POTION_INC_INT	= 31,	//	���� mp ������(����).
// 	EMR_OPT_SP_POTION_INC_INT	= 32,	//	���� sp ������(����).
// 	EMR_OPT_CP_INC_INT			= 33,	//	���� ų�� CP ������(����).
// 
// 	EMR_OPT_SIZE				= 34,
// };

enum EMCOOL_TYPE
{
	EMCOOL_ITEMID	= 0,	// MID/SID
	EMCOOL_ITEMTYPE	= 1,	// ������ Ÿ��
	EMCOOL_SIZE		= 2,	// ������
};

enum EMPARTS_TYPE
{
	EMBIKE_BT5	= 0,	// ����ũ ����
	EMBIKE_BT7	= 1,	
	EMBIKE_BT9	= 2,
	EMBIKE_EV1	= 3,
	EMBIKE_SIZE	= 4,	// ����ũ ���� ������
	EMBIKE_NSIZE = 10,	// �ִ� ������
	
	EMBOARD_HOVER = 0,	// ���� ����	
	EMHOVER_SIZE = 1,	// ���� ���� ������

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
	EMGRINDING_ATTRIBUTE_RANDOM			  = 0x0001, // ���� ���� ����
	EMGRINDING_ATTRIBUTE_NODEGRATION	  = 0x0002, // ��������� ���ҵ��� ����
	EMGRINDING_ATTRIBUTE_IGNOREPROTECTED  = 0x0004, // ������ȣ(��ȭ��,�ҹ��� ��)�� ����
	EMGRINDING_ATTRIBUTE_IGNORERATEINC	  = 0x0008, // Ȯ������(��ȭ��,�̺�Ʈ ��)�� ����
	EMGRINDING_ATTRIBUTE_IGNOREDEFAULTREQ = 0x0010, // �⺻ �䱸���� ���� (������ ��� üũ�� �����ϰų�, ������ �Ҹ𰳼��� ������ �Ѱ��� �����Ѵ�)
	EMGRINDING_ATTRIBUTE_ADDED			  = 0x0020, // ��� ������ġ�� ������ ���� (�ܵ����� ���� �ƹ�ȿ�� ���� RANDOM�� ���� ����ؾ���)
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
	EMGRINDINGOPTION_NONE			       = 0x0000, // �ɼ� ����
	EMGRINDINGOPTION_ANTIDISAPPEAR	       = 0x0001, // �ҹ��� ���� (�Ҹ� ����)
	EMGRINDINGOPTION_INC_GRINDINGRATE      = 0x0002, // ��ȭ�� ���� (��ȭ Ȯ�� ����)
	EMGRINDINGOPTION_ANTIRESET			   = 0x0004, // ��ȭ�� ��� (���� �ܰ� ����)
};

enum EMGRINDINGCONDITION
{
	EMGRINDINGCONDITION_FAILED		= 0, // �Ϲ� ����
	EMGRINDINGCONDITION_SUCCESS		= 1, // ����
	EMGRINDINGCONDITION_NOITEM		= 2, // �������� ����
	EMGRINDINGCONDITION_MAX			= 3, // ���� �ִ�ġ ����
	EMGRINDINGCONDITION_NOTBEST		= 4, // �������� �ֻ���� �ƴ�
	EMGRINDINGCONDITION_NOTHIGH		= 5, // �������� ����� �ƴ�
	EMGRINDINGCONDITION_NOTBESTITEM	= 6, // �ֻ�� �̻����� ������ �� ���� ������
	EMGRINDINGCONDITION_NOTNUM		= 7, // ������ ���� ����
	EMGRINDINGCONDITION_DEFCLASS	= 8, // �����۰� �������� Ÿ���� �ٸ�
	EMGRINDINGCONDITION_MIN			= 9,  // �ּ� �������� ��ġ �̸��� �����ۿ� ����� ���
	EMGRINDINGCONDITION_RANDOM_MAX	= 10, // ���� �������� ������ġ����, ���� �������� �ִ� ���� ��ġ�� ���ų� ���� ���,
	EMGRINDINGCONDITION_TIME_LIMIT  = 11, // �Ͻð�ȭ���� ��� - �Ⱓ���� ��� �Ұ�
	EMGRINDINGCONDITION_RE_ENCHANT  = 12, // �Ͻð�ȭ���� ��� - �簭ȭ
	EMGRINDINGCONDITION_TLGRIND_ENCHANT  = 13, // �Ͻð�ȭ���� ��� - �Ͻð�ȭ�� �ο��Ǿ� �ִ�.
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

	DWORD		dwID;		//	����� ������ ID
	DWORD		dwCoolID;	//	��Ÿ�� ID ( Ÿ�Կ� ���� �ٸ� ) 
	__time64_t	tUseTime;	//	���ð�
	__time64_t	tCoolTime;	//	��밡���� �ð�

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
    ACCETYPEA		= 0, //! �Ӹ�, ����
    ACCETYPEB		= 1, //! ��, �ٸ�, ����
    ACCETYPESIZE	= 2,
};

namespace ITEM
{
	struct SVAR
	{
		EMITEM_VAR		emTYPE;		//	���� Ÿ��.
		float			fVariate;	//	���� ��.

		SVAR ()
			: emTYPE(EMVAR_NONE)
			, fVariate(0.0f)
		{
		}
	};

	struct SVOL
	{
		EMITEM_VAR	emTYPE;		//	���� Ÿ��.
		float		fVolume;	//	���� ��.

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
	EMITEM_SUIT_CHANGE_COLOR	= 0x00000001, // ���� ����;
	EMITEM_SUIT_UNIQUE				= 0x00000002, // ���� ����(���� ���� ����);
};

namespace COMMENT
{
	extern std::string ITEMLEVEL[EMITEM_LEVEL_NSIZE];
	
    //! ������ Ÿ��
    extern std::string ITEMTYPE[ITEM_NSIZE];
    std::string ItemTypeName(EMITEM_TYPE Type);

	extern std::string ITEMSUIT[SUIT_NSIZE];
    std::string ItemSuitName(EMSUIT Type);

	//extern std::string ITEMSLOT[SLOT_NSIZE_S]; // ������.
	extern std::string ITEMATTACK[ITEMATT_NSIZE+1];

	extern std::string ITEMDRUG_ER[ITEM_DRUG_ER_SIZE];
	extern std::string ITEMDRUG[ITEM_DRUG_SIZE];
	extern std::string ITEMDRUG_VAR[ITEM_DRUG_SIZE];

	extern std::string ITEMPROHIBIT[PROHIBIT_ITEM_SIZE];

	extern std::string ITEMADDON[EMADD_SIZE];
	
	extern std::string ITEMVAR[EMVAR_SIZE];
	extern float ITEMADDON_VAR_SCALE[EMADD_SIZE];

	extern std::string ITEMVOL[EMVAR_SIZE]; // by ���
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


// �ڽ�Ƭ�� ����Ǵ� ������ �Ӽ� Ÿ��.
enum COSTUME_STATS {
	E_NONE			  = 0,
	// �Ϲ� �Ӽ�
	E_EXPMULTIPLE     = 1,	// ����ġ(-327.00% ~ +327.00%)ġ
	//E_HITRATE,				// ���߷���
	//E_AVOIDRATE,			// ȸ������
	//E_DAMAGE,				// ���ݷ�(Low, High ���� ���)��
	E_DEFENCE,				// ���·�
	//E_RESIST_FIRE,			// ����(��)��)
	//E_RESIST_ICE,			// ����(����)
	//E_RESIST_ELECTRIC,		// ����(����)
	//E_RESIST_POISON,		// ����(��)
	//E_RESIST_SPIRIT,		// ����(����)
	E_RESIST,
	E_EMPTY,

	E_GENERAL_MAX     = E_EMPTY,

	// ���� ȿ��			
	E_ADD_HITRATE	  = 5,	// ���߷�
	E_ADD_AVOIDRATE,		// ȸ����
	E_ADD_DAMAGE,			// ���ݷ�
	E_ADD_DEFENSE,			// ����
	E_ADD_HP,				// ü��(HP)
	//E_ADD_MANA,				// ����
	//E_ADD_STEMINA,			// ���׹̳�
	E_ADD_STATS_POW,		// Stats ��
	E_ADD_STATS_STA,		// Stats ü��
	E_ADD_STATS_SPI,		// Stats ����
	E_ADD_STATS_DEX,		// Stats ��ø
	//E_ADD_STATS_INT,		// Stats ����
	//E_ADD_STATS_STR,		// Stats �ٷ�
	E_ADD_ENERGY,			// ���ġ
	E_ADD_SHOOTING,			// ���ġ
	E_ADD_MELEE,			// ����ġ
	E_ADD_EMPTY1,
	E_ADD_EMPTY2,
	E_ADD_EMPTY3,

	E_ADDED_MAX		  = E_ADD_EMPTY3 - E_EMPTY,
	// ��ȭ��				
	E_R_HP_INC		  = 20,	// HP ������
	E_R_MP_INC,				// MP ������
	E_R_SP_INC,				// SP ������
	E_R_HMSP_INC,			// HP+MP+SP ������
	E_R_MOVESPEED,			// �̵��ӵ� ������
	E_R_ATTACKSPEED,		// ���ݼӵ� ������
	E_R_CRITICALHIT,		// ũ��Ƽ�� �߻�Ȯ��
	E_R_STRIKEHIT,			// ����Ÿ�� �߻�Ȯ��
	E_R_EMPTY,
	
	E_VARRATE_MAX		  = E_R_EMPTY - E_ADD_EMPTY3,
	
	// ��ȭ��				
	//E_A_HP_INC		  = 35,	// HP ������
	//E_A_MP_INC,				// MP ������
	//E_A_SP_INC,				// SP ������
	//E_A_HMSP_INC,			// HP+MP+SP ������
	//E_A_MOVESPEED,			// �̵��ӵ� ������
	//E_A_ATTACKSPEED,		// ���ݼӵ� ������
	//E_A_CRITICALHIT,		// ũ��Ƽ�� �߻���
};	

#define MAX_GEN		E_GENERAL_MAX - 1
#define MAX_ADDED   E_ADDED_MAX   - 3
#define MAX_VAR     E_VARRATE_MAX - 1


#define ID_STAT(id, result) if( E_EXPMULTIPLE <= id && id <= E_EMPTY ) { result = id - E_EXPMULTIPLE; } \
							else if ( E_ADD_HITRATE <= id && id <= E_ADD_EMPTY3 ) { result = id - E_ADD_HITRATE; } \
							else if ( E_R_HP_INC    <= id && id <= E_R_EMPTY ) { result = id - E_R_HP_INC; }
							

//! ������ �κ��丮 Ÿ��
//! db ���� ����ϹǷ� �����ϸ� �ȵ�
enum INVEN_TYPE
{
	INVEN_DELETED		= 0,	//! ������ ������(�ʵ忡 �����ų� �׷� ����̴�.)
	INVEN_PUTON			= 1,	//! ���������
	INVEN_INVEN			= 2,	//! �Ϲ� �κ��丮
	INVEN_QUEST_PROC	= 3,	//! �������� Quest Item
	INVEN_QUEST_END		= 4,	//! �Ϸ�� Quest Item
	INVEN_POST			= 5,	//! �������� �������� �ִ� ������
	INVEN_LOCKER		= 6,	//! ĳ���� ��Ŀ
	INVEN_CLUB_LOCKER	= 7,	//! Ŭ�� ��Ŀ
	INVEN_VEHICLE		= 8,	//! Vehicle
	INVEN_PET			= 9,	//! Pet
	INVEN_DELETED2		= 10,	//! ������ ������(������ ������ �Ȱų� �ð��� �ٵǾ� �����ǰų� ���� ������ ����̴�.)
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
