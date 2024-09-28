#ifndef CMapInfoTrigger_H
#define CMapInfoTrigger_H

#include "Trigger.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"

namespace TriggerSystem
{

/**
	�� ���� Ʈ����

	�δ���, ���� �� ���� ��ü ������ ������.

 */
class CMapInfoTrigger
	: public CTrigger
{
public:
		CMapInfoTrigger();

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

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
		SNATIVEID	    m_mapid;		//@ ��id
        DWORD           m_gateid;       //@ �Ա� ����Ʈ id
		int			    m_dailylimit;	//@ �Ϸ� ���� ����
		int			    m_owntime;		//@ �ͼ� �ð�
		int			    m_person;		//@ �ο� ����
        int		        m_waitentrance;	//@ ���� Ȯ�� ��� �ð�
};

} // end namespace

#endif