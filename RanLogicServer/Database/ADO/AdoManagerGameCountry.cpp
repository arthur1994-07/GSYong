#include "pch.h"

#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Json/MinJson.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "../../Character/GLCharAG.h"
#include "../../AgentServer/GLAgentServer.h"

#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace Country;

namespace db
{

	// Update User Country;
	int AdoManager::UpdateUserCountry ( IN DWORD dwUserID, IN Country::SCOUNTRY_INFO& sCountryInfo )
	{

		sc::db::AdoExt adoLogDB( m_LogDBConnString );
		APPEND_IPARAM_BIGINT( adoLogDB, "@UserID", dwUserID );
		APPEND_IPARAM_BIGINT( adoLogDB, "@Continent", sCountryInfo.wContinentID );
		APPEND_IPARAM_BIGINT( adoLogDB, "@Country", sCountryInfo.wCountryID );

		ExecuteStoredProcedure( "dbo.sp_LogUserCountry", adoLogDB );

		sc::db::AdoExt adoUserDB( m_UserDBConnString );
		APPEND_IPARAM_BIGINT( adoUserDB, "@UserID", dwUserID );
		APPEND_IPARAM_BIGINT( adoUserDB, "@Continent", sCountryInfo.wContinentID );
		APPEND_IPARAM_BIGINT( adoUserDB, "@Country", sCountryInfo.wCountryID );

		return ExecuteStoredProcedure( "dbo.sp_UpdateUserCountry", adoUserDB );
	}

	int AdoManager::LoadVictoriousCountry ( OUT Country::VCOUNTRY_VEC& vecVCountry )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoGameDB( m_GameDBConnString );

		if ( !adoGameDB.Execute4Cmd( "dbo.sp_LoadVictoriousCountry", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if ( adoGameDB.GetEOF() || adoGameDB.GetBOF() )
		{
			return nRet;
		}

		do
		{
			int iFlag = 0;
			SVICTORIOUS_COUNTRY sVictoriousCountry;

			// CountrySystemFlag, ContinentID, CountryID, MapMainID, MapSubID
			adoGameDB.GetCollect( "CountrySystemFlag", iFlag );
			adoGameDB.GetCollect( "ContinentID", sVictoriousCountry.sCountryInfo.wContinentID );
			adoGameDB.GetCollect( "CountryID", sVictoriousCountry.sCountryInfo.wCountryID );
			adoGameDB.GetCollect( "MapMainID", sVictoriousCountry.sMapID.wMainID );
			adoGameDB.GetCollect( "MapSubID", sVictoriousCountry.sMapID.wSubID );

			sVictoriousCountry.emFlag = static_cast< Country::EMCOUNTRY_SYSTEM_FLAG >( iFlag );

			vecVCountry.push_back( sVictoriousCountry );

		} while( adoGameDB.Next() );

		return nRet;
	}

	int AdoManager::AddVictoriousCountry ( IN Country::SVICTORIOUS_COUNTRY& sVictoriousCountry )
	{
		sc::db::AdoExt adoLogDB( m_LogDBConnString );
		APPEND_IPARAM_INT( adoLogDB, "@CountrySystemFlag", static_cast< int >( sVictoriousCountry.emFlag ) );
		APPEND_IPARAM_BIGINT( adoLogDB, "@ContinentID", sVictoriousCountry.sCountryInfo.wContinentID );
		APPEND_IPARAM_BIGINT( adoLogDB, "@CountryID", sVictoriousCountry.sCountryInfo.wCountryID );
		APPEND_IPARAM_INT( adoLogDB, "@MapMainID", sVictoriousCountry.sMapID.Mid() );
		APPEND_IPARAM_INT( adoLogDB, "@MapSubID", sVictoriousCountry.sMapID.Sid() );

		ExecuteStoredProcedure( "dbo.sp_LogVictoriousCountry", adoLogDB );

		/************************************************************************/
		/* @CountrySystemFlag				int,			                    */
		/* @ContinentID						BIGINT,                             */
		/* @CountryID						BIGINT,                             */
		/* @MapMainID						int,                                */
		/* @MapSubID						int                                 */
		/************************************************************************/
		sc::db::AdoExt adoGameDB( m_GameDBConnString );
		APPEND_IPARAM_INT( adoGameDB, "@CountrySystemFlag", static_cast< int >( sVictoriousCountry.emFlag ) );
		APPEND_IPARAM_BIGINT( adoGameDB, "@ContinentID", sVictoriousCountry.sCountryInfo.wContinentID );
		APPEND_IPARAM_BIGINT( adoGameDB, "@CountryID", sVictoriousCountry.sCountryInfo.wCountryID );
		APPEND_IPARAM_INT( adoGameDB, "@MapMainID", sVictoriousCountry.sMapID.Mid() );
		APPEND_IPARAM_INT( adoGameDB, "@MapSubID", sVictoriousCountry.sMapID.Sid() );

		return ExecuteStoredProcedure( "dbo.sp_InsertVictoriousCountry", adoGameDB );
	}

	int AdoManager::ResetVictoriousCountry ( IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag )
	{
		/************************************************************************/
		/* @CountrySystemFlag				int                                 */
		/************************************************************************/
		sc::db::AdoExt adoGameDB( m_GameDBConnString );
		APPEND_IPARAM_INT( adoGameDB, "@CountrySystemFlag", static_cast< int >( emFlag ) );

		return ExecuteStoredProcedure( "dbo.sp_ResetVictoriousCountry", adoGameDB );
	}

	int AdoManager::ConfirmStateVictoriousCountry (
		IN Country::EMCOUNTRY_SYSTEM_FLAG emFlag,
		OUT Country::VCOUNTRY_VEC& vecVCountry )
	{
		int nRet = sc::db::DB_OK;

		// 전장 서버가 아닐 경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return nRet;

		// 전장 서버 입장 서버일 경우에만 수행한다;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return nRet;

		sc::db::AdoExt adoWBDB( m_WorldBattleDBConnString );
		APPEND_IPARAM_INT( adoWBDB, "@CountrySystemFlag", static_cast< int >( emFlag ) );

		if ( !adoWBDB.Execute4Cmd( "dbo.sp_ConfirmStateVictoriousCountry", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if ( adoWBDB.GetEOF() || adoWBDB.GetBOF() )
		{
			return nRet;
		}

		do
		{
			int iFlag = 0;
			SVICTORIOUS_COUNTRY sVictoriousCountry;

			// CountrySystemFlag, ContinentID, CountryID, MapMainID, MapSubID
			adoWBDB.GetCollect( "CountrySystemFlag", iFlag );
			adoWBDB.GetCollect( "ContinentID", sVictoriousCountry.sCountryInfo.wContinentID );
			adoWBDB.GetCollect( "CountryID", sVictoriousCountry.sCountryInfo.wCountryID );
			adoWBDB.GetCollect( "MapMainID", sVictoriousCountry.sMapID.wMainID );
			adoWBDB.GetCollect( "MapSubID", sVictoriousCountry.sMapID.wSubID );

			sVictoriousCountry.emFlag = static_cast< Country::EMCOUNTRY_SYSTEM_FLAG >( iFlag );

			vecVCountry.push_back( sVictoriousCountry );

		} while( adoWBDB.Next() );

		return nRet;
	}
}