#include "../pch.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "../Country/GLVictoriousCountryManField.h"

#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

Country::SCOUNTRY_INFO GLGaeaServer::GetCountry( DWORD ChaDbNum )
{
	CFieldServer* pFieldServer =
		static_cast< CFieldServer* >( m_pMsgServer );

	if ( pFieldServer )
	{
		return pFieldServer->GetCountry( ChaDbNum );
	}

	return Country::SCOUNTRY_INFO();
}

void GLGaeaServer::InitializeCountry ( DWORD dwClientID )
{
	CFieldServer* pFieldServer =
		static_cast< CFieldServer* >( m_pMsgServer );

	if ( pFieldServer )
	{
		return pFieldServer->InitializeCountry( dwClientID );
	}
}

BOOL GLGaeaServer::MsgVCountryAdd ( DWORD dwClientID
								  , DWORD dwGaeaID
								  , NET_MSG_GENERIC* nmg )
{
	Country::GLVictoriousCountryManField* pVCountryMan =
		Country::GLVictoriousCountryManField::Instance();
	if ( !pVCountryMan )
		return FALSE;

	GLMSG::NET_VCOUNTRY_ADD_DAF* pAddMsg =
		static_cast< GLMSG::NET_VCOUNTRY_ADD_DAF* >( nmg );
	if ( NULL == pAddMsg )
		return FALSE;

	// 이전 국가 ID 저장해놓는다;
	DWORD dwPreCountryID =
		pVCountryMan->GetVCountryFirst(
		pAddMsg->m_sVCountry.emFlag );

	// 새로운 국가 추가;
	pVCountryMan->MsgVCountryADD( nmg );

	// 콜백 호출;
	InstanceSystem::ManagerField* const pInstanceManagerField =
		GetInstanceManagerField();
	if ( pInstanceManagerField )
	{
		Country::SVICTORIOUS_COUNTRY sVCountryInfo = pAddMsg->m_sVCountry;
		pInstanceManagerField->EventUpdateVictoriousCountry(
			static_cast< DWORD >( sVCountryInfo.emFlag ),				// Flag;
			dwPreCountryID,												// Pre Country;
			sVCountryInfo.sCountryInfo.dwID );							// Cur Country;
	}

	return TRUE;
}

BOOL GLGaeaServer::MsgVCountryReset ( DWORD dwClientID
									, DWORD dwGaeaID
									, NET_MSG_GENERIC* nmg )
{
	Country::GLVictoriousCountryManField* pVCountryMan =
		Country::GLVictoriousCountryManField::Instance();
	if ( !pVCountryMan )
		return FALSE;

	GLMSG::NET_VCOUNTRY_RESET_DAF* pResetMsg =
		static_cast< GLMSG::NET_VCOUNTRY_RESET_DAF* >( nmg );
	if ( NULL == pResetMsg )
		return FALSE;

	// 이전 국가 ID 저장해놓는다;
	DWORD dwPreCountryID =
		pVCountryMan->GetVCountryFirst(
		pResetMsg->m_emFlag );

	// 국가 리셋;
	pVCountryMan->MsgVCountryReset( nmg );

	// 콜백 호출;
	InstanceSystem::ManagerField* const pInstanceManagerField =
		GetInstanceManagerField();
	if ( pInstanceManagerField )
	{
		pInstanceManagerField->EventUpdateVictoriousCountry(
			static_cast< DWORD >( pResetMsg->m_emFlag ),				// Flag;
			dwPreCountryID,												// Pre Country;
			0 );														// Cur Country;
	}

	return TRUE;
}