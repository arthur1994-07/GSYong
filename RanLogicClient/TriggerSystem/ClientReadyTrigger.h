#ifndef CClientReadyTrigger_H
#define CClientReadyTrigger_H

#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ ���� ��� Ʈ����
	������ CReadyTrigger ����
	
 */
class CClientReadyTrigger
	: public CClientTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_READY; }

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