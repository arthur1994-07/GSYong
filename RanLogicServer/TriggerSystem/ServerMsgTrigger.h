#ifndef CServerMsgTrigger_H
#define CServerMsgTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	�޽��� Ʈ����
	Ư�� ��Ȳ�� �����鿡�� �޽����� ��� �Ѵ�.
	
 */
class CServerMsgTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType		    GetType() const { return TRIGGER_TYPE_MSG; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void			    Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			���ε� �������̽� �Լ���

		 */
		void			    SetMessage(TSTRING msg) { m_msg = msg; }
		const TSTRING	    GetMessage() const { return m_msg; }

protected:
		TSTRING			    m_msg;	//@ �޽��� ����
};

} // end namespace

#endif