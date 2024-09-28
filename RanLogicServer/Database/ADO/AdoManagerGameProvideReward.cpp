#include "pch.h"

#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Json/MinJson.h"

#include "../../../RanLogic/GLUseFeatures.h"

#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace ProvideRewardSystem;

namespace db
{

	int AdoManager::ProvideRewardInitialize ( )
	{
		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return sc::db::DB_OK;

		// 전장서버 입장서버인 경우에만 동작한다;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return sc::db::DB_OK;

		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoWBDB( m_WorldBattleDBConnString );
		nRet = ExecuteStoredProcedure( "dbo.sp_ProvideRewardInitialize", adoWBDB );

		return nRet;
	}


	int AdoManager::ProvideRewardInsert ( const ProvideRewardSystem::PROVIDE_REWARD_DATA& _ProvideRewardData )
	{
		// 전장서버에서만 등록 가능하다;
		if ( false == GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return sc::db::DB_OK;

		// 전장서버 입장서버인 경우에만 동작하지 않는다;
		if ( GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return sc::db::DB_OK;

		/************************************************************************/
		/* @ChaNum							BIGINT,                             */
		/* @RewardType						int,                                */
		/* @RewardValue1					BIGINT,	                            */
		/* @RewardValue2					BIGINT,                             */
		/* @RewardValue3					BIGINT,                             */
		/* @RewardValue4					BIGINT,                             */
		/* @TextIndex						int                                 */
		/************************************************************************/

		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoGameDB( m_GameDBConnString );
		APPEND_IPARAM_BIGINT( adoGameDB, "@ChaNum", _ProvideRewardData.GetChaDBNum() );
		APPEND_IPARAM_INT( adoGameDB, "@RewardType", _ProvideRewardData.GetRewardType() );
		APPEND_IPARAM_BIGINT( adoGameDB, "@RewardValue1", _ProvideRewardData.GetRewardValue1() );
		APPEND_IPARAM_BIGINT( adoGameDB, "@RewardValue2", _ProvideRewardData.GetRewardValue2() );
		APPEND_IPARAM_BIGINT( adoGameDB, "@RewardValue3", _ProvideRewardData.GetRewardValue3() );
		APPEND_IPARAM_BIGINT( adoGameDB, "@RewardValue4", _ProvideRewardData.GetRewardValue4() );
		APPEND_IPARAM_INT( adoGameDB, "@TextIndex", _ProvideRewardData.GetTextIndex() );

		nRet = ExecuteStoredProcedure( "dbo.sp_ProvideRewardInsert", adoGameDB );

		return nRet;
	}


	int AdoManager::ProvideRewardSelect (
		DWORD _dwChaDBNum,
		DWORD _dwServerGroupNum,
		OUT ProvideRewardSystem::VEC_PROVIDE_REWARD& _vecProvideRewardData )
	{
		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return sc::db::DB_OK;

		// 전장서버 입장서버인 경우에만 동작한다;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return sc::db::DB_OK;

		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoWBDB( m_WorldBattleDBConnString );
		APPEND_IPARAM_BIGINT( adoWBDB, "@ChaNum", _dwChaDBNum );
		APPEND_IPARAM_BIGINT( adoWBDB, "@SGNum", _dwServerGroupNum );

		if ( !adoWBDB.Execute4Cmd( "dbo.sp_ProvideRewardSelect", adCmdStoredProc ) )
		{
			nRet = sc::db::DB_ERROR;
		}

		if ( adoWBDB.GetEOF() || adoWBDB.GetBOF() )
		{
			return nRet;
		}

		do
		{
			DWORD dwUniqueID = 0;
			DWORD dwChaNum = 0;
			int nRewardType = 0;
			DWORD dwRewardValue1 = 0;
			DWORD dwRewardValue2 = 0;
			DWORD dwRewardValue3 = 0;
			DWORD dwRewardValue4 = 0;
			int nTextIndex = 0;

			// Seq, ChaNum, RewardType, RewardValue1, RewardValue2, RewardValue3, RewardValue4, TextIndex
			adoWBDB.GetCollect( "Seq", dwUniqueID );
			adoWBDB.GetCollect( "ChaNum", dwChaNum );
			adoWBDB.GetCollect( "RewardType", nRewardType );
			adoWBDB.GetCollect( "RewardValue1", dwRewardValue1 );
			adoWBDB.GetCollect( "RewardValue2", dwRewardValue2 );
			adoWBDB.GetCollect( "RewardValue3", dwRewardValue3 );
			adoWBDB.GetCollect( "RewardValue4", dwRewardValue4 );
			adoWBDB.GetCollect( "TextIndex", nTextIndex );

			ProvideRewardSystem::PROVIDE_REWARD_DATA provideRewardData;
			provideRewardData.SetUniqueID( dwUniqueID );
			provideRewardData.SetCharacter( dwChaNum );
			provideRewardData.SetRewardType( static_cast< ProvideRewardSystem::PROVIDE_REWARD_DATA::EMREWARD_TYPE >( nRewardType ) );
			provideRewardData.SetRewardValue( dwRewardValue1, dwRewardValue2, dwRewardValue3, dwRewardValue4 );
			provideRewardData.SetTextIndex( nTextIndex );

			_vecProvideRewardData.push_back( provideRewardData );

		} while( adoWBDB.Next() );

		return nRet;
	}


	int AdoManager::ProvideRewardClearDeleteFlag ( )
	{
		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return sc::db::DB_OK;

		// 전장서버 입장서버인 경우에만 동작한다;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return sc::db::DB_OK;

		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoWBDB( m_WorldBattleDBConnString );
		nRet = ExecuteStoredProcedure( "dbo.sp_ProvideRewardFlagClear", adoWBDB );

		return nRet;
	}


	int AdoManager::ProvideRewardComplete ( DWORD _dwUniqueID )
	{
		// 전장서버가 아닌경우에만 동작한다;
		if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
			return sc::db::DB_OK;

		// 전장서버 입장서버인 경우에만 동작한다;
		if ( false == GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
			return sc::db::DB_OK;

		int nRet = sc::db::DB_OK;

		sc::db::AdoExt adoWBDB( m_WorldBattleDBConnString );
		APPEND_IPARAM_BIGINT( adoWBDB, "@UniqueID", _dwUniqueID );
		nRet = ExecuteStoredProcedure( "dbo.sp_ProvideRewardComplete", adoWBDB );

		return nRet;
	}

}