#pragma once

#include "GLOddEvenDefine.h"

namespace MiniGame
{
	class GLOddEven : public IMiniGame
	{
	public:
		const MiniGame::EMODDEVEN_STAGE getCurrentStage(void) const;
		const MiniGame::EMODDEVEN getSelected(void) const;		
		const DWORD getCurrentStep(void) const;
		const DWORD getSelectItemIndex(void) const;
		const DWORD* const getDetermineNumber(void) const;
		const SNATIVEID* const getDetermineItemID(const DWORD nIndex) const;

		/*
		 * nextStage()의 경우 reset()과 동일하게 초기 상태로 되돌리나;
		 * 연승 횟수에 대해서만은 초기화 하지 않음;
		 */
		const bool nextStage(void);
		const bool reset(void);

	public:
		GLOddEven(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		virtual ~GLOddEven(void);

	protected:
		EMODDEVEN_STAGE m_emCurrentStage; // 진행 단계;
		DWORD m_nCurrentStep; // 연승 단계;

		DWORD m_nDetermineNumber[MiniGame::EMODDEVEN_MAX_DICE]; // 결정된 번호;
		MiniGame::EMODDEVEN m_emSelected; // 선택된 짝수/홀수;
		MiniGame::EMODDEVEN m_emResult; // 결과;

		DWORD m_nSelectItemIndex; // 선택한 보상의 인덱스;

		SNATIVEID m_DetermineItemID[MiniGame::EMODDEVEN_MAX_REWARD];
	};
}

