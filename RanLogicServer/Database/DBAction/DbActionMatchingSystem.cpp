#include "pch.h"

#include "../../../SigmaCore/Database/DbDefine.h"
#include "../../../RanLogic/Msg/ClubMsg.h"
#include "../../Club/ClubLogMan.h"
#include "../../Server/CacheServer.h"
#include "../../Server/AgentServer.h"
#include "./DbActionMatchingSystem.h"
#include "DbActionMatchingSystem.h"
#include "../DbManager.h"

#include "./DbAction.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{


	MatchingOnloadAction::MatchingOnloadAction(std::string Matching_Content_Type,
		std::string Betting_Script,
		std::string Room_Admin_Script,
		std::string Matching_Script,
		std::string Lobby_Field_Manager )
		:   m_Matching_Content_Type(Matching_Content_Type), 
			m_Betting_Script( Betting_Script ),
			m_Room_Admin_Script( Room_Admin_Script ),
			m_Matching_Script( Matching_Script ),
			m_Lobby_Field_Manager( Lobby_Field_Manager )
	{

	}

	MatchingOnloadAction::~MatchingOnloadAction() { }

	int MatchingOnloadAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingOnloadAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingOnloadAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingOnload(m_Matching_Content_Type,m_Betting_Script,m_Room_Admin_Script,m_Matching_Script,m_Lobby_Field_Manager);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingOnloadAction) failed to call MatchingOnloadAction().") );
		}

		return nReturn;
	}

	MatchingGroupAction::MatchingGroupAction(DWORD GroupID,
		DWORD GameScriptID,
		bool  IsDeleted )
		:   m_GroupID(GroupID), 
		m_GameScriptID( GameScriptID ),
		m_IsDeleted( IsDeleted )
	{

	}

	MatchingGroupAction::~MatchingGroupAction() { }

	int MatchingGroupAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingGroupAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingGroupAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingGroup(m_GroupID,m_GameScriptID,m_IsDeleted);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingGroupAction) failed to call MatchingGroupAction().") );
		}

		return nReturn;
	}


	MatchingQueueInAction::MatchingQueueInAction(	DWORD ClientID,
		DWORD GameScriptID,
		bool  IsDeleted  )
		:   m_ClientID(ClientID), 
		m_GameScriptID( GameScriptID ),
		m_IsDeleted( IsDeleted )
	{

	}

	MatchingQueueInAction::~MatchingQueueInAction() { }

	int MatchingQueueInAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingQueueInAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingQueueInAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingQueueIn(m_ClientID,m_GameScriptID,m_IsDeleted);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingQueueInAction) failed to call MatchingQueueInAction().") );
		}

		return nReturn;
	}

	MatchingInstanceJoinAction::MatchingInstanceJoinAction(DWORD GroupID,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID  )
		:   m_GroupID(GroupID), 
		m_MapID(MapID),
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingInstanceJoinAction::~MatchingInstanceJoinAction() { }

	int MatchingInstanceJoinAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceJoinAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingInstanceJoinAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingInstanceJoin(m_GroupID,m_MapID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceJoinAction) failed to call MatchingInstanceJoinAction().") );
		}

		return nReturn;
	}


	MatchingInstanceOnStartAction::MatchingInstanceOnStartAction(
		DWORD GroupReadySize,
		DWORD GameScriptID,
		DWORD  MatchingScriptID  )
		:   m_GroupReadySize(GroupReadySize), 
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingInstanceOnStartAction::~MatchingInstanceOnStartAction() { }

	int MatchingInstanceOnStartAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceOnStartAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingInstanceOnStartAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingInstanceOnStart(m_GroupReadySize,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceOnStartAction) failed to call MatchingInstanceOnStartAction().") );
		}

		return nReturn;
	}

	MatchingInstanceOutAction::MatchingInstanceOutAction(
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID  )
		:   m_GroupID(GroupID), 
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingInstanceOutAction::~MatchingInstanceOutAction() { }

	int MatchingInstanceOutAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceOutAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingInstanceOutAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingInstanceOut(m_GroupID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceOutAction) failed to call MatchingInstanceOutAction().") );
		}

		return nReturn;
	}


	MatchingInstanceResualtAction::MatchingInstanceResualtAction(
		DWORD Faction,
		bool IsLose,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
		:   m_Faction(Faction), 
		m_IsLose(IsLose),
		m_MapID(MapID),
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingInstanceResualtAction::~MatchingInstanceResualtAction() { }

	int MatchingInstanceResualtAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceResualtAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingInstanceResualtAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingInstanceResualt(m_Faction,m_IsLose,m_MapID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceResualtAction) failed to call MatchingInstanceResualtAction().") );
		}

		return nReturn;
	}

	MatchingRoomReqAction::MatchingRoomReqAction(
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
		:  
		m_MapID(MapID),
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingRoomReqAction::~MatchingRoomReqAction() { }

	int MatchingRoomReqAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingRoomReqAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingRoomReqAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingRoomReq(m_MapID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingRoomReqAction) failed to call MatchingRoomReqAction().") );
		}

		return nReturn;
	}

	MatchingFinalWinnerAction::MatchingFinalWinnerAction(
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
		:  
		m_GroupID(GroupID),
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingFinalWinnerAction::~MatchingFinalWinnerAction() { }

	int MatchingFinalWinnerAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingFinalWinnerAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingFinalWinnerAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingFinalWinner(m_GroupID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingFinalWinnerAction) failed to call MatchingFinalWinnerAction().") );
		}

		return nReturn;
	}

	MatchingInstanceTournamentAction::MatchingInstanceTournamentAction(
		int   tournamentPosition,
		const char* GroupName,
		int   GroupIndexPosition,
		int   GroupSize,
		int   GroupMaxSize,
		int   GroupHeadLevel,
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
		:  
		m_tournamentPosition(tournamentPosition),
		m_GroupName( GroupName ),
		m_GroupIndexPosition( GroupIndexPosition ),
		m_GroupSize( GroupSize ),
		m_GroupMaxSize( GroupMaxSize ),
		m_GroupHeadLevel( GroupHeadLevel ),
		m_GroupID(GroupID),
		m_GameScriptID( GameScriptID ),
		m_MatchingScriptID( MatchingScriptID )
	{

	}

	MatchingInstanceTournamentAction::~MatchingInstanceTournamentAction() { }

	int MatchingInstanceTournamentAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceTournamentAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "MatchingInstanceTournamentAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->MatchingInstanceTournament(m_tournamentPosition,m_GroupName,m_GroupIndexPosition,
			m_GroupSize,m_GroupMaxSize,m_GroupHeadLevel,m_GroupID,m_GameScriptID,m_MatchingScriptID);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(MatchingInstanceTournamentAction) failed to call MatchingInstanceTournamentAction().") );
		}

		return nReturn;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	CMatchingJoinCountLoadAction::CMatchingJoinCountLoadAction (
		MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList )
		: m_refMapCharJoinCountList ( _refMapCharJoinCountList ) { }
	CMatchingJoinCountLoadAction::~CMatchingJoinCountLoadAction ( ) { }

	int CMatchingJoinCountLoadAction::Execute ( NetServer* _pServer )
	{
		if ( !_pServer )
			return NET_ERROR;

		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "CMatchingJoinCountLoadAction::Execute. Use ADO" );

			return NET_ERROR;
		}

		int nReturn = m_pDbManager->MatchingJoinCountLoad( m_refMapCharJoinCountList );
		if ( sc::db::DB_ERROR == nReturn )
		{
			sc::writeLogError( std::string(
				"DbAction(CMatchingJoinCountLoadAction) failed to call CMatchingJoinCountLoadAction()." ) );
		}

		return nReturn;
	}

	CMatchingJoinCountResetAction::CMatchingJoinCountResetAction ( ) { }
	CMatchingJoinCountResetAction::~CMatchingJoinCountResetAction ( ) { }

	int CMatchingJoinCountResetAction::Execute ( NetServer* _pServer )
	{
		if ( !_pServer )
			return NET_ERROR;

		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "CMatchingJoinCountResetAction::Execute. Use ADO" );

			return NET_ERROR;
		}

		int nReturn = m_pDbManager->MatchingJoinCountReset( );
		if ( sc::db::DB_ERROR == nReturn )
		{
			sc::writeLogError( std::string(
				"DbAction(CMatchingJoinCountResetAction) failed to call CMatchingJoinCountResetAction()." ) );
		}

		return nReturn;
	}

	CMatchingJoinCountUpdateAction::CMatchingJoinCountUpdateAction (
		DWORD _dwChaDBNum,
		const MatchingSystem::SJOIN_COUNT& _sJoinCount )
		: m_dwChaDBNum ( _dwChaDBNum )
		, m_sJoinCount ( _sJoinCount ) { }
	CMatchingJoinCountUpdateAction::~CMatchingJoinCountUpdateAction ( ) { }

	int CMatchingJoinCountUpdateAction::Execute ( NetServer* _pServer )
	{
		if ( !_pServer )
			return NET_ERROR;

		if ( db::ADO != m_pDbManager->GetConnectionType() )
		{
			sc::writeLogError( "CMatchingJoinCountUpdateAction::Execute. Use ADO" );

			return NET_ERROR;
		}

		int nReturn = m_pDbManager->MatchingJoinCountUpdate( m_dwChaDBNum, m_sJoinCount );
		if ( sc::db::DB_ERROR == nReturn )
		{
			sc::writeLogError( std::string(
				"DbAction(CMatchingJoinCountUpdateAction) failed to call CMatchingJoinCountUpdateAction()." ) );
		}

		return nReturn;
	}

}