#ifndef _GLOGIC_DATA_H_
#define _GLOGIC_DATA_H_

#pragma once

#include "../SigmaCore/File/SFileSystem.h"
#include "../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"

#include "./Party/GLPartyDefine.h"
#include "./Character/GLCharData.h"	//<---
#include "./Inventory/GLInventory.h"
#include "./Pet/GLPet.h"
#include "./Vehicle/GLVEHICLE.h"
#include "./Summon/GLSUMMON.h"	//<---
#include "./GLCDMDefine.h"	//<---
#include "./Product/GLProductRecipe.h"

class DxSkinDataDummy;

enum GLCONST
{
	MAX_SERVERCHAR		= MAX_ONESERVERCHAR_NUM,
	MAX_SELECTCHAR		= 3, // ĳ���� ���� ȭ�鿡�� ���̴� �ִ� ĳ������ ��, ���� 3��
	MAX_VIEWRANGE		= 300, // �ִ� ���� (��Ƽ�� ���� �����Ѱ�?...)

	MAX_HEAD			= 64,
	MAX_HAIR			= 64,

	MAX_SCHOOL			= 10,
	MAX_LEVEL			= 310,

	MAX_CLUBSTORAGE		= 6,
	MAX_CLUBRANK		= 10,

	GRADE_NORMAL		= 4,
	GRADE_HIGH			= 9,
	GRADE_LIMIT_MAX		= 200, //Mars 20170622 �_����Ʒ��ߵȼ�

	MAX_SERIALIZED_CHAR_SIZE =	36864,	//36 * 1024
};

enum GLENALBE_NEW_CLASS
{
    RESTRICT_SCIENTIST_NEW_CLASS	= 0, // ���к�, �μ���, ������ ����.
    RESTRICT_ASSASSIN_NEW_CLASS	= 1, // �μ���, ������ ����.
    RESTRICT_TRICKER_NEW_CLASS	= 2, // ������ ����.
	RESTRICT_ACTOR_NEW_CLASS	= 3, // ���غ�
    NO_RESTRICT_NEW_CLASS		= 4, // ���� ����.
};

//	Note : ĳ���� ������ �⺻ ���.
//
struct GLCONST_CHARCLASS
{
	float				fWALKVELO;		//	ĳ���� �ȱ� �ӵ�.
	float				fRUNVELO;		//	ĳ���� �ٱ� �ӵ�.

	float				fHP_STR;		//	str(ü��) -> HP ȯ�� ����.
	float				fMP_SPI;		//	spi(����) -> MP ȯ�� ����.
	float				fSP_STA;		//	sta(�ٷ�) -> SP ȯ�� ����.
	float				fHIT_DEX;		//	dex(��ø) -> HIT rate ȯ�� ����.
	float				fAVOID_DEX;		//	dex(��ø) -> AVOID rate ȯ�� ����.
	float				fDEFENSE_DEX;	//	dex(��ø) -> DEFENSE ȯ�� ����.

	float				fPA_POW;		//	pow(��) -> PA(����ġ) ȯ�� ����.
	float				fPA_DEX;		//	dex(��ø) -> PA(����ġ) ȯ�� ����.

	float				fSA_POW;		//	pow(��) -> SA(���ġ) ȯ�� ����.
	float				fSA_DEX;		//	dex(��ø) -> SA(���ġ) ȯ�� ����.

	float				fMA_DEX;		//	dex(��ø) -> MA(����ġ) ȯ�� ����.
	float				fMA_SPI;		//	spi(����) -> MA(����ġ) ȯ�� ����.
	float				fMA_INT;		//	int(����) -> MA(����ġ) ȯ�� ����.

	float				fCONV_AP;		//	ȯ�� �⺻ ���ݷ�.
	float				fCONV_DP;		//	ȯ�� �⺻ ����.
	float				fCONV_PA;		//	ȯ�� �⺻ ����ġ.
	float				fCONV_SA;		//	ȯ�� �⺻ ���ġ.

	SCHARSTATS		sBEGIN_STATS;									// �ɷ�ġ �ʱ� ��ġ.
	FCHARSTATS			sLVLUP_STATS;									// ���� ���� �� �⺻ �ɷ�ġ ���� ��ġ.

	WORD				wBEGIN_AP;		//	�ʱ� ���ݷ�.
	WORD				wBEGIN_DP;		//	�ʱ� ����.
	WORD				wBEGIN_PA;		//	�ʱ� ����ġ.
	WORD				wBEGIN_SA;		//	�ʱ� ���ġ.

	float				fLVLUP_AP;		//	������ ���ݷ� ����.
	float				fLVLUP_DP;		//	������ ���� ����.
	float				fLVLUP_PA;		//	������ ����ġ ����.
	float				fLVLUP_SA;		//	������ ���ġ ����.

	//	���ϸ��̼� ����. ( �ð�/���� )
	//	server ������ �ʿ��� ����.
	VECANIATTACK		m_ANIMATION[AN_TYPE_SIZE][AN_SUB_00_SIZE];

	DWORD				dwHEADNUM;
	DWORD				dwHEADNUM_SELECT;
	std::string			strHEAD_CPS[MAX_HEAD];

	DWORD				dwHAIRNUM;
	DWORD				dwHAIRNUM_SELECT;
	std::string			strHAIR_CPS[MAX_HAIR];

	std::string			strCLASS_EFFECT;

	DxSkinDataDummy*	m_pSkinDataDummy;

	void LoadAniSet ( char* szSkinObj, bool bToolmode = false );
	void ClearAniSet ();

	BOOL LOADFILE ( const char* szFileName );

	GLCONST_CHARCLASS () :
		fWALKVELO(12),
		fRUNVELO(34),

		fHP_STR(0),
		fMP_SPI(0),
		fSP_STA(0),
		fHIT_DEX(0),
		fAVOID_DEX(0),
		fPA_POW(0),
		fSA_DEX(0),

		fMA_DEX(0),
		fMA_SPI(0),
		fMA_INT(0),

		fCONV_AP(0),
		fCONV_DP(0),
		fCONV_PA(0),
		fCONV_SA(0),

		wBEGIN_AP(0),
		wBEGIN_DP(0),
		wBEGIN_PA(0),
		wBEGIN_SA(0),

		fLVLUP_AP(0),
		fLVLUP_DP(0),
		fLVLUP_PA(0),
		fLVLUP_SA(0),

		dwHEADNUM(0),
		dwHEADNUM_SELECT(0),

		dwHAIRNUM(0),
		dwHAIRNUM_SELECT(0),
		m_pSkinDataDummy(NULL)
	{
	}

	GLCONST_CHARCLASS ( float _fHP_STR, float _fMP_SPI, float _fSP_STA, float _fHIT_DEX,
		float _fAVOID_DEX, float _fDEFENSE_DEX, float _fPA_POW, float _fSA_DEX,
		SCHARSTATS _fBEGIN_STATS, FCHARSTATS _fLVLUP_STATS,
		WORD _wBEGIN_AP, WORD _wBEGIN_DP, WORD _wBEGIN_PA, WORD _wBEGIN_SA,
		float _fLVLUP_AP, float _fLVLUP_DP, float _fLVLUP_PA, float _fLVLUP_SA ) 
		: fWALKVELO(12.0f)
		, fRUNVELO(34)
		
		, fHP_STR(_fHP_STR)
		, fMP_SPI(_fMP_SPI)
		, fSP_STA(_fSP_STA)
		, fHIT_DEX(_fHIT_DEX)
		, fAVOID_DEX(_fAVOID_DEX)
		, fDEFENSE_DEX(_fDEFENSE_DEX)
		, fPA_POW(_fPA_POW)
		, fSA_DEX(_fSA_DEX)
		
		, sBEGIN_STATS(_fBEGIN_STATS)
		, sLVLUP_STATS(_fLVLUP_STATS)
		, wBEGIN_AP(_wBEGIN_AP)
		, wBEGIN_DP(_wBEGIN_DP)
		, wBEGIN_PA(_wBEGIN_PA)
		, wBEGIN_SA(_wBEGIN_SA)
		
		, fLVLUP_AP(_fLVLUP_AP)
		, fLVLUP_DP(_fLVLUP_DP)
		, fLVLUP_PA(_fLVLUP_PA)
		, fLVLUP_SA(_fLVLUP_SA)
		, m_pSkinDataDummy(NULL)
	{
	}

	GLCONST_CHARCLASS::~GLCONST_CHARCLASS();

private:
	//	���� ������ ���Ͼ�� ��õ������ ��������.
	GLCONST_CHARCLASS& operator= ( GLCONST_CHARCLASS &Input )	{ GASSERT(0); };	
};

struct GLCLUBRANK
{
	DWORD	m_dwMasterLvl;
	DWORD	m_dwLivingPoint;
	DWORD	m_dwPay;
	DWORD	m_dwMember;

	GLCLUBRANK () :
		m_dwMasterLvl(0),
		m_dwLivingPoint(0),
		m_dwPay(0),
		m_dwMember(0)
	{
	}

	GLCLUBRANK ( DWORD dwMLvl, DWORD dwLP, DWORD dwPY, DWORD dwMem ) :
		m_dwMasterLvl(dwMLvl),
		m_dwLivingPoint(dwLP),
		m_dwPay(dwPY),
		m_dwMember(dwMem)
	{
	}
};

struct SPLAYERKILL
{
	DWORD		dwLEVEL;
	DWORD		dwNAME_COLOR;
	int			nPKPOINT;
	std::string	strNAME;
	float		fPK_EXP_RATE;

	DWORD		dwITEM_DROP_NUM;
	float		fITEM_DROP_RATE;

	float		fSHOP_2BUY_RATE;
	float		fSHOP_2SALE_RATE;

	SPLAYERKILL () :
		dwLEVEL(0),
		dwNAME_COLOR(0),
		nPKPOINT(0),
		fPK_EXP_RATE(0),

		dwITEM_DROP_NUM(0),
		fITEM_DROP_RATE(0),

		fSHOP_2BUY_RATE(0),
		fSHOP_2SALE_RATE(0)
	{
	}

	SPLAYERKILL ( DWORD _dwLEVEL, DWORD _dwNAME_COLOR, int _nPKPOINT, std::string _strNAME, float _fPK_EXP_RATE,
		DWORD _dwITEM_DROP_NUM, float _fITEM_DROP_RATE, float _fSHOP_2BUY_RATE, float _fSHOP_2SALE_RATE ) :
		dwLEVEL(_dwLEVEL),
		dwNAME_COLOR(_dwNAME_COLOR),
		nPKPOINT(_nPKPOINT),
		strNAME(_strNAME),
		fPK_EXP_RATE(_fPK_EXP_RATE),

		dwITEM_DROP_NUM(_dwITEM_DROP_NUM),
		fITEM_DROP_RATE(_fITEM_DROP_RATE),

		fSHOP_2BUY_RATE(_fSHOP_2BUY_RATE),
		fSHOP_2SALE_RATE(_fSHOP_2SALE_RATE)
	{
	}

	SPLAYERKILL ( const SPLAYERKILL &value )
	{
		operator= ( value );
	}

	SPLAYERKILL& operator= ( const SPLAYERKILL &value )
	{
		dwLEVEL = value.dwLEVEL;
		dwNAME_COLOR = value.dwNAME_COLOR;
		nPKPOINT = value.nPKPOINT;
		strNAME = value.strNAME;
		fPK_EXP_RATE = value.fPK_EXP_RATE;

		dwITEM_DROP_NUM = value.dwITEM_DROP_NUM;
		fITEM_DROP_RATE = value.fITEM_DROP_RATE;

		fSHOP_2BUY_RATE = value.fSHOP_2BUY_RATE;
		fSHOP_2SALE_RATE = value.fSHOP_2SALE_RATE;

		return *this;
	}
};

struct SSTATPOINT_BYLEVEL
{
	DWORD dwLevel;
	DWORD dwStatPoint;

	SSTATPOINT_BYLEVEL ( )
		: dwLevel ( 0 )
		, dwStatPoint ( 0 )
	{

	}

	SSTATPOINT_BYLEVEL ( DWORD _dwLevel, DWORD _dwStatPoint )
		: dwLevel ( _dwLevel )
		, dwStatPoint ( _dwStatPoint )
	{

	}
};

//	Note : ĳ���� ���� �⺻ ���.
//
namespace GLCONST_CHAR
{
	enum
	{
		DIE_DECEXP_NUM	= 31,
		SCREEN_EFFECT_NUM = 3,
		CAMERA_EFFECT_NUM = 5,
		EMITEMLEVEL_NUM = 6,
	};

	extern BOOL			bTESTSERVER;			// �׽�Ʈ �������� ����.
	extern INT			nUI_KEYBOARD;			// �������̽� Ű���� ����
	extern INT			nUI_DEFAULTKEYTYPE;		// �⺻ ŰŸ��
	extern BOOL			bBATTLEROYAL;			// ��Ʋ�ξ�
	extern INT			nMAX_FRIEND_NUMBER;		// �ִ� ģ��, ���� ��� ����
    extern INT          nMAX_FRIEND_LIST;       // �߰��� �� �ִ� ģ�� ��� �� //2012-07-31 by cwBack
    extern INT          nMAX_FRIEND_GROUP;      // ������ �� �ִ� �׷� �� //2012-09-05 bt cwBack
    extern INT          nMAX_FRIEND_BLOCK;      // ������ �� �ִ� ���� ��� �� //2012-07-31 by cwBack
	extern WORD			wLEVEL_FOR_EXTREME;		// �ذ��θ� �����ϱ� ���� �����Ǿ�� �ϴ� ����(lv.190)
	extern WORD			wSTART_LEVEL_FOR_EXTREME; // �ذ��� ���� ����
	extern WORD			wLEVEL_FOR_ACTOR;		// ������ ���� ���� ����;
    extern BYTE         ExcessExpProcessType;   // �ʰ� ����ġ ó�� Ÿ��. 0 : ������ 1 : �ذ��θ� 2 : ���

	//	Note : �Ϲ� ���.
	//
	extern float		fFIELD_NEXTPICK;		//	�㿡 �ִ� ������(��) ������ ó�� �޽��� ������ ���� ���� �ð�.
	extern WORD			wSERVER_NORMAL;
	extern WORD			wSERVER_CONGEST;
	extern WORD			wSERVER_NOVACANCY;

	//---------------------------------------------------------------------------------------[�ɸ��� �⺻]
	extern WORD			wSCHOOLNUM;
	extern std::string	strSCHOOLNAME[MAX_SCHOOL];
	extern SNATIVEID	nidSTARTMAP[MAX_SCHOOL];
	extern DWORD		dwSTARTGATE[MAX_SCHOOL];
	extern float		fCHAR_HEIGHT[GLCI_NUM_ACTOR];

    extern WORD         wENABLE_NEW_CLASS;  // �ű� Ŭ���� ���� ������ ���� ���� ��.

	//---------------------------------------------------------------------------------------[]
	extern BOOL			bPARTY_2OTHERSCHOOL;
	extern BOOL			bCLUB_2OTHERSCHOOL;

    //---------------------------------------------------------------------------------------[ä�� �������]
    extern float fCHAT_OVERINPUT_TIMER; // ���� ���� ��� �� ���� �Է� �ð��� ���� ��(��).
    extern WORD wCHAT_OVERINPUT_CANCEL_TIMER; // ���� ���� ��� ���ӽð�(��).
    extern int nCHAT_OVERINPUT_COUNT; // ���� ���� ��� �� ���� �Է� ���ڿ� ���� �Է� ��� Ƚ��.

    //---------------------------------------------------------------------------------------[��Ȱ â ��õ ������ ���]
    extern SNATIVEID   nidREBIRTH_RECOMMEND_ITEM;    // ��Ȱ â���� ��õ�Ǵ� ������ �̹����� ID.
    extern std::string strREBIRTH_RECOMMEND_ITEM;    // ��Ȱ â���� ��õ�Ǿ� ����Ʈ �������� �˻��� �������� �̸�.
    extern WORD wAUTO_REBIRTH_TIMER; // ��Ȱ â���� �ڵ� ��Ȱ �ð�.

    //---------------------------------------------------------------------------------------[NPC ��ȭ â ��õ ������ ���]
    extern SNATIVEID nidDIALOGUE_RECOMMEND_ITEM;    // NPC ��ȭ â���� ��õ�Ǵ� ������ �̹����� ID.
    extern std::string strDIALOGUE_RECOMMEND_ITEM;    // NPC ��ȭ â���� ��õ�Ǿ� ����Ʈ �������� �˻��� �������� �̸�.

    //---------------------------------------------------------------------------------------[�ڸ����]
    extern float fBECOME_AWAY_TIME; // �Է��� ���� �� �ڸ���� ���� �ɸ��� �ð�(��).
    extern int nAWAY_GESTURE_NUMBER;    // �ڸ���� �ÿ� ĳ���Ͱ� ���� ������ ��ȣ.

    //---------------------------------------------------------------------------------------[����ũ ž�½� ������ϴ� ȿ����]
    extern WORD wNON_APPLY_QBOX_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_QBOX_ON_VEHICLE[QUESTION_SIZE]; // Ż �� ž�½ÿ� ���� �� �� QBOX.
    extern WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE[SKILL::FOR_TYPE_SIZE]; // Ż �� ž�½ÿ� ���� �� �� ��ų Ÿ�Ե�.
    extern WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[EIMPACTA_SIZE]; // Ż �� ž�½ÿ� ���� �� �� ��ų ȿ����.
    extern WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE[EMSPECA_NSIZE]; // Ż �� ž�½ÿ� ���� �� �� ��ų Ư��ȿ����.

    extern WORD wMaxRebuyList;
    extern float fRebuyMultiplePrice;
    extern WORD wMinRebuyPrice;

	//	extern float		fDISPRICE;				//	������ ���Ǹ� ����.
	extern WORD			wMAXITEM_AGE;			//	ITEM, �ִ� ���ӽð�.
	extern WORD			wMAXITEM_HOLD;			//	ITEM, �ӽ� ������ ��ȣ �ð�.
	extern WORD			wMAXMOB_TRACKING;		//	�ִ� �� ���� ��.

	extern WORD			wMAX_LEVEL;				//	���� �ִ� ����.
	extern WORD			wMAX_EXTREME_LEVEL;		//	�ذ��� ���� �ִ� ����.
	extern WORD			wLVL_STATS_P;			//	������ ���� ����Ʈ �ο���.
	extern WORD			wLVL_2ndSTATS_P;			//	������ ���� ����Ʈ �ο���.
	extern float		fLVL_EXP_G;				//	���� ����ġ ���⿡ �ʿ��� ȯ�� ����.
	extern float		fLVL_EXP_S;				//	���� ����ġ ���⿡ �ʿ��� ȯ�� ����.
	extern float		fKILL_EXP_RATE;			//	"��ȹ�� ���� ����ġ" * ���϶� ����ġ.


	extern float		fDIE_DECEXP[DIE_DECEXP_NUM];	//	����� ����ġ ���� ����.

	extern float		fDIE_RECOVERYEXP[DIE_DECEXP_NUM];	// ����ġ ���� ����
	extern float		fEXP_RATE_MONEY[DIE_DECEXP_NUM];	// ����ġ ���� ���

	extern float		fREC_EXP_RATE;			//	������ ȸ������ �ٶ� ȹ���ϴ� ����ġ�� ����.

	//---------------------------------------------------------------------------------------[����]

	//extern WORD			wGRADE_MAX;				// ���� �Ҽ� �ִ� ��� ����ġ 
	//extern WORD			wGRADE_MAX_REGI;		// ���� �Ҽ� �ִ� ��� ����ġ(����) 

	//extern WORD			wDAMAGE_GRADE;			//	�� ��޴� ���� ���ݷ� ��ġ.
	//extern WORD			wDEFENSE_GRADE;			//	�� ��޴� ���� ���� ��ġ.

	//extern float		fDAMAGE_GRADE;			//	�� ��޴� ������ ���ݷ� ��ġ.
	//extern float		fDEFENSE_GRADE;			//	�� ��޴� ������ ���� ��ġ.

	//extern float		fDAMAGE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];		// �ֻ��� ��� ���ݷ� ������
	//extern float		fDEFENSE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];	// �ֻ��� ��� ���� ������
	//extern	WORD		wUSE_GRADE_NUM[GRADE_LIMIT_MAX-GRADE_HIGH];			// �ֻ��� ��� �ʿ��� ������ ����

	//extern WORD			wRESIST_FIRE_GRADE;		//	�� ��޴� ���� ����(ȭ) ��ġ.
	//extern WORD			wRESIST_ICE_GRADE;		//	�� ��޴� ���� ����(��) ��ġ.
	//extern WORD			wRESIST_ELEC_GRADE;		//	�� ��޴� ���� ����(��) ��ġ.
	//extern WORD			wRESIST_POISON_GRADE;	//	�� ��޴� ���� ����(��) ��ġ.
	//extern WORD			wRESIST_SPIRIT_GRADE;	//	�� ��޴� ���� ����(��) ��ġ.

	//---------------------------------------------------------------------------------------
	extern WORD			wLVL_SKILL_P;			//	������ ��ų ����Ʈ �ο���.
	extern WORD			wLVL_2ndSKILL_P;			//	������ ��ų ����Ʈ �ο���.

	extern WORD			wMAXATRANGE_SHORT;		//	���� ���ݰŸ�.
	extern float		fPUSHPULL_VELO;			//	�аų� ��涧 �̵� �ӵ�.

	extern int		nHitRateMinimum;			// ���� ���߷�;
	extern int		nHitRateMaximum;		    // �ִ� ���߷�;
	extern float	fHitRatePercentUnit;		// ���߷��� ��з��� ��ȯ�� �����ų ����;
	extern float	fAvoidRatePercentUnit;		// ȸ�Ƿ��� ��з��� ��ȯ�� �����ų ����;
	extern float	fAttackSuccesRate_LvScale;	// ���ݼ����� ���Ŀ��� ���Ǵ� ���� ���� �����;
    extern float		fMOB_TRACING;			//	�� ���� �Ÿ�.
    extern float		fNPC_INTERACTION_RANGE;  //  NPC�� ��ȣ�ۿ� �� �� �ִ� �Ÿ�;
    extern float		fNPC_INTERACTION_ADJUST; //  NPC�� ��ȣ�ۿ��� �Ҷ� ���� ���� ���� (����, ����Ʈ, ���� ���� �� �Ŵ� ��ġ���� ���� �˳��ϰ� ��ȿ�� ����);
	extern float		fLOW_SEED_DAMAGE;		//	damage ���� ���� ���� damage.
	extern float		fCONFT_SPTY_EXP;		//	�б� ��� �¸��ÿ� ��ü ����ġ �й�.

	extern WORD			wBODYRADIUS;			//	ĳ���� ��ü �ݰ�.

	// fREACT_VALID_SCALE ���� ���� ���� ���� ���ε� ��κ� �������� 30.0f�� �Ǿ� �ִ�;
	// ���� ������ �������� ��������, ������ ���� ��ȿ�� ������ ���ϰڴٴ� �ǹ̷� ���ǰ� ����;
	extern float		fREACT_VALID_SCALE;		//	�׼��� ��ȿ �Ÿ� ����.

	extern float		fATTACK_RANGE_ADJUST_RATE; // �Ϲ� ���� ���� ���� ����;

	extern float		fUNIT_TIME;			    //	ȸ�� ���� �ð� (Actor ��ü);
	extern float		fHP_INC_PER;			//	ĳ���� �����ð��� HP ȸ����(%);
	extern float		fMP_INC_PER;		    //	ĳ���� �����ð��� MP ȸ����(%);
	extern float		fSP_INC_PER;		    //	ĳ���� �����ð��� SP ȸ����(%);

	extern float		fHP_INC;				//	ĳ���� �����ð��� HP ȸ����;
	extern float		fMP_INC;				//	ĳ���� �����ð��� MP ȸ����;
	extern float		fSP_INC;				//	ĳ���� �����ð��� SP ȸ����;

	extern float		fRESIST_PHYSIC_G;		//	���� ���ݽ� �Ӽ� ����ġ ���� ����.
	extern float		fRESIST_G;				//	�Ӽ� ����ġ ���� ����.
	extern float		fREGIST_MAX_G;		// �Ӽ� ����ġ �ִ� ���� ����;
	extern float		fRESIST_MAX_CROW;		// �Ӽ� ����ġ �ִ� ���� ����(Crow);
	extern WORD			wBASIC_DIS_SP;			//	�⺻ �Ҹ� SP.

	extern float		fLOWSP_MOTION;			//	���¹̳� (%) ���� ������ ��� ó��.
	extern float		fLOWSP_DAMAGE;			//	���¹̳� ������ Damage ����.
	extern float		fLOWSP_HIT_DROP;		//	���¹̳� ������ ������ ����.
	extern float		fLOWSP_AVOID_DROP;		//	���¹̳� ������ ȸ���� ����.
	extern DWORD		dwACCEPT_LOWERSTATS;	//	stats �������� ���¹̳��� Ŀ���� �� �ִ� ���� �ѵ�. ( ���� ���⿡ ����. )

	extern float		fDAMAGE_DEC_RATE;		//	����� ������.
	extern float		fDAMAGE_GRADE_K;			//	(�⺻����+�������) ����� �ݿ���.
	extern float		fDAMAGE_DEC_MAX_G;		// �ִ� ������ ������;

	extern DWORD		dwCRITICAL_DAMAGE;		//	ũ��Ƽ�� ������ ����
	extern DWORD		dwCRITICAL_MAX;			//	ũ��Ƽ�� Ȯ�� MAX
	extern DWORD		dwCRUSHING_BLOW_DAMAGE;	//  ����Ÿ�� ������ ����
	extern DWORD		dwCRUSHING_BLOW_MAX;	//  ����Ÿ�� Ȯ�� max
	extern float		fCRUSH_BLOW_RANGE;		//  ����Ÿ�ݽ� �з����� �Ÿ� ( ������ ) 	

	extern DWORD		dwReleaseStigmaDistance;	// ���� ȿ�� ���� �Ÿ�(����Ʈ);

	extern SNATIVEID	sProductMotionID;		// ���� �� ���� ��� ID
	extern FLOAT		fReferCharDelay;		// ĳ���� �������� ��û ������

	//---------------------------------------------------------------------------------------
	extern std::string	sInvenExItemSearch;		// �κ��丮 Ȯ��ī�� �˻���
	extern WORD			wInvenDefaultLine;		// �κ��丮 �⺻ �ټ�
	extern WORD			wInvenExtendLine;		// �κ��丮 Ȯ�� ������ �ִ� �ټ�(EInvenTotalLine - wInvenDefaultLine - EInvenPremiumLine)
	extern WORD			wInvenExtendEventLine;	// �κ��丮 �⺻ Ȯ�� �ټ�(ĳ���� ���� �� ����)

	//---------------------------------------------------------------------------------------
	extern float		fTrash_SellCostPer;		// ������ �̿�� 1���� (���ӸӴ�)
	extern DWORD		wTrash_Cost;			// ������ �̿�� 2���� (���ӸӴ�)
	extern DWORD		wTrash_Point;			// ������ �̿�� 3���� (�ΰ��� ĳ��)

	extern int			nBrightMAX;				// ����Ʈ ���� ���� �ִ밪
	extern int			nBrightMIN;				// ����Ʈ ���� ���� �ּҰ�

	//	Note : ----------------------------------------------------------------���
	extern float		fCONFRONT_TIME;			//	��� ���� �ð�.
	extern float		fCONFRONT_ELAP;			//	���� ��� ���� ���� �ð�.
	extern DWORD		dwCONFRONT_MAX;			//	���� ��� �� ���� Ƚ��.
	extern float		fCONFRONT_RESET;		//	���� ��� ���� ���� �ð�.
	extern float		fCONFRONT_ONE_DIST;		//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )
	extern float		fCONFRONT_PY_DIST;		//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )
	extern float		fCONFRONT_CLB_DIST;		//	��� �Ÿ� ����. ( 0�� ��� ���Ѵ�. )

	extern int			nCONFRONT_WIN_LP;		//	��� �¸��� living point ( ��Ȱ ���� ).
	extern int			nCONFRONT_LOSS_LP;		//	��� �й�� living point ( ��Ȱ ���� ).

	extern int			nCONFRONT_CLB_WIN_LP;	//	Ŭ�� ��� �¸��� living point
	extern int			nCONFRONT_CLB_LOSS_LP;	//	Ŭ�� ��� �й�� living point

	//	Note : ----------------------------------------------------------------���
	enum { EMCONFT_RC_TYPENUM = 10, EMCONFT_STATE_TYPENUM = 8 };
	extern WORD			wCONFT_RC_TYPE[EMCONFT_RC_TYPENUM];			//	��� ȸ���� ��밡�� Ƚ��.
	extern float		fCONFT_STATE_TYPE[EMCONFT_STATE_TYPENUM];	//	��� ü�� Ȯ�� ����.

	extern WORD			wCONFRONT_SCHOOL_LIMIT_NUM;					//	�б��� ��� �ּ� �ο�.
	extern WORD			wCONFRONT_SCHOOL_LIMIT_LEVEL;				//	�б��� ��� �ּ� ����.
	extern float		fCONFRONT_SCHOOL_EXP_SCALE;					//	�б��� ��ý� ���� ����ġ ������.

	extern WORD			wCONFRONT_SCHOOL_WIN_NUM;
	extern WORD			wCONFRONT_SCHOOL_WIN_RATE;
	extern float		fCONFRONT_SCHOOL_GENITEM;

	//	Note : ----------------------------------------------------------------pk
	enum { EMPK_STATE_LEVEL = 5, };
	extern BOOL			bPK_MODE;				//	pk on/off.
	extern BOOL			bPKLESS;				//  �������� PK on/off.
	extern int			nPK_LIMIT_LEVEL;		//	pk ���� ����.
	extern int			nPK_LIMIT_DX_LEVEL;		//	pk ������ ���� ����.
	extern int			nPK_TRY_BRIGHT_POINT;	//	pk �õ��� ����Ʈ.
	extern int			nPK_KILL_BRIGHT_POINT;	//	pk �׿��� ��� ����Ʈ.
	extern int			nPK_TRY_LIVING_POINT;	//	pk �õ��� ����Ʈ.
	extern int			nPK_KILL_LIVING_POINT;	//	pk �׿��� ��� ����Ʈ.
	extern float		fNONPK_ITEM_DROP;		//	non pk �� ������ �����.
	extern float		fPK_ITEM_DROP;			//	pk �� ������ �����.

	//	 ( pk level : 0 ~ 4 )
	//	0 - �Ϲ��л�
	//	1 - �ҷ��л�
	//	2 - ������
	//	3 - ���θ�
	//	4 - ���α�
	extern DWORD		dwPK_RECALL_ENABLE_LEVEL;	//	��ȯ ��� ���� ���� ����.
	extern DWORD		dwPK_DRUG_ENABLE_LEVEL;		//	ȸ���� ��� ���� ���� ����.


	extern float		fPK_JUSTNESS_TIME;		//	�ݰ� ������ ������� �ð�.
	extern float		fPK_SAFE_TIME;			//	pk ���� �ð�.

	extern float		fCDM_SAFE_TIME_ACCRUE[CDM_ACCRUE_COUNT];	//	CDM���� ���� �ð�.
	extern float		fCDM_SAFE_TIME_MOVE_SPEED;					//	CDM���� ���� �ð� �̵��ӵ� ���ҷ� ( 0.0f ~ 1.0f )

	extern DWORD		dwPK_NORMAL_NAME_COLOR;	//	pk ��ġ�� ������ �̸� ����.
	extern float		fPK_POINT_DEC_RATE;		//	pk ��ġ ������. ( �ð���. )
	extern float		fPK_POINT_DEC_PHY;		//  pk ������ ������ ( ���� ).
	extern float		fPK_POINT_DEC_MAGIC;	//  pk ������ ������ ( ���� ).

	extern float		fReductionRate_PVP_PHY_P_Damage;			// PVP ������ ( ���ݷ�, ����ġ );
	extern float		fReductionRate_PVP_PHY_S_Damage;			// PVP ������ ( ���ݷ�, ���ġ );
	extern float		fReductionRate_PVP_MAGIC_Damage;			// PVP ������ ( ���ݷ�, ���ġ );
	extern float		fReductionRate_PVP_PHY_Defense;				// PVP ������ ( ���� );
	extern float		fReductionRate_PVP_MAGIC_Resistance;		// PVP ������ ( ���� );

	extern SPLAYERKILL	sPK_STATE[EMPK_STATE_LEVEL];
    extern int          nNON_PK_CONNECT_BRIGHT_POINT; //! non pk ä�� ���Ӱ��� ���� ��������

	//	Note : ----------------------------------------------------------------�����̻�
	enum { EMSTATEBLOW_LEVEL_SIZE = 10, EMSTATEBLOW_LEVEL_BASE = 1, };
	extern int			nSTATEBLOW_LEVEL[EMSTATEBLOW_LEVEL_SIZE];

	//	Note : ----------------------------------------------------------------����ġ Table
	//
	enum { EXPTABLE_SIZE = 52, EXPTABLE_LVL = 19, EXPBASEINDEX = 10, EXP_LVL_STEP = 5,
		EXPTABLE_RANGE = 61,  EXPTABLE_RANGE_BASE = 30 };
	extern WORD			wEXPTABLE[EXPTABLE_SIZE][EXPTABLE_LVL];
	extern float		fEXP_RATE_TABLE[EXPTABLE_RANGE];

	extern LONGLONG		lnEXP_MAX_TABLE[MAX_LEVEL];
	extern LONGLONG		lnEXP_MAX_TABLE_2nd[MAX_LEVEL];
	extern DWORD		dwEXP_PC_TABLE[MAX_LEVEL];

	//	Note : ----------------------------------------------------------------DPS Meter;
	extern float		fDPS_TurnToPeaceModeInterval;

	//	Note : ----------------------------------------------------------------��Ƽ.
	extern float		fPARTYEXPRANGE;				//	����ġ ������ ������ �ִ� �Ÿ�.
	extern float		fPARTYEXP_S;						//	��Ƽ ����ġ �й�� �������̿� ���� ���Һ���.
	extern float		fPartyTenderTime;					// ���� ���� �ð�(��Ƽ)
	extern float		fExpeditionTenderTime;				// ���� ���� �ð�(������);
	extern float		fUPartyTenderTime;
	extern float		fUExpeditionTenderTime;
	extern float		fPartyTenderRange;					// ���� ���� ����(��Ƽ);
	extern float		fExpeditionTenderRange;			// ���� ���� ����(������);	
	extern float		fExpeditionDecreaseExpRate;		// ����ġ ���� ����(������);
	extern float		fSwapSlotBlankInterval;				// �ڸ� �̵� Ȱ��ȭ�� ���� ������ �ð� ����;
	extern WORD	aPARTYEXP_ONE[MAXPARTY];
	extern WORD	aPARTYKILLEXP_ONE[MAXPARTY];
	extern DWORD	dwPARTY_INVITE_MAX_WAIT_TIME; // ��Ƽ �ʴ�� ���� ��� �ð�;	
	extern DWORD	dwExpeditionCheckReadyTime;	// �غ� Ȯ�� ���� �ð�(30��); 

	//	Note : ----------------------------------------------------------------Ŭ��
	extern DWORD		dwCLUB_PARTYNUM;			//	Ŭ�� â���� �ּ� ��Ƽ(�ʱ�) �ο�.
	extern DWORD		dwCLUB_DISSOLUTION_DAY;		//	Ŭ�� ��ü ���� ��.
	extern DWORD		dwCLUB_JOINBLOCK_DAY;		//	Ŭ�� Ż��� ���� ���� ��.
	extern DWORD		dwMAX_CLUBRANK;
	extern GLCLUBRANK	sCLUBRANK[MAX_CLUBRANK];	//	Ŭ�� ��ũ�� ���.
	extern DWORD		dwCLUB_AUTHORITY;			//	Ŭ�������� ���� ���� �ð�			
	extern DWORD		dwCLUB_ALLIANCE_SEC;		//	Ŭ�� ���� Ż��, ������ �簡�� ���ѽð�.
	extern DWORD		dwCLUB_ALLIANCE_DIS;		//	Ŭ�� ���� ��ü�� �Ἲ ���ѽð�.
	extern DWORD		dwCLUB_ALLIANCE_NUM;		//	Ŭ�� ���� �ִ� ����.

	extern BOOL 		bCLUB_BATTLE;				//	Ŭ�� ��Ʋ ���� ����
	extern DWORD		dwCLUB_BATTLE_MAX;			//  Ŭ�� ��Ʋ ���� ���� ����
	extern DWORD		dwCLUB_BATTLE_RANK;			//  Ŭ�� ��Ʋ ��ũ ����
	extern DWORD		dwCLUB_BATTLE_TIME;			//  Ŭ�� ��Ʋ �ð�(��)
	extern DWORD		dwCLUB_BATTLE_TIMEMIN;		//	Ŭ�� ��Ʋ �ּҽð�(��)
	extern DWORD		dwCLUB_BATTLE_TIMEMAX;		//	Ŭ�� ��Ʋ �ִ�ð�(��)
	extern DWORD		dwCLUB_BATTLE_GUID_TIME;	//  Ŭ�� ��Ʋ ������ ���� �ð�(��)
	extern DWORD		dwCLUB_BATTLE_DIS_TIME;		//  Ŭ�� ��Ʋ ���� �� �׺� ������ �ð�(��)
	extern BOOL			bCLUB_BATTLE_ALLIANCE;		//  Ŭ�� ��Ʋ�� ���� �ڵ� ���� ���

	extern BOOL			bCLUB_DEATHMATCH;			//	Ŭ�� ������ġ ���� ����
	extern DWORD		dwCLUB_DEATHMATCH_MEM;		//	Ŭ�� ������ġ ���� �����
	extern DWORD		dwCLUB_DEATHMATCH_RANK;		//	Ŭ�� ������ġ ���� ��ũ

    extern DWORD        dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME; // Ŭ�� ��ũ ���� ���� �ð�(��).

	extern DWORD		dwClubMarkARGB;				// Ŭ�� ��ũ RGB;
	extern std::vector< DWORD > vecClubMarkARGB;

	//	Note : ----------------------------------------------------------------������
	extern float		fMAX_COMMISSION;			//	�ִ� ���� ���� ������ ����.
	extern float		fDEFAULT_COMMISSION;		//	�� ���� ������ ������.
	extern float		fEARNING_RATE;				//	������ ������.
	extern float		fEARNING_RATE_NPC;			//	NPC ��ȯ ������ ������.
	extern float		fCDCERTIFY_DIST;			//	���� ���� �Ÿ�.
	extern float		fCDCERTIFY_DIST2;			//	���� ���� �Ÿ�.
	extern float		fCDCERTIFY_TIME;			//	���� �ҿ� �ð�.

	//	Note : ----------------------------------------------------------------���� ����
	extern float		fEXP_SCALE;					//	Note : ����ġ ����.
    extern float		fITEM_DROP_SCALE;			//	Note : ������ �����.
    extern float		fITEM_DROP_LOWER_SCALE;		//	Note : ��� ������ �����.
	extern float		fMONEY_DROP_SCALE;			//	Note : �� �����.

    extern float        fITEM_DROP_SPECIFIC_GRADE_SCALE; //  Note : Ư�� ������ ��� �����.(����� genitem���� ����)
    extern WORD         wITEM_DROP_SPECIFIC_GRADE;       //  Note : ������� ������ų Ư�� �������� ���


	//	Note : ---------------------------------------------------------------- �̺�Ʈ ������ ����
	extern float		fEVENT_EXP_SCALE;					//	Note : �̺�Ʈ ����ġ ����.
	extern float		fEVENT_ITEM_DROP_SCALE;			//	Note : �̺�Ʈ ������ �����.
	extern float		fEVENT_MONEY_DROP_SCALE;			//	Note : �̺�Ʈ �� �����.

	//	Note : ----------------------------------------------------------------�п� ���� pk
	extern bool			bSCHOOL_FREE_PK_ALWAYS;		//	�п��� ���� pk �׻� ���� ����.
	extern bool			bSCHOOL_FREE_PK;			//	�п��� ���� pk on/off.
	extern bool			bSCHOOL_FREE_PK_TIME_REAL;	//	�п��� ���� pk ���� �ð� ( on : ���� �ð�, off : ���ӽð� )
	extern bool			bSCHOOL_FREE_PK_Z_FORCED;	//	�п��� ���� pk ���� ����.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_START;//	�п��� ���� pk ���� �ð�.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_TERM;	//	�п��� ���� pk ���� �ð�.
	extern float		fSCHOOL_FREE_PK_ITEM_DROP;	//	�п��� ���� pk ������ ����� ���갪. ( �⺻ ����� + ���갪. )
	//	Note : ----------------------------------------------------------------

	//	Note : ----------------------------------------------------------------��� ����.
	//extern bool			bENCHANT_TERMINATE_ITEM;	//	��þƮ�� ������ �ļ� ����.
	extern bool			bMONEY_DROP2FIELD;			//	�� �ٴڿ� ������ ���� ����.

	//	Note : ----------------------------------------------------------------���� Ȯ��.
	//extern float		fGRADE_RATE[GRADE_LIMIT_MAX];
	//extern float		fGRADE_RESET_RATE[GRADE_LIMIT_MAX];
	//extern float		fGRADE_TERMINATE_RATE[GRADE_LIMIT_MAX];
	//extern float        fGRADE_ITEMOPTION_INC_GRINDINGRATE[GRADE_LIMIT_MAX];  // ��ȭ�� ���� Ȯ��
	//extern int			nGRADE_ITEMOPTION_ANTIRESET[GRADE_LIMIT_MAX];		  // ��ȭ�� ���� ���� �ܰ� 
	extern float		fGRADE_EVENT_RATE;


	//	Note : ---------------------------------------------------------------- ����(����) ���� hsshin
	extern bool			bUnableProductType[Product::ETypeTotal];
	extern bool			bPublicStudyProductType[Product::ETypeTotal];
	extern DWORD		dwStudyPointDefault;

	//	Note : ---------------------------------------------------------------- ä�� ����
	extern bool			bCHAT_EXTEND;					// ����, ��Ƽ���� Ȱ��ȭ ����
	extern float		dwCHAT_AREA_CHARGE;				// ���� ä�� ���
	extern float		dwCHAT_PARTY_RECRUIT_CHARGE;	// ��Ƽ ���� ���
	extern float		fCHAT_PARTY_RECRUIT_TIME;		// ��Ƽ ���� ���� �ð�

	//	Note : ----------------------------------------------------------------����
	extern std::string	strGRINDING_SUCCEED;
	extern std::string	strGRINDING_FAIL;
	extern std::string	strGRINDING_RESET;
	extern std::string	strGRINDING_BROKEN;

	extern std::string	strGAMBLING_SHUFFLE;	// Monster7j
	extern std::string	strGAMBLING_WIN;		
	extern std::string	strGAMBLING_LOSE;
	extern std::string	strDICE_ROLL;

	extern std::string	strITEMDROP_SUIT;
	extern std::string	strITEMDROP_WAPON;
	extern std::string	strITEMDROP_SHOES;
	extern std::string	strITEMDROP_RING;
	extern std::string	strITEMDROP_QBOX;

	extern std::string	strITEMDROP_SCROLL;
	extern std::string	strITEMDROP_COIN;
	extern std::string	strITEMDROP_DRUGS;

	extern std::string	strPICKUP_ITEM;
	extern std::string	strQITEM_FACT;
	extern std::string	strQITEM_BGM[QUESTION_SIZE];
	extern std::string	strITEMDROP_RANK[EMITEM_LEVEL_NSIZE];

	//	Note : ----------------------------------------------------------------ȿ��
	extern std::string	strSELECT_CHAR;			//	�κ񿡼� �ɸ��� ���ý�.

	extern std::string	strREBIRTH_EFFECT;		//	ĳ�� ��Ȱ ����Ʈ.
	extern std::string	strLEVELUP_EFFECT;		//	���� �� ����Ʈ.

	extern std::string	strSRIKE_ELMT_EFFECT[EMELEMENT_MAXNUM]; //	Ÿ�ݽ� ����Ʈ.
	extern std::string	strSTRIKE_ARROW_EFFECT;					// ȭ�� ����ü ����Ʈ;
	extern std::string	strSTRIKE_BULLET_EFFECT;				// �Ѿ� ����ü ����Ʈ;

	extern std::string	strAMBIENT_EFFECT;		//	Ÿ�ݽ� Ambient ����Ʈ
	extern std::string	strERASE_EFFECT;		//	�� ������� ����Ʈ.

	extern std::string	strERASE_SUMMON_EFFECT;	 //  ��ȯ�� ������� ����Ʈ.
	extern float		fERASE_SUMMON_BEGIN_TIME; // ��ȯ�� �������� �����ϴ� �ð� ( EndTime - fERASE_SUMMON_BEGIN_TIME ���� )

	extern std::string	strSKILL_LEARN_EFFECT;	//	��ų �����.
	extern std::string	strSKILL_UP_EFFECT;		//	��ų ��.

	extern std::string strQUEST_START;			//	����Ʈ ����.
	extern std::string strQUEST_END_SUCCEED;	//	����Ʈ ����.
	extern std::string strQUEST_END_FAIL;		//	����Ʈ ����.

	extern std::string	strMOB_GEM_EFFECT;		//	�� ������ ����Ʈ.
	extern std::string	strMOB_DIE_BLOOD;		//	���� �׾�����.

	// PET
	extern std::string	strPET_GEN_EFFECT;		// �� ������ ����Ʈ

	// Vehicle
	extern std::string  strVEHICLE_GEN_EFFECT;	// ž�� ����Ʈ

	extern std::string	strCONFRONT_BOUND;		//	��� ��輱 ǥ�� ȿ��.

	extern std::string	strHALFALPHA_EFFECT;	//	���� �ɸ���.

	extern std::string	strBLOW_NUMB_EFFECT;
	extern std::string	strBLOW_STUN_EFFECT;
	extern std::string	strBLOW_STONE_EFFECT;
	extern std::string	strBLOW_BURN_EFFECT;
	extern std::string	strBLOW_FROZEN_EFFECT;

	extern std::string	strBLOW_MAD_EFFECT;
	extern std::string	strBLOW_POISON_EFFECT;
	extern std::string	strBLOW_CURSE_EFFECT;

	extern std::string	strBLOW_BODY_NUMB_EFFECT;
	extern std::string	strBLOW_BODY_STUN_EFFECT;
	extern std::string	strBLOW_BODY_STONE_EFFECT;
	extern std::string	strBLOW_BODY_BURN_EFFECT;
	extern std::string	strBLOW_BODY_FROZEN_EFFECT;

	extern std::string	strBLOW_BODY_MAD_EFFECT;
	extern std::string	strBLOW_BODY_POISON_EFFECT;
	extern std::string	strBLOW_BODY_CURSE_EFFECT;

	extern std::string strBLOW_EFFECTS[EMBLOW_SIZE];
	extern std::string strBLOW_BODY_EFFECTS[EMBLOW_SIZE];

    extern std::string strCRUSHING_BLOW_EFFECT;		// ����Ÿ�� ����Ʈ
    extern std::string strAUTH_EFFECT;              // ���� ���� ���� ����Ʈ
    extern std::string strAUTH_SM_EFFECT;		    // �����п� ���� ����Ʈ
    extern std::string strAUTH_HA_EFFECT;		    // �����п� ���� ����Ʈ
    extern std::string strAUTH_BH_EFFECT;		    // ��Ȳ�п� ���� ����Ʈ

	extern std::string strCOMPLETE_NPC_EFFECT;		// �Ϸ� ����Ʈ ����Ʈ

	extern std::string strCLICK_EFFECT;				// �̵� ���� ��ġ ǥ�� ����Ʈ
	extern std::string strMINE_EFFECT;				// �ڱ� �ڽ��� ���������� ������ ����Ʈ
	extern std::string strCURSOR_TARGET_EFFECT;		// Ŀ�� ���� ��� ����Ʈ
	extern std::string strTARGET_EFFECT;			// ���� ��� ����Ʈ
	extern std::string strTARGET_EFFECT_ENEMY;		// ���� ��� ����Ʈ(��)
	extern std::string strTARGET_EFFECT_NPC;		// ���� ��� ����Ʈ(npc)
	extern std::string strTARGET_EFFECT_ALLIANCE;	// ���� ��� ����Ʈ(�Ʊ�)
	extern std::string strARROW_EFFECT_ENEMY;		// ȭ��ǥ ����Ʈ(��)
	extern std::string strARROW_EFFECT_NPC;			// ȭ��ǥ ����Ʈ(npc)
	extern std::string strARROW_EFFECT_ALLIANCE;	// ȭ��ǥ ����Ʈ(�Ʊ�)
	extern std::string strSCOPE_TEXTURE_CIRCLE;		// ���� ����Ʈ (��)
	extern std::string strSCOPE_TEXTURE_FANWISE;	// ���� ����Ʈ (��ä��)
	extern std::string strBattleIcon;				// ������� ������
	extern SNATIVEID sBattleIconIndex;				// ������� ������ �ε���.

	extern std::string strCA_EFFECT;				// �ݰ�

    extern std::string strCOMPLATE_ACTIVITY_EFFECT; //  Activity �Ϸ�

	extern GLCONST_CHARCLASS	cCONSTCLASS[GLCI_NUM_ACTOR];	//	ĳ���� Ŭ������ ���.

	extern std::string strCLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR];
	extern std::string strJUMPING_CLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR];

	extern std::vector<std::string>	vecGUIDANCE_FILE;
	extern std::vector<std::string>	vecClubDM_FILE;

	extern WORD	wPK_KILL_Combat_Point;	//	CP �߰� ����Ʈ
	extern BOOL	bCOMBAT_POINT;			//	CP ��� ����
	extern float fIllusionInterval;

    //
    // ������
    extern bool     bDurabilityEmsuit[SUIT_NSIZE];  // ���� ���, ������ ������ġ ���� �� 	( Ư�� ���(EMSUIT) ������ ������ �ý����� ���� ����  )
    extern WORD     wDurabilityDropDeathPvE;        // ����ϸ� �������� �����մϴ�. (��)
    extern WORD     wDurabilityDropDeathPvP;        // ����ϸ� �������� �����մϴ�. (���)
    extern DWORD    dwDurabilityCountLimit;         // �������� ī���ü��� �����ϸ� 1 �����մϴ�. (ex ī���� 1000�� == ������ 1���� )
    extern DWORD    dwDurabilityDropSkillUse;       // ��ų�� ����ϸ� �����ϴ� ī��Ʈ
    extern DWORD    dwDurabilityDropHitPvE;	        // ����� 1Point�� �����ϴ� ī��Ʈ (��)
    extern DWORD    dwDurabilityDropHitPvP;	        // ����� 1Point�� �����ϴ� ī��Ʈ (���)
    extern int      nDurabilityRateTable[DURABILITY_RATE_COUNT];        // ������ ������ ���� ������ ������
    extern int      nDurabilityRatePerformance[DURABILITY_RATE_COUNT];  // ������ ���ҿ� ���� ������ �ɷ�ġ ����

	extern int      nWPowDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWDexDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWSpiDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWMixDurRatePerformance[DURABILITY_RATE_COUNT];  

// #ifdef _RELEASED
	extern BYTE		nDurabilityApply[DURABILITY_APPLY_COUNT];
	extern float	fDurabilityAlarmRemainPer;		// ���� ���� ���Ͻ� ���޴��� �˸������� �ν�Ʈ���� ����, ���޴����� �˸���;
// #endif

	//! �ΰ��ɷ� ǥ�� ����
	extern bool		bAdditional_ability;

	//! ĳ���� Ŭ������ �ʱ� ������.
	extern char	szCharSkin[GLCI_NUM_ACTOR][MAX_PATH];	//	ĳ���� �ʱ� ����.

	//	Note : ĳ���� �ܼ�ȭ ����
	//
	extern char					szManSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH];	//	ĳ���� �ܼ�ȭ ���� ���� ����. 
	extern char					szWomenSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH];	//	ĳ���� �ܼ�ȭ ���� ���� ����.
	extern std::string			strCharBoard[GLCI_NUM_ACTOR];	//	ĳ���ͺ� �ܼ�ȭ ������ �̹���	
	extern std::string			strItemLevelIcon[EMITEMLEVEL_NUM];

    extern std::string strIPEventFileName;
	extern std::string strQuestGroupFileName;

	extern std::vector<DWORD>	vecCTFNotify;

	extern float fCTF_BECOME_AWAY_TIME; // CTF ���� �Է��� ���� �� �ڸ���� ���� �ɸ��� �ð�(��).

	extern float fPremiumMapExitCheckTime;							// �����̾� �� �������� üũ �ð� (��)
	extern std::vector< DWORD > vecPremiumMapForcedExitNotify;		// �����̾� �� �������� ���� �ð� �˸�

	extern LONGLONG NotifyMoney;	// ���� �Ӵ� ��ȭ�� �˸� ����
	extern int nDecomposeCost;		// ���ؽ� ���;
	extern float fQBoxPickableTime;	// Q�ڽ� �ݱ� �ð�;
	extern float fMacro_TermsofClose_LimitTime; // �����ð��� ���� ��ũ�� ��������;
	extern int nMacro_TermsofClose_Potion; // ���� ���࿡ ���� ��ũ�� ��������;
	extern float fMacro_MaxFillTime; // �������� ä����ִ� ��ũ�� �ð�(����-��);

	extern std::string strScreenEffect[SCREEN_EFFECT_NUM];
	extern std::string strCameraEffect[CAMERA_EFFECT_NUM];
	extern std::string strOPItemIcon;
	extern std::vector< SSTATPOINT_BYLEVEL > vecMaxStatPoint;
	extern std::vector< SSTATPOINT_BYLEVEL > vecExtremeMaxStatPoint;
	extern std::vector<INT> vecMacroRemainTimeAlarm;

	extern float	fIgnore_BossDamageMax;				// ���������� �ǰݽ� ������ ����;
	extern int		nBossFixedDamageMax;				// ������ ���ݽ� �߰� ���� ������;
	extern float	fIncRCriticalMax_EmergencyMob;		// ���� ������ ������ ũ��Ƽ�� �߻��� ����;
	extern float	fIncRCriticalMax_EmergencyBoss;		// ���� ������ �������� ũ��Ƽ�� �߻��� ����;
	extern float	fDecRDamageMax_EmergencyState;		// ���� ������ ��� ������ ����;

	// ��޻��� ��ȯ HP Rate;
	extern float	fEmergency_CharHPR;
	extern float	fEmergency_MobHPR;
	extern float	fEmergency_BossHPR;
	// ��� ���½� �ʴ� ������ Ƚ��;
	extern int		nEmergency_BlinkCount;

	extern float	fIncRExpAddEffectMax;				// ����ġ ������ ȿ��'�� ������Ŵ;
	extern float	fIncRDropGenMoneyMax;				// ����Ӵ� ����;
	extern float	fDecRDamageMax_MeleeSkill;			// ���� ���� ��ų �ǰݽ� ��� ������ ����;
	extern float	fDecRDamageMax_RangeSkill;			// ��� ��ų �ǰݽ� ��� ������ ����;
	extern float	fDecRDamageMax_MagicSkill;			// ���� ��ų �ǰݽ� ��� ������ ����;
	extern int		nInc_HPMax_SoloPlay;				// �ַ��÷��̽� HP���� (��Ƽ�� �ƴҽ�);
	extern int		nInc_HPMax_PartyPlay;				// ��Ƽ�÷��̽� HP����;
	extern float	fIncR_AtkSpeedMax_SoloPlay;			// �ַ��÷��̽� �������� (��Ƽ�� �ƴҽ�);
	extern float	fIncR_AtkSpeedMax_PartyPlay;		// ��Ƽ�÷��̽� ��������;
	
	extern float	fInc_RandomOp_SkillDurationAddTimeMax;	// ��ų ��޺� ���ӽð� �ִ� ������;
	extern float	fIncR_RandomOp_SkillDamageAddMax;		// ��ų ��޺� ������ �ִ� ������;
	extern float	fDecR_SkillDelayTMax;					// ��ų ��޺� ������ �ִ� ���Ұ�;
	extern float	fIncR_CriticalDamageMax;				// ũ��Ƽ�� ������ �ִ� ������;
	extern float	fIncR_CrushingBlowMax;					// ����Ÿ�� ������ �ִ� ������;
	extern float	fIncR_LifePerKillMax;					// óġ�� ������ �ִ� ȸ����;
	extern float	fIncR_MPPerKillMax;						// óġ�� MP �ִ� ȸ����;
	extern float	fIncR_SPPerKillMax;						// óġ�� HP+MP+SP �ִ� ȸ����;
	extern float	fIncR_HPMPSPPerKillMax;					// óġ�� SP �ִ� ȸ����;
	extern float	fIncR_AllMPRatioToAttackMax;				// ��ü MP ������ŭ ���ݷ�����Max;
	extern float	fIncR_AllSPRatioToHpMax;					// ��ü SP ������ŭ ü������Max;
	extern float	fIncR_AllHitRatioToMeleeMax;				// ��ü ���� ������ŭ ����ġ ����Max;
	extern float	fIncR_AllHitRatioToRangeMax;				// ��ü ���� ������ŭ ���ġ ����Max;
	extern float	fIncR_AllHitRatioToMagicMax;				// ��ü ���� ������ŭ ���ġ ����;

    BOOL LOADFILE(const std::string& FileName, BOOL bServer=TRUE);
	BOOL EXP_LOADFILE ( char* szFileName );

	BOOL EXP_PC_LOADFILE ( char* szFileName );
	BOOL EXP_MAX_LOADFILE ( char* szFileName );
	BOOL EXP_MAX_2nd_LOADFILE ( char* szFileName );

	BOOL VAID_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex, const BOOL bJumping = FALSE );
	SCHARDATA2& GET_CHAR_DATA2 ( WORD wSchool, EMCHARINDEX emIndex, const BOOL bJumping = FALSE );

	inline const char* GETSTRIKE_EFFECT ( EMELEMENT emELEMENT=EMELEMENT_SPIRIT )
	{
		GASSERT(emELEMENT>=0);
		GASSERT(emELEMENT<EMELEMENT_MAXNUM);

		return strSRIKE_ELMT_EFFECT[emELEMENT].c_str();
	}

    void    SET_EXCESS_EXP_TYPE( BYTE expType );
    BYTE    GET_EXCESS_EXP_TYPE();

	const char* GETSCHOOLNAME ( WORD wSCHOOL );

	void SET_EVENT_ITEM_SCALE ( float fScale );		//	�̺�Ʈ ���� ����
	float GET_ITEM_SCALE ();						//	������ * �̺�Ʈ ����

	void SET_EVENT_MONEY_SCALE ( float fScale );	//	�̺�Ʈ ���� ����
	float GET_MONEY_SCALE ( const float fAddScale = 0.0f );						//	������ * �̺�Ʈ ����

	void SET_EVENT_EXP_SCALE ( float fScale );		//	�̺�Ʈ ���� ����
	float GET_EXP_SCALE ();							//	������ * �̺�Ʈ ����

	DWORD GETEXP ( int nAttackerLev, int nDefenserLev );
	float GETEXP_RATE ( int nAttackerLev, int nDefenserLev );

	DWORD GETEXP_PC ( WORD wLev );

    //! non pk ä�� ���Ӱ��� ���� ��������
    int GetNonPkConnectBrightPoint();
	// �ش� ��ų�� ��� Ŭ���� ������ ����;
	EMCLASSINDEX GetClassInfoUsingSkill(SNATIVEID sNID);
};

struct PETSTYLE
{
	std::string	strSTYLE_CPS[MAX_HAIR];
	WORD		wSTYLE_COLOR[MAX_HAIR];
	WORD		wSTYLENum;


	PETSTYLE () :
	wSTYLENum(0)
	{
		memset( wSTYLE_COLOR,0,sizeof(wSTYLE_COLOR));
	}
};

typedef std::vector<PGLPET> VEC_PGLPET;
typedef std::vector<PETSTYLE> VEC_PET_STYLE;

namespace GLCONST_PET
{
	extern VEC_PGLPET pGLPET;
	extern VEC_PET_STYLE sPETSTYLE;

	extern float		fRunArea;
	extern float		fWalkArea;
	extern float		fOwnerDistance;
	extern int			nMAXVIEWRANGE;
	extern float		fMaginotLineHP;
	extern float		fMaginotLineMP;
	extern float		fMaginotLineSP;

	PGLPET   GetPetData ( int emTYPE );
	PETSTYLE GetPetStyle ( int emTYPE );
	bool     IsChangeablePetColor ( int emTYPE );

	static WORD GetPetStyleSize()
	{
		 return (WORD) sPETSTYLE.size();
	}

	static WORD GetPetStyleNum( WORD wIndex )
	{
		if( 0 > wIndex || GetPetStyleSize() < wIndex )
			return 0;

		return sPETSTYLE[ wIndex ].wSTYLENum;
	}
};

namespace GLCONST_VEHICLE
{
	extern PGLVEHICLE		pGLVEHICLE[VEHICLE_TYPE_SIZE];
	extern char				szVehicleInit[VEHICLE_TYPE_SIZE][MAX_PATH];

	extern int				nFullDecrVehicle[VEHICLE_TYPE_SIZE]; // ������ ����ġ

	PGLVEHICLE		GetVehicleData ( VEHICLE_TYPE emTYPE );

};

namespace GLCONST_SUMMON
{
	extern PGLSUMMON		pGLSUMMON               [SUMMON_TYPE_SIZE];
	extern FLOAT            fSummonGateDistanceRatio[SUMMON_TYPE_SIZE];	
	extern DWORD            dwIdleGestureTimeMax    [SUMMON_TYPE_SIZE];
	extern DWORD            dwIdleGestureTimeMin    [SUMMON_TYPE_SIZE];
	extern DWORD            dwMaxSummon             [SUMMON_TYPE_SIZE];
	extern WORD				wMaxMobTracking			[SUMMON_TYPE_SIZE];
	extern char				szSummonInit            [SUMMON_TYPE_SIZE][MAX_PATH];

	PGLSUMMON GetSummonData ( SUMMON_TYPE emTYPE );
};

namespace GLCONST_ATTENDANCE
{
	struct	ATTEND_REWARD
	{
		int				nComboDay;
		SNATIVEID		idReward;

		ATTEND_REWARD()
			: nComboDay ( 0 )
			, idReward( NATIVEID_NULL() )
		{
		}
	};


	extern bool		bUse;
	extern DWORD	dwMAXREWARD;
	extern DWORD	dwAttendTime;
	extern std::vector<ATTEND_REWARD>	vecAttendReward;

	BOOL LoadFile(const std::string& strFileName, BOOL bServer);
};

namespace MINIGAME_ODDEVEN // �̴� ����, Ȧ¦ ����
{
	const unsigned int MAX_ROUND = 5;

	extern float	fReturnRate[MAX_ROUND];		// ���� ���� �μ�(�������� = ����*1ȸ���μ�*2ȸ���μ�...)
	extern float	fSuccessRate[MAX_ROUND];	// ���� Ȱ��
	extern float	fTimeLimit;					// ���� �ð�(��)
	extern float	fShuffleTime;				// ���� �ð�(��)
	extern UINT		uiMaxBattingMoney;			// �ִ� ���� ���� �ݾ�
};

namespace HAIRCOLOR
{
	enum { MAXSEX = 2, MAXHAIRCOLOR = 16 };

	extern WORD wHairColor[MAXSEX][MAX_HAIR];

	extern WORD GetHairColor ( WORD wClass, WORD wStyle ); 
};


namespace MONEYCOLOR
{
	enum 
	{
		MAX_COLOR = 6,
	};

    extern DWORD dwTEXT_MONEY_COLOR_BEGIN;
    extern DWORD dwMONEY_COLOR_DEFAULT;
	extern DWORD dwMONEY_COLOR[MAX_COLOR];

    extern const DWORD GetMoneyColor (const LONGLONG lnMoney);
}

namespace ITEM_INFOR_COLOR
{
    extern DWORD dwDefault;					// �⺻ �÷�.
    extern DWORD dwItemRank[EMITEM_LEVEL_NSIZE];	// ���� ��޿� ���� �÷�.
    extern DWORD dwReqWrong; 				// ���� ���� �÷�.
    extern DWORD dwInchant1; 				// �⺻ ���ݷ� + ���� �ջ� �÷�.
    extern DWORD dwInchant2; 				// ���� ���� �÷�.
    extern DWORD dwEquipCostum;				// �ڽ�Ƭ �� ���� ���� ���� �÷�.
    extern DWORD dwRankUp1; 				// �⺻ �ɷ�ġ + ���� �ջ� �÷�.
    extern DWORD dwRankUp2; 				// ���� ���� �÷�.
    extern DWORD dwSPReduce;				// SP �߰� �Ҹ� �÷�.
    extern DWORD dwAddAbility;				// ���� ȿ�� ��� �÷�.
    extern DWORD dwRandomAbility;			// ���� �ɼ� ��� �÷�.
    extern DWORD dwElementalAbility;		// �����̻� ��� �÷�.
    extern DWORD dwTrade;					// �ŷ� �� ��ȯ �Ұ� ��� �÷�.
    extern DWORD dwLimitTime;				// ������ ��ȿ�Ⱓ ��� �÷�.
	extern DWORD dwAddDisguise;				// �ڽ�Ƭ ������ ���� ���� �ɼ� �÷�.
	extern DWORD dwMoney;					// ��� �ؽ�Ʈ �÷�.
	extern DWORD dwSetItem;					// ��Ʈ ������ �÷�.

	//////////////////////////////////////////////////////////////////////////

	// ������ ���� ����
	struct ITEM_GRIND
	{
		ITEM_GRIND() { RangeStart=0; RangeEnd = 0; GrindColor = 0; }
		static const int TOTAL_COUNT = 5;
		int RangeStart;
		int RangeEnd;
		DWORD GrindColor;
	};

	extern ITEM_GRIND dwItemGrind[ITEM_GRIND::TOTAL_COUNT];
}

namespace SKILL_INFOR_COLOR
{
    extern DWORD dwDefault; // �⺻ �÷�.
    extern DWORD dwSkillName; // ��ų �̸� ��� �÷�.
    extern DWORD dwSkillGrade; // ��ų ��� ��� �÷�.
    extern DWORD dwSkillType; // ��ų Ÿ�� ��� �÷�.(�Ϲ���, �нú��� ���)
    extern DWORD dwSkillElemental; // ��ų �Ӽ� ��� �÷�.
    extern DWORD dwReqWrong; // ��ų ���� ���� ���� ��� �÷�.
    extern DWORD dwSkillRange; // ��ų ���� ���� ��� �÷�.
    extern DWORD dwAddAbility; // ��ų ȿ�� ���� ��� �÷�.
    extern DWORD dwElementalAbility; // ��ų �����̻� ��� �÷�.
}

namespace CHAT_INFOR_COLOR
{
	extern DWORD CHAT_NORMAL;
	extern DWORD CHAT_PRIVATE;
	extern DWORD CHAT_PARTY;
	extern DWORD CHAT_MASTER;
	extern DWORD CHAT_GUILD;
	extern DWORD CHAT_ALLIANCE;
	extern DWORD CHAT_AREA;
	extern DWORD CHAT_PARTY_RECRUIT;
	extern DWORD CHAT_TOALL;
	extern DWORD CHAT_SYSTEM;
	extern DWORD CHAT_FACTION;
	extern DWORD CHAT_CHAR_NAME;
	extern DWORD CHAT_GM;
}

struct SENTRYLIMITINFO
{
	__time64_t tEntryTime;
	DWORD	   dwReEntryTime;
	SNATIVEID  sMapID;

	SENTRYLIMITINFO() : tEntryTime(0), dwReEntryTime(0), sMapID(NATIVEID_NULL())
	{
	};

	void SetData( const __time64_t &entryTime, const DWORD &reEntryTime, const SNATIVEID &mapID )
	{
		tEntryTime		= entryTime;
		dwReEntryTime	= reEntryTime;
		sMapID			= mapID;
	}

	bool IsEntryLimitEnd()
	{
		CTime CurTime	= CTime::GetCurrentTime();
		CTime EntryTime = tEntryTime;

		CTimeSpan timeSpan = CurTime - EntryTime;

		if( timeSpan.GetTotalSeconds() > dwReEntryTime*60 ) return TRUE;

		return FALSE;
	}

	int GetEntryLimitMinutes()
	{
		CTime CurTime	= CTime::GetCurrentTime();
		CTime EntryTime = tEntryTime;

		CTimeSpan timeSpan = CurTime - EntryTime;


		int nEntryLimitTime = dwReEntryTime - (int)timeSpan.GetTotalMinutes();
		if( nEntryLimitTime == 0 ) nEntryLimitTime = 1;

		return nEntryLimitTime;
	}

};

//
//mjeon.Post.Send
//

struct SPOSTINFO
{
	LONGLONG			llPostID;	
	int					iDesign;

	BYTE				byChargePayment;	
	BYTE				byPostClass;
	BYTE				byPostState;
	BYTE				byPostType;	

	char				Title[POST_MAX_TITLE_LENGTH];
	char				Content[POST_MAX_CONTENT_LENGTH];
	//char				SendDate[20]; //CurrentTime	//yyyy-mm-dd hh:mm:ss
	//char				RecvDate[20];
	//char				ReturnDate[20];
	//char				RecvBackDate[20];
	//char				DeleteDate[20];

	LONGLONG			llAttachMoney;
	LONGLONG			llCharge;

	int					nReqID;			//This ID's scope is valid only for the Sender.

	int					iSender;
	int					iRecver;
	DWORD				dwUserNum;
	int					nUserNumRecver;	//Recver's UserNum

	char				SenderName[CHR_ID_LENGTH +1];
	char				RecverName[CHR_ID_LENGTH +1];

    BOOL				bAttach01;	// Sender attached a item ?
	WORD				xAttach01;	// x-axis in inventory.
	WORD				yAttach01;	// y-axis in inventory.
	WORD				nAttach01;	// item count
	SNATIVEID			idAttach01;	// item id
	
	WORD				wLevel;			// For sp_InsertChaLastInfo()
	LONGLONG			llRestMoney;	// Cha's original money - attachedMoney

	DWORD				dwPostFee;		// Post Fee
};


//
//mjeon.post.get
//
struct SAPOST
{
    enum { 
        SAPOST_CHAR_LENGTH = CHR_ID_LENGTH + 3,
    };

	LONGLONG			m_llPostID;
		
	BYTE				m_byPostState;
	BYTE				m_byPostType;	
	BYTE				m_byPostClass;
	BYTE				m_byChargePayment;

	int					m_iSender;
	int					m_iRecver;
	
	int					m_iDesign;

	char				m_Title[POST_MAX_TITLE_LENGTH];
	char				m_Content[POST_MAX_CONTENT_LENGTH];

    __time64_t          m_SendDate; // CurrentTime
    __time64_t          m_RecvDate;
    __time64_t          m_ReturnDate;
    __time64_t          m_RecvBackDate;
    __time64_t          m_DeleteDate;

	char				m_SenderName[SAPOST_CHAR_LENGTH];	//+3 : to avoid margin
	char				m_RecverName[SAPOST_CHAR_LENGTH];	//+3 : to avoid margin

	LONGLONG			m_llAttachMoney;
	LONGLONG			m_llCharge;
	
	SITEMCUSTOM			m_sAttach01;

	BYTE				m_byMoneyAccept;
	BYTE				m_byAttach01Accept;

    SAPOST()
    {
        m_llPostID = 0;
        m_byPostState = 0;
        m_byPostType = 0;
        m_byPostClass = 0;
        m_byChargePayment = 0;
        m_iSender = 0;
        m_iRecver = 0;
        m_iDesign = 0;

        m_SendDate = 0;
        m_RecvDate = 0;
        m_ReturnDate = 0;
        m_RecvBackDate = 0;
        m_DeleteDate = 0;

        memset(m_Title, 0, POST_MAX_TITLE_LENGTH);
        memset(m_Content, 0, POST_MAX_CONTENT_LENGTH);
        memset(m_SenderName, 0, SAPOST_CHAR_LENGTH); //+3 : to avoid margin
        memset(m_RecverName, 0, SAPOST_CHAR_LENGTH); //+3 : to avoid margin

        m_llAttachMoney = 0;
        m_llCharge = 0;

        m_byMoneyAccept = 0;
        m_byAttach01Accept = 0;
    }

    void SetTitle(const std::string& Data)
    {
        StringCchCopy(m_Title, POST_MAX_TITLE_LENGTH, Data.c_str());
    }

    void SetContent(const std::string& Data)
    {
        StringCchCopy(m_Content, POST_MAX_CONTENT_LENGTH, Data.c_str());
    }

    void SetSenderName(const std::string& Data)
    {
        StringCchCopy(m_SenderName, SAPOST_CHAR_LENGTH, Data.c_str()); //+3 : to avoid margin
    }

    void SetRecverName(const std::string& Data)
    {
        StringCchCopy(m_RecverName, SAPOST_CHAR_LENGTH, Data.c_str()); //+3 : to avoid margin
    }
};


//
//mjeon.post.state
//
struct SAPOSTSTATE
{
    enum { 
        POST_NAME_LENGTH = CHR_ID_LENGTH + 3,
    };

	LONGLONG			llPostID;

	BYTE				byPostState;
	BYTE				byPostType;	
	BYTE				byPostClass;	

	int					iSender;
	int					iRecver;

	int					iDesign;

	LONGLONG			llCharge;

    __time64_t          SendDate;
    __time64_t          RecvDate;
    __time64_t          ReturnDate;
    __time64_t          RecvBackDate;
    __time64_t          DeleteDate;    // Optional

	char				SenderName[POST_NAME_LENGTH];	//+3 : to avoid margin
	char				RecverName[POST_NAME_LENGTH];	//+3 : to avoid margin

    SAPOSTSTATE()
    {
        llPostID = 0;

        byPostState = 0;
        byPostType = 0;
        byPostClass = 0;

        iSender = 0;
        iRecver = 0;

        iDesign = 0;

        llCharge = 0;

        SendDate = 0;
        RecvDate = 0;
        ReturnDate = 0;
        RecvBackDate = 0;
        DeleteDate = 0;    //Optional

        memset(SenderName, 0, POST_NAME_LENGTH);	//+3 : to avoid margin
        memset(RecverName, 0, POST_NAME_LENGTH);	//+3 : to avoid margin
    }

    void SetSenderName(const std::string& Name)
    {
        StringCchCopy(SenderName, POST_NAME_LENGTH, Name.c_str());
    }

    void SetRecverName(const std::string& Name)
    {
        StringCchCopy(RecverName, POST_NAME_LENGTH, Name.c_str());
    }
};


//
//mjeon.sns.facebook.auth
//
struct SFACEBOOK
{	
	int		ChaNum;
	char	SKey[SNS_MAX_SKEY_LENGTH];	
	char	UID[SNS_MAX_UID_LENGTH];

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & ChaNum;
		ar & SKey;
		ar & UID;
	}
};


//
//mjeon.sns.twitter.auth
//
struct STWITTER
{	
	int		ChaNum;
	char	AToken[SNS_MAX_TOKEN_LENGTH];
	char	ATokenS[SNS_MAX_TOKEN_LENGTH];
	char	UID[SNS_MAX_UID_LENGTH];

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & ChaNum;
		ar & AToken;
		ar & ATokenS;
		ar & UID;
	}
};


//
//mjeon.activity
//
struct ACTIVITY_SYNC	//basic-unit of Activity sync between server and client (used in msg)
{
	DWORD	dwID;
	DWORD	dwStatus;
	DWORD	dwValue;
};


//
//mjeon.activity.title
//
struct TITLE_SYNC	//basic-unit of Activity sync between server and client (used in msg)
{
	UINT	nIndex;
	DWORD	dwClass;
	BOOL	bSelected;
	char	szTitle[EM_TITLEID_MAXLENGTH];		//XML-ID of the title
};


#define CTF_MAXSIZE_OF_BUFF_ARRAY_4_GLMSG   10
#define CTF_PLAYER_NUM_LIMIT_PER_SCHOOL		100	//Script���� �����ϴ� �� �п� �� CTF Player�� �ο� ���� �� ���� �Ѿ�� �ȵȴ�.
#define CTF_RANKERS_NUM_4_NOTIFY			10	//CTF���� UI�� ��µǴ� ��Ŀ������ �ִ� ������� �����Ұ��ΰ�?

enum EMCTF_REWARD_BUFF_TYPE
{
	EMCTF_REWARD_BUFF_TYPE_NULL	   = 0,
    EMCTF_REWARD_BUFF_TYPE_PLAYER  = 1,
    EMCTF_REWARD_BUFF_TYPE_SCHOOL  = 2,
	EMCTF_REWARD_BUFF_TYPE_DEFAULT = 3,
};

struct CTF_REWARD_BUFF
{
    DWORD   dwID;
    WORD    wLevel;

	CTF_REWARD_BUFF()
		: dwID(UINT_MAX)
		, wLevel(0)
	{
	}

	CTF_REWARD_BUFF(DWORD _dwID, WORD _wLevel)
		:dwID(_dwID)
		,wLevel(_wLevel)
	{
	}
};

typedef std::vector<CTF_REWARD_BUFF>	VCTF_REWARD_BUFF;
typedef VCTF_REWARD_BUFF::iterator		VCTF_REWARD_BUFF_ITER;

enum ENUM_CTF_JOIN_RESULT
{
	CTF_JOIN_OK				        = 0,
	CTF_JOIN_NO_CHAR		        = 1,	//the character does not exist
	CTF_JOIN_WRONG_CHANNEL	        = 2,	//the character is in another channel
	CTF_JOIN_WRONG_LEVEL	        = 3,	//the character's level is lower than the requirement of CTF	
	CTF_JOIN_WRONG_PVE              = 4,	//the character is in pve instance map
    CTF_JOIN_WRONG_MATCHING_CONTENTS = 5,	//the character is in Matching System Contents;
};



//
//mjeon.db.man
//
typedef std::vector<SAPOST>				VPOST;
typedef VPOST::iterator					VPOST_ITER;

typedef std::vector<SAPOSTSTATE>		VPOSTSTATE;
typedef VPOSTSTATE::iterator			VPOSTSTATE_ITER;

typedef std::vector<LONGLONG>			VPOSTID;
typedef std::vector<LONGLONG>::iterator	ITER_VPOSTID;


#endif // _GLOGIC_DATA_H_