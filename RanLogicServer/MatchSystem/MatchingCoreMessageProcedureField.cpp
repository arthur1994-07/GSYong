#include "pch.h"
#include "./MatchingManager.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"
#include "../Database/DBAction/DbActionGameCompetition.h"
#include "../Database/DBAction/DbActionGameBetting.h"
#include "../../RanLogicServer/Character/GLChar.h"


namespace MatchingSystem
{
	const bool MatchingCore::messageProcedureField(const NET_MSG_GENERIC* _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		if ( GLUseFeatures::GetInstance().IsUsingTournament() == false)
			return true;		

		const GLMSG::NET_MATCHING_MESSAGE* const _pMatchingMessage((const GLMSG::NET_MATCHING_MESSAGE* const)_pMessage);

		switch(_pMatchingMessage->emType)
		{
		case EMMESSAGE_ADMISSION_CF:
			return _messageJoinFieldMatchedGame(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_MONEY_FEE_OK_AF:
			return _messageBettingMoneyFeeCheck(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_MONEY_FEE_SPEND_AF:
			return _messageTournamentMoneyFeeSpend(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MONEY_OK_AF:
			return _messageTournamentMoneyFeeCheck(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MONEY_OK_SPEND_AF:
			return _messageBettingMoneyFeeSpend(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MONEY_OK_INCOME_AF:
			return _messageBettingMoneyINCOMESpend(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MONEY_OK_RESTORE_AF:
			return _messageBettingMoneyRestore(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case  EMMESSAGE_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF:
			return _messageSendPostWinMoneyFC(_pMatchingMessage, _dwClientID, _dwGaeaID);
		}

		return true;
	}

	const bool MatchingCore::_messageSendPostWinMoneyFC(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MESSAGE_REQ_POST_WIN_MONEY_AF* const)_pMessage);


		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == _pMatchedGameMessage->ScriptID )
			{
				land->SendPostWinMoneyFC(
					_pMatchedGameMessage->m_dwDbNum,
					_pMatchedGameMessage->m_dwKang,
					_pMatchedGameMessage->m_winMoney,
					_pMatchedGameMessage->m_emPostType);
				return true;
			}
		}
		return false;
	}

	const bool MatchingCore::_messageBettingMoneyRestore( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID ) 
	{
		const GLMSG::NET_BETTING_MONEY_OK_RESTORE_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_RESTORE_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);

		if(pCharacter == NULL)
			return false;

		pCharacter->AddInvenMoneyServer(_pMatchedGameMessage->Money, ADD_INVEN_MONEY_MATCHING_BETTING_RESTORE);
		pCharacter->SendInvenMoneyUpdate();
		//TODO 로그남겨야함

		GLMSG::NET_BETTING_MONEY_OK_RESTORE_FA Answer(_pMatchedGameMessage->dwDbNum,true);
		gpGaeaServer->SENDTOAGENT(&Answer);
		return true;

	}

	const bool MatchingCore::_messageBettingMoneyINCOMESpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		//돈주라
		const GLMSG::NET_BETTING_MONEY_OK_INCOME_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_INCOME_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);
		if ( pCharacter == NULL )
			return false;

		pCharacter->AddInvenMoneyServer(_pMatchedGameMessage->Money, ADD_INVEN_MONEY_MATCHING_BETTING_INCOME);
		pCharacter->SendInvenMoneyUpdate();
		//TODO 로그남겨야함

		GLMSG::NET_MATCHING_MONEY_FEE_OK_FA Answer(_pMatchedGameMessage->dwDbNum,true);
		gpGaeaServer->SENDTOAGENT(&Answer);
		return true;

	}

	const bool MatchingCore::_messageBettingMoneyFeeSpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MONEY_OK_SPEND_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_SPEND_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);
		if ( pCharacter == NULL )
			return false;

		if(_pMatchedGameMessage->Money > pCharacter->GetInvenMoney() )
		{
			//돈없다.
			GLMSG::NET_BETTING_MONEY_OK_SPEND_FA Answer(_pMatchedGameMessage->dwDbNum,false);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}
		else
		{
			//돈있다.
			pCharacter->SubtractInvenMoneyServer(_pMatchedGameMessage->Money, SUBTRACT_INVEN_MONEY_MATCHING_BETTING_COST);
			pCharacter->SendInvenMoneyUpdate();
			//TODO 로그남겨야함

			GLMSG::NET_BETTING_MONEY_OK_SPEND_FA Answer(_pMatchedGameMessage->dwDbNum,true);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}

		return false;
	}

	const bool MatchingCore::_messageTournamentMoneyFeeCheck(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MONEY_OK_SPEND_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_SPEND_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);
		if ( pCharacter == NULL )
			return false;

		if(_pMatchedGameMessage->Money > pCharacter->GetInvenMoney() )
		{
			//돈없다.
			GLMSG::NET_MATCHING_MONEY_FEE_OK_FA Answer(_pMatchedGameMessage->dwDbNum,false);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}
		else
		{
			//돈있다.
			pCharacter->SubtractInvenMoneyServer(_pMatchedGameMessage->Money, SUBTRACT_INVEN_MONEY_MATCHING_BETTING_COST);
			pCharacter->SendInvenMoneyUpdate();
			//TODO 로그남겨야함

			//돈있다.
			GLMSG::NET_MATCHING_MONEY_FEE_OK_FA Answer(_pMatchedGameMessage->dwDbNum,true);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}

		return false;
	}

	const bool MatchingCore::_messageTournamentMoneyFeeSpend(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_MONEY_FEE_SPEND_AF* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_MONEY_FEE_SPEND_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);
		if ( pCharacter == NULL )
			return false;

		if(_pMatchedGameMessage->Money > pCharacter->GetInvenMoney() )
		{
			//돈없다.
			GLMSG::NET_MATCHING_MONEY_FEE_SPEND_FA Answer(_pMatchedGameMessage->dwDbNum,false);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}
		else
		{
			//돈있다.
			pCharacter->SubtractInvenMoneyServer(_pMatchedGameMessage->Money, SUBTRACT_INVEN_MONEY_MATCHING_FEE);
			pCharacter->SendInvenMoneyUpdate();
			//TODO 로그남겨야함

			GLMSG::NET_MATCHING_MONEY_FEE_SPEND_FA Answer(_pMatchedGameMessage->dwDbNum,true);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}

		return false;
	}

	const bool MatchingCore::_messageBettingMoneyFeeCheck(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MONEY_OK_AF* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_AF* const)_pMessage);

		GLChar* pCharacter = gpGaeaServer->GetCharByDbNum(_pMatchedGameMessage->dwDbNum);
		if ( pCharacter == NULL )
			return false;

		if(_pMatchedGameMessage->Money > pCharacter->GetInvenMoney() )
		{
			//돈없다.
			GLMSG::NET_BETTING_MONEY_OK_FA Answer(_pMatchedGameMessage->dwDbNum,false);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}
		else
		{
			//돈있다.
			GLMSG::NET_BETTING_MONEY_OK_FA Answer(_pMatchedGameMessage->dwDbNum,true);
			gpGaeaServer->SENDTOAGENT(&Answer);
			return true;
		}


		//m_pServer->SENDTOAGENT()
		return false;
	}

	const bool MatchingCore::_messageJoinFieldMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_ADMISSION_CF* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_ADMISSION_CF* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == _pMatchedGameMessage->TornanmentID )
			{
				// 전장점수 시스템은 Agent에서 사용하라.

				land->JoinTournament(_pMatchedGameMessage->dwDbNum);
				return true;
			}
		}

		return false;
	}
}