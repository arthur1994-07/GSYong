#ifndef CServerPortalCloseTrigger_H
#define CServerPortalCloseTrigger_H

#include "../../RanLogic/TriggerSystem/PortalCloseTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� ��Ż �ݱ� Ʈ����
	�������� ���۽� ��Ż�� ���� �� ��� �Ѵ�.
	�ݱ�� ������ ������ ���������� ��������� �ٸ��Ƿ�
    ���� �и� �Ѵ�. �̷��� �����ν� ��ũ��Ʈ�� ��Ȯ�ϰ� �� �� �ִ�.

    �Ʒ� �ڼ��� ���� m_toMapID �� m_toGateID �Է��� ó�� ���� �ʴ´�.
    ��Ż�� �ݴ� ������� ���� �ʴ�(ID_NULL) ���� �����ν� �۵����� �ʰ� �ϴ� ����̴�.

 */
class CServerPortalCloseTrigger
	: public CServerTrigger<CPortalCloseTrigger>
{
public:
		CServerPortalCloseTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_PORTALCLOSE; }

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
        const SNATIVEID	    GetToMapID() const { return m_toMapID; }
        const DWORD	        GetToGateID() const { return m_toGateID; }

protected:
        SNATIVEID           m_toMapID;      //@ �̵��� �� ID
        DWORD               m_toGateID;     //@ �̵��� ����Ʈ ID 
};

} // end namespace

#endif