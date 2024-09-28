#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Memory/SmartPtr.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/File/BaseStream.h"
#include "../../../SigmaCore/Util/MinMessageBox.h"
#include "../../../SigmaCore/Math/Random.h"

#include "../../../EngineLib/G-Logic/GLOGIC.h"

#include "../../GLogicFile.h"
#include "../../GLogicDataMan.h"

#include "GLOddEvenSettings.h"

namespace MiniGame
{
	namespace OddEven
	{
		HRESULT LoadFile(const std::string& _FileName, const bool _bPastLoad)
		{
			if ( _bPastLoad == false )
				return S_OK;

			sc::lua_init();

			std::string FullPath(GLOGIC::GetServerPath());
			FullPath.append("scripts\\");
			FullPath.append(_FileName);

			if ( sc::lua_doFile(FullPath) == false )
				return E_FAIL;

			OddEvenSettings* const pOddEvenSettings = OddEvenSettings::getInstance();

			LuaPlus::LuaObject BasicGroup(sc::lua_getGlobalFromName("Basic"));
			if ( BasicGroup.IsNil() == true )
				return E_FAIL;			

			const LuaPlus::LuaObject requireLevelObject(BasicGroup.GetByName("nRequireLevel"));
			if ( requireLevelObject.IsNil() == true )
				pOddEvenSettings->m_nRequireLevel = 10;
			else
				pOddEvenSettings->m_nRequireLevel = requireLevelObject.GetInteger();

			const LuaPlus::LuaObject requireMoneyObject(BasicGroup.GetByName("nRequireMoney"));
			if ( requireMoneyObject.IsNil() == true )
				pOddEvenSettings->m_nRequireMoney = 100000;
			else				
				pOddEvenSettings->m_nRequireMoney = requireMoneyObject.GetInteger();

			const LuaPlus::LuaObject timeLimitObject(BasicGroup.GetByName("fTimeLimit"));
			if ( timeLimitObject.IsNil() == true )
				pOddEvenSettings->m_fTimeLimit = 10.0f;
			else				
				pOddEvenSettings->m_fTimeLimit = timeLimitObject.GetFloat();

			const LuaPlus::LuaObject animateTimeObject(BasicGroup.GetByName("fAnimateTime"));
			if ( timeLimitObject.IsNil() == true )
				pOddEvenSettings->m_fAnimateTime = 1.0f / 1.5f;
			else				
				pOddEvenSettings->m_fAnimateTime = 1.0f / animateTimeObject.GetFloat();

			const LuaPlus::LuaObject animateIdleTimeObject(BasicGroup.GetByName("fAnimateIdleTime"));
			if ( timeLimitObject.IsNil() == true )
				pOddEvenSettings->m_fAnimateIdleTime = 0.5f * pOddEvenSettings->m_fAnimateTime;
			else				
				pOddEvenSettings->m_fAnimateIdleTime = animateIdleTimeObject.GetFloat() * pOddEvenSettings->m_fAnimateTime;

			const LuaPlus::LuaObject loseRewardObject(BasicGroup.GetByName("bLoseReward"));
			if ( loseRewardObject.IsNil() == true )
				pOddEvenSettings->m_bLoseReward = false;
			else
				pOddEvenSettings->m_bLoseReward = loseRewardObject.GetBoolean();

			DWORD nStageIndex = 0;
			LuaPlus::LuaObject RewardSetGroup = sc::lua_getGlobalFromName("RewardSet");			
			for ( LuaPlus::LuaTableIterator iterReward(RewardSetGroup); iterReward; iterReward.Next() )
			{
				LuaPlus::LuaTableIterator iterNode(iterReward.GetValue());

				const DWORD nSuccessRate = DWORD(iterNode.GetValue().GetFloat() * 100.0f);iterNode.Next();
				const DWORD nCostMoney = iterNode.GetValue().GetInteger();iterNode.Next();
				const DWORD nTakeRate = DWORD(iterNode.GetValue().GetFloat() * 100.0f);iterNode.Next();
				pOddEvenSettings->push(nSuccessRate, nTakeRate, nCostMoney);
				
				for (; iterNode; )
				{					
					const WORD wMainID = iterNode.GetValue().GetInteger();iterNode.Next();
					const WORD wSubID = iterNode.GetValue().GetInteger();iterNode.Next();
					const DWORD nTakeRate = DWORD(iterNode.GetValue().GetFloat() * float(MiniGame::EMODDEVEN_REWARD_PRECISION));iterNode.Next();
					if ( pOddEvenSettings->push(nStageIndex, nTakeRate, SNATIVEID(wMainID, wSubID)) == false )
					{
						std::string ErrorMsg(sc::string::format("%1% file open failed.", FullPath));
						sc::ErrorMessageBox(ErrorMsg, false);
						return false;
					}
				}
				++nStageIndex;
			}

			pOddEvenSettings->m_nMaxWin = nStageIndex - 1;

			pOddEvenSettings->doComplete();

			return S_OK;
		}
	}	


	OddEvenSettings::Stage::Stage(const DWORD nSuccessRate, const DWORD nTakeRate, const DWORD nCostMoney)
		: m_nSuccessRate(nSuccessRate)
		, m_nTakeRate(nTakeRate)
		, m_nCostMoney(nCostMoney)
	{
	}
	OddEvenSettings::Stage::Stage(const Stage& rhs)
		: m_nSuccessRate(rhs.m_nSuccessRate)
		, m_nTakeRate(rhs.m_nTakeRate)
		, m_nCostMoney(rhs.m_nCostMoney)
		, m_RewardItemVector(rhs.m_RewardItemVector)
	{
	}
	OddEvenSettings::Stage::Stage(void)
	{
	}

	void OddEvenSettings::Stage::doComplete(void)
	{
		std::sort(m_RewardItemVector.begin(), m_RewardItemVector.end());
	}

	void OddEvenSettings::Stage::getTakeItem(const DWORD nSelectIndex, SNATIVEID pItemID[MiniGame::EMODDEVEN_MAX_REWARD]) const
	{
		// 출력용 아이템 목록 만들기;
		const DWORD nItemSize = m_RewardItemVector.size();
		std::vector<DWORD> shuffleBuffer;
		shuffleBuffer.reserve(nItemSize);
		for ( DWORD _i = 0; _i < nItemSize; ++_i )
			shuffleBuffer.push_back(_i);
		
		// 받아야 할 아이템 정하기;
		const bool bTake = DWORD(sc::Random::getInstance().RandomNumber(0, 99)) < m_nTakeRate;
		if ( bTake == true )
		{
			const DWORD nTakeNumber = sc::Random::getInstance().GetValue(0, MiniGame::EMODDEVEN_REWARD_PRECISION - 1);
			DWORD nRateBuffer = 0;
			for ( DWORD _i = 0; _i < m_RewardItemVector.size(); ++_i )
			{
				const RewardItem& rewardItem = m_RewardItemVector[_i];
				nRateBuffer += rewardItem.m_nTakeRate;
				if ( nTakeNumber > nRateBuffer )
					continue;

				pItemID[0] = rewardItem.m_ItemID;
				shuffleBuffer.erase(shuffleBuffer.begin() + _i);
				break;
			}
		}
		else
		{
			pItemID[0] = SNATIVEID(false);
		}

		const DWORD nBufferSize = shuffleBuffer.size();
		const DWORD nSize = nBufferSize > (MiniGame::EMODDEVEN_MAX_REWARD - 1) ? (MiniGame::EMODDEVEN_MAX_REWARD - 1) : nBufferSize;
		std::random_shuffle(shuffleBuffer.begin(), shuffleBuffer.end());
		for ( DWORD _i = 0; _i < nSize; ++_i )
		{
			const DWORD nItemIndex = shuffleBuffer[_i];
			pItemID[_i + 1] = m_RewardItemVector[nItemIndex].m_ItemID;
		}

		for ( DWORD _i = nSize; _i < MiniGame::EMODDEVEN_MAX_REWARD - 1; ++_i )
			pItemID[_i + 1] = SNATIVEID(false);

		SNATIVEID BackUpItemID = pItemID[nSelectIndex];
		pItemID[nSelectIndex] = pItemID[0];
		pItemID[0] = BackUpItemID;
	}


	const bool OddEvenSettings::Stage::RewardItem::operator <(const RewardItem& rhs)
	{
		return m_nTakeRate < rhs.m_nTakeRate;
	}
	OddEvenSettings::Stage::RewardItem::RewardItem(const SNATIVEID& ItemID, const DWORD nTakeRate)
		: m_ItemID(ItemID)
		, m_nTakeRate(nTakeRate)
	{
	}
	OddEvenSettings::Stage::RewardItem::RewardItem(const RewardItem& rhs)
		: m_ItemID(rhs.m_ItemID)
		, m_nTakeRate(rhs.m_nTakeRate)
	{
	}
	OddEvenSettings::Stage::RewardItem::RewardItem(void)
	{
	}



	OddEvenSettings* const OddEvenSettings::getInstance(void)
	{
		static OddEvenSettings _instance;
		return &_instance;
	}
	OddEvenSettings::~OddEvenSettings(void)
	{
	}

	OddEvenSettings::OddEvenSettings(void)
		: m_nRequireLevel(10)
		, m_nRequireMoney(100000)
		, m_nMaxWin(0)
		, m_fTimeLimit(10.0f)
		, m_fAnimateTime(1.5f)
		, m_fAnimateIdleTime(0.5f)
		, m_bLoseReward(false)
	{
	}

	void OddEvenSettings::setCostMoney(const DWORD nMaxWin, const DWORD* const nCostMoney)
	{
		if ( m_StageVector.size() == 0 )
		{
			OddEvenSettings::push(0, 0, 0);
			for ( DWORD _i = 0; _i < nMaxWin; ++_i )
				OddEvenSettings::push(0, 0, nCostMoney[_i]);
		}
		else if ( (nMaxWin + 1) == m_StageVector.size() )
		{
			m_StageVector[0].m_nCostMoney = 0;
			for ( DWORD _i = 0; _i < nMaxWin; ++_i )
			{
				m_StageVector[_i + 1].m_nCostMoney = nCostMoney[_i];
			}
		}		
	}

	const bool OddEvenSettings::getTakeItem(const DWORD nStageIndex, const DWORD nSelectIndex, SNATIVEID pItemID[MiniGame::EMODDEVEN_MAX_REWARD]) const
	{
		if ( (nStageIndex + 1) >= m_StageVector.size() )
			return false;
		
		m_StageVector[nStageIndex + 1].getTakeItem(nSelectIndex, pItemID);
		return true;
	}

	const DWORD OddEvenSettings::getSuccessRate(const DWORD nStageIndex) const
	{
		if ( (nStageIndex + 1) >= m_StageVector.size() )
			return 0;

		return m_StageVector[nStageIndex + 1].m_nSuccessRate;
	}

	const LONGLONG OddEvenSettings::getCostMoney(const DWORD nStageIndex) const
	{
		if ( (nStageIndex + 1) >= m_StageVector.size() )
			return 0;

		return m_StageVector[nStageIndex + 1].m_nCostMoney;
	}

	const bool OddEvenSettings::push(const DWORD nSuccessRate, const DWORD nTakeRate, const DWORD nCostMoney)
	{
		m_StageVector.push_back(Stage(nSuccessRate, nTakeRate, nCostMoney));
		return true;
	}

	const bool OddEvenSettings::push(const DWORD nStageIndex, const DWORD nTakeRate, const SNATIVEID& ItemID)
	{
		if ( nStageIndex >= m_StageVector.size() )
			return false;
		
		m_StageVector[nStageIndex].m_RewardItemVector.push_back(Stage::RewardItem(ItemID, nTakeRate));
		return true;
	}

	void OddEvenSettings::doComplete(void)
	{
		for ( StageVectorIter iter = m_StageVector.begin(); iter != m_StageVector.end(); ++iter )
		{
			iter->doComplete();
		}
	}
}