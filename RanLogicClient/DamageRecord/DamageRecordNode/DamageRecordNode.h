#pragma once

#include <map>
#include <list>
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../DamageRecordDefine.h"

namespace DamageRecord
{
	class LogElement
	{
	public:
		LogElement(const float fTime, const DWORD nDamage, const float fDPS, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag);

	public:
		const SNATIVEID m_SkillID;
		const DWORD m_nDamage;
		const DWORD m_dwDamageFlag;
		const float m_fTime;
		const float m_fDPS;
		const WORD m_wSkillLevel;
	};	

	class DetailElement
	{
	public:
		DetailElement(const SNATIVEID& SkillID, const DWORD nDamage);
		DetailElement(void);

	public:
		SNATIVEID m_SkillID;
		DWORD m_nDamage;
		DWORD m_nUseCounter;
	};

	class Calculator
	{
	public:
		const float doDamage(const DWORD nDamage, const float fTime);		

		void reset(void);

	public:
		Calculator(void);

	public:		
		DWORD m_nDamageForDPS; // DPS 계산을 위한 임시 피해량;

		float m_fDPS; // 피해량/초;
		float m_fLastestWroteTime; // 가장 최근 기록된 시간;
		float m_fDPSCheckTime; // DPS 계산을 위한 임시 시간;
		float m_fElpasedTime; // 첫 DPS 측정 후 경과 시간;
	};

	class ViewElement
	{
	public :
		typedef std::list<LogElement> LogList;
		typedef LogList::const_iterator LogListCIter;
		typedef LogList::iterator LogListIter;

		typedef std::map<SNATIVEID, DetailElement> DetailMap;
		typedef DetailMap::value_type DetailMapValue;
		typedef DetailMap::const_iterator DetailMapCIter;
		typedef DetailMap::iterator DetailMapIter;
		
	public:
		const DWORD getNDetailElement(void) const;
		void doDamage(const DWORD nDamage, const float fTime, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag);
		void getDetailElement(DetailElement* const pElement, const DWORD nElement, const DWORD nBeginIndex = 0) const;
		void saveCSV(std::fstream& streamFile);

		void reset(void);

	public:
		ViewElement(const LogID& ActorID, const char* const szName);
		virtual ~ViewElement(void);

	public:
		const LogID m_ActorID;
		LogList m_ListLog; // 로그 파일에 기록될 항목 리스트;
		DetailMap m_mapDetail; // 스킬별 세부 정보;
		Calculator m_Calculator; // DPS 계산기;
		
		char m_szName[CHAR_SZNAME]; // 이름;
		SNATIVEID m_SkillID; // 최종 사용한 스킬ID;
		DWORD m_nAccumDamage; // 총 누적 피해량;
		WORD m_wSkillLevel; // 최종 사용한 스킬 레벨;
	};		
}