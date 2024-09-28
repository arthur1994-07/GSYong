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
	PIECE_CAPE		    = 10,	// DxSkinPiece::VERSION = 0x00000114; 때 추가됨.

    PIECE_SAVE_SIZE     = 11,	// 저장해야하는 사이즈 - chf 시 저장되는 것들

    PIECE_GRIDE_UP      = 11,   // 연마 상의 ( chf 에 저장 필요 없음 )					// cps 에는 저장이 된다. 관리 필요.
	PIECE_GRIDE_DOWN    = 12,   // 연마 하의 ( chf 에 저장 필요 없음 )					// cps 에는 저장이 된다. 관리 필요.
    PIECE_GRIDE_HAND    = 13,   // 연마 장갑 ( chf 에 저장 필요 없음 )					// cps 에는 저장이 된다. 관리 필요.
	PIECE_GRIDE_FOOT    = 14,   // 연마 신발 ( chf 에 저장 필요 없음 )					// cps 에는 저장이 된다. 관리 필요.
	PIECE_RHAND_HIDDEN	= 15,	// 스킬 사용 시 두번째 무기를 불러 오기 위한 슬롯		// cps 에는 저장이 된다. 관리 필요.
	PIECE_LHAND_HIDDEN	= 16,	// 스킬 사용 시 두번째 무기를 불러 오기 위한 슬롯		// cps 에는 저장이 된다. 관리 필요.
	PIECE_HAIR_4_HAT	= 17,	// 모자를 쓸 경우 추가되는 헤어							// cps 에는 저장이 된다. 관리 필요.

	PIECE_SIZE		    = 18,   // 저장 + 저장안되는것(게임에서 필요에 의해 사용되는 것)
    PIECE_NONE		    = 18,   // 정보없는 Cps

	// 밑에 값은 건들지 마세요.
	PIECE_SAVE_SIZE_OLD_SANIMCONINFO_0x0113 = 10,	// 이전에 사용했던 수치. 
	PIECE_SIZE_OLD							= 14,   // 이전에 사용되었단 Max Size ( PIECE_SAVE_SIZE 가 PIECE_SIZE_OLD 를 넘으면 필요없는 데이터 )
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
	EMPEACE_WEAPON_NONE_MINUS    	= -1,	// [shhan]p2017.03.22] 사용하지 않도록 한다. EMPEACE_WEAPON_NONE(17) 을 쓰도록 한다.
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
	EMPEACE_WEAPON_WHIP_SLOT		= 11,	// Skin 된 마술부용 채찍
	EMPEACE_WEAPON_BOX_SLOT			= 12,	// Skin 된 마술부용 마술상자
	EMPEACE_WEAPON_HAMMER_SLOT		= 13,
	EMPEACE_WEAPON_SHIELD_SLOT		= 14,
	EMPEACE_WEAPON_UMBRELLA_SLOT	= 15,	// Skin 된 연극부용 우산
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
	EMSCD_ZERO			= 0x00,		// 아무것도 셋팅 안됨.
	EMSCD_USERCOLOR		= 0x01,		// bUserColor - 유저들이 정한 컬러를 사용 Enable/Disable
	EMSCD_SET_BASE_CPS	= 0x02,		// chf 의 기본값 cps 를 입힌다.
	EMSCD_MESH_THREAD	= 0x04,		// 형상을 ThreadLoading 한다
	EMSCD_GAME_PLAYER	= 0x08,		// 게임을 하고있는 캐릭터 인가~?
	EMSCD_JIGGLE_BONE	= 0x10,		// 지글본 사용할 캐릭터인가~?
	EMSCD_ANI_THREAD	= 0x20,		// 애니메이션을 ThreadLoading 한다
	EMSCD_TEXTURE_THREAD= 0x40,		// Texture를 ThreadLoading 한다

	EMSCD_ALL_THREAD	= EMSCD_MESH_THREAD | EMSCD_ANI_THREAD | EMSCD_TEXTURE_THREAD,		// 모든 데이터를 ThreadLoading 한다
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