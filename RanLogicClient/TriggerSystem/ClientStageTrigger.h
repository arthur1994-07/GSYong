#ifndef CClientStageTrigger_H
#define CClientStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
    �������� ǥ�ñ�
    ���� ���������� ǥ�� �Ѵ�.
	
 */
class CClientStageTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientStageTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_STAGE; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

        /**
            ������ ��������

         */
        void                SetEndStage(int end) { m_end = end; }
        const int           GetEndStage() const { return m_end; }

        /**
            ǥ�� �������� ����

         */
        void                SetRange(int range) { m_range = range; }
        const int           GetRange() const { return m_range; }

protected:
        int                 m_end;      //@ �� ��������
        int                 m_range;    //@ �������� ǥ�� ����
};

} // end namespace

#endif