#ifndef CServerStageTrigger_H
#define CServerStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� �δ� ���̵� Ʈ����
	���̵��� Ŭ��� �ʿ䰡 ���� ���������� �ʿ�� �Ѵ�.
	���� ����, ���ݷ� ���� ���� �� �� �ִ�.
	
 */
class CServerStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerStageTrigger();

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
			���ε� �������̽� �Լ���

		 */
        void		        SetMission(int mission) { m_mission = mission; }
        const int			GetMission() const { return m_mission; }
        void		        SetLastStage() { m_last = true; }
        const bool		    GetLastStage() const { return m_last; }
        void		        SetDelayTime(float delay) { m_delay = delay; }
        const float			GetDelayTime() const { return m_delay; }

protected:
        int                 m_mission;  //@ �̼� ��, �츮�� �� ����ŭ �̼��� �Ϸ�Ǿ�� ���� �Ϸ� �ȴ�.
        bool                m_last;     //@ ������ �������� ����
        float               m_delay;    //@ ����, ���� �� ������ Ÿ��(Ŭ�󿡼� �޽��� ��� �ð�)
};

} // end namespace

#endif