#ifndef _GL_CLUB_DEFINE_H_
#define _GL_CLUB_DEFINE_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <set>
#include <list>
#include "../../=MsgPack/include/msgpack.hpp"
#include "../Character/GLCharDefine.h"

namespace wb
{
    enum EM_COUNTRY;
}

namespace club
{
	enum EMCLUB_FLAG
	{
		CLUBNAME_LIMIT = 10,
		// 중문은 상대적으로 메모리사이즈가 크므로 늘려줌;
		CLUBNAME_LIMIT_TAIWAN = 20,
	};
    //! 클럽 공개타입
    enum EM_PUBLIC_TYPE
    {
        PUBLIC_FOR_NEWBIE            = 0x0001, //! 신입생들에게 공개
        PUBLIC_FOR_OTHER_CLUB_MEMBER = 0x0002, //! 다른 클럽원들에게 공개
        PUBLIC_ALL                   = PUBLIC_FOR_NEWBIE | PUBLIC_FOR_OTHER_CLUB_MEMBER,
    };

    //! 자동추방
    struct AUTO_KICK_OUT
    {
        size_t m_GradeIndex;  //! 추방할 계급
        WORD   m_Day;         //! 얼마동안 접속하지 않으면 kick 할 것인가
        bool   m_Use; //! 자동 추방을 사용/사용하지 않음

        MSGPACK_DEFINE(m_GradeIndex, m_Day, m_Use);

        AUTO_KICK_OUT()
            : m_GradeIndex(0)
            , m_Day(30)
            , m_Use(false)
        {
        }

        AUTO_KICK_OUT(bool AutoKickOut, size_t GradeIndex, WORD Day)
            : m_GradeIndex(GradeIndex)
            , m_Day(Day)
            , m_Use(AutoKickOut)
        {
        }

        void Reset()
        {
            m_Use = false;
            m_GradeIndex = 0;
            m_Day = 30;            
        }
    };

} // namespace club

//! 구조를 변경하면 VERSION 을 올리십시오. 컴파일 오류가 나는 곳을 고쳐야 합니다.
//! MSGPACK_DEFINE 에 Network 을 통해 전송될 Member 변수를 적어주셔야 전송이 됩니다.
struct GLCLUBMEMBER
{
public:
    //! 구조를 변경하면 VERSION 을 올리십시오. 컴파일 오류가 나는 곳을 고쳐야 합니다.    
    enum { VERSION = 0x0004, };

public:
    GLCLUBMEMBER();
    
    GLCLUBMEMBER(
        DWORD ChaDbNum,
        const char* szChaName,
        DWORD Flags,
        WORD ChaLevel,
        const __time64_t& LastPlayTime,
        bool bOnline,
        DWORD ChaClass);

    GLCLUBMEMBER(
        DWORD _dwID,
        WORD ChaLevel,
        int	  _nCHANNEL,
        const MapID& _nidMAP,
        DWORD dwFlags,
        const D3DXVECTOR3& _vecPOS,
        const char* szName,
        bool _bOnline,
        const __time64_t& LastPlayTime,
        DWORD ChaClass);

    GLCLUBMEMBER(const GLCLUBMEMBER& value);

    GLCLUBMEMBER& operator = (const GLCLUBMEMBER& value);

public:
    DWORD		m_ChaDbNum;            //! 캐릭터 DB 번호
    WORD        m_ChaLevel;            //! 캐릭터 레벨
    int				m_Channel;             //! 현재 위치한 채널
    MapID		m_CurrentMap;          //! 맵번호
    DWORD		m_Grade;               //! 길드 지위, AUTHORITY_GRADE 참고
    D3DXVECTOR3	m_CurrentPos;          //! 위치번호
    std::string	m_ChaName;             //! 캐릭터명
    bool		m_Online;              //! 온라인상태
    __time64_t  m_LastPlayTime;        //! 마지막 플레이 시간 게임진입/게임종료
    DWORD       m_ChaClass;            //! 캐릭터 직업 EMCHARCLASS

    //! MSGPACK_DEFINE 에 Network 을 통해 전송될 Member 변수를 적어주셔야 전송이 됩니다.
    MSGPACK_DEFINE(
        m_ChaDbNum,
        m_ChaLevel,
        m_Channel,
        m_CurrentMap,
        m_Grade,
        m_CurrentPos.x, 
        m_CurrentPos.y, 
        m_CurrentPos.z, 
        m_ChaName, 
        m_Online, 
        m_LastPlayTime,
        m_ChaClass);

public:
    //! 캐릭터명
    void SetName(const char* szName)
    {
        if (szName)
            m_ChaName = szName;
    }

    void SetName(const std::string& strName)
    {
        SetName(strName.c_str());
    }    

    const char* GetName() const { return m_ChaName.c_str(); }
    const std::string& GetNameStr() const { return m_ChaName; }
    std::string GetNameStr() { return m_ChaName; }

    //! 캐릭터 DB 번호
    DWORD GetChaDbNum() const { return m_ChaDbNum; }

    //! 길드 지위, AUTHORITY_GRADE 참고
    DWORD GetGrade() const { return m_Grade; }
    void SetGrade(DWORD Grade) { m_Grade = Grade; }

    //! 마지막 플레이 시간
    void SetLastPlayTime(const __time64_t& LastPlayTime) { m_LastPlayTime = LastPlayTime; }
    __time64_t GetLastPlayTime() const { return m_LastPlayTime; }

    //! 캐릭터 직업 EMCHARCLASS
    EMCHARCLASS GetChaClass() const { return static_cast<EMCHARCLASS> (m_ChaClass); }
};

struct SCLUBMEMBER
{
    std::vector<GLCLUBMEMBER> vecClubMember;

    MSGPACK_DEFINE(vecClubMember);
};

struct GLCLUBALLIANCE
{
    DWORD m_dwID;
    DWORD m_MarkVersion;
    char  m_szName[CHAR_SZNAME]; //! 클럽이름
    char  m_szMasterName[CHAR_SZNAME]; //! 클럽 Master 이름

    GLCLUBALLIANCE(DWORD MarkVersion) 
        : m_dwID(CLUB_NULL)
        , m_MarkVersion(MarkVersion)
    {
        memset(m_szName, 0, sizeof(char) * CHAR_SZNAME);
        memset(m_szMasterName, 0, sizeof(char) * CHAR_SZNAME);
    }

    GLCLUBALLIANCE(DWORD dwID, const char* szName, const char* szMasterName, DWORD MarkVersion)
    {
        m_dwID = dwID;
        m_MarkVersion = MarkVersion;
        if (szName)
            StringCchCopy(m_szName, CHAR_SZNAME, szName);
        if (szMasterName)
            StringCchCopy(m_szMasterName, CHAR_SZNAME, szMasterName);

    }

    DWORD GetClubDbNum() const { return m_dwID; }
    const char* GetClubName() const { return m_szName; }
    const char* GetClubMasterName() const { return m_szMasterName; }
    DWORD GetMarkVersion() const { return m_MarkVersion; }
};

inline bool operator < (const GLCLUBALLIANCE& lvalue, const GLCLUBALLIANCE& rvalue)
{
    return lvalue.m_dwID < rvalue.m_dwID;
}

typedef std::map<DWORD, GLCLUBMEMBER> CLUBMEMBERS;
typedef CLUBMEMBERS::iterator		  CLUBMEMBERS_ITER;
typedef CLUBMEMBERS::const_iterator	  CLUBMEMBERS_CITER;
typedef CLUBMEMBERS::value_type       CLUBMEMBERS_VALUE;

typedef std::set<DWORD>           CLUBCONFT;
typedef CLUBCONFT::iterator       CLUBCONFT_ITER;
typedef CLUBCONFT::const_iterator CLUBCONFT_CITER;

typedef std::list<DWORD>             CLUB_CDM_MEM;
typedef CLUB_CDM_MEM::iterator       CLUB_CDM_MEM_ITER;
typedef CLUB_CDM_MEM::const_iterator CLUB_CDM_MEM_CITER;

typedef std::map<DWORD, GLCLUBALLIANCE> CLUB_ALLIANCE;
typedef CLUB_ALLIANCE::iterator	        CLUB_ALLIANCE_ITER;
typedef CLUB_ALLIANCE::const_iterator   CLUB_ALLIANCE_CITER;
typedef CLUB_ALLIANCE::value_type       CLUB_ALLIANCE_VALUE;

/*
struct GLCLUBINFO
{
    //! 구조를 변경시키면 VERSION 을 올리십시오.
    //! VERSION 을 올리시면 compile 에러가 나는곳이 수정해야 할 부분입니다.
    enum { VERSION=0x0005, };

    DWORD		m_DbNum; //! DB 고유번호
    wb::EM_COUNTRY m_Country; //! 국가정보 2012-02-13 jgkim
    TCHAR		m_szName[CHAR_SZNAME]; //! 클럽이름

    DWORD		m_MasterCharDbNum; //! 클럽 마스터 Char DB Num
    TCHAR		m_szMasterName[CHAR_SZNAME]; //! 클럽 마스터 이름
    int         m_MasterLevel; //! 클럽 마스터 레벨 2012-02-13 jgkim
    DWORD		m_dwCDCertifior; //! 선도클럽 CD 인증 대리자 Char DB Num

    DWORD		m_dwRank; //! 클럽 랭크

    DWORD		m_dwMarkVER; //! 클럽 마크버전
    DWORD		m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY]; //! 클럽마크

    __time64_t	m_tOrganize;		//! 클럽 창설 시간
    __time64_t	m_tDissolution;		//! 클럽 해체 예정 시간.
    __time64_t	m_tAuthority;		//! 클럽마스터 위임 권한

    __time64_t	m_tAllianceSec;		//! 동맹 탈퇴 혹은 제명 시간 + ( 가입 제한시간 )
    __time64_t	m_tAllianceDis;		//! 동맹 해산 시간 + ( 창설 제한 시간 )

    LONGLONG	m_lnIncomeMoney;
    LONGLONG	m_lnStorageMoney;

    DWORD		m_dwBattleWin;		//	배틀 포인트 ( 승 )
    DWORD		m_dwBattleLose;		//	배틀 포인트 ( 패 )
    DWORD		m_dwBattleDraw;		//	배틀 포인트 ( 무)
    __time64_t	m_tLastBattle;		//	마지막 배틀 종료시간

    DWORD		m_dwAllianceBattleWin;		//	동맹배틀 포인트 ( 승 )
    DWORD		m_dwAllianceBattleLose;		//	동맹배틀 포인트 ( 패 )
    DWORD		m_dwAllianceBattleDraw;		//	동맹배틀 포인트 ( 무)

    TCHAR		m_szNotice[EMCLUB_NOTICE_LEN+1];
    TCHAR		m_szNoticeChaName[CHAR_SZNAME]; //! 공지작성 캐릭터 이름
    __time64_t	m_NoticeDate;		//! 공지작성 날짜/시간

    DWORD m_PublicType; //! 클럽 공개 타입 EM_PUBLIC_TYPE 참고

public:
    GLCLUBINFO();
    GLCLUBINFO(const GLCLUBINFO& value)
    {
        operator= (value);
    }

    GLCLUBINFO& operator = (const GLCLUBINFO& rhs);

    inline void SetMasterName(const std::string& MasterName)
    {
        StringCchCopy(m_szMasterName, CHAR_SZNAME, MasterName.c_str());
    }

    //! 클럽 마스터 레벨 2012-02-13 jgkim
    inline void SetMasterLevel(int Level)
    {
        m_MasterLevel = Level;
    }

    //! 클럽 공지
    inline void SetNotice(const std::string& Notice)
    {
        StringCchCopy(m_szNotice, EMCLUB_NOTICE_LEN+1, Notice.c_str());
    }

    inline void SetNoticeChaName(const std::string& NoticeChaName)
    {
        StringCchCopy(m_szNoticeChaName, CHAR_SZNAME, NoticeChaName.c_str());
    }

    //! 국가정보 2012-02-13 jgkim
    inline wb::EM_COUNTRY GetCountry() const { return m_Country; }

    //! DB 고유번호
    inline DWORD GetDbNum() const { return m_DbNum; }

    //! 클럽 이름
    inline const char* GetName() const { return m_szName; }

    //! 클럽 마스터 Char DB Num
    inline DWORD GetMasterDbNum() const { return m_MasterCharDbNum; }

    inline const char* GetMasterName() const { return m_szMasterName; }

    //! 클럽 마스터 레벨 2012-02-13 jgkim
    inline int GetMasterLevel() const { return m_MasterLevel; }

    inline DWORD GetRank() const { return m_dwRank; }

    //! 클럽 창설 시간
    inline __time64_t GetOrganizeDate() const { return m_tOrganize; }
};
*/
struct GLCLUBBATTLE_LOBY;

struct GLCLUBBATTLE
{
    enum CLUB_BATTLE_ENUM
    {
        CLUB_BATTLE_NOW				= 0,	// 진행중	
        CLUB_BATTLE_DRAW			= 1,	// 무승부
        CLUB_BATTLE_ARMISTICE		= 2,	// 휴전
        CLUB_BATTLE_SUBMISSION		= 3,	// 항복
        CLUB_BATTLE_SUBMISSION_WIN	= 4,	// 항복 승리
        CLUB_BATTLE_LOSE			= 5,	// 패배
        CLUB_BATTLE_WIN				= 6		// 승리
    };

    DWORD				m_dwCLUBID;					// 적대 클럽 ID
    __time64_t			m_tStartTime;				// 시작시간
    __time64_t			m_tEndTime;					// 종료시간
    WORD				m_wKillPointDB;				// 배틀 점수 DB
    WORD				m_wDeathPointDB;			// 배틀 점수 DB
    WORD				m_wKillPoint;				// 배틀 점수
    WORD				m_wDeathPoint;				// 배틀 점수
    WORD				m_wKillPointTemp;			// Update용 배틀 포인트( 필드용 )
    WORD				m_wDeathPointTemp;			// Update용 배틀 포인트( 필드용 )
    char				m_szClubName[CLUB_NAME_LENGTH];	// 상대클럽 이름 ( 클라용 )
    bool				m_bKillUpdate;				// Kill Update 확인
    CLUB_BATTLE_ENUM	m_nBattleFlag;				// 현재 배틀 상태 확인
    bool				m_bAlliance;				// 동맹 배틀 정보

    GLCLUBBATTLE()
        : m_dwCLUBID ( 0 )
        , m_wKillPointDB ( 0 )
        , m_wDeathPointDB ( 0 )
        , m_wKillPoint ( 0 )
        , m_wDeathPoint ( 0 )
        , m_wKillPointTemp( 0 )
        , m_wDeathPointTemp( 0 )
        , m_bKillUpdate ( false )
        , m_bAlliance( false )
        , m_nBattleFlag ( CLUB_BATTLE_NOW )
    {
        CTime tDisTime(1970,2,1,1,1,1);
        m_tStartTime = tDisTime.GetTime();
        m_tEndTime = tDisTime.GetTime();

        memset(m_szClubName, 0, sizeof(char)* CHAR_SZNAME);
    }

    GLCLUBBATTLE(const GLCLUBBATTLE& value)
    {
        operator = (value);
    }

    GLCLUBBATTLE(const GLCLUBBATTLE_LOBY& value)
    {
        operator = (value);
    }

    //! 상대클럽 이름 (클라용)
    void SetName(const char* szName)
    {
        StringCchCopy(m_szClubName, CHAR_SZNAME, szName);
    }

public:
    GLCLUBBATTLE& operator = ( const GLCLUBBATTLE& value );
    GLCLUBBATTLE& operator = ( const GLCLUBBATTLE_LOBY& value );
};

// 삭제 예정인 클럽배틀 정보
struct GLCLUBBATTLE_DEL
{
    GLCLUBBATTLE	m_sClubBattle;		// 클럽 정보
    DWORD			m_dwField;			// 필드 정보 수락여부
    bool			m_bIsComplated;		// 정보 조합 끝났는지

    GLCLUBBATTLE_DEL()
        : m_dwField ( 0 )
        , m_bIsComplated ( false )
    {
    }
};

typedef std::map<DWORD, GLCLUBBATTLE> CLUB_BATTLE;
typedef CLUB_BATTLE::iterator    	  CLUB_BATTLE_ITER;
typedef CLUB_BATTLE::const_iterator   CLUB_BATTLE_CITER;
typedef CLUB_BATTLE::value_type    	  CLUB_BATTLE_VALUE;

typedef std::map<DWORD, GLCLUBBATTLE_DEL> CLUB_BATTLE_DEL;
typedef CLUB_BATTLE_DEL::iterator		  CLUB_BATTLE_DEL_ITER;
typedef CLUB_BATTLE_DEL::const_iterator	  CLUB_BATTLE_DEL_CITER;
typedef CLUB_BATTLE_DEL::value_type 	  CLUB_BATTLE_DEL_VALUE;

struct GLCLUBBATTLE_LOBY
{
    DWORD			m_dwCLUBID;					// 적대 클럽 ID
    __time64_t		m_tStartTime;				// 시작시간
    __time64_t		m_tEndTime;					// 종료시간
    WORD			m_wKillPoint;				// 배틀 점수
    WORD			m_wDeathPoint;				// 배틀 점수
    bool			m_bAlliance;
    char			m_szClubName[CHAR_SZNAME];	// 상대클럽 이름 ( 클라용 )

    GLCLUBBATTLE_LOBY() 
        : m_dwCLUBID(0)
        , m_tStartTime(0)
        , m_tEndTime(0)
        , m_wKillPoint(0)
        , m_wDeathPoint(0)
    {
        memset( m_szClubName, 0, sizeof( char ) * CHAR_SZNAME );
    }

    GLCLUBBATTLE_LOBY ( const GLCLUBBATTLE_LOBY& value )
    {
        operator=( value );
    }

    GLCLUBBATTLE_LOBY ( const GLCLUBBATTLE& value )
    {
        operator=( value );
    }

    GLCLUBBATTLE_LOBY& operator = ( const GLCLUBBATTLE_LOBY& value );
    GLCLUBBATTLE_LOBY& operator = ( const GLCLUBBATTLE& value );

    void SetName(const char* szName)
    {
        if (szName)
            StringCchCopy(m_szClubName, CHAR_SZNAME, szName);
    }
};

//! 2012-02-27 jgkim
namespace club
{

    //! 정렬가능한 항목
    enum ALIGN_ORDER_TYPE
    {
        ORDER_CHA_LEVEL,
        ORDER_CHA_NAME, //! Character name    
        ORDER_MAP,
        ORDER_FLAG,
    };

    //! 정렬순서 
    enum ALIGN_ORDER_DES_ASC
    {
        SQL_ASC, //! 오름차순
        SQL_DESC, //! 내림차순
    };

    struct ALIGN_ORDER
    {
        ALIGN_ORDER_TYPE m_Type; //! 정렬 항목
        ALIGN_ORDER_DES_ASC m_Degree; //! 오름차순/내림차순

        ALIGN_ORDER(ALIGN_ORDER_TYPE Type, ALIGN_ORDER_DES_ASC Degree=SQL_ASC)
            : m_Type(Type)
            , m_Degree(Degree)
        {
        }

        //! 정렬 항목
        inline ALIGN_ORDER_TYPE Type() const { return m_Type; }

        //! 오름차순/내림차순
        inline ALIGN_ORDER_DES_ASC Degree() const { return m_Degree; }
    };

    //! 클럽 권한
    enum AUTHORITY
    {
        AUTHORITY_NONE          = 0x00000000,
        AUTHORITY_MEMBER_JOIN   = 0x00000001, //! 클럽에 가입 시킬 수 있는 권한
        AUTHORITY_MEMBER_KICK   = 0x00000002, //! 클럽에서 제명 시킬 수 있는 권한
        AUTHORITY_NOTICE_CHANGE = 0x00000004, //! 클럽 공지를 작성할 수 있는 권한    
        AUTHORITY_MARK_CHANGE   = 0x00000008, //! 클럽 마크를 변경할 수 있는 권한
        AUTHORITY_DEATH_MATCH   = 0x00000010, //! CDM 참가 자격
        AUTHORITY_RANK_UP       = 0x00000020, //! 클럽 랭크업
        AUTHORITY_CHANGE        = 0x00000040, //! 클럽원 계급 편집/변경
        AUTHORITY_ITEM          = 0x00000080, //! 클럽 창고/아이템 넣기/꺼내기
        AUTHORITY_MONEY         = 0x00000100, //! 클럽 창고/입출금
        AUTHORITY_ALLIANCE      = 0x00000200, //! 클럽 동맹 신청하기/받기
        AUTHORITY_ALLIANCE_DIS  = 0x00000400, //! 클럽 동맹 해지
        AUTHORITY_CLUB_BATTLE   = 0x00000800, //! 클럽 배틀 신청 하기/받기/항복/휴전
        AUTHORITY_CD		    = 0x00001000, //! 선도클럽 결정전에 CD 를 인증 할 수 있는 권한

        AUTHORITY_NUMBER = 12, //! CD 인증권한은 빼고 계산해야 한다.
        AUTHORITY_GRADE = 10, //! 클럽원 권한 개수

        AUTHORITY_SUBMATER = AUTHORITY_MEMBER_JOIN | AUTHORITY_MEMBER_KICK | AUTHORITY_NOTICE_CHANGE | AUTHORITY_MARK_CHANGE | AUTHORITY_DEATH_MATCH |
                             AUTHORITY_RANK_UP | AUTHORITY_CHANGE | AUTHORITY_ITEM | AUTHORITY_MONEY | AUTHORITY_ALLIANCE | 
                             AUTHORITY_ALLIANCE_DIS | AUTHORITY_CLUB_BATTLE | AUTHORITY_CD,
    };

    //! 클럽 권한
    struct AUTH_GRADE
    {
        std::string m_Desc; //! 문자열
        DWORD m_Flag; //! 권한 AUTHORITY

        AUTH_GRADE()
            : m_Flag(0)
        {
        }

        AUTH_GRADE(const std::string& Desc, DWORD Flag)
            : m_Desc(Desc)
            , m_Flag(Flag)
        {
        }

        AUTH_GRADE(const char* szDesc, DWORD Flag)
            : m_Flag(Flag)
        {
            if (szDesc)
                m_Desc = szDesc;
        }

        void Reset()
        {
            m_Desc = "";
            m_Flag = 0;
        }
    };

    //! 클럽 권한 DB 에서 가져오기 용도
    struct AUTH_GRADE_DB : public AUTH_GRADE
    {
        size_t m_Index;

        AUTH_GRADE_DB(size_t Index, const std::string& Desc, DWORD Flag)
            : AUTH_GRADE(Desc, Flag)
            , m_Index(Index)
        {
        }
    };

    extern __time64_t ClubLogDeleteTime; //! Club Log 삭제 시간

    //! 클럽 로그 타입
    enum EM_LOG_TYPE
    {
        LOG_ALL               =  0, //! 모든 로그
        LOG_MEMBER            =  1, //! 멤버 탈퇴/삭제
        LOG_CDM               =  2, //! Club Death Math
        LOG_GUIDANCE          =  3, //! 선도클럽
        LOG_CLUB_STORAGE      =  4, //! 클럽 창고
        LOG_CLUB_NOTICE       =  5, //! 클럽 공지 변경
        LOG_CLUB_BATTLE       =  6, //! 클럽 Battle
        LOG_CLUB_ALLIANCE     =  7, //! 클럽 동맹
        LOG_CLUB_RANK_UP      =  8, //! 클럽 랭크업
        LOG_CLUB_AUTH_CHANGE  =  9, //! 클럽 권한 관련
        LOG_ENCHANT           = 10, //! 아이템 개조 관련
        LOG_PARTY_RECRUIT     = 11, //! 파티 모집
        LOG_KILLED_BY_MONSTER = 12, //! 몬스터에게 사망함
        LOG_KILL_MONSTER      = 13, //! 몬스터를 쓰러뜨림
        LOG_CTF               = 14, //! 점령전 로그
        LOG_MEMBER_LEVEL_UP   = 15, //! 클럽 멤버 레벨업
        LOG_KILL_PLAYER       = 16, //! 플레이어를 쓰러뜨림
		LOG_CLUB_RENAME       = 17, //! 클럽 이름 변경
    };

    struct LOG_DATA
    {
        __int64     m_LogNum;
        __time64_t  m_LogDate;
        BYTE        m_LogType; // club::EM_LOG_TYPE        
        std::string m_LogText;

        MSGPACK_DEFINE(m_LogNum, m_LogDate, m_LogType, m_LogText);

        LOG_DATA()
            : m_LogNum(0)
            , m_LogType(0)
            , m_LogDate(0)            
        {
        }

        LOG_DATA(
            __int64 _LogNum,
            club::EM_LOG_TYPE _LogType,
            const __time64_t& _LogDate,
            const std::string& _LogText)
            : m_LogNum(_LogNum)
            , m_LogType(_LogType)
            , m_LogDate(_LogDate)
            , m_LogText(_LogText)
        {
        }
    };

    struct GLCLUBMEMBER_CLIENT : public GLCLUBMEMBER
    {
        std::string m_MapName; //! 현재 맵 이름
        std::string m_GradeName; //! 권한 이름

        GLCLUBMEMBER_CLIENT()
        {
        }

        //! 현재 맵
        std::string GetMapName() const { return m_MapName; }
        void SetMapName(const std::string& MapName) { m_MapName=MapName; }

        //! 권한 이름
        std::string GetGradeName() const { return m_GradeName; }
        void SetGradeName(const std::string& GradeName) { m_GradeName=GradeName; }
    };

    //! Online/Offline
    struct MEMBER_ON_OFFLINE
    {
        WORD m_Online; //! Online 인 클럽원 숫자
        WORD m_Total; //! 전체 클럽원 숫자

        MEMBER_ON_OFFLINE()
            : m_Online(0)
            , m_Total(0)
        {
        }
    };

    struct GRADE_FLAG
    {
        size_t m_Index;
        DWORD  m_Flag;

        MSGPACK_DEFINE(m_Index, m_Flag);

        GRADE_FLAG()
            : m_Index(0)
            , m_Flag(0)
        {
        }

        GRADE_FLAG(size_t Index, DWORD Flag)
            : m_Index(Index)
            , m_Flag(Flag)
        {
        }
    };

} // namespace club

#endif // _GL_CLUB_DEFINE_H_
