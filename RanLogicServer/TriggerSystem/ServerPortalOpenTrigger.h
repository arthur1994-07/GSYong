#ifndef CServerPortalOpenTrigger_H
#define CServerPortalOpenTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� ��Ż ���� Ʈ����
	���������� Ŭ���� �Ǹ� ��Ż�� ���� �������� �̵� ��ų �� �ִ�.
	�̸� �۵� �ϴ� Ʈ����, ���������� ���� �Ѵ�.
	
 */
class CServerPortalOpenTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerPortalOpenTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_PORTALOPEN; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			���ε� �������̽� �Լ���

		 */
		void		        SetFromGateID(DWORD id) { m_fromGateID = id; }
		const DWORD		    GetFromGateID() const { return m_fromGateID; }

        void		        SetToMapID(WORD mid, WORD sid) { m_toMapID = SNATIVEID(mid, sid); }
        const SNATIVEID	    GetToMapID() const { return m_toMapID; }

        void		        SetToGateID(DWORD gateid) { m_toGateID = gateid; }
        const DWORD	        GetToGateID() const { return m_toGateID; }

private:
		DWORD		        m_fromGateID;   //@ ���� �ʿ��� Ÿ�� ����Ʈ ID
        SNATIVEID           m_toMapID;      //@ �̵��� �� ID
        DWORD               m_toGateID;     //@ �̵��� ����Ʈ ID        
};

} // end namespace

#endif