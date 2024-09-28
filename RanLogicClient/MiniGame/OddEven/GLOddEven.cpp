#include "pch.h"

#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "../../../RanLogic/MiniGame/OddEven/GLOddEvenSettings.h"
#include "../../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../Land/GLLandManClient.h"
#include "../../Char/GLCharacter.h"
#include "../../GLGaeaClient.h"
#include "GLOddEven.h"

namespace MiniGame
{
	GLOddEvenClient::GLOddEvenClient(GLGaeaClient* const pGaeaClient, const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
		: MiniGame::GLOddEven(dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID)
		, m_pGaeaClient(pGaeaClient)
	{
	}
	GLOddEvenClient::~GLOddEvenClient(void)
	{
	}

	const MiniGame::EMODDEVEN GLOddEvenClient::getResult(void) const
	{
		return m_emResult;
	}

	const bool GLOddEvenClient::_setDetermineNumber(const DWORD* const nDetermineNumber)
	{
		if ( m_emSelected != MiniGame::EMODDEVEN_ODD &&
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return false; // 유효한 값이 선택 되 있지 않다면 실패;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT &&
			m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_ROLL )
			return false; // 유효한 스테이지에서의 요청이 아니라면 실패;

		DWORD nSumNumber = 0;
		for ( DWORD _i = MiniGame::EMODDEVEN_MAX_DICE; _i; --_i )
		{
			const DWORD nNumber = nDetermineNumber[_i - 1];
			if ( nNumber - 1 > 5 )
				return false; // nNumber가 1~6사이가 아니면 실패;

			m_nDetermineNumber[_i - 1] = nNumber;
			nSumNumber += nNumber;
		}

		m_emResult = nSumNumber & 0x1 ? MiniGame::EMODDEVEN_ODD : MiniGame::EMODDEVEN_EVEN;

		return true;
	}


	void GLOddEvenClient::FrameMove(const float fTime, const float fElapsedTime)
	{
	}


	/*
	 * 아래, 참조 값이 없는 메세지임에도 불구하고,
	 * 굳이 형변환을 해서 작성 해 놓은 이유는 디버깅시 참조 검색을 하기 위함;
	 * release 모드에서는 컴파일러가 코드를 최적화 하므로 성능상의 불이익은 없음;
	*/

	const bool GLOddEvenClient::_doMessageRequestBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN* const pRequestBeginMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_READY )
			return true;	// 중복되어 올 수있다;

		MiniGame::OddEvenSettings* const pOddEvenSettings = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEvenSettings->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		GLCharacter* const pPlayer = m_pGaeaClient->GetCharacter();
		const LONGLONG& lnMoney = pPlayer->GetInvenMoney();
		if ( pOddEvenSettings->getRequireMoney() > lnMoney )
		{ // 기본 요구 금액보다 가진 돈이 적으면 실패;
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::DISABLE,
				std::string(ID2GAMEINTEXT("MINIGAME_ODDEVEN_NOT_ENOUGH_MONEY")));	
			return false;
		}
		if ( pOddEvenSettings->getCostMoney(m_nCurrentStep) > lnMoney )
		{ // 스테이지 요구 금액보다 가진 돈이 적으면 실패;
			// true를 반환 하면 꺼지지는 않는다;
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::DISABLE,
				std::string(ID2GAMEINTEXT("MINIGAME_ODDEVEN_NOT_ENOUGH_MONEY")));	
			return false;
		}

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN());

		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN* const pRequestBeginMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )
			return true;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return true; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN(pRequestBeginMessage->emOddEven));

		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{ // 기획팀 요청으로 사용되지 않음;
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL* const pRequestBeginMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )
			return false; // 홀/짝 선택 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( m_emSelected != MiniGame::EMODDEVEN_ODD && 
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return true; // 홀/짝 선택이 되어 있지 않으면 보류;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL(m_emSelected, m_nCurrentStep));

		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT* const pRequestBeginMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_ROLL )
			return false; // 굴리기 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( m_emSelected != MiniGame::EMODDEVEN_ODD && 
			m_emSelected != MiniGame::EMODDEVEN_EVEN )
			return false; // 홀/짝 선택이 되어 있지 않으면 실패;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT());

		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD* const pRequestRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RESULT )
			return false; // 결과 보기 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( m_emResult != MiniGame::EMODDEVEN_ODD && 
			m_emResult !=  MiniGame::EMODDEVEN_EVEN )
			return false; // 결과 값이 유효하지 않으면 실패;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD(m_emResult));
		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD* const pRequestSelectRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD* const)pMessage;

		if ( m_emCurrentStage == MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD )
			return true;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT_REWARD )
			return false; // 보상 선택 단계가 아니면 실패;		

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;
		
		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD(pRequestSelectRewardMessage->nItemIndex));
		return true;
	}

	const bool GLOddEvenClient::_doMessageRequestReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD* const pRequestReceiveReward =
			(const GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD )
			return false; // 보상 받기 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD());
		return true;
	}	


	const bool GLOddEvenClient::_doMessageSettings(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_SETTINGS* const pSettingsMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_SETTINGS* const)pMessage;

		MiniGame::OddEvenSettings* const pOddEvenSettings = MiniGame::OddEvenSettings::getInstance();

		pOddEvenSettings->setMaxWin(pSettingsMessage->nMaxWin);
		pOddEvenSettings->setRequireLevel(pSettingsMessage->nRequireLevel);
		pOddEvenSettings->setRequireMoney(pSettingsMessage->nRequireMoney);
		pOddEvenSettings->setTimeLimit(pSettingsMessage->fTimeLimit);
		pOddEvenSettings->setAnimateTime(pSettingsMessage->fAnimateTime);
		pOddEvenSettings->setAnimateIdleTime(pSettingsMessage->fAnimateIdleTime);
		pOddEvenSettings->setLoseReward(pSettingsMessage->bLoseReward);

		pOddEvenSettings->setCostMoney(pSettingsMessage->nMaxWin, pSettingsMessage->nCostMoney);

		GLCharacter* const pPlayer = m_pGaeaClient->GetCharacter();
		const LONGLONG& lnMoney = pPlayer->GetInvenMoney();
		if ( pSettingsMessage->nRequireMoney > lnMoney )
		{ // 기본 요구 금액보다 가진 돈이 적으면 실패;
			return false;
		}

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoBegin(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_BEGIN* const pDoBeginMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_BEGIN* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_READY )
			return false; // 준비 단계가 아니면 실패;

		MiniGame::OddEvenSettings* const pOddEvenSettings = MiniGame::OddEvenSettings::getInstance();	

		if ( m_nCurrentStep >= pOddEvenSettings->getMaxWin() )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( pDoBeginMessage->nCurrentStep != m_nCurrentStep )
			return false; // 클라이언트의 정보와 서버에서 보낸정보가 맞지 않으면 실패;

		if ( pDoBeginMessage->nCostMoney != pOddEvenSettings->getCostMoney(m_nCurrentStep) )
			return false; // 클래이언트의 소모 금액 정보와 서버에서 보낸 정보가 맞지 않으면 실패;
			

		GLCharacter* const pPlayer = m_pGaeaClient->GetCharacter();		
		pPlayer->SubtractInvenMoney(pDoBeginMessage->nCostMoney);
		pOddEvenSettings->getLoseReward();
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_SELECT;
		m_emSelected = MiniGame::EMODDEVEN_NOT_SELECTED;

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Turn_Stage );

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_Inventory_UpdateMoney,
			"-l", pPlayer->GetInvenMoney() );

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoSelectOddEven(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN* const pDoSelectOddEvenMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_ODDEVEN* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )
			return false; // 홀/짝 선택 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		m_emSelected = pDoSelectOddEvenMessage->emOddEven;

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoRoll(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_ROLL* const pDoRollMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_ROLL* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT )
			return false; // 홀/짝 선택 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( _setDetermineNumber(pDoRollMessage->nDetermineNumber) == false )
			return false;

		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_ROLL;

		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Turn_Stage);

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoResult(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_RESULT* const pDoResultMessage = 
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_RESULT* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_ROLL )
			return false; // 굴리기 단계가 아니면 실패;

		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		if ( m_emSelected != pDoResultMessage->emSelected )
			return false; // 서버의 선택값과 클라이언트의 선택 값이 일치 하지 않다면 실패;

		if ( m_emResult != pDoResultMessage->emResult )
			return false;

		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_RESULT;

		m_pGaeaClient->NETSENDTOFIELD(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD(m_emResult));

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_REWARD* const pDoRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_REWARD* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RESULT )
			return true;

		MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEven->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;
		
		const bool bWin = m_emResult == m_emSelected;		
		const bool bLoseReward = pOddEven->getLoseReward();
		const MiniGame::EMODDEVEN_STAGE emNextStage = ((bWin || bLoseReward) ? MiniGame::EMODDEVEN_STAGE_SELECT_REWARD : MiniGame::EMODDEVEN_STAGE_READY);
		if ( pDoRewardMessage->emNextStage != emNextStage )
			return false; // 서버의 결과와 같지 않으면 실패;
		
		m_emCurrentStage = emNextStage;
		switch ( emNextStage )
		{
		case MiniGame::EMODDEVEN_STAGE_READY:
			GLOddEven::reset();
			break;		
		}	

		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Turn_Stage);

		return true;
	}

	const bool GLOddEvenClient::_doMessageDoSelectReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD* const pDoSelectRewardMessage =
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_SELECT_REWARD* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_SELECT_REWARD )
			return false;

		MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEven->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;		

		const bool bWin = m_emResult == m_emSelected;
		const bool bLoseReward = pOddEven->getLoseReward();
		if ( (bWin == false) && (bLoseReward == false) )
			return false;

		const DWORD nStage = ((bWin == true) ? m_nCurrentStep : MiniGame::EMODDEVEN_REWARD_FAIL);
		if ( nStage != pDoSelectRewardMessage->nStage )
			return false; // 서버의 정보와 맞지 않으면 실패;		

		m_nSelectItemIndex = pDoSelectRewardMessage->nItemIndex;
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD;
		for ( DWORD _i = MiniGame::EMODDEVEN_MAX_REWARD; _i; --_i )
			m_DetermineItemID[_i - 1] = pDoSelectRewardMessage->ItemID[_i - 1];

		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Turn_Stage);

		return true;
	}
	
	const bool GLOddEvenClient::_doMessageDoReceiveReward(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage)
	{
		const GLMSG::NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD* const pDoReceiveReward =
			(const GLMSG::NET_MINIGAME_ODDEVEN_DO_RECEIVE_REWARD* const)pMessage;

		if ( m_emCurrentStage != MiniGame::EMODDEVEN_STAGE_RECEIVE_REWARD )
			return false;

		MiniGame::OddEvenSettings* const pOddEven = MiniGame::OddEvenSettings::getInstance();
		const DWORD nMaxWin = pOddEven->getMaxWin();
		if ( m_nCurrentStep >= nMaxWin )
			return false; // 이미 연승 횟수가 지정한 횟수에 다다라 있으면 실패;

		const bool bWin = m_emResult == m_emSelected;
			
		const bool bResult = bWin ? nextStage() : reset();
		if ( bResult == false )
			return false;

		GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_MiniGame_Dice_Turn_Stage);

		return true;
	}

	const bool GLOddEvenClient::doBegin(void)
	{		
		return true;
	}

	const bool GLOddEvenClient::doEnd(void)
	{
		return true;
	}

	const bool GLOddEvenClient::doMessage(const GLMSG::NET_MINIGAME_MESSAGE* const pMessage, const DWORD)
	{
		GLCharacter* const pCharacter = m_pGaeaClient->GetCharacter();
		if ( pCharacter->UserLevel() < USER_GM3 )
		{ // GM이 아닌 경우 해당 NPC가 존재 하고, 반경 내에 있는지 검사;
			GLLandManClient* const pLandManager = m_pGaeaClient->GetActiveMap();
			if ( pLandManager == NULL )
				return false;

			const std::tr1::shared_ptr<GLCrowClient> pNPC = pLandManager->GetCrow(m_dwNPCGaeaID);
			if ( pNPC == NULL )
				return false;
			
            if ( !pNPC->IsNpcActionable(pCharacter->GetPosition(), pCharacter->GETBODYRADIUS()) )
                return false;
		}

		switch ( pMessage->nSubType )
		{
		case MiniGame::EMMESSAGE_ODDEVEN_SETTINGS:
			return _doMessageSettings(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_BEGIN:
			return _doMessageRequestBegin(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_ODDEVEN:
			return _doMessageRequestSelectOddEven(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_ROLL:
			return _doMessageRequestRoll(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RESULT:
			return _doMessageRequestResult(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_REWARD:
			return _doMessageRequestReward(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_SELECT_REWARD:
			return _doMessageRequestSelectReward(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_REQUEST_RECEIVE_REWARD:
			return _doMessageRequestReceiveReward(pMessage);

		case MiniGame::EMMESSAGE_ODDEVEN_DO_BEGIN:
			return _doMessageDoBegin(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_SELECT_ODDEVEN:
			return _doMessageDoSelectOddEven(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_ROLL:
			return _doMessageDoRoll(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_RESULT:
			return _doMessageDoResult(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_REWARD:
			return _doMessageDoReward(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_SELECT_REWARD:
			return _doMessageDoSelectReward(pMessage);
		case MiniGame::EMMESSAGE_ODDEVEN_DO_RECEIVE_REWARD:
			return _doMessageDoReceiveReward(pMessage);
		}

		return false;
	}
}
