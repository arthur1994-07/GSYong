#include "../pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../MfcExLib/RanFilter.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "../Database/DBAction/DbActionGameCountry.h"

#include "../Country/GLVictoriousCountryManServer.h"

#include "../Server/AgentServer.h"

#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

Country::SCOUNTRY_INFO GLAgentServer::GetCountry( DWORD ChaDbNum ) const
{
	CAgentServer* pAgentServer =
		static_cast< CAgentServer* >( m_pMsgServer );

	if ( pAgentServer )
	{
		return pAgentServer->GetCountry( ChaDbNum );
	}

	return Country::SCOUNTRY_INFO();
}

BOOL GLAgentServer::MsgVCountryAddFAC ( NET_MSG_GENERIC* nmg
									 , DWORD dwClientID
									 , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryADDFAC( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgVCountryAddDA ( NET_MSG_GENERIC* nmg
									 , DWORD dwClientID
									 , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryADDDA( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgVCountryAddAA ( NET_MSG_GENERIC* nmg
									 , DWORD dwClientID
									 , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryADDAA( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgVCountryResetFAC ( NET_MSG_GENERIC* nmg
									   , DWORD dwClientID
									   , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryResetFAC( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgVCountryResetDA ( NET_MSG_GENERIC* nmg
									   , DWORD dwClientID
									   , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryResetDA( nmg );

	return TRUE;
}

BOOL GLAgentServer::MsgVCountryResetAA ( NET_MSG_GENERIC* nmg
									   , DWORD dwClientID
									   , DWORD dwGaeaID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return FALSE;

	pVCountryMan->MsgVCountryResetAA( nmg );

	return TRUE;
}

void GLAgentServer::CountrySystemSyncState ( DWORD dwClientID )
{
	Country::GLVictoriousCountryManServer* pVCountryMan =
		Country::GLVictoriousCountryManServer::Instance();
	if ( !pVCountryMan )
		return;

	pVCountryMan->SyncState( dwClientID );
}