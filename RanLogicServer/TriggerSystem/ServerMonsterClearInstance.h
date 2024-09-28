#ifndef CServerMonsterClearInstance_H
#define CServerMonsterClearInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	몬스터를 클리어 해야 하는 상태를 나타낸다.
    우리는 몬스터가 삭제 될 때마다 유저들에게 몬스터 상태를 전달 한다.
    또한 일정 시간마다 남은 시간을 전달 한다.
	
 */
class CServerMonsterClearInstance
    : public CServerTriggerInstance
{
public:
        CServerMonsterClearInstance();

        /**
            클리어 몬스터를 카운트 한다.

         */
        void        IncCur();
        int         GetCur();

        /**
            인스턴스가 생성될 때 호출 된다.

         */
        void		Start();

		/**
			프레임 처리를 수행 한다.
			갱신 패킷이 있다면 날려주는 역할을 한다.
			
		 */
        void		FrameMove(float fAccTime, float fElapsedTime);

        /**
            현재 상태를 갱신 한다.

            \param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void        Update(DWORD dwCharDbNum = 0);

        /**
        	유저가 진입했을 경우 호출 된다.

         */
        void        EntryPC(SMessage* msg);

        /**
            우리는 몬스터가 맵에서 삭제 되면 이를 카운트 한다.
            그런데 거의 동시다발적으로 몹이 죽을수도 있으므로
            m_bUpdate 변수를 두어 프레임에 상태를 전송하는 식으로 처리해야
            네트워크 부하가 적어 진다.

        */
        void        DieCrow(SMessage* msg);

protected:
        bool        m_update;   //@ 몬스터 수 갱신 유무
        int         m_count;    //@ 잡은 몬스터 수
};

} // end namespace

#endif