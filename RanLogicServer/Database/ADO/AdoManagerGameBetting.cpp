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

	int AdoManager::TournamentBettingSelect(DWORD ChaDbNum , DWORD BettingStatus)
	{
		MatchingCore * pMatchingCore = MatchingCore::getinstance();
		if(pMatchingCore == NULL)
			return sc::db::DB_ERROR;

		sc::db::AdoExt Ado(m_GameDBConnString);
		APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);
		APPEND_IPARAM_INT(Ado, "@BettingStatus", BettingStatus);

		if (!Ado.Execute4Cmd("dbo.sp_BettingSelect", adCmdStoredProc))	
			return sc::db::DB_ERROR;

		if (Ado.GetEOF())
			return sc::db::DB_OK;

		do
		{   
			DWORD outChaDbNum;
			LONGLONG outBettingCount;

			Ado.GetCollect("ChaNum", outChaDbNum);
			Ado.GetCollect("BettingCount", outBettingCount);

			//sc::writeLogInfo(sc::string::format("NotifySelect %1% %2%", strGuid, szGuid));

			pMatchingCore->SetPastBettingUser(outChaDbNum,outBettingCount);

		} while (Ado.Next());

		return sc::db::DB_OK;

	}

	int AdoManager::TournamentBettingInsert(DWORD ChaNum,LONGLONG BettingCount, DWORD BettingGameSciptid,DWORD BettingGroup)
	{
		//char szJsonStr[512] = {0};
		//StringCchCopy(szJsonStr, 512, Data.c_str());
		sc::db::AdoExt Ado(m_GameDBConnString);
		APPEND_IPARAM_INT(Ado, "@ChaNum", ChaNum);
		APPEND_IPARAM_BIGINT    (Ado, "@BettingCount",     BettingCount);    
		APPEND_IPARAM_INT  (Ado, "@BettingGameInfo", BettingGameSciptid);
		APPEND_IPARAM_INT(Ado, "@BettingGroup", BettingGroup);

		return ExecuteStoredProcedure("dbo.sp_BettingInsert", Ado);
	}
	int AdoManager::TournamentBettingUpdate( DWORD ChaNum,DWORD BettingGameSciptid,DWORD BettingStatus)
	{   
		sc::db::AdoExt Ado(m_GameDBConnString);

		APPEND_IPARAM_INT(Ado, "@NotifyGuid", ChaNum);
		APPEND_IPARAM_INT(Ado, "@BettingGameSciptid", ChaNum);
		APPEND_IPARAM_INT(Ado, "@BettingStatus", BettingStatus);

		return ExecuteStoredProcedure("dbo.sp_BettingUpdate", Ado);
	}

	int AdoManager::TournamentBettingDelete(DWORD ChaNum)
	{   
		sc::db::AdoExt Ado(m_GameDBConnString);

		APPEND_IPARAM_INT(Ado, "@ChaNum", ChaNum);

		return ExecuteStoredProcedure("dbo.sp_BettingDelete", Ado);
	}
}