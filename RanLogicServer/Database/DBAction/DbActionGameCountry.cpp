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

	// ���濡 ���������� Agent ���� �޽����� �־��ش�;
	// ������ Ŭ���̾�Ʈ�� Country ������ �����Ѵ�;
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

// ���� Set ������� �����Ѵ�;
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

	// ���濡 ���������� Agent ���� �޽����� �־��ش�;
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

	// ���濡 ���������� Agent ���� �޽����� �־��ش�;
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
		// ���弭���� ������ ������ ���µ� ���̺꼭������ �ִ� ���;
		// ���̺� ������ ������ �ʱ�ȭ�Ѵ�;
		if ( 0 != dwVCountryFirst )
		{
			pGLAgentServer->AddGameAdoJob( db::DbActionPtr(
				new CResetVictoriousCountryAction( m_emFlag ) ) );

			return nReturn;
		}
		// ���弭���� ���̺꼭�� �� �� ������ ������ ���� ���;
		// �ƹ��͵� ���� �ʴ´�;
		else
		{
			return nReturn;
		}
	}

	// ���弭���� ���̺꼭���� ������ ������ ���� ���;
	// �ƹ��͵� ���� �ʴ´�;
	DWORD dwSearchVCountryFirst = m_vecVCountry.at( 0 ).sCountryInfo.dwID;
	if ( dwVCountryFirst == dwSearchVCountryFirst )
		return nReturn;

	// ���弭���� ������ ������ �ִµ� ���̺꼭������ �ִ� ���;
	// �� ������ �ٸ��ٸ� �ϴ� ���̺� ������ ������ �ʱ�ȭ�Ѵ�;
	// �ٽ� ���ν����� ȣ��Ǿ��� �� ����� ���õǰ� �ȴ�;
	if ( 0 != dwVCountryFirst && dwVCountryFirst != dwSearchVCountryFirst )
	{
		pGLAgentServer->AddGameAdoJob( db::DbActionPtr(
			new CResetVictoriousCountryAction( m_emFlag ) ) );

		return nReturn;
	}

	// ���弭���� ������ ������ �ִµ� ���̺꼭������ ���°��;
	// ������ ������ �߰��Ѵ�;
	for ( Country::VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
		iter != m_vecVCountry.end(); ++iter )
	{
		Country::SVICTORIOUS_COUNTRY sVCountry = *iter;
		pVMan->AddDBAction( sVCountry );
	}

	pVMan->CompleteLoading();	

	return nReturn;
}