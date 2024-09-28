#ifndef CClientMapInfoTrigger_H
#define CClientMapInfoTrigger_H

#include "../../RanLogic/TriggerSystem/MapInfoTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	맵 정보를 가진다.
	이를 통해 외부에서 맵을 제어 한다.
	
 */
class CClientMapInfoTrigger
	: public CClientTrigger<CMapInfoTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType		GetType() const { return TRIGGER_TYPE_MAPINFO; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void			Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void			SetMapName(TSTRING name) { m_mapname = name; }
		const TSTRING	GetMapName() const { return m_mapname; }

		void			SetObject(TSTRING Object) { m_mapObject = Object; }//통합전장 UI용
		const TSTRING	GetObject() const { return m_mapObject; }

		void			SetMapInfo(TSTRING info) { m_mapinfo = info; }
		const TSTRING	GetMapInfo() const { return m_mapinfo; }

		void			SetMapInfoRn(TSTRING infoRn) { m_mapinfoRn = infoRn; } // 통합전장 UI용
		const TSTRING	GetMapInfoRn() const { return m_mapinfoRn; }


		void			SetMapReward(TSTRING reward) { m_mapreward = reward; }
		const TSTRING	GetMapReward() const { return m_mapreward; }

protected:
		TSTRING			m_mapname;		//@ 인던명(XML)
		TSTRING			m_mapObject;	//@ 맵목표(XML) 통합전장 UI용
		TSTRING			m_mapinfo;		//@ 맵정보(XML)
		TSTRING			m_mapinfoRn;	//@ 맵정보(XML) 통합전장 UI용
		TSTRING			m_mapreward;	//@ 맵보상(XML)
};

} // end namespace

#endif