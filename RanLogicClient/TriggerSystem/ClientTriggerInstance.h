#ifndef CClientTriggerInstance_H
#define CClientTriggerInstance_H

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

class GLGaeaClient;

namespace TriggerSystem
{

/**
	트리거 인스턴스 처리기
    사용하는 인스턴스는 모두 이 클래스를 상속 받아 구현 하도록 한다.
    사용하는 인스턴스의 공용 인터페이스 함수들을 관리 한다.

 */
class CClientTriggerInstance
    : public CTriggerInstance
{
public:
virtual ~CClientTriggerInstance() {}

        /**
            아래 함수들은 메시지에 의해 호출되는 함수들이다.
            함수 설명은 호출하는 메시지 주석을 참조 한다.

         */
virtual void            Ready(SMessage* msg) {}
virtual void            MonsterClear(SMessage* msg) {}
virtual void            PositioningClear(SMessage* msg) {}
virtual void            StartStage(SMessage* msg) {}
virtual void            Stage(SMessage* msg) {}
virtual void            Message(SMessage* msg) {}
virtual void            Action(SMessage* msg) {}
virtual void            Rebirth(SMessage* msg) {}

        /**
            랜드맨을 추출 한다.
            자주 쓰는 함수이므로 공용 함수로 뺀다.

         */
        GLGaeaClient*   GetGaeaClient();
};

} // end namespace

#endif