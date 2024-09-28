#ifndef CServerMsgInstance_H
#define CServerMsgInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	메시지를 전송 한다.
    메시지는 XML 정의된 키워드이어야 한다.
    이 메시지를 받은 클라이언트는 현재 상태에 맞게 UI에 출력 하게 된다.

 */
class CServerMsgInstance
    : public CServerTriggerInstance
{
public:
        /**
            인스턴스가 생성될 때 호출 된다.

         */
        void		Start();

        /**
            현재 상태를 갱신 한다.

            \param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void        Update(DWORD dwCharDbNum = 0);

        /**
        	유저가 진입했을 경우 호출 된다.

         */
        void        EntryPC(SMessage* msg);
};

} // end namespace

#endif