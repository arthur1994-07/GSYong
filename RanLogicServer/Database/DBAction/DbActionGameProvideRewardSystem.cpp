#include "pch.h"

#include "./DbActionGameProvideRewardSystem.h"

#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"

#include "../../ProvideRewardSystem/GLProvideRewardManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



/// Initialize Action;
CProvideRewardInitializeAction::CProvideRewardInitializeAction ( )
{

}

CProvideRewardInitializeAction::~CProvideRewardInitializeAction ( ) { }

int CProvideRewardInitializeAction::Execute ( NetServer* pServer )
{
	if ( NULL == pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardInitializeAction): pServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CProvideRewardInitializeAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ProvideRewardInitialize();
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardInitializeAction) failed to call CProvideRewardInitializeAction()." ) );
	}

	return nReturn;
}





/// Insert Action;
CProvideRewardInsertAction::CProvideRewardInsertAction (
	const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )
	: m_ProvideRewardData ( _ProvideRewardData )
{

}

CProvideRewardInsertAction::~CProvideRewardInsertAction ( ) { }

int CProvideRewardInsertAction::Execute ( NetServer* pServer )
{
	if ( NULL == pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardInsertAction): pServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CProvideRewardInsertAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ProvideRewardInsert( m_ProvideRewardData );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardInsertAction) failed to call CProvideRewardInsertAction()." ) );
	}

	return nReturn;
}




/// Select Action;
CProvideRewardSelectAction::CProvideRewardSelectAction (
	DWORD _dwChaDBNum )
	: m_dwChaDBNum ( _dwChaDBNum )
{

}

CProvideRewardSelectAction::~CProvideRewardSelectAction ( ) { }

int CProvideRewardSelectAction::Execute ( NetServer* pServer )
{
	if ( NULL == pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardSelectAction): pServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CProvideRewardSelectAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	GLGaeaServer* pGaeaServer = pFieldServer->GetGLGaeaServer();
	if ( NULL == pGaeaServer )
		return sc::db::DB_ERROR;

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ProvideRewardSelect( m_dwChaDBNum, pGaeaServer->GetServerGroup(), m_vecProvideRewardData );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardSelectAction) failed to call CProvideRewardSelectAction()." ) );
	}

	// Ã³¸®;
	ProvideRewardSystem::GLProvideRewardManager* pProvideRewardMan =
		ProvideRewardSystem::GLProvideRewardManager::Instance();
	if ( pProvideRewardMan )
	{
		for ( ProvideRewardSystem::VEC_PROVIDE_REWARD_ITER iter = m_vecProvideRewardData.begin();
			iter != m_vecProvideRewardData.end(); ++iter )
		{
			ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData = *iter;
			pProvideRewardMan->OnRegisterProvideReward( provideRewardData );
		}
	}

	return nReturn;
}




/// Delete Flag Action;
CProvideRewardClearDeleteFlagAction::CProvideRewardClearDeleteFlagAction ( )
{

}

CProvideRewardClearDeleteFlagAction::~CProvideRewardClearDeleteFlagAction ( ) { }

int CProvideRewardClearDeleteFlagAction::Execute ( NetServer* pServer )
{
	if ( NULL == pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardClearDeleteFlagAction): pServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CProvideRewardClearDeleteFlagAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ProvideRewardClearDeleteFlag();
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardClearDeleteFlagAction) failed to call CProvideRewardClearDeleteFlagAction()." ) );
	}

	return nReturn;
}




/// Complete Action
CProvideRewardCompleteAction::CProvideRewardCompleteAction (
	DWORD _dwUniqueID )
	: m_dwUniqueID ( _dwUniqueID )
{

}

CProvideRewardCompleteAction::~CProvideRewardCompleteAction ( ) { }

int CProvideRewardCompleteAction::Execute ( NetServer* pServer )
{
	if ( NULL == pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast< CFieldServer* >( pServer );
	if ( NULL == pFieldServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardCompleteAction): pServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CProvideRewardCompleteAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ProvideRewardComplete( m_dwUniqueID );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CProvideRewardCompleteAction) failed to call CProvideRewardCompleteAction()." ) );
	}

	return nReturn;
}