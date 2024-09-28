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
		// �߹��� ��������� �޸𸮻���� ũ�Ƿ� �÷���;
		CLUBNAME_LIMIT_TAIWAN = 20,
	};
    //! Ŭ�� ����Ÿ��
    enum EM_PUBLIC_TYPE
    {
        PUBLIC_FOR_NEWBIE            = 0x0001, //! ���Ի��鿡�� ����
        PUBLIC_FOR_OTHER_CLUB_MEMBER = 0x0002, //! �ٸ� Ŭ�����鿡�� ����
        PUBLIC_ALL                   = PUBLIC_FOR_NEWBIE | PUBLIC_FOR_OTHER_CLUB_MEMBER,
    };

    //! �ڵ��߹�
    struct AUTO_KICK_OUT
    {
        size_t m_GradeIndex;  //! �߹��� ���
        WORD   m_Day;         //! �󸶵��� �������� ������ kick �� ���ΰ�
        bool   m_Use; //! �ڵ� �߹��� ���/������� ����

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

//! ������ �����ϸ� VERSION �� �ø��ʽÿ�. ������ ������ ���� ���� ���ľ� �մϴ�.
//! MSGPACK_DEFINE �� Network �� ���� ���۵� Member ������ �����ּž� ������ �˴ϴ�.
struct GLCLUBMEMBER
{
public:
    //! ������ �����ϸ� VERSION �� �ø��ʽÿ�. ������ ������ ���� ���� ���ľ� �մϴ�.    
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
    DWORD		m_ChaDbNum;            //! ĳ���� DB ��ȣ
    WORD        m_ChaLevel;            //! ĳ���� ����
    int				m_Channel;             //! ���� ��ġ�� ä��
    MapID		m_CurrentMap;          //! �ʹ�ȣ
    DWORD		m_Grade;               //! ��� ����, AUTHORITY_GRADE ����
    D3DXVECTOR3	m_CurrentPos;          //! ��ġ��ȣ
    std::string	m_ChaName;             //! ĳ���͸�
    bool		m_Online;              //! �¶��λ���
    __time64_t  m_LastPlayTime;        //! ������ �÷��� �ð� ��������/��������
    DWORD       m_ChaClass;            //! ĳ���� ���� EMCHARCLASS

    //! MSGPACK_DEFINE �� Network �� ���� ���۵� Member ������ �����ּž� ������ �˴ϴ�.
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
    //! ĳ���͸�
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

    //! ĳ���� DB ��ȣ
    DWORD GetChaDbNum() const { return m_ChaDbNum; }

    //! ��� ����, AUTHORITY_GRADE ����
    DWORD GetGrade() const { return m_Grade; }
    void SetGrade(DWORD Grade) { m_Grade = Grade; }

    //! ������ �÷��� �ð�
    void SetLastPlayTime(const __time64_t& LastPlayTime) { m_LastPlayTime = LastPlayTime; }
    __time64_t GetLastPlayTime() const { return m_LastPlayTime; }

    //! ĳ���� ���� EMCHARCLASS
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
    char  m_szName[CHAR_SZNAME]; //! Ŭ���̸�
    char  m_szMasterName[CHAR_SZNAME]; //! Ŭ�� Master �̸�

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
    //! ������ �����Ű�� VERSION �� �ø��ʽÿ�.
    //! VERSION �� �ø��ø� compile ������ ���°��� �����ؾ� �� �κ��Դϴ�.
    enum { VERSION=0x0005, };

    DWORD		m_DbNum; //! DB ������ȣ
    wb::EM_COUNTRY m_Country; //! �������� 2012-02-13 jgkim
    TCHAR		m_szName[CHAR_SZNAME]; //! Ŭ���̸�

    DWORD		m_MasterCharDbNum; //! Ŭ�� ������ Char DB Num
    TCHAR		m_szMasterName[CHAR_SZNAME]; //! Ŭ�� ������ �̸�
    int         m_MasterLevel; //! Ŭ�� ������ ���� 2012-02-13 jgkim
    DWORD		m_dwCDCertifior; //! ����Ŭ�� CD ���� �븮�� Char DB Num

    DWORD		m_dwRank; //! Ŭ�� ��ũ

    DWORD		m_dwMarkVER; //! Ŭ�� ��ũ����
    DWORD		m_aryMark[EMCLUB_MARK_SX*EMCLUB_MARK_SY]; //! Ŭ����ũ

    __time64_t	m_tOrganize;		//! Ŭ�� â�� �ð�
    __time64_t	m_tDissolution;		//! Ŭ�� ��ü ���� �ð�.
    __time64_t	m_tAuthority;		//! Ŭ�������� ���� ����

    __time64_t	m_tAllianceSec;		//! ���� Ż�� Ȥ�� ���� �ð� + ( ���� ���ѽð� )
    __time64_t	m_tAllianceDis;		//! ���� �ػ� �ð� + ( â�� ���� �ð� )

    LONGLONG	m_lnIncomeMoney;
    LONGLONG	m_lnStorageMoney;

    DWORD		m_dwBattleWin;		//	��Ʋ ����Ʈ ( �� )
    DWORD		m_dwBattleLose;		//	��Ʋ ����Ʈ ( �� )
    DWORD		m_dwBattleDraw;		//	��Ʋ ����Ʈ ( ��)
    __time64_t	m_tLastBattle;		//	������ ��Ʋ ����ð�

    DWORD		m_dwAllianceBattleWin;		//	���͹�Ʋ ����Ʈ ( �� )
    DWORD		m_dwAllianceBattleLose;		//	���͹�Ʋ ����Ʈ ( �� )
    DWORD		m_dwAllianceBattleDraw;		//	���͹�Ʋ ����Ʈ ( ��)

    TCHAR		m_szNotice[EMCLUB_NOTICE_LEN+1];
    TCHAR		m_szNoticeChaName[CHAR_SZNAME]; //! �����ۼ� ĳ���� �̸�
    __time64_t	m_NoticeDate;		//! �����ۼ� ��¥/�ð�

    DWORD m_PublicType; //! Ŭ�� ���� Ÿ�� EM_PUBLIC_TYPE ����

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

    //! Ŭ�� ������ ���� 2012-02-13 jgkim
    inline void SetMasterLevel(int Level)
    {
        m_MasterLevel = Level;
    }

    //! Ŭ�� ����
    inline void SetNotice(const std::string& Notice)
    {
        StringCchCopy(m_szNotice, EMCLUB_NOTICE_LEN+1, Notice.c_str());
    }

    inline void SetNoticeChaName(const std::string& NoticeChaName)
    {
        StringCchCopy(m_szNoticeChaName, CHAR_SZNAME, NoticeChaName.c_str());
    }

    //! �������� 2012-02-13 jgkim
    inline wb::EM_COUNTRY GetCountry() const { return m_Country; }

    //! DB ������ȣ
    inline DWORD GetDbNum() const { return m_DbNum; }

    //! Ŭ�� �̸�
    inline const char* GetName() const { return m_szName; }

    //! Ŭ�� ������ Char DB Num
    inline DWORD GetMasterDbNum() const { return m_MasterCharDbNum; }

    inline const char* GetMasterName() const { return m_szMasterName; }

    //! Ŭ�� ������ ���� 2012-02-13 jgkim
    inline int GetMasterLevel() const { return m_MasterLevel; }

    inline DWORD GetRank() const { return m_dwRank; }

    //! Ŭ�� â�� �ð�
    inline __time64_t GetOrganizeDate() const { return m_tOrganize; }
};
*/
struct GLCLUBBATTLE_LOBY;

struct GLCLUBBATTLE
{
    enum CLUB_BATTLE_ENUM
    {
        CLUB_BATTLE_NOW				= 0,	// ������	
        CLUB_BATTLE_DRAW			= 1,	// ���º�
        CLUB_BATTLE_ARMISTICE		= 2,	// ����
        CLUB_BATTLE_SUBMISSION		= 3,	// �׺�
        CLUB_BATTLE_SUBMISSION_WIN	= 4,	// �׺� �¸�
        CLUB_BATTLE_LOSE			= 5,	// �й�
        CLUB_BATTLE_WIN				= 6		// �¸�
    };

    DWORD				m_dwCLUBID;					// ���� Ŭ�� ID
    __time64_t			m_tStartTime;				// ���۽ð�
    __time64_t			m_tEndTime;					// ����ð�
    WORD				m_wKillPointDB;				// ��Ʋ ���� DB
    WORD				m_wDeathPointDB;			// ��Ʋ ���� DB
    WORD				m_wKillPoint;				// ��Ʋ ����
    WORD				m_wDeathPoint;				// ��Ʋ ����
    WORD				m_wKillPointTemp;			// Update�� ��Ʋ ����Ʈ( �ʵ�� )
    WORD				m_wDeathPointTemp;			// Update�� ��Ʋ ����Ʈ( �ʵ�� )
    char				m_szClubName[CLUB_NAME_LENGTH];	// ���Ŭ�� �̸� ( Ŭ��� )
    bool				m_bKillUpdate;				// Kill Update Ȯ��
    CLUB_BATTLE_ENUM	m_nBattleFlag;				// ���� ��Ʋ ���� Ȯ��
    bool				m_bAlliance;				// ���� ��Ʋ ����

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

    //! ���Ŭ�� �̸� (Ŭ���)
    void SetName(const char* szName)
    {
        StringCchCopy(m_szClubName, CHAR_SZNAME, szName);
    }

public:
    GLCLUBBATTLE& operator = ( const GLCLUBBATTLE& value );
    GLCLUBBATTLE& operator = ( const GLCLUBBATTLE_LOBY& value );
};

// ���� ������ Ŭ����Ʋ ����
struct GLCLUBBATTLE_DEL
{
    GLCLUBBATTLE	m_sClubBattle;		// Ŭ�� ����
    DWORD			m_dwField;			// �ʵ� ���� ��������
    bool			m_bIsComplated;		// ���� ���� ��������

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
    DWORD			m_dwCLUBID;					// ���� Ŭ�� ID
    __time64_t		m_tStartTime;				// ���۽ð�
    __time64_t		m_tEndTime;					// ����ð�
    WORD			m_wKillPoint;				// ��Ʋ ����
    WORD			m_wDeathPoint;				// ��Ʋ ����
    bool			m_bAlliance;
    char			m_szClubName[CHAR_SZNAME];	// ���Ŭ�� �̸� ( Ŭ��� )

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

    //! ���İ����� �׸�
    enum ALIGN_ORDER_TYPE
    {
        ORDER_CHA_LEVEL,
        ORDER_CHA_NAME, //! Character name    
        ORDER_MAP,
        ORDER_FLAG,
    };

    //! ���ļ��� 
    enum ALIGN_ORDER_DES_ASC
    {
        SQL_ASC, //! ��������
        SQL_DESC, //! ��������
    };

    struct ALIGN_ORDER
    {
        ALIGN_ORDER_TYPE m_Type; //! ���� �׸�
        ALIGN_ORDER_DES_ASC m_Degree; //! ��������/��������

        ALIGN_ORDER(ALIGN_ORDER_TYPE Type, ALIGN_ORDER_DES_ASC Degree=SQL_ASC)
            : m_Type(Type)
            , m_Degree(Degree)
        {
        }

        //! ���� �׸�
        inline ALIGN_ORDER_TYPE Type() const { return m_Type; }

        //! ��������/��������
        inline ALIGN_ORDER_DES_ASC Degree() const { return m_Degree; }
    };

    //! Ŭ�� ����
    enum AUTHORITY
    {
        AUTHORITY_NONE          = 0x00000000,
        AUTHORITY_MEMBER_JOIN   = 0x00000001, //! Ŭ���� ���� ��ų �� �ִ� ����
        AUTHORITY_MEMBER_KICK   = 0x00000002, //! Ŭ������ ���� ��ų �� �ִ� ����
        AUTHORITY_NOTICE_CHANGE = 0x00000004, //! Ŭ�� ������ �ۼ��� �� �ִ� ����    
        AUTHORITY_MARK_CHANGE   = 0x00000008, //! Ŭ�� ��ũ�� ������ �� �ִ� ����
        AUTHORITY_DEATH_MATCH   = 0x00000010, //! CDM ���� �ڰ�
        AUTHORITY_RANK_UP       = 0x00000020, //! Ŭ�� ��ũ��
        AUTHORITY_CHANGE        = 0x00000040, //! Ŭ���� ��� ����/����
        AUTHORITY_ITEM          = 0x00000080, //! Ŭ�� â��/������ �ֱ�/������
        AUTHORITY_MONEY         = 0x00000100, //! Ŭ�� â��/�����
        AUTHORITY_ALLIANCE      = 0x00000200, //! Ŭ�� ���� ��û�ϱ�/�ޱ�
        AUTHORITY_ALLIANCE_DIS  = 0x00000400, //! Ŭ�� ���� ����
        AUTHORITY_CLUB_BATTLE   = 0x00000800, //! Ŭ�� ��Ʋ ��û �ϱ�/�ޱ�/�׺�/����
        AUTHORITY_CD		    = 0x00001000, //! ����Ŭ�� �������� CD �� ���� �� �� �ִ� ����

        AUTHORITY_NUMBER = 12, //! CD ���������� ���� ����ؾ� �Ѵ�.
        AUTHORITY_GRADE = 10, //! Ŭ���� ���� ����

        AUTHORITY_SUBMATER = AUTHORITY_MEMBER_JOIN | AUTHORITY_MEMBER_KICK | AUTHORITY_NOTICE_CHANGE | AUTHORITY_MARK_CHANGE | AUTHORITY_DEATH_MATCH |
                             AUTHORITY_RANK_UP | AUTHORITY_CHANGE | AUTHORITY_ITEM | AUTHORITY_MONEY | AUTHORITY_ALLIANCE | 
                             AUTHORITY_ALLIANCE_DIS | AUTHORITY_CLUB_BATTLE | AUTHORITY_CD,
    };

    //! Ŭ�� ����
    struct AUTH_GRADE
    {
        std::string m_Desc; //! ���ڿ�
        DWORD m_Flag; //! ���� AUTHORITY

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

    //! Ŭ�� ���� DB ���� �������� �뵵
    struct AUTH_GRADE_DB : public AUTH_GRADE
    {
        size_t m_Index;

        AUTH_GRADE_DB(size_t Index, const std::string& Desc, DWORD Flag)
            : AUTH_GRADE(Desc, Flag)
            , m_Index(Index)
        {
        }
    };

    extern __time64_t ClubLogDeleteTime; //! Club Log ���� �ð�

    //! Ŭ�� �α� Ÿ��
    enum EM_LOG_TYPE
    {
        LOG_ALL               =  0, //! ��� �α�
        LOG_MEMBER            =  1, //! ��� Ż��/����
        LOG_CDM               =  2, //! Club Death Math
        LOG_GUIDANCE          =  3, //! ����Ŭ��
        LOG_CLUB_STORAGE      =  4, //! Ŭ�� â��
        LOG_CLUB_NOTICE       =  5, //! Ŭ�� ���� ����
        LOG_CLUB_BATTLE       =  6, //! Ŭ�� Battle
        LOG_CLUB_ALLIANCE     =  7, //! Ŭ�� ����
        LOG_CLUB_RANK_UP      =  8, //! Ŭ�� ��ũ��
        LOG_CLUB_AUTH_CHANGE  =  9, //! Ŭ�� ���� ����
        LOG_ENCHANT           = 10, //! ������ ���� ����
        LOG_PARTY_RECRUIT     = 11, //! ��Ƽ ����
        LOG_KILLED_BY_MONSTER = 12, //! ���Ϳ��� �����
        LOG_KILL_MONSTER      = 13, //! ���͸� �����߸�
        LOG_CTF               = 14, //! ������ �α�
        LOG_MEMBER_LEVEL_UP   = 15, //! Ŭ�� ��� ������
        LOG_KILL_PLAYER       = 16, //! �÷��̾ �����߸�
		LOG_CLUB_RENAME       = 17, //! Ŭ�� �̸� ����
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
        std::string m_MapName; //! ���� �� �̸�
        std::string m_GradeName; //! ���� �̸�

        GLCLUBMEMBER_CLIENT()
        {
        }

        //! ���� ��
        std::string GetMapName() const { return m_MapName; }
        void SetMapName(const std::string& MapName) { m_MapName=MapName; }

        //! ���� �̸�
        std::string GetGradeName() const { return m_GradeName; }
        void SetGradeName(const std::string& GradeName) { m_GradeName=GradeName; }
    };

    //! Online/Offline
    struct MEMBER_ON_OFFLINE
    {
        WORD m_Online; //! Online �� Ŭ���� ����
        WORD m_Total; //! ��ü Ŭ���� ����

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
