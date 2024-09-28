#ifndef _GL_GUIDANCE_H_
#define _GL_GUIDANCE_H_

#pragma once

#include <vector>

#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"

// 구버전의 선도전 [10/29/2014 hsshin];
// 주의 ! : 구버전의 선도전은 신규 선도전과 혼용될 수 없다.;
// 구버전을 사용하고 싶다면 신규 선도전은 해당 인던스크립트를 로드를 하지 않아야한다.;
// 신규 선도전은 게임진행에 대한 부분을 인던에서 처리 후;
// 선도클럽 보상에 대한 부분은 구버전을 사용하기 때문에 구버전으로 열리는 선도전이 있다면 제대로된 동작을 보장하기 어렵다.;

// 리뉴얼된 신규인던의 선도전이 전부 들어가게 되면 해당 코드는 삭제해야함;
// 현재는 선도클럽의 대한 보상부분은 구버전의 기능을 사용하고 있어서 삭제불가;
// 해당부분을 새로 만들거나 옮긴후 삭제해야함;

namespace db {
    class DbAction;
}

struct GLGUID_MAP_EFFECT
{
	float				fTime;
	SMAP_EFFECT			sMapEffect;

	GLGUID_MAP_EFFECT()
	{
		fTime = 0.0f;		
	}	
};

typedef std::vector<GLGUID_MAP_EFFECT>	VEC_GUID_MAP_EFFECT;
typedef VEC_GUID_MAP_EFFECT::iterator	VEC_GUID_MAP_EFFECT_ITER;

struct SBATTLE_TIME
{
	DWORD dwWeekDay; // 일 월 화 수 목 금 토 1 2 3 4 5 6 7
	DWORD dwStartTime; // 시간 (0-23까지)

	SBATTLE_TIME () :
		dwWeekDay(0),
		dwStartTime(0)
	{
	}
};

struct SAWARD_MOB
{
	SNATIVEID	nidGenMob;	//	결정전 완료후 생성되는 보상-몹.
	WORD		wGenPosX;	//	' 의 위치.
	WORD		wGenPosY;

	SAWARD_MOB() :
		nidGenMob(0,0),
		wGenPosX(0),
		wGenPosY(0)
	{
	}
};

struct SGUID_KILL_LOG
{
	WORD wKill;
	WORD wDeath;

	SGUID_KILL_LOG()
		: wKill( 0 ) 
		, wDeath( 0 ) 
	{
		wKill = 0;
		wDeath = 0;
	}
};

class GLAgentServer;
class GLGaeaServer;
class GLChar;

typedef std::map<DWORD,SGUID_KILL_LOG>	GUID_KILL_LOG_MAP;
typedef GUID_KILL_LOG_MAP::iterator		GUID_KILL_LOG_MAP_ITER;

class GLGuidance
{
public:
	enum { MAX_TIME = 4, };

    struct NEW_COMMISSION
    {
        DWORD m_ReqChaDbNum;           //! 커미션을 변경을 예약한 캐릭터 DB 번호
        float m_fCommissionTimer;	   //! 새 커미션 변경 타이머
        float m_fNewCommissionPercent; //! 새 커미션
        bool  m_bNewCommission;		   //! 새 커미션 변경 예약

        NEW_COMMISSION()
            : m_ReqChaDbNum(0)            
            , m_fCommissionTimer(0.0f)
            , m_fNewCommissionPercent(0.0f)
            , m_bNewCommission(false)
        {
        }

        void SetData(DWORD ReqChaDbNum, bool bNewCommission, float fCommissionTimer, float fNewCommissionPercent)
        {
            m_ReqChaDbNum = ReqChaDbNum;
            m_bNewCommission = bNewCommission;
            m_fCommissionTimer = fCommissionTimer;
            m_fNewCommissionPercent = fNewCommissionPercent;
        }

        inline void AddCommissionTimer(float AddTime)
        {
            m_fCommissionTimer += AddTime;
        }

        inline float CommissionTimer() const { return m_fCommissionTimer; }
        inline bool NewCommission() const { return m_bNewCommission; }
        inline DWORD ReqChaDbNum() const { return m_ReqChaDbNum; }
        inline float NewCommissionPercent() const { return m_fNewCommissionPercent; }
    };    

public:
    GLGuidance();
    virtual ~GLGuidance();
    GLGuidance(const GLGuidance& value)
    {
        operator= (value);
    }

    GLGuidance& operator= (const GLGuidance& value);

    bool operator < (const GLGuidance& sGuidance)
    {
        return m_dwID < sGuidance.m_dwID;
    }

public:
	DWORD				m_dwID;					//! 관리 ID.
	std::string			m_strName;				//! 관리 이름.
	std::vector<DWORD>	m_vecMaps;				//! 선도 관리 맵.

	DWORD				m_dwClubMap;			//!	클럽 맵. ( 전산실 )
	DWORD				m_dwCLubMapGate;		//!	클럽 맵에 있는 출입구.
	DWORD				m_dwClubHallMap;		//!	클럽 맵. ( 입구맵 ) 

	float				m_fItemDropRate;		//! 아이템 드롭 확율.
	float				m_fDefaultCommissionPer;//! 기본 수수료율

	DWORD				m_dwGuidanceClub;		//!	선도 클럽 ID
	float				m_fCommissionPercent;	//!	수수료율

	DWORD				m_dwBattleTime;
	BOOL				m_bLog;					//! 로그 출력 여부
	SBATTLE_TIME		m_sTIME[MAX_TIME];		//! 전투시간.

	std::vector<SAWARD_MOB> m_vecAwardMob;

    NEW_COMMISSION m_NewCommission; //! 새 커미션
	
	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;
	
	bool				m_bBattle;
	DWORD				m_dwBattleOrder;
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;         // 선도전 진행시간
    float               m_fNotifyRemainTimer;   // 남은시간 알림을 위한 타이머 (현재 10분마다)

	DWORD				m_dwCERTIFY_CHARID;
	DWORD				m_dwCERTIFY_NPCID;
	float				m_fCERTIFY_TIMER;
	D3DXVECTOR3			m_vCERTIFY_POS;

	bool				m_bBattleEndCheck;		// 선도전 종료직후 전산실 입장을 막기위해
	float				m_fCHECK_TIMER;
	float				m_fCHECK_TIME;	
	float				m_fLog_TIMER;

	bool				m_bMapEffectUse;		// 맵 이펙트 효과 사용여부
	float				m_fHoldCertifyTime;		// 인증 지속시간 타임
	int					m_nMapEffectIndex;		// 적용되고 있는 맵 이펙트 인덱스	

	GUID_KILL_LOG_MAP	m_mapKillLog;
		
	VEC_GUID_MAP_EFFECT	m_vecMapEffect;

public:
    void AddGameDbJob(db::DbAction* pJob);
	bool Load(const std::string& strFile, bool bToolMode);

    //! 관리 이름
    const std::string& GetName() const { return m_strName; }
    std::string GetName() { return m_strName; }

    //! 관리 ID
    DWORD GetId() const { return m_dwID; }

	inline bool IsCertify() const { return m_dwCERTIFY_CHARID!=0; }
    
    inline bool IsNewCommission() const { return m_NewCommission.m_bNewCommission; }
	bool IsGuidClub(DWORD dwClubID) const;

    inline bool IsBattle() const { return m_bBattle; }
	DWORD IsBattleWeekDay ( int nDayOfWeek, int nDay );
	
	// 선도클럽 시작시간인가?
	bool IsBattleHour ( DWORD dwORDER, int nHour );

	// 최종적으로 선도클럽결정전이 일어난 날을 세팅한다.
	void UpdateBattleDay ( int nDay );
	// 선도클럽 결정전 남은 시간을 클라이언트에게 알린다.
	void UpdateNotifyBattle(GLAgentServer* pGLAgentServer, DWORD dwORDER, int nHour, int nMinute );
	void UpdateCommission(GLAgentServer* pGLAgentServer, float fElaps);

	void CheckExtraGuild(GLGaeaServer* pGLGaeaServer, float fElaps);

	void CheckMapEffect(GLGaeaServer* pGLGaeaServer, float fElaps);

	// 선도클럽 결정전을 시작한다.
	void DoBattleStart (GLAgentServer* pGLAgentServer, DWORD dwORDER, int nDay );
	void DoBattleEnd(GLAgentServer* pGLAgentServer);

	void DoCertifyEnd();

	bool ChangeGuidClub(GLAgentServer* pGLAgentServer, DWORD dwClubID);
	bool ChangeCommission(GLAgentServer* pGLAgentServer, float fRate);

    void AddKillLog( DWORD dwKillChar, DWORD dwDeathChar );		//	캐릭터 Kill/Death 로그를 남긴다.
};

class GLGuidanceMan
{
public:
    GLGuidanceMan();
    virtual ~GLGuidanceMan();

public:
	typedef std::vector<GLGuidance>		GUIDANCE_VEC;
	typedef GUIDANCE_VEC::iterator		GUIDANCE_VEC_ITER;
	typedef std::pair<GUIDANCE_VEC_ITER,GUIDANCE_VEC_ITER>	GUIDANCE_VEC_PAIR;

protected:
	std::vector<GLGuidance>		m_vecGuidance;

public:
	bool Load(std::vector<std::string>& vecFiles, bool bToolMode);
	bool SetState(const VECGUID_DB& vecGUID);

	GLGuidance* Find ( DWORD dwID );
	bool IsBattle() const;
	bool IsCheckExtraGuild ();
	std::vector<GLGuidance>& GetVecGLGuidance();
};

inline bool operator < (const GLGuidance& lValue, const GLGuidance& rValue)
{
	return (lValue.m_dwID < rValue.m_dwID);
}

#endif // _GL_GUIDANCE_H_