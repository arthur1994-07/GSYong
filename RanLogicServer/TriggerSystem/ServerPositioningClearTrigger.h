#ifndef CServerPositioningClearTrigger_H
#define CServerPositioningClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	Ư�� ��ġ ���޽� �Ϸ� Ʈ����
	Ư�� ��ġ�� �ο� �� ��ŭ ���޽� �Ϸ� �ȴ�.
    �̸� ���� ������ ������.
	
 */
class CServerPositioningClearTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerPositioningClearTrigger();

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

		/**
			���ε� �������̽� �Լ���

		 */
		void		        SetMin(float x, float y) { m_min = D3DXVECTOR2(x, y); }
		const D3DXVECTOR2	GetMin() const { return m_min; }

		void		        SetMax(float x, float y) { m_max = D3DXVECTOR2(x, y); }
		const D3DXVECTOR2	GetMax() const { return m_max; }

        void		        SetCount(int count) { m_count = count; }
        const int	        GetCount() const { return m_count; }

private:
		D3DXVECTOR2	        m_min;
		D3DXVECTOR2	        m_max;
        int                 m_count;
};

} // end namespace

#endif