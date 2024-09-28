#ifndef CClientMonsterClearTrigger_H
#define CClientMonsterClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	���� Ŭ���� üũ ǥ�ñ�
    ���� ���� ����, ��ƾ� �ϴ� ���͸� ǥ�� �Ѵ�.
	
 */
class CClientMonsterClearTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientMonsterClearTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_MONSTERCLEAR; }

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