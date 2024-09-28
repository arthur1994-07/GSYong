#ifndef CServerPartyTrigger_H
#define CServerPartyTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	��Ƽ ���� ó���� ���� �Ѵ�.
	����� ��Ż�� �δ����� ������ ó���� �Ѵ�.
	��Ƽ ���� ó���� �̰����� �����ϵ��� ���ȭ �Ѵ�.
	
 */
class CServerPartyTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerPartyTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType		    GetType() const { return TRIGGER_TYPE_PARTY; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void			    Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			���ε� �������̽� �Լ���

		 */
		void		        SetAutoOut(bool bAuto) { m_bAutoOut = bAuto; }
		bool				GetAutoOut() const { return m_bAutoOut; }

protected:
		bool				m_bAutoOut;	//@ ��Ż�� �δ����� ������ ���� ����
};

} // end namespace

#endif