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
		BATTLE_NOSTART		= 0,	//	시작하지 않음
		BATTLE_READY		= 1,	//	배틀 대기중
		BATTLE_START		= 2,	//	배틀 중
		BATTLE_END_AWARD	= 3,	//	배틀 종료후 정리 시간 ( 상품 지급 )
		BATTLE_END			= 4,	//	배틀 종료후 튕기는 시간
	};

public:
	DWORD				m_dwID;					// 관리 ID.
	std::string			m_strName;				// 관리 이름.

	DWORD				m_dwClubMap;			//	클럽 맵. ( 전산실 )
	DWORD				m_dwCLubMapGate;		//	클럽 맵에 있는 출입구.
	DWORD				m_dwClubHallMap;		//	클럽 맵. ( 입구맵 ) 
	DWORD				m_dwClubHallGate;		//	클럽 맵 출구  ( 입구맵 ) 

	DWORD				m_dwBattleTime;
	BOOL				m_bLog;					//	로그 출력 여부
	SCDM_TIME			m_sCdmTIME[MAX_TIME];		//	전투시간.

	SCDM_AWARD_ITEM		m_sCdmAwardItem;		
	
	CDM_AWARD_CHAR		m_vecCdmAwardChar;		//	보상을 받은 캐릭터 ( 두번받는걸 방지하기 위해 )
	CDM_RANK_INFO_MAP	m_mapCdmScore;			//	cdm 스코어

	bool				m_bNotifyOneHour;
	bool				m_bNotifyHalfHour;
	bool				m_bNotify10MinHour;

	EM_BATTLE_STATE		m_emBattleState;
	DWORD				m_dwBattleOrder;		//	이번주에 몇번째인지...
	DWORD				m_dwLastBattleDay;
	float				m_fBattleTimer;
    float               m_fNotifyRemainTimer;   //  남은시간 알림을 위한 타이머 (현재 10분마다)

	float				m_fCHECK_TIMER;			//	배틀 종료후 타이머들 ( 상품이랑 유저 팅길때 사용 ) 
	float				m_fCHECK_TIME;			//	배틀 종료후 타이머들 ( 상품이랑 유저 팅길때 사용 ) 
	float				m_fRankingUpdate;		//	랭킹 타이머

	bool				m_bFieldUsed;			//	필드서버 사용여부( 현재 필드에서 진행중인지 확인 )
	
	float				m_fReadyTimer;			//  준비상태가 된후 해당 타이머 만큼 시간이 지나면 시작상태로 바뀐다
	bool				m_bGMReady;				//	GM명령어 ready

	float				m_fLog_TIMER;
	float				m_fMemberCheck;			// 입장제한인원 초과검사 주기;
	float				m_fMemberCheckTimer;    // 입장제한인원 초과검사 타이머;
	
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
	
	// 시작 시간인가.
	bool IsBattleHour ( DWORD dwORDER, int nHour );

public:
	// 최종적으로 진행한 일자 지정
	void UpdateBattleDay ( int nDay );
	// 남은 시간을 client에게 알린다.
	void UpdateNotifyBattle(GLAgentServer* pServer, DWORD dwORDER, int nHour, int nMinute);

	//void CheckAwardClub( float fElaps );	//	보상 지급
	//void CheckExtraClub( float fElaps );	//	유저 내보내기 

	//void UpdateCDMRanking();
	//void CDMAwardItem();

public:
	// CDM을 시작한다.
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
    bool MakeClubDeathMatch_RemainTime_Packet( msgpack::sbuffer& PackBuffer);   //선도전 종료까지 남은 시간에 대한 패킷을 만듬
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