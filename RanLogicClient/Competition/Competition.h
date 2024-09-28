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
	//CDM Rank���� ȹ�� 
	void SetCDMTopList (std::vector<DWORD> vecClubId,
							LONGLONG lnStartTime,
							LONGLONG lnEndTime,
							std::vector<DWORD> vecRank );
	void SetClubMasterClubName( std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName );
	void SendCDMRankInfo(void);

//Lua ȣ�� �Լ���
public:
	// �ű� �δ��ý��� ���� ���
	LuaTable GetMatchingInstanceList ( const int _nContentsType );
	// CTF
	LuaTable GetExceptionPvPInstanceList(void);
	// PVE ���
	LuaTable GetPvEInstanceList(void);
	// �ش� �δ� �������ΰ�?
	bool IsJoined( DWORD dwScriptID );
	// ������ �δ� ����
	DWORD GetSelectStatus();
	// PVP �δ� ����
	void SelectInstanceDungeon ( DWORD _dwMatchingBaseID, DWORD _dwStatus );
	// CDM��ũ ����
	bool ReqCDMRankInfo( DWORD dwScriptID );

	LuaTable GetClientTimeInfo(void);				//����UI���� ���̴� �ð� ��
	
	//Rank
	LuaTable GetCTFRankInfo(void);					// CTF��ũ ����

	// CTF
	LuaTable GetCTFConsecutivelyCaptureSchool(void);

	// Matching Confirm Join;
	void ConfirmJoin_OK ();
	void ConfirmJoin_Cancel ();

	//PvE ���� �ͼ�����	
	void OnEnterance(const unsigned int nIndex);
	void AckPVEEntrance(const bool bShow, DWORD dwMapID );
	void OnEnteranceConfirm( const bool bEnterance );
	void AckPVEEntranceConfirm( const BYTE byResult, const std::string &strChaName );	//�δ����� ��� (Server->Client)
	
	//�ͼ�����
	void OnClearOfBelonging(const unsigned int nIndex);
	
	//PvP ���� �ݱ�(�ű� �δ��ý���)
	void OnEnteranceTournament( const DWORD dwSelectID );		//����
	
	//CTF
	LuaTable	GetCTFRewardFlag(void);					//CTF ���� �÷���
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
	 //�ԷµǴ� ��ID�� ������ ���� ���� �Ѵ�.
	bool FindTable(const SNATIVEID mid, const int nLevel, MAP_TABLE*& pTable);
	//���� ����Ʈ�ڽ� ���� ���õ� �� ������ ���� �Ѵ�.
	bool GetSelCurInfo(TriggerMapInfo*& info, int& level, unsigned int nIndex = 0);
	bool GetSelCurTable(MAP_TABLE*& pTable, unsigned int nIndex = 0);

private:
	//CDM Rank ���� ���� ����
	std::vector<DWORD> m_vecClubId;
	LONGLONG m_lnStartTime;
	LONGLONG m_lnEndTime;
	std::vector<DWORD> m_vecRank;
	std::vector<std::string> m_vecMasterName;		//Ŭ�� ������ �̸�
	std::vector<std::string> m_vecClubName;		//Ŭ�� ��Ī

private:
	Competition(void);
	Competition( const Competition& value );
	~Competition(void);
};
