#pragma once
//
//mjeon.CaptureTheField
//
#include "../../enginelib/G-Logic/TypeDefine.h"
#include "./CaptureTheFieldScriptMan.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLCompetition.h"
#include "../../SigmaCore/gassert.h"
#include "../Database/DbManager.h"

//#include <boost/tr1/unordered_set.hpp>
#include <set>

class GLGaeaServer;
class GLAgentServer;
class GLLandMan;
class GLAGLandMan;
class GLCharAG;
class GLChar;
class GLCrow;
struct PVP_PERFORMANCE;

typedef std::set<DWORD>								SET_CTF_PLAYERS;
typedef std::set<DWORD>::iterator					SET_CTF_PLAYERS_ITER;
typedef std::set<DWORD>::const_iterator				SET_CTF_PLAYERS_CITER;

typedef std::deque<DWORD>							DEQ_CTF_JOINERS;
typedef std::deque<DWORD>::iterator					DEQ_CTF_JOINERS_ITER;
typedef std::deque<DWORD>::const_iterator			DEQ_CTF_JOINERS_CITER;

//typedef std::tr1::unordered_set<DWORD>			USET_CTF_JOINERS;
//typedef std::tr1::unordered_set<DWORD>::iterator	USET_CTF_JOINERS_ITER;


//
//CaptureTheField will be prepared once per a server-group
//
class CaptureTheFieldMan
{
public:
	CaptureTheFieldMan();
	virtual ~CaptureTheFieldMan();
	
	BOOL Initialize();					//Set up CaptureTheFieldMan based on the information loaded by LoadScript().
	virtual void HookAfterInit() = 0;	//Called by Initialize after it's done.

	UINT GetChannel()			{return m_nChannel;}

	DWORD GetPvEMapID()			{return m_dwPVEMapID;}
	DWORD GetPvPMapID()			{return m_dwPVPMapID;}

	DWORD GetCert1ID()			{return m_dwCertificationID1;}
	DWORD GetCert2ID()			{return m_dwCertificationID2;}
	DWORD GetCert3ID()			{return m_dwCertificationID3;}

	DWORD GetStartGate()		{return m_dwGateStart;}
	DWORD GetRevivalGate(WORD wSchool)
	{
		GASSERT(wSchool < SCHOOL_NUM);
		return m_dwGateRevival[wSchool];
	}

    void  AddRewardBuff(SNATIVEID id, WORD wLevel);
    void  GetRewardBuff(UINT idx, OUT CTF_REWARD_BUFF &rewardBuff);
	
	std::vector<CTF_REWARD_BUFF>& GetRewardBuff()
	{
		return m_vecRewardBuffs;
	}

    UINT  GetRewardBuffSize()	{return (UINT)m_vecRewardBuffs.size();}
    void  ClearRewardBuff()     {m_vecRewardBuffs.clear();}


	ENUM_CTF_STATE	GetState()	{return m_state;}

	
	CTime& GetStartTime()		{return m_ctStartTime;}
	CTime& GetEndTime()			{return m_ctEndTime;}		//정해진 StopTime은 최대 시간이고, 그 이전에 종료될 수 있다.
	CTime& GetStopTime()		{return m_ctStopTime;}
	
	UINT GetStartCycleH()		{return m_nStartCycleH;}
    UINT GetStartCycleS()		{return m_nStartCycleS;}

    UINT GetPlayTimeM()			{return m_nPlayTimeM;}
	UINT GetRewardTimeM()		{return m_nRewardTimeM;}

	BOOL IsStarted()			{return m_state == CTF_STATE_START;}
	BOOL IsDone()				{return m_state == CTF_STATE_DONE;}
	BOOL IsRewarding()			{return m_state == CTF_STATE_REWARD;}

	WORD GetAuthedSchool()		{return m_wSchoolAuthed;}

	WORD GetRequiredLevel()		{return m_wRequiredLevel;}

    UINT GetMaxNumOfPlayers()	{return m_nMaxNumOfPlayers;}

	DWORD GetConsecutivelyNumber() {return m_dwConsecutivelyNumber;} 

	bool GetConsecutivelyCapturel() {return m_bConsecutivelyCapturel == TRUE ? true : false;}

	bool GetPrevConsecutively() {return m_bPrevConsecutively == TRUE ? true : false;}
	bool GetConsecutively() {return m_bConsecutively == TRUE ? true : false;}

	BOOL IsConsecutively(WORD my_wSchoolAuthed)	
	{
		if (m_bConsecutivelyCapturel == TRUE || m_bConsecutively == TRUE)
			return TRUE;

		if(m_wPrevSchoolAuthed == my_wSchoolAuthed)		
			return FALSE;

		return TRUE;
	}

	void SetConsecutively(WORD my_wSchoowinner)
	{
		//이전 상태 저장
		m_bPrevConsecutively = m_bConsecutively;
		//밀어내고
		if(m_vecConsecutively.size() >= m_dwConsecutivelyNumber)
		{
			if ( m_vecConsecutively.size() != 0 )
				m_vecConsecutively.pop_front();
		}

		//넣어두고
		m_vecConsecutively.push_back(my_wSchoowinner);
		//계산함

		std::list<WORD>::iterator biter = m_vecConsecutively.begin();
		std::list<WORD>::iterator eiter = m_vecConsecutively.end();

		for(; biter != eiter; ++biter)
		{
			if( *biter != my_wSchoowinner)
			{
				m_bConsecutively = TRUE; 
				return;
			}
		}

		m_bConsecutively = FALSE;
	}

	DWORD GetDamage() { return m_dwDamage; }
	DWORD GetRecovery() { return m_dwRecovery; }

	void SetReadyTime( CTime& ctReadyTime ) { m_ctReadyTime = ctReadyTime; }
	CTime GetReadyTime() { return m_ctReadyTime; }

protected:
	DWORD	m_dwID;	//unique id of the CaptureTheFieldMan.	

	ENUM_CTF_STATE	m_state;
	
	DWORD	m_dwPVEMapID;
	DWORD	m_dwPVPMapID;
	DWORD	m_dwCertificationID1;
	DWORD	m_dwCertificationID2;
	DWORD	m_dwCertificationID3;


	UINT	m_nChannel;						//CTF가 개최될 채널 번호

	//
	// Gate information
	//
	DWORD	m_dwGateStart;

	DWORD	m_dwGateExit1;					//연구단지로 향한 출구 게이트
	DWORD	m_dwGateExit2;					//본동으로 향한 출구 게이트
	
	DWORD	m_dwGateRevival[SCHOOL_NUM];	//각 학원별 부활 게이트

	DWORD	m_dwMapIDKickOut[SCHOOL_NUM];	//CTF 시작시에 신청자를 제외한 캐릭터들을 어디로 recall할 것인가?
	DWORD	m_dwGateKickOut[SCHOOL_NUM];

	//
	// Time setup
	//
	CTime	m_ctStartTime;
	CTime	m_ctStopTime;					//StartTime부터 최대 PlayTimeM 만큼 진행했을 경우 경기를 멈추어야 할 시간
	CTime	m_ctEndTime;					//CTF가 실제 종료한 시간 (ctStopTime은 limit time이고, 경기는 그 전에 종료될 수 있다.)
	CTime	m_ctExtraTime;					//Stop 이 후, 보상 지급 및 결과검토 시간(m_ctsRewardDuration)이 종료되는 시점.
	
	CTimeSpan m_ctsStartCycleH;
	UINT	m_nStartCycleH;					//필드점령 시작 주기: 몇 시간 후에 다음 필드 점령이 열릴 것인가?
	UINT	m_nStartCycleS;					//m_nStartCycleH 값을 초단위로 환산(x3600)한 값. BuffMananger에서 사용됨.

	CTimeSpan m_ctsPlayTimeM;
	UINT	m_nPlayTimeM;

	CTimeSpan m_ctsReadyDurationM;
	UINT	m_nReadyDurationM;				//준비단계 시간 - 몇 분 전에 준비상태로 들어갈것인가?

	CTimeSpan m_ctsRewardTimeM;
	UINT	m_nRewardTimeM;					//보상단계 시간 - 몇 분 동안 보상상태를 유지할 것인가?

	CTime	m_ctReadyTime;					//준비 시작 시간

	UINT	m_nMaxNumOfJoiners;				//각 학원별 최대 신청가능 수
	UINT	m_nMaxNumOfPlayers;				//각 학원별 최대 참여가능 수

	UINT	m_nMaxRebirthTime;				//최대 인원일 경우의 최대 부활 시간 (sec)
	UINT	m_nUpdatePlayersFreq;			//Player의 숫자/목록 갱신 주기 (msec)
	float	m_fUpdatePlayerPositionFreq;	//miniMap에 같은 학원 플레이어들의 위치 갱신 주기

	UINT	m_nContributionPoint4Winner;	//승리팀에 지급될 기여도
	UINT	m_nContributionPoint4Loser;		//패배팀에 지급될 기여도
	UINT	m_nContributionPointMax;		//CTF 한판에서 획득할 수 있는 최대 기여도

	WORD	m_wRequiredLevel;				//CTF참여를 위한 최소 요구 레벨

	WORD	m_wSchoolAuthed;				//승리한 학원
	WORD	m_wPrevSchoolAuthed;			//이전 승리한 학원

    
	DWORD	m_dwConsecutivelyNumber;		// N회부터

	DWORD	m_dwDamage;						// 승리학원생 기여도 지급 조건 피해량
	DWORD	m_dwRecovery;						// 승리학원생 기여도 지급 조건 회복량


    //std::vector<std::pair<DWORD, WORD>> m_vecRewardBuffs;
	std::vector<CTF_REWARD_BUFF>	m_vecRewardBuffs;	


	std::list<WORD>	m_vecConsecutively;	// 연속 점령학원 저장.

	BOOL	m_bConsecutively;			// 속도를 위해 연속 상태 저장.
	BOOL	m_bPrevConsecutively;		// 이전상태 저장.
	BOOL	m_bConsecutivelyCapturel;	// 연속 점령학원 참여가능/불가능
	

	CaptureTheFieldScriptMan *m_pScriptMan;
};


//
//CaptureTheField manager for FieldServer
//
class CaptureTheFieldManFld : public CaptureTheFieldMan
{
public:
	CaptureTheFieldManFld(GLGaeaServer *pGaeaServer);
	virtual ~CaptureTheFieldManFld();

	bool FrameMove(float fElapsedAppTime);


public:
	virtual void HookAfterInit();	//Called by Initialize after it's done.

	void Ready();
	void UpdateReady();
	BOOL Prepare();
	void Start();
	void Done();	
	void Reward(WORD wSchoolAuthed, UINT nHowMany, CTF_REWARD_BUFF *arrRewardBuff);
	void Stop();
    void Quit(DWORD dwChaNum, ENUM_CTF_STATE state);
	void Rebirth(GLChar *pChar);
    void NewPlayerHasCome(DWORD dwChaNum);
	void NewPlayerHasCome(GLChar *pChar);
	void Captured();
    void PlayerNumHasUpdated();
	void UpdatePlayersPosition(float fElapsedAppTime);

	void SetRemainMinutes(UINT nRemainMinutes) {m_nRemainMinutes = nRemainMinutes;}
	void SetNumberOfPlayers(UINT idx, UINT num);
	UINT GetNumberOfPlayers(UINT idx);

	DWORD GetRebirthTimeSec(WORD wSchool);    
	BOOL IsAuthed();
	BOOL IsBattleGround()	{return m_bCTFBattleGround;}

	BOOL UpdateContributionPoint(GLChar *pChar);

	const D3DXVECTOR3& GetCertPos1();
	const D3DXVECTOR3& GetCertPos2();
	const D3DXVECTOR3& GetCertPos3();

	//
	//각 인증기을 인증한 학원번호
	//
	WORD GetSchoolAuthedCert1();
	WORD GetSchoolAuthedCert2();
	WORD GetSchoolAuthedCert3();

	std::set<PVP::CTF_RANKER>& GetRankingSet() {return m_setRanking;}

	
protected:
	GLGaeaServer	*m_pGLGaeaServer;
	GLLandMan		*m_pLandManPvE;
	GLLandMan		*m_pLandManPvP;

	BOOL			m_bCTFBattleGround;	//Is this FieldServer the CTF battle-ground? -> (Channel == 0 && has CTF map)
	
	UINT			m_nRemainMinutes;	//synced by msg from AgentServer.

	UINT			m_arrPlayerNum[SCHOOL_NUM];
	DWORD			m_arrRebirthTimeSec[SCHOOL_NUM];
		
	float			m_fUpdatePlayersPositionTimer;

	GLCrow*			m_pCertification1;
	GLCrow*			m_pCertification2;
	GLCrow*			m_pCertification3;

	std::set<PVP::CTF_RANKER>	m_setRanking;		//Players-ranking based on PvP performance
};


//
//CaptureTheField manager for AgentServer
//
class CaptureTheFieldManAgt : public CaptureTheFieldMan
{
public:
	CaptureTheFieldManAgt(GLAgentServer* pServer);
	virtual ~CaptureTheFieldManAgt();

	bool FrameMove(float fElapsedAppTime);

public:
	ENUM_CTF_STATE RunStateMachine();

	virtual void HookAfterInit();	//Called by Initialize after it's done.

	//
	//returns 0 if joined, else positive-number n that means n-th queued.
	// -1 means that an error occured.
	//
	int  Join(DWORD dwClientID, GLCharAG *pChaAG, BOOL bJoin, WORD wLevel, ENUM_CTF_JOIN_RESULT emReciveJoinResult);
	void Start(UINT nFieldSvr);
	//void Sync(DWORD dwClientID);	//Sync current CTF-state to the client.

	BOOL RemoveJoiner(DWORD dwChaNum, WORD wSchool);
	
	void Quit(DWORD dwChaNum, WORD wSchool);			//called by destructor of GLCharAG when a player disconnected.
	void RemovePlayer(DWORD dwChaNum, WORD wSchool);	//called by Quit button on the CTF-UI.

	void KickOut(GLCharAG *pChaAG);
	BOOL KickOutNonPlayers();
	void RecallPlayers();
	BOOL RecallOnePlayer(GLCharAG *pChaAG);
	BOOL Recall4Winner(GLCharAG *pChaAG);
	void NotifyStart(GLCharAG *pChaAG);
	void QuitParty(GLCharAG *pChaAG);

	void CTFNoneUserKickOut();

	void AuthedBy(WORD wSchool);

	void SetRankers(UINT nHowMany, PVP::CTF_RANKER_BASIC *arrRanking);

	std::vector<PVP::CTF_RANKER_BASIC>& GetRankingVector() {return m_history.vecRanking;}
	PVP::CTF_HISTORY& GetHistory()	{return m_history;}

	bool LoadFromDB(db::IDbManager* pDBMan, db::IDbManager* pAdoMan);

	//
	// Change CTF state based on GM command.
	//
	ENUM_CTF_STATE MoveStateByGM();
	
	bool IsRegisterMember(DWORD dwCharDbNum);

	void ForceOut( GLCharAG *pChaAG );

protected:
	void KickForce(GLCharAG *pChaAG);
public:
	//
	//called by RunStateMachine()
	//
	virtual BOOL DoReady();
	virtual void DoUpdateReady();
	virtual BOOL DoStart();
	virtual void DoUpdatePlayers();
	virtual void DoDone();	
    virtual void DoReward();
	virtual void DoStop();

public:
	GLAGLandMan* GetPvPLandMan()	{return m_pLandManPvP;}
	GLAGLandMan* GetPvELandMan()	{return m_pLandManPvE;}

	inline const UINT GetFieldNum() const	{ return m_nFieldSvr; }

protected:
	GLAgentServer	*m_pGLAgentServer;
	GLAGLandMan		*m_pLandManPvE;
	GLAGLandMan		*m_pLandManPvP;

	//
	// members for calculating of elapsed time.
	//
	DWORD			m_dwBaseTime;
	DWORD			m_dwCurrentTime;
	DWORD			m_dwPrevElapsedMinutes;

	DWORD			m_dwKickCheckTime;

	UINT			m_nFieldSvr;						//FieldServer number(index) which plays CTF.
	
	SET_CTF_PLAYERS		m_setCTFPlayers[SCHOOL_NUM];	//Players: who are playing CTF
	DEQ_CTF_JOINERS		m_deqCTFJoiners[SCHOOL_NUM];	//Joiners: who registered to play CTF

	PVP::CTF_HISTORY		m_history;
};
