#ifndef CServerPositioningClearInstance_H
#define CServerPositioningClearInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	특정 위치에 인원수가 위치하면
    완료되는 상태를 처리 한다.
    	
 */
class CServerPositioningClearInstance
    : public CServerTriggerInstance
{
public:
        CServerPositioningClearInstance();

        /**
            인스턴스가 생성될 때 호출 된다.

         */
        void		    Start();

		/**
			프레임 처리를 수행 한다.
			이동 포인트에 위치 하는지 살펴 완료 처리를 한다.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

        /**
        	유저가 진입했을 경우 호출 된다.

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
            현재 상태를 갱신 한다.
            우리는 찍어야할 위치를 클라에 알려 준다.

            \param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void			Update(DWORD dwCharDbNum = 0);

		/**
			트리거로 부터 필요한 정보를 가져 온다.
			매번 트리거를 형변환해서 쓸수도 있지만 불편해서 이렇게 미리 가져와서 쓴다.

		 */
		void			Parsing();
    
        /**
            클리어를 체크 한다.

         */
        bool            Check();

        /**
            현재 위치가 민맥스에 포함되는지 살핀다.

         */
        bool            InMinMax(const D3DXVECTOR3& pos);

		D3DXVECTOR2		m_minipos;	//@ 클라이언트에 보낼 미니맵 좌표
		D3DXVECTOR2		m_min;		//@ 계산에 사용될 AABB 최소점
		D3DXVECTOR2		m_max;		//@ 계산에 사용될 AABB 최대점
        int				m_nNow;	    //@ 현재 인원 수
		int				m_count;	//@ 최소 허용 인원수, 이 인원수만큼 영역에 있어야 통과 된다.

        CHARDBNUM_LIST  m_entrance; //@ 입장한 인원 리스트
        bool            m_completed;//@ 완료 유무
};

} // end namespace

#endif