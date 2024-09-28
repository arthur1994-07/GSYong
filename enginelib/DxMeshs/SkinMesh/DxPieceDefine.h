#ifndef _DX_PIECE_DEFINE_H_
#define _DX_PIECE_DEFINE_H_

#pragma once

enum EMPICETYPE
{
	PIECETYPE_CHARACTER  = 0, // CPS
	PIECETYPE_ATTACHBONE = 1, // APS
	PIECETYPE_V_PARTS	 = 2, // VPS

	PIECETYPE_SIZE,
};

enum EMPIECECHAR
{
	PIECE_HEAD		    = 0,
	PIECE_UPBODY	    = 1,
	PIECE_LOBODY	    = 2,
	PIECE_GLOVE		    = 3,
	PIECE_RHAND		    = 4,
	PIECE_LHAND		    = 5,
	PIECE_FOOT		    = 6,
	PIECE_HAIR		    = 7,
	PIECE_HEADGEAR	    = 8,
	PIECE_VEHICLE	    = 9,
	PIECE_CAPE		    = 10,	// DxSkinPiece::VERSION = 0x00000114; �� �߰���.

    PIECE_SAVE_SIZE     = 11,	// �����ؾ��ϴ� ������ - chf �� ����Ǵ� �͵�

    PIECE_GRIDE_UP      = 11,   // ���� ���� ( chf �� ���� �ʿ� ���� )					// cps ���� ������ �ȴ�. ���� �ʿ�.
	PIECE_GRIDE_DOWN    = 12,   // ���� ���� ( chf �� ���� �ʿ� ���� )					// cps ���� ������ �ȴ�. ���� �ʿ�.
    PIECE_GRIDE_HAND    = 13,   // ���� �尩 ( chf �� ���� �ʿ� ���� )					// cps ���� ������ �ȴ�. ���� �ʿ�.
	PIECE_GRIDE_FOOT    = 14,   // ���� �Ź� ( chf �� ���� �ʿ� ���� )					// cps ���� ������ �ȴ�. ���� �ʿ�.
	PIECE_RHAND_HIDDEN	= 15,	// ��ų ��� �� �ι�° ���⸦ �ҷ� ���� ���� ����		// cps ���� ������ �ȴ�. ���� �ʿ�.
	PIECE_LHAND_HIDDEN	= 16,	// ��ų ��� �� �ι�° ���⸦ �ҷ� ���� ���� ����		// cps ���� ������ �ȴ�. ���� �ʿ�.
	PIECE_HAIR_4_HAT	= 17,	// ���ڸ� �� ��� �߰��Ǵ� ���							// cps ���� ������ �ȴ�. ���� �ʿ�.

	PIECE_SIZE		    = 18,   // ���� + ����ȵǴ°�(���ӿ��� �ʿ信 ���� ���Ǵ� ��)
    PIECE_NONE		    = 18,   // �������� Cps

	// �ؿ� ���� �ǵ��� ������.
	PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 = 10,	// ������ ����ߴ� ��ġ. 
	PIECE_SIZE_OLD							= 14,   // ������ ���Ǿ��� Max Size ( PIECE_SAVE_SIZE �� PIECE_SIZE_OLD �� ������ �ʿ���� ������ )
};

enum EMPIECEATTBONE
{
	ATTBONE_BODY	= 0,
	ATTBONE_PART0	= 1,
	ATTBONE_PART1	= 2,
	ATTBONE_PART2	= 3,
	ATTBONE_PART3	= 4,
	ATTBONE_PART4	= 5,
	ATTBONE_PART5	= 6,
	ATTBONE_PART6	= 7,
	ATTBONE_PART7	= 8,

	ATTBONE_SIZE	= 9,	
};

enum EMPIECEVEHICLE
{
	PART_V_BODY	= 0,
	PART_V_PART0	= 1,
	PART_V_PART1	= 2,
	PART_V_PART2	= 3,
	PART_V_PART3	= 4,
	PART_V_PART4	= 5,
	PART_V_PART5	= 6,
	PART_V_PART6	= 7,

	PART_V_SIZE	= 8,	
};

enum EMPEACEZONEWEAPON
{
	EMPEACE_WEAPON_NONE_MINUS    	= -1,	// [shhan]p2017.03.22] ������� �ʵ��� �Ѵ�. EMPEACE_WEAPON_NONE(17) �� ������ �Ѵ�.
	EMPEACE_WEAPON_RSLOT	    	= 0,
	EMPEACE_WEAPON_WHAL_SLOT    	= 1,
	EMPEACE_WEAPON_STICK_RSLOT  	= 2,
	EMPEACE_WEAPON_R_GUN_SLOT   	= 3,
	EMPEACE_WEAPON_L_GUN_SLOT   	= 4,
	EMPEACE_WEAPON_GUN_RSLOT	    = 5,
    EMPEACE_WEAPON_R_SPEAR_SLOT     = 6,
    EMPEACE_WEAPON_L_SPEAR_SLOT     = 7,
    EMPEACE_WEAPON_SIDE_SLOT        = 8,
    EMPEACE_WEAPON_KUNAI_SLOT       = 9,
	EMPEACE_WEAPON_USER_SLOT		= 10,
	EMPEACE_WEAPON_WHIP_SLOT		= 11,	// Skin �� �����ο� ä��
	EMPEACE_WEAPON_BOX_SLOT			= 12,	// Skin �� �����ο� ��������
	EMPEACE_WEAPON_HAMMER_SLOT		= 13,
	EMPEACE_WEAPON_SHIELD_SLOT		= 14,
	EMPEACE_WEAPON_UMBRELLA_SLOT	= 15,	// Skin �� ���غο� ���
	EMPEACE_WEAPON_MAGIC_STICK_SLOT	= 16,
	EMPEACE_WEAPON_NONE				= 17,	// NULL
	EMPEACE_WEAPON_SIZE		       	= 18,

};

enum EMATTBONEPARRENTOBJTYPE
{
	EMATTBONEPARRENTOBJTYPE_CHAR    = 0,
	EMATTBONEPARRENTOBJTYPE_VEHICLE = 1,

	EMATTBONEPARRENTOBJTYPE_SIZE    = 2,
};

enum EMFLAGS_SetCharData
{
	EMSCD_ZERO			= 0x00,		// �ƹ��͵� ���� �ȵ�.
	EMSCD_USERCOLOR		= 0x01,		// bUserColor - �������� ���� �÷��� ��� Enable/Disable
	EMSCD_SET_BASE_CPS	= 0x02,		// chf �� �⺻�� cps �� ������.
	EMSCD_MESH_THREAD	= 0x04,		// ������ ThreadLoading �Ѵ�
	EMSCD_GAME_PLAYER	= 0x08,		// ������ �ϰ��ִ� ĳ���� �ΰ�~?
	EMSCD_JIGGLE_BONE	= 0x10,		// ���ۺ� ����� ĳ�����ΰ�~?
	EMSCD_ANI_THREAD	= 0x20,		// �ִϸ��̼��� ThreadLoading �Ѵ�
	EMSCD_TEXTURE_THREAD= 0x40,		// Texture�� ThreadLoading �Ѵ�

	EMSCD_ALL_THREAD	= EMSCD_MESH_THREAD | EMSCD_ANI_THREAD | EMSCD_TEXTURE_THREAD,		// ��� �����͸� ThreadLoading �Ѵ�
};
typedef DWORD EMSCD_FLAGS;

namespace SKINTYPE_STING
{
	const TCHAR	m_szWHEREBACKTYPE[EMPEACE_WEAPON_SIZE][64] =
	{
		_T("slot_rhand"),
		_T("whal_slot"),
		_T("stick_rslot"),
		_T("r_gun_slot"),
		_T("l_gun_slot"),
		_T("gun_slot"),
        _T("r_spear_slot"),
        _T("l_spear_slot"),
        _T("side_slot"),
        _T("kunai_slot"),
		_T("user_slot"),
		_T("whip_slot"),
		_T("box_slot"),
		_T("hammer_slot"),
		_T("shield_slot"),
		_T("umbrella_slot"),
		_T("tc_stick_slot"),
		_T("NONE"),
	};

	const TCHAR	m_szSTART_SKIN_BONE[EMPEACE_WEAPON_SIZE][64] =
	{
		_T(""),	//_T("slot_rhand"),
		_T(""),	//_T("whal_slot"),
		_T(""),	//_T("stick_rslot"),
		_T(""),	//_T("r_gun_slot"),
		_T(""),	//_T("l_gun_slot"),
		_T(""),	//_T("gun_slot"),
		_T(""),	//_T("r_spear_slot"),
		_T(""),	//_T("l_spear_slot"),
		_T(""),	//_T("side_slot"),
		_T(""),	//_T("kunai_slot"),
		_T(""),	//_T("user_slot"),
		_T("whip_hand"),
		_T("magic_box"),
		_T(""),	//_T("hammer_slot"),
		_T(""),	//_T("shield_slot"),
		_T("umbrella_bone00"),
		_T(""),	//_T("tc_stick_slot"),
		_T(""),	//_T("NONE"),
	};

	const char m_szPIECETYPE[PIECETYPE_SIZE][64] = 
	{
		"Character Piece Setting",
		"Attach Bone Piece Setting",
		"Vehicle Parts Setting",
	};

	const char m_szATTPARENTOBJTYPE[EMATTBONEPARRENTOBJTYPE_SIZE][64] = 
	{
		"Character Parts",
		"Vehicle Parts",
	};
}

#endif // _DX_PIECE_DEFINE_H_