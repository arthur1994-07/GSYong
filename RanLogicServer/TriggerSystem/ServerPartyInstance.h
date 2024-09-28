#ifndef CServerPartyInstance_H
#define CServerPartyInstance_H

#include "ServerTriggerInstance.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
	파티에 관련된 처리를 수행 한다.

    우리는 현재 파티에서 탈퇴하면 인던 밖으로 보낸다.
    향후 파티 관련 필요한 사항이 있다면 이곳에서 처리해 주면 된다.
	만약 이를 원정대 나 개인 등으로 변경 하고 싶다면 파티트리거를
	해당 트리거를 새로 만들어 쓰면 된다.

 */
class CServerPartyInstance
    : public CServerTriggerInstance
{
public:
        CServerPartyInstance();

		/**
            인스턴스가 생성될 때 호출 된다.
            시작 될때 상태를 전송 한다.

         */
        void		    Start();

        /**
        	유저가 진입했을 경우 호출 된다.
            우리는 해당 유저에게만 상태를 전송 한다.

         */
        void            EntryPC(SMessage* msg);

        /**
        	PC 가 퇴장했을 경우 호출 된다.

         */
        void            ExitPC(SMessage* msg);

		// 플레이어가 파티를 탈퇴했을 경우 호출 된다;
		void			PartyOutPC(SMessage* msg);
		
		/**
			프레임 처리를 수행 한다.
			파티에서 탈퇴한 유저를 돌려 보낸다.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

protected:
		// 입장 인원 삭제;
		const bool		ExitPartyMember ( const DWORD _dwChaNum );

		CHARDBNUM_LIST  m_entrance; //@ 현재 맵에 입장한 인원 리스트
		bool			m_bAutoOut;	//@ 파탙시 자동 아웃 유무
};

} // end namespace

#endif