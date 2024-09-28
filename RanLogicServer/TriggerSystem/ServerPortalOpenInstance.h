#ifndef CServerPortalOpenInstance_H
#define CServerPortalOpenInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

struct SServerPortalMessage;

/**
	포탈을 오픈 한다.(클라입장에서는 보이게, 서버입장에서는 작동되게)
    닫혀져 있는 포탈이 작동 되도록 오픈 한다.
	
 */
class CServerPortalOpenInstance
    : public CServerTriggerInstance
{
public:
        CServerPortalOpenInstance();

        /**
            인스턴스가 생성될 때 호출 된다.
            우리는 인스턴스가 시작되었다는 것은 미션을
            완료했다는 것이므로 포탈 오픈 패킷을 전송 한다.

         */
        void		    Start();

        /**
        	유저가 진입했을 경우 호출 된다.
            포탈을 열고 난 다음에도 퇴장한 유저의 입장이
            있을 수 있으므로 유저가 입장 하면 열도록 요청 한다.

         */
        void            EntryPC(SMessage* msg);

        /**
        	PC 가 퇴장했을 경우 호출 된다.

         */
        void            ExitPC(SMessage* msg);

		// 플레이어가 파티를 탈퇴했을 경우 호출 된다;
		void			PartyOutPC(SMessage* msg);

        /**
            포탈을 요청 할 경우 호출 된다.
            클라이언트의 게이트에 의해 요청 되어 진다.
            우리는 실제 이동을 처리하고 최종 종료 한다.

        */
        void            DoPortal(SMessage* msg);

protected:
        /**
            클라이언트에 포탈을 열도록 요청 한다.

            \param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void            Update(DWORD dwCharDbNum = 0);

		/**
			우리는 포탈 타기전에 죽은 유저들을 부활 시킨다.

		 */
		void			Rebirth();

        /**
            캐릭터를 이동 시킨다.            

         */
        void            Teleport(const SServerPortalMessage* pPortalMsg);
};

} // end namespace

#endif