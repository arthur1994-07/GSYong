#ifndef CServerPortalCloseInstance_H
#define CServerPortalCloseInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	포탈을 닫는다.(클라입장에서는 안보이게, 서버입장에서는 작동되지 않게)
    기본적으로 포탈은 보여 지는 상태로 맵툴에서 만들 므로 새로 클라이언트가
    접속 하면 닫아 주도록 메시지를 보내야 한다. 이를 처리 한다.
	
 */
class CServerPortalCloseInstance
    : public CServerTriggerInstance
{
public:
        CServerPortalCloseInstance();

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