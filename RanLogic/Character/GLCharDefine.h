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

//static const�� ���ͷ��̴�.
// MAP
static const int    MAP_NAME_LENGTH = 50;


// SMS
static const int    SMS_DEFAULT     = 0;
static const int    SMS_LENGTH      = 80; // SMS �� ��ü���� ����.
static const int    SMS_SENDER      = 14; // SMS ������ ��� ����.
static const int    SMS_RECEIVER    = 14;  // SMS �޴� ��� ����.


// CHARDEFINE
static const DWORD  GAEAID_NULL     = 0xFFFFFFFF;
static const int    CLUB_NULL       = 0;
static const int    PARTY_NULL      = 0;
static const int    EXPEDITION_NULL = 0;
static const DWORD  DBNUM_NULL      = GAEAID_NULL;
static const int    CHAR_SZNOTE     = 160;
static const int    CHAR_MAXSKILL   = 32;


// CLUB
static const int    EMCLUB_MARK_SX	        = 16; // Ŭ����ũ�� �� 16 �ȼ�
static const int    EMCLUB_MARK_SY	        = 11; // Ŭ����ũ�� ���� 11 �ȼ�
static const int    EMCLUB_MARK_SIZE        = sizeof(DWORD)*EMCLUB_MARK_SX*EMCLUB_MARK_SY;
static const int    EMCLUB_NOTICE_LEN_LINE	= 40;
static const int    EMCLUB_NOTICE_LINE		= 10;
// Ŭ�� ������ ���ڼ� ���� ( ���ڼ� * ���� + ( ���ΰ���ǥ�� )  ) 								   
static const int    EMCLUB_NOTICE_LEN		=  ( EMCLUB_NOTICE_LEN_LINE * EMCLUB_NOTICE_LINE ) + (( EMCLUB_NOTICE_LINE - 1 ) * 2 );


// LAND
static const int    MAXITEM			= 3000;		//	SUIT, ��� ���� �Ҽ� �ִ� �ִ� ����.
static const int    MAXMONEY		= 3000;		//	MONEY, ��� ���� �Ҽ� �ִ� �ִ� ����.
static const int    MAXCROW			= 3000;		//	CROW, ��� ���� �Ҽ� �ִ� �ִ� ����.
static const int    MAXPET			= 3000;		//  PET, �ʻ� ���� �Ҽ� �ִ� �ִ� ����.
static const int    MAXSUMMON		= 3000;		//	SUMMON, ��� ���� �Ҽ� �ִ� �ִ� ����.
static const int    MAXLANDEVENT    = 3000;     //  LANDEVENT, �ʻ� ���� �Ҽ� �ִ� �ִ� ����

// Max;
static const int	MAXOFFENCE		= 99999;
static const int	MAXDEFENCE		= 9999;
static const int	MAXREGIST		= 999;
static const int	MAXACCURACY		= 999;
static const int	MAXAVOIDANCE	= 999;

//! ĳ���� �ε���
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

//! ĳ���� ���� FLAG (������ WidgetDef.lua ���ϵ� ���� ����)
enum EMCHARCLASS
{
	GLCC_NONE			= 0X00000000,	//	�߰�, �ƹ��͵� ���õ��� ���� ����

	GLCC_FIGHTER_M		= 0x00000001,	// GLCC_FIGHTER_M���� CHANGE...
	GLCC_ARMS_M			= 0x00000002,	// GLCC_ARMS_M���� CHANGE...
	GLCC_ARCHER_W		= 0x00000004,	// GLCC_ARCHER_W���� CHANGE...
	GLCC_SPIRIT_W		= 0x00000008,	// GLCC_SPIRIT_W���� CHANGE...

	GLCC_EXTREME_M		= 0x00000010,   // �ذ��� ��
	GLCC_EXTREME_W		= 0x00000020,   // �ذ��� ��

	GLCC_FIGHTER_W		= 0x00000040,
	GLCC_ARMS_W			= 0x00000080,
	GLCC_ARCHER_M		= 0x00000100,
	GLCC_SPIRIT_M		= 0x00000200,

	GLCC_SCIENTIST_M	= 0x00000400,	//	���к� ��
	GLCC_SCIENTIST_W	= 0x00000800,	//	���к� ��

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
	GLCL_FIGHTER		= 0, // ������;
	GLCL_ARMS			= 1, // �˵���;
	GLCL_ARCHER		= 2, // ��ú�;
	GLCL_SPIRIT		= 3, // �⿹��;
	GLCL_EXTREME	= 4, // �ذ���;
	GLCL_SCIENTIST	= 5, // ���к�;
    GLCL_ASSASSIN	= 6, // �μ���;
	GLCL_TRICKER		= 7, // ������;
    GLCL_ETC			= 8, // ��Ÿ;
	GLCL_ACTOR		= 9,
	GLCL_NUM_CLASS	= 10, // Ŭ���� ����
};

EMCLASSINDEX CharClassToClassIndex ( const EMCHARCLASS emClass );
EMCHARINDEX CharClassToIndex( const EMCHARCLASS emClass );
EMCHARCLASS CharIndexToClass( const EMCHARINDEX emIndex );
EMCHARCLASS CharClassToSex( const EMCHARCLASS emClass );
EMCHARINDEX CharIndexToSex( const EMCHARINDEX emIndex );
EMCHARCLASS CharSexandClassIndexToCharClass( const int nSex, 
											 const EMCLASSINDEX emclassIndex );

VOID GetCharSkillClassIndex( const EMCHARCLASS emClass, WORD & wBeginIndex, WORD & wEndIndex );

//	Note : ĳ���� �ܼ�ȭ ���� �ε���.
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
	float		fPow;	//	��.
	float		fStr;	//	ü��.
	float		fSpi;	//	����.
	float		fDex;	//	��ø.
	float		fInt;	//	����.
	float		fSta;	//	�ٷ�.

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
	WORD		wPow;	//	��.   Power
	WORD		wStr;	//	ü��. String
	WORD		wSpi;	//	����. Spirit
	WORD		wDex;	//	��ø. Dex
	WORD		wInt;	//	����. Inteligent
	WORD		wSta;	//	�ٷ�. Strength

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

	//	Note : ������ ��ġ�� �䱸��ġ�� ���յǴ��� �˻�.
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
    std::string  m_ItemName;        //! ������ �̸� [MAP_NAME_LENGTH]
	DWORD		 dwReqCharClass;			  // ���� Ŭ����
	DWORD		 dwSuitType;				  // ������ Ÿ��
	WORD		 wReqLevel;					  // ���� �ּ� ����
	SCHARSTATS	 sReqStats;					  // ������ ���� ����ġ
	LONGLONG	 llPRICE;					  // ������ ����
		
	BYTE		 RandOptTYPE1;				// ���� �ɼ�Ÿ�� 1
	BYTE		 RandOptTYPE2;				// ���� �ɼ�Ÿ�� 2
	BYTE		 RandOptTYPE3;				// ���� �ɼ�Ÿ�� 3
	BYTE		 RandOptTYPE4;				// ���� �ɼ�Ÿ�� 4

	float		 fRandOptValue1;			// ���� �ɼǰ� 1	
	float		 fRandOptValue2;			// ���� �ɼǰ� 2
	float		 fRandOptValue3;			// ���� �ɼǰ� 3	
	float		 fRandOptValue4;			// ���� �ɼǰ� 4


	BYTE		 cDAMAGE;					//	���ݷ� ���� ���.
	BYTE		 cDEFENSE;					//	���� ���� ���.

	BYTE		 cRESIST_FIRE;				//	����(ȭ) ���� ���.
	BYTE		 cRESIST_ICE;				//	����(��) ���� ���.
	BYTE		 cRESIST_ELEC;				//	����(��) ���� ���.
	BYTE		 cRESIST_POISON;			//	����(��) ���� ���.
	BYTE		 cRESIST_SPIRIT;			//	����(��) ���� ���.

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
	EMELEMENT_OLDMAX		= 5,	// ���� �ִ�ġ
	
	EMELEMENT_STONE			= 5,	
	EMELEMENT_MAD			= 6,
	EMELEMENT_STUN			= 7,
	EMELEMENT_CURSE			= 8,	
	EMELEMENT_MAXNUM		= 9,	// New
	
	EMELEMENT_ARM			= 9,
	//	element �Ӽ��� ���⿡ �������� ���.

	EMELEMENT_MAXNUM2		= 10,
};


//	Note : �� 100 ������ �������� ����.
struct SRESIST_100
{	
	int		nFire;		//	��.
	int		nIce;		//	����.
	
	int		nElectric;	//	����.
	int		nPoison;	//	��.

	int		nDivine;	//	��.

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
	short	nFire;		//	��.
	short	nIce;		//	����.
	
	short	nElectric;	//	����.
	short	nPoison;	//	��.
};

struct SRESIST
{	
	short	nFire;		//	��.
	short	nIce;		//	����.
	
	short	nElectric;	//	����.
	short	nPoison;	//	��.

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
	GLAT_SKILL,				// Skill �� ���� ����ϴ� �κ�.
	GLAT_SHOCK,
	GLAT_PUSHPULL,    // ���ο� EMACTIONTYPE�� �߰��� ���, m_bSTATE_STUN �÷��׷� ���� FrameMove�Լ����� GLAT_SHOCK�� ��������� �Ϳ� ������ ��
	GLAT_GATHERING,
	GLAT_TALK,
	GLAT_CONFT_END,
	GLAT_MOTION,

	GLAT_AUTHED,	//mjeon.CaptureTheField - Ư�� �п��� ���� Crow�� �����Ǿ���(GLAT_FALLING ���).
	GLAT_FALLING,
	GLAT_DIE,
	GLAT_FEIGN_FALLING,
	GLAT_FEIGN_DIE,

	GLAT_SKILL_WAIT,		// Skill �� ����ϸ� AniThread ������ �����̰� ���ܼ� ����ϵ��� �Ѵ�.

	GLAT_SIZE,
};
typedef EMACTIONTYPE*		PEMACTIONTYPE;

enum EMTRIBE
{
	TRIBE_HUMAN			= 0,	//	�ΰ�.
	TRIBE_MONSTER		= 1,	//	����.
	TRIBE_ANGEL			= 2,	//	õ��.
	TRIBE_DEVIL			= 3,	//	�Ǹ�.
	TRIBE_SPIRIT		= 4,	//	����.
	TRIBE_FAIRY			= 5,	//	����.
	TRIBE_DRAGON		= 6,	//	�巡��.

	TRIBE_NSIZE			= 7		
};

enum EMBRIGHT
{
	BRIGHT_LIGHT		= 0,
	BRIGHT_DARK			= 1,
	
	BRIGHT_BOTH			= 2,	//	��� ����. ( item �׸񿡼�. )

	BRIGHT_SIZE			= 3,
};

enum EM_BRIGHT_FB
{
	BFB_DIS				= 0,	//	�Ҹ�.
	BFB_AVER			= 1,	//	����.
	BFB_ADV				= 2,	//	����.

	BFB_SIZE			= 3,
};

enum EMMOVETYPE
{
	MOVE_LAND			= 0,	//	���� �̵�.
	MOVE_FLY			= 1,	//	���� �̵�.
	MOVE_SWIM			= 2,	//	���� �̵�.

	MOVETYPE_NSIZE		= 3
};

enum EMIMPACT_TARGET
{
	TAR_SELF		= 0,		//	�ڱ� ��ġ ����.
	TAR_SPEC		= 1,		//	��� ��ġ ����.
	TAR_SELF_TOSPEC	= 2,		//	�ڱ� ��ġ���� ��� ��ġ����. ( ȭ�� ���� )
	TAR_ZONE		= 3,		//	������ġ�� �ֺ� Ÿ�ٵ��� �����´�.
	TAR_LAND		= 4,		//  ������ġ

	EMIMPACT_TARGET_NSIZE		= 5,
};

enum EMIMPACT_REALM
{
	REALM_SELF		= 0,		//	'��ǥ' �ڽ�.
	REALM_ZONE		= 1,		//	'��ǥ' ����.
	REALM_KEEP_ZONE	= 2,		//	'��ǥ' ���� ����.
	REALM_FANWIZE	= 3,		//	'��ǥ'�� ���� ��ä�� ���.

	EMIMPACT_REALM_NSIZE		= 4
};

enum EMIMPACT_SIDE
{
	EMIMPACT_SIDE_PARTY		= 0,		//	�ڱ�����.
	EMIMPACT_SIDE_ENEMY		= 1,		//	������.
	EMIMPACT_SIDE_ANYBODY	= 2,		//	��ο���.

	EMIMPACT_SIDE_NSIZE		= 3,


    // �� �κ��� ��ųŸ�ٽý��ۿ��� ���� �κ����� GetSelectTargetAround (�ֺ� Ÿ�� �˻�) ������ �ӽ÷� ������� ��;
    // !�ٸ��������� ��� ����;
    // ���� ��ųŸ�� ���� ���� ENUM������ ��ü�Ǿ�� ��;
    EMIMPACT_SIDE_OUR       = 4,        // �Ʊ�����;

// ��ų Ÿ�� �ý����� ����ÿ� �߰��ɸ��� ���׿� ���ؼ� �ۼ��س��� �κ�;
// (������ ����� �������� ������ ������� ����);
    //EMIMPACT_SIDE_ALLY      = 0x0010,   // �Ʊ�;
    //EMIMPACT_SIDE_NEUTRAL   = 0x0020,   // �߸�;
    //EMIMPACT_SIDE_ENEMY     = 0x0040,   // ��;
    //EMIMPACT_SIDE_FACTION   = 0x0080,   // �ڽ��� ����;

    //EMIMPACT_SIDE_PARTY     = 0x0100,   // �ڽ��� ��Ƽ;
    //EMIMPACT_SIDE_CLUB      = 0x0200,   // �ڽ��� Ŭ����;
    //EMIMPACT_SIDE_EXPEDITION= 0x0400,   // �ڽ��� ������;
    //EMIMPACT_SIDE_SCHOOL    = 0x0800,   // �ڽ��� �п���;
};

enum EMDISORDER
{
	DIS_NULL			= 0x00,	// ����
	DIS_NUMB			= 0x01,	// ����.
	DIS_STUN			= 0x02,	// ����.
	DIS_STONE			= 0x04,	// ��ȭ.
	DIS_BURN			= 0x08,	// ȭ��.
	DIS_FROZEN			= 0x10,	// �õ�.

	DIS_MAD				= 0x20,	// ����.
	DIS_POISON			= 0x40,	// �ߵ�.
	DIS_CURSE			= 0x80,	// ����.

	DIS_ALL				= (DIS_NUMB | DIS_STUN | DIS_STONE | DIS_BURN | DIS_FROZEN | DIS_MAD | DIS_POISON | DIS_CURSE),
};

// ��ųȿ�� EMSPECA_FEIGN_DEATH üũ �÷���
enum EMFEIGNDEATH
{
	FEIGN_DEATH_TARGETING_DISABLE = 0x01,
	FEIGN_DEATH_BUF_DEL			  = 0x02,

	FEIGN_DEATH_ALL     = (FEIGN_DEATH_TARGETING_DISABLE | FEIGN_DEATH_BUF_DEL),
};
//	���� �̻� ȿ��.
//
enum EMSTATE_BLOW
{
	EMBLOW_NONE		= 0,	//	���� �̻� ���� ����.
	EMBLOW_NUMB		= 1,	//	����.
	EMBLOW_STUN		= 2,	//	����.
	EMBLOW_STONE		= 3,	//	��ȭ.
	EMBLOW_BURN			= 4,	//	ȭ��.
	EMBLOW_FROZEN		= 5,	//	�õ�.
	EMBLOW_SINGLE		= 5,	//	�ܵ����θ� �߻��ϴ� "�����̻�" ȿ��.

	EMBLOW_MAD			= 6,	//	����.
	EMBLOW_POISON		= 7,	//	�ߵ�.
	EMBLOW_CURSE		= 8,	//	����.
	EMBLOW_PANT		= 9,	//	�涱��.
	
	EMBLOW_SIZE			= 10,
	EMBLOW_MULTI		= 4,
};

enum EMSTATE_BLOW_FLAG
{
	EMBLOW_FLAG_NULL			= 0x00,	//	���� �̻� ���� ����.
	EMBLOW_FLAG_NUMB		= (1 << EMBLOW_NUMB),		//	����.
	EMBLOW_FLAG_STUN		= (1 << EMBLOW_STUN),		//	����.
	EMBLOW_FLAG_STONE		= (1 << EMBLOW_STONE),		//	��ȭ.
	EMBLOW_FLAG_BURN		= (1 << EMBLOW_BURN),		//	ȭ��.
	EMBLOW_FLAG_FROZEN		= (1 << EMBLOW_FROZEN),	//	�õ�.	
	EMBLOW_FLAG_MAD			= (1 << EMBLOW_MAD),		//	����.
	EMBLOW_FLAG_POISON		= (1 << EMBLOW_POISON),	//	�ߵ�.
	EMBLOW_FLAG_CURSE		= (1 << EMBLOW_CURSE),		//	����.	
	EMBLOW_FLAG_PANT		= (1 << EMBLOW_PANT),		//	����.	
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
	EMIMPACTA_HITRATE		= 1,	//	������ ��ȭ��.
	EMIMPACTA_AVOIDRATE		= 2,	//	ȸ���� ��ȭ��.
	
	EMIMPACTA_DAMAGE		= 3,	//	����ġ ��ȭ��.
	EMIMPACTA_DEFENSE		= 4,	//	���ġ ��ȭ��.

	EMIMPACTA_VARHP			= 5,	//	HP ��ȭ��.
	EMIMPACTA_VARMP			= 6,	//	MP ��ȭ��.
	EMIMPACTA_VARSP			= 7,	//	SP ��ȭ��.
	EMIMPACTA_VARAP			= 8,	//	HP,MP,SP ��ȭ��.

	EMIMPACTA_DAMAGE_RATE	= 9,	//	����ġ ��ȭ��.
	EMIMPACTA_DEFENSE_RATE	= 10,	//	���ġ ��ȭ��.

	EMIMPACTA_PA			= 11,	//	����ġ.
	EMIMPACTA_SA			= 12,	//	���ġ.
	EMIMPACTA_MA			= 13,	//	���ġ.

	EMIMPACTA_HP_RATE		= 14,	//	HP ������.
	EMIMPACTA_MP_RATE		= 15,	//	MP ������.
	EMIMPACTA_SP_RATE		= 16,	//	SP ������.

	EMIMPACTA_RESIST		= 17,	//	���� ��ġ ����
	EMIMPACTA_STATS			= 18,	//	���� ��ġ ���� ( ��,��ø,���� ) 

    EMIMPACTA_HP_ADD        = 19,  //  HP ȸ���� ����
    EMIMPACTA_MP_ADD        = 20,  //  MP ȸ���� ����
    EMIMPACTA_SP_ADD        = 21,  //  SP ȸ���� ����
    EMIMPACTA_CP_ADD        = 22,  //  CP ȸ���� ����

    EMIMPACTA_CP_DUR        = 23,  //  CP �ڵ����� ����

	EIMPACTA_SIZE			= 24,
};

#define SKILL_ADDITIONAL_ID( skillid )					(skillid | 0xFFFF0000)
#define IS_ADDITIONAL_ID( skillid )						((skillid >> 16) == 0xFFFF)
#define SKILL_TO_ADDITIONAL( skillid )					(skillid & 0xFFFF)

//�߰��� �ƴ϶� �߰� emum �ε����� ���� �Ǹ� GameWord.xml, ITEM_INFO_SET_BONUS �ε����� ���� �����������
enum EMSPEC_ADDON
{
	EMSPECA_NULL						= 0,		//	��� ����.
	EMSPECA_PUSHPULL					= 1,		//	����/�б�.
	EMSPECA_REFDAMAGE				= 2,		//	����� �ݻ�.
	EMSPECA_REBIRTH					= 3,		//	��Ȱ.
	
	EMSPECA_HP_GATHER				= 4,		//	HP�� �������� ������.
	EMSPECA_MP_GATHER				= 5,		//	MP�� �������� ������.
	EMSPECA_SP_GATHER				= 6,		//	SP�� �������� ������.

	EMSPECA_HP_DIV						= 7,		//	HP�� �Ϻθ� ������ ��.
	EMSPECA_MP_DIV					= 8,		//	MP�� �Ϻθ� ������ ��.
	EMSPECA_SP_DIV						= 9,		//	SP�� �Ϻθ� ������ ��.

	EMSPECA_NONBLOW					= 10,		//	�����̻� ����.
	EMSPECA_RECBLOW					= 11,		//	�����̻� ȸ��.

	EMSPECA_PIERCE						= 12,		//	��������.
	EMSPECA_RANGE_PSY_LONG		= 13,		//	���� ����� �����Ÿ� �� ���뿵�� ����( ���� )

	EMSPECA_MOVEVELO				= 14,		//  �̵��ӵ�.
	EMSPECA_ONWARD					= 15,		//  ����.
	EMSPECA_INVISIBLE					= 16,		//	����.
	EMSPECA_RECVISIBLE				= 17,		//	���� ����.

	EMSPECA_ATTACKVELO				= 18,		//	���ݼӵ�.
	EMSPECA_SKILLDELAY				= 19,		//	��ų ������ �ð� ��ȭ.
	EMSPECA_CRUSHING_BLOW		= 20,		//	����Ÿ�� ��ų

	EMSPECA_PSY_DAMAGE_REDUCE				= 21,		//  ���� ������ �����
	EMSPECA_MAGIC_DAMAGE_REDUCE			= 22,		//  ���� ������ �����
	EMSPECA_PSY_DAMAGE_REFLECTION		= 23,		//  ���� ������ �ݻ���
	EMSPECA_MAGIC_DAMAGE_REFLECTION	= 24,		//  ���� ������ �ݻ���

	EMSPECA_EFFECT_REMOVE			= 25,		//	ȿ�� ���� ( �Ϲ�,����,�����,����Ʈ �������� ) 
	EMSPECA_STUN							= 26,		//	���� ȿ��

	EMSPECA_DEFENSE_SKILL_ACTIVE	= 27,		//	�ǰݽ� �ߵ� 
	EMSPECA_PULLIN							= 28,		//	���� ����
	EMSPECA_KNOCKBACK					= 29,		//	�о� ����

	EMSPECA_DURATION_DAMAGE		= 30,		//	���ӵ�����
	EMSPECA_CURSE							= 31,		//	����

	EMSPECA_TARRANGE_ALL				= 32,		//	��ų �����Ÿ� ��ȭ
	EMSPECA_APPLYRANGE_ALL			= 33,		//	��ų ���뿵�� ��ȭ

	EMSPECA_NON_DRUG					= 34,		//	���� ��� ���� ( ��ǰ�� ��ü ���� ) 
	EMSPECA_NON_SKILL					= 35,		//	��ų ��� ���� ( ��ǰ�� ��ü ���� ) 

	EMSPECA_IGNORED_DAMAGE			= 36,		//	������ ���(����)

    EMSPECA_ITEMDROP_RATE			= 37,		//	�����۵����
    EMSPECA_MONEYDROP_RATE			= 38,		//	�ݾ׵����
    EMSPECA_EXP_RATE						= 39,		//	����ġ������
    EMSPECA_NPC_TALK						= 40,		//	Ư�� NPC ��ȭ
    EMSPECA_ITEMDROP_SPECIFIC		= 41,		//	Ư�� ������ ���

	EMSPECA_TELEPORT						= 42,		//	�����̵�
	EMSPECA_SWAPPOS						= 43,		//	��ġ����

	EMSPECA_INCREASE_EFF				= 44,		//	��ȭ ȿ��( ȿ�� ��� ������ ��ȭ )
	EMSPECA_ILLUSION						= 45,		//	���ȿ��
    EMSPECA_VEHICLE_GETOFF			= 46,		//  ����ȿ��
    EMSPECA_ENDURE_VARY				= 47,		//  ���ӽð� ����

    EMSPECA_STIGMA						= 48,		//  ����ȿ��    
    EMSPECA_HALLUCINATE				= 49,		//  ȯ��ȿ��
    EMSPECA_RELEASE_ENDURE			= 50,		//  �ǰݽ� ȿ�� ���� 
    EMSPECA_LINK_HP						= 51,		//  ��ȿ��( HP ��ũ )
    EMSPECA_RELEASE_PET				= 52,		//  ����ȿ��( �� ���� )
    EMSPECA_IMMUNE						= 53,		//  �鿪ȿ��
    EMSPECA_TURN_STATE_BATTLE		= 54,		//  �������� ȿ��( ���� ���·� �ٲ� )
    EMSPECA_TURN_STATE_PEACE		= 55,		//  �������� ����( ��ȭ ���·� �ٲ� )
    EMSPECA_TAUNT							= 56,		//  ���� ȿ��    
	EMSPECA_DOMINATION					= 57,		//  ���� ȿ��
	EMSPECA_RANDOM_EXP_RATE		= 58,		//  ���� ����ġ ������;

	EMSPECA_SKILL_LINK					= 59,		//  ��ų ��ũ
	
	//Random_opt EMIMPACTA ��� �߰� 
	//�ε��� ��ȣ�� ���� �Ǹ� GameWord.xml, ITEM_INFO_SET_BONUS �ε���, comment.lua�� ���� �����������
	EMSPECA_DAMAGE_VALUE				= 60,		//	���ݷ�(����).
	EMSPECA_DAMAGE_RATE				= 61,		//	���ݷ� % ����.

	EMSPECA_DEFENSE_VALUE				= 62,		//	����(����).
	EMSPECA_DEFENSE_RATE				= 63,		//	���� ��ü % ����.
	
	EMSPECA_HIT_VALUE					= 64,		//	������(����).
	EMSPECA_HIT_RATE						= 65,		//	������ ��ü % ����

	EMSPECA_AVOID_VALUE				= 66,		//	ȸ����(����).
	EMSPECA_AVOID_RATE					= 67,		//	ȸ���� ��ü % ���� 
	
	EMSPECA_HP_VALUE						= 68,		//	hp ����.
	EMSPECA_MP_VALUE						= 69,		//	mp ����.
	EMSPECA_SP_VALUE						= 70,		//	sp ����.
	EMSPECA_CP_VALUE						= 71,		//	cp ����.  %ó������ ����%

	EMSPECA_HP_VARIATION_RATE		= 72,		//	hp ��ȭ��(Ư��ȿ��).
	EMSPECA_MP_VARIATION_RATE		= 73,		//	mp ��ȭ��(Ư��ȿ��).
	EMSPECA_SP_VARIATION_RATE		= 74,		//	sp ��ȭ��(Ư��ȿ��).
	EMSPECA_HMS_VARIATION_RATE		= 75,		//	hp, mp, sp ��ȭ��(Ư��ȿ��).

	EMSPECA_ATTACK_RANGE_VALUE	= 76,		//	���� ����.
	EMSPECA_DIS_SP_VALUE				= 77,		//	sp �Ҹ�.
	EMSPECA_RESIST_VALUE				= 78,		//	����.

	EMSPECA_STATS_POW_VALUE		= 79,		//	��(����).
	EMSPECA_STATS_STR_VALUE			= 80,		//	ü��(����).
	EMSPECA_STATS_SPI_VALUE			= 81,		//	����(����).
	EMSPECA_STATS_DEX_VALUE			= 82,		//	��ø(����).
	EMSPECA_STATS_STA_VALUE		= 83,		//	�ٷ�(����).

	EMSPECA_PA_VALUE						= 84,		//	����ġ(����).
	EMSPECA_SA_VALUE						= 85,		//	���ġ(����).
	EMSPECA_MA_VALUE					= 86,		//	����ġ(����).

	EMSPECA_HP_POTION_INC_VALUE	= 87,		//	���� hp ������(����).
	EMSPECA_MP_POTION_INC_VALUE	= 88,		//	���� mp ������(����).
	EMSPECA_SP_POTION_INC_VALUE	= 89,		//	���� sp ������(����).

	EMSPECA_CP_INC_VALUE				= 90,		//	���� ų�� CP ������(����).
	EMSPECA_MOVEVELO_VALUE			= 91,		// �̵��ӵ� ����(����)
	EMSPECA_DAMAGE_RATE_ALL			= 92,		// ������ (�ٰŸ�, ��� ����ġ ���� ���� ���� ��ġ���� ������)
	EMSPECA_VIEW_RANGE_RATE			= 93,		// �þ� ����;
	EMSPECA_AIRBORNE				= 94,		// ���;

	EMSPECA_SUMMON_CHANGE_SKILLPAGE	= 95,	// ��ȯ�� ��ų ������ ����;
	EMSPECA_SUMMON_EVENT_SKILL		= 96,	// ��ȯ�� �̺�Ʈ ��ų;
	EMSPECA_SUMMON_TARGETING		= 97,	// ��ȯ�� Ÿ�� ����;
	EMSPECA_SUMMON_RUNAWAY			= 98,	// ��ȯ�� ����;
	EMSPECA_DELAY_ACTION_SKILL		= 99,	// �����ߵ�
	EMSPECA_CLONE					= 100,	// ����ȿ��
	
    EMSPECA_MOVEVELO_LIMIT          = 101,  //  �̵��ӵ� ����;
	EMSPECA_COUNTATTACK				= 102,  // �ݰ�

	EMSPECA_SCREEN_THROW			= 103,  // ȭ����ô
	EMSPECA_ENTRANCE_EXIT			= 104,  // ���ⱸ
	EMSPECA_FEIGN_DEATH				= 105,  // ����ô
	EMSPECA_CAMERA_HIT				= 106,  // ī�޶�ġ��
	EMSPECA_DISGUISE				= 107,  // ����
	EMSPECA_PROVOKE					= 108,  // ����
	EMSPECA_SPECIAL_IMMUNE			= 109,  // Ư��ȿ���鿪
	EMSPECA_ACTIVATE_DAMAGE			= 110,  // ���عߵ�
	EMSPECA_REMOVE_SKILL_EFFECT		= 111,	// ��ųȿ�� ���� ( �� ȿ������ ���� );


	EMSPECA_NSIZE
};

enum EMSPEC_RELEASE_ENDURE_TYPE // �ǰݽ� ȿ�� ���� ���� Ÿ��;
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

enum EMSPEC_STIGMA_TYPE // ���� ȿ�� ���� Ÿ��
{		
	EMSPEC_STIGMA_TYPE_DAMAGE_NORMAL    = 0,
	EMSPEC_STIGMA_TYPE_DAMAGE_SKILL     = 1,
	EMSPEC_STIGMA_TYPE_BUFF_SKILL       = 2,
	EMSPEC_STIGMA_TYPE_DEBUFF_SKILL     = 3,
	EMSPEC_STIGMA_TYPE_LIMIT_SKILL      = 4,

	EMSPEC_STIGMA_TYPE_NSIZE            = 5,
};

enum EMSPEC_STIGMA_TYPE_DWORD // ���� ȿ�� ���� Ÿ�� �÷���
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
	EMSPEC_DOMINATE_TYPE_RANDOM = 2,	// ����, �̵� ��ġ ����;
	EMSPEC_DOMINATE_TYPE_FIX = 3,		// ����, �̵�X, ��ġ ����;
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
    EMSPEC_INC_TYPE_PLUS  = 0,//  ����
    EMSPEC_INC_TYPE_MINUS   = 1,// ����

    EMSPEC_INC_TYPE_NSIZE   = 2,
};

enum EMSPEC_ASIGN_TYPE
{
    EMSPEC_SKILL_ASING_BEFORE  = 0,// ���� ����ȿ��
    EMSPEC_SKILL_ASING_AFTER   = 1,// ���� ����ȿ��

    EMSPEC_SKILL_ASING_NSIZE   = 2,
};

enum EMSPEC_ASIGN_TYPE_DWORD
{
    EMSPEC_SKILL_ASING_DWORD_NULL   = 0x00,
    EMSPEC_SKILL_ASING_DWORD_BEFORE = ( 1 << EMSPEC_SKILL_ASING_BEFORE ),// ���� ����ȿ��
    EMSPEC_SKILL_ASING_DWORD_AFTER  = ( 1 << EMSPEC_SKILL_ASING_AFTER  ),// ���� ����ȿ��
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
	EM_ACT_RUN					= 0x00000001L,		// �ٱ� �÷���. ( off �ÿ��� �ȱ�. )
	EM_ACT_CONTINUEMOVE			= 0x00000002L,		// ���� �̵� �÷���.	
	EM_ACT_PEACEMODE			= 0x00000004L,		// ���� ���.
	EM_ACT_OBSERVE				= 0x00000008L,		// ���� ���;
	EM_GETVA_AFTER				= 0x00000010L,		// GetViewAround() Ȯ�� ������ ���� �÷���.
	EM_ACT_WAITING				= 0x00000020L,		// ĳ���� ��� ��� �÷���.
	EM_ACT_DIE					= 0x00000040L,		// ĳ���� ��� ��� �÷���.
	EM_REQ_GATEOUT				= 0x00000080L,		// �� ������ ��� �÷���.
	EM_REQ_LOGOUT				= 0x00000100L,		// �α׾ƿ� ��û �÷���.
	EM_RECALLED_BY_CTF			= 0x00000200L,		// CTF ������ ���� Recall �� //mjeon.CaptureTheField
	EM_REQ_VISIBLENONE			= 0x00000400L,		// ĳ���� ���� ������� ����.
	EM_REQ_VISIBLEOFF			= 0x00000800L,		// ĳ���� �ڽſ��Ը� �������ϰ� ���̱�.
	EM_ACT_CONFT_WIN			= 0x00001000L,		// ��� �¸� �÷���.
	EM_ACT_PK_MODE				= 0x00002000L,		// PK ��� �÷���.
	EM_ACT_CREATE				= 0x00004000L,		// Actor ���� ����;
	EM_ACT_GESTURE				= 0x00008000L,		// Actor Gesture ����;
	EM_ACT_MACROMODE			= 0x00010000L,		// ��ũ�� ����;
	EM_ACT_FEIGN_DIE			= 0x00020000L,		// ����ô ����,
	EM_ACT_EMERGENCY			= 0x00040000L,		// ��޻���
	EM_ACT_ALL					= 0xffffffff,		// ��� �÷��׸� ������ �� ����Ѵ�;
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

	EMSKILL_ACTOR_00	= 60,	// ��Ƽ wMain
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

//	Note : ��� Ÿ��.
//
enum EMCONFT_TYPE
{
	EMCONFT_NONE	= 0,	//	����. ( �����ϰ� ���� ������. )
	EMCONFT_ONE		= 1,	//	���� ���.
	EMCONFT_PARTY	= 2,	//	��Ƽ ���.
	EMCONFT_GUILD	= 3	//	��� ���.
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
	DISGUISE_NONE,			// ����
	DISGUISE_BEFORE_SKILL,	// ������ ��ų
	DISGUISE_AFTER_SKILL,	// ������ ��ų	
};

// ����� ���������� ����;
enum EMTYPE_ITEM_REMODEL
{
	EMTYPE_ITEM_REMODEL_NONE = 0,
	EMTYPE_ITEM_REMODEL_RANDOMOPTION = 1,
	EMTYPE_ITEM_REMODEL_BASICSTATS = 2,
	EMTYPE_ITEM_REMODEL_ADDSTATS = 3,
	EMTYPE_ITEM_REMODEL_ITEMSKILL = 4,
};

//	Note : ��� �α�.
//
struct SCONFT_LOG
{
	DWORD	dwCOUNT;		//	��� Ƚ��.
	float	fTIMER;			//	�ֱ� ����� ���.

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
	bool	bSCHOOL;	//	���� ������� �˻�.
	bool	bBOUND;		//	���� ���� ����.
	WORD	wRECOVER;	//	ȸ���� ��� ���� ����.

	float	fHP_RATE;		//	ü�� Ȯ�� ����.
	float	fTAR_HP_RATE;	//	ü�� Ȯ�� ���� [ ����(���δ�ýø�) ].

	//	�б� ��ýÿ���.
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

// ���� �����Ϳ��� �����ϴ� Ư�� ��ɵ�
enum EMETCFUNC
{
	EMETCFUNC_LIMITTIME	  = 0, // ���� �ð�
	EMETCFUNC_CONTROLCAM  = 1, // ī�޶� ����
	EMETCFUNC_LANDEFFECT  = 2, // ���� ȿ��
	EMETCFUNC_DIFFICULTY  = 3, // ���̵� ����
	EMETCFUNC_AROUNDREGEN = 4, // Ư�� ���� �ֺ��� ����
	EMETCFUNC_MOBCHASE	  = 5, // ��� ���� �ش� Ÿ���� ���������� ����
	EMETCFUNC_REGENONCE	  = 6, // ���� Gen�� �ѹ��� �ϵ��� ����
	EMETCFUNC_REENTRYLIMIT= 7, // ������ð� ����

	EMETCFUNC_SIZE,
};

enum EMLANDEFFECT_TYPE
{

	EMLANDEFFECT_ATK_SPEED		= 0,	//	���ݼӵ�
	EMLANDEFFECT_MOVE_SPEED		= 1,	//	�̵��ӵ�

	EMLANDEFFECT_HP_RATE		= 2,	//	HP ������.
	EMLANDEFFECT_MP_RATE		= 3,	//	MP ������

	EMLANDEFFECT_RECOVER_RATE	= 4,	//	ȸ����
	EMLANDEFFECT_DAMAGE_RATE	= 5,	//	����ġ ��ȭ��
	EMLANDEFFECT_DEFENSE_RATE	= 6,	//	���ġ ��ȭ��
	EMLANDEFFECT_RESIST_RATE	= 7,	//	���� ��ġ

	EMLANDEFFECT_CANCEL_ALLBUFF	= 8,	//	��� ���� ���
	EMLANDEFFECT_SAFE_ZONE		= 9,	//	��������

	EMLANDEFFECT_SIZE			= 10,	//	ũ��

	EMLANDEFFECT_MULTI			= 10,	//  �ִ� �������� ����ȿ��
};

enum EMMAP_EFFECT_TYPE
{
	EMMAP_EFFECT_NONE			= 0,	//	����
	EMMAP_EFFECT_ATTACK_SPEED	= 1,	//	���ݼӵ�
	EMMAP_EFFECT_MOVE_SPEED		= 2,	//	�̵��ӵ�

	EMMAP_EFFECT_HP_RATE		= 3,	//	HP������
	EMMAP_EFFECT_MP_RATE		= 4,	//	MP������
	EMMAP_EFFECT_ALL_RATE		= 5,	//	��üȸ����

	EMMAP_EFFECT_DAMAGE_RATE	= 6,	//	���ݷ�
	EMMAP_EFFECT_DEFENSE_RATE	= 7,	//	����
	
	EMMAP_EFFECT_SIZE			= 8,	//	ũ��
	EMMAP_EFFECT_MAX			= 10,	//	�ִ� ���밡���� ����ȿ��
};

struct SGETWHISPERLIST
{
	DWORD dwClientID;
	DWORD dwGaeaID;
};


//	�ʺ��� ����Ǵ� ����ȿ��
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

//! Character �� Point (PointShop)
struct CHARACTER_POINT
{
    __int64 m_PointRefundable; //! ȯ�Ұ��� ����Ʈ
    __int64 m_PointNotRefundable; //! ȯ�ҺҰ� ����Ʈ

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

// ��� ��ȯ���� ���� ������;
// �ʵ� <-> �ʵ� ���� �̵��� ���� GLCharDefine�� �����Ѵ�;
struct SHIRESUMMONINFO
{
	DWORD dwGaeaID;			// ��� ��ȯ���� GaeaID;
	bool bSummoning;		// ��ȯ �����ΰ�?;
	bool bResummoning;		// ���ȯ �����ΰ�?;
	//bool bMoveMap;			// �� �̵� �����ΰ�?;

	float fRemainedTime;	// ���� ��ȯ �ð�;
	DWORD dwFamiliarType;	// AI Type;
	bool bEventSkill;		// �̺�Ʈ ��ų ��� ���� ����;
	DWORD dwMaxSummon;		// �ִ� ��ȯ ��;
	WORD wSkillLevel;		// ��ȯ�� ��ų ����;

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
	bool bSummonable;			// ��ȯ ���� �Ѱ�?;
	DWORD dwSummonLevel;		// ��ȯ ���� ����;
	SHIRESUMMONINFO hireInfo;	// ��� ����;

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

	// ��ȯ ���¸� �����Ѵ�;
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
	OPTION_NEW_CHARACTER					= 0x00000001, // �ű� ĳ����, ���� �� �����ϸ� �ȵ� 
	OPTION_ITEM_RANDOMOPTION_INSERT_DONE	= 0x00000002, // ĳ���� �κ��丮, ����â ������ �����ɼ� �α� ��� �Ϸ�
};

enum EMREBUILDTYPE
{
	EMREBUILDTYPE_NONE = 0,
	EMREBUILDTYPE_TRY_KEEPORIGIN_CACHERESULT	= 1,	// ���� �õ�, ���� ���¸� �����ϰ� ������� �޴´�, ���ÿ� ���� ������� �����Ѵ�;
	EMREBUILDTYPE_TRY_APPLYRESULT_CACHEORIGINAL	= 2,	// ���� �õ�, ������� �ٷ� �����ϰ� ���� ������ �����Ų��;
};

enum EMSKILLRESET
{
	EMGRADE				= 31,
	EMMARBLE_MID		= 104,

	EMGRADE_MIN			= 4,
};
