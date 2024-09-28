
#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

#include "../../RanLogic/Land/FactionManager/FactionManagerDefine.h"

class GLCountRecorder
{
public:
	// Ȯ�强�� ���� DWORD �� Key �� ���Ѵ�;
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
	// ���̴� Record;
	MAP_COUNTER				m_mapDropOutCrowCount;			// ���� ���� ����;
	MAP_COUNTER				m_mapDropItemCount;				// ������ ������ ����;
	LONGLONG				m_lDropMoneyCount;				// ������ �� ����;
	LONGLONG				m_lEnterPlayerCount;			// �ʿ� ������ ���� ����;

	// ������ Record;
	DWORD					m_dwCurPlayerCount;				// ���� ���� ��;
	MAP_COUNTER				m_mapCurPartyPlayerCount;		// ���� �� ��Ƽ�� ���� ��;
	MAP_COUNTER				m_mapCurClubPlayerCount;		// ���� �� Ŭ���� ���� ��;
	MAP_COUNTER				m_mapCurCountryPlayerCount;		// ���� �� ������ ���� ��;
	MAP_COUNTER				m_mapCurSchoolPlayerCount;		// ���� �� �п��� ���� ��;

	MAP_FACTION_COUNTER		m_mapCurFactionPlayerCount;		// ���� �� ������ ���� ��;
	
	CTime					m_tRecordStartTime;				// ��� ���� �ð�;
	double					m_dTime;						// ��� ���� �ð�;


public:
	GLCountRecorder ( GLLandMan* const pLandMan );
	~GLCountRecorder ( void );


public:
	// ���̴� Record ���� �ʱ�ȭ�Ѵ�;
	void Clear ( void );

	// ���� ���������� �ʱ�ȭ�Ѵ�;
	void Clear_UserInfo ( void );


public:
	// ���� ��� ���� �ð��� ����Ѵ�;
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
