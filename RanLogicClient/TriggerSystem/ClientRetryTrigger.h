#ifndef CClientRetryTrigger_H
#define CClientRetryTrigger_H

#include "../../RanLogic/TriggerSystem/RetryTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	�絵�� Ʈ����
	������������ �׾��� �� ��ų�� ��Ȱ�� ����������
    �Ϲ� ��Ȱ�� �Ұ��� �ϰ� ó�� �Ѵ�.
	
 */
class CClientRetryTrigger
	: public CClientTrigger<CRetryTrigger>
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