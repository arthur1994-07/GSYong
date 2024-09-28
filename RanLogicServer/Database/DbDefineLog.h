#ifndef _DB_DEFINE_LOG_H_
#define _DB_DEFINE_LOG_H_

#pragma once

#include "../RanLogic/Character/GLCharDefine.h"
#include "../RanLogic/Item/GLITEMLMT.h"
#include "../SigmaCore/String/MinGuid.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmacORE/String/StringFormat.h"

#define MAX_ALLIY 2

#define DEFAULT_LOGDATE_VALUE 10800  // 1970-1-1 12:00

//! 아이템 거래로그
struct LOG_ITEM_EXCHANGE
{
    int			m_nNID_M;			// 아이템 주번호
    int			m_nNID_S;			// 아이템 부번호
    int			m_nSGNum;			// 서버그룹
    int			m_nSvrNum;			// 서버번호
    int			m_nFldNum;			// 필드번호
    int			m_nMakeType;		// 타입
	sc::MGUID	m_guid;		// 생성번호
    gamelog::EMIDTYPE	m_nFromType;		// from의 종류 ( 유저, 케릭터, 클럽 )
    int			m_nItemFrom;		// 아이템 출발지
    gamelog::EMIDTYPE	m_nToType;			// to의 종류 ( 유저, 케릭터, 클럽 )
    int			m_nItemTo;			// 아이템 목적지
    EMITEM_ROUTE m_nExchangeFlag;	// 교환 타입
	EMITEM_ROUTE_DETAIL m_nExchangeFlagDetail;
    int			m_nNum;

    //	코스툼 변환 정보.
    int			m_nCOSTUME_MID;
    int			m_nCOSTUME_SID;

    //	연마 정보.
    short		m_nDamage;
    short		m_nDefense;
    short		m_nFire;
    short		m_nIce;
    short		m_nPoison;
    short		m_nElectric;
    short		m_nSpirit;

	int			m_nBaseMainMapID;
	int			m_nBaseSubMapID;
    int			m_nGaeaMainMapID;
	int			m_nGaeaSubMapID;

	short		m_nOriNIDMain;
	short		m_nOriNIDSub;

    __time64_t m_LogDate; //! 로그시간

    LOG_ITEM_EXCHANGE(
        int _nNID_M,
        int _nNID_S,
        int _nSGNum,
        int _nSvrNum,
        int _nFldNum,
        int _nMakeType,
        sc::MGUID _guid,
        gamelog::EMIDTYPE _nFromType,
        int _nItemFrom,
        gamelog::EMIDTYPE _nToType,
        int _nItemTo,
        EMITEM_ROUTE _nExchangeFlag,
        int _nNum,

        int _nCOSTUME_MID,
        int _nCOSTUME_SID,

        short _nDamage,
        short _nDefense,
        short _nFire,
        short _nIce,
        short _nPoison,
        short _nElectric,
        short _nSpirit,
		int _baseMainMapID,
		int _baseSubMapID,
		int _gaeaMainMapID,
		int _gaeaSubMapID,
		short nOriNIDMain,
		short nOriNIDSub,
		EMITEM_ROUTE_DETAIL nExchangeFlagDetail,
        __time64_t _LogDate ) 
        : m_nNID_M(_nNID_M)
        , m_nNID_S(_nNID_S)
        , m_nSGNum(_nSGNum)
        , m_nSvrNum(_nSvrNum)
        , m_nFldNum(_nFldNum)
        , m_nMakeType(_nMakeType)
        , m_guid(_guid)
        , m_nFromType(_nFromType)
        , m_nItemFrom(_nItemFrom)
        , m_nToType(_nToType)
        , m_nItemTo(_nItemTo)
        , m_nExchangeFlag(_nExchangeFlag)
        , m_nNum(_nNum)
        , m_nCOSTUME_MID(_nCOSTUME_MID)
        , m_nCOSTUME_SID(_nCOSTUME_SID)
        , m_nDamage(_nDamage)
        , m_nDefense(_nDefense)
        , m_nFire(_nFire)
        , m_nIce(_nIce)
        , m_nPoison(_nPoison)
        , m_nElectric(_nElectric)
        , m_nSpirit(_nSpirit)
		, m_nBaseMainMapID(_baseMainMapID)
		, m_nBaseSubMapID(_baseSubMapID)
		, m_nGaeaMainMapID(_gaeaMainMapID)
		, m_nGaeaSubMapID(_gaeaSubMapID)
		, m_nOriNIDMain(nOriNIDMain)
		, m_nOriNIDSub(nOriNIDSub)
		, m_nExchangeFlagDetail( nExchangeFlagDetail )
        , m_LogDate(_LogDate)
    {
    }

    LOG_ITEM_EXCHANGE () 
        : m_nNID_M(0)
        , m_nNID_S(0)
        , m_nSGNum(0)
        , m_nSvrNum(0)
        , m_nFldNum(0)
        , m_nMakeType(0)
        , m_nFromType(gamelog::ID_NONE)
        , m_nItemFrom(0)
        , m_nToType(gamelog::ID_NONE)
        , m_nItemTo(0)
        , m_nExchangeFlag(ITEM_ROUTE_CHAR)
        , m_nNum(0)
        , m_nCOSTUME_MID(0)
        , m_nCOSTUME_SID(0)
        , m_nDamage(0)
        , m_nDefense(0)
        , m_nFire(0)
        , m_nIce(0)
        , m_nPoison(0)
        , m_nElectric(0)
        , m_nSpirit(0)
		, m_nBaseMainMapID(0)
		, m_nBaseSubMapID(0)
		, m_nGaeaMainMapID(0)
		, m_nGaeaSubMapID(0)
		, m_nOriNIDMain(0)
		, m_nOriNIDSub(0)
		, m_nExchangeFlagDetail(ITEM_ROUTE_DETAIL_UNKNOWN)

    {
        m_LogDate = DEFAULT_LOGDATE_VALUE; // 1970-1-1 12:00
    }

	BOOL Verify()
	{
		BOOL bRet = TRUE;

		//아래 값들 중 tinyint에 대해 type의 범위를 넘는지 체크하자.
		//CacheServer에서 tinyint 산술 overflow가 발생하는 문제를 추적하기 위함.

		//m_nNID_M			/*smallint*/
		//m_nNID_S			/*smallint*/

		if (m_nSGNum < 0 || m_nSGNum > 255)			/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nSGNum is out of tinyint range(%1%)",m_nSGNum));
		}

		if (m_nSvrNum < 0 || m_nSvrNum > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nSvrNum is out of tinyint range(%1%)", m_nSvrNum));
		}

		if (m_nFldNum < 0 || m_nFldNum > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nFldNum is out of tinyint range(%1%)", m_nFldNum));
		}

		if (m_nMakeType < 0 || m_nMakeType > 255)	/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nMakeType is out of tinyint range(%1%)", m_nMakeType));
		}

		//m_lnMakeNum		/*money*/

		if (m_nFromType < 0 || m_nFromType > 255)	/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nFromType is out of tinyint range(%1%)", m_nFromType));
		}

		//m_nItemFrom		/*int*/

		if (m_nToType < 0 || m_nToType > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nToType is out of tinyint range(%1%)", m_nToType));
		}

		//m_nItemTo			/*int*/

		if (m_nExchangeFlag < 0 || m_nExchangeFlag > 255)	/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nExchangeFlag is out of tinyint range(%1%)", m_nExchangeFlag));
		}

		if (m_nNum > SHRT_MAX || m_nNum < SHRT_MIN)	/*smallint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nNum is out of smallint range(%1%)", m_nNum));
		}

		//m_nCOSTUME_MID	/*int*/
		//m_nCOSTUME_SID	/*int*/

		if (m_nDamage < 0 || m_nDamage > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nDamage is out of tinyint range(%1%)", m_nDamage));
		}

		if (m_nDefense < 0 || m_nDefense > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nDefense is out of tinyint range(%1%)", m_nDefense));
		}

		if (m_nFire < 0 || m_nFire > 255)			/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nFire is out of tinyint range(%1%)", m_nFire));
		}

		if (m_nIce < 0 || m_nIce > 255)				/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nIce is out of tinyint range(%1%)", m_nIce));
		}

		if (m_nPoison < 0 || m_nPoison > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nPoison is out of tinyint range(%1%)", m_nPoison));
		}

		if (m_nElectric < 0 || m_nElectric > 255)	/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nElectric is out of tinyint range(%1%)", m_nElectric));
		}

		if (m_nSpirit < 0 || m_nSpirit > 255)		/*tinyint*/
		{
			bRet = FALSE;
			sc::writeLogError(sc::string::format("LOG_ITEM_EXCHANGE - m_nSpirit is out of tinyint range(%1%)", m_nSpirit));
		}

		return bRet;
	}
};

//! 게임머니 거래로그
struct LOG_MONEY_EXCHANGE
{
    int m_nSGNum;
    int	m_nSvrNum;
    int	m_nFldNum;
    gamelog::EMIDTYPE m_FromType;
    int	m_nItemFrom;
    gamelog::EMIDTYPE m_ToType;
    int	m_nItemTo;
    LONGLONG m_lnPrice;
    EMITEM_ROUTE m_nExchangeFlag;    
	EMITEM_ROUTE_DETAIL m_nExchangeFlagDetail;
	int			m_nBaseMainMapID;
	int			m_nBaseSubMapID;
    int			m_nGaeaMainMapID;
	int			m_nGaeaSubMapID;

	short		m_nOriNIDMain;
	short		m_nOriNIDSub;
    __time64_t m_LogDate; //! 로그시간

	sc::MGUID m_guid;

    LOG_MONEY_EXCHANGE () 
        : m_nSGNum(0)
        , m_nSvrNum(0)
        , m_nFldNum(0)
        , m_FromType(gamelog::ID_NONE)
        , m_nItemFrom(0)
        , m_ToType(gamelog::ID_NONE)
        , m_nItemTo(0)
        , m_lnPrice(0)
        , m_nExchangeFlag(ITEM_ROUTE_CHAR)
		, m_nBaseMainMapID(0)
		, m_nBaseSubMapID(0)
		, m_nGaeaMainMapID(0)
		, m_nGaeaSubMapID(0)
		, m_nOriNIDMain(0)
		, m_nOriNIDSub(0)
		, m_nExchangeFlagDetail(ITEM_ROUTE_DETAIL_UNKNOWN)
    {
        m_LogDate = DEFAULT_LOGDATE_VALUE; // 1970-1-1 12:00
    }

    LOG_MONEY_EXCHANGE (
        int _nSGNum,
        int _nSvrNum,
        int _nFldNum,

        gamelog::EMIDTYPE _nFromType,
        int _nItemFrom,

        gamelog::EMIDTYPE _nToType,
        int _nItemTo,

        LONGLONG _lnPrice,
        EMITEM_ROUTE _nExchangeFlag,
		
		int _baseMainMapID,
		int _baseSubMapID,
		int _gaeaMainMapID,
		int _gaeaSubMapID,

		short nOriNIDMain,
		short nOriNIDSub,

		EMITEM_ROUTE_DETAIL nExchangeFlagDetail,

        __time64_t _LogDate, 

		sc::MGUID& guid
        ) 
        : m_nSGNum(_nSGNum)
        , m_nSvrNum(_nSvrNum)
        , m_nFldNum(_nFldNum)
        , m_FromType(_nFromType)
        , m_nItemFrom(_nItemFrom)
        , m_ToType(_nToType)
        , m_nItemTo(_nItemTo)
        , m_lnPrice(_lnPrice)
        , m_nExchangeFlag(_nExchangeFlag)
		, m_nBaseMainMapID(_baseMainMapID)
		, m_nBaseSubMapID(_baseSubMapID)
		, m_nGaeaMainMapID(_gaeaMainMapID)
		, m_nGaeaSubMapID(_gaeaSubMapID)
		, m_nOriNIDMain(nOriNIDMain)
		, m_nOriNIDSub(nOriNIDSub)
		, m_nExchangeFlagDetail(nExchangeFlagDetail)
        , m_LogDate(_LogDate)
		, m_guid( guid )
    {
    }
};

//! 아이템 인첸트로그
struct LOG_ITEM_CONVERSION
{
    int			m_nNIDMain;
    int			m_nNIDSub;
    int			m_nSGNum;
    int			m_nSvrNum;
    int			m_nFldNum;
    int			m_nMakeType;
	int			m_nExchangeFlagDetail;
    sc::MGUID	m_guid;

    gamelog::EMIDTYPE    m_nFromType;
    int			m_nItemFrom;

    int			m_nCOSTUME_MID;
    int			m_nCOSTUME_SID;
	
    short		m_nDamage;
    short		m_nDefense;
    short		m_nFire;
    short		m_nIce;
    short		m_nPoison;
    short		m_nElectric;
    short		m_nSpirit;

	int			m_nBaseMainMapID;
	int			m_nBaseSubMapID;
	int			m_nGaeaMainMapID;
	int			m_nGaeaSubMapID;

	short		m_nOriNIDMain;
	short		m_nOriNIDSub;

    __time64_t m_LogDate;

	int			m_nExchangeFlag;
    
    LOG_ITEM_CONVERSION () 
        : m_nNIDMain(0)
        , m_nNIDSub(0)
        , m_nSGNum(0)
        , m_nSvrNum(0)
        , m_nFldNum(0)
        , m_nMakeType(0)
        , m_nFromType(gamelog::ID_NONE)
        , m_nItemFrom(0)
        , m_nCOSTUME_MID(0)
        , m_nCOSTUME_SID(0)
        , m_nDamage(0)
        , m_nDefense(0)
        , m_nFire(0)
        , m_nIce(0)
        , m_nPoison(0)
        , m_nElectric(0)
        , m_nSpirit(0)
		, m_nBaseMainMapID(0)
		, m_nBaseSubMapID(0)
		, m_nGaeaMainMapID(0)
		, m_nGaeaSubMapID(0)
		, m_nOriNIDMain(0)
		, m_nOriNIDSub(0)
		, m_nExchangeFlagDetail(ITEM_ROUTE_DETAIL_UNKNOWN)
		, m_nExchangeFlag(0)
    {
        m_LogDate = DEFAULT_LOGDATE_VALUE; // 1970-1-1 12:00
    }

    LOG_ITEM_CONVERSION (
        int _nNIDMain,
        int _nNIDSub,
        int _nSGNum,
        int _nSvrNum,
        int _nFldNum,
        int _nMakeType,
        sc::MGUID _guid,

        gamelog::EMIDTYPE _nFromType,
        int _nItemFrom,

        int _nCOSTUME_MID,
        int _nCOSTUME_SID,

        short _nDamage,
        short _nDefense,
        short _nFire,
        short _nIce,
        short _nPoison,
        short _nElectric,
        short _nSpirit,

		int nBaseMainMapID,
		int nBaseSubMapID,
		int nGaeaMainMapID,
		int nGaeaSubMapID,

		short nOriNIDMain,
		short nOriNIDSub,

		int nExchangeFlagDetail,
        
        __time64_t _LogDate,
		int	nExchangeFlag) 
        : m_nNIDMain(_nNIDMain)
        , m_nNIDSub(_nNIDSub)
        , m_nSGNum(_nSGNum)
        , m_nSvrNum(_nSvrNum)
        , m_nFldNum(_nFldNum)
        , m_nMakeType(_nMakeType)
        , m_guid(_guid)
        , m_nFromType(_nFromType)
        , m_nItemFrom(_nItemFrom)
        , m_nCOSTUME_MID(_nCOSTUME_MID)
        , m_nCOSTUME_SID(_nCOSTUME_SID)
        , m_nDamage(_nDamage)
        , m_nDefense(_nDefense)
        , m_nFire(_nFire)
        , m_nIce(_nIce)
        , m_nPoison(_nPoison)
        , m_nElectric(_nElectric)
        , m_nSpirit(_nSpirit)
		, m_nBaseMainMapID(nBaseMainMapID)
		, m_nBaseSubMapID(nBaseSubMapID)
		, m_nGaeaMainMapID(nGaeaMainMapID)
		, m_nGaeaSubMapID(nGaeaSubMapID)
		, m_nOriNIDMain(nOriNIDMain)
		, m_nOriNIDSub(nOriNIDSub)
		, m_nExchangeFlagDetail(nExchangeFlagDetail)
        , m_LogDate(_LogDate)
		, m_nExchangeFlag(nExchangeFlag)
    {
    }
};

//! 캐릭터 행동 로그
struct LOG_ACTION
{
    int m_nCHARID;
    gamelog::EMACTION m_ActionType;
    gamelog::EMIDTYPE m_TargetType;
    int m_TargetId;

    __int64 m_nEXP;
    int m_nBRIGHT_POINT;
    int m_nLIFE_POINT;
    int m_nMONEY;

	int			m_nBaseMainMapID;
	int			m_nBaseSubMapID;
    int			m_nGaeaMainMapID;
	int			m_nGaeaSubMapID;

    __time64_t m_LogDate; //! 로그시간

    LOG_ACTION(
        int nCHARID,
        gamelog::EMACTION nTYPE,
        gamelog::EMIDTYPE nTARTYPE,
        int nTARID,
        __int64 nEXP,
        int BRIGHT_POINT,
        int nLIFE_POINT,
        int nMONEY,
		int _baseMainMapID,
		int _baseSubMapID,
		int _gaeaMainMapID,
		int _gaeaSubMapID,
        __time64_t LogDate)
        : m_nCHARID(nCHARID)
        , m_ActionType(nTYPE)
        , m_TargetType(nTARTYPE)
        , m_TargetId(nTARID)
        , m_nEXP(nEXP)
        , m_nBRIGHT_POINT(BRIGHT_POINT)
        , m_nLIFE_POINT(nLIFE_POINT)
        , m_nMONEY(nMONEY)
		, m_nBaseMainMapID(_baseMainMapID)
		, m_nBaseSubMapID(_baseSubMapID)
		, m_nGaeaMainMapID(_gaeaMainMapID)
		, m_nGaeaSubMapID(_gaeaSubMapID)
        , m_LogDate(LogDate)
    {
    }
};

//! 아이템의 랜덤옵션 로그를 남기기 위한 구조체.
struct LOG_RANDOM_OPTION
{
    int m_Mid; // 아이템고유번호 MID
    int m_Sid;  // 아이템고유번호 SID
    int m_ServerGroup;   // 서버그룹
    int m_ServerNumber;  
    int m_FieldNumber;  // 필드번호
    int m_MakeType; // 발생타입
    sc::MGUID m_guid; // 아이템고유번호

    //	랜덤 옵션 종류
    BYTE m_OptionType1;
    BYTE m_OptionType2;
    BYTE m_OptionType3;
    BYTE m_OptionType4;
	BYTE m_OptionType5;

    // 랜덤 옵션 값
    short m_OptionValue1;
    short m_OptionValue2;
    short m_OptionValue3;
    short m_OptionValue4;
	short m_OptionValue5;

    // 랜덤 옵션 스탬프 횟수
    BYTE m_OptionStamp1;
    BYTE m_OptionStamp2;
    BYTE m_OptionStamp3;
    BYTE m_OptionStamp4;
	BYTE m_OptionStamp5;

    __time64_t m_LogDate; //! 로그시간

	int m_InsertOption;

    LOG_RANDOM_OPTION(
        int Mid,
		int Sid,
        int ServerGroup,
		int ServerNumber,
		int FieldNumber,
        int MakeType,
		sc::MGUID guid,    
        BYTE OptionType1,
		BYTE OptionType2,
		BYTE OptionType3,
		BYTE OptionType4,
		BYTE OptionType5,
        short OptionValue1,
		short OptionValue2,
		short OptionValue3,
		short OptionValue4,
		short OptionValue5,
        BYTE OptionStamp1,
		BYTE OptionStamp2,
		BYTE OptionStamp3,
		BYTE OptionStamp4, 
		BYTE OptionStamp5, 
        __time64_t _LogDate,
		int InsertOption)
        : m_Mid(Mid), m_Sid(Sid)
        , m_ServerGroup(ServerGroup), m_ServerNumber(ServerNumber), m_FieldNumber(FieldNumber)
        , m_MakeType(MakeType), m_guid(guid)
        , m_OptionType1(OptionType1), m_OptionType2(OptionType2), m_OptionType3(OptionType3), m_OptionType4(OptionType4), m_OptionType5(OptionType5)
        , m_OptionValue1(OptionValue1), m_OptionValue2(OptionValue2), m_OptionValue3(OptionValue3), m_OptionValue4(OptionValue4), m_OptionValue5(OptionValue5)
        , m_OptionStamp1(OptionStamp1), m_OptionStamp2(OptionStamp2), m_OptionStamp3(OptionStamp3), m_OptionStamp4(OptionStamp4), m_OptionStamp5(OptionStamp5)
        , m_LogDate(_LogDate)
		, m_InsertOption( InsertOption )
    {
    }
};

//! 아이템의 가산옵션 로그를 남기기 위한 구조체.
struct LOG_ADDON_OPTION
{
	int m_Mid;				// 아이템고유번호 MID
	int m_Sid;				// 아이템고유번호 SID
	int m_ServerGroup;		// 서버그룹
	int m_ServerNumber;
	int m_FieldNumber;		// 필드번호
	int m_MakeType;			// 발생타입
	sc::MGUID m_guid;		// 아이템고유번호

	// 가산 옵션 종류
	int m_OptionType1;
	int m_OptionType2;
	int m_OptionType3;
	int m_OptionType4;
	int m_OptionType5;
	int m_OptionType6;

	// 가산 옵션 값
	int m_OptionValue1;
	int m_OptionValue2;
	int m_OptionValue3;
	int m_OptionValue4;
	int m_OptionValue5;
	int m_OptionValue6;

	// 가산 옵션 값
	float m_OptionValueF1;
	float m_OptionValueF2;
	float m_OptionValueF3;
	float m_OptionValueF4;
	float m_OptionValueF5;
	float m_OptionValueF6;

	__time64_t m_LogDate; //! 로그시간

	LOG_ADDON_OPTION(
		int Mid,
		int Sid,
		int ServerGroup,
		int ServerNumber,
		int FieldNumber,
		int MakeType,
		sc::MGUID guid,    
		int OptionType1,
		int OptionType2,
		int OptionType3,
		int OptionType4,
		int OptionType5,
		int OptionType6,
		int OptionValue1,
		int OptionValue2,
		int OptionValue3,
		int OptionValue4,
		int OptionValue5,
		int OptionValue6,
		float OptionValueF1,
		float OptionValueF2,
		float OptionValueF3,
		float OptionValueF4,
		float OptionValueF5,
		float OptionValueF6,
		__time64_t _LogDate )
		: m_Mid( Mid ), m_Sid( Sid )
		, m_ServerGroup( ServerGroup ), m_ServerNumber( ServerNumber ), m_FieldNumber( FieldNumber )
		, m_MakeType( MakeType ), m_guid( guid )
		, m_OptionType1( OptionType1 ), m_OptionType2( OptionType2 ), m_OptionType3( OptionType3 ), m_OptionType4( OptionType4 ), m_OptionType5( OptionType5 ), m_OptionType6( OptionType6 )
		, m_OptionValue1( OptionValue1 ), m_OptionValue2( OptionValue2 ), m_OptionValue3( OptionValue3 ), m_OptionValue4( OptionValue4 ), m_OptionValue5( OptionValue5 ), m_OptionValue6( OptionValue6 )
		, m_OptionValueF1( OptionValueF1 ), m_OptionValueF2( OptionValueF2 ), m_OptionValueF3( OptionValueF3 ), m_OptionValueF4( OptionValueF4 ), m_OptionValueF5( OptionValueF5 ), m_OptionValueF6( OptionValueF6 )
		, m_LogDate( _LogDate )
	{
	}
};

//! 아이템의 링크스킬 로그를 남기기 위한 구조체.
struct LOG_LINKSKILL_OPTION
{
	int m_Mid;				// 아이템고유번호 MID
	int m_Sid;				// 아이템고유번호 SID
	int m_ServerGroup;		// 서버그룹
	int m_ServerNumber;
	int m_FieldNumber;		// 필드번호
	int m_MakeType;			// 발생타입
	sc::MGUID m_guid;		// 아이템고유번호

	int	m_LinkSkillTarget;					// 링크 스킬 타겟;
	int m_LinkSkillMid;						// 링크 스킬 ID;
	int m_LinkSkillSid;						// 링크 스킬 ID;
	int m_LinkSkillLevel;					// 링크 스킬 Level;
	float m_LinkSkillOccurRate;				// 링크 스킬 발생 확률;

	__time64_t m_LogDate; //! 로그시간

	LOG_LINKSKILL_OPTION(
		int Mid,
		int Sid,
		int ServerGroup,
		int ServerNumber,
		int FieldNumber,
		int MakeType,
		sc::MGUID guid,    
		int LinkSkillTarget,
		int LinkSkillMid,
		int LinkSkillSid,
		int LinkSkillLevel,
		float LinkSkillOccurRate,
		__time64_t _LogDate )
		: m_Mid( Mid ), m_Sid( Sid )
		, m_ServerGroup( ServerGroup ), m_ServerNumber( ServerNumber ), m_FieldNumber( FieldNumber )
		, m_MakeType( MakeType ), m_guid( guid )
		, m_LinkSkillTarget( LinkSkillTarget ), m_LinkSkillMid( LinkSkillMid ), m_LinkSkillSid( LinkSkillSid ), m_LinkSkillLevel( LinkSkillLevel ), m_LinkSkillOccurRate( LinkSkillOccurRate )
		, m_LogDate( _LogDate )
	{
	}
};

//! 아이템의 기본스텟 로그를 남기기 위한 구조체.
struct LOG_BASICSTAT_OPTION
{
	int m_Mid;				// 아이템고유번호 MID
	int m_Sid;				// 아이템고유번호 SID
	int m_ServerGroup;		// 서버그룹
	int m_ServerNumber;
	int m_FieldNumber;		// 필드번호
	int m_MakeType;			// 발생타입
	sc::MGUID m_guid;		// 아이템고유번호

	WORD m_BasicAttackDamageMin;
	WORD m_BasicAttackDamageMax;
	int m_BasicDefence;

	__time64_t m_LogDate; //! 로그시간

	LOG_BASICSTAT_OPTION(
		int Mid,
		int Sid,
		int ServerGroup,
		int ServerNumber,
		int FieldNumber,
		int MakeType,
		sc::MGUID guid,    
		WORD BasicAttackDamageMin,
		WORD BasicAttackDamageMax,
		int BasicDefence,
		__time64_t _LogDate )
		: m_Mid( Mid ), m_Sid( Sid )
		, m_ServerGroup( ServerGroup ), m_ServerNumber( ServerNumber ), m_FieldNumber( FieldNumber )
		, m_MakeType( MakeType ), m_guid( guid )
		, m_BasicAttackDamageMin( BasicAttackDamageMin ), m_BasicAttackDamageMax( BasicAttackDamageMax ), m_BasicDefence( BasicDefence )
		, m_LogDate( _LogDate )
	{
	}
};

//! 펫 액션 로그를 남기기 위한 구조체.
struct LOG_PET_ACTION 
{
    int m_PetNum;		// 펫 넘버
    int m_ItemMID;		// 아이템 MID
    int m_ItemSID;		// 아이템 SID
    pet::EM_ACTION m_ActionType;	// 펫 액션 타입
    int m_PetFull;		// 펫 포만도
    __time64_t m_LogDate; //! 로그시간

    LOG_PET_ACTION(
        int PetNum,
        int ItemMID,
        int ItemSID,
        pet::EM_ACTION ActionType,
        int PetFull,
        __time64_t _LogDate)
        : m_PetNum(PetNum)
        , m_ItemMID(ItemMID)
        , m_ItemSID(ItemSID)
        , m_ActionType(ActionType)
        , m_PetFull(PetFull)
        , m_LogDate(_LogDate)
    {
    }
};

//! 탈것 로그
struct LOG_VEHICLE_ACTION 
{
    int m_VehicleNum;	// 넘버
    int m_ItemMID;		// 아이템 MID
    int m_ItemSID;		// 아이템 SID
    EMVEHICLE_ACTION m_ActionType;	// 액션 타입
    int m_VehicleFull;	// 탈것 연료
    __time64_t m_LogDate; //! 로그시간

    LOG_VEHICLE_ACTION(
        int VehicleNum,
        int ItemMID,
        int ItemSID,
        EMVEHICLE_ACTION ActionType,
        int VehicleFull,
        __time64_t _LogDate)
        : m_VehicleNum(VehicleNum)
        , m_ItemMID(ItemMID)
        , m_ItemSID(ItemSID)
        , m_ActionType(ActionType)
        , m_VehicleFull(VehicleFull)
        , m_LogDate(_LogDate)
    {
    }
};

//! 선도클럽 로그
struct LOG_GUIDANCE
{
    WORD	wMapMID;
    WORD	wMapSID;
    DWORD	dwClubID;
    DWORD	dwAllianceID;
    DWORD	dwAlliance[MAX_ALLIY];
    DWORD	dwCharID;
    DWORD	dwClass;
    WORD	wLevel;
    WORD	wKill;
    WORD	wDeath;
    int		nCertify;
    CString strTime;

    LOG_GUIDANCE()
        : wMapMID ( 0 ) 
        , wMapSID ( 0 ) 
        , dwClubID ( CLUB_NULL ) 
        , dwAllianceID ( CLUB_NULL )	
        , dwCharID( 0 ) 
        , dwClass( 0 ) 
        , wLevel( 0 ) 
        , wKill( 0 ) 
        , wDeath( 0 ) 
        , nCertify( 0 ) 
    {
        memset( dwAlliance, 0, sizeof( DWORD) * MAX_ALLIY );
    }

    LOG_GUIDANCE& operator= ( const LOG_GUIDANCE& value )
    {
        wMapMID = value.wMapMID;
        wMapSID = value.wMapSID;
        dwClubID = value.dwClubID;
        dwAllianceID = value.dwAllianceID;
        memcpy( dwAlliance, value.dwAlliance, sizeof( DWORD ) * MAX_ALLIY ) ;
        dwCharID = value.dwCharID;
        dwClass = value.dwClass;
        wLevel = value.wLevel;
        wKill = value.wKill;
        wDeath = value.wDeath;
        nCertify = value.nCertify;
        strTime = value.strTime;
        return *this;
    }
};

//! Club Death Match, 클럽데스매치
struct LOG_CDM
{
    WORD	wMapMID;
    WORD	wMapSID;
    DWORD	dwClubID;
    DWORD	dwCharID;
    DWORD	dwClass;
    WORD	wLevel;
    WORD	wKill;
    WORD	wDeath;
    WORD	wClubKill;
    WORD	wClubDeath;
    CString strTime;

    LOG_CDM()
        : wMapMID ( 0 ) 
        , wMapSID ( 0 ) 
        , dwClubID ( CLUB_NULL ) 
        , dwCharID( 0 ) 
        , dwClass( 0 ) 
        , wLevel( 0 ) 
        , wKill( 0 ) 
        , wDeath( 0 ) 
        , wClubKill( 0 ) 
        , wClubDeath( 0 ) 
    {
    }
};

//! Club Death Match Result, 클럽 데스매치 결과
struct LOG_CDM_RESULT
{
    WORD	wMapMID;
    WORD	wMapSID;
    WORD	wClubRank;
    DWORD	dwClubID;
    CString strClubName;
    DWORD	dwCharID;
    DWORD	dwClass;
    WORD	wLevel;
    WORD	wKill;
    WORD	wDeath;
    WORD	wClubKill;
    WORD	wClubDeath;
    CString strTime;

    LOG_CDM_RESULT()
        : wMapMID ( 0 ) 
        , wMapSID ( 0 ) 
        , wClubRank ( 0 ) 
        , dwClubID ( CLUB_NULL ) 
        , dwCharID( 0 ) 
        , dwClass( 0 ) 
        , wLevel( 0 ) 
        , wKill( 0 ) 
        , wDeath( 0 ) 
        , wClubKill( 0 ) 
        , wClubDeath( 0 ) 
    {
    }
};

struct LOG_COSTUME_STATS
{
	int m_iCharDbNum;
	int m_Mid;              // 아이템고유번호 MID
	int m_Sid;              // 아이템고유번호 SID
	int m_ServerGroup;      // 서버그룹
	int m_ServerNumber;  
	int m_FieldNumber;      // 필드번호
	int m_MakeType;         // 발생타입
	sc::MGUID m_guid;  // 아이템고유번호
	__time64_t m_tFireData; // 만료시간

	//	능력치 타입
	BYTE m_Type1;
	BYTE m_Type2;
	BYTE m_Type3;
	BYTE m_Type4;
	BYTE m_Type5;
	BYTE m_Type6;
	BYTE m_Type7;

	// 능력치에 대한 값
	float m_point1;
	float m_point2;
	float m_point3;
	float m_point4;
	float m_point5;
	float m_point6;
	float m_point7;

	__time64_t m_LogDate; //! 로그시간

	LOG_COSTUME_STATS(
		int dbNum,
		int Mid,
		int Sid,
		int ServerGroup,
		int ServerNumber,
		int FieldNumber,
		int MakeType,
		sc::MGUID guid,  
		__time64_t tFireData,
		BYTE Type1,
		BYTE Type2,
		BYTE Type3,
		BYTE Type4,
		BYTE Type5,
		BYTE Type6,
		BYTE Type7,

		float point1,
		float point2,
		float point3,
		float point4,
		float point5,
		float point6,
		float point7,
		__time64_t _LogDate )
		: m_iCharDbNum(dbNum), m_Mid(Mid), m_Sid(Sid)
		, m_ServerGroup(ServerGroup), m_ServerNumber(ServerNumber), m_FieldNumber(FieldNumber), m_MakeType(MakeType)
		, m_guid(guid), m_tFireData(tFireData)
		, m_Type1(Type1), m_Type2(Type2), m_Type3(Type3), m_Type4(Type4), m_Type5(Type5), m_Type6(Type6), m_Type7(Type7)
		, m_point1(point1), m_point2(point2), m_point3(point3), m_point4(point4), m_point5(point5), m_point6(point6), m_point7(point7)
		, m_LogDate(_LogDate)
	{
	}
};

#endif // _DB_DEFINE_LOG_H_
