#ifndef CServerReadyInstance_H
#define CServerReadyInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	인던 입장 상태를 처리 한다.
    현재 입장 수 및 대기 시간을 처리 한다.
	
 */
class CServerReadyInstance
    : public CServerTriggerInstance
{
public:
        CServerReadyInstance() : m_bUpdate(false), m_all(0), m_cur(0) {}

		/**
			인스턴스가 시작될 떄 호출 된다.
			여러가지 초기화를 수행한다.

		 */
		void			Start();

        /**
            현재 상태를 갱신 한다.
            갱신은 클라이언트에 현재 상태를 전달함을 의미 한다.

         */
        void            Update();

        /**
            인스턴스가 삭제될 때 호출 된다.
            우리는 이때 마지막으로 상태를 전송 한다.

         */
        void            Exit();

		/**
			프레임 처리를 수행 한다.
			우리는 m_bUpdate 를 살펴 패킷을 전송 한다.
            이렇게 하는 이유는 네트워크 부하를 줄이기 위함이다.
            만약 여러번 Update() 가 호출된다면 호출 될떄마다 패킷을 보내줘야 하는데
            이렇게 하면 한번에 변경상태를 몰아서 전송하므로 효율적이다.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

        /**
        	PC 가 입장했을 경우 호출 된다.
         */
        void            EntryPC(SMessage* msg);

        /**
        	PC 가 퇴장했을 경우 호출 된다.
         */
        void            ExitPC(SMessage* msg);

		// 플레이어가 파티를 탈퇴했을 경우 호출 된다;
		void			PartyOutPC(SMessage* msg);

protected:
        /**
            입장한 사람을 설정 한다.

         */
        void            SetCur(DWORD dwChaDbNum);
        void            RemoveCur(DWORD dwChaDbNum);

        /**
            캐릭터의 움직임 유무를 설정 한다.
            중복 코드 방지

            \param dwCharDbNum 캐릭터 DB ID
            \param bEnable 움직임 가능하게 할지 유무
         */
        void            SetMovement(DWORD dwCharDbNum, bool bEnable);
		
        bool            m_bUpdate;  //@ 패킷 전송 유무
        CHARDBNUM_LIST  m_entrance; //@ 이미 입장한 인원 리스트

		int				m_all;		//@ 전체 입장해야할 인원들
		int				m_cur;		//@ 현재 입장한 인원들
};

} // end namespace

#endif