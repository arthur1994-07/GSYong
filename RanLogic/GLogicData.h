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
// Encrypt_RCC RanOnline-TH
namespace GLOGIC
{
	extern BOOL			bGLOGIC_ZIPFILE;
	extern std::string	strGLOGIC_ZIPFILE;
	extern std::string	strQUEST_ZIPFILE;
	extern std::string	strLEVEL_ZIPFILE;
	extern std::string	strNPCTALK_ZIPFILE;

	void SetFullPath( BOOL bPack );
};

enum GLCONST
{
	MAX_SERVERCHAR		= MAX_ONESERVERCHAR_NUM,
	MAX_SELECTCHAR		= 3, // 캐릭터 선택 화면에서 보이는 최대 캐릭터의 수, 현재 3개
	MAX_VIEWRANGE		= 300, // 최대 범위 (파티원 끼리 인접한가?...)

	MAX_HEAD			= 64,
	MAX_HAIR			= 64,

	MAX_SCHOOL			= 10,
	MAX_LEVEL			= 310,

	MAX_CLUBSTORAGE		= 6,
	MAX_CLUBRANK		= 10,

	GRADE_NORMAL		= 4,
	GRADE_HIGH			= 9,
	GRADE_LIMIT_MAX		= 200,

	MAX_SERIALIZED_CHAR_SIZE =	36864,	//36 * 1024
};

enum GLENALBE_NEW_CLASS
{
    RESTRICT_SCIENTIST_NEW_CLASS	= 0, // 과학부, 인술부, 마술부 제한.
    RESTRICT_ASSASSIN_NEW_CLASS	= 1, // 인술부, 마술부 제한.
    RESTRICT_TRICKER_NEW_CLASS	= 2, // 마술부 제한.
	RESTRICT_ACTOR_NEW_CLASS	= 3, // 연극부
    NO_RESTRICT_NEW_CLASS		= 4, // 제한 없음.
};

//	Note : 캐릭터 종류별 기본 상수.
//
struct GLCONST_CHARCLASS
{
	float				fWALKVELO;		//	캐릭의 걷기 속도.
	float				fRUNVELO;		//	캐릭의 뛰기 속도.

	float				fHP_STR;		//	str(체력) -> HP 환산 비율.
	float				fMP_SPI;		//	spi(정신) -> MP 환산 비율.
	float				fSP_STA;		//	sta(근력) -> SP 환산 비율.
	float				fHIT_DEX;		//	dex(민첩) -> HIT rate 환산 비율.
	float				fAVOID_DEX;		//	dex(민첩) -> AVOID rate 환산 비율.
	float				fDEFENSE_DEX;	//	dex(민첩) -> DEFENSE 환산 비율.

	float				fPA_POW;		//	pow(힘) -> PA(격투치) 환산 비율.
	float				fPA_DEX;		//	dex(민첩) -> PA(격투치) 환산 비율.

	float				fSA_POW;		//	pow(힘) -> SA(사격치) 환산 비율.
	float				fSA_DEX;		//	dex(민첩) -> SA(사격치) 환산 비율.

	float				fMA_DEX;		//	dex(민첩) -> MA(마법치) 환산 비율.
	float				fMA_SPI;		//	spi(정신) -> MA(마법치) 환산 비율.
	float				fMA_INT;		//	int(지력) -> MA(마법치) 환산 비율.

	float				fCONV_AP;		//	환산 기본 공격력.
	float				fCONV_DP;		//	환산 기본 방어력.
	float				fCONV_PA;		//	환산 기본 격투치.
	float				fCONV_SA;		//	환산 기본 사격치.

	SCHARSTATS		sBEGIN_STATS;									// 능력치 초기 수치.
	FCHARSTATS			sLVLUP_STATS;									// 레벨 증가 당 기본 능력치 증가 수치.

	WORD				wBEGIN_AP;		//	초기 공격력.
	WORD				wBEGIN_DP;		//	초기 방어력.
	WORD				wBEGIN_PA;		//	초기 격투치.
	WORD				wBEGIN_SA;		//	초기 사격치.

	float				fLVLUP_AP;		//	랩업당 공격력 증가.
	float				fLVLUP_DP;		//	랩업당 방어력 증가.
	float				fLVLUP_PA;		//	랩업당 격투치 증가.
	float				fLVLUP_SA;		//	랩업당 사격치 증가.

	//	에니메이션 정보. ( 시간/스탭 )
	//	server 에서만 필요한 정보.
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
	//	대입 연산이 못일어나게 원천적으로 막혀있음.
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

//	Note : 캐릭터 공통 기본 상수.
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

	extern BOOL			bTESTSERVER;			// 테스트 서버에만 적용.
	extern INT			nUI_KEYBOARD;			// 인터페이스 키보드 설정
	extern INT			nUI_DEFAULTKEYTYPE;		// 기본 키타입
	extern BOOL			bBATTLEROYAL;			// 배틀로얄
	extern INT			nMAX_FRIEND_NUMBER;		// 최대 친구, 차단 목록 개수
    extern INT          nMAX_FRIEND_LIST;       // 추가할 수 있는 친구 목록 수 //2012-07-31 by cwBack
    extern INT          nMAX_FRIEND_GROUP;      // 생성할 수 있는 그룹 수 //2012-09-05 bt cwBack
    extern INT          nMAX_FRIEND_BLOCK;      // 차단할 수 있는 차단 목록 수 //2012-07-31 by cwBack
	extern WORD			wLEVEL_FOR_EXTREME;		// 극강부를 생성하기 위해 만족되어야 하는 레벨(lv.190)
	extern WORD			wSTART_LEVEL_FOR_EXTREME; // 극강부 시작 레벨
	extern WORD			wLEVEL_FOR_ACTOR;		// 변술부 생성 만족 레벨;
    extern BYTE         ExcessExpProcessType;   // 초과 경험치 처리 타입. 0 : 사용안함 1 : 극강부만 2 : 모두

	//	Note : 일반 상수.
	//
	extern float		fFIELD_NEXTPICK;		//	멥에 있는 아이템(돈) 주을때 처음 메시지 전송후 다음 가능 시간.
	extern WORD			wSERVER_NORMAL;
	extern WORD			wSERVER_CONGEST;
	extern WORD			wSERVER_NOVACANCY;

	//---------------------------------------------------------------------------------------[케릭터 기본]
	extern WORD			wSCHOOLNUM;
	extern std::string	strSCHOOLNAME[MAX_SCHOOL];
	extern SNATIVEID	nidSTARTMAP[MAX_SCHOOL];
	extern DWORD		dwSTARTGATE[MAX_SCHOOL];
	extern float		fCHAR_HEIGHT[GLCI_NUM_ACTOR];

    extern WORD         wENABLE_NEW_CLASS;  // 신규 클래스 오픈 제한을 위한 설정 값.

	//---------------------------------------------------------------------------------------[]
	extern BOOL			bPARTY_2OTHERSCHOOL;
	extern BOOL			bCLUB_2OTHERSCHOOL;

    //---------------------------------------------------------------------------------------[채팅 도배방지]
    extern float fCHAT_OVERINPUT_TIMER; // 도배 방지 기능 중 이전 입력 시간과 차이 값(초).
    extern WORD wCHAT_OVERINPUT_CANCEL_TIMER; // 도배 방지 기능 지속시간(초).
    extern int nCHAT_OVERINPUT_COUNT; // 도배 방지 기능 중 이전 입력 문자열 연속 입력 허용 횟수.

    //---------------------------------------------------------------------------------------[부활 창 추천 아이템 기능]
    extern SNATIVEID   nidREBIRTH_RECOMMEND_ITEM;    // 부활 창에서 추천되는 아이템 이미지의 ID.
    extern std::string strREBIRTH_RECOMMEND_ITEM;    // 부활 창에서 추천되어 포인트 상점으로 검색될 아이템의 이름.
    extern WORD wAUTO_REBIRTH_TIMER; // 부활 창에서 자동 부활 시간.

    //---------------------------------------------------------------------------------------[NPC 대화 창 추천 아이템 기능]
    extern SNATIVEID nidDIALOGUE_RECOMMEND_ITEM;    // NPC 대화 창에서 추천되는 아이템 이미지의 ID.
    extern std::string strDIALOGUE_RECOMMEND_ITEM;    // NPC 대화 창에서 추천되어 포인트 상점으로 검색될 아이템의 이름.

    //---------------------------------------------------------------------------------------[자리비움]
    extern float fBECOME_AWAY_TIME; // 입력이 없을 시 자리비움 까지 걸리는 시간(초).
    extern int nAWAY_GESTURE_NUMBER;    // 자리비움 시에 캐릭터가 취할 제스쳐 번호.

    //---------------------------------------------------------------------------------------[바이크 탑승시 적용안하는 효과들]
    extern WORD wNON_APPLY_QBOX_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_QBOX_ON_VEHICLE[QUESTION_SIZE]; // 탈 것 탑승시에 적용 안 할 QBOX.
    extern WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_TYPE_ON_VEHICLE[SKILL::FOR_TYPE_SIZE]; // 탈 것 탑승시에 적용 안 할 스킬 타입들.
    extern WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_IMPACT_ON_VEHICLE[EIMPACTA_SIZE]; // 탈 것 탑승시에 적용 안 할 스킬 효과들.
    extern WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE_NUM;
    extern WORD wNON_APPLY_SKILL_SPEC_ON_VEHICLE[EMSPECA_NSIZE]; // 탈 것 탑승시에 적용 안 할 스킬 특수효과들.

    extern WORD wMaxRebuyList;
    extern float fRebuyMultiplePrice;
    extern WORD wMinRebuyPrice;

	//	extern float		fDISPRICE;				//	아이템 재판매 가격.
	extern WORD			wMAXITEM_AGE;			//	ITEM, 최대 지속시간.
	extern WORD			wMAXITEM_HOLD;			//	ITEM, 임시 소유자 보호 시간.
	extern WORD			wMAXMOB_TRACKING;		//	최대 몹 몰이 수.

	extern WORD			wMAX_LEVEL;				//	레밸 최대 제한.
	extern WORD			wMAX_EXTREME_LEVEL;		//	극강부 레밸 최대 제한.
	extern WORD			wLVL_STATS_P;			//	렙업당 스탯 포인트 부여값.
	extern WORD			wLVL_2ndSTATS_P;			//	렙업당 스탯 포인트 부여값.
	extern float		fLVL_EXP_G;				//	렙업 경험치 산출에 필요한 환산 팩터.
	extern float		fLVL_EXP_S;				//	렙업 경험치 산출에 필요한 환산 팩터.
	extern float		fKILL_EXP_RATE;			//	"총획득 가능 경험치" * 죽일때 경험치.


	extern float		fDIE_DECEXP[DIE_DECEXP_NUM];	//	사망시 경험치 감산 정도.

	extern float		fDIE_RECOVERYEXP[DIE_DECEXP_NUM];	// 경험치 복구 비율
	extern float		fEXP_RATE_MONEY[DIE_DECEXP_NUM];	// 경험치 복구 비용

	extern float		fREC_EXP_RATE;			//	상대방을 회복시켜 줄때 획득하는 경험치의 배율.

	//---------------------------------------------------------------------------------------[연마]

	//extern WORD			wGRADE_MAX;				// 연마 할수 있는 등급 제한치 
	//extern WORD			wGRADE_MAX_REGI;		// 연마 할수 있는 등급 제한치(저항) 

	//extern WORD			wDAMAGE_GRADE;			//	한 등급당 증가 공격력 수치.
	//extern WORD			wDEFENSE_GRADE;			//	한 등급당 증가 방어력 수치.

	//extern float		fDAMAGE_GRADE;			//	한 등급당 증가율 공격력 수치.
	//extern float		fDEFENSE_GRADE;			//	한 등급당 증가율 방어력 수치.

	//extern float		fDAMAGE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];		// 최상위 등급 공격력 증가율
	//extern float		fDEFENSE_GRADE_TOP[GRADE_LIMIT_MAX-GRADE_HIGH];	// 최상위 등급 방어력 증가율
	//extern	WORD		wUSE_GRADE_NUM[GRADE_LIMIT_MAX-GRADE_HIGH];			// 최상위 등급 필요한 연마제 갯수

	//extern WORD			wRESIST_FIRE_GRADE;		//	한 등급당 증가 저항(화) 수치.
	//extern WORD			wRESIST_ICE_GRADE;		//	한 등급당 증가 저항(빙) 수치.
	//extern WORD			wRESIST_ELEC_GRADE;		//	한 등급당 증가 저항(전) 수치.
	//extern WORD			wRESIST_POISON_GRADE;	//	한 등급당 증가 저항(독) 수치.
	//extern WORD			wRESIST_SPIRIT_GRADE;	//	한 등급당 증가 저항(정) 수치.

	//---------------------------------------------------------------------------------------
	extern WORD			wLVL_SKILL_P;			//	렙업당 스킬 포인트 부여값.
	extern WORD			wLVL_2ndSKILL_P;			//	렙업당 스킬 포인트 부여값.

	extern WORD			wMAXATRANGE_SHORT;		//	근접 공격거리.
	extern float		fPUSHPULL_VELO;			//	밀거나 당길때 이동 속도.

	extern int		nHitRateMinimum;			// 최저 명중률;
	extern int		nHitRateMaximum;		    // 최대 명중률;
	extern float	fHitRatePercentUnit;		// 명중력을 백분률로 변환시 적용시킬 단위;
	extern float	fAvoidRatePercentUnit;		// 회피력을 백분률로 변환시 적용실킬 단위;
	extern float	fAttackSuccesRate_LvScale;	// 공격성공률 공식에서 사용되는 레벨 조정 상수값;
    extern float		fMOB_TRACING;			//	몹 추적 거리.
    extern float		fNPC_INTERACTION_RANGE;  //  NPC와 상호작용 할 수 있는 거리;
    extern float		fNPC_INTERACTION_ADJUST; //  NPC와 상호작용을 할때 오차 보정 범위 (상점, 퀘스트, 개조 등은 말 거는 위치보다 조금 넉넉하게 요효성 판정);
	extern float		fLOW_SEED_DAMAGE;		//	damage 들어갈때 최저 수용 damage.
	extern float		fCONFT_SPTY_EXP;		//	학교 대련 승리시에 전체 경험치 분배.

	extern WORD			wBODYRADIUS;			//	캐릭의 몸체 반경.

	// fREACT_VALID_SCALE 기존 범위 보정 비율 값인데 대부분 국가들이 30.0f로 되어 있다;
	// 최종 범위에 곱해지는 값인지라, 범위에 대한 유효성 판정을 안하겠다는 의미로 사용되고 있음;
	extern float		fREACT_VALID_SCALE;		//	액션의 유효 거리 팩터.

	extern float		fATTACK_RANGE_ADJUST_RATE; // 일반 공격 범위 보정 비율;

	extern float		fUNIT_TIME;			    //	회복 단위 시간 (Actor 전체);
	extern float		fHP_INC_PER;			//	캐릭터 단위시간당 HP 회복율(%);
	extern float		fMP_INC_PER;		    //	캐릭터 단위시간당 MP 회복율(%);
	extern float		fSP_INC_PER;		    //	캐릭터 단위시간당 SP 회복율(%);

	extern float		fHP_INC;				//	캐릭터 단위시간당 HP 회복량;
	extern float		fMP_INC;				//	캐릭터 단위시간당 MP 회복량;
	extern float		fSP_INC;				//	캐릭터 단위시간당 SP 회복량;

	extern float		fRESIST_PHYSIC_G;		//	물리 공격시 속성 저항치 적용 비율.
	extern float		fRESIST_G;				//	속성 저항치 적용 비율.
	extern float		fREGIST_MAX_G;		// 속성 저항치 최대 적용 비율;
	extern float		fRESIST_MAX_CROW;		// 속성 저항치 최대 적용 비율(Crow);
	extern WORD			wBASIC_DIS_SP;			//	기본 소모 SP.

	extern float		fLOWSP_MOTION;			//	스태미나 (%) 보다 작을때 모션 처리.
	extern float		fLOWSP_DAMAGE;			//	스태미나 부족시 Damage 감소.
	extern float		fLOWSP_HIT_DROP;		//	스태미나 부족시 명중율 감소.
	extern float		fLOWSP_AVOID_DROP;		//	스태미나 부족시 회피율 감소.
	extern DWORD		dwACCEPT_LOWERSTATS;	//	stats 부족분을 스태미나로 커버할 수 있는 가용 한도. ( 공격 무기에 한함. )

	extern float		fDAMAGE_DEC_RATE;		//	대미지 감소율.
	extern float		fDAMAGE_GRADE_K;			//	(기본공격+연마등급) 대미지 반영율.
	extern float		fDAMAGE_DEC_MAX_G;		// 최대 데미지 흡수율;

	extern DWORD		dwCRITICAL_DAMAGE;		//	크리티컬 데미지 비율
	extern DWORD		dwCRITICAL_MAX;			//	크리티컬 확률 MAX
	extern DWORD		dwCRUSHING_BLOW_DAMAGE;	//  강한타격 데미지 비율
	extern DWORD		dwCRUSHING_BLOW_MAX;	//  강한타격 확률 max
	extern float		fCRUSH_BLOW_RANGE;		//  강한타격시 밀려나는 거리 ( 아이템 ) 	

	extern DWORD		dwReleaseStigmaDistance;	// 낙인 효과 해제 거리(디폴트);

	extern SNATIVEID	sProductMotionID;		// 조합 시 취할 모션 ID
	extern FLOAT		fReferCharDelay;		// 캐릭터 정보보기 요청 딜레이

	//---------------------------------------------------------------------------------------
	extern std::string	sInvenExItemSearch;		// 인벤토리 확장카드 검색어
	extern WORD			wInvenDefaultLine;		// 인벤토리 기본 줄수
	extern WORD			wInvenExtendLine;		// 인벤토리 확장 가능한 최대 줄수(EInvenTotalLine - wInvenDefaultLine - EInvenPremiumLine)
	extern WORD			wInvenExtendEventLine;	// 인벤토리 기본 확장 줄수(캐릭터 생성 시 적용)

	//---------------------------------------------------------------------------------------
	extern float		fTrash_SellCostPer;		// 휴지통 이용비 1순위 (게임머니)
	extern DWORD		wTrash_Cost;			// 휴지통 이용비 2순위 (게임머니)
	extern DWORD		wTrash_Point;			// 휴지통 이용비 3순위 (인게임 캐시)

	extern int			nBrightMAX;				// 퀘스트 보상 성향 최대값
	extern int			nBrightMIN;				// 퀘스트 보상 성향 최소값

	//	Note : ----------------------------------------------------------------대련
	extern float		fCONFRONT_TIME;			//	대련 제한 시간.
	extern float		fCONFRONT_ELAP;			//	연속 대련 방지 지연 시간.
	extern DWORD		dwCONFRONT_MAX;			//	연속 대련 총 가능 횟수.
	extern float		fCONFRONT_RESET;		//	연속 대련 방지 리셋 시간.
	extern float		fCONFRONT_ONE_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )
	extern float		fCONFRONT_PY_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )
	extern float		fCONFRONT_CLB_DIST;		//	대련 거리 제한. ( 0일 경우 무한대. )

	extern int			nCONFRONT_WIN_LP;		//	대련 승리시 living point ( 생활 점수 ).
	extern int			nCONFRONT_LOSS_LP;		//	대련 패배시 living point ( 생활 점수 ).

	extern int			nCONFRONT_CLB_WIN_LP;	//	클럽 대련 승리시 living point
	extern int			nCONFRONT_CLB_LOSS_LP;	//	클럽 대련 패배시 living point

	//	Note : ----------------------------------------------------------------대련
	enum { EMCONFT_RC_TYPENUM = 10, EMCONFT_STATE_TYPENUM = 8 };
	extern WORD			wCONFT_RC_TYPE[EMCONFT_RC_TYPENUM];			//	대련 회복약 사용가능 횟수.
	extern float		fCONFT_STATE_TYPE[EMCONFT_STATE_TYPENUM];	//	대련 체력 확대 비율.

	extern WORD			wCONFRONT_SCHOOL_LIMIT_NUM;					//	학교간 대련 최소 인원.
	extern WORD			wCONFRONT_SCHOOL_LIMIT_LEVEL;				//	학교간 대련 최소 레벨.
	extern float		fCONFRONT_SCHOOL_EXP_SCALE;					//	학교간 대련시 습득 경험치 증가율.

	extern WORD			wCONFRONT_SCHOOL_WIN_NUM;
	extern WORD			wCONFRONT_SCHOOL_WIN_RATE;
	extern float		fCONFRONT_SCHOOL_GENITEM;

	//	Note : ----------------------------------------------------------------pk
	enum { EMPK_STATE_LEVEL = 5, };
	extern BOOL			bPK_MODE;				//	pk on/off.
	extern BOOL			bPKLESS;				//  절대적인 PK on/off.
	extern int			nPK_LIMIT_LEVEL;		//	pk 제한 레벨.
	extern int			nPK_LIMIT_DX_LEVEL;		//	pk 레벨에 따른 제한.
	extern int			nPK_TRY_BRIGHT_POINT;	//	pk 시도시 포인트.
	extern int			nPK_KILL_BRIGHT_POINT;	//	pk 죽였을 경우 포인트.
	extern int			nPK_TRY_LIVING_POINT;	//	pk 시도시 포인트.
	extern int			nPK_KILL_LIVING_POINT;	//	pk 죽였을 경우 포인트.
	extern float		fNONPK_ITEM_DROP;		//	non pk 시 아이템 드롭율.
	extern float		fPK_ITEM_DROP;			//	pk 시 아이템 드롭율.

	//	 ( pk level : 0 ~ 4 )
	//	0 - 일반학생
	//	1 - 불량학생
	//	2 - 살인자
	//	3 - 살인마
	//	4 - 살인귀
	extern DWORD		dwPK_RECALL_ENABLE_LEVEL;	//	귀환 사용 가능 레벨 설정.
	extern DWORD		dwPK_DRUG_ENABLE_LEVEL;		//	회복약 사용 가능 레벨 설정.


	extern float		fPK_JUSTNESS_TIME;		//	반격 가능한 정당방위 시간.
	extern float		fPK_SAFE_TIME;			//	pk 안전 시간.

	extern float		fCDM_SAFE_TIME_ACCRUE[CDM_ACCRUE_COUNT];	//	CDM전용 안전 시간.
	extern float		fCDM_SAFE_TIME_MOVE_SPEED;					//	CDM전용 안전 시간 이동속도 감소량 ( 0.0f ~ 1.0f )

	extern DWORD		dwPK_NORMAL_NAME_COLOR;	//	pk 수치가 없을때 이름 색깔.
	extern float		fPK_POINT_DEC_RATE;		//	pk 수치 감소율. ( 시간당. )
	extern float		fPK_POINT_DEC_PHY;		//  pk 데미지 감소율 ( 물리 ).
	extern float		fPK_POINT_DEC_MAGIC;	//  pk 데미지 감소율 ( 마법 ).

	extern float		fReductionRate_PVP_PHY_P_Damage;			// PVP 감소율 ( 공격력, 격투치 );
	extern float		fReductionRate_PVP_PHY_S_Damage;			// PVP 감소율 ( 공격력, 사격치 );
	extern float		fReductionRate_PVP_MAGIC_Damage;			// PVP 감소율 ( 공격력, 기력치 );
	extern float		fReductionRate_PVP_PHY_Defense;				// PVP 감소율 ( 방어력 );
	extern float		fReductionRate_PVP_MAGIC_Resistance;		// PVP 감소율 ( 저항 );

	extern SPLAYERKILL	sPK_STATE[EMPK_STATE_LEVEL];
    extern int          nNON_PK_CONNECT_BRIGHT_POINT; //! non pk 채널 접속가능 최하 성향점수

	//	Note : ----------------------------------------------------------------상태이상
	enum { EMSTATEBLOW_LEVEL_SIZE = 10, EMSTATEBLOW_LEVEL_BASE = 1, };
	extern int			nSTATEBLOW_LEVEL[EMSTATEBLOW_LEVEL_SIZE];

	//	Note : ----------------------------------------------------------------경험치 Table
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

	//	Note : ----------------------------------------------------------------파티.
	extern float		fPARTYEXPRANGE;				//	경험치 수신이 가능한 최대 거리.
	extern float		fPARTYEXP_S;						//	파티 경험치 분배시 레벨차이에 따른 감소비율.
	extern float		fPartyTenderTime;					// 입찰 제한 시간(파티)
	extern float		fExpeditionTenderTime;				// 입찰 제한 시간(원정대);
	extern float		fUPartyTenderTime;
	extern float		fUExpeditionTenderTime;
	extern float		fPartyTenderRange;					// 입찰 가능 범위(파티);
	extern float		fExpeditionTenderRange;			// 입찰 가능 범위(원정대);	
	extern float		fExpeditionDecreaseExpRate;		// 경험치 감소 비율(원정대);
	extern float		fSwapSlotBlankInterval;				// 자리 이동 활성화시 슬롯 깜박임 시간 간격;
	extern WORD	aPARTYEXP_ONE[MAXPARTY];
	extern WORD	aPARTYKILLEXP_ONE[MAXPARTY];
	extern DWORD	dwPARTY_INVITE_MAX_WAIT_TIME; // 파티 초대시 응답 대기 시간;	
	extern DWORD	dwExpeditionCheckReadyTime;	// 준비 확인 제한 시간(30초); 

	//	Note : ----------------------------------------------------------------클럽
	extern DWORD		dwCLUB_PARTYNUM;			//	클럽 창설시 최소 파티(초기) 인원.
	extern DWORD		dwCLUB_DISSOLUTION_DAY;		//	클럽 해체 유보 일.
	extern DWORD		dwCLUB_JOINBLOCK_DAY;		//	클럽 탈퇴시 가입 제한 일.
	extern DWORD		dwMAX_CLUBRANK;
	extern GLCLUBRANK	sCLUBRANK[MAX_CLUBRANK];	//	클럽 랭크별 상수.
	extern DWORD		dwCLUB_AUTHORITY;			//	클럽마스터 위임 제한 시간			
	extern DWORD		dwCLUB_ALLIANCE_SEC;		//	클럽 동맹 탈퇴, 제명시 재가입 재한시간.
	extern DWORD		dwCLUB_ALLIANCE_DIS;		//	클럽 동맹 해체시 결성 재한시간.
	extern DWORD		dwCLUB_ALLIANCE_NUM;		//	클럽 동맹 최대 숫자.

	extern BOOL 		bCLUB_BATTLE;				//	클럽 배틀 가능 여부
	extern DWORD		dwCLUB_BATTLE_MAX;			//  클럽 배틀 동시 진행 갯수
	extern DWORD		dwCLUB_BATTLE_RANK;			//  클럽 배틀 랭크 제한
	extern DWORD		dwCLUB_BATTLE_TIME;			//  클럽 배틀 시간(분)
	extern DWORD		dwCLUB_BATTLE_TIMEMIN;		//	클럽 배틀 최소시간(분)
	extern DWORD		dwCLUB_BATTLE_TIMEMAX;		//	클럽 배틀 최대시간(분)
	extern DWORD		dwCLUB_BATTLE_GUID_TIME;	//  클럽 배틀 선도전 제약 시간(분)
	extern DWORD		dwCLUB_BATTLE_DIS_TIME;		//  클럽 배틀 휴전 및 항복 가능한 시간(분)
	extern BOOL			bCLUB_BATTLE_ALLIANCE;		//  클럽 배틀시 동맹 자동 참가 기능

	extern BOOL			bCLUB_DEATHMATCH;			//	클럽 데스매치 가능 여부
	extern DWORD		dwCLUB_DEATHMATCH_MEM;		//	클럽 데스매치 가능 멤버수
	extern DWORD		dwCLUB_DEATHMATCH_RANK;		//	클럽 데스매치 가능 랭크

    extern DWORD        dwCLUB_CHANGE_CLUB_MARK_LIMIT_TIME; // 클럽 마크 변경 제한 시간(초).

	extern DWORD		dwClubMarkARGB;				// 클럽 마크 RGB;
	extern std::vector< DWORD > vecClubMarkARGB;

	//	Note : ----------------------------------------------------------------선도전
	extern float		fMAX_COMMISSION;			//	최대 선도 지역 수수료 제한.
	extern float		fDEFAULT_COMMISSION;		//	비 선도 지역의 수수료.
	extern float		fEARNING_RATE;				//	수수료 수익율.
	extern float		fEARNING_RATE_NPC;			//	NPC 소환 수수료 수익율.
	extern float		fCDCERTIFY_DIST;			//	인증 가능 거리.
	extern float		fCDCERTIFY_DIST2;			//	인증 가능 거리.
	extern float		fCDCERTIFY_TIME;			//	인증 소요 시간.

	//	Note : ----------------------------------------------------------------배율 조정
	extern float		fEXP_SCALE;					//	Note : 경험치 배율.
    extern float		fITEM_DROP_SCALE;			//	Note : 아이탬 드롭율.
    extern float		fITEM_DROP_LOWER_SCALE;		//	Note : 희귀 아이탬 드롭율.
	extern float		fMONEY_DROP_SCALE;			//	Note : 돈 드롭율.

    extern float        fITEM_DROP_SPECIFIC_GRADE_SCALE; //  Note : 특정 아이템 등급 드롭율.(등급은 genitem에서 설정)
    extern WORD         wITEM_DROP_SPECIFIC_GRADE;       //  Note : 드롭율을 증가시킬 특정 아이템의 등급


	//	Note : ---------------------------------------------------------------- 이벤트 조정용 배율
	extern float		fEVENT_EXP_SCALE;					//	Note : 이벤트 경험치 배율.
	extern float		fEVENT_ITEM_DROP_SCALE;			//	Note : 이벤트 아이탬 드롭율.
	extern float		fEVENT_MONEY_DROP_SCALE;			//	Note : 이벤트 돈 드롭율.

	//	Note : ----------------------------------------------------------------학원 프리 pk
	extern bool			bSCHOOL_FREE_PK_ALWAYS;		//	학원간 프리 pk 항상 가능 여부.
	extern bool			bSCHOOL_FREE_PK;			//	학원간 프리 pk on/off.
	extern bool			bSCHOOL_FREE_PK_TIME_REAL;	//	학원간 프리 pk 설정 시간 ( on : 실제 시간, off : 게임시간 )
	extern bool			bSCHOOL_FREE_PK_Z_FORCED;	//	학원간 프리 pk 강제 공격.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_START;//	학원간 프리 pk 시작 시간.
	extern DWORD		dwSCHOOL_FREE_PK_TIME_TERM;	//	학원간 프리 pk 지속 시간.
	extern float		fSCHOOL_FREE_PK_ITEM_DROP;	//	학원간 프리 pk 아이템 드롭율 가산값. ( 기본 드롭율 + 가산값. )
	//	Note : ----------------------------------------------------------------

	//	Note : ----------------------------------------------------------------기능 설정.
	//extern bool			bENCHANT_TERMINATE_ITEM;	//	인첸트시 아이템 파손 가능.
	extern bool			bMONEY_DROP2FIELD;			//	돈 바닥에 버리기 가능 여부.

	//	Note : ----------------------------------------------------------------연마 확율.
	//extern float		fGRADE_RATE[GRADE_LIMIT_MAX];
	//extern float		fGRADE_RESET_RATE[GRADE_LIMIT_MAX];
	//extern float		fGRADE_TERMINATE_RATE[GRADE_LIMIT_MAX];
	//extern float        fGRADE_ITEMOPTION_INC_GRINDINGRATE[GRADE_LIMIT_MAX];  // 강화주 증가 확률
	//extern int			nGRADE_ITEMOPTION_ANTIRESET[GRADE_LIMIT_MAX];		  // 완화주 리셋 감소 단계 
	extern float		fGRADE_EVENT_RATE;


	//	Note : ---------------------------------------------------------------- 조합(제조) 관련 hsshin
	extern bool			bUnableProductType[Product::ETypeTotal];
	extern bool			bPublicStudyProductType[Product::ETypeTotal];
	extern DWORD		dwStudyPointDefault;

	//	Note : ---------------------------------------------------------------- 채팅 관련
	extern bool			bCHAT_EXTEND;					// 지역, 파티모집 활성화 여부
	extern float		dwCHAT_AREA_CHARGE;				// 지역 채팅 요금
	extern float		dwCHAT_PARTY_RECRUIT_CHARGE;	// 파티 모집 요금
	extern float		fCHAT_PARTY_RECRUIT_TIME;		// 파티 모집 갱신 시간

	//	Note : ----------------------------------------------------------------사운드
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

	//	Note : ----------------------------------------------------------------효과
	extern std::string	strSELECT_CHAR;			//	로비에서 케릭터 선택시.

	extern std::string	strREBIRTH_EFFECT;		//	캐릭 부활 이팩트.
	extern std::string	strLEVELUP_EFFECT;		//	래밸 업 이팩트.

	extern std::string	strSRIKE_ELMT_EFFECT[EMELEMENT_MAXNUM]; //	타격시 이팩트.
	extern std::string	strSTRIKE_ARROW_EFFECT;					// 화살 투사체 이펙트;
	extern std::string	strSTRIKE_BULLET_EFFECT;				// 총알 투사체 이펙트;

	extern std::string	strAMBIENT_EFFECT;		//	타격시 Ambient 이펙트
	extern std::string	strERASE_EFFECT;		//	몹 사라지는 이팩트.

	extern std::string	strERASE_SUMMON_EFFECT;	 //  소환수 사라지는 이펙트.
	extern float		fERASE_SUMMON_BEGIN_TIME; // 소환수 지워지기 시작하는 시간 ( EndTime - fERASE_SUMMON_BEGIN_TIME 부터 )

	extern std::string	strSKILL_LEARN_EFFECT;	//	스킬 습득시.
	extern std::string	strSKILL_UP_EFFECT;		//	스킬 업.

	extern std::string strQUEST_START;			//	퀘스트 시작.
	extern std::string strQUEST_END_SUCCEED;	//	퀘스트 성공.
	extern std::string strQUEST_END_FAIL;		//	퀘스트 실패.

	extern std::string	strMOB_GEM_EFFECT;		//	몹 생성시 이팩트.
	extern std::string	strMOB_DIE_BLOOD;		//	몹이 죽었는지.

	// PET
	extern std::string	strPET_GEN_EFFECT;		// 팻 생성시 이팩트

	// Vehicle
	extern std::string  strVEHICLE_GEN_EFFECT;	// 탑승 이펙트

	extern std::string	strCONFRONT_BOUND;		//	대련 경계선 표시 효과.

	extern std::string	strHALFALPHA_EFFECT;	//	투명 케릭터.

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

    extern std::string strCRUSHING_BLOW_EFFECT;		// 강한타격 이펙트
    extern std::string strAUTH_EFFECT;              // 인증 공통 인증 이펙트
    extern std::string strAUTH_SM_EFFECT;		    // 성문학원 인증 이펙트
    extern std::string strAUTH_HA_EFFECT;		    // 현암학원 인증 이펙트
    extern std::string strAUTH_BH_EFFECT;		    // 봉황학원 인증 이펙트

	extern std::string strCOMPLETE_NPC_EFFECT;		// 완료 퀘스트 이펙트

	extern std::string strCLICK_EFFECT;				// 이동 예정 위치 표시 이펙트
	extern std::string strMINE_EFFECT;				// 자기 자신이 소유가능한 아이템 이펙트
	extern std::string strCURSOR_TARGET_EFFECT;		// 커서 오버 대상 이펙트
	extern std::string strTARGET_EFFECT;			// 선택 대상 이펙트
	extern std::string strTARGET_EFFECT_ENEMY;		// 선택 대상 이펙트(적)
	extern std::string strTARGET_EFFECT_NPC;		// 선택 대상 이펙트(npc)
	extern std::string strTARGET_EFFECT_ALLIANCE;	// 선택 대상 이펙트(아군)
	extern std::string strARROW_EFFECT_ENEMY;		// 화살표 이펙트(적)
	extern std::string strARROW_EFFECT_NPC;			// 화살표 이펙트(npc)
	extern std::string strARROW_EFFECT_ALLIANCE;	// 화살표 이펙트(아군)
	extern std::string strSCOPE_TEXTURE_CIRCLE;		// 범위 이펙트 (원)
	extern std::string strSCOPE_TEXTURE_FANWISE;	// 범위 이펙트 (부채꼴)
	extern std::string strBattleIcon;				// 적대상태 아이콘
	extern SNATIVEID sBattleIconIndex;				// 적대상태 아이콘 인덱스.

	extern std::string strCA_EFFECT;				// 반격

    extern std::string strCOMPLATE_ACTIVITY_EFFECT; //  Activity 완료

	extern GLCONST_CHARCLASS	cCONSTCLASS[GLCI_NUM_ACTOR];	//	캐릭터 클래스별 상수.

	extern std::string strCLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR];
	extern std::string strJUMPING_CLASS_INIT[MAX_SCHOOL][GLCI_NUM_ACTOR];

	extern std::vector<std::string>	vecGUIDANCE_FILE;
	extern std::vector<std::string>	vecClubDM_FILE;

	extern WORD	wPK_KILL_Combat_Point;	//	CP 추가 포인트
	extern BOOL	bCOMBAT_POINT;			//	CP 사용 여부
	extern float fIllusionInterval;

    //
    // 내구도
    extern bool     bDurabilityEmsuit[SUIT_NSIZE];  // 착용 장비, 무기의 착용위치 정보 값 	( 특정 장비(EMSUIT) 부위에 내구도 시스템을 적용 여부  )
    extern WORD     wDurabilityDropDeathPvE;        // 사망하면 내구도가 감소합니다. (몹)
    extern WORD     wDurabilityDropDeathPvP;        // 사망하면 내구도가 감소합니다. (사람)
    extern DWORD    dwDurabilityCountLimit;         // 내구도는 카운팅수가 만족하면 1 감소합니다. (ex 카운팅 1000만 == 내구도 1감소 )
    extern DWORD    dwDurabilityDropSkillUse;       // 스킬을 사용하면 증가하는 카운트
    extern DWORD    dwDurabilityDropHitPvE;	        // 대미지 1Point당 증가하는 카운트 (몹)
    extern DWORD    dwDurabilityDropHitPvP;	        // 대미지 1Point당 증가하는 카운트 (사람)
    extern int      nDurabilityRateTable[DURABILITY_RATE_COUNT];        // 내구도 비율에 따른 내구도 감소율
    extern int      nDurabilityRatePerformance[DURABILITY_RATE_COUNT];  // 내구도 감소에 따른 아이템 능력치 비율

	extern int      nWPowDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWDexDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWSpiDurRatePerformance[DURABILITY_RATE_COUNT];  
	extern int      nWMixDurRatePerformance[DURABILITY_RATE_COUNT];  

// #ifdef _RELEASED
	extern BYTE		nDurabilityApply[DURABILITY_APPLY_COUNT];
	extern float	fDurabilityAlarmRemainPer;		// 일정 비율 이하시 퀵메뉴에 알리기위한 부스트비율 설정, 퀵메뉴에서 알린다;
// #endif

	//! 부가능력 표시 여부
	extern bool		bAdditional_ability;

	//! 캐릭터 클래스별 초기 설정값.
	extern char	szCharSkin[GLCI_NUM_ACTOR][MAX_PATH];	//	캐릭별 초기 형상.

	//	Note : 캐릭터 단순화 복장
	//
	extern char					szManSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH];	//	캐릭별 단순화 남자 복장 형상. 
	extern char					szWomenSimpleSkin[EM_SIMPLESKIN_NUM][MAX_PATH];	//	캐릭별 단순화 여자 복장 형상.
	extern std::string			strCharBoard[GLCI_NUM_ACTOR];	//	캐릭터별 단순화 빌보드 이미지	
	extern std::string			strItemLevelIcon[EMITEMLEVEL_NUM];

    extern std::string strIPEventFileName;
	extern std::string strQuestGroupFileName;

	extern std::vector<DWORD>	vecCTFNotify;

	extern float fCTF_BECOME_AWAY_TIME; // CTF 에서 입력이 없을 시 자리비움 까지 걸리는 시간(초).

	extern float fPremiumMapExitCheckTime;							// 프리미엄 맵 강제퇴장 체크 시간 (초)
	extern std::vector< DWORD > vecPremiumMapForcedExitNotify;		// 프리미엄 맵 강제퇴장 남은 시간 알림

	extern LONGLONG NotifyMoney;	// 게임 머니 변화량 알림 기준
	extern int nDecomposeCost;		// 분해시 비용;
	extern float fQBoxPickableTime;	// Q박스 줍기 시간;
	extern float fMacro_TermsofClose_LimitTime; // 남은시간에 따른 매크로 종료조건;
	extern int nMacro_TermsofClose_Potion; // 남은 물약에 따른 매크로 종료조건;
	extern float fMacro_MaxFillTime; // 물약으로 채울수있는 매크로 시간(단위-초);

	extern std::string strScreenEffect[SCREEN_EFFECT_NUM];
	extern std::string strCameraEffect[CAMERA_EFFECT_NUM];
	extern std::string strOPItemIcon;
	extern std::vector< SSTATPOINT_BYLEVEL > vecMaxStatPoint;
	extern std::vector< SSTATPOINT_BYLEVEL > vecExtremeMaxStatPoint;
	extern std::vector<INT> vecMacroRemainTimeAlarm;

	extern float	fIgnore_BossDamageMax;				// 보스몹에게 피격시 데미지 차감;
	extern int		nBossFixedDamageMax;				// 보스몹 공격시 추가 고정 데미지;
	extern float	fIncRCriticalMax_EmergencyMob;		// 위급 상태의 몹에게 크리티컬 발생률 증가;
	extern float	fIncRCriticalMax_EmergencyBoss;		// 위급 상태의 보스에게 크리티컬 발생률 증가;
	extern float	fDecRDamageMax_EmergencyState;		// 위급 상태일 경우 데미지 감소;

	// 긴급상태 전환 HP Rate;
	extern float	fEmergency_CharHPR;
	extern float	fEmergency_MobHPR;
	extern float	fEmergency_BossHPR;
	// 긴급 상태시 초당 깜빡임 횟수;
	extern int		nEmergency_BlinkCount;

	extern float	fIncRExpAddEffectMax;				// 경험치 증가율 효과'를 증가시킴;
	extern float	fIncRDropGenMoneyMax;				// 드랍머니 증가;
	extern float	fDecRDamageMax_MeleeSkill;			// 물리 근접 스킬 피격시 경우 데미지 감소;
	extern float	fDecRDamageMax_RangeSkill;			// 사격 스킬 피격시 경우 데미지 감소;
	extern float	fDecRDamageMax_MagicSkill;			// 마법 스킬 피격시 경우 데미지 감소;
	extern int		nInc_HPMax_SoloPlay;				// 솔로플레이시 HP증가 (파티가 아닐시);
	extern int		nInc_HPMax_PartyPlay;				// 파티플레이시 HP증가;
	extern float	fIncR_AtkSpeedMax_SoloPlay;			// 솔로플레이시 공속증가 (파티가 아닐시);
	extern float	fIncR_AtkSpeedMax_PartyPlay;		// 파티플레이시 공속증가;
	
	extern float	fInc_RandomOp_SkillDurationAddTimeMax;	// 스킬 등급별 지속시간 최대 증가값;
	extern float	fIncR_RandomOp_SkillDamageAddMax;		// 스킬 등급별 데미지 최대 증가율;
	extern float	fDecR_SkillDelayTMax;					// 스킬 등급별 딜레이 최대 감소값;
	extern float	fIncR_CriticalDamageMax;				// 크리티컬 데미지 최대 증가율;
	extern float	fIncR_CrushingBlowMax;					// 강한타격 데미지 최대 증가율;
	extern float	fIncR_LifePerKillMax;					// 처치시 생명력 최대 회복율;
	extern float	fIncR_MPPerKillMax;						// 처치시 MP 최대 회복율;
	extern float	fIncR_SPPerKillMax;						// 처치시 HP+MP+SP 최대 회복율;
	extern float	fIncR_HPMPSPPerKillMax;					// 처치시 SP 최대 회복율;
	extern float	fIncR_AllMPRatioToAttackMax;				// 전체 MP 비율만큼 공격력증가Max;
	extern float	fIncR_AllSPRatioToHpMax;					// 전체 SP 비율만큼 체력증가Max;
	extern float	fIncR_AllHitRatioToMeleeMax;				// 전체 명중 비율만큼 격투치 증가Max;
	extern float	fIncR_AllHitRatioToRangeMax;				// 전체 명중 비율만큼 사격치 증가Max;
	extern float	fIncR_AllHitRatioToMagicMax;				// 전체 명중 비율만큼 기력치 증가;

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

	void SET_EVENT_ITEM_SCALE ( float fScale );		//	이벤트 배율 설정
	float GET_ITEM_SCALE ();						//	데이터 * 이벤트 배율

	void SET_EVENT_MONEY_SCALE ( float fScale );	//	이벤트 배율 설정
	float GET_MONEY_SCALE ( const float fAddScale = 0.0f );						//	데이터 * 이벤트 배율

	void SET_EVENT_EXP_SCALE ( float fScale );		//	이벤트 배율 설정
	float GET_EXP_SCALE ();							//	데이터 * 이벤트 배율

	DWORD GETEXP ( int nAttackerLev, int nDefenserLev );
	float GETEXP_RATE ( int nAttackerLev, int nDefenserLev );

	DWORD GETEXP_PC ( WORD wLev );

    //! non pk 채널 접속가능 최하 성향점수
    int GetNonPkConnectBrightPoint();
	// 해당 스킬의 사용 클래스 정보를 얻음;
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

	extern int				nFullDecrVehicle[VEHICLE_TYPE_SIZE]; // 포만감 감소치

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

namespace MINIGAME_ODDEVEN // 미니 게임, 홀짝 게임
{
	const unsigned int MAX_ROUND = 5;

	extern float	fReturnRate[MAX_ROUND];		// 배당금 배율 인수(최종배당금 = 원금*1회차인수*2회차인수...)
	extern float	fSuccessRate[MAX_ROUND];	// 성공 활률
	extern float	fTimeLimit;					// 제한 시간(초)
	extern float	fShuffleTime;				// 섞임 시간(초)
	extern UINT		uiMaxBattingMoney;			// 최대 배팅 가능 금액
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
    extern DWORD dwDefault;					// 기본 컬러.
    extern DWORD dwItemRank[EMITEM_LEVEL_NSIZE];	// 무기 등급에 따른 컬러.
    extern DWORD dwReqWrong; 				// 착용 오류 컬러.
    extern DWORD dwInchant1; 				// 기본 공격력 + 연마 합산 컬러.
    extern DWORD dwInchant2; 				// 연마 증가 컬러.
    extern DWORD dwEquipCostum;				// 코스튬 및 염색 가능 정보 컬러.
    extern DWORD dwRankUp1; 				// 기본 능력치 + 연마 합산 컬러.
    extern DWORD dwRankUp2; 				// 연마 증가 컬러.
    extern DWORD dwSPReduce;				// SP 추가 소모 컬러.
    extern DWORD dwAddAbility;				// 가산 효과 사용 컬러.
    extern DWORD dwRandomAbility;			// 랜덤 옵션 사용 컬러.
    extern DWORD dwElementalAbility;		// 상태이상 사용 컬러.
    extern DWORD dwTrade;					// 거래 및 교환 불가 사용 컬러.
    extern DWORD dwLimitTime;				// 아이템 유효기간 사용 컬러.
	extern DWORD dwAddDisguise;				// 코스튬 부착에 의한 가산 옵션 컬러.
	extern DWORD dwMoney;					// 골드 텍스트 컬러.
	extern DWORD dwSetItem;					// 셋트 아이템 컬러.

	//////////////////////////////////////////////////////////////////////////

	// 연마에 관한 색깔
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
    extern DWORD dwDefault; // 기본 컬러.
    extern DWORD dwSkillName; // 스킬 이름 사용 컬러.
    extern DWORD dwSkillGrade; // 스킬 등급 사용 컬러.
    extern DWORD dwSkillType; // 스킬 타입 사용 컬러.(일반형, 패시브형 출력)
    extern DWORD dwSkillElemental; // 스킬 속성 사용 컬러.
    extern DWORD dwReqWrong; // 스킬 습등 조건 오류 사용 컬러.
    extern DWORD dwSkillRange; // 스킬 적용 영역 사용 컬러.
    extern DWORD dwAddAbility; // 스킬 효과 적용 사용 컬러.
    extern DWORD dwElementalAbility; // 스킬 상태이상 사용 컬러.
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
#define CTF_PLAYER_NUM_LIMIT_PER_SCHOOL		100	//Script에서 설정하는 각 학원 별 CTF Player의 인원 수는 이 값을 넘어서는 안된다.
#define CTF_RANKERS_NUM_4_NOTIFY			10	//CTF통합 UI에 출력되는 랭커정보를 최대 몇명까지 전송할것인가?

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
