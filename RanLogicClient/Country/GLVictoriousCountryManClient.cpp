#include "pch.h"

#include "../GLGaeaClient.h"

#include "./GLVictoriousCountryManClient.h"

namespace Country
{

	void GLVictoriousCountryManClient::AddVCountry ( const Country::SVICTORIOUS_COUNTRY& sVCountry )
	{
		// 중복값은 추가하지 않는다;
		for ( VCOUNTRY_VEC_CITER iter = m_vecVCountry.begin();
			iter != m_vecVCountry.end(); ++iter )
		{
			SVICTORIOUS_COUNTRY sTempVCountry = *iter;
			if ( sTempVCountry == sVCountry )
				return;
		}

		m_vecVCountry.push_back( sVCountry );
	}

	void GLVictoriousCountryManClient::ResetVCountry (
		Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
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

	void GLVictoriousCountryManClient::Initialize ()
	{
		ResetVCountry( CAPTURE_THE_ISLANDS );
	}

	const DWORD GLVictoriousCountryManClient::GetVCountryFirst (
		Country::EMCOUNTRY_SYSTEM_FLAG emFlag ) const
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

	const std::string GLVictoriousCountryManClient::GetVCountryFileNameFirst (
		Country::EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		std::string strReturnFileName = UNKNOWN_COUNTRY_FILENAME;

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return strReturnFileName;
		
		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( GetVCountryFirst( emFlag ) );
		if ( NULL == pCountry )
			return strReturnFileName;

		strReturnFileName = pCountry->GetCountryImageFileName();

		return strReturnFileName;
	}

	const std::string GLVictoriousCountryManClient::GetVCountryImageIDFirst (
		EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		std::string strReturnImageID;

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return strReturnImageID;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( GetVCountryFirst( emFlag ) );
		if ( NULL == pCountry )
			return strReturnImageID;

		strReturnImageID = pCountry->GetCountryImageID();

		return strReturnImageID;
	}

	// 승전국 Mini Image ID;
	const std::string GLVictoriousCountryManClient::GetVCountryMiniImageIDFirst (
		EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		std::string strReturnMiniImageID;

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return strReturnMiniImageID;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( GetVCountryFirst( emFlag ) );
		if ( NULL == pCountry )
			return strReturnMiniImageID;

		strReturnMiniImageID = pCountry->GetCountryMiniImageID();

		return strReturnMiniImageID;
	}

	const std::string GLVictoriousCountryManClient::GetVCountryNameFirst (
		EMCOUNTRY_SYSTEM_FLAG emFlag ) const
	{
		std::string strReturnFileName;

		Country::GLCountryManClient* pCountryMan =
			Country::GLCountryManClient::Instance();
		if ( NULL == pCountryMan )
			return strReturnFileName;

		Country::GLCountry* pCountry =
			pCountryMan->GetCountryUsingID( GetVCountryFirst( emFlag ) );
		if ( NULL == pCountry )
			return strReturnFileName;

		strReturnFileName = pCountry->GetCountryName();

		return strReturnFileName;
	}

	void GLVictoriousCountryManClient::MsgVCountryADD ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_ADD_DAF* pAddMsg =
			static_cast< GLMSG::NET_VCOUNTRY_ADD_DAF* >( pMsg );

		if ( pAddMsg )
		{
			ResetVCountry( pAddMsg->m_sVCountry.emFlag );

			AddVCountry( pAddMsg->m_sVCountry );
		}
	}

	void GLVictoriousCountryManClient::MsgVCountryReset ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_RESET_DAF* pResetMsg =
			static_cast< GLMSG::NET_VCOUNTRY_RESET_DAF* >( pMsg );

		if ( pResetMsg )
		{
			ResetVCountry( pResetMsg->m_emFlag );
		}
	}

	void GLVictoriousCountryManClient::MsgSyncVictoriousCountry ( NET_MSG_GENERIC* pMsg )
	{
		GLMSG::NET_VCOUNTRY_SYNC_AC* pSyncMsg =
			static_cast< GLMSG::NET_VCOUNTRY_SYNC_AC* >( pMsg );
		if ( pSyncMsg )
		{
			AddVCountry( pSyncMsg->m_sVictoriousCountry );
		}
	}

}