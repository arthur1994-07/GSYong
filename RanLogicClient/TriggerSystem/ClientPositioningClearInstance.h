#ifndef CClientPositioningClearInstance_H
#define CClientPositioningClearInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	위치 클리어 상태를 처리 한다.
	
 */
class CClientPositioningClearInstance
    : public CClientTriggerInstance
{
public:
    CClientPositioningClearInstance();
    ~CClientPositioningClearInstance();

    /**
        스테이지 진행 상태를 전달 한다.
        이것은 스테이지의 상태가 변경되었을 때 서버로 부터 전송되어
        클라이언트가 적당히 UI 를 갱신 한다.

     */
    void    Stage(SMessage* msg);

    /**
        위치를 잡아야 할 경우 호출 된다.
        우리는 현재 상태를 UI 로 뿌려 준다.

    */
    void    PositioningClear(SMessage* msg);

protected:
    /**
        변경된 상태를 반영 한다.

     */
    void    Update(bool bEnable);

    int     m_x;  //@ 위기 잡기 X 좌표
    int     m_y;  //@ 위기 잡기 Y 좌표

    int     m_nNow;           //@ 현재 완료한 사람
    int     m_nNeed;          //@ 미션 완료에 필요한 사람
};

} // end namespace

#endif