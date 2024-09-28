#ifndef CServerPostStageTrigger_H
#define CServerPostStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
    �������� �� ó��
    �츮�� �������� ����(���� or ����) �� �Ǹ� ���� ���� UI �� ������ �ð� ���� ��� ��
    ���� �������� ���Ḧ ó�� �Ѵ�. �� �����̸� ���� ���������
    ���� ���������� �� ��� ó���� ������ ���⵵�� ���� ������ �и��ϸ� ������ ���ؼ� ����
	
 */
class CServerPostStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_POSTSTAGE; }

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
        void		        SetTime(int time) { m_time = time; }
        const int	        GetTime() const { return m_time; }

protected:
        int			        m_time;	//@ �������� ���� �� ��� �ð�
};

} // end namespace

#endif