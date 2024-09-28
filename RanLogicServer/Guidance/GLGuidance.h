#ifndef _GL_GUIDANCE_H_
#define _GL_GUIDANCE_H_

#pragma once

#include <vector>

#include "../../RanLogic/Guidance/GLGuidanceDefine.h"
#include "../../RanLogic/Character/GLCharDefine.h"

// �������� ������ [10/29/2014 hsshin];
// ���� ! : �������� �������� �ű� �������� ȥ��� �� ����.;
// �������� ����ϰ� �ʹٸ� �ű� �������� �ش� �δ���ũ��Ʈ�� �ε带 ���� �ʾƾ��Ѵ�.;
// �ű� �������� �������࿡ ���� �κ��� �δ����� ó�� ��;
// ����Ŭ�� ���� ���� �κ��� �������� ����ϱ� ������ ���������� ������ �������� �ִٸ� ����ε� ������ �����ϱ� ��ƴ�.;

// ������� �ű��δ��� �������� ���� ���� �Ǹ� �ش� �ڵ�� �����ؾ���;
// ����� ����Ŭ���� ���� ����κ��� �������� ����� ����ϰ� �־ �����Ұ�;
// �ش�κ��� ���� ����ų� �ű��� �����ؾ���;

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
	DWORD dwWeekDay; // �� �� ȭ �� �� �� �� 1 2 3 4 5 6 7
	DWORD dwStartTime; // �ð� (0-23����)

	SBATTLE_TIME () :
		dwWeekDay(0),
		dwStartTime(0)
	{
	}
};

struct SAWARD_MOB
{
	SNATIVEID	nidGenMob;	//	������ �Ϸ��� �����Ǵ� ����-��.
	WORD		wGenPosX;	//	' �� ��ġ.
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
        DWORD m_ReqChaDbNum;           //! Ŀ�̼��� ������ ������ ĳ���� DB ��ȣ
        float m_fCommissionTimer;	   //! �� Ŀ�̼� ���� Ÿ�̸�
        float m_fNewCommissionPercent; //! �� Ŀ�̼�
        bool  m_bNewCommission;		   //! �� Ŀ�̼� ���� ����

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
	DWORD				m_dwID;					//! ���� ID.
	std::string			m_strName;				//! ���� �̸�.
	std::vector<DWORD>	m_vecMaps;				//! ���� ���� ��.

	DWORD				m_dwClubMap;			//!	Ŭ�� ��. ( ����� )
	DWORD				m_dwCLubMapGate;		//!	Ŭ�� �ʿ� �ִ� ���Ա�.
	DWORD				m_dwClubHallMap;		//!	Ŭ�� ��. ( �Ա��� ) 

	float				m_fItemDropRate;		//! ������ ��� Ȯ��.
	float				m_fDefaultCommissionPer;//! �⺻ ��������

	DWORD				m_dwGuidanceClub;		//!	���� Ŭ�� ID
	float				m_fCommissionPercent;	//!	��������

	DWORD				m_dwBattleTime;
	BOOL				m_bLog;					//! �α� ��� ����
	SBATTLE_TIME		m_sTIME[MAX_TIME];		//! �����ð�.

	std::vector<SAWARD_MOB> m_vecAwardMob;

    NEW_COMMISSION m_NewCommission; //! �� Ŀ�̼�
	
	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;
	
	bool				m_bBattle;
	DWORD				m_dwBattleOrder;
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;         // ������ ����ð�
    float               m_fNotifyRemainTimer;   // �����ð� �˸��� ���� Ÿ�̸� (���� 10�и���)

	DWORD				m_dwCERTIFY_CHARID;
	DWORD				m_dwCERTIFY_NPCID;
	float				m_fCERTIFY_TIMER;
	D3DXVECTOR3			m_vCERTIFY_POS;

	bool				m_bBattleEndCheck;		// ������ �������� ����� ������ ��������
	float				m_fCHECK_TIMER;
	float				m_fCHECK_TIME;	
	float				m_fLog_TIMER;

	bool				m_bMapEffectUse;		// �� ����Ʈ ȿ�� ��뿩��
	float				m_fHoldCertifyTime;		// ���� ���ӽð� Ÿ��
	int					m_nMapEffectIndex;		// ����ǰ� �ִ� �� ����Ʈ �ε���	

	GUID_KILL_LOG_MAP	m_mapKillLog;
		
	VEC_GUID_MAP_EFFECT	m_vecMapEffect;

public:
    void AddGameDbJob(db::DbAction* pJob);
	bool Load(const std::string& strFile, bool bToolMode);

    //! ���� �̸�
    const std::string& GetName() const { return m_strName; }
    std::string GetName() { return m_strName; }

    //! ���� ID
    DWORD GetId() const { return m_dwID; }

	inline bool IsCertify() const { return m_dwCERTIFY_CHARID!=0; }
    
    inline bool IsNewCommission() const { return m_NewCommission.m_bNewCommission; }
	bool IsGuidClub(DWORD dwClubID) const;

    inline bool IsBattle() const { return m_bBattle; }
	DWORD IsBattleWeekDay ( int nDayOfWeek, int nDay );
	
	// ����Ŭ�� ���۽ð��ΰ�?
	bool IsBattleHour ( DWORD dwORDER, int nHour );

	// ���������� ����Ŭ���������� �Ͼ ���� �����Ѵ�.
	void UpdateBattleDay ( int nDay );
	// ����Ŭ�� ������ ���� �ð��� Ŭ���̾�Ʈ���� �˸���.
	void UpdateNotifyBattle(GLAgentServer* pGLAgentServer, DWORD dwORDER, int nHour, int nMinute );
	void UpdateCommission(GLAgentServer* pGLAgentServer, float fElaps);

	void CheckExtraGuild(GLGaeaServer* pGLGaeaServer, float fElaps);

	void CheckMapEffect(GLGaeaServer* pGLGaeaServer, float fElaps);

	// ����Ŭ�� �������� �����Ѵ�.
	void DoBattleStart (GLAgentServer* pGLAgentServer, DWORD dwORDER, int nDay );
	void DoBattleEnd(GLAgentServer* pGLAgentServer);

	void DoCertifyEnd();

	bool ChangeGuidClub(GLAgentServer* pGLAgentServer, DWORD dwClubID);
	bool ChangeCommission(GLAgentServer* pGLAgentServer, float fRate);

    void AddKillLog( DWORD dwKillChar, DWORD dwDeathChar );		//	ĳ���� Kill/Death �α׸� �����.
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