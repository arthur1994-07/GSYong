#ifndef CServerMonsterClearTrigger_H
#define CServerMonsterClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���͸� �Էµ� ������ ��ŭ Ŭ����(�׿���)
	�����ϴ� �̼� Ʈ���Ÿ� �ǹ� �Ѵ�.
	
 */
class CServerMonsterClearTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerMonsterClearTrigger();

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

		/**
			���ε� �������̽� �Լ���

		 */
		void		        SetCount(int count) { m_count = count; }
		const int			GetCount() const { return m_count; }

private:
		int			        m_count;	//@ Ŭ���� ������
};

} // end namespace

#endif