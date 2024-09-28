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
		 * nextStage()�� ��� reset()�� �����ϰ� �ʱ� ���·� �ǵ�����;
		 * ���� Ƚ���� ���ؼ����� �ʱ�ȭ ���� ����;
		 */
		const bool nextStage(void);
		const bool reset(void);

	public:
		GLOddEven(const DWORD dwGaeaID, const DWORD dwMiniGameID, const float fBeginTime, const SNATIVEID& NPCID, const DWORD dwNPCGaeaID);
		virtual ~GLOddEven(void);

	protected:
		EMODDEVEN_STAGE m_emCurrentStage; // ���� �ܰ�;
		DWORD m_nCurrentStep; // ���� �ܰ�;

		DWORD m_nDetermineNumber[MiniGame::EMODDEVEN_MAX_DICE]; // ������ ��ȣ;
		MiniGame::EMODDEVEN m_emSelected; // ���õ� ¦��/Ȧ��;
		MiniGame::EMODDEVEN m_emResult; // ���;

		DWORD m_nSelectItemIndex; // ������ ������ �ε���;

		SNATIVEID m_DetermineItemID[MiniGame::EMODDEVEN_MAX_REWARD];
	};
}

