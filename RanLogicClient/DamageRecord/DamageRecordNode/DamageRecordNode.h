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
		DWORD m_nDamageForDPS; // DPS ����� ���� �ӽ� ���ط�;

		float m_fDPS; // ���ط�/��;
		float m_fLastestWroteTime; // ���� �ֱ� ��ϵ� �ð�;
		float m_fDPSCheckTime; // DPS ����� ���� �ӽ� �ð�;
		float m_fElpasedTime; // ù DPS ���� �� ��� �ð�;
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
		LogList m_ListLog; // �α� ���Ͽ� ��ϵ� �׸� ����Ʈ;
		DetailMap m_mapDetail; // ��ų�� ���� ����;
		Calculator m_Calculator; // DPS ����;
		
		char m_szName[CHAR_SZNAME]; // �̸�;
		SNATIVEID m_SkillID; // ���� ����� ��ųID;
		DWORD m_nAccumDamage; // �� ���� ���ط�;
		WORD m_wSkillLevel; // ���� ����� ��ų ����;
	};		
}