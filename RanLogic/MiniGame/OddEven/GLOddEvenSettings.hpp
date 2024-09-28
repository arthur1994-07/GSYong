#pragma once

namespace MiniGame
{
	__forceinline const DWORD OddEvenSettings::getMaxWin(void) const
	{
		return m_nMaxWin;
	}
	__forceinline const DWORD OddEvenSettings::getRequireLevel(void) const
	{
		return m_nRequireLevel;
	}
	__forceinline const DWORD OddEvenSettings::getRequireMoney(void) const
	{
		return m_nRequireMoney;
	}
	__forceinline const float OddEvenSettings::getTimeLimit(void) const
	{
		return m_fTimeLimit;
	}
	__forceinline const float OddEvenSettings::getAnimateTime(void) const
	{
		return m_fAnimateTime;
	}
	__forceinline const float OddEvenSettings::getAnimateIdleTime(void) const
	{
		return m_fAnimateIdleTime;
	}
	__forceinline const bool OddEvenSettings::getLoseReward(void) const
	{
		return m_bLoseReward;
	}

	__forceinline void OddEvenSettings::setMaxWin(const DWORD nMaxWin)
	{
		this->m_nMaxWin = nMaxWin;
	}
	__forceinline void OddEvenSettings::setRequireLevel(const DWORD nRequireLevel)
	{
		this->m_nRequireLevel = nRequireLevel;
	}
	__forceinline void OddEvenSettings::setRequireMoney(const DWORD nRequireMoney)
	{
		this->m_nRequireMoney = nRequireMoney;
	}
	__forceinline void OddEvenSettings::setTimeLimit(const float fTimeLimit)
	{
		this->m_fTimeLimit = fTimeLimit;
	}
	__forceinline void OddEvenSettings::setAnimateTime(const float fAnimateTime)
	{
		this->m_fAnimateTime = fAnimateTime;
	}
	__forceinline void OddEvenSettings::setAnimateIdleTime(const float fAnimateIdleTime)
	{
		this->m_fAnimateIdleTime = fAnimateIdleTime;
	}
	__forceinline void OddEvenSettings::setLoseReward(const bool bLoseReward)
	{
		this->m_bLoseReward = bLoseReward;
	}
}

