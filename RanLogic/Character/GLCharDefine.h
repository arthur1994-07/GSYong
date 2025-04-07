#pragma once

#include <hash_set>
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../Item/GLITEMLMT.h"

#define _SKILLFACT_DEFAULTTIME		  (-1.0f)
#define _SKILLFACT_INFINITY		      (FLT_MAX)

#define _SKILLFACT_DEFAULTCUSTOMCOUNT (0)

namespace sc
{
    class Excel;
}

enum GAIN_TYPE
{ 
    GAINTYPE_MAX,
    GAINTYPE_HALF,
    GAINTYPE_EMPTY,
};

//static const는 리터럴이다.
// MAP
static const int    MAP_NAME_LENGTH = 50;


// SMS
static const int    SMS_DEFAULT     = 0;
static const int    SMS_LENGTH      = 80; // SMS 의 전체길이 제한.
static const int    SMS_SENDER      = 14; // SMS 보내는 사람 길이.
static const int    SMS_RECEIVER    = 14;  // SMS 받는 사람 길이.


// CHARDEFINE
static const DWORD  GAEAID_NULL     = 0xFFFFFFFF;
static const int    CLUB_NULL       = 0;
static const int    PARTY_NULL      = 0;
static const int    EXPEDITION_NULL = 0;
static const DWORD  DBNUM_NULL      = GAEAID_NULL;
static const int    CHAR_SZNOTE     = 160;
static const int    CHAR_MAXSKILL   = 32;


// CLUB
static const int    EMCLUB_MARK_SX	        = 16; // 클럽마크의 폭 16 픽셀
static const int    EMCLUB_MARK_SY	        = 11; // 클럽마크의 높이 11 픽셀
static const int    EMCLUB_MARK_SIZE        = sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY;
static const int    EMCLUB_NOTICE_LEN_LINE	= 40;
static const int    EMCLUB_NOTICE_LINE		= 10;
// 클럽 공지의 글자수 제한 ( 글자수 * 라인 + ( 라인개행표시 )  ) 								   
static const int    EMCLUB_NOTICE_LEN		=  ( EMCLUB_NOTICE_LEN_LINE * EMCLUB_NOTICE_LINE ) + (( EMCLUB_NOTICE_LINE - 1 ) * 2 );


// LAND
static const int    MAXITEM			= 3000;		//	SUIT, 멥상에 존재 할수 있는 최대 갯수.
static const int    MAXMONEY		= 3000;		//	MONEY, 멥상에 존재 할수 있는 최대 갯수.
static const int    MAXCROW			= 3000;		//	CROW, 멥상에 존재 할수 있는 최대 갯수.
static const int    MAXPET			= 3000;		//  PET, 맵상에 존재 할수 있는 최대 갯수.
static const int    MAXSUMMON		= 3000;		//	SUMMON, 멥상에 존재 할수 있는 최대 갯수.
static const int    MAXLANDEVENT    = 3000;     //  LANDEVENT, 맵상에 존재 할수 있는 최대 갯수

// Max;
static const int	MAXOFFENCE		= 99999;
static const int	MAXDEFENCE		= 9999;
static const int	MAXREGIST		= 999;
static const int	MAXACCURACY		= 999;
static const int	MAXAVOIDANCE	= 999;

//! 캐릭터 인덱스
enum EMCHARINDEX
{
	GLCI_FIGHTER_M		= 0,
	GLCI_ARMS_M			= 1,
	GLCI_ARCHER_W		= 2,
	GLCI_SPIRIT_W		= 3,

	GLCI_EXTREME_M		= 4,	
	GLCI_EXTREME_W	= 5,

	GLCI_FIGHTER_W		= 6,
	GLCI_ARMS_W			= 7,
	GLCI_ARCHER_M		= 8,
	GLCI_SPIRIT_M		= 9,

	GLCI_SCIENTIST_M	= 10,
	GLCI_SCIENTIST_W	= 11,

	GLCI_ASSASSIN_M	= 12,
	GLCI_ASSASSIN_W	= 13,

	GLCI_TRICKER_M		= 14,
	GLCI_TRICKER_W		= 15,

	GLCI_ETC_M			= 16,
	GLCI_ETC_W			= 17,

	GLCI_ACTOR_M		= 18,
	GLCI_ACTOR_W		= 19,

	GLCI_NUM					= 4,
	GLCI_NUM_EX				= 6,
	GLCI_NUM_NEWSEX		= 10,
	GLCI_NUM_SCIENTIST	= 12,
	GLCI_NUM_ASSASSIN	= 14,
	GLCI_NUM_TRICKER		= 16,
	GLCI_NUM_ETC			= 18,
	GLCI_NUM_ACTOR			= 20,
};

//! 캐릭터 종류 FLAG (수정시 WidgetDef.lua 파일도 같이 수정)
enum EMCHARCLASS
{
	GLCC_NONE			= 0X00000000,	//	추가, 아무것도 선택되지 않은 상태

	GLCC_FIGHTER_M		= 0x00000001,	// GLCC_FIGHTER_M으로 CHANGE...
	GLCC_ARMS_M			= 0x00000002,	// GLCC_ARMS_M으로 CHANGE...
	GLCC_ARCHER_W		= 0x00000004,	// GLCC_ARCHER_W으로 CHANGE...
	GLCC_SPIRIT_W		= 0x00000008,	// GLCC_SPIRIT_W으로 CHANGE...

	GLCC_EXTREME_M		= 0x00000010,   // 극강부 남
	GLCC_EXTREME_W		= 0x00000020,   // 극강부 여

	GLCC_FIGHTER_W		= 0x00000040,
	GLCC_ARMS_W			= 0x00000080,
	GLCC_ARCHER_M		= 0x00000100,
	GLCC_SPIRIT_M		= 0x00000200,

	GLCC_SCIENTIST_M	= 0x00000400,	//	과학부 남
	GLCC_SCIENTIST_W	= 0x00000800,	//	과학부 여

	GLCC_ASSASSIN_M		= 0x00001000,
	GLCC_ASSASSIN_W		= 0x00002000,

	GLCC_TRICKER_M		= 0x00004000,
	GLCC_TRICKER_W		= 0x00008000,

	GLCC_ETC_M			= 0x00010000,
	GLCC_ETC_W			= 0x00020000,

	GLCC_ACTOR_M		= 0x00040000,
	GLCC_ACTOR_W		= 0x00080000,

	GLCC_FIGHTER		= ( GLCC_FIGHTER_M | GLCC_FIGHTER_W ),
	GLCC_ARMS			= ( GLCC_ARMS_M | GLCC_ARMS_W ),
	GLCC_ARCHER			= ( GLCC_ARCHER_M | GLCC_ARCHER_W ),
	GLCC_SPIRIT			= ( GLCC_SPIRIT_M | GLCC_SPIRIT_W ),
	GLCC_EXTREME		= ( GLCC_EXTREME_M | GLCC_EXTREME_W ),
	GLCC_SCIENTIST		= ( GLCC_SCIENTIST_M | GLCC_SCIENTIST_W ),
	GLCC_ASSASSIN		= ( GLCC_ASSASSIN_M | GLCC_ASSASSIN_W ),
	GLCC_TRICKER		= ( GLCC_TRICKER_M | GLCC_TRICKER_W ),
	GLCC_ETC			= ( GLCC_ETC_M | GLCC_ETC_W ),
	GLCC_ACTOR			= ( GLCC_ACTOR_M | GLCC_ACTOR_W ),

	GLCC_ALL			= (GLCC_FIGHTER | GLCC_ARMS | GLCC_ARCHER | GLCC_SPIRIT),
	GLCC_ALL_OLD_EX		= (GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME),
	GLCC_ALL_NEWSEX		= (GLCC_ALL | GLCC_EXTREME),

	GLCC_ALL_SCIENTIST	= ( GLCC_ALL_NEWSEX | GLCC_SCIENTIST ),
	GLCC_ALL_ASSASSIN	= ( GLCC_ALL_SCIENTIST | GLCC_ASSASSIN ),
	GLCC_ALL_TRICKER	= ( GLCC_ALL_ASSASSIN | GLCC_TRICKER ),
	GLCC_ALL_ACTOR		= ( GLCC_ALL_TRICKER | GLCC_ACTOR ),

	GLCC_MAN			= (GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M | GLCC_EXTREME_M | GLCC_SCIENTIST_M | GLCC_ASSASSIN_M | GLCC_TRICKER_M | GLCC_ETC_M | GLCC_ACTOR_M),
	GLCC_WOMAN			= (GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME_W | GLCC_SCIENTIST_W | GLCC_ASSASSIN_W | GLCC_TRICKER_W | GLCC_ETC_W | GLCC_ACTOR_W),

	GLCC_TOOL_MAN		= (GLCC_FIGHTER_M | GLCC_ARMS_M | GLCC_ARCHER_M | GLCC_SPIRIT_M | GLCC_EXTREME_M | GLCC_SCIENTIST_M | GLCC_ASSASSIN_M | GLCC_TRICKER_M | GLCC_ACTOR_M),
	GLCC_TOOL_WOMAN		= (GLCC_FIGHTER_W | GLCC_ARMS_W | GLCC_ARCHER_W | GLCC_SPIRIT_W | GLCC_EXTREME_W | GLCC_SCIENTIST_W | GLCC_ASSASSIN_W | GLCC_TRICKER_W | GLCC_ACTOR_W),
};
typedef EMCHARCLASS*	PEMCHARCLASS;

void SaveEmCharClassExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName);

//
//mjeon.CaptureTheField
//
enum ENUM_SCHOOL
{    
	SCHOOL_SM		= 0,
    SCHOOL_HA		= 1,
    SCHOOL_BH		= 2,	
	SCHOOL_NUM		= 3,
	SCHOOL_NONE		= 4,
};

enum EMSCHOOLFLAG
{
	GLSCHOOL_00		= 0x1,
	GLSCHOOL_01		= 0x2,
	GLSCHOOL_02		= 0x4,

	GLSCHOOL_NUM	= 3,
	GLSCHOOL_ALL	= (GLSCHOOL_00|GLSCHOOL_01|GLSCHOOL_02),
};
WORD school2index(const EMSCHOOLFLAG emSchool);
EMSCHOOLFLAG index2school(const WORD wSchool);
void SaveEmSchoolFlagExcel(sc::Excel& Xls, int SheetNum, const std::string& SheetName);

enum EMCLASSINDEX
{
	GLCL_FIGHTER		= 0, // 격투부;
	GLCL_ARMS			= 1, // 검도부;
	GLCL_ARCHER		= 2, // 양궁부;
	GLCL_SPIRIT		= 3, // 기예부;
	GLCL_EXTREME	= 4, // 극강부;
	GLCL_SCIENTIST	= 5, // 과학부;
    GLCL_ASSASSIN	= 6, // 인술부;
	GLCL_TRICKER		= 7, // 마술부;
    GLCL_ETC			= 8, // 기타;
	GLCL_ACTOR		= 9,
	GLCL_NUM_CLASS	= 10, // 클래스 갯수
};

EMCLASSINDEX CharClassToClassIndex ( const EMCHARCLASS emClass );
EMCHARINDEX CharClassToIndex( const EMCHARCLASS emClass );
EMCHARCLASS CharIndexToClass( const EMCHARINDEX emIndex );
EMCHARCLASS CharClassToSex( const EMCHARCLASS emClass );
EMCHARINDEX CharIndexToSex( const EMCHARINDEX emIndex );
EMCHARCLASS CharSexandClassIndexToCharClass( const int nSex, 
											 const EMCLASSINDEX emclassIndex );

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex );

//	Note : 캐릭터 단순화 복장 인덱스.
//

enum EMCHAR_SIMPLESKIN
{
	EM_SIMPLESKIN_CAP	= 0,
	EM_SIMPLESKIN_BODY	= 1,	
	EM_SIMPLESKIN_LEG	= 2,
	EM_SIMPLESKIN_FOOT	= 3,
	EM_SIMPLESKIN_NUM	= 4,
	

};

enum EMSTATS
{
	EMPOW	= 0,
	EMSTR	= 1,

	EMSPI	= 2,
	EMDEX	= 3,

	EMINT	= 4,
	EMSTA	= 5,

	EMSIZE	= 6,
};

struct FCHARSTATS
{
	float		fPow;	//	힘.
	float		fStr;	//	체력.
	float		fSpi;	//	정신.
	float		fDex;	//	민첩.
	float		fInt;	//	지력.
	float		fSta;	//	근력.

    MSGPACK_DEFINE(fPow, fStr, fSpi, fDex, fInt, fSta);

	FCHARSTATS () :
		fPow(0),
		fStr(0),
		fSpi(0),
		fDex(0),
		fInt(0),
		fSta(0)
	{
	}

	FCHARSTATS ( float _fPow, float _fStr, float _fSpi, float _fDex, float _fInt, float _fSta ) :
		fPow(_fPow),
		fStr(_fStr),
		fSpi(_fSpi),
		fDex(_fDex),
		fInt(_fInt),
		fSta(_fSta)
	{
	}

	float& GET ( EMSTATS emID )
	{
		switch(emID)
		{
		case EMPOW:	return fPow;
		case EMSTR:	return fStr;

		case EMSPI:	return fSpi;
		case EMDEX:	return fDex;

		case EMINT:	return fInt;
		case EMSTA:	return fSta;
		};

		static float fNULL=0.0f;
		return fNULL;
	}
};

struct SCHARSTATS
{
	WORD		wPow;	//	힘.   Power
	WORD		wStr;	//	체력. String
	WORD		wSpi;	//	정신. Spirit
	WORD		wDex;	//	민첩. Dex
	WORD		wInt;	//	지력. Inteligent
	WORD		wSta;	//	근력. Strength

    MSGPACK_DEFINE(wPow, wStr, wSpi, wDex, wInt, wSta);

	SCHARSTATS () :
		wPow(0),
		wStr(0),
		wSpi(0),
		wDex(0),
		wInt(0),
		wSta(0)
	{
	}
	
	SCHARSTATS ( WORD _wPow, WORD _wStr, WORD _wSpi, WORD _wDex, WORD _wInt, WORD _wSta ) :
		wPow(_wPow),
		wStr(_wStr),
		wSpi(_wSpi),
		wDex(_wDex),
		wInt(_wInt),
		wSta(_wSta)
	{
	}

	//	Note : 현제의 수치가 요구수치에 부합되는지 검사.
	BOOL CHECK_REQ ( SCHARSTATS sREQ, int nAccept=0 )
	{
		if ( wDex+nAccept < sREQ.wDex )	return FALSE;
		//if ( wInt+nAccept < sREQ.wInt )	return FALSE;
		if ( wPow+nAccept < sREQ.wPow )	return FALSE;
		if ( wSpi+nAccept < sREQ.wSpi )	return FALSE;
		if ( wSta+nAccept < sREQ.wSta )	return FALSE;
		if ( wStr+nAccept < sREQ.wStr )	return FALSE;

		return TRUE;
	}

	BOOL CHECK_REQ_ONLY_ONE ( SCHARSTATS sREQ, int nAccept=0 )
	{
		if ( (wDex+nAccept > sREQ.wDex && 0 != sREQ.wDex) ||
			 /*(wInt+nAccept > sREQ.wInt && 0 != sREQ.wInt) ||*/
			 (wPow+nAccept > sREQ.wPow && 0 != sREQ.wPow) ||
			 (wSpi+nAccept > sREQ.wSpi && 0 != sREQ.wSpi) ||
			 (wSta+nAccept > sREQ.wSta && 0 != sREQ.wSta) ||
			 (wStr+nAccept > sREQ.wStr && 0 != sREQ.wStr) )
			return TRUE;

		return FALSE;
	}

	void RESET ()
	{
		*this = SCHARSTATS();
	}

	WORD& GET ( EMSTATS emID )
	{
		switch(emID)
		{
		case EMPOW:	return wPow;
		case EMSTR:	return wStr;

		case EMSPI:	return wSpi;
		case EMDEX:	return wDex;

		case EMINT:	return wInt;
		case EMSTA:	return wSta;
		};

		static WORD wNULL=0;
		return wNULL;
	}

	bool IsZERO () const
	{
		WORD wSUM_STATS = wPow + wStr + wSpi + wDex + wInt + wSta;
		return wSUM_STATS==0;
	}

	const DWORD GetSumStats () const
	{
		return wPow + wStr + wSpi + wDex + wInt + wSta;
	}

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & wPow;
		ar & wStr;
		ar & wSpi;
		ar & wDex;
		ar & wInt;
		ar & wSta;
	}
};
typedef SCHARSTATS*	PSCHARSTATS;

inline bool operator != ( const SCHARSTATS& lvalue, const SCHARSTATS& rvalue )
{
	if ( lvalue.wPow != rvalue.wPow ) return true;
	if ( lvalue.wStr != rvalue.wStr ) return true;
	if ( lvalue.wSpi != rvalue.wSpi ) return true;
	if ( lvalue.wDex != rvalue.wDex ) return true;
	if ( lvalue.wInt != rvalue.wInt ) return true;
	if ( lvalue.wSta != rvalue.wSta ) return true;

	return false;
}

inline SCHARSTATS& operator + ( const SCHARSTATS& lvalue, const SCHARSTATS& rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	+= rvalue.wPow;
	sSTATS.wStr	+= rvalue.wStr;
	sSTATS.wSpi	+= rvalue.wSpi;
	sSTATS.wDex	+= rvalue.wDex;
	sSTATS.wInt	+= rvalue.wInt;
	sSTATS.wSta	+= rvalue.wSta;

	return sSTATS;
}

inline SCHARSTATS& operator * ( const SCHARSTATS& lvalue, const float rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	= static_cast<WORD>( sSTATS.wPow * rvalue );
	sSTATS.wStr	= static_cast<WORD> ( sSTATS.wStr * rvalue );
	sSTATS.wSpi	= static_cast<WORD> ( sSTATS.wSpi * rvalue );
	sSTATS.wDex	= static_cast<WORD> ( sSTATS.wDex * rvalue );
	sSTATS.wInt	= static_cast<WORD> ( sSTATS.wInt * rvalue );
	sSTATS.wSta	= static_cast<WORD> (sSTATS. wSta * rvalue );

	return sSTATS;
}

inline SCHARSTATS& operator * ( const SCHARSTATS& lvalue, const int rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	= static_cast<WORD> ( sSTATS.wPow * rvalue );
	sSTATS.wStr	= static_cast<WORD> ( sSTATS.wStr * rvalue );
	sSTATS.wSpi	= static_cast<WORD> ( sSTATS.wSpi * rvalue );
	sSTATS.wDex	= static_cast<WORD> ( sSTATS.wDex * rvalue );
	sSTATS.wInt	= static_cast<WORD> ( sSTATS.wInt * rvalue );
	sSTATS.wSta	= static_cast<WORD> (sSTATS. wSta * rvalue );

	return sSTATS;
}

inline SCHARSTATS& operator + ( const SCHARSTATS& lvalue, const FCHARSTATS& rvalue )
{
	static SCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.wPow	+= static_cast<WORD> ( rvalue.fPow );
	sSTATS.wStr	+= static_cast<WORD> ( rvalue.fStr );
	sSTATS.wSpi	+= static_cast<WORD> ( rvalue.fSpi );
	sSTATS.wDex	+= static_cast<WORD> ( rvalue.fDex );
	sSTATS.wInt	+= static_cast<WORD> ( rvalue.fInt );
	sSTATS.wSta	+= static_cast<WORD> ( rvalue.fSta );

	return sSTATS;
}

inline FCHARSTATS& operator * ( const FCHARSTATS& lvalue, const float rvalue )
{
	static FCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.fPow	= ( sSTATS.fPow * rvalue );
	sSTATS.fStr	= ( sSTATS.fStr * rvalue );
	sSTATS.fSpi	= ( sSTATS.fSpi * rvalue );
	sSTATS.fDex	= ( sSTATS.fDex * rvalue );
	sSTATS.fInt	= ( sSTATS.fInt * rvalue );
	sSTATS.fSta	= (sSTATS. fSta * rvalue );

	return sSTATS;
}

inline FCHARSTATS& operator * ( const FCHARSTATS& lvalue, const int rvalue )
{
	static FCHARSTATS sSTATS;
	sSTATS = lvalue;

	sSTATS.fPow	= ( sSTATS.fPow * rvalue );
	sSTATS.fStr	= ( sSTATS.fStr * rvalue );
	sSTATS.fSpi	= ( sSTATS.fSpi * rvalue );
	sSTATS.fDex	= ( sSTATS.fDex * rvalue );
	sSTATS.fInt	= ( sSTATS.fInt * rvalue );
	sSTATS.fSta	= (sSTATS. fSta * rvalue );

	return sSTATS;
}


struct SSEARCHITEMDATA
{
    std::string  m_ItemName;        //! 아이템 이름 [MAP_NAME_LENGTH]
	DWORD		 dwReqCharClass;			  // 착용 클레스
	DWORD		 dwSuitType;				  // 아이템 타입
	WORD		 wReqLevel;					  // 착용 최소 레벨
	SCHARSTATS	 sReqStats;					  // 아이템 착용 스텟치
	LONGLONG	 llPRICE;					  // 아이템 가격
		
	BYTE		 RandOptTYPE1;				// 랜덤 옵션타입 1
	BYTE		 RandOptTYPE2;				// 랜덤 옵션타입 2
	BYTE		 RandOptTYPE3;				// 랜덤 옵션타입 3
	BYTE		 RandOptTYPE4;				// 랜덤 옵션타입 4

	float		 fRandOptValue1;			// 랜덤 옵션값 1	
	float		 fRandOptValue2;			// 랜덤 옵션값 2
	float		 fRandOptValue3;			// 랜덤 옵션값 3	
	float		 fRandOptValue4;			// 랜덤 옵션값 4


	BYTE		 cDAMAGE;					//	공격력 개조 등급.
	BYTE		 cDEFENSE;					//	방어력 개조 등급.

	BYTE		 cRESIST_FIRE;				//	저항(화) 개조 등급.
	BYTE		 cRESIST_ICE;				//	저항(빙) 개조 등급.
	BYTE		 cRESIST_ELEC;				//	저항(전) 개조 등급.
	BYTE		 cRESIST_POISON;			//	저항(독) 개조 등급.
	BYTE		 cRESIST_SPIRIT;			//	저항(정) 개조 등급.

    MSGPACK_DEFINE(
        m_ItemName, dwReqCharClass, dwSuitType, wReqLevel, sReqStats, llPRICE,
        RandOptTYPE1, RandOptTYPE2, RandOptTYPE3, RandOptTYPE4,
        fRandOptValue1, fRandOptValue2, fRandOptValue3, fRandOptValue4,
        cDAMAGE, cDEFENSE,
        cRESIST_FIRE, cRESIST_ICE, cRESIST_ELEC, cRESIST_POISON, cRESIST_SPIRIT);

	SSEARCHITEMDATA() 
		: llPRICE(0)
		, dwReqCharClass(0)
		, dwSuitType(0)
		, wReqLevel(0)
		, cDAMAGE(0)
		, cDEFENSE(0)
		, cRESIST_FIRE(0)
		, cRESIST_ICE(0)
		, cRESIST_ELEC(0)
		, cRESIST_POISON(0)
		, cRESIST_SPIRIT(0)
		, RandOptTYPE1(0)
		, RandOptTYPE2(0)
		, RandOptTYPE3(0)
		, RandOptTYPE4(0)
		, fRandOptValue1(0.0f)
		, fRandOptValue2(0.0f)
		, fRandOptValue3(0.0f)
		, fRandOptValue4(0.0f)
	{		
		//memset(m_ItemName, 0, sizeof(char) * (MAP_NAME_LENGTH));
	}

};	


struct SSEARCHITEMESULT
{
	char szShopName[MAP_NAME_LENGTH];
	D3DXVECTOR3 vShopPos;
	SNATIVEID	nSearchItemID;
	LONGLONG	llPRICE;

	SSEARCHITEMESULT();
    void SetShopName(const std::string& Name);
};	

enum EMELEMENT
{
	EMELEMENT_SPIRIT		= 0,
	
	EMELEMENT_FIRE			= 1,
	EMELEMENT_ICE			= 2,
	EMELEMENT_ELECTRIC		= 3,
	EMELEMENT_POISON		= 4,	
	EMELEMENT_OLDMAX		= 5,	// 예전 최대치
	
	EMELEMENT_STONE			= 5,	
	EMELEMENT_MAD			= 6,
	EMELEMENT_STUN			= 7,
	EMELEMENT_CURSE			= 8,	
	EMELEMENT_MAXNUM		= 9,	// New
	
	EMELEMENT_ARM			= 9,
	//	element 속성이 무기에 종속적일 경우.

	EMELEMENT_MAXNUM2		= 10,
};


//	Note : 몹 100 버전의 정보에서 사용됨.
struct SRESIST_100
{	
	int		nFire;		//	불.
	int		nIce;		//	얼음.
	
	int		nElectric;	//	전기.
	int		nPoison;	//	독.

	int		nDivine;	//	독.

	SRESIST_100 () :
		nFire(0),
		nIce(0),

		nElectric(0),
		nPoison(0),
		nDivine(0)
	{
	}
};

struct SRESIST_101
{	
	short	nFire;		//	불.
	short	nIce;		//	얼음.
	
	short	nElectric;	//	전기.
	short	nPoison;	//	독.
};

struct SRESIST
{	
	short	nFire;		//	불.
	short	nIce;		//	얼음.
	
	short	nElectric;	//	전기.
	short	nPoison;	//	독.

	short	nSpirit;

	SRESIST () :
		nFire(0),
		nIce(0),

		nElectric(0),
		nPoison(0),

		nSpirit(0)
	{
	}

	SRESIST ( short iF, short iI, short iE, short iP, short iS ) :
		nFire(iF),
		nIce(iI),

		nElectric(iE),
		nPoison(iP),

		nSpirit(iS)
	{
	}

	void RESET ()
	{
		*this = SRESIST();
	}

	short GetElement ( EMELEMENT emElement ) const
	{
		switch ( emElement )
		{
		case EMELEMENT_SPIRIT:		return nSpirit;
		case EMELEMENT_FIRE:		return nFire;
		case EMELEMENT_ICE:			return nIce;
		case EMELEMENT_ELECTRIC:	return nElectric;
		case EMELEMENT_POISON:		return nPoison;
		case EMELEMENT_STONE:		return nFire;
		case EMELEMENT_MAD:			return nIce;
		case EMELEMENT_STUN:		return nElectric;
		case EMELEMENT_CURSE:		return nPoison;
		};

		return 0;
	}

	BOOL operator== ( const SRESIST& rRight ) const
	{
		return (nFire==rRight.nFire) && (nIce==rRight.nIce) && (nElectric==rRight.nElectric) && (nPoison==rRight.nPoison) && (nSpirit==rRight.nSpirit);
	}

	BOOL operator!= ( const SRESIST& rRight ) const
	{
		return !( (nFire==rRight.nFire) && (nIce==rRight.nIce) && (nElectric==rRight.nElectric) && (nPoison==rRight.nPoison) && (nSpirit==rRight.nSpirit) );
	}

	/*SRESIST& operator + ( const SRESIST &rvalue )
	{
		static SRESIST sREG;
		sREG = *this;

		sREG.nFire += rvalue.nFire;
		sREG.nIce += rvalue.nIce;
	
		sREG.nElectric += rvalue.nElectric;
		sREG.nPoison += rvalue.nPoison;

		sREG.nSpirit += rvalue.nSpirit;

		return sREG;
	}*/

	SRESIST& operator += ( const SRESIST &rvalue )
	{
		nFire += rvalue.nFire;
		nIce += rvalue.nIce;
	
		nElectric += rvalue.nElectric;
		nPoison += rvalue.nPoison;

		nSpirit += rvalue.nSpirit;

		return *this;
	}

	SRESIST& operator + ( const int rvalue )
	{
		short srvalue = static_cast<short>(rvalue);
		static SRESIST sREG;
		sREG = *this;

		sREG.nFire += srvalue;
		sREG.nIce += srvalue;
	
		sREG.nElectric += srvalue;
		sREG.nPoison += srvalue;

		sREG.nSpirit += srvalue;

		return sREG;
	}

	SRESIST& operator += ( const int rvalue )
	{
		short srvalue = static_cast<short>(rvalue);

		nFire += srvalue;
		nIce += srvalue;
	
		nElectric += srvalue;
		nPoison += srvalue;

		nSpirit += srvalue;

		return *this;
	}

	void Assign ( const SRESIST_101 &sRESIST )
	{
		nFire = sRESIST.nFire;
		nIce = sRESIST.nIce;
	
		nElectric = sRESIST.nElectric;
		nPoison = sRESIST.nPoison;
	}

	void LIMIT()
	{
		if ( nFire < 0 ) nFire = 0;
		if ( nIce < 0 ) nIce = 0;
		if ( nElectric < 0 ) nElectric = 0;
		if ( nPoison < 0 ) nPoison = 0;
		if ( nSpirit < 0 ) nSpirit = 0;
	}
};
typedef SRESIST*	PSRESIST;


inline SRESIST& operator + ( const SRESIST& lvalue, const SRESIST& rvalue )
{
	static SRESIST sREG;
	sREG = lvalue;

	sREG.nFire      += rvalue.nFire;
	sREG.nIce       += rvalue.nIce;
	sREG.nElectric  += rvalue.nElectric;
	sREG.nPoison    += rvalue.nPoison;
	sREG.nSpirit    += rvalue.nSpirit;

	return sREG;
}

inline SRESIST& operator - ( const SRESIST& lvalue, const SRESIST& rvalue )
{
    static SRESIST sREG;
    sREG = lvalue;

    sREG.nFire      -= rvalue.nFire;
    sREG.nIce       -= rvalue.nIce;
    sREG.nElectric  -= rvalue.nElectric;
    sREG.nPoison    -= rvalue.nPoison;
    sREG.nSpirit    -= rvalue.nSpirit;

    return sREG;
}

enum EMACTIONTYPE
{
	GLAT_IDLE, 
	GLAT_MOVE,
	GLAT_ATTACK,
	GLAT_SKILL,				// Skill 을 실제 사용하는 부분.
	GLAT_SHOCK,
	GLAT_PUSHPULL,    // 새로운 EMACTIONTYPE을 추가할 경우, m_bSTATE_STUN 플래그로 인해 FrameMove함수에서 GLAT_SHOCK에 덮어씌워지는 것에 주의할 것
	GLAT_GATHERING,
	GLAT_TALK,
	GLAT_CONFT_END,
	GLAT_MOTION,

	GLAT_AUTHED,	//mjeon.CaptureTheField - 특정 학원에 의해 Crow가 인증되었음(GLAT_FALLING 대신).
	GLAT_FALLING,
	GLAT_DIE,
	GLAT_FEIGN_FALLING,
	GLAT_FEIGN_DIE,

	GLAT_SKILL_WAIT,		// Skill 을 사용하면 AniThread 때문에 딜레이가 생겨서 대기하도록 한다.

	GLAT_SIZE,
};
typedef EMACTIONTYPE*		PEMACTIONTYPE;

enum EMTRIBE
{
	TRIBE_HUMAN			= 0,	//	인간.
	TRIBE_MONSTER		= 1,	//	괴물.
	TRIBE_ANGEL			= 2,	//	천사.
	TRIBE_DEVIL			= 3,	//	악마.
	TRIBE_SPIRIT		= 4,	//	정령.
	TRIBE_FAIRY			= 5,	//	요정.
	TRIBE_DRAGON		= 6,	//	드래곤.

	TRIBE_NSIZE			= 7		
};

enum EMBRIGHT
{
	BRIGHT_LIGHT		= 0,
	BRIGHT_DARK			= 1,
	
	BRIGHT_BOTH			= 2,	//	모두 가능. ( item 항목에서. )

	BRIGHT_SIZE			= 3,
};

enum EM_BRIGHT_FB
{
	BFB_DIS				= 0,	//	불리.
	BFB_AVER			= 1,	//	보통.
	BFB_ADV				= 2,	//	유리.

	BFB_SIZE			= 3,
};

enum EMMOVETYPE
{
	MOVE_LAND			= 0,	//	지면 이동.
	MOVE_FLY			= 1,	//	공중 이동.
	MOVE_SWIM			= 2,	//	수면 이동.

	MOVETYPE_NSIZE		= 3
};

enum EMIMPACT_TARGET
{
	TAR_SELF		= 0,		//	자기 위치 영향.
	TAR_SPEC		= 1,		//	대상 위치 영향.
	TAR_SELF_TOSPEC	= 2,		//	자기 위치에서 대상 위치까지. ( 화살 관통 )
	TAR_ZONE		= 3,		//	지면위치의 주변 타겟들을 가져온다.
	TAR_LAND		= 4,		//  지면위치

	EMIMPACT_TARGET_NSIZE		= 5,
};

enum EMIMPACT_REALM
{
	REALM_SELF		= 0,		//	'목표' 자신.
	REALM_ZONE		= 1,		//	'목표' 주위.
	REALM_KEEP_ZONE	= 2,		//	'목표' 주위 지속.
	REALM_FANWIZE	= 3,		//	'목표'를 향한 부채꼴 모양.

	EMIMPACT_REALM_NSIZE		= 4
};

enum EMIMPACT_SIDE
{
	EMIMPACT_SIDE_PARTY		= 0,		//	자기편에게.
	EMIMPACT_SIDE_ENEMY		= 1,		//	적에게.
	EMIMPACT_SIDE_ANYBODY	= 2,		//	모두에게.

	EMIMPACT_SIDE_NSIZE		= 3,


    // 이 부분은 스킬타겟시스템에는 없는 부분으로 GetSelectTargetAround (주변 타겟 검사) 때문에 임시로 만들어진 값;
    // !다른곳에서는 사용 금지;
    // 추후 스킬타겟 개편때 밑의 ENUM값으로 대체되어야 함;
    EMIMPACT_SIDE_OUR       = 4,        // 아군에게;

// 스킬 타겟 시스템의 개편시에 추가될만한 사항에 대해서 작성해놓은 부분;
// (지금은 제대로 동작하지 않으니 사용하지 말것);
    //EMIMPACT_SIDE_ALLY      = 0x0010,   // 아군;
    //EMIMPACT_SIDE_NEUTRAL   = 0x0020,   // 중립;
    //EMIMPACT_SIDE_ENEMY     = 0x0040,   // 적;
    //EMIMPACT_SIDE_FACTION   = 0x0080,   // 자신의 진영;

    //EMIMPACT_SIDE_PARTY     = 0x0100,   // 자신의 파티;
    //EMIMPACT_SIDE_CLUB      = 0x0200,   // 자신의 클럽원;
    //EMIMPACT_SIDE_EXPEDITION= 0x0400,   // 자신의 원정대;
    //EMIMPACT_SIDE_SCHOOL    = 0x0800,   // 자신의 학원생;
};

enum EMDISORDER
{
	DIS_NULL			= 0x00,	// 없음
	DIS_NUMB			= 0x01,	// 마비.
	DIS_STUN			= 0x02,	// 기절.
	DIS_STONE			= 0x04,	// 석화.
	DIS_BURN			= 0x08,	// 화염.
	DIS_FROZEN			= 0x10,	// 냉동.

	DIS_MAD				= 0x20,	// 착란.
	DIS_POISON			= 0x40,	// 중독.
	DIS_CURSE			= 0x80,	// 저주.

	DIS_ALL				= (DIS_NUMB | DIS_STUN | DIS_STONE | DIS_BURN | DIS_FROZEN | DIS_MAD | DIS_POISON | DIS_CURSE),
};

// 스킬효과 EMSPECA_FEIGN_DEATH 체크 플래그
enum EMFEIGNDEATH
{
	FEIGN_DEATH_TARGETING_DISABLE = 0x01,
	FEIGN_DEATH_BUF_DEL			  = 0x02,

	FEIGN_DEATH_ALL     = (FEIGN_DEATH_TARGETING_DISABLE | FEIGN_DEATH_BUF_DEL),
};
//	상태 이상 효과.
//
enum EMSTATE_BLOW
{
	EMBLOW_NONE		= 0,	//	상태 이상 유발 없음.
	EMBLOW_NUMB		= 1,	//	마비.
	EMBLOW_STUN		= 2,	//	기절.
	EMBLOW_STONE		= 3,	//	석화.
	EMBLOW_BURN			= 4,	//	화염.
	EMBLOW_FROZEN		= 5,	//	냉동.
	EMBLOW_SINGLE		= 5,	//	단독으로만 발생하는 "상태이상" 효과.

	EMBLOW_MAD			= 6,	//	착란.
	EMBLOW_POISON		= 7,	//	중독.
	EMBLOW_CURSE		= 8,	//	저주.
	EMBLOW_PANT		= 9,	//	헐떡임.
	
	EMBLOW_SIZE			= 10,
	EMBLOW_MULTI		= 4,
};

enum EMSTATE_BLOW_FLAG
{
	EMBLOW_FLAG_NULL			= 0x00,	//	상태 이상 유발 없음.
	EMBLOW_FLAG_NUMB		= (1 << EMBLOW_NUMB),		//	마비.
	EMBLOW_FLAG_STUN		= (1 << EMBLOW_STUN),		//	기절.
	EMBLOW_FLAG_STONE		= (1 << EMBLOW_STONE),		//	석화.
	EMBLOW_FLAG_BURN		= (1 << EMBLOW_BURN),		//	화염.
	EMBLOW_FLAG_FROZEN		= (1 << EMBLOW_FROZEN),	//	냉동.	
	EMBLOW_FLAG_MAD			= (1 << EMBLOW_MAD),		//	착란.
	EMBLOW_FLAG_POISON		= (1 << EMBLOW_POISON),	//	중독.
	EMBLOW_FLAG_CURSE		= (1 << EMBLOW_CURSE),		//	저주.	
	EMBLOW_FLAG_PANT		= (1 << EMBLOW_PANT),		//	저주.	
};

enum EMTRANSFORM_SET
{
	EMTRANSFORM_SET_MAXSIZE			= 40,
	EMTRANSFORM_CHAR_SET_MAXSIZE    = 20,
	EMTRANSFORM_CHAR_SEX			= 2,
};

inline EMDISORDER STATE_TO_DISORDER ( EMSTATE_BLOW emBLOW )
{
	switch ( emBLOW )
	{
	default:			return DIS_NULL;
	case EMBLOW_NUMB:	return DIS_NUMB;
	case EMBLOW_STUN:	return DIS_STUN;
	case EMBLOW_STONE:	return DIS_STONE;
	case EMBLOW_BURN:	return DIS_BURN;
	case EMBLOW_FROZEN:	return DIS_FROZEN;

	case EMBLOW_MAD:	return DIS_MAD;
	case EMBLOW_POISON:	return DIS_POISON;
	case EMBLOW_CURSE:	return DIS_CURSE;
	};
}

inline EMELEMENT STATE_TO_ELEMENT ( EMSTATE_BLOW emBLOW )
{
	switch ( emBLOW )
	{
	case EMBLOW_NUMB:	return EMELEMENT_ELECTRIC;
	case EMBLOW_STUN:	return EMELEMENT_STUN;
	case EMBLOW_STONE:	return EMELEMENT_STONE;
	case EMBLOW_BURN:	return EMELEMENT_FIRE;
	case EMBLOW_FROZEN:	return EMELEMENT_ICE;
	case EMBLOW_MAD:	return EMELEMENT_MAD;
	case EMBLOW_POISON:	return EMELEMENT_POISON;
	case EMBLOW_CURSE:	return EMELEMENT_CURSE;
	};

	return EMELEMENT_SPIRIT;
}

enum EMGROUP
{
	EMGROUP_ONE		= 0,
	EMGROUP_PARTY	= 1,
	EMGROUP_GUILD	= 2,

	EMGROUP_NSIZE	= 3,
};

enum EMIMPACT_ADDON
{
	EMIMPACTA_NONE			= 0,
	EMIMPACTA_HITRATE		= 1,	//	명중율 변화량.
	EMIMPACTA_AVOIDRATE		= 2,	//	회피율 변화량.
	
	EMIMPACTA_DAMAGE		= 3,	//	공격치 변화량.
	EMIMPACTA_DEFENSE		= 4,	//	방어치 변화량.

	EMIMPACTA_VARHP			= 5,	//	HP 변화율.
	EMIMPACTA_VARMP			= 6,	//	MP 변화율.
	EMIMPACTA_VARSP			= 7,	//	SP 변화율.
	EMIMPACTA_VARAP			= 8,	//	HP,MP,SP 변화율.

	EMIMPACTA_DAMAGE_RATE	= 9,	//	공격치 변화율.
	EMIMPACTA_DEFENSE_RATE	= 10,	//	방어치 변화율.

	EMIMPACTA_PA			= 11,	//	격투치.
	EMIMPACTA_SA			= 12,	//	사격치.
	EMIMPACTA_MA			= 13,	//	방어치.

	EMIMPACTA_HP_RATE		= 14,	//	HP 증폭율.
	EMIMPACTA_MP_RATE		= 15,	//	MP 증폭율.
	EMIMPACTA_SP_RATE		= 16,	//	SP 증폭율.

	EMIMPACTA_RESIST		= 17,	//	저항 수치 변경
	EMIMPACTA_STATS			= 18,	//	스탯 수치 변경 ( 힘,민첩,정신 ) 

    EMIMPACTA_HP_ADD        = 19,  //  HP 회복력 증감
    EMIMPACTA_MP_ADD        = 20,  //  MP 회복력 증감
    EMIMPACTA_SP_ADD        = 21,  //  SP 회복력 증감
    EMIMPACTA_CP_ADD        = 22,  //  CP 회복력 증감

    EMIMPACTA_CP_DUR        = 23,  //  CP 자동지속 증감

	EIMPACTA_SIZE			= 24,
};

#define SKILL_ADDITIONAL_ID( skillid )					(skillid | 0xFFFF0000)
#define IS_ADDITIONAL_ID( skillid )						((skillid >> 16) == 0xFFFF)
#define SKILL_TO_ADDITIONAL( skillid )					(skillid & 0xFFFF)

//추가가 아니라 중간 emum 인덱스가 수정 되면 GameWord.xml, ITEM_INFO_SET_BONUS 인덱스도 같이 수정해줘야함
enum EMSPEC_ADDON
{
	EMSPECA_NULL						= 0,		//	기능 없음.
	EMSPECA_PUSHPULL					= 1,		//	당기기/밀기.
	EMSPECA_REFDAMAGE				= 2,		//	대미지 반사.
	EMSPECA_REBIRTH					= 3,		//	부활.
	
	EMSPECA_HP_GATHER				= 4,		//	HP의 일정량을 빼앗음.
	EMSPECA_MP_GATHER				= 5,		//	MP의 일정량을 빼앗음.
	EMSPECA_SP_GATHER				= 6,		//	SP의 일정량을 빼앗음.

	EMSPECA_HP_DIV						= 7,		//	HP의 일부를 나누어 줌.
	EMSPECA_MP_DIV					= 8,		//	MP의 일부를 나누어 줌.
	EMSPECA_SP_DIV						= 9,		//	SP의 일부를 나누어 줌.

	EMSPECA_NONBLOW					= 10,		//	상태이상 방지.
	EMSPECA_RECBLOW					= 11,		//	상태이상 회복.

	EMSPECA_PIERCE						= 12,		//	관통정도.
	EMSPECA_RANGE_PSY_LONG		= 13,		//	물리 사격형 사정거리 및 적용영역 증가( 감소 )

	EMSPECA_MOVEVELO				= 14,		//  이동속도.
	EMSPECA_ONWARD					= 15,		//  진격.
	EMSPECA_INVISIBLE					= 16,		//	은신.
	EMSPECA_RECVISIBLE				= 17,		//	은신 감지.

	EMSPECA_ATTACKVELO				= 18,		//	공격속도.
	EMSPECA_SKILLDELAY				= 19,		//	스킬 딜래이 시간 변화.
	EMSPECA_CRUSHING_BLOW		= 20,		//	강한타격 스킬

	EMSPECA_PSY_DAMAGE_REDUCE				= 21,		//  물리 데미지 흡수율
	EMSPECA_MAGIC_DAMAGE_REDUCE			= 22,		//  매직 데미지 흡수율
	EMSPECA_PSY_DAMAGE_REFLECTION		= 23,		//  물리 데미지 반사율
	EMSPECA_MAGIC_DAMAGE_REFLECTION	= 24,		//  매직 데미지 반사율

	EMSPECA_EFFECT_REMOVE			= 25,		//	효과 제거 ( 일반,버프,디버프,리미트 종류별로 ) 
	EMSPECA_STUN							= 26,		//	스턴 효과

	EMSPECA_DEFENSE_SKILL_ACTIVE	= 27,		//	피격시 발동 
	EMSPECA_PULLIN							= 28,		//	끌어 오기
	EMSPECA_KNOCKBACK					= 29,		//	밀어 내기

	EMSPECA_DURATION_DAMAGE		= 30,		//	지속데미지
	EMSPECA_CURSE							= 31,		//	저주

	EMSPECA_TARRANGE_ALL				= 32,		//	스킬 사정거리 변화
	EMSPECA_APPLYRANGE_ALL			= 33,		//	스킬 적용영역 변화

	EMSPECA_NON_DRUG					= 34,		//	물약 사용 금지 ( 약품류 전체 금지 ) 
	EMSPECA_NON_SKILL					= 35,		//	스킬 사용 금지 ( 약품류 전체 금지 ) 

	EMSPECA_IGNORED_DAMAGE			= 36,		//	데미지 흡수(무시)

    EMSPECA_ITEMDROP_RATE			= 37,		//	아이템드롭율
    EMSPECA_MONEYDROP_RATE			= 38,		//	금액드롭율
    EMSPECA_EXP_RATE						= 39,		//	경험치증가율
    EMSPECA_NPC_TALK						= 40,		//	특정 NPC 대화
    EMSPECA_ITEMDROP_SPECIFIC		= 41,		//	특정 아이템 드롭

	EMSPECA_TELEPORT						= 42,		//	순간이동
	EMSPECA_SWAPPOS						= 43,		//	위치교대

	EMSPECA_INCREASE_EFF				= 44,		//	강화 효과( 효과 대상 공격자 강화 )
	EMSPECA_ILLUSION						= 45,		//	허상효과
    EMSPECA_VEHICLE_GETOFF			= 46,		//  하차효과
    EMSPECA_ENDURE_VARY				= 47,		//  지속시간 증감

    EMSPECA_STIGMA						= 48,		//  낙인효과    
    EMSPECA_HALLUCINATE				= 49,		//  환독효과
    EMSPECA_RELEASE_ENDURE			= 50,		//  피격시 효과 해제 
    EMSPECA_LINK_HP						= 51,		//  고리효과( HP 링크 )
    EMSPECA_RELEASE_PET				= 52,		//  수독효과( 펫 해제 )
    EMSPECA_IMMUNE						= 53,		//  면역효과
    EMSPECA_TURN_STATE_BATTLE		= 54,		//  전투상태 효과( 전투 상태로 바꿈 )
    EMSPECA_TURN_STATE_PEACE		= 55,		//  전투상태 해제( 평화 상태로 바꿈 )
    EMSPECA_TAUNT							= 56,		//  도발 효과    
	EMSPECA_DOMINATION					= 57,		//  지배 효과
	EMSPECA_RANDOM_EXP_RATE		= 58,		//  랜덤 경험치 증가율;

	EMSPECA_SKILL_LINK					= 59,		//  스킬 링크
	
	//Random_opt EMIMPACTA 기능 추가 
	//인덱스 번호가 수정 되면 GameWord.xml, ITEM_INFO_SET_BONUS 인덱스, comment.lua도 같이 수정해줘야함
	EMSPECA_DAMAGE_VALUE				= 60,		//	공격력(정수).
	EMSPECA_DAMAGE_RATE				= 61,		//	공격력 % 증가.

	EMSPECA_DEFENSE_VALUE				= 62,		//	방어력(정수).
	EMSPECA_DEFENSE_RATE				= 63,		//	방어력 전체 % 증가.
	
	EMSPECA_HIT_VALUE					= 64,		//	명중율(정수).
	EMSPECA_HIT_RATE						= 65,		//	명중율 전체 % 증가

	EMSPECA_AVOID_VALUE				= 66,		//	회피율(정수).
	EMSPECA_AVOID_RATE					= 67,		//	회피율 전체 % 증가 
	
	EMSPECA_HP_VALUE						= 68,		//	hp 증가.
	EMSPECA_MP_VALUE						= 69,		//	mp 증가.
	EMSPECA_SP_VALUE						= 70,		//	sp 증가.
	EMSPECA_CP_VALUE						= 71,		//	cp 증가.  %처리되지 않음%

	EMSPECA_HP_VARIATION_RATE		= 72,		//	hp 변화율(특수효과).
	EMSPECA_MP_VARIATION_RATE		= 73,		//	mp 변화율(특수효과).
	EMSPECA_SP_VARIATION_RATE		= 74,		//	sp 변화율(특수효과).
	EMSPECA_HMS_VARIATION_RATE		= 75,		//	hp, mp, sp 변화율(특수효과).

	EMSPECA_ATTACK_RANGE_VALUE	= 76,		//	공격 영역.
	EMSPECA_DIS_SP_VALUE				= 77,		//	sp 소모량.
	EMSPECA_RESIST_VALUE				= 78,		//	저항.

	EMSPECA_STATS_POW_VALUE		= 79,		//	힘(정수).
	EMSPECA_STATS_STR_VALUE			= 80,		//	체력(정수).
	EMSPECA_STATS_SPI_VALUE			= 81,		//	정신(정수).
	EMSPECA_STATS_DEX_VALUE			= 82,		//	민첩(정수).
	EMSPECA_STATS_STA_VALUE		= 83,		//	근력(정수).

	EMSPECA_PA_VALUE						= 84,		//	격투치(정수).
	EMSPECA_SA_VALUE						= 85,		//	사격치(정수).
	EMSPECA_MA_VALUE					= 86,		//	마력치(정수).

	EMSPECA_HP_POTION_INC_VALUE	= 87,		//	포션 hp 증가량(정수).
	EMSPECA_MP_POTION_INC_VALUE	= 88,		//	포션 mp 증가량(정수).
	EMSPECA_SP_POTION_INC_VALUE	= 89,		//	포션 sp 증가량(정수).

	EMSPECA_CP_INC_VALUE				= 90,		//	몬스터 킬시 CP 증가량(정수).
	EMSPECA_MOVEVELO_VALUE			= 91,		// 이동속도 증가(정수)
	EMSPECA_DAMAGE_RATE_ALL			= 92,		// 데미지 (근거리, 사격 보정치 적용 받은 최종 수치에서 곱연산)
	EMSPECA_VIEW_RANGE_RATE			= 93,		// 시야 제한;
	EMSPECA_AIRBORNE				= 94,		// 에어본;

	EMSPECA_SUMMON_CHANGE_SKILLPAGE	= 95,	// 소환수 스킬 페이지 변경;
	EMSPECA_SUMMON_EVENT_SKILL		= 96,	// 소환수 이벤트 스킬;
	EMSPECA_SUMMON_TARGETING		= 97,	// 소환수 타겟 지정;
	EMSPECA_SUMMON_RUNAWAY			= 98,	// 소환수 도망;
	EMSPECA_DELAY_ACTION_SKILL		= 99,	// 지연발동
	EMSPECA_CLONE					= 100,	// 복제효과
	
    EMSPECA_MOVEVELO_LIMIT          = 101,  //  이동속도 제한;
	EMSPECA_COUNTATTACK				= 102,  // 반격

	EMSPECA_SCREEN_THROW			= 103,  // 화면투척
	EMSPECA_ENTRANCE_EXIT			= 104,  // 입출구
	EMSPECA_FEIGN_DEATH				= 105,  // 죽은척
	EMSPECA_CAMERA_HIT				= 106,  // 카메라치기
	EMSPECA_DISGUISE				= 107,  // 변장
	EMSPECA_PROVOKE					= 108,  // 도발
	EMSPECA_SPECIAL_IMMUNE			= 109,  // 특정효과면역
	EMSPECA_ACTIVATE_DAMAGE			= 110,  // 피해발동
	EMSPECA_REMOVE_SKILL_EFFECT		= 111,	// 스킬효과 제거 ( 각 효과별로 제거 );


	EMSPECA_NSIZE
};

enum EMSPEC_RELEASE_ENDURE_TYPE // 피격시 효과 해제 적용 타입;
{
	EMSPEC_RELEASE_ENDURE_TYPE_NORMAL = 0,
	EMSPEC_RELEASE_ENDURE_TYPE_SKILL = 1,
	EMSPEC_RELEASE_ENDURE_TYPE_NSIZE = 2,
};

enum EMSPEC_RELEASE_ENDURE_TYPE_DWORD
{
	EMSPEC_RELEASE_ENDURE_TYPE_DWORD_NULL = 0x0,
	EMSPEC_RELEASE_ENDURE_TYPE_DWORD_NORMAL = 0x1,
	EMSPEC_RELEASE_ENDURE_TYPE_DWORD_SKILL = 0x2,
};

enum EMSPEC_STIGMA_TYPE // 낙인 효과 적용 타입
{		
	EMSPEC_STIGMA_TYPE_DAMAGE_NORMAL    = 0,
	EMSPEC_STIGMA_TYPE_DAMAGE_SKILL     = 1,
	EMSPEC_STIGMA_TYPE_BUFF_SKILL       = 2,
	EMSPEC_STIGMA_TYPE_DEBUFF_SKILL     = 3,
	EMSPEC_STIGMA_TYPE_LIMIT_SKILL      = 4,

	EMSPEC_STIGMA_TYPE_NSIZE            = 5,
};

enum EMSPEC_STIGMA_TYPE_DWORD // 낙인 효과 적용 타입 플래그
{
	EMSPEC_STIGMA_TYPE_DWORD_NULL           = 0x00,
	EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_NORMAL  = (1 << EMSPEC_STIGMA_TYPE_DAMAGE_NORMAL),
	EMSPEC_STIGMA_TYPE_DWORD_DAMAGE_SKILL   = (1 << EMSPEC_STIGMA_TYPE_DAMAGE_SKILL ),
	EMSPEC_STIGMA_TYPE_DWORD_BUFF_SKILL     = (1 << EMSPEC_STIGMA_TYPE_BUFF_SKILL   ),
	EMSPEC_STIGMA_TYPE_DWORD_DEBUFF_SKILL   = (1 << EMSPEC_STIGMA_TYPE_DEBUFF_SKILL ),
	EMSPEC_STIGMA_TYPE_DWORD_LIMIT_SKILL    = (1 << EMSPEC_STIGMA_TYPE_LIMIT_SKILL  ),
};

enum EMSPEC_DOMINATE_TYPE
{
	EMSPEC_DOMINATE_TYPE_SELF = 0,
	EMSPEC_DOMINATE_TYPE_CASTER = 1,
	EMSPEC_DOMINATE_TYPE_RANDOM = 2,	// 지배, 이동 위치 랜덤;
	EMSPEC_DOMINATE_TYPE_FIX = 3,		// 지배, 이동X, 위치 고정;
	EMSPEC_DOMINATE_TYPE_NSIZE = 4,
};

enum EMSPEC_HALLUCINATE_TYPE
{
	EMSPEC_HALLUCINATE_TYPE_TRANSFORM = 0,
	EMSPEC_HALLUCINATE_TYPE_EMOTICON = 1,
	EMSPEC_HALLUCINATE_TYPE_NONTARGETABLE = 2,	
	EMSPEC_HALLUCINATE_TYPE_NSIZE = 3,
};
enum EMSPEC_HALLUCINATE_FLAG
{
	EMSPEC_HALLUCINATE_DWORD_NULL = 0x00,
	EMSPEC_HALLUCINATE_DWORD_TRANSFORM = (1 << EMSPEC_HALLUCINATE_TYPE_TRANSFORM),
	EMSPEC_HALLUCINATE_DWORD_EMOTICON = (1 << EMSPEC_HALLUCINATE_TYPE_EMOTICON),	
	EMSPEC_HALLUCINATE_DWORD_NONTARGETABLE = (1 << EMSPEC_HALLUCINATE_TYPE_NONTARGETABLE),	
};

enum EMSPEC_INC_TYPE
{
    EMSPEC_INC_TYPE_PLUS  = 0,//  증가
    EMSPEC_INC_TYPE_MINUS   = 1,// 감소

    EMSPEC_INC_TYPE_NSIZE   = 2,
};

enum EMSPEC_ASIGN_TYPE
{
    EMSPEC_SKILL_ASING_BEFORE  = 0,// 현재 지속효과
    EMSPEC_SKILL_ASING_AFTER   = 1,// 이후 지속효과

    EMSPEC_SKILL_ASING_NSIZE   = 2,
};

enum EMSPEC_ASIGN_TYPE_DWORD
{
    EMSPEC_SKILL_ASING_DWORD_NULL   = 0x00,
    EMSPEC_SKILL_ASING_DWORD_BEFORE = ( 1 << EMSPEC_SKILL_ASING_BEFORE ),// 현재 지속효과
    EMSPEC_SKILL_ASING_DWORD_AFTER  = ( 1 << EMSPEC_SKILL_ASING_AFTER  ),// 이후 지속효과
};

enum EMSPEC_INVISIBLE_ANI_TYPE
{
	EMSPEC_INVISIBLE_ANI_TYPE_NONE = 0,
	EMSPEC_INVISIBLE_ANI_TYPE_WALK = 1,
	EMSPEC_INVISIBLE_ANI_TYPE_RUN = 2,
	EMSPEC_INVISIBLE_ANI_TYPE_NSIZE = 3,
};

enum EMSPEC_SWAPPOS_DIR_TYPE
{
	EMSPEC_SWAPPOS_DIR_BOTH = 0,
	EMSPEC_SWAPPOS_DIR_SELF,
	EMSPEC_SWAPPOS_DIR_TARGET,
	EMSPEC_SWAPPOS_DIR_NSIZE,
};

enum EMCHAR_ACTSTATE
{
	EM_ACT_RUN					= 0x00000001L,		// 뛰기 플래그. ( off 시에는 걷기. )
	EM_ACT_CONTINUEMOVE			= 0x00000002L,		// 연속 이동 플래그.	
	EM_ACT_PEACEMODE			= 0x00000004L,		// 비경계 모드.
	EM_ACT_OBSERVE				= 0x00000008L,		// 관전 모드;
	EM_GETVA_AFTER				= 0x00000010L,		// GetViewAround() 확인 전인지 점검 플래그.
	EM_ACT_WAITING				= 0x00000020L,		// 캐릭터 대기 모드 플래그.
	EM_ACT_DIE					= 0x00000040L,		// 캐릭터 사망 모드 플래그.
	EM_REQ_GATEOUT				= 0x00000080L,		// 맵 진입후 대기 플래그.
	EM_REQ_LOGOUT				= 0x00000100L,		// 로그아웃 요청 플래그.
	EM_RECALLED_BY_CTF			= 0x00000200L,		// CTF 참여를 위해 Recall 중 //mjeon.CaptureTheField
	EM_REQ_VISIBLENONE			= 0x00000400L,		// 캐릭터 전혀 출력하지 않음.
	EM_REQ_VISIBLEOFF			= 0x00000800L,		// 캐릭터 자신에게만 반투명하게 보이기.
	EM_ACT_CONFT_WIN			= 0x00001000L,		// 대련 승리 플래그.
	EM_ACT_PK_MODE				= 0x00002000L,		// PK 모드 플래그.
	EM_ACT_CREATE				= 0x00004000L,		// Actor 생성 상태;
	EM_ACT_GESTURE				= 0x00008000L,		// Actor Gesture 상태;
	EM_ACT_MACROMODE			= 0x00010000L,		// 매크로 상태;
	EM_ACT_FEIGN_DIE			= 0x00020000L,		// 죽은척 상태,
	EM_ACT_EMERGENCY			= 0x00040000L,		// 긴급상태
	EM_ACT_ALL					= 0xffffffff,		// 모든 플래그를 해제할 때 사용한다;
};

enum EMSKILLCLASS
{
	EMSKILL_FIGHTER_01	=	0,
	EMSKILL_FIGHTER_02	=	1,
	EMSKILL_FIGHTER_03	=	2,
	EMSKILL_FIGHTER_04	=	3,

	EMSKILL_ARMS_01		=	4,
	EMSKILL_ARMS_02		=	5,
	EMSKILL_ARMS_03		=	6,
	EMSKILL_ARMS_04		=	7,

	EMSKILL_ARCHER_01	=	8,
	EMSKILL_ARCHER_02	=	9,
	EMSKILL_ARCHER_03	=	10,
	EMSKILL_ARCHER_04	=	11,

	EMSKILL_SPIRIT_01	=	12,
	EMSKILL_SPIRIT_02	=	13,
	EMSKILL_SPIRIT_03	=	14,
	EMSKILL_SPIRIT_04	=	15,

	EMSKILL_NPC_01		=	16,
	EMSKILL_NPC_02		=	17,
	EMSKILL_NPC_03		=	18,
	EMSKILL_NPC_04		=	19,
	EMSKILL_NPC_05		=	20,
	EMSKILL_NPC_06		=	21,
	EMSKILL_NPC_07		=	22,
	EMSKILL_NPC_08		=	23,
	EMSKILL_NPC_09		=	24,
	EMSKILL_NPC_10		=	25,

	EMSKILL_PET			=	26,
	EMSKILL_NPC_12		=	27,
	EMSKILL_NPC_13		=	28,
	EMSKILL_NPC_14		=	29,
	EMSKILL_EXTREME_01	=	30,
	EMSKILL_EXTREME_02	=	31,
	EMSKILL_EXTREME_03	=	32,
	EMSKILL_EXTREME_04	=	33,
	EMSKILL_NPC_19		=	34,
	EMSKILL_NPC_20		=	35,

	EMSKILL_SCIENTIST_01	=	36,
	EMSKILL_SCIENTIST_02	=	37,
	EMSKILL_SCIENTIST_03	=	38,
	EMSKILL_SCIENTIST_04	=	39,

	EMSKILL_LUNCHBOX		=   40,

	EMSKILL_SUMMON			=   41,

	EMSKILL_BIKE			=   42,

    EMSKILL_ASSASSIN_01 = 43,
    EMSKILL_ASSASSIN_02 = 44,
    EMSKILL_ASSASSIN_03 = 45,
    EMSKILL_ASSASSIN_04 = 46,

    EMSKILL_ETC_01      = 47,
    EMSKILL_ETC_02      = 48,
    EMSKILL_ETC_03      = 49,
    EMSKILL_ETC_04      = 50,

	EMSKILL_CTF				= 51,
	EMSKILL_EVENT			= 52,

	EMSKILL_EVENT_COUNTRY	= 53,
	EMSKILL_TEST					= 54,

	EMSKILL_TRICKER_00	= 55,
	EMSKILL_TRICKER_01	= 56,
	EMSKILL_TRICKER_02	= 57,
	EMSKILL_TRICKER_03	= 58,
	EMSKILL_TRICKER_04	= 59,

	EMSKILL_ACTOR_00	= 60,	// 멀티 wMain
	EMSKILL_ACTOR_01	= 61,
	EMSKILL_ACTOR_02	= 62,
	EMSKILL_ACTOR_03	= 63,
	EMSKILL_ACTOR_04	= 64,
	EMSKILL_ACTOR_05	= 65,
	EMSKILL_ACTOR_06	= 66,
	EMSKILL_ACTOR_07	= 67,
	EMSKILL_ACTOR_08	= 68,

	EMSKILLCLASS_NSIZE	=	69,
	EMSKILL_PC_CLASSNUM	=	10,
//	EMSKILL_PC_CLASSNUM	=	3,
};

//	Note : 대련 타입.
//
enum EMCONFT_TYPE
{
	EMCONFT_NONE	= 0,	//	없음. ( 지행하고 있지 않을때. )
	EMCONFT_ONE		= 1,	//	개인 대련.
	EMCONFT_PARTY	= 2,	//	파티 대련.
	EMCONFT_GUILD	= 3	//	길드 대련.
};

enum
{
	EMCONFT_COUNT	= 5,
};

enum
{
	EMSPECA_CAK_MAX = 25,
};

enum {
	DISGUISE_NONE,			// 없음
	DISGUISE_BEFORE_SKILL,	// 변장전 스킬
	DISGUISE_AFTER_SKILL,	// 변장후 스킬	
};

// 사용한 개조아이템 종류;
enum EMTYPE_ITEM_REMODEL
{
	EMTYPE_ITEM_REMODEL_NONE = 0,
	EMTYPE_ITEM_REMODEL_RANDOMOPTION = 1,
	EMTYPE_ITEM_REMODEL_BASICSTATS = 2,
	EMTYPE_ITEM_REMODEL_ADDSTATS = 3,
	EMTYPE_ITEM_REMODEL_ITEMSKILL = 4,
};

//	Note : 대련 로그.
//
struct SCONFT_LOG
{
	DWORD	dwCOUNT;		//	대련 횟수.
	float	fTIMER;			//	최근 대련후 경과.

	SCONFT_LOG () :
		dwCOUNT(0),
		fTIMER(0.0f)
	{
	}
	SCONFT_LOG ( DWORD _dwcount, float _ftimer ) :
		dwCOUNT(_dwcount),
		fTIMER(_ftimer)
	{
	}
};

struct SCONFT_OPTION
{
	bool	bSCHOOL;	//	스쿨 대련인지 검사.
	bool	bBOUND;		//	영역 제한 여부.
	WORD	wRECOVER;	//	회복약 사용 갯수 제한.

	float	fHP_RATE;		//	체력 확대 비율.
	float	fTAR_HP_RATE;	//	체력 확대 비율 [ 상대방(개인대련시만) ].

	//	학교 대련시에만.
	WORD	wMY_SCHOOL;
	WORD	wTAR_SCHOOL;

	SCONFT_OPTION () 
		: bSCHOOL(false)
		, bBOUND(true)
		, wRECOVER(USHRT_MAX)
		, fHP_RATE(1.0f)
		, fTAR_HP_RATE(1.0f)
		, wMY_SCHOOL(0)
		, wTAR_SCHOOL(0)
	{
	}

	void RESET ()
	{
		bSCHOOL = (false);
		bBOUND = (true);
		wRECOVER = (USHRT_MAX);
		fHP_RATE = (1.0f);
		fTAR_HP_RATE = (1.0f);

		wMY_SCHOOL = (0);
		wTAR_SCHOOL = (0);
	}

	bool VALID_OPT () const
	{
		if ( fHP_RATE < 1.0f )			return false;
		if ( fHP_RATE > 10.0f )			return false;

		if ( fTAR_HP_RATE < 1.0f )		return false;
		if ( fTAR_HP_RATE > 10.0f )		return false;

		return true;
	}

	bool operator!= ( const SCONFT_OPTION &rvalue ) const
	{
		if ( bBOUND!= rvalue.bBOUND )				return true;
		if ( wRECOVER!=rvalue.wRECOVER )			return true;
		if ( fHP_RATE!=rvalue.fHP_RATE )			return true;
		if ( fTAR_HP_RATE!=rvalue.fTAR_HP_RATE )	return true;
		return false;
	}

	bool operator== ( const SCONFT_OPTION &rvalue ) const
	{
		if ( bBOUND!= rvalue.bBOUND )				return false;
		if ( wRECOVER!=rvalue.wRECOVER )			return false;
		if ( fHP_RATE!=rvalue.fHP_RATE )			return false;
		if ( fTAR_HP_RATE!=rvalue.fTAR_HP_RATE )	return false;
		return true;
	}
};

struct SCONFTING
{
	EMCONFT_TYPE	emTYPE;
	DWORD			dwTAR_ID;
	DWORD			dwCHARID;
	float			fTIMER;

	D3DXVECTOR3		vPosition;

	SCONFT_OPTION	sOption;
	WORD			wRECOVER;

	bool			bFIGHT;
	float			fTIMER_FIGHT;
	float			fPOWERFULTIME;

	GLDWDATA		sBACKUP_HP;
	GLDWDATA		sBACKUP_MP;
	GLDWDATA		sBACKUP_SP;

	SCONFTING () 
		: emTYPE(EMCONFT_NONE)
		, dwTAR_ID(GAEAID_NULL)
		, dwCHARID(0)
		, fTIMER(0.0f)
		, vPosition(FLT_MAX,FLT_MAX,FLT_MAX)
		, wRECOVER(0)
		, bFIGHT(false)
		, fTIMER_FIGHT(EMCONFT_COUNT)
		, fPOWERFULTIME(0.0f)
		, sBACKUP_HP(1,1)
		, sBACKUP_MP(1,1)
		, sBACKUP_SP(1,1)
	{
	}

	void RESET ()
	{
		emTYPE = (EMCONFT_NONE);
		dwTAR_ID = (GAEAID_NULL);
		dwCHARID = 0;
		fTIMER = (0.0f);
		fPOWERFULTIME = (0.0f);

		vPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		sOption = SCONFT_OPTION();
		wRECOVER = 0;

		bFIGHT = false;
		fTIMER_FIGHT = EMCONFT_COUNT;
	}

	bool IsPOWERFULTIME () const
	{
		return ( fPOWERFULTIME > 0.0f );
	}

	bool IsCONFRONTING () const
	{
		return emTYPE != EMCONFT_NONE;
	}

	bool IsSAMENESS ( EMCONFT_TYPE _emtype, DWORD _dwtarid ) const
	{
		return _emtype==emTYPE && _dwtarid==dwTAR_ID;
	}

	bool IsFIGHTING () const
	{
		return bFIGHT;
	}

	bool IsRECOVE () const
	{
		if ( sOption.wRECOVER==USHRT_MAX )	return true;
		return ( wRECOVER < sOption.wRECOVER );
	}

	void SETPOWERFULTIME ( float fTime )
	{
		fPOWERFULTIME = fTime;
	} 

	void COUNTRECOVE ()
	{
		wRECOVER++;
	}

	bool UPDATE ( float fElaps )
	{
		if ( false==IsFIGHTING() )
		{
			fTIMER_FIGHT -= fElaps;
			return bFIGHT = ( fTIMER_FIGHT < 0.0f );
		}

		return false;
	}

	void UPDATEPWRFULTIME ( float fElaps )
	{
		fPOWERFULTIME -= fElaps;
		if ( fPOWERFULTIME < 0.0f )
			fPOWERFULTIME = 0.0f;
	}
};

struct SCONFTING_CLT
{
	typedef std::set<DWORD>     CONFT_MEM;
	typedef CONFT_MEM::iterator	CONFT_MEM_ITER;


	EMCONFT_TYPE	emTYPE;
	DWORD			dwTAR_ID;
	float			fTIMER;

	std::string		strTAR_NAME;
	CONFT_MEM		setConftMember;
	D3DXVECTOR3		vPosition;

	SCONFT_OPTION	sOption;
	WORD			wRECOVER;

	bool			bFIGHT;
	float			fTIMER_FIGHT;

	SCONFTING_CLT () :
		emTYPE(EMCONFT_NONE),
		dwTAR_ID(GAEAID_NULL),
		fTIMER(0.0f),
		vPosition(FLT_MAX,FLT_MAX,FLT_MAX),
		wRECOVER(0),

		bFIGHT(0),
		fTIMER_FIGHT(EMCONFT_COUNT)
	{
	}

	void RESET ()
	{
		emTYPE = (EMCONFT_NONE);
		dwTAR_ID = (GAEAID_NULL);
		fTIMER = (0.0f);

		strTAR_NAME.clear ();

		setConftMember.clear ();

		vPosition = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

		sOption = SCONFT_OPTION();
		wRECOVER = 0;

		bFIGHT = false;
		fTIMER_FIGHT = EMCONFT_COUNT;
	}

	bool IsCONFRONTING () const
	{
		return emTYPE != EMCONFT_NONE;
	}

	const char* GETTARNAME() const	{ return strTAR_NAME.c_str(); }

	bool IsFIGHTING () const
	{
		return bFIGHT;
	}

	bool IsRECOVE () const
	{
		if ( sOption.wRECOVER==USHRT_MAX )
            return true;
		return ( wRECOVER < sOption.wRECOVER );
	}

	void DoFIGHTING ()
	{
		bFIGHT = true;
	}

	void DEL_CONFT_MEMBER ( DWORD dwID )
	{
		CONFT_MEM_ITER pos = setConftMember.find(dwID);
		if ( pos!=setConftMember.end() )
            setConftMember.erase(pos);
	}

	DWORD UPDATE ( float fElaps )
	{
		if ( false==IsFIGHTING() )
		{
			bool bCOUNT = (int(fTIMER_FIGHT-fElaps)<int(fTIMER_FIGHT)) && (int(fTIMER_FIGHT)>=0);
			
			if ( int(fTIMER_FIGHT)>=-1 )
                fTIMER_FIGHT -= fElaps;
			else
                fTIMER_FIGHT = -1.0f;

			if ( !bFIGHT && bCOUNT )
                return int(fTIMER_FIGHT);
			return EMCONFT_COUNT;
		}

		return EMCONFT_COUNT;
	}
};

// 레벨 에디터에서 설정하는 특이 기능들
enum EMETCFUNC
{
	EMETCFUNC_LIMITTIME	  = 0, // 제한 시간
	EMETCFUNC_CONTROLCAM  = 1, // 카메라 제어
	EMETCFUNC_LANDEFFECT  = 2, // 지형 효과
	EMETCFUNC_DIFFICULTY  = 3, // 난이도 조절
	EMETCFUNC_AROUNDREGEN = 4, // 특정 몬스터 주변만 리젠
	EMETCFUNC_MOBCHASE	  = 5, // 모든 몬스터 해당 타겟이 죽을때까지 추적
	EMETCFUNC_REGENONCE	  = 6, // 몬스터 Gen을 한번만 하도록 설정
	EMETCFUNC_REENTRYLIMIT= 7, // 재입장시간 제어

	EMETCFUNC_SIZE,
};

enum EMLANDEFFECT_TYPE
{

	EMLANDEFFECT_ATK_SPEED		= 0,	//	공격속도
	EMLANDEFFECT_MOVE_SPEED		= 1,	//	이동속도

	EMLANDEFFECT_HP_RATE		= 2,	//	HP 증폭율.
	EMLANDEFFECT_MP_RATE		= 3,	//	MP 증폭율

	EMLANDEFFECT_RECOVER_RATE	= 4,	//	회복율
	EMLANDEFFECT_DAMAGE_RATE	= 5,	//	공격치 변화율
	EMLANDEFFECT_DEFENSE_RATE	= 6,	//	방어치 변화율
	EMLANDEFFECT_RESIST_RATE	= 7,	//	저항 수치

	EMLANDEFFECT_CANCEL_ALLBUFF	= 8,	//	모든 버프 취소
	EMLANDEFFECT_SAFE_ZONE		= 9,	//	세이프존

	EMLANDEFFECT_SIZE			= 10,	//	크기

	EMLANDEFFECT_MULTI			= 10,	//  최대 겹쳐지는 지형효과
};

enum EMMAP_EFFECT_TYPE
{
	EMMAP_EFFECT_NONE			= 0,	//	없음
	EMMAP_EFFECT_ATTACK_SPEED	= 1,	//	공격속도
	EMMAP_EFFECT_MOVE_SPEED		= 2,	//	이동속도

	EMMAP_EFFECT_HP_RATE		= 3,	//	HP증폭률
	EMMAP_EFFECT_MP_RATE		= 4,	//	MP증폭률
	EMMAP_EFFECT_ALL_RATE		= 5,	//	전체회복률

	EMMAP_EFFECT_DAMAGE_RATE	= 6,	//	공격력
	EMMAP_EFFECT_DEFENSE_RATE	= 7,	//	방어력
	
	EMMAP_EFFECT_SIZE			= 8,	//	크기
	EMMAP_EFFECT_MAX			= 10,	//	최대 적용가능한 지형효과
};

struct SGETWHISPERLIST
{
	DWORD dwClientID;
	DWORD dwGaeaID;
};


//	맵별로 적용되는 지형효과
struct SMAP_EFFECT
{
	EMMAP_EFFECT_TYPE	emType;
	float				fRate;
	
	SMAP_EFFECT()
		: emType ( EMMAP_EFFECT_NONE ) 
		, fRate ( 0.0f ) 
	{
	}

	bool operator == ( const SMAP_EFFECT &value )
	{
		if ( emType != value.emType ) return false;
		if ( fRate != value.fRate ) return false;

		return true;
	}

};

typedef std::vector<SMAP_EFFECT>		VEC_MAP_EFFECT;
typedef VEC_MAP_EFFECT::iterator		VEC_MAP_EFFECT_ITER;

//! Character 의 Point (PointShop)
struct CHARACTER_POINT
{
    __int64 m_PointRefundable; //! 환불가능 포인트
    __int64 m_PointNotRefundable; //! 환불불가 포인트

    CHARACTER_POINT()
        : m_PointRefundable(0)
        , m_PointNotRefundable(0)
    {
    }

    CHARACTER_POINT(
        __int64 PointRefundable,
        __int64 PointNotRefundable)
        : m_PointRefundable(PointRefundable)
        , m_PointNotRefundable(PointNotRefundable)
    {
    }

    inline __int64 Total() const { return m_PointRefundable+m_PointNotRefundable; }
    
    inline __int64 Refundable() const { return m_PointRefundable; }
    inline __int64 NotRefundable() const { return m_PointNotRefundable; }

    inline void SetRefundable(__int64 Point) { m_PointRefundable = Point; }
    inline void SetNotRefundable(__int64 Point) { m_PointNotRefundable = Point; }

    inline void AddRefundable(__int64 Point) { m_PointRefundable += Point; }
    inline void AddNotRefundable(__int64 Point) { m_PointNotRefundable += Point; }
};

namespace COMMENT
{
	extern std::string ACTIONTYPE[GLAT_SIZE];

	extern std::string TRIBE[TRIBE_NSIZE];
	extern std::string MOVETYPE[MOVETYPE_NSIZE];
	extern std::string ELEMENT[EMELEMENT_MAXNUM2];

	extern std::string BRIGHT[BRIGHT_SIZE];

	extern std::string CHARCLASS[GLCI_NUM_ACTOR];	
	extern std::string EMSCHOOL[GLSCHOOL_NUM];

	extern std::string CHARCOMMENT[GLCI_NUM_ACTOR];

	extern std::string TRANSFORM_SET[EMTRANSFORM_SET_MAXSIZE];
	extern std::string TFCHAR_SET[EMTRANSFORM_CHAR_SET_MAXSIZE];

	extern std::string BLOW[EMBLOW_SIZE];

	extern std::string BLOW_VAR1[EMBLOW_SIZE];
	extern float BLOW_VAR1_SCALE[EMBLOW_SIZE];
	inline bool IsBLOW1_PER ( EMSTATE_BLOW emTYPE )			{ return BLOW_VAR1_SCALE[emTYPE]==100.0f; }
	
	extern std::string BLOW_VAR2[EMBLOW_SIZE];
	extern float BLOW_VAR2_SCALE[EMBLOW_SIZE];
	inline bool IsBLOW2_PER ( EMSTATE_BLOW emTYPE )			{ return BLOW_VAR2_SCALE[emTYPE]==100.0f; }

	extern std::string IMPACT_ADDON[EIMPACTA_SIZE];
	extern float IMPACT_ADDON_SCALE[EIMPACTA_SIZE];
	inline bool IsIMPACT_ADDON_PER ( EMIMPACT_ADDON emTYPE )	{ return IMPACT_ADDON_SCALE[emTYPE]==100.0f; }
	
	extern int SPEC_CA[EMSPECA_CAK_MAX];
	extern std::string SPEC_ADDON[EMSPECA_NSIZE];
	extern std::string SPEC_ADDON_VAR1[EMSPECA_NSIZE];
	extern float SPEC_ADDON_VAR1_SCALE[EMSPECA_NSIZE];
	inline bool IsSPEC_ADDON1_PER ( EMSPEC_ADDON emTYPE )		{ return SPEC_ADDON_VAR1_SCALE[emTYPE]==100.0f; }

	extern std::string SPEC_ADDON_VAR2[EMSPECA_NSIZE];
	extern float SPEC_ADDON_VAR2_SCALE[EMSPECA_NSIZE];
	inline bool IsSPEC_ADDON2_PER ( EMSPEC_ADDON emTYPE )		{ return SPEC_ADDON_VAR2_SCALE[emTYPE]==100.0f; }
	
	extern std::string SPEC_STIGMA_TYPE[EMSPEC_STIGMA_TYPE_NSIZE];
	extern std::string SPEC_HALLUCINATE_TYPE[EMSPEC_HALLUCINATE_TYPE_NSIZE];
	extern std::string SPEC_DOMINATE_TYPE[EMSPEC_DOMINATE_TYPE_NSIZE];	
    extern std::string SPEC_ASIGN_TYPE[EMSPEC_SKILL_ASING_NSIZE];
     extern std::string SPEC_INC_TYPE[EMSPEC_INC_TYPE_NSIZE];
	extern std::string SPEC_INVISIBLE_USE_TYPE;
    extern std::string SPEC_SKILL_CONTINUE;
	extern std::string SPEC_INVISIBLE_ANI_TYPE[EMSPEC_INVISIBLE_ANI_TYPE_NSIZE];
	extern std::string SPEC_SWAPPOS_DIR_TYPE[EMSPEC_SWAPPOS_DIR_NSIZE];

	extern std::string IMPACT_TAR[EMIMPACT_TARGET_NSIZE];
	extern std::string IMPACT_REALM[EMIMPACT_REALM_NSIZE];
	extern std::string IMPACT_SIDE[EMIMPACT_SIDE_NSIZE];

	extern std::string SKILLCLASS[EMSKILLCLASS_NSIZE];

	extern std::string LANDEFFECT_TYPE[EMLANDEFFECT_SIZE];
}

// 고용 소환수를 위한 데이터;
// 필드 <-> 필드 간의 이동을 위해 GLCharDefine에 정의한다;
struct SHIRESUMMONINFO
{
	DWORD dwGaeaID;			// 고용 소환수의 GaeaID;
	bool bSummoning;		// 소환 상태인가?;
	bool bResummoning;		// 재소환 상태인가?;
	//bool bMoveMap;			// 맵 이동 상태인가?;

	float fRemainedTime;	// 남은 소환 시간;
	DWORD dwFamiliarType;	// AI Type;
	bool bEventSkill;		// 이벤트 스킬 사용 가능 여부;
	DWORD dwMaxSummon;		// 최대 소환 수;
	WORD wSkillLevel;		// 소환수 스킬 레벨;

	MSGPACK_DEFINE(
		dwGaeaID,
		bSummoning,
		bResummoning,
		fRemainedTime,
		dwFamiliarType,
		bEventSkill,
		dwMaxSummon,
		wSkillLevel );

	SHIRESUMMONINFO()
		: dwGaeaID( 0 )
		, bSummoning( false )
		, bResummoning( false )
		//, bMoveMap( false )
		, fRemainedTime( 0.f )
		, dwFamiliarType( 0 )
		, bEventSkill( true )
		, dwMaxSummon( 1 )
		, wSkillLevel( 0 )
	{

	}

	void RESET()
	{
		dwGaeaID = 0;
		bSummoning = false;
		//bMoveMap = false;
		bResummoning = false;
		fRemainedTime = 0.f;
		dwFamiliarType = 0;
		bEventSkill = true;
		dwMaxSummon = 1;
		wSkillLevel = 0;
	}

private:
	friend class boost::serialization::access;
	template < typename Archive >
	void serialize ( Archive& ar, const unsigned int nVersion )
	{
		ar & dwGaeaID;
		ar & bSummoning;
		ar & bResummoning;
		ar & fRemainedTime;
		ar & dwFamiliarType;
		ar & bEventSkill;
		ar & dwMaxSummon;
		ar & wSkillLevel;
	}
};

struct SSUMMONABLE
{
	bool bSummonable;			// 소환 가능 한가?;
	DWORD dwSummonLevel;		// 소환 가능 레벨;
	SHIRESUMMONINFO hireInfo;	// 고용 정보;

	MSGPACK_DEFINE(
		bSummonable,
		dwSummonLevel,
		hireInfo );

	SSUMMONABLE()
		: bSummonable( false )
		, dwSummonLevel( 0 )
	{

	}

	void RESET()
	{
		bSummonable = false;
		dwSummonLevel = 0;
		//hireInfo.RESET();
	}

	// 소환 상태를 해제한다;
	void Clear()
	{
		hireInfo.bSummoning = false;
		hireInfo.bResummoning = false;
	}

private:
	friend class boost::serialization::access;
	template < typename Archive >
	void serialize ( Archive& ar, const unsigned int nVersion )
	{
		ar & bSummonable;
		ar & dwSummonLevel;
		ar & hireInfo;
	}
};

enum CHARACTER_OPTION
{
	OPTION_NONE								= 0x00000000,
	OPTION_NEW_CHARACTER					= 0x00000001, // 신규 캐릭터, 절대 값 변경하면 안됨 
	OPTION_ITEM_RANDOMOPTION_INSERT_DONE	= 0x00000002, // 캐릭터 인벤토리, 장착창 아이템 랜덤옵션 로그 기록 완료
};

enum EMREBUILDTYPE
{
	EMREBUILDTYPE_NONE = 0,
	EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT	= 1,	// 개조 시도, 기존 상태를 유지하고 결과물을 받는다, 선택에 따라 결과물을 적용한다;
	EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL	= 2,	// 개조 시도, 결과물을 바로 적용하고 기존 정보를 저장시킨다;
};

enum EMSKILLRESET
{
	EMGRADE				= 31,
	EMMARBLE_MID		= 104,

	EMGRADE_MIN			= 4,
};
