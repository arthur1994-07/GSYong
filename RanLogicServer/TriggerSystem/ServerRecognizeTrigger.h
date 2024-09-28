#ifndef CServerRecognizeTrigger_H
#define CServerRecognizeTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� ���� �ð� Ʈ����
	���Ͱ� ĳ���͸� ������ �����ϴ� �ð��� ó�� �Ѵ�.
	
 */
class CServerRecognizeTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType			GetType() const { return TRIGGER_TYPE_RECOGNIZE; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void				Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			���ε� �������̽� �Լ���

		 */
		void				SetRecognizeTime(float time) { m_time = time; }
		const float			GetRecognizeTime() const { return m_time; }

protected:
		float				m_time;	//@ ���� �ð�(1 = 1s)
};

} // end namespace

#endif