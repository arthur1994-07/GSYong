#ifndef CClientStageInstance_H
#define CClientStageInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	몬스터 클리어 상태를 처리 한다.
	
 */
class CClientStageInstance
    : public CClientTriggerInstance
{
public:
    CClientStageInstance();
    ~CClientStageInstance();

    /**
        스테이지 진행 상태를 전달 한다.
        이것은 스테이지의 상태가 변경되었을 때 서버로 부터 전송되어
        클라이언트가 적당히 UI 를 갱신 한다.

     */
    void    Stage(SMessage* msg);

	/**
        우리는 아바타가 죽으면
		사망 대기 UI를 출력 한다.

     */
	void    Action(SMessage* msg);

	/**
		우리는 아바타가 살아나면
		사망 대기 UI를 제거 한다.

	 */
	void    Rebirth(SMessage* msg);

	/**
		프레임 처리를 수행 한다.
		시간의 흐름을 처리 한다.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        변경된 상태를 반영 한다.

     */
    void    Update(bool bEnable);

	/**
		스테이지 상태를 출력 한다.

	 */
	void	StageState(int nStage, bool bEnable);

    int     m_stage;    //@ 현재 스테이지
    float   m_time;     //@ 스테이지 남은 시간
    bool    m_visible;  //@ 죽음 상태 등이 되면 UI를 보이지 않게 처리 
};

} // end namespace

#endif