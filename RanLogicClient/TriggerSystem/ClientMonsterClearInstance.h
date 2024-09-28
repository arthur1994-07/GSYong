#ifndef CClientMonsterClearInstance_H
#define CClientMonsterClearInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	몬스터 클리어 상태를 처리 한다.
	
 */
class CClientMonsterClearInstance
    : public CClientTriggerInstance
{
public:
    CClientMonsterClearInstance();
    ~CClientMonsterClearInstance();

    /**
        스테이지 진행 상태를 전달 한다.
        이것은 스테이지의 상태가 변경되었을 때 서버로 부터 전송되어
        클라이언트가 적당히 UI 를 갱신 한다.

     */
    void    Stage(SMessage* msg);

    /**
        몬스터가 처치되었을 경우 호출 된다.
        우리는 현재 상태를 UI 로 뿌려 준다.

    */
    void    MonsterClear(SMessage* msg);

protected:
    /**
        변경된 상태를 반영 한다.

     */
    void    Update(bool bEnable = true);

    int     m_cur;  //@ 현재 잡은 몬스터수
    int     m_all;  //@ 총 잡아야할 몬스터수
};

} // end namespace

#endif