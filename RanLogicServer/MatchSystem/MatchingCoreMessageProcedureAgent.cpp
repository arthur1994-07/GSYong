#include "pch.h"
#include "./MatchingManager.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlMatching.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../RanLogic/Tournament/GLGroupManager.h"
#include "../Database/DBAction/DbActionGameCompetition.h"
#include "../Database/DBAction/DbActionGameBetting.h"
#include "../../RanLogicServer/Character/GLChar.h"

#include "../Database/DBAction/DbActionLogInstantDungeon.h"


namespace MatchingSystem
{
	const bool MatchingCore::messageProcedureMsgPack(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		NET_MSG_PACK* pPacket = (NET_MSG_PACK*) _pMessage;
		GLMSG::NET_MATCHING_MSGPACK_MESSAGE _pMatchingMessage;
		if ( CastNetMsg(_pMessage, _pMatchingMessage) == false )
			return false;

		switch ( _pMatchingMessage.emType )
		{
		case EMMESSAGE_CAPTURE_THE_FLAG_RESULT_FA:
			return _messageCaptureTheFlagResult(_pMessage, _dwClientID, _dwGaeaID);
		}

		return false;
	}

	const bool MatchingCore::_messageCaptureTheFlagResult(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		GLMSG::NET_MATCHING_CAPTURE_THE_FLAG_RESULT_FA _msg;
		if ( CastNetMsg(_pMessage, _msg) == false )
			return false;

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->CaptureTheFlag_Result(_msg) == true )
				return true;
		}

		return false;
	}







	const bool MatchingCore::messageProcedureAgent(const NET_MSG_GENERIC* _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		if ( GLUseFeatures::GetInstance().IsUsingTournament() == false)
			return true;		

		const GLMSG::NET_MATCHING_MESSAGE* const _pMatchingMessage((const GLMSG::NET_MATCHING_MESSAGE* const)_pMessage);

		switch(_pMatchingMessage->emType)
		{
		case EMMESSAGE_MATCHING_REQUIRE_NOTIFY_AT_FIRST_CA:
			return _messageMatchedReqInfomation(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_ADMISSION_FA:
			return _messageJoinMatchedGame(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_CANCEL_CA:
			return _messageCanselMatchedGame(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_OBSERVE_ADMISSION_CA:
			return _messageObserveMatchedGame(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_OBSERVE_OUT_ADMISSION_AC:
			return _messageObserveOut(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_GM_COMMAND_CA:
			return _messageGMCommand(_pMatchingMessage, _dwClientID, _dwGaeaID);
			///인던 완료 메시지
		case EMMESSAGE_MATCHING_RESUALT_SEND_START_FA:
			return _messageResaultStart(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_RESUALT_SEND_END_FA:
			return _messageResaultEnd(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATHCING_RESUALT_SCORE_FA:
			return _messageResaultScore(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_RESUALT_FACTION_RESUALT:
			return _messageResaultFactionResualt(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_RESULT_CUSTOM_MSG_FA:
			return _messageResultCustomMsg(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_RESUALT_ERROR_FA:
			return _messageResaultError(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_NOTIFY_RUOK_CA:
			return _messageAnswerRUOK(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_GROUPTABLEINFO_REQ_CA:
			return _messageGroupTableInfpReq(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_MONEY_FEE_OK_FA:
			return _messageTournamentMoneyFeeCheckANS(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_MONEY_FEE_SPEND_FA:
			return _messageTournamentMoneyFeeSpendANS(_pMatchingMessage, _dwClientID, _dwGaeaID);
			//베팅 메시지
		case EMMESSAGE_BETTING_MONEY_OK_FA:
			return _messageBettingMoneyFeeCheckANS(_pMatchingMessage, _dwClientID, _dwGaeaID); //NET_MATCHING_MONEY_FEE_OK_FA
		case EMMESSAGE_BETTING_MONEY_OK_SPEND_FA:
			return _messageBettingMoneyFeeSpendANS(_pMatchingMessage, _dwClientID, _dwGaeaID); //NET_MATCHING_MONEY_FEE_OK_FA
		case EMMESSAGE_BETTING_GROUP_EPLOYEE_INFO_CA:
			return _messageBettingGroupEployeeInfoANS(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MY_BETTING_INFO_CA:
			return _messageBettingMyBettingInfo(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_GROUPINFO_CA:
			return _messageBettingGroupInfo(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_BETTINGINFO_CA:
			return _messageBettingBettingInfo(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_APPLY_CA:
			return _messageBettingApply(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_CANSEL_CA:
			return _messageCanselBetting(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_BETTING_MONEY_OK_RESTORE_FA:
			return _messageRestoreBetting(_pMatchingMessage, _dwClientID, _dwGaeaID);

			//전장 점수
		case EMMESSAGE_COMPETITION_RATING_GETINFO_DA:
			return _messageCompetitionRatingGetInfoDF(_pMatchingMessage, _dwClientID, _dwGaeaID);

		case EMMESSAGE_MATCHING_INFO_UPDATE:
			return _messageFieldMatchingInfoUpdate(_pMatchingMessage, _dwClientID, _dwGaeaID);
		case EMMESSAGE_MATCHING_CUSTOM_MSG_CA:
			return _messageRoomCustomMessage(_pMatchingMessage, _dwClientID, _dwGaeaID);
		}

		return true;
	}



	const bool MatchingCore::_messageObserveOut( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID) 
	{
		const GLMSG::NET_MATCHING_OBSERVE_OUT_ADMISSION_AC* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_OBSERVE_OUT_ADMISSION_AC* const)_pMessage);


		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->FieldMatchingObserveOut(_pMatchedGameMessage->RequestID,_pMatchedGameMessage->TornanmentMapID);
		}



		return true;
	}


	const bool MatchingCore::_messageFieldMatchingInfoUpdate(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_INFO_UPDATE* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_INFO_UPDATE* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == _pMatchedGameMessage->ScriptID )
			{
				// 전장점수 시스템은 Agent에서 사용하라.

				land->FieldMatchingStatusChaged(_pMatchedGameMessage->MapStatus);
				return true;
			}
		}

		return false;
	}

	const bool MatchingCore::_messageCanselMatchedGame(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_CANCEL_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_CANCEL_CA* const)_pMessage);
		//사용하지 않음

		return false;
	}

	const bool MatchingCore::_messageBettingMyBettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MY_BETTING_INFO_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MY_BETTING_INFO_CA* const)_pMessage);
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->RequestMyBettingInfo(_pMatchedGameMessage->dwDbNum);
		}
		return false;
	}
	const bool MatchingCore::_messageBettingGroupInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_GROUPINFO_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_GROUPINFO_CA* const)_pMessage);
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->RequestGroupInfo(_pMatchedGameMessage->dwDbNum);
		}
		return false;
	}
	const bool MatchingCore::_messageBettingBettingInfo(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_BETTINGINFO_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_BETTINGINFO_CA* const)_pMessage);
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->SendTournamentBettingRaito(_pMatchedGameMessage->ClientDbid,_pMatchedGameMessage->LeageId);
		}
		return false;
	}

	const bool MatchingCore::_messageBettingApply(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_APPLY_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_APPLY_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->SetBetting(_pMatchedGameMessage->ClientDbid,
				_pMatchedGameMessage->bettingGroup,_pMatchedGameMessage->BettingCount,
				_pMatchedGameMessage->LeageId);
		}

		return false;
	}



	const bool MatchingCore::_messageBettingGroupEployeeInfoANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_GROUP_EPLOYEE_INFO_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_GROUP_EPLOYEE_INFO_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->BettingGroupEployeeInfo(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->dwGroupID);
		}

		return false;
	}

	const bool MatchingCore::_messageBettingMoneyFeeCheckANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MONEY_OK_FA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->FieldCheckMoneyOK(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->ISGOOD);
		}

		return false;
	}

	//토너먼트 베팅 요금 처리.
	const bool MatchingCore::_messageTournamentMoneyFeeSpendANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_BETTING_MONEY_OK_SPEND_FA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_MONEY_OK_SPEND_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->ClientMoneySpendCallBack(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->ISGOOD);
		}

		return false;
	}

	const bool MatchingCore::_messageTournamentMoneyFeeCheckANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_MONEY_FEE_OK_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_MONEY_FEE_OK_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->ClientMoneyCheckCallBack(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->ISGOOD);
		}

		return false;
	}

	const bool MatchingCore::_messageBettingMoneyFeeSpendANS(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_MONEY_FEE_SPEND_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_MONEY_FEE_SPEND_FA* const)_pMessage);
		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			_betting->FieldSpendMoneyOK(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->ISGOOD);
		}
		return false;
	}

	const bool MatchingCore::_messageGroupTableInfpReq(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_GROUPTABLEINFO_REQ_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_GROUPTABLEINFO_REQ_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(_pMatchedGameMessage->dwTornanmentID == land->GetScriptID() )
				land->RequireMatchingData(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->dwTornanmentID);
		}
		return false;
	}


	const bool MatchingCore::_messageAnswerRUOK(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_NOTIFY_RUOK_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_NOTIFY_RUOK_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(land->AnswerRUOK(_pMatchedGameMessage->dwDbNum,_pMatchedGameMessage->dwGameScriptID,_pMatchedGameMessage->isImok))
				return true;
		}
		return false;
	}

	const bool MatchingCore::_messageResaultStart(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESUALT_SEND_START_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESUALT_SEND_START_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(land->NoticeMatchedGameSendStart(_pMatchedGameMessage->TornanmentMapID))
				return true;
		}

		return true;
	}
	const bool MatchingCore::_messageResaultEnd(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESUALT_SEND_END_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESUALT_SEND_END_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(land->NoticeMatchedGameSendEnd(_pMatchedGameMessage->TornanmentMapID))
				return true;
		}

		return true;
	}
	const bool MatchingCore::_messageResaultScore(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESUALT_SCORE_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESUALT_SCORE_FA* const)_pMessage);

		if(_pMatchedGameMessage->LuaType == LUA_TNUMBER)
		{
			BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
			{

				if(land->NoticeMatchedGameSenddata(_pMatchedGameMessage->TornanmentMapID,
					_pMatchedGameMessage->szluavarName,_pMatchedGameMessage->luavaluefloat))
					return true;
			}
		}
		else if(_pMatchedGameMessage->LuaType == LUA_TSTRING)
		{
			BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
			{

				if(land->NoticeMatchedGameSenddata(_pMatchedGameMessage->TornanmentMapID,
					_pMatchedGameMessage->szluavarName,_pMatchedGameMessage->LuavalueChar))
					return true;
			}
		}
		else if(_pMatchedGameMessage->LuaType == LUA_TBOOLEAN)
		{
			BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
			{

				if(land->NoticeMatchedGameSenddata(_pMatchedGameMessage->TornanmentMapID,
					_pMatchedGameMessage->szluavarName,_pMatchedGameMessage->luavaluebool))
					return true;
			}
		}


		return true;
	}
	const bool MatchingCore::_messageResaultFactionResualt(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESUALT_FACTION_RESUALT_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESUALT_FACTION_RESUALT_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->NoticeMatchedGameFinish(_pMatchedGameMessage->TornanmentMapID,_pMatchedGameMessage->m_FactionID,_pMatchedGameMessage->IsWinner);
		}

		return true;
	}
	const bool MatchingCore::_messageResultCustomMsg(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESULT_CUSTOM_MSG_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESULT_CUSTOM_MSG_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			land->NoticeMatchedGameCustomMSG(_pMatchedGameMessage->TornanmentMapID,
				_pMatchedGameMessage->param1, _pMatchedGameMessage->param2, _pMatchedGameMessage->param3, _pMatchedGameMessage->param4);
		}

		return true;
	}
	const bool MatchingCore::_messageResaultError(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_RESUALT_ERROR_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_RESUALT_ERROR_FA* const)_pMessage);
		return true;
	}

	const bool MatchingCore::_messageGMCommand( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_GM_COMMAND_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_GM_COMMAND_CA* const)_pMessage);
		switch(_pMatchedGameMessage->mstype)
		{
		case EM_GM_MATCHING_RELOAD:
			if ( _pMatchedGameMessage->Param1 == 0 )
			{
				OnReload();
			}
			else
			{
				OnReload(_pMatchedGameMessage->Param1);
			}

			if ( gpAgentServer )
			{
				gpAgentServer->SENDTOALLCHANNEL((NET_MSG_GENERIC*)_pMatchedGameMessage);
			}
			return true;
		case EM_GM_MATCHING_FORCE_START:
			OnForceStart(_pMatchedGameMessage->Param1);
			return true;
		case EM_GM_MATCHING_COUNT_PACKET:
			{
				const unsigned int _sizePacket(this->packetCounter.size());
				GLMSG::NET_INSTANCE_NOTIFY_COUNT_PACKET _notifyCountPacket;
				for ( unsigned int _index(0); _index < _sizePacket; ++_index )
				{
					const unsigned int _countPacket(this->packetCounter[_index]);
					if ( _notifyCountPacket.push_back(_index, _countPacket) == false )
					{
						gpAgentServer->SENDTOCLIENT(_dwClientID, &_notifyCountPacket);
						_notifyCountPacket.clear();
					}
				}

				if ( _notifyCountPacket.isValid() == true )
					gpAgentServer->SENDTOCLIENT(_dwClientID, &_notifyCountPacket);

			}
			return true;
		}

		return false;
	}

	const bool MatchingCore::_messageObserveMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_OBSERVE_ADMISSION_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_OBSERVE_ADMISSION_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if(land->GetScriptID() == _pMatchedGameMessage->ScriptID)
			{
				land->getRoomManager()->RequestObserve(_pMatchedGameMessage->GroupID,_pMatchedGameMessage->RequestID,false);
				return true;
			}
		}
		sc::writeLogInfo("Observe Matching::Invaild Script ID");
		return false;

	}
	
	const bool MatchingCore::_messageJoinMatchedGame( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_ADMISSION_FA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_ADMISSION_FA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == _pMatchedGameMessage->TornanmentID )
			{
				// 전장점수 시스템을 사용하면서 전장점수가 로드되어있지 않은경우 전장점수를 DB에서 로드한후 입장;
				const bool bUseRating(land->isUseCompetitionRating());
				const bool bExsistRating(m_pRatingManager->GetRatingInfo(_pMatchedGameMessage->dwDbNum, land->getCompetitionContentType()));
				if ( bUseRating && (bExsistRating == false) )
				{
					m_pRatingManager->LoadRatingInfo(_pMatchedGameMessage->dwDbNum, land->getCompetitionContentType(), land->GetScriptID());
				}
				else
				{
					land->JoinTournament(_pMatchedGameMessage->dwDbNum);
				}
				return true;
			}
		}

		return false;
	}
	
	const bool MatchingCore::_messageMatchedReqInfomation( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_MESSAGE_INFO_REQ_CA* const
			_pMatchedGameMessage((const GLMSG::NET_MATCHING_MESSAGE_INFO_REQ_CA* const)_pMessage);
		MatchingInfoamtionSend(_pMatchedGameMessage->dwDbNum);
		return true;
	}

	const bool MatchingCore::_messageCompetitionRatingGetInfoDF(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_COMPETITION_RATING_GETINFO_DA* const
			pMsg = (const GLMSG::NET_MATCHING_COMPETITION_RATING_GETINFO_DA*)_pMessage;
		m_pRatingManager->LoadRatingInfoFB(_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == pMsg->dwScriptID )
			{
				land->JoinTournament(pMsg->dwCharNum);
				return true;
			}
		}

		return false;
	}

	const bool MatchingCore::_messageRoomCustomMessage(const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_MATCHING_CUSTOM_MSG_CA* const 
			pMsg = (const GLMSG::NET_MATCHING_CUSTOM_MSG_CA*)_pMessage;

		GLCharAG* pCharAG = gpAgentServer->GetCharByGaeaID(_dwGaeaID);                
		if ( pCharAG == NULL )
			return false;

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			if ( land->GetScriptID() == pMsg->dwGameScriptID )
			{
				land->EventCustomMessage(pCharAG->ChaDbNum(), pMsg->dwParam1, pMsg->dwParam2);
				return true;
			}
		}
		return false;
	}

	const bool MatchingCore::_messageCanselBetting( const GLMSG::NET_MATCHING_MESSAGE* const _pMessage, const DWORD _dwClientID, const DWORD _dwGaeaID )
	{
		const GLMSG::NET_BETTING_CANSEL_CA* const
			_pMatchedGameMessage((const GLMSG::NET_BETTING_CANSEL_CA* const)_pMessage);

		BOOST_FOREACH(MatchingGameBase* land,m_matchingLandlist)
		{
			GLTournamentBetting* _betting = land->getBettingManager();
			if(_betting == NULL)
				continue;
			if(land->GetScriptID() == _pMatchedGameMessage->LeageId)
			{
				if(_betting->CanselBetting(_pMatchedGameMessage->ClientDbid) )
				{
					// 토너먼트 로그
					GLCharAG* const pChar(gpAgentServer->GetCharByDbNum(_pMatchedGameMessage->ClientDbid));
					if ( pChar == 0 )
						break;

					db::LogTournament* pDbAction = new db::LogTournament(9102);

					pDbAction->dwCharDbNum =  _pMatchedGameMessage->ClientDbid;   
					pDbAction->strChaName = pChar->GetChaName();

					gpAgentServer->AddLogAdoJob( db::DbActionPtr(pDbAction) );
				}

				break;
			}
		}

		return false;
	}
}