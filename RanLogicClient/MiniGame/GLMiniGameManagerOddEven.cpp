#include "pch.h"

#include "../RanLogic/MiniGame/OddEven/GLOddEvenSettings.h"
#include "../RanLogic/Msg/GLContrlMiniGameMsg.h"
#include "./OddEven/GLOddEven.h"
#include "./GLMiniGameManager.h"

namespace MiniGame
{
	void GLManagerClient::doMessageRequestBegin(void)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_BEGIN());
	}
	void GLManagerClient::doMessageRequestSelectOddEven(const bool _bOdd)
	{
		MiniGame::EMODDEVEN _typeOddEven(_bOdd == true ? MiniGame::EMODDEVEN_ODD : MiniGame::EMODDEVEN_EVEN);		
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_ODDEVEN(_typeOddEven));
	}
	void GLManagerClient::doMessageRequestRoll(void)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_ROLL());
	}
	void GLManagerClient::doMessageRequestResult(void)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RESULT());
	}
	void GLManagerClient::doMessageRequestReward(void)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_REWARD());
	}
	void GLManagerClient::doMessageRequestSelectReward(const unsigned int _indexReward)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_SELECT_REWARD(_indexReward));
	}
	void GLManagerClient::doMessageRequestReceiveReward(void)
	{
		GLManagerClient::doMessage(&GLMSG::NET_MINIGAME_ODDEVEN_REQUEST_RECEIVE_REWARD());
	}
	const int GLManagerClient::getStageCurrent(void) const
	{
		if ( m_pMiniGame == NULL )
			return false;

		const MiniGame::GLOddEvenClient* const _pOddEven((const MiniGame::GLOddEvenClient* const)m_pMiniGame);
		return int(_pOddEven->getCurrentStage());
	}
	const int GLManagerClient::getStepCurrent(void) const
	{
		if ( m_pMiniGame == NULL )
			return false;

		const MiniGame::GLOddEvenClient* const _pOddEven((const MiniGame::GLOddEvenClient* const)m_pMiniGame);
		return int(_pOddEven->getCurrentStep());
	}
	const int GLManagerClient::getStepMaximum(void) const
	{
		MiniGame::OddEvenSettings* const _pSettingsOddEven(MiniGame::OddEvenSettings::getInstance());
		return _pSettingsOddEven->getMaxWin();
	}
	const int GLManagerClient::getMoneyCost(void) const
	{		
		if ( m_pMiniGame == NULL )
			return 0;

		const MiniGame::GLOddEvenClient* const _pOddEven((const MiniGame::GLOddEvenClient* const)m_pMiniGame);
		const DWORD _indexStep(_pOddEven->getCurrentStep());

		MiniGame::OddEvenSettings* const _pSettingsOddEven(MiniGame::OddEvenSettings::getInstance());
		const LONGLONG lnCostMoney(_pSettingsOddEven->getCostMoney(_indexStep));
		
		return lnCostMoney;
	}
	const DWORD GLManagerClient::getItemIDReward(const unsigned int _indexItemReward) const
	{
		if ( m_pMiniGame == NULL )
			return 0;

		const MiniGame::GLOddEvenClient* const _pOddEven((const MiniGame::GLOddEvenClient* const)m_pMiniGame);
		const SNATIVEID* const _pItemRewardID(_pOddEven->getDetermineItemID(_indexItemReward));
		if ( _pItemRewardID == 0 )
			return 0;

		return _pItemRewardID->dwID;
	}
	const bool GLManagerClient::isWin(void) const
	{
		if ( m_pMiniGame == NULL )
			return false;

		const MiniGame::GLOddEvenClient* const _pOddEven((const MiniGame::GLOddEvenClient* const)m_pMiniGame);
		if ( _pOddEven->getSelected() == _pOddEven->getResult() )
			return true;

		return false;
	}

	const float GLManagerClient::getTimeLimit() const
	{
		MiniGame::OddEvenSettings* const _pSettingsOddEven(MiniGame::OddEvenSettings::getInstance());
		return _pSettingsOddEven->getTimeLimit();
	}
}