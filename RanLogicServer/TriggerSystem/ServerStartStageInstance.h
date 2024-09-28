#ifndef CServerStartStageInstance_H
#define CServerStartStageInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	스테이지가 시작될 때의 로직을 처리 한다.
    현재는 클라에 시작 메시지를 전달 한다.

	스테이지에서 처리해두 되는데 굳이 스테이지 시작이 있는 이유는
	스테이지가 바로 시작되는게 아니라 일정 텀(시간)이 있기 때문에
	스테이지 시작이라는 트리거가 존재 한다.

 */
class CServerStartStageInstance
    : public CServerTriggerInstance
{
public:
        /**
            인스턴스가 생성될 때 호출 된다.

         */
        void	Start();

        /**
            현재 상태를 갱신 한다.

            \param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void    Update(DWORD dwCharDbNum = 0);

        /**
        	유저가 진입했을 경우 호출 된다.

         */
        void    EntryPC(SMessage* msg);
};

} // end namespace

#endif