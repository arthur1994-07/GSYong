#ifndef CServerRetryTrigger_H
#define CServerRetryTrigger_H

#include "../../RanLogic/TriggerSystem/RetryTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
    �絵�� Ʈ����
    ������������ �׾��� �� ��ų�� ��Ȱ�� ����������
    �Ϲ� ��Ȱ�� �Ұ��� �ϰ� ó�� �Ѵ�.
	
 */
class CServerRetryTrigger	
    : public CServerTrigger<CRetryTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_RETRY; }

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