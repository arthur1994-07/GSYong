#include "pch.h"

#include "./DbActionGameCountry.h"

#include "../../Server/AgentServer.h"
#include "../../Server/s_CFieldServer.h"
#include "../../FieldServer/GLGaeaServer.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "../../Country/GLVictoriousCountryManServer.h"
#include "../../Country/GLVictoriousCountryManField.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// Update User Country Action;
CUpdateUserCountryAction::CUpdateUserCountryAction ( DWORD dwUserID
												   , Country::SCOUNTRY_INFO sCountryInfo )
												   : m_dwUserID ( dwUserID )
												   , m_sCountryInfo ( sCountryInfo )
{

}

CUpdateUserCountryAction::~CUpdateUserCountryAction() { }

int CUpdateUserCountryAction::Execute( NetServer* pServer )
{
	if ( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if ( !pAgentServer )
	{
		sc::writeLogError(
			std::string("DbAction(CUpdateUserCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CUpdateUserCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->UpdateUserCountry( m_dwUserID, m_sCountryInfo );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CUpdateUserCountryAction) failed to call CUpdateUserCountryAction().") );
	}

	sc::writeLogInfo( "[ Country Log ] [ Change User Country ( End DB ) ]" );

	// 변경에 성공했으면 Agent 서버 메시지에 넣어준다;
	// 서버와 클라이언트의 Country 정보를 갱신한다;
	GLMSG::NET_COUNTRY_CHANGE_DA NetMsg( m_sCountryInfo, m_dwUserID );
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	return nReturn;
}


CLoadAgentVictoriousCountryAction::CLoadAgentVictoriousCountryAction ()
{

}

CLoadAgentVictoriousCountryAction::~CLoadAgentVictoriousCountryAction () { }

int CLoadAgentVictoriousCountryAction::Execute ( NetServer* pServer )
{
	if ( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if ( !pAgentServer )
	{
		sc::writeLogError(
			std::string("DbAction(CLoadAgentVictoriousCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLoadAgentVictoriousCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LoadVictoriousCountry( m_vecVCountry );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CLoadAgentVictoriousCountryAction) failed to call CLoadAgentVictoriousCountryAction().") );
	}

	sc::writeLogInfo ( "[ Country Log ] [ Load Agent Victorious Country ]" );

	Country::GLVictoriousCountryManServer* pVMan =
		Country::GLVictoriousCountryManServer::Instance();

	if ( pVMan )
	{
		for ( Country::VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			Country::SVICTORIOUS_COUNTRY sVCountry = *iter;
			pVMan->AddVCountry( sVCountry );
		}

		pVMan->CompleteLoading();
	}

	return nReturn;
}


CLoadFieldVictoriousCountryAction::CLoadFieldVictoriousCountryAction ()
{

}

CLoadFieldVictoriousCountryAction::~CLoadFieldVictoriousCountryAction () { }

int CLoadFieldVictoriousCountryAction::Execute ( NetServer* pServer )
{
	if( !pServer )
		return NET_ERROR;

	CFieldServer* pFieldServer = reinterpret_cast<CFieldServer*>( pServer );
	if ( !pFieldServer )
	{
		sc::writeLogError(
			std::string("DbAction(CLoadFieldVictoriousCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CLoadFieldVictoriousCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->LoadVictoriousCountry( m_vecVCountry );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CLoadFieldVictoriousCountryAction) failed to call CLoadFieldVictoriousCountryAction().") );
	}

	sc::writeLogInfo ( "[ Country Log ] [ Load Field Victorious Country ]" );

	Country::GLVictoriousCountryManField* pVMan =
		Country::GLVictoriousCountryManField::Instance();

	if ( pVMan )
	{
		for ( Country::VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			Country::SVICTORIOUS_COUNTRY sVCountry = *iter;
			pVMan->AddVCountry( sVCountry );
		}

		pVMan->CompleteLoading();
	}

	return nReturn;
}


CAddVictoriousCountryAction::CAddVictoriousCountryAction ( 
	Country::SVICTORIOUS_COUNTRY sVCountry )
	: m_sVCountry ( sVCountry )
{

}

CAddVictoriousCountryAction::~CAddVictoriousCountryAction () { }

// 현재 Set 기능으로 동작한다;
int CAddVictoriousCountryAction::Execute ( NetServer* pServer )
{
	if ( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if ( !pAgentServer )
	{
		sc::writeLogError(
			std::string("DbAction(CAddVictoriousCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CAddVictoriousCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ResetVictoriousCountry( m_sVCountry.emFlag );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CAddVictoriousCountryAction) failed to call CAddVictoriousCountryAction().") );
	}

	nReturn = m_pDbManager->AddVictoriousCountry( m_sVCountry );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CAddVictoriousCountryAction) failed to call CAddVictoriousCountryAction().") );
	}

	sc::writeLogInfo( "[ Country Log ] [ Set Victorious Country ]" );

	// 변경에 성공했으면 Agent 서버 메시지에 넣어준다;
	GLMSG::NET_VCOUNTRY_ADD_DAF NetMsg( m_sVCountry );
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	return nReturn;
}


CResetVictoriousCountryAction::CResetVictoriousCountryAction (
	Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
	: m_emFlag ( emFlag )
{

}

CResetVictoriousCountryAction::~CResetVictoriousCountryAction () { }

int CResetVictoriousCountryAction::Execute ( NetServer* pServer )
{
	if ( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if ( !pAgentServer )
	{
		sc::writeLogError(
			std::string("DbAction(CResetVictoriousCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CResetVictoriousCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ResetVictoriousCountry( m_emFlag );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string("DbAction(CResetVictoriousCountryAction) failed to call CResetVictoriousCountryAction().") );
	}

	sc::writeLogInfo( "[ Country Log ] [ Reset Victorious Country ]" );

	// 변경에 성공했으면 Agent 서버 메시지에 넣어준다;
	GLMSG::NET_VCOUNTRY_RESET_DAF NetMsg( m_emFlag );
	pAgentServer->InsertMsg( DBACTION_CLIENT_ID, &NetMsg );

	return nReturn;
}


CConfirmStateVictoriousCountryAction::CConfirmStateVictoriousCountryAction (
	Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
	: m_emFlag ( emFlag )
{

}

CConfirmStateVictoriousCountryAction::~CConfirmStateVictoriousCountryAction () { }

int CConfirmStateVictoriousCountryAction::Execute ( NetServer* pServer )
{
	if ( !pServer )
		return NET_ERROR;

	CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*>( pServer );
	if ( !pAgentServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CConfirmStateVictoriousCountryAction): pAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	GLAgentServer* pGLAgentServer = pAgentServer->GetGLAgentServer();
	if ( NULL == pGLAgentServer )
	{
		sc::writeLogError(
			std::string( "DbAction(CConfirmStateVictoriousCountryAction): pGLAgentServer is NULL.") );

		return sc::db::DB_ERROR;
	}

	if ( m_pDbManager->GetConnectionType() != db::ADO )
	{
		sc::writeLogError( "CConfirmStateVictoriousCountryAction::Execute. Use ADO" );

		return NET_ERROR;
	}

	int nReturn = NET_ERROR;
	nReturn = m_pDbManager->ConfirmStateVictoriousCountry( m_emFlag, m_vecVCountry );
	if ( nReturn == sc::db::DB_ERROR )
	{
		sc::writeLogError(
			std::string( "DbAction(CConfirmStateVictoriousCountryAction) failed to call CConfirmStateVictoriousCountryAction().") );
	}

	Country::GLVictoriousCountryManServer* pVMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( NULL == pVMan )
		return sc::db::DB_ERROR;

	DWORD dwVCountryFirst = pVMan->GetVCountryFirst( m_emFlag );

	if ( m_vecVCountry.empty() )
	{
		// 전장서버의 승전국 정보는 없는데 라이브서버에는 있는 경우;
		// 라이브 서버의 정보를 초기화한다;
		if ( 0 != dwVCountryFirst )
		{
			pGLAgentServer->AddGameAdoJob( db::DbActionPtr(
				new CResetVictoriousCountryAction( m_emFlag ) ) );

			return nReturn;
		}
		// 전장서버와 라이브서버 둘 다 승전국 정보가 없는 경우;
		// 아무것도 하지 않는다;
		else
		{
			return nReturn;
		}
	}

	// 전장서버와 라이브서버의 승전국 정보가 같을 경우;
	// 아무것도 하지 않는다;
	DWORD dwSearchVCountryFirst = m_vecVCountry.at( 0 ).sCountryInfo.dwID;
	if ( dwVCountryFirst == dwSearchVCountryFirst )
		return nReturn;

	// 전장서버의 승전국 정보가 있는데 라이브서버에도 있는 경우;
	// 두 정보가 다르다면 일단 라이브 서버의 정보를 초기화한다;
	// 다시 프로시저가 호출되었을 때 제대로 세팅되게 된다;
	if ( 0 != dwVCountryFirst && dwVCountryFirst != dwSearchVCountryFirst )
	{
		pGLAgentServer->AddGameAdoJob( db::DbActionPtr(
			new CResetVictoriousCountryAction( m_emFlag ) ) );

		return nReturn;
	}

	// 전장서버의 승전국 정보가 있는데 라이브서버에는 없는경우;
	// 승전국 정보를 추가한다;
	for ( Country::VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
		iter != m_vecVCountry.end(); ++iter )
	{
		Country::SVICTORIOUS_COUNTRY sVCountry = *iter;
		pVMan->AddDBAction( sVCountry );
	}

	pVMan->CompleteLoading();	

	return nReturn;
}