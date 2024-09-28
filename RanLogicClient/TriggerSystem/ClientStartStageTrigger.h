#ifndef CClientStartStageTrigger_H
#define CClientStartStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ �������� ���� Ʈ����
	�����鿡�� �������� ���� �޽����� ��� �Ѵ�.
	
 */
class CClientStartStageTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientStartStageTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_STARTSTAGE; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
            Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�..

         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif