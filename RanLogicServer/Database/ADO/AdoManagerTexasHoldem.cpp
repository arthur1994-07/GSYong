#include "pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../RanLogic/Msg/GLContrlTexasHoldemMsg.h"
#include "./ADOManager.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{
	int AdoManager::TexasHoldemChipMapSelect( std::vector< TexasHoldem::sChipPrice >& vecChip )
	{
		sc::db::AdoExt Ado( m_UserDBConnString );
		if ( !Ado.Execute4Cmd( "dbo.sp_GetTexasHoldemChipMap", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		if ( Ado.GetEOF() )
		{
			Ado.PRINT( "ADO:There's no data. RecordSet(TexasHoldemChipMapSelect) is Empty." );
			return sc::db::DB_ERROR;
		}

		// chip, gamemoney, point
		LONGLONG Chip = 0;
		LONGLONG GameMoney = 0;
		LONGLONG Point = 0;
		do
		{            
			Ado.GetCollect( 0, Chip );
			Ado.GetCollect( 1, GameMoney );
			Ado.GetCollect( 2, Point );
			if ( !Ado.IsCollectedAll() )
				return sc::db::DB_ERROR;

			TexasHoldem::sChipPrice sChip( Chip, GameMoney, Point );
			vecChip.push_back( sChip );
		} while ( Ado.Next() );

		return sc::db::DB_OK;
	}

	int AdoManager::TexasHoldemChipMapVersionSelect( LONGLONG& Version )
	{
		sc::db::AdoExt Ado( m_UserDBConnString );
		if ( !Ado.Execute4Cmd( "dbo.sp_GetTexasHoldemChipMapVersion", adCmdStoredProc ) )
			return sc::db::DB_ERROR;

		if ( Ado.GetEOF() )
		{
			Ado.PRINT( "ADO:There's no data. RecordSet(TexasHoldemChipMapVersionSelect) is Empty." );
			return sc::db::DB_ERROR;
		}

		LONGLONG VersionTemp = 0;
		do
		{            
			Ado.GetCollect( 0, VersionTemp );
			if ( !Ado.IsCollectedAll() )
				return sc::db::DB_ERROR;
		} while ( Ado.Next() );

		Version = VersionTemp;

		sc::writeLogInfo( sc::string::format( "TexasHoldemChipMapVersionSelect version %1%", Version ) );

		return sc::db::DB_OK;
	}

	int AdoManager::TexasHoldemChipSelect( SCHARDATA2* pChaData2 )
	{
		sc::db::AdoExt Ado( m_GameDBConnString );
		APPEND_IPARAM_INT( Ado, "@nChaNum", pChaData2->CharDbNum() );

		if ( FALSE == Ado.ExecuteStoredProcedure( "dbo.sp_GetTexasHoldemChip" ) )
			return sc::db::DB_ERROR;

		if ( Ado.GetEOF() )    
			return sc::db::DB_ERROR;

		LONGLONG chip = 0;
		Ado.GetCollect( "chip", chip );      
		pChaData2->SetTexasHoldemChip( chip );

		return sc::db::DB_OK;
	}

	int	AdoManager::TexasHoldemChipSelectFromWeb( int nServerGroup, DWORD dwChaDbNum, LONGLONG& chip )
	{
		sc::db::AdoExt Ado( m_TexasHoldemDBConnString );
		std::string Query( sc::string::format( "SELECT UserChip FROM dbo.V_RanMiniHoldem_UserInfo WHERE SGNum = %1% AND ChaNum = %2%", nServerGroup, dwChaDbNum ) );
		Ado.Execute4Cmd( Query );

		if ( Ado.GetEOF() )
		{
			// 없을 수도 있다.
			return sc::db::DB_OK;
		}
		else
		{
			do
			{
				LONGLONG chipTemp = 0;
				Ado.GetCollect( "UserChip", chipTemp );
				chip = chipTemp;
			}
			while ( Ado.Next() );
		}

		return sc::db::DB_OK;
	}

	int	AdoManager::TexasHoldemChipUpdate( DWORD dwChaDbNum, LONGLONG& chip, bool bPlus /*= false*/ )
	{
		if ( bPlus )
		{
			sc::db::AdoExt Ado( m_GameDBConnString );
			APPEND_IPARAM_INT( Ado, "@nChaNum", dwChaDbNum );
			APPEND_IPARAM_BIGINT( Ado, "@chip", chip );

			return Ado.ExecuteStoredProcedure( "dbo.sp_AddTexasHoldemChip" );
		}
		else
		{
			sc::db::AdoExt Ado( m_GameDBConnString );
			APPEND_IPARAM_INT( Ado, "@nChaNum", dwChaDbNum );
			APPEND_IPARAM_BIGINT( Ado, "@chip", chip );

			return Ado.ExecuteStoredProcedure( "dbo.sp_UpdateTexasHoldemChip" );
		}
	}

	int	AdoManager::TexasHoldemChipLogInsert( DWORD dwChaDbNum, LONGLONG& chip, LONGLONG& GameMoney, LONGLONG& Point )
	{
		sc::db::AdoExt Ado( m_LogDBConnString );
		APPEND_IPARAM_INT( Ado, "@nChaNum", dwChaDbNum );
		APPEND_IPARAM_BIGINT( Ado, "@chip", chip );
		APPEND_IPARAM_BIGINT( Ado, "@gamemoney", GameMoney );
		APPEND_IPARAM_BIGINT( Ado, "@point", Point );

		return Ado.ExecuteStoredProcedure( "dbo.sp_InsertLogTexasHoldemChip" );
	}

	int	AdoManager::TexasHoldemAuthKeyInsert( DWORD dwChaDbNum, const std::string& strGUID )
	{
		sc::db::AdoExt Ado( m_GameDBConnString );
		APPEND_IPARAM_INT( Ado, "@nChaNum", dwChaDbNum );
		APPEND_IPARAM_GUID( Ado, "@AuthKey", strGUID.c_str() );

		return Ado.ExecuteStoredProcedure( "dbo.sp_AddTexasHoldemAuthKey" );
	}

	int	AdoManager::TexasHoldemGameTimeInsert( DWORD dwChaDbNum, DWORD dwUserNum, int nSecond )
	{
		sc::db::AdoExt Ado( m_GameDBConnString );
		APPEND_IPARAM_INT( Ado, "@nChaNum", dwChaDbNum );
		APPEND_IPARAM_INT( Ado, "@nUserNum", dwUserNum );
		APPEND_IPARAM_INT( Ado, "@GameTimeSec", nSecond );

		return Ado.ExecuteStoredProcedure( "dbo.sp_AddLogTexasHoldemGameTime" );
	}
}