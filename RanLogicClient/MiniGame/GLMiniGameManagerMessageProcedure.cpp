#include "pch.h"

#include "../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../GLGaeaClient.h"
#include "GLMiniGameManager.h"

namespace MiniGame
{

	const bool GLManagerClient::_messageProcedureDoOpen(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage)
	{
		const GLMSG::NET_MINIGAME_DO_OPEN* const pDoOpenMessage = 
			(const GLMSG::NET_MINIGAME_DO_OPEN* const)pMiniGameMessage;

		if ( pDoOpenMessage->emError != MiniGame::EMERROR_NO_ERROR )
		{
			RequestClose();
			return false;
		}

		const DWORD dwMiniGameID = pDoOpenMessage->dwMiniGameID;
		const DWORD dwMyGaeaID = m_pGaeaClient->GETMYGAEAID();
		const DWORD dwNPCGaeaID = pDoOpenMessage->dwNPCGaeaID;		
		const SNATIVEID& NPCID = pDoOpenMessage->NPCID;
		const float fBeginTime = pDoOpenMessage->fBeginTime;

		if ( m_pMiniGame != NULL )
			_endMiniGame(); // 기존에 활성화 되있던게 있다면 삭제;

		const bool bResult = _doMiniGame(dwMyGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID);
		if ( bResult == false )
			m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_REQUEST_CLOSE());
		// 클라이언트에서 생성에 실패 한 경우 서버에 close요청을 함;
		// RequestClose() 함수는 미니 게임이 생성 되있지 "않은" 경우,
		// 아무 일도 하지 않기 때문에;
		// 생성 실패에 한해서 직접 close요청 메세지를 송신;

		return true;
	}
	const bool GLManagerClient::_messageProcedureDoClose(const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage)
	{
		const GLMSG::NET_MINIGAME_DO_CLOSE* const pDoCloseMessage = 
			(const GLMSG::NET_MINIGAME_DO_CLOSE* const)pMiniGameMessage;

		MiniGame::EMERROR_TYPE emError = pDoCloseMessage->emError;
		switch ( emError )
		{
		case MiniGame::EMERROR_NO_LAND:
		case MiniGame::EMERROR_NO_NPC:
		case MiniGame::EMERROR_NO_ACTIONABLE:
		case MiniGame::EMERROR_ALREADY_BEGUN:
		case MiniGame::EMERROR_CREATE_FAIL:
		case MiniGame::EMERROR_NO_CREATED:
		case MiniGame::EMERROR_DIFFERENT_ID:
		case MiniGame::EMERROR_PROCESS_ERROR:
			// 경우에 맞는 에러 메세지 출력;
//			m_pInterface->
			break;
		}

		return _endMiniGame();
	}

	const bool GLManagerClient::doMessage(NET_MSG_GENERIC* const pNetMsg)
	{
		const GLMSG::NET_MINIGAME_MESSAGE* const pMiniGameMessage = (const GLMSG::NET_MINIGAME_MESSAGE* const)pNetMsg;

		switch ( pMiniGameMessage->nSubType )
		{
		case MiniGame::EMMESSAGE_DO_OPEN:
			return _messageProcedureDoOpen(pMiniGameMessage);
		case MiniGame::EMMESSAGE_DO_CLOSE:
			return _messageProcedureDoClose(pMiniGameMessage);		
		}

		if ( m_pMiniGame == NULL )
			return false;

		if ( m_pMiniGame->m_dwMiniGameID != pMiniGameMessage->dwMiniGameID )
			return false;

		if ( m_pMiniGame->doMessage(pMiniGameMessage) == false )
		{
			RequestClose();
			return false;
		}

		return true;
	}
}

