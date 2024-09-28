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
			_endMiniGame(); // ������ Ȱ��ȭ ���ִ��� �ִٸ� ����;

		const bool bResult = _doMiniGame(dwMyGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID);
		if ( bResult == false )
			m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_REQUEST_CLOSE());
		// Ŭ���̾�Ʈ���� ������ ���� �� ��� ������ close��û�� ��;
		// RequestClose() �Լ��� �̴� ������ ���� ������ "����" ���,
		// �ƹ� �ϵ� ���� �ʱ� ������;
		// ���� ���п� ���ؼ� ���� close��û �޼����� �۽�;

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
			// ��쿡 �´� ���� �޼��� ���;
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

