#ifndef CClientTriggerInstance_H
#define CClientTriggerInstance_H

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

class GLGaeaClient;

namespace TriggerSystem
{

/**
	Ʈ���� �ν��Ͻ� ó����
    ����ϴ� �ν��Ͻ��� ��� �� Ŭ������ ��� �޾� ���� �ϵ��� �Ѵ�.
    ����ϴ� �ν��Ͻ��� ���� �������̽� �Լ����� ���� �Ѵ�.

 */
class CClientTriggerInstance
    : public CTriggerInstance
{
public:
virtual ~CClientTriggerInstance() {}

        /**
            �Ʒ� �Լ����� �޽����� ���� ȣ��Ǵ� �Լ����̴�.
            �Լ� ������ ȣ���ϴ� �޽��� �ּ��� ���� �Ѵ�.

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
            ������� ���� �Ѵ�.
            ���� ���� �Լ��̹Ƿ� ���� �Լ��� ����.

         */
        GLGaeaClient*   GetGaeaClient();
};

} // end namespace

#endif