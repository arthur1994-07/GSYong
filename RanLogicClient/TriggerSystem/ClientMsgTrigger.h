#ifndef CClientMsgTrigger_H
#define CClientMsgTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ �޽��� Ʈ����
	Ư�� ��Ȳ�� �����鿡�� �޽����� ��� �Ѵ�.
	
 */
class CClientMsgTrigger
	: public CClientTrigger<CTrigger>
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
            Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�..

         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif