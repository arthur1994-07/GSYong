
#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../../RanLogic/Land/FactionManager/FactionManagerDefine.h"

class GLCountRecorder
{
public:
	// 확장성을 위해 DWORD 를 Key 로 정한다;
	typedef std::map< DWORD, LONGLONG >					MAP_COUNTER;
	typedef	MAP_COUNTER::iterator						MAP_COUNTER_ITR;
	typedef	MAP_COUNTER::const_iterator					MAP_COUNTER_CITR;
	typedef MAP_COUNTER::value_type						MAP_COUNTER_VALUE;

	typedef std::map< Faction::FactionID, LONGLONG >	MAP_FACTION_COUNTER;
	typedef	MAP_FACTION_COUNTER::iterator				MAP_FACTION_COUNTER_ITR;
	typedef	MAP_FACTION_COUNTER::const_iterator			MAP_FACTION_COUNTER_CITR;
	typedef MAP_FACTION_COUNTER::value_type				MAP_FACTION_COUNTER_VALUE;


private:
	GLLandMan* const	m_pLandMan;


private:
	// 쌓이는 Record;
	MAP_COUNTER				m_mapDropOutCrowCount;			// 잡은 몬스터 집계;
	MAP_COUNTER				m_mapDropItemCount;				// 떨어진 아이템 집계;
	LONGLONG				m_lDropMoneyCount;				// 떨어진 돈 집계;
	LONGLONG				m_lEnterPlayerCount;			// 맵에 입장한 유저 집계;

	// 현재의 Record;
	DWORD					m_dwCurPlayerCount;				// 현재 유저 수;
	MAP_COUNTER				m_mapCurPartyPlayerCount;		// 현재 각 파티별 유저 수;
	MAP_COUNTER				m_mapCurClubPlayerCount;		// 현재 각 클럽별 유저 수;
	MAP_COUNTER				m_mapCurCountryPlayerCount;		// 현재 각 국가별 유저 수;
	MAP_COUNTER				m_mapCurSchoolPlayerCount;		// 현재 각 학원별 유저 수;

	MAP_FACTION_COUNTER		m_mapCurFactionPlayerCount;		// 현재 각 진영별 유저 수;
	
	CTime					m_tRecordStartTime;				// 기록 시작 시간;
	double					m_dTime;						// 기록 진행 시간;


public:
	GLCountRecorder ( GLLandMan* const pLandMan );
	~GLCountRecorder ( void );


public:
	// 쌓이는 Record 만을 초기화한다;
	void Clear ( void );

	// 현재 유저정보를 초기화한다;
	void Clear_UserInfo ( void );


public:
	// 현재 기록 진행 시간을 계산한다;
	void FrameMove ( const float fElapsedTime );
	

public:
	inline const MAP_COUNTER& GetDropOutCrowMap ( void ) const					{ return m_mapDropOutCrowCount; }
	inline const MAP_COUNTER& GetDropItemMap ( void ) const						{ return m_mapDropItemCount; }
	inline const LONGLONG& GetDropMoneyCount ( void ) const						{ return m_lDropMoneyCount; }
	inline const LONGLONG& GetEnterPlayerCount ( void ) const					{ return m_lEnterPlayerCount; }

	inline const DWORD GetCurPlayerCount ( void ) const							{ return m_dwCurPlayerCount; }
	inline const MAP_COUNTER& GetCurPartyPlayerCount ( void ) const				{ return m_mapCurPartyPlayerCount; }
	inline const MAP_COUNTER& GetCurClubPlayerCount ( void ) const				{ return m_mapCurClubPlayerCount; }
	inline const MAP_COUNTER& GetCurCountryPlayerCount ( void ) const			{ return m_mapCurCountryPlayerCount; }
	inline const MAP_COUNTER& GetCurSchoolPlayerCount ( void ) const			{ return m_mapCurSchoolPlayerCount; }

	inline const MAP_FACTION_COUNTER& GetCurFactionPlayerCount ( void ) const	{ return m_mapCurFactionPlayerCount; }

	inline const double& GetElapsedTime ( void ) const							{ return m_dTime; }


public:
	void add_DropOutCrowCount ( const DWORD dwID );
	void add_DropItemCount ( const DWORD dwID );
	void add_DropMoneyCount ( const LONGLONG& lMoney );
	void add_EnterPlayerCount ( void );

	void add_CurPlayerCount ( void );
	void add_CurPartyPlayerCount ( const DWORD dwID );
	void add_CurClubPlayerCount ( const DWORD dwID );
	void add_CurCountryPlayerCount ( const DWORD dwID );
	void add_CurSchoolPlayerCount ( const DWORD dwID );

	void del_CurPlayerCount ( void );
	void del_CurPartyPlayerCount ( const DWORD dwID );
	void del_CurClubPlayerCount ( const DWORD dwID );
	void del_CurCountryPlayerCount ( const DWORD dwID );
	void del_CurSchoolPlayerCount ( const DWORD dwID );

	void add_CurFactionPlayerCount ( const Faction::FactionID& sFactionID );
	void del_CurFactionPlayerCount ( const Faction::FactionID& sFactionID );
};
