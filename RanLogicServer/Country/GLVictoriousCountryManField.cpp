#include "pch.h"

#include "../../RanLogic/Msg/GLContrlCountry.h"

#include "../Database/DBAction/DbActionGameCountry.h"

#include "../FieldServer/GLGaeaServer.h"

#include "./GLVictoriousCountryManField.h"

namespace Country
{
	void GLVictoriousCountryManField::LoadDBAction ()
	{
		if ( m_pServer )
		{
			m_pServer->AddGameAdoJob( db::DbActionPtr(
				new CLoadFieldVictoriousCountryAction() ) );
		}
	}

	void GLVictoriousCountryManField::AddVCountry ( const Country::SVICTORIOUS_COUNTRY& sVCountry )
	{
		m_vecVCountry.push_back( sVCountry );
	}

	void GLVictoriousCountryManField::ResetVCountry ( EMCOUNTRY_SYSTEM_FLAG emFlag )
	{
		for ( VCOUNTRY_VEC_ITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); )
		{
			SVICTORIOUS_COUNTRY sVCountry = *iter;
			if ( sVCountry.emFlag == emFlag )
				iter = m_vecVCountry.erase( iter );
			else
				++iter;
		}
	}

	const DWORD GLVictoriousCountryManField::GetVCountryFirst ( EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		for ( VCOUNTRY_VEC_CITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			SVICTORIOUS_COUNTRY sVCountry = *iter;
			if ( sVCountry.emFlag == emFlag )
				return sVCountry.sCountryInfo.dwID;
		}

		return SCOUNTRY_INFO().dwID;
	}

	void GLVictoriousCountryManField::SendAddVCountryFA (
		const Country::SVICTORIOUS_COUNTRY& sVCountry,
		bool bTempVCountry )
	{
		if ( m_pServer )
		{
			GLMSG::NET_VCOUNTRY_ADD_FAC sMsg( sVCountry );
			sMsg.m_bTempVCountry = bTempVCountry;

			m_pServer->SENDTOAGENT( &sMsg );
		}
	}

	void GLVictoriousCountryManField::SendResetVCountryFA (
		Country::EMCOUNTRY_SYSTEM_FLAG emFlag,
		bool bTempVCountry )
	{
		if ( m_pServer )
		{
			GLMSG::NET_VCOUNTRY_RESET_FAC sMsg( emFlag );
			sMsg.m_bTempVCountry = bTempVCountry;

			m_pServer->SENDTOAGENT( &sMsg );
		}
	}

	void GLVictoriousCountryManField::MsgVCountryADD ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_ADD_DAF* pAddMsg =
			static_cast< GLMSG::NET_VCOUNTRY_ADD_DAF* >( pMsg );

		if ( pAddMsg )
		{
			ResetVCountry( pAddMsg->m_sVCountry.emFlag );

			AddVCountry( pAddMsg->m_sVCountry );

			if ( m_pServer )
				m_pServer->SENDTOALLCLIENT( pAddMsg );
		}
	}

	void GLVictoriousCountryManField::MsgVCountryReset ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_RESET_DAF* pResetMsg =
			static_cast< GLMSG::NET_VCOUNTRY_RESET_DAF* >( pMsg );

		if ( pResetMsg )
		{
			ResetVCountry( pResetMsg->m_emFlag );

			if ( m_pServer )
				m_pServer->SENDTOALLCLIENT( pResetMsg );
		}
	}
}