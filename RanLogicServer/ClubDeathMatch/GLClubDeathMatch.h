#ifndef _GL_CLUB_DEATH_MATCH_H_
#define _GL_CLUB_DEATH_MATCH_H_

#pragma once

#include "../../RanLogic/GLCDMDefine.h"
#include "../Database/DBAction/DbAction.h"

#define RANKING_NUM		9
#define CLUB_NAME		10

namespace db {
    class DbAction;
}

class GLAgentServer;
class GLGaeaServer;

class GLClubDeathMatch
{
public:
	enum 
	{		
		MAX_TIME = 4,
	};

	enum EM_BATTLE_STATE 
	{
		BATTLE_NOSTART		= 0,	//	�������� ����
		BATTLE_READY		= 1,	//	��Ʋ �����
		BATTLE_START		= 2,	//	��Ʋ ��
		BATTLE_END_AWARD	= 3,	//	��Ʋ ������ ���� �ð� ( ��ǰ ���� )
		BATTLE_END			= 4,	//	��Ʋ ������ ƨ��� �ð�
	};

public:
	DWORD				m_dwID;					// ���� ID.
	std::string			m_strName;				// ���� �̸�.

	DWORD				m_dwClubMap;			//	Ŭ�� ��. ( ����� )
	DWORD				m_dwCLubMapGate;		//	Ŭ�� �ʿ� �ִ� ���Ա�.
	DWORD				m_dwClubHallMap;		//	Ŭ�� ��. ( �Ա��� ) 
	DWORD				m_dwClubHallGate;		//	Ŭ�� �� �ⱸ  ( �Ա��� ) 

	DWORD				m_dwBattleTime;
	BOOL				m_bLog;					//	�α� ��� ����
	SCDM_TIME			m_sCdmTIME[MAX_TIME];		//	�����ð�.

	SCDM_AWARD_ITEM		m_sCdmAwardItem;		
	
	CDM_AWARD_CHAR		m_vecCdmAwardChar;		//	������ ���� ĳ���� ( �ι��޴°� �����ϱ� ���� )
	CDM_RANK_INFO_MAP	m_mapCdmScore;			//	cdm ���ھ�

	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;

	EM_BATTLE_STATE		m_emBattleState;
	DWORD				m_dwBattleOrder;		//	�̹��ֿ� ���°����...
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;
    float               m_fNotifyRemainTimer;   //  �����ð� �˸��� ���� Ÿ�̸� (���� 10�и���)

	float				m_fCHECK_TIMER;			//	��Ʋ ������ Ÿ�̸ӵ� ( ��ǰ�̶� ���� �ñ涧 ��� ) 
	float				m_fCHECK_TIME;			//	��Ʋ ������ Ÿ�̸ӵ� ( ��ǰ�̶� ���� �ñ涧 ��� ) 
	float				m_fRankingUpdate;		//	��ŷ Ÿ�̸�

	bool				m_bFieldUsed;			//	�ʵ弭�� ��뿩��( ���� �ʵ忡�� ���������� Ȯ�� )
	
	float				m_fReadyTimer;			//  �غ���°� ���� �ش� Ÿ�̸� ��ŭ �ð��� ������ ���ۻ��·� �ٲ��
	bool				m_bGMReady;				//	GM��ɾ� ready

	float				m_fLog_TIMER;
	float				m_fMemberCheck;			// ���������ο� �ʰ��˻� �ֱ�;
	float				m_fMemberCheckTimer;    // ���������ο� �ʰ��˻� Ÿ�̸�;
	
	CDM_KILL_LOG_MAP		m_mapKillLog;	
	CDM_CHAR_LOG_MAP		m_mapCharLog;
	CDM_ACCRUE_COUNT_MAP	m_mapAccrueLog;

public:
	GLClubDeathMatch ();

	GLClubDeathMatch ( const GLClubDeathMatch &value )
	{
		operator= ( value );
	}

	GLClubDeathMatch& operator= ( const GLClubDeathMatch& value );

	bool operator < ( const GLClubDeathMatch& sClubDM )
	{
		return m_dwID < sClubDM.m_dwID;
	}

public:
	bool Load(const std::string& strFile, bool bToolMode);

public:
	//void AddCDMScore( DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar );
	SCDM_RANK_INFO* GetCDMRankInof( DWORD dwClubID );

	DWORD GetAccrueCount( DWORD dwCharID );

public:
	bool IsBattle() const { return ( m_emBattleState == BATTLE_START ); }
	bool IsBattleReady() const { return ( m_emBattleState == BATTLE_READY ); }
	bool IsBattleEndAward() const { return ( m_emBattleState == BATTLE_END_AWARD); }
	bool IsBattleEnd() const { return ( m_emBattleState == BATTLE_END ); }
	bool IsEnterMap( DWORD dwClubID );

	bool IsFieldUsed()	{ return m_bFieldUsed; }

	DWORD IsBattleWeekDay ( int nDayOfWeek, int nDay );

	void SetBattle( EM_BATTLE_STATE emBattleState )	{ m_emBattleState = emBattleState; }
	
	// ���� �ð��ΰ�.
	bool IsBattleHour ( DWORD dwORDER, int nHour );

public:
	// ���������� ������ ���� ����
	void UpdateBattleDay ( int nDay );
	// ���� �ð��� client���� �˸���.
	void UpdateNotifyBattle(GLAgentServer* pServer, DWORD dwORDER, int nHour, int nMinute);

	//void CheckAwardClub( float fElaps );	//	���� ����
	//void CheckExtraClub( float fElaps );	//	���� �������� 

	//void UpdateCDMRanking();
	//void CDMAwardItem();

public:
	// CDM�� �����Ѵ�.
	void DoBattleStart(GLAgentServer* pServer, DWORD dwORDER, int nDay);
	void DoBattleEnd(GLAgentServer* pServer);
};

class GLClubDeathMatchMan
{
public:
	typedef std::vector<GLClubDeathMatch>			CDM_VEC;
	typedef CDM_VEC::iterator						CDM_VEC_ITER;
	typedef std::pair<CDM_VEC_ITER,CDM_VEC_ITER>	CDM_VEC_PAIR;

protected:
	CDM_VEC m_vecClubDM;

public:
	bool Load(const std::vector<std::string>& vecFiles, bool bToolMode);

public:
	GLClubDeathMatch* Find ( DWORD dwID );

	bool IsBattle() const;
    bool IsBattleReady() const;
	bool IsBattle(DWORD dwID);
	bool IsBattleEnd(DWORD dwID);

public:
    GLClubDeathMatchMan() {}
    virtual ~GLClubDeathMatchMan() {}
};

class GLClubDeathMatchAgentMan : public GLClubDeathMatchMan
{
public:
    GLClubDeathMatchAgentMan(GLAgentServer* pServer);
    virtual ~GLClubDeathMatchAgentMan();

protected:
    GLAgentServer* m_pServer;

public:
	bool SetMapState();
	const bool IsClubDeathMatchMap(const SNATIVEID& _baseMapID);
	bool FrameMove(float fElapsedAppTime);
    void SendClubDeathMatch_RemainTime(DWORD ClientSlot);
	CDM_VEC& GetVecClubDeathMatch();

//protected:
    bool MakeClubDeathMatch_RemainTime_Packet( msgpack::sbuffer& PackBuffer);   //������ ������� ���� �ð��� ���� ��Ŷ�� ����
    void SendClubDeathMatch_RemainTime_BRD();
};

class GLClubDeathMatchFieldMan : public GLClubDeathMatchMan
{
public:
    GLClubDeathMatchFieldMan(GLGaeaServer* pGaeaServer);
    virtual ~GLClubDeathMatchFieldMan() {}
    //static GLClubDeathMatchFieldMan& GetInstance();

private:
    GLGaeaServer* m_pGLGaeaServer;

public:
    bool FrameMove(float fElaps);
	bool SetMapState ();
	bool ReadyBattle( DWORD dwID );
	bool BeginBattle ( DWORD dwID );
	bool EndBattle ( DWORD dwID );
	const std::string GetName ( DWORD dwID );
	void DoLogPrint( DWORD dwID );
	void DoResultLog( DWORD dwID );
	int GetAccrueCount( DWORD dwID, DWORD dwCharID );
    void AddGameDbJob(db::DbActionPtr spJob);
    void CheckExtraClub(GLClubDeathMatch& sClubDM, float fElaps);
    void AddCDMScore(GLClubDeathMatch* pCDM, DWORD dwKillClub, DWORD dwDeathClub, DWORD dwKillChar, DWORD dwDeathChar);
    void UpdateCDMRanking(GLClubDeathMatch* pCDM);
    void CheckAwardClub(GLClubDeathMatch* pCDM, float fElaps);
    void CDMAwardItem(GLClubDeathMatch* pCDM);
	void CheckCDMMemberLimited(GLClubDeathMatch* pCDM);
};

inline bool operator < ( const GLClubDeathMatch& a, const GLClubDeathMatch& b )
{
	return (a.m_dwID < b.m_dwID);
}

#endif // _GL_CLUB_DEATH_MATCH_H_