#ifndef CClientStartStageInstance_H
#define CClientStartStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	스테이지 시작 트리거의 인스턴스
    스테이지가 시작되면 메시지를 출력 한다.
	
 */
class CClientStartStageInstance
    : public CClientTriggerInstance
{
public:
    CClientStartStageInstance() : m_time(0.f) {}
    ~CClientStartStageInstance();

    /**
        스테이지 시작 메시지에 의해 호출 된다.
        우리는 이때스테이지 정보를 출력 한다.

    */
    void    StartStage(SMessage* msg);

    /**
		프레임 처리를 수행 한다.
		시간의 흐름을 처리 한다.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        변경된 상태를 반영 한다.

     */
    void    Update(bool bEnable = true);

    float   m_time; //@ 출력 남은 시간    
};

} // end namespace

#endif