#ifndef CClientMsgInstance_H
#define CClientMsgInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	스테이지 메시지 인스턴스
    스테이지내에서 유저들에게 알림 메시지를 출력 한다.
	
 */
class CClientMsgInstance
    : public CClientTriggerInstance
{
public:
    CClientMsgInstance() : m_time(0.f) {}
	~CClientMsgInstance();

    /**
        출력 해야할 메시지가 있을 때 호출 된다. 
        우리는 UI 에 전달된 메시지를 출력 한다.

    */
    void    Message(SMessage* msg);

	/**
        스테이지 상태 메시지를 처리 한다.
        우리는 게임 종료 상태일 때는 메시지 출력을 막는다.

     */
    void    Stage(SMessage* msg);

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

    float   m_time; //@ 메시지 출력 남은 시간
    TSTRING m_msg;  //@ 출력 메시지
};

} // end namespace

#endif