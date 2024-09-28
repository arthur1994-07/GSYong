#include "pch.h"

#include "../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../../RanLogic/MiniGame/IMiniGame.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Character/GLChar.h"
#include "GLMiniGameManager.h"


namespace MiniGame
{

	const bool GLManagerField::_messageProcedureRequestOpen(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_REQUEST_OPEN* const pRequestOpenMessage = 
			(const GLMSG::NET_MINIGAME_REQUEST_OPEN* const)pMiniGameMessage;		

		GLChar* const pPlayer = m_pGaeaServer->GetChar(dwGaeaID);
		if ( pPlayer == NULL )
		{
			endMiniGame(dwGaeaID);
			return false;
		}

		const DWORD dwMiniGameID = pRequestOpenMessage->dwMiniGameID;
		const DWORD dwNPCGaeaID = pRequestOpenMessage->dwNPCGaeaID;
		const SNATIVEID& NPCID = pRequestOpenMessage->NPCID;

		if ( pPlayer->UserLevel() < USER_GM3 )
			return doMiniGame(dwGaeaID, dwMiniGameID, NPCID, dwNPCGaeaID);

		return doMiniGame(dwGaeaID, dwMiniGameID);
	}

	const bool GLManagerField::_messageProcedureRequestClose(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_REQUEST_CLOSE* const pRequestCloseMessage = 
			(const GLMSG::NET_MINIGAME_REQUEST_CLOSE* const)pMiniGameMessage;	

		return endMiniGame(dwGaeaID);		
	}	

	const bool GLManagerField::MessageProcedure(NET_MSG_GENERIC* nmg, const DWORD dwGaeaID)
	{
		const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage = 
			(const GLMSG::NET_MINIGAME_MESSAGE* const)nmg;

		switch ( pMiniGameMessage->nSubType )
		{
		case MiniGame::EMMESSAGE_REQUEST_OPEN:
			return _messageProcedureRequestOpen(pMiniGameMessage, dwGaeaID);
		case MiniGame::EMMESSAGE_REQUEST_CLOSE:
			return _messageProcedureRequestClose(pMiniGameMessage, dwGaeaID);		
		}


		MiniGameMapIter iter = m_MiniGameMap.find(dwGaeaID);
		if ( iter == m_MiniGameMap.end() )
			return endMiniGame(dwGaeaID, MiniGame::EMERROR_NO_CREATED);

		MiniGame::IMiniGame* const pMiniGame = iter->second;
		if ( pMiniGameMessage->dwMiniGameID != pMiniGame->m_dwMiniGameID )
			return endMiniGame(dwGaeaID, MiniGame::EMERROR_DIFFERENT_ID);


		if ( pMiniGame->doMessage(pMiniGameMessage, dwGaeaID) == false )
		{
			endMiniGame(dwGaeaID, MiniGame::EMERROR_PROCESS_ERROR);
			return false;
		}

		return true;
	}
}

