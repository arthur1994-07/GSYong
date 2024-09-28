#include "pch.h"

#include <vector>
#include "GLOddEvenSettings.h"
#include "GLOddEven.h"

namespace MiniGame
{
	GLOddEven::GLOddEven(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID)
		: IMiniGame(dwGaeaID, dwMiniGameID, fBeginTime, NPCID, dwNPCGaeaID)
		, m_emCurrentStage(MiniGame::EMODDEVEN_STAGE_READY)
		, m_nCurrentStep(0)
		, m_emSelected(MiniGame::EMODDEVEN_NOT_SELECTED)
		, m_nSelectItemIndex(MiniGame::EMODDEVEN_MAX_REWARD)
	{
		m_nDetermineNumber[0] = 0;
		m_nDetermineNumber[1] = 0;

		for ( DWORD _i = MiniGame::EMODDEVEN_MAX_REWARD; _i; --_i )
			m_DetermineItemID[_i - 1] = SNATIVEID(false);
	}
	GLOddEven::~GLOddEven(void)
	{
	}

	const bool GLOddEven::nextStage(void)
	{
		++m_nCurrentStep;	
		
		const DWORD nMaxWin = MiniGame::OddEvenSettings::getInstance()->getMaxWin();
		if ( m_nCurrentStep > nMaxWin )
		{
			reset();
			return false;
		}

		m_nSelectItemIndex = MiniGame::EMODDEVEN_MAX_REWARD;
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_READY;
		m_emSelected = MiniGame::EMODDEVEN_NOT_SELECTED;
		m_emResult = MiniGame::EMODDEVEN_NOT_SELECTED;

		return true;
	}

	const bool GLOddEven::reset(void)
	{
		m_nCurrentStep = 0;
		m_nSelectItemIndex = MiniGame::EMODDEVEN_MAX_REWARD;
		m_emCurrentStage = MiniGame::EMODDEVEN_STAGE_READY;
		m_emSelected = MiniGame::EMODDEVEN_NOT_SELECTED;
		m_emResult = MiniGame::EMODDEVEN_NOT_SELECTED;

		return true;
	}

	const MiniGame::EMODDEVEN_STAGE GLOddEven::getCurrentStage(void) const
	{
		return m_emCurrentStage;
	}
	const MiniGame::EMODDEVEN GLOddEven::getSelected(void) const
	{
		return m_emSelected;
	}
	const DWORD GLOddEven::getCurrentStep(void) const
	{
		return m_nCurrentStep;
	}
	const DWORD GLOddEven::getSelectItemIndex(void) const
	{
		return m_nSelectItemIndex;
	}

	const SNATIVEID* const GLOddEven::getDetermineItemID(const DWORD nIndex) const
	{
		if ( nIndex >= MiniGame::EMODDEVEN_MAX_REWARD )
			return NULL;

		return &m_DetermineItemID[nIndex];
	}
	const DWORD* const GLOddEven::getDetermineNumber(void) const
	{
		return m_nDetermineNumber;
	}
}


