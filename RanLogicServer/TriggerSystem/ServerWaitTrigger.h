#ifndef CServerWaitTrigger_H
#define CServerWaitTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� �ð� ��� Ʈ����
	���� �ð� ���� Ʈ������ ������ ��� ��Ų��.
	������� �������� Ŭ���� �� ���� ������ ���� ���ʰ� ��⸦ ��ų �� �ִ�.
	
 */
class CServerWaitTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_WAIT; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

		 */
		void		SetTime(float time) { SetLifeTime(time); }
};

} // end namespace

#endif