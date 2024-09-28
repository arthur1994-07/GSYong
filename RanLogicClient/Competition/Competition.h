#pragma once

#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/PVE/MapTable.h"
#include "../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"

typedef TriggerSystem::SClientTriggerMapInfo TriggerMapInfo;
typedef TriggerSystem::TriggerMapInfoList TriggerMapInfoList;

class Competition
{
public:
	static Competition& GetInstance(void);

public:
	//CDM Rank정보 획득 
	void SetCDMTopList (std::vector<DWORD> vecClubId,
							LONGLONG lnStartTime,
							LONGLONG lnEndTime,
							std::vector<DWORD> vecRank );
	void SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName );
	void SendCDMRankInfo(void);

//Lua 호출 함수들
public:
	// 신규 인던시스템 관리 목록
	LuaTable GetMatchingInstanceList ( const int _nContentsType );
	// CTF
	LuaTable GetExceptionPvPInstanceList(void);
	// PVE 목록
	LuaTable GetPvEInstanceList(void);
	// 해당 인던 참여중인가?
	bool IsJoined( DWORD dwScriptID );
	// 선택한 인던 상태
	DWORD GetSelectStatus();
	// PVP 인던 선택
	void SelectInstanceDungeon ( DWORD _dwMatchingBaseID, DWORD _dwStatus );
	// CDM랭크 정보
	bool ReqCDMRankInfo( DWORD dwScriptID );

	LuaTable GetClientTimeInfo(void);				//전장UI에서 쓰이는 시간 값
	
	//Rank
	LuaTable GetCTFRankInfo(void);					// CTF랭크 정보

	// CTF
	LuaTable GetCTFConsecutivelyCaptureSchool(void);

	// Matching Confirm Join;
	void ConfirmJoin_OK ();
	void ConfirmJoin_Cancel ();

	//PvE 입장 귀속해제	
	void OnEnterance(const unsigned int nIndex);
	void AckPVEEntrance(const bool bShow, DWORD dwMapID );
	void OnEnteranceConfirm( const bool bEnterance );
	void AckPVEEntranceConfirm( const BYTE byResult, const std::string &strChaName );	//인던입장 결과 (Server->Client)
	
	//귀속해제
	void OnClearOfBelonging(const unsigned int nIndex);
	
	//PvP 입장 닫기(신규 인던시스템)
	void OnEnteranceTournament( const DWORD dwSelectID );		//입장
	
	//CTF
	LuaTable	GetCTFRewardFlag(void);					//CTF 보상 플래그
	void				ReqCTFJoin(void);
	void				ReqCancel(void);
	void				ReqExit(void);
	
	//Tournament 
	//Betting
	LuaTable GetBettingGroupInfo( const int nIndex ) const;
	const bool ReqGetBettingInfo( const DWORD dwScriptID ) const;
	const bool ReqSetTournamentBettingMoney( const DWORD dwIndex, const LONGLONG llMoney, const DWORD dwScriptID ) const;
	const bool ReqCancelTournamentBettingMoney( const int nIndex ) const;
	//Observe
	const bool ReqObserveTournament(const DWORD dwScriptID, const char* _nameCharacterTargetObserve);
	//Refer
	const bool ReqReferChar( const int nIndex, const DWORD dwScriptID ) const;

private:
	//PvE
	 //입력되는 맵ID를 가지는 맵을 추출 한다.
	bool FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);
	//현재 리스트박스 에서 선택된 맵 정보를 추출 한다.
	bool GetSelCurInfo(TriggerMapInfo*& info, int& level, unsigned int nIndex = 0);
	bool GetSelCurTable(MAP_TABLE*& pTable, unsigned int nIndex = 0);

private:
	//CDM Rank 정보 저장 변수
	std::vector<DWORD> m_vecClubId;
	LONGLONG m_lnStartTime;
	LONGLONG m_lnEndTime;
	std::vector<DWORD> m_vecRank;
	std::vector<std::string> m_vecMasterName;		//클럽 마스터 이름
	std::vector<std::string> m_vecClubName;		//클럽 명칭

private:
	Competition(void);
	Competition( const Competition& value );
	~Competition(void);
};
