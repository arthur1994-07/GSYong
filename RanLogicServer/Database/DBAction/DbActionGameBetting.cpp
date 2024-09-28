#include "../../pch.h"

#include "../../../SigmaCore/Database/DbDefine.h"
#include "../../../RanLogic/Msg/ClubMsg.h"
#include "../../Club/ClubLogMan.h"
#include "../../Server/CacheServer.h"
#include "./DbActionGameBetting.h"
#include "../DbManager.h"

#include "./DbAction.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{


	BettingSelectAction::BettingSelectAction( DWORD _ChaDbNum , DWORD _BettingStatus )
		:ChaDbNum(_ChaDbNum), BettingStatus( _BettingStatus )
	{

	}

	BettingSelectAction::~BettingSelectAction() { }

	int BettingSelectAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "CBettingSystemAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TournamentBettingSelect( ChaDbNum, BettingStatus);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction) failed to call CBettingSystemAction().") );
		}

		return nReturn;
	}

	BettingInsertAction::BettingInsertAction( DWORD _ChaNum,LONGLONG _BettingCount, DWORD _BettingGameSciptid,DWORD _BettingGroup )
		:ChaNum(_ChaNum), BettingCount( _BettingCount ), BettingGameSciptid(_BettingGameSciptid), BettingGroup(_BettingGroup)
	{

	}

	BettingInsertAction::~BettingInsertAction() { }

	int BettingInsertAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "CBettingSystemAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TournamentBettingInsert( ChaNum,BettingCount,BettingGameSciptid,BettingGroup );
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction) failed to call CBettingSystemAction().") );
		}

		return nReturn;
	}


	BettingUpdateAction::BettingUpdateAction( DWORD _ChaNum,DWORD _BettingGameSciptid,DWORD _BettingStatus )
		:ChaNum(_ChaNum), BettingGameSciptid(_BettingGameSciptid), BettingStatus(_BettingStatus)
	{

	}

	BettingUpdateAction::~BettingUpdateAction() { }

	int BettingUpdateAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "CBettingSystemAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TournamentBettingUpdate( ChaNum,BettingGameSciptid ,BettingStatus);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction) failed to call CBettingSystemAction().") );
		}

		return nReturn;
	}

	BettingDeleteAction::BettingDeleteAction( DWORD _ChaNum )
		:ChaNum(_ChaNum)
	{

	}

	BettingDeleteAction::~BettingDeleteAction() { }

	int BettingDeleteAction::Execute( NetServer* pServer )
	{
		if( !pServer )
			return NET_ERROR;

		CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
		if( !pAgentServer )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction): pAgentServer is NULL.") );
			return sc::db::DB_ERROR;
		}

		if( m_pDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError( "CBettingSystemAction::Execute. Use ADO" );
			return NET_ERROR;
		}

		int nReturn = NET_ERROR;
		nReturn = m_pDbManager->TournamentBettingDelete( ChaNum);
		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( std::string("DbAction(CBettingSystemAction) failed to call CBettingSystemAction().") );
		}

		return nReturn;
	}

}