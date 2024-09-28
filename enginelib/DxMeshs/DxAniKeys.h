#ifndef DXANIKEYS_H_
#define DXANIKEYS_H_

#pragma once

#include "../../../RanThirdParty/DirectX/Include/d3dx9math.h"

//
//
//	Note : xFile ���ϸ��̼� �������� ����ü.
//
//
//
//
//

#define UNITANIKEY_PERSEC	(4800.0f)

// X File formation rotate key
struct SRotateKeyXFile
{
	DWORD			dwTime;
	DWORD			dwFloats;	
	float			w;
	float			x;
	float			y;
	float			z;
};

struct SScaleKeyXFile
{
	DWORD		dwTime; 
	DWORD		dwFloats;
	D3DXVECTOR3	vScale;
};

struct SPositionKeyXFile
{
	DWORD		dwTime;
	DWORD		dwFloats;	
	D3DXVECTOR3	vPos;	
};

struct SMatrixKeyXFile
{
	DWORD		dwTime;
	DWORD		dwFloats;	
	D3DXMATRIX	mat;	
};

// in memory versions
struct SRotateKey
{
	DWORD			dwTime;
	D3DXQUATERNION	quatRotate;	
};

struct SPositionKey
{
	DWORD		dwTime;
	D3DXVECTOR3	vPos;	
};

struct SScaleKey
{
	DWORD		dwTime;
	D3DXVECTOR3	vScale;	
};

struct SMatrixKey
{
	DWORD			dwTime;
	D3DXMATRIXA16	mat;

	SMatrixKey() :
		dwTime(0)
	{
	}

	~SMatrixKey()
	{
	}

	bool operator < ( const SMatrixKey &rvalue ) const
	{
		return dwTime < rvalue.dwTime;
	}
};

struct QUATCOMP
{
	DWORD dwFirst;	// x,y
	DWORD dwSecond;	// z,w

	QUATCOMP()	{}
	QUATCOMP(DWORD a, DWORD b)
        : dwFirst(a)
        , dwSecond(b)
	{
	}
};

struct SQuatPosKey
{
	DWORD			m_dwTime;
	D3DXVECTOR3		m_vScale;
	D3DXVECTOR3		m_vPos;
	QUATCOMP		m_vQuatComp;
//	D3DXQUATERNION	m_vQuat;
	

	SQuatPosKey() :
		m_dwTime(0L),
		m_vScale(0.f,0.f,0.f),
		m_vPos(0.f,0.f,0.f),
		m_vQuatComp(0,0)
//		m_vQuat(0.f,0.f,0.f,1.f)
	{
	}

	~SQuatPosKey()
	{
	}

	bool operator < ( const SQuatPosKey &rvalue ) const
	{
		return m_dwTime < rvalue.m_dwTime;
	}
};

struct SQuatPos
{
	D3DXVECTOR3		m_vScale;
	D3DXVECTOR3		m_vPos;
	D3DXQUATERNION	m_vQuat;

	SQuatPos() :
		m_vScale(1.f,1.f,1.f),
		m_vPos(0.f,0.f,0.f),
		m_vQuat(0.f,0.f,0.f,0.f)
	{
	}

	~SQuatPos()
	{
	}
};

enum EMANI_MAINTYPE
{
	// CHF ���� �ִ� Ÿ��
	AN_GUARD_N			= 0,	//	���.
	AN_PLACID			= 1,	//	�޽�.
	AN_WALK				= 2,	//	�ȱ�.
	AN_RUN				= 3,	//	�ٱ�.
	AN_ATTACK			= 4,	//	����.
	AN_SHOCK			= 5,	//	���.
	AN_DIE				= 6,	//	����.

	AN_CREATE			= 7,	//	����.
	AN_TALK				= 8,	//	��ȭ.

	AN_SKILL			= 9,	//	��ų.
	AN_GESTURE			= 10,	//	����.
	AN_SHOCK_MIX		= 11,	//	���,mix��.

	AN_GUARD_L			= 12,	//	���,SP����.
	AN_CONFT_WIN		= 13,	//	���,�¸�.
	AN_CONFT_LOSS		= 14,	//	���,�й�.

	AN_SPEC				= 15,	//	Ư��.

	AN_SKILL_B			= 16,	//	��ų.
	AN_SKILL_C			= 17,	//	��ų.
	AN_SKILL_D			= 18,	//	��ų.
	AN_SKILL_E			= 19,	//	��ų. - �ذ��ο� ���� ��ų ����. < EMANI_SUBTYPE �� 10���̴� �ͺ��� �̰� ���̴� ���� ȿ����.~! >

	AN_GATHERING		= 20,	//	ä��

	AN_BIKE_A			= 21,   //  ����ũ A
	AN_BIKE_B			= 22,   //  ����ũ B
	AN_BIKE_C			= 23,   //  ����ũ C
	AN_BIKE_D			= 24,	//	����ũ D

	AN_FLY              = 25,   //  ����
	AN_SKILL_F			= 26,	//	��ų

    AN_SKILL_G          = 27,   //	��ų
    AN_SKILL_H          = 28,   //	��ų

	AN_CAR_A			= 29,
	AN_CAR_B			= 30,
	AN_CAR_C			= 31,
	AN_CAR_D			= 32,
	AN_REARCAR_A		= 33,
	AN_REARCAR_B		= 34,
	AN_REARCAR_C		= 35,
	AN_REARCAR_D		= 36,

	AN_NONE				= 37,
	AN_TYPE_SIZE		= 37,

	/// �� �̻� 'CHF ���� �ִ� Ÿ��' �� �߰����� ����, �̹� �ʹ� ����;

	// ABF(ATTACH BONE) ���� �ִ� Ÿ��
	AN_ATTBONE_STAY			= 0,	//	��� - ���� �ִϸ��̼�.
	AN_ATTBONE_ATTACK_001	= 1,	//	ATTACK 001
	AN_ATTBONE_ATTACK_002	= 2,	//	ATTACK 002
	AN_ATTBONE_ATTACK_003	= 3,	//	ATTACK 003
	AN_ATTBONE_ATTACK_004	= 4,	//	ATTACK 004
	AN_ATTBONE_ATTACK_005	= 5,	//	ATTACK 005

	AN_ATTBONE_SANIM_001	= 6,	//	SANIM 001
	AN_ATTBONE_SANIM_002	= 7,	//	SANIM 002
	AN_ATTBONE_SANIM_003	= 8,	//	SANIM 003
	AN_ATTBONE_SANIM_004	= 9,	//	SANIM 004
	AN_ATTBONE_SANIM_005	= 10,	//	SANIM 005
	AN_ATTBONE_SANIM_006	= 11,	//	SANIM 006
	AN_ATTBONE_SANIM_007	= 12,	//	SANIM 007
	AN_ATTBONE_SANIM_008	= 13,	//	SANIM 008
	AN_ATTBONE_SANIM_009	= 14,	//	SANIM 009
	AN_ATTBONE_SANIM_010	= 15,	//	SANIM 010
	AN_ATTBONE_SANIM_011	= 16,	//	SANIM 011
	AN_ATTBONE_SANIM_012	= 17,	//	SANIM 012

	AN_ATTBONE_SIZE			= 18,

	// VCF(Vehicle) ���� �ִ� Ÿ��
	AN_VEHICLE_STAY			= 0,	//	��� - ���� �ִϸ��̼�.
	AN_VEHICLE_WALK			= 1,	//	WALK
	AN_VEHICLE_BOOSTER		= 2,	//	BOOSTER ON
	AN_VEHICLE_RUN			= 3,	//	RUN
	AN_VEHICLE_DOWN			= 4,	//	DOWN
	AN_VEHICLE_SHOCK		= 5,	//	SHOCK
	AN_VEHICLE_ATTACK		= 6,	//	ATTACK
	AN_VEHICLE_SKILL_A		= 7,	//	SKILL A
	AN_VEHICLE_SKILL_B		= 8,	//	SKILL B
	AN_VEHICLE_SKILL_C		= 9,	//	SKILL C
	AN_VEHICLE_SKILL_D		= 10,	//	SKILL D

	AN_VEHICLE_ANIM_001		= 11,	//	ANIM 001
	AN_VEHICLE_ANIM_002		= 12,	//	ANIM 002
	AN_VEHICLE_ANIM_003		= 13,	//	ANIM 003
	AN_VEHICLE_ANIM_004		= 14,	//	ANIM 004
	AN_VEHICLE_ANIM_005		= 15,	//	ANIM 005
	AN_VEHICLE_NONE			= 16,

	AN_VEHICLE_SIZE			= 17,

	
};

enum EMANI_SUBTYPE
{
	AN_SUB_NONE			= 0,	//	�Ǽ�.
	AN_SUB_ONEHSWORD	= 1,	//	�Ѽհ�.
	AN_SUB_TWOHSWORD	= 2,	//	��հ�.
	AN_SUB_EITHERSWORD	= 3,	//	�ְ�.
	AN_SUB_DAGGER		= 4,	//	�ܰ�.
	AN_SUB_SPEAR		= 5,	//	â.
	AN_SUB_BOW			= 6,	//	Ȱ.
	AN_SUB_THROW		= 7,	//	��ô.
	AN_SUB_MANA			= 8,	//	����.
	AN_SUB_BIGHSWORD	= 9,	//	��հ� ����.
	AN_SUB_STICK		= 10,	//	������.
	
	AN_SUB_ONESABER		= 11,	//	�Ѽյ�.
	AN_SUB_TWOSABER		= 12,	//	��յ�.
	AN_SUB_BIGSABER		= 13,	//	��յ� ����.
	AN_SUB_EITHERSABER	= 14,	//	�ֵ�.
	
	AN_SUB_GWON			= 15,	//	�ǹ�.

	AN_SUB_BROOM		= 16,	//	���ڷ�.
	AN_SUB_HOVERBOARD	= 17,	//	����.

	AN_SUB_PISTOL		= 18,	//	����.
	AN_SUB_RIFLE		= 19,	//	����

    AN_SUB_SIDE				= 20,   // ���� ��
    AN_SUB_DUALSPEAR		= 21,   // ��� ���Ǿ�
    AN_SUB_THROWING_KNIFE	= 22,   // ������

	AN_SUB_EXTREME_GLOVE	= 23,	// �ذ��۷���.

	AN_SUB_TRICK_STICK	= 24, // ���� ������;
	AN_SUB_TRICK_BOX	= 25, // ���� ����;
	AN_SUB_TRICK_WHIP	= 26, // ä��;

	AN_SUB_ACTOR_HAMMER		= 27, // ���� �ظ�
	AN_SUB_ACTOR_SHIELD		= 28, // ���� ����
	AN_SUB_ACTOR_UMBRELLA	= 29, // ���� ���

	AN_SUB_SIZE			= 30,

	AN_SUB_PULLIN		= 0,
	AN_SUB_KNOCKBACK	= 1,

	AN_SUB_00			= 0,
	AN_SUB_01			= 1,
	AN_SUB_02			= 2,
	AN_SUB_03			= 3,
	AN_SUB_04			= 4,
	AN_SUB_05			= 5,
	AN_SUB_06			= 6,
	AN_SUB_07			= 7,
	AN_SUB_08			= 8,
	AN_SUB_09			= 9,
	AN_SUB_10			= 10,

	AN_SUB_FLY_SIZE		= 10, // �ֺ���

	AN_SUB_11			= 11,
	AN_SUB_12			= 12,
	AN_SUB_13			= 13,
	AN_SUB_14			= 14,
	AN_SUB_15			= 15,
	AN_SUB_16			= 16,
	AN_SUB_17			= 17,
	AN_SUB_18			= 18,
	AN_SUB_19			= 19,

	AN_SUB_20			= 20,
	AN_SUB_21			= 21,
	AN_SUB_22			= 22,
	AN_SUB_23			= 23,
	AN_SUB_24			= 24,
	AN_SUB_25			= 25,
	AN_SUB_26			= 26,
	AN_SUB_27			= 27,
	AN_SUB_28			= 28,
	AN_SUB_29			= 29,
	AN_SUB_30			= 30,
	AN_SUB_31			= 31,
	AN_SUB_32			= 32,
	AN_SUB_33			= 33,
	AN_SUB_34			= 34,
	AN_SUB_35			= 35,
	AN_SUB_36			= 36,
	AN_SUB_37			= 37,
	AN_SUB_38			= 38,
	AN_SUB_39			= 39,

	//AN_SUB_00_SIZE		= 40,

	AN_SUB_40			= 40,
	AN_SUB_41			= 41,
	AN_SUB_42			= 42,
	AN_SUB_43			= 43,
	AN_SUB_44			= 44,
	AN_SUB_45			= 45,
	AN_SUB_46			= 46,
	AN_SUB_47			= 47,
	AN_SUB_48			= 48,
	AN_SUB_49			= 49,

	AN_SUB_50			= 50,
	AN_SUB_51			= 51,
	AN_SUB_52			= 52,
	AN_SUB_53			= 53,
	AN_SUB_54			= 54,
	AN_SUB_55			= 55,
	AN_SUB_56			= 56,
	AN_SUB_57			= 57,
	AN_SUB_58			= 58,
	AN_SUB_59			= 59,

	AN_SUB_60			= 60,
	AN_SUB_61			= 61,
	AN_SUB_62			= 62,
	AN_SUB_63			= 63,
	AN_SUB_64			= 64,
	AN_SUB_65			= 65,
	AN_SUB_66			= 66,
	AN_SUB_67			= 67,
	AN_SUB_68			= 68,
	AN_SUB_69			= 69,

	AN_SUB_70			= 70,
	AN_SUB_71			= 71,
	AN_SUB_72			= 72,
	AN_SUB_73			= 73,
	AN_SUB_74			= 74,
	AN_SUB_75			= 75,
	AN_SUB_76			= 76,
	AN_SUB_77			= 77,
	AN_SUB_78			= 78,
	AN_SUB_79			= 79,

	AN_SUB_80			= 80,
	AN_SUB_81			= 81,
	AN_SUB_82			= 82,
	AN_SUB_83			= 83,
	AN_SUB_84			= 84,
	AN_SUB_85			= 85,
	AN_SUB_86			= 86,
	AN_SUB_87			= 87,
	AN_SUB_88			= 88,
	AN_SUB_89			= 89,

	AN_SUB_90			= 90,
	AN_SUB_91			= 91,
	AN_SUB_92			= 92,
	AN_SUB_93			= 93,
	AN_SUB_94			= 94,
	AN_SUB_95			= 95,
	AN_SUB_96			= 96,
	AN_SUB_97			= 97,
	AN_SUB_98			= 98,
	AN_SUB_99			= 99,

	AN_SUB_00_SIZE		= 100,
	AN_SUB_CROW_SIZE	= 10,			//	CrowData ����

	/// �� �̻� 'CHF ���� �ִ� Ÿ��' �� �߰����� ����, �̹� �ʹ� ����;

	AN_SUB_DRIVER_SIT	= 0,			// ������ ž��
	AN_SUB_DRIVER_STOP	= 1,			// ������ ����
	AN_SUB_DRIVER_WALK	= 2,			// ������ �ȱ�
	AN_SUB_DRIVER_RUN	= 3,			// ������ �ٱ�
	AN_SUB_DRIVER_BOOST	= 4,			// ������ �ν���
	AN_SUB_DRIVER_DIE	= 5,			// ������ ����
	AN_SUB_PASSENGER_SIT	= 6,		// ������ ž��
	AN_SUB_PASSENGER_STOP	= 7,		// ������ ����
	AN_SUB_PASSENGER_WALK	= 8,		// ������ �ȱ�
	AN_SUB_PASSENGER_RUN	= 9,		// ������ �ٱ�
	AN_SUB_PASSENGER_BOOST	= 10,		// ������ �ν���
	AN_SUB_PASSENGER_DIE	= 11,		// ������ ����

};

#endif // DXANIKEYS_H_
