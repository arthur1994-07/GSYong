#include "pch.h"

#include "../../../RanLogic/GLogicData.h"
#include "DamageRecordNode.h"

namespace DamageRecord
{
	LogElement::LogElement(const float fTime, const DWORD nDamage, const float fDPS, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag)
		: m_SkillID(SkillID)
		, m_wSkillLevel(wSkillLevel)
		, m_fTime(fTime)
		, m_nDamage(nDamage)
		, m_dwDamageFlag(dwDamageFlag)
		, m_fDPS(fDPS)
	{
	}

	DetailElement::DetailElement(const SNATIVEID& SkillID, const DWORD nDamage)
		: m_SkillID(SkillID)
		, m_nDamage(nDamage)
		, m_nUseCounter(1)
	{
	}
	DetailElement::DetailElement(void)
		: m_nUseCounter(0)
	{
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////

	// m_fLastestWroteTime의 초기값을 -(fINTERVAL_TIME + 1.0f)로 준 이유는;
	// 검사 직후 바로 들어오더라도 한번은 초기화 될 수 있게 하기 위함;
	Calculator::Calculator(void)
		: m_fLastestWroteTime(-(GLCONST_CHAR::fDPS_TurnToPeaceModeInterval + 1.0f)) 		
		, m_fDPS(0.0f)
		, m_fElpasedTime(0.0f)
	{
	}

	const float Calculator::doDamage(const DWORD nDamage, const float fTime)
	{
		const float fIdleTime = fTime - m_fLastestWroteTime;
		m_fLastestWroteTime = fTime;

		const bool bOverIdle = fIdleTime > GLCONST_CHAR::fDPS_TurnToPeaceModeInterval;
		if ( bOverIdle )
		{
			m_nDamageForDPS = 0;
			m_fDPSCheckTime = fTime;
		}

		m_nDamageForDPS += nDamage;		

		m_fElpasedTime = fTime - m_fDPSCheckTime;
		const float fUElapsedTime = 1.0f / (m_fElpasedTime < 0.01f ? 1.0f : m_fElpasedTime);
		m_fDPS = m_nDamageForDPS * (bOverIdle ? 1.0f : fUElapsedTime);

		return m_fDPS;
	}

	void Calculator::reset(void)
	{
		m_fLastestWroteTime = -(GLCONST_CHAR::fDPS_TurnToPeaceModeInterval + 1.0f);		
		m_fDPS = 0.0f;
		m_fElpasedTime = 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	
	ViewElement::ViewElement(const LogID& ActorID, const char* const szName)
		: m_ActorID(ActorID)
		, m_SkillID(SNATIVEID(false))
		, m_nAccumDamage(0)
	{
		StringCchCopy(m_szName, CHAR_SZNAME, szName);
	}
	ViewElement::~ViewElement(void)
	{
	}

	const DWORD ViewElement::getNDetailElement(void) const
	{
		return m_mapDetail.size();
	}
	void ViewElement::doDamage(const DWORD nDamage, const float fTime, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag)
	{
		// DPS 계산;
		const float fDPS = m_Calculator.doDamage(nDamage, fTime);

		{ // 로그 리스트에 기록;
			const LogElement Element(fTime, nDamage, fDPS, SkillID, wSkillLevel, dwDamageFlag);
			m_ListLog.push_back(Element);
		}

		{ // 세부 항목 기록;
			DetailMapIter iterDetail = m_mapDetail.find(SkillID);
			// 존재하지 않는 항목인 경우 UseCounter(1), nDamage(nDamage)로 바로 설정해서 삽입;
			if ( iterDetail == m_mapDetail.end() )
				m_mapDetail.insert(DetailMapValue(SkillID, DetailElement(SkillID, nDamage)));
			else
			{
				DetailElement& Element = iterDetail->second;
				Element.m_nDamage += nDamage;
				Element.m_nUseCounter += 1;
			}
		}		

		m_SkillID = SkillID;
		m_wSkillLevel = wSkillLevel;
		m_nAccumDamage += nDamage;
	}

	void ViewElement::getDetailElement(DetailElement* const pElement, const DWORD nElement, const DWORD nBeginIndex /* = 0 */) const
	{
		DWORD nCount = 0;
		const DWORD nSize = m_mapDetail.size();
		DetailMapCIter iter = m_mapDetail.begin();
		for ( DWORD _i = 0; _i < nSize; ++iter, ++_i )
		{
			if ( nCount == nElement )
				break;

			if ( nBeginIndex > _i )
				continue;

			pElement[nCount] = iter->second;
			++nCount;
		}
	}

	void ViewElement::saveCSV(std::fstream& streamFile)
	{
		if ( m_ListLog.size() == 0 )
			return;

		const float fUMinute = 1.0f / 60.0f;
		const DWORD nType = m_ActorID.nType;		

		for ( LogListIter iter = m_ListLog.begin(); iter != m_ListLog.end(); ++iter )
		{
			const LogElement& Element = *iter;

			if ( nType == CROW_PC )
				streamFile<<m_ActorID.dwDbNum<<",";
			else
			{
				CString strIDFormat;
				strIDFormat.Format("CN_%03d_%03d", m_ActorID.wMainID, m_ActorID.wSubID);
				streamFile<<strIDFormat<<",";
			}

			streamFile<<Element.m_nDamage<<",";

			{
				const DWORD nMinute = DWORD(Element.m_fTime * fUMinute);
				const float fSecond = Element.m_fTime - float(nMinute * 60.0f);
				CString strTimeFormat;
				strTimeFormat.Format("%d:%2.2f", nMinute, fSecond);
				streamFile<<strTimeFormat<<",";
			}

			CString strDPSFormat;
			strDPSFormat.Format("%.2f", Element.m_fDPS);
			streamFile<<strDPSFormat<<",";			

			CString strSkillIDFormat;
			if ( Element.m_SkillID == SNATIVEID(false) )
				strSkillIDFormat.Format("--/--");
			else
				strSkillIDFormat.Format("SN_%03d_%03d", Element.m_SkillID.Mid(), Element.m_SkillID.Sid());
			streamFile<<strSkillIDFormat<<",";

			CString strSkillLevelFormat;
			strSkillLevelFormat.Format("%d", Element.m_wSkillLevel + 1);
			streamFile<<strSkillLevelFormat<<",";

			//DAMAGE_TYPE_PSY_REFLECTION

			streamFile<<std::endl;
		}

		m_ListLog.clear();
	}

	void ViewElement::reset(void)
	{
		m_nAccumDamage = 0;
		m_Calculator.reset();
		m_mapDetail.clear();
	}
}