#ifndef CServerReadyTrigger_H
#define CServerReadyTrigger_H

#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� ���� ��� Ʈ����
	������ CReadyTrigger ����
	
 */
class CServerReadyTrigger	
    : public CServerTrigger<CTrigger>
{
public:
        CServerReadyTrigger() : m_time(0), m_count(0) {}

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
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		void		        SetCount(int count) { m_count = count; }
		const int			GetCount() const { return m_count; }

protected:
		int			        m_time;	//@ ��� �ð�(sec)
		int			        m_count;//@ ��� �ο�
};

} // end namespace

#endif