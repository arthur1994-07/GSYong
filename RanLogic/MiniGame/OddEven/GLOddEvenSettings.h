#pragma once

#include "GLOddEvenDefine.h"

namespace MiniGame
{
	namespace OddEven
	{
		extern HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad);
	}

	class OddEvenSettings
	{
	public:
		__forceinline const DWORD getMaxWin(void) const;
		__forceinline const DWORD getRequireLevel(void) const;
		__forceinline const DWORD getRequireMoney(void) const;
		__forceinline const float getTimeLimit(void) const;
		__forceinline const float getAnimateTime(void) const;
		__forceinline const float getAnimateIdleTime(void) const;
		__forceinline const bool getLoseReward(void) const;

		__forceinline void setMaxWin(const DWORD nMaxWin);
		__forceinline void setRequireLevel(const DWORD nRequireLevel);
		__forceinline void setRequireMoney(const DWORD nRequireMoney);
		__forceinline void setTimeLimit(const float fTimeLimit);
		__forceinline void setAnimateTime(const float fAnimateTime);
		__forceinline void setAnimateIdleTime(const float fAnimateIdleTime);
		__forceinline void setLoseReward(const bool bLoseReward);

		void setCostMoney(const DWORD nMaxWin, const DWORD* const nCostMoney);
		const bool getTakeItem(const DWORD nStageIndex, const DWORD nSelectIndex, SNATIVEID pItemID[MiniGame::EMODDEVEN_MAX_REWARD]) const;
		const DWORD getSuccessRate(const DWORD nStageIndex) const;
		const LONGLONG getCostMoney(const DWORD nStageIndex) const;

	private:
		const bool push(const DWORD nSuccessRate, const DWORD nTakeRate, const DWORD nCostMoney);
		const bool push(const DWORD nStageIndex, const DWORD nTakeRate, const SNATIVEID& ItemID);
		void doComplete(void);

	public:
		static OddEvenSettings* const getInstance(void);
		~OddEvenSettings(void);

	private:
		OddEvenSettings(void);

	private:
		friend HRESULT MiniGame::OddEven::LoadFile(const std::string& _FileName, const bool _bPastLoad);

		DWORD m_nRequireLevel; // �÷��� �ּ� �䱸 ����;
		DWORD m_nRequireMoney; // �÷��� �ּ� �䱸 ���ӸӴ�;
		DWORD m_nMaxWin; // �ִ� ���� �ܰ�;
		float m_fTimeLimit; // ���� ���� ���� �ð�;
		float m_fAnimateTime; // ���� �ִϸ��̼� ���� �ð�;
		float m_fAnimateIdleTime; // ���� �ִϸ��̼� �� ��� �ð�;
		bool m_bLoseReward; // �й�� ���� ���� ��/��;

		class Stage
		{
			friend class OddEvenSettings;
		private:
			void doComplete(void);
			void getTakeItem(const DWORD nSelectIndex, SNATIVEID pItemID[MiniGame::EMODDEVEN_MAX_REWARD]) const;

		public:
			Stage(const DWORD nSuccessRate, const DWORD nTakeRate, const DWORD nCostMoney);
			Stage(const Stage& rhs);
			Stage(void);

		private:
			class RewardItem
			{
			public:
				const bool operator <(const RewardItem& rhs);
				RewardItem(const SNATIVEID& ItemID, const DWORD nTakeRate);
				RewardItem(const RewardItem& rhs);
				RewardItem(void);

				SNATIVEID m_ItemID;
				DWORD m_nTakeRate; // ȹ�� Ȯ��; 1/100000;
			};

			typedef std::vector<RewardItem> RewardItemVector;
			typedef RewardItemVector::iterator RewardItemVectorIter;
			typedef RewardItemVector::const_iterator RewardItemVectorCIter;

			RewardItemVector m_RewardItemVector;
			
			DWORD m_nTakeRate; // ������ ȹ�� Ȯ��; 1/100;
			DWORD m_nSuccessRate; // ���ӿ��� �̱� Ȯ��; 1/100;
			DWORD m_nCostMoney;
		};	

		typedef std::vector<Stage> StageVector;
		typedef StageVector::iterator StageVectorIter;
		typedef StageVector::const_iterator StageVectorCIter;
		
		StageVector m_StageVector;
	};
}

#include "GLOddEvenSettings.hpp"

