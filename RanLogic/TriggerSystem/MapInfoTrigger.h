#ifndef CMapInfoTrigger_H
#define CMapInfoTrigger_H

#include "Trigger.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace TriggerSystem
{

/**
	맵 정보 트리거

	인던명, 제한 등 맵의 전체 정보를 가진다.

 */
class CMapInfoTrigger
	: public CTrigger
{
public:
		CMapInfoTrigger();

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void		    SetMap(WORD mid, WORD sid) { m_mapid = SNATIVEID(mid, sid); }
		const SNATIVEID	GetMap() const { return m_mapid; }

        void		    SetGateID(DWORD gateid) { m_gateid = gateid; }
        const DWORD	    GetGateID() const { return m_gateid; }

		void		    SetDailyLimit(int limit) { m_dailylimit = limit; }
		const int		GetDailyLimit() const { return m_dailylimit; }

		void		    SetOwnTime(int time) { m_owntime = time; }
		const int		GetOwnTime() const { return m_owntime; }

		void		    SetPerson(int num) { m_person = num; }
		const int		GetPerson() const { return m_person; }

        void		    SetWaitEntrance(int time) { m_waitentrance = time; }
        const int		GetWaitEntrance() const { return m_waitentrance; }

protected:
		SNATIVEID	    m_mapid;		//@ 맵id
        DWORD           m_gateid;       //@ 입구 게이트 id
		int			    m_dailylimit;	//@ 하루 입장 제한
		int			    m_owntime;		//@ 귀속 시간
		int			    m_person;		//@ 인원 제한
        int		        m_waitentrance;	//@ 입장 확인 대기 시간
};

} // end namespace

#endif