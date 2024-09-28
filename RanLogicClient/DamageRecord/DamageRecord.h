#pragma once

#include "./DamageRecordNode/DamageRecordNode.h"
#include "DamageRecordDefine.h"

#include "../Widget/GLWidgetScript.h"

/*
 * ���(addActor) �� Actor���� ������ �� �ʴ� ������(DPS) �� ���;
 * ����(dropActor) �� ���� �� ��� ���� ��Ͽ� �߰���;
 * saveCsv()�� ���� ��� ����;
*/

/*
 * ���� �� �ý����� ID ���´�;
 * dwDbNum, dwGaeaID, dwGlobID, wMainID/wSubID�� �������� �־�;
 * �Ѱ��� ������ ID�� CROW, PC���� ������ �ȵ�;
 * �׷��� ������ ���� DamageRecord������ LogID�� �����Ͽ� ���;
*/

class GLGaeaClient;
namespace DamageRecord
{
	class Manager
	{
		typedef std::map<LogID, ViewElement> ViewMap;
		typedef ViewMap::value_type ViewMapValue;
		typedef ViewMap::const_iterator ViewMapCIter;
		typedef ViewMap::iterator ViewMapIter;

		typedef std::vector<LogID> ViewSlotVector;
		typedef ViewSlotVector::const_iterator ViewSlotVectorCIter;
		typedef ViewSlotVector::iterator ViewSlotVectorIter;

	public: // UI;
		const DWORD size(void) const;
		const ViewElement* const GetViewElement(const DWORD nSlotIndex) const;
		void dropActorByIndex(const DWORD nIndex);
		void resetActorByIndex(const DWORD nIndex);

	public: // Logic;
		const bool isOn(void) const;
		void turnOn(void);
		void addActor(const DWORD dwDbNum, const char* const szName);
		void addActor(const WORD wMainID, const WORD wSubID, const char* const szName);
		void dropActor(const DWORD dwDbNum);
		void dropActor(const WORD wMainID, const WORD wSubID);		
		bool doDamage(const DWORD dwDbNum, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag);
		bool doDamage(const WORD wMainID, const WORD wSubID, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag);		

	private:
		__forceinline BOOL _addActor(const LogID& ActorID, const char* const szName);
		__forceinline void _dropActor(const LogID& ActorID);
		__forceinline bool _doDamage(const LogID& ActorID, const char* const szName, const DWORD nDamage, const SNATIVEID& SkillID, const WORD wSkillLevel, const DWORD dwDamageFlag);

	public: // Special;
		const bool saveCSV(void);
		void reset(void);

	public:
		Manager(void);
		virtual ~Manager(void);

	private:
		ViewSlotVector m_vectorViewSlot; // ���� ��ϵ� ���(VIEW_SLOT_NSIZE��);
		ViewMap m_mapView; // ��� �Ǿ����ų� ��� �� ���;
		bool m_bUse;

	public :
		LuaTable GetRecord();
		LuaTable GetDetail( int nIndex );
		
	};	
}


