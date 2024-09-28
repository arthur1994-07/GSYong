#pragma once

#include "./DamageRecordNode/DamageRecordNode.h"
#include "DamageRecordDefine.h"

#include "../Widget/GLWidgetScript.h"

/*
 * 등록(addActor) 된 Actor들의 데미지 및 초당 데미지(DPS) 를 기록;
 * 해제(dropActor) 후 재등록 한 경우 기존 기록에 추가됨;
 * saveCsv()로 파일 출력 가능;
*/

/*
 * 기존 란 시스템의 ID 형태는;
 * dwDbNum, dwGaeaID, dwGlobID, wMainID/wSubID로 나뉘어져 있어;
 * 한가지 형태의 ID로 CROW, PC등의 구분이 안됨;
 * 그러한 연유로 인해 DamageRecord에서는 LogID로 통합하여 사용;
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
		ViewSlotVector m_vectorViewSlot; // 현재 등록된 대상(VIEW_SLOT_NSIZE개);
		ViewMap m_mapView; // 등록 되었었거나 등록 된 대상;
		bool m_bUse;

	public :
		LuaTable GetRecord();
		LuaTable GetDetail( int nIndex );
		
	};	
}


