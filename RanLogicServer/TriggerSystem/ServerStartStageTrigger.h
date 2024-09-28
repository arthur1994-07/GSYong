#ifndef CServerStartStageTrigger_H
#define CServerStartStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	�������� ���� Ʈ����
    �������� ���ۿ� ���õ� �����͸� ������.
	
 */
class CServerStartStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType		    GetType()  const { return TRIGGER_TYPE_STARTSTAGE; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void			    Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif