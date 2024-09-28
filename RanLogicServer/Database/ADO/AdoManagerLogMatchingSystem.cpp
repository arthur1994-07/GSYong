#include "pch.h"
#include "../../../SigmaCore/gassert.h"
#include "../../../SigmaCore/Json/MinJson.h"
#include "./AdoManager.h"

#include "../../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "../../LottoSystem/GLLottoSystemManAgent.h"
#include "../../LottoSystem/LottoSystem.h"

#include "../../Character/GLCharAG.h"

#include "../../AgentServer/GLAgentServer.h"
#include "../../../RanLogicServer/MatchSystem/MatchingManager.h"

#include <boost/tokenizer.hpp>

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace MatchingSystem;
namespace db
{


	//////////////////////////////////////////////////////////////////////////
	//
	// MATCHING SYSTEM
	//

	int AdoManager::MatchingOnload( std::string Matching_Content_Type,
		std::string Betting_Script,
		std::string Room_Admin_Script,
		std::string Matching_Script,
		std::string Lobby_Field_Manager )
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamVarchar("@Matching_Content_Type", Matching_Content_Type.c_str(), Matching_Content_Type.length());
		Ado.AppendIParamVarchar("@Betting_Script", Betting_Script.c_str(), Betting_Script.length());
		Ado.AppendIParamVarchar("@Room_Admin_Script", Room_Admin_Script.c_str(), Room_Admin_Script.length());
		Ado.AppendIParamVarchar("@Matching_Script", Matching_Script.c_str(), Matching_Script.length());
		Ado.AppendIParamVarchar("@Lobby_Field_Manager", Lobby_Field_Manager.c_str(), Lobby_Field_Manager.length());

		if (!Ado.Execute4Cmd("dbo.sp_MatchingOnload", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingGroup(DWORD GroupID,
		DWORD GameScriptID,
		bool  IsDeleted)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamInteger("@IsDeleted", IsDeleted);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingGroup", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}

	int AdoManager::MatchingGroupUser( DWORD GroupID,
		DWORD GameScriptID,
		DWORD UserDBNum,
		bool  IsDeleted)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@UserDBNum" , UserDBNum);
		Ado.AppendIParamInteger("@IsDeleted", IsDeleted);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingGroupUser", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingQueueIn( DWORD ClientID,
		DWORD GameScriptID,
		bool  IsDeleted)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@ClientID", ClientID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamInteger("@IsCansel", IsDeleted);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingQueueIn", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceIn( DWORD GroupID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingInstanceIn", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceJoin( DWORD GroupID,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD  MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@MapID", MapID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingInstanceJoin", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceOnStart( DWORD GroupReadySize,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupReadySize", GroupReadySize);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingInstanceOnStart", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceOut( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) 
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingInstanceOut", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceResualt( DWORD Faction,
		bool IsLose,
		DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) 
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@Faction", Faction);
		Ado.AppendIParamInteger("@IsLose", IsLose);
		Ado.AppendIParamBigint("@MapID", MapID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingInstanceResualt", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingRoomDestroy( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID) 
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@MapID", MapID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingRoomDestroy", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingRoomReq( DWORD MapID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@MapID", MapID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingRoomReq", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingFinalWinner( DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingFinalWinner", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}
	int AdoManager::MatchingInstanceTournament( int   tournamentPosition,
		std::string GroupName,
		int   GroupIndexPosition,
		int   GroupSize,
		int   GroupMaxSize,
		int   GroupHeadLevel,
		DWORD GroupID,
		DWORD GameScriptID,
		DWORD MatchingScriptID)
	{
		sc::db::AdoExt Ado(m_LogDBConnString);
		Ado.AppendIParamBigint("@tournamentPosition", tournamentPosition);
		Ado.AppendIParamVarchar("@GroupName", GroupName.c_str(), GroupName.length());
		Ado.AppendIParamInteger("@GroupIndexPosition", GroupIndexPosition);
		Ado.AppendIParamInteger("@GroupSize", GroupSize);
		Ado.AppendIParamInteger("@GroupMaxSize", GroupMaxSize);
		Ado.AppendIParamInteger("@GroupHeadLevel", GroupHeadLevel);
		Ado.AppendIParamBigint("@GroupID", GroupID);
		Ado.AppendIParamBigint("@GameScriptID", GameScriptID);
		Ado.AppendIParamBigint("@MatchingScriptID", MatchingScriptID);

		if (!Ado.Execute4Cmd("dbo.sp_MatchingFinalWinner", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		return sc::db::DB_OK;
	}

	int AdoManager::MatchingJoinCountLoad (
		MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST& _refMapCharJoinCountList )
	{
		int nRet = sc::db::DB_OK;

		sc::db::AdoExt ado( m_GameDBConnString );
		if ( !ado.Execute4Cmd( "dbo.sp_MatchingJoinCountLoad", adCmdStoredProc ) )
		{
			return sc::db::DB_ERROR;
		}

		if ( ado.GetEOF() || ado.GetBOF() )
			return sc::db::DB_ERROR;

		do 
		{
			DWORD dwChaDBNum;
			MatchingSystem::SJOIN_COUNT sJoinCount;

			ado.GetCollect( "ChaNum", dwChaDBNum );
			ado.GetCollect( "MatchingBaseID", sJoinCount.m_dwMatchingBaseID );
			ado.GetCollect( "JoinCount", sJoinCount.m_dwJoinCount );

			MatchingSystem::MAP_CHAR_JOIN_COUNT_LIST_ITER iter = _refMapCharJoinCountList.find( dwChaDBNum );
			if ( iter != _refMapCharJoinCountList.end() )
			{
				MatchingSystem::VEC_JOIN_COUNT& vecTempJoinCount = iter->second;

				for ( unsigned int i=0; i<vecTempJoinCount.size(); ++i )
				{
					MatchingSystem::SJOIN_COUNT& sTempJoinCount = vecTempJoinCount.at( i );
					if ( sJoinCount.m_dwMatchingBaseID == sTempJoinCount.m_dwMatchingBaseID )
					{
						// 캐릭터가 매칭에 들어간 횟수를 올린다;
						sTempJoinCount.m_dwJoinCount = sJoinCount.m_dwJoinCount;

						continue;
					}
				}

				// 캐릭터가 한번도 들어간 적이 없던 매칭이라면 처리한다;
				vecTempJoinCount.push_back( sJoinCount );

				continue;
			}

			// 캐릭터가 전체 매칭에 한번도 들어간 적이 없다면 처리한다;
			VEC_JOIN_COUNT vecJoinCount;
			vecJoinCount.push_back( sJoinCount );

			_refMapCharJoinCountList.insert(
				MAP_CHAR_JOIN_COUNT_LIST_VALUE(
				dwChaDBNum,
				vecJoinCount ) );

		} while( ado.Next() );

		return sc::db::DB_OK;
	}

	int AdoManager::MatchingJoinCountReset ( )
	{
		sc::db::AdoExt ado( m_GameDBConnString );
		return ExecuteStoredProcedure( "dbo.sp_MatchingJoinCountReset", ado );
	}

	int AdoManager::MatchingJoinCountUpdate (
		const DWORD _dwCharDBNum,
		const MatchingSystem::SJOIN_COUNT _sJoinCount )
	{
		sc::db::AdoExt ado( m_GameDBConnString );

		APPEND_IPARAM_BIGINT( ado, "@ChaNum", _dwCharDBNum );
		APPEND_IPARAM_INT( ado, "@MatchinbBaseID", _sJoinCount.m_dwMatchingBaseID );
		APPEND_IPARAM_INT( ado, "@JoinCount", _sJoinCount.m_dwJoinCount );

		return ExecuteStoredProcedure( "dbo.sp_MatchingJoinCountUpdate", ado );
	}
}