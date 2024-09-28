#ifndef CServerStageTrigger_H
#define CServerStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 인던 난이도 트리거
	난이도는 클라는 필요가 없고 서버에서만 필요로 한다.
	몬스터 방어력, 공격력 등을 조절 할 수 있다.
	
 */
class CServerStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerStageTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_STAGE; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			바인딩 인터페이스 함수들

		 */
        void		        SetMission(int mission) { m_mission = mission; }
        const int			GetMission() const { return m_mission; }
        void		        SetLastStage() { m_last = true; }
        const bool		    GetLastStage() const { return m_last; }
        void		        SetDelayTime(float delay) { m_delay = delay; }
        const float			GetDelayTime() const { return m_delay; }

protected:
        int                 m_mission;  //@ 미션 수, 우리는 이 수만큼 미션이 완료되어야 최종 완료 된다.
        bool                m_last;     //@ 마지막 스테이지 유무
        float               m_delay;    //@ 성공, 실패 후 딜레이 타임(클라에서 메시지 출력 시간)
};

} // end namespace

#endif