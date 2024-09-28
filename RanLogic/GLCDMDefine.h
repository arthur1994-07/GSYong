#pragma once

#include "./Character/GLCharData.h"
#include <set>

#define RANKING_NUM		9
#define CLUB_NAME		10

//	킬수 누적 카운트 ( 부활시간 관련 횟수 ) 
#define	CDM_ACCRUE_COUNT	10


struct SCDM_TIME
{
	DWORD dwWeekDay; // 일 월 화 수 목 금 토 1 2 3 4 5 6 7
	DWORD dwStartTime; // 시간 (0-23까지)

	SCDM_TIME () :
		dwWeekDay(0),
		dwStartTime(0)
	{
	}
};

struct SCDM_RANK_EX
{
	WORD	wClubRanking;
	WORD	wKillNum;
	WORD	wDeathNum;

	SCDM_RANK_EX()
		: wClubRanking(0)
		, wKillNum (0)
		, wDeathNum(0)
	{
	}

	void Init()
	{
		wClubRanking = 0;
		wKillNum = 0;
		wDeathNum = 0;
	}
};

struct SCDM_RANK_INFO
{
	int		nIndex;   
	DWORD	dwClubID;
	WORD	wClubRanking;
	WORD	wKillNum;
	WORD	wDeathNum;
	char	szClubName[CLUB_NAME+1];	
	
	SCDM_RANK_INFO()
        : nIndex ( -1 )
		, dwClubID( CLUB_NULL )
		, wClubRanking(0)
		, wKillNum(0)
		, wDeathNum(0)
	{
		memset( szClubName, 0, sizeof(char) * (CLUB_NAME+1) );
	}

	SCDM_RANK_INFO ( const SCDM_RANK_INFO &value )
	{
		operator=(value);
	}

	SCDM_RANK_INFO& operator = ( const SCDM_RANK_INFO& rvalue )
	{
		nIndex = rvalue.nIndex;
		dwClubID = rvalue.dwClubID;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );
		
		wKillNum = rvalue.wKillNum;
		wDeathNum = rvalue.wDeathNum;

		return *this;
	}

	bool operator < ( const SCDM_RANK_INFO& sCdmRank )
	{			
		if (wKillNum > sCdmRank.wKillNum)
            return true;
		else if (wKillNum == sCdmRank.wKillNum && wDeathNum < sCdmRank.wDeathNum)
            return true;		
		return  false;
	}

	bool operator == (const SCDM_RANK_INFO& sCdmRank) 
	{
		if (wKillNum == sCdmRank.wKillNum && wDeathNum == sCdmRank.wDeathNum)
            return true;
        else
		    return false;
	}
};

struct SCDM_CHAR_RANK
{
    DWORD m_ClubDbNum;
    DWORD m_ChaDbNum;
    WORD  m_Kill;
    WORD  m_Death;
    std::string m_ChaName;

    SCDM_CHAR_RANK(DWORD ClubDbNum, DWORD ChaDbNum, WORD Kill, WORD Death, const std::string& ChaName)
        : m_ClubDbNum(ClubDbNum)
        , m_ChaDbNum(ChaDbNum)
        , m_Kill(Kill)
        , m_Death(Death)
        , m_ChaName(ChaName)
    {        
    }
};

inline bool operator < (const SCDM_CHAR_RANK& a, const SCDM_CHAR_RANK& b)
{
    if (a.m_Kill > b.m_Kill)
        return true;
    else if (a.m_Kill == b.m_Kill && a.m_Death < b.m_Death)
        return true;
    else
        return false;
}

struct SCDM_RANK
{
	int nIndex;
	WORD wClubRanking;
	char szClubName[CLUB_NAME+1];

	SCDM_RANK() 
		: nIndex(-1)
		, wClubRanking (0)
	{
		memset( szClubName, 0, sizeof(char) * (CLUB_NAME+1) );
	}

	SCDM_RANK ( const SCDM_RANK &value )
	{
		operator=(value);
	}

	SCDM_RANK& operator = ( const SCDM_RANK& rvalue )
	{
		nIndex = rvalue.nIndex;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );	

		return *this;
	}

	SCDM_RANK ( const SCDM_RANK_INFO &value )
	{
		operator=(value);
	}

	SCDM_RANK& operator = ( const SCDM_RANK_INFO& rvalue )
	{
		nIndex = rvalue.nIndex;
		wClubRanking = rvalue.wClubRanking;
		StringCchCopy( szClubName, CLUB_NAME+1, rvalue.szClubName );

		return *this;
	}
};

typedef std::vector<SCDM_RANK_INFO>		CDM_RANK_INFO_VEC;
typedef CDM_RANK_INFO_VEC::iterator		CDM_RANK_INFO_VEC_ITER;

typedef std::vector<SCDM_RANK>			CDM_RANK_VEC;
typedef CDM_RANK_VEC::iterator			CDM_RANK_VEC_ITER;

typedef std::map<DWORD, SCDM_RANK_INFO>	  CDM_RANK_INFO_MAP;
typedef CDM_RANK_INFO_MAP::iterator		  CDM_RANK_INFO_MAP_ITER;
typedef CDM_RANK_INFO_MAP::const_iterator CDM_RANK_INFO_MAP_CITER;
typedef CDM_RANK_INFO_MAP::value_type     CDM_RANK_INFO_MAP_VALUE;

typedef std::set<DWORD>					CDM_AWARD_CHAR;			//	보상을 받은 캐릭터
typedef CDM_AWARD_CHAR::iterator		CDM_AWARD_CHAR_ITER;

struct SCDM_AWARD_ITEM
{
	SNATIVEID	nAwardItem[RANKING_NUM];		// 1,2,3,기타
	DWORD		dwAwardLimit;

	SCDM_AWARD_ITEM()
		: dwAwardLimit(1)
	{	
		//memset( nAwardItem, -1, sizeof( SNATIVEID ) * 4 );
        nAwardItem[0].dwID = -1;
        nAwardItem[1].dwID = -1;
        nAwardItem[2].dwID = -1;
        nAwardItem[3].dwID = -1;
	}
};


struct SCDM_KILL_LOG
{
	WORD wKill;
	WORD wDeath;

	SCDM_KILL_LOG()
		: wKill ( 0 ) 
		, wDeath ( 0 ) 
	{
	}
};

typedef std::map<DWORD, SCDM_KILL_LOG>   CDM_KILL_LOG_MAP;			// 캐릭터별 킬/데스 로그
typedef CDM_KILL_LOG_MAP::iterator	     CDM_KILL_LOG_MAP_ITER;
typedef CDM_KILL_LOG_MAP::const_iterator CDM_KILL_LOG_MAP_CITER;
typedef CDM_KILL_LOG_MAP::value_type     CDM_KILL_LOG_MAP_VALUE;

struct SCDM_CHAR_LOG
{
	DWORD	dwClubID;
	DWORD	dwClass;
	WORD	wLevel;

	SCDM_CHAR_LOG()
		: dwClubID ( CLUB_NULL ) 
        , dwClass ( 0 ) 
		, wLevel ( 0 ) 
	{		
	}
};

typedef std::map<DWORD,SCDM_CHAR_LOG>		CDM_CHAR_LOG_MAP;			//	참여한 캐릭터
typedef CDM_CHAR_LOG_MAP::iterator			CDM_CHAR_LOG_MAP_ITER;


struct SCDM_ACCRUE_COUNT
{
	DWORD dwKillClub;		//	유저를 죽인 클럽
	DWORD dwKillCount;	//	해당 클럽에 연속으로 죽은횟수

	SCDM_ACCRUE_COUNT()
		: dwKillClub( 0 ) 
		, dwKillCount ( 0 )
	{
	}
};

typedef std::map<DWORD,SCDM_ACCRUE_COUNT>	CDM_ACCRUE_COUNT_MAP;			// 캐릭터별 데스 누적 로그
typedef CDM_ACCRUE_COUNT_MAP::iterator		CDM_ACCRUE_COUNT_MAP_ITER;

