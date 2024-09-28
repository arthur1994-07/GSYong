#ifndef CClientPositioningClearTrigger_H
#define CClientPositioningClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	��ġ Ŭ���� üũ ǥ�ñ�
    Ư�� ��ġ�� ��ġ�ϸ� Ŭ���� �ȴ�.
	
 */
class CClientPositioningClearTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientPositioningClearTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_POSITIONINGCLEAR; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif