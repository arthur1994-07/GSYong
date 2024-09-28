#ifndef CServerStageInstance_H
#define CServerStageInstance_H

#include "ServerTriggerInstance.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
    기본적인 스테이지를 처리 한다.

	시간이 흐르고 여러개의 미션을 가지고 있으며
	해당 미션이 완료되면 클리어 되는 스테이지를 정의 한다.
	
 */
class CServerStageInstance
    : public CServerTriggerInstance
{
public:
        CServerStageInstance();

        /**
            현재 상태를 갱신 한다.
            갱신은 클라이언트에 현재 상태를 전달함을 의미 한다.

        	\param dwCharDbNum 상태를 전송할 캐릭터 DB ID, 0 일경우 맵 전체에 전달.
         */
        void            Update(DWORD dwCharDbNum = 0, int nState = GLMSG::STAGE_STATE_ONGOING);

        /**
            인스턴스가 생성될 때 호출 된다.
            시작 될때 상태를 전송 한다.

         */
        void		    Start();

        /**
            인스턴스가 삭제될 때 호출 된다.
            우리는 이때 마지막으로 상태를 전송 한다.
            스테이지 시간 오버 메시지를 전송 한다.

         */
        void            Exit();

        /**
            게임이 끝나기전에 귀환 메시지가 오면
            이것은 귀속 시간이 종료 되었을 경우 이다.

         */
        void            ReturnPC(SMessage* msg);

		/**
			프레임 처리를 수행 한다.
			우리는 10초마다 유저들에게 서버와 동기화를 맞춘다.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

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

		// 플레이어가 액션을 수행했을 경우 호출 된다;
		void			ActionPC(SMessage* msg);

        /**
        	PC 가 죽었을 경우 호출 된다.

         */
        void            DiePC(SMessage* msg);

        /**
            스테이지에서 해야할 미션이 완료되었을 경우 호출 된다.
            우리는 미션수를 카운트해서 모두 완료시 종료 처리 한다.

        */
        void            MissionCompleted(SMessage* msg);

protected:
        /**
            최종 완료를 처리 한다.
            우리는 클라이언트에 미션 완료를 알리고 종료 처리 한다.

         */
        void            StageCompleted();

        /**
            스테이지 실패를 처리 한다.
            시간이 모두 종료되었을 경우 호출 된다.

         */
        void            StageFailed();

        /**
            스테이지 정보를 얻는다.
            현재 스테이지와 플레이 타임을 미리 구해 놓는다.
            향후 Update 시에 중복 사용 된다.

         */
        bool            GetInfo();

        /**
            스테이지 결과에 따른 패킷을 전송 한다.

         */
        void            SendStageClear(bool bCompleted);

        int             m_stage;    //@ 현재 스테이지
        int             m_playtime; //@ 스테이지 총 진행 시간(sec)
        int             m_mission;  //@ 처리 해야 할 총 미션수
        bool            m_last;     //@ 마지막 스테이지 유무
        bool            m_exit;     //@ 스테이지 종료 유무, 시간 종료와 그렇지 않은 경우를 구분 한다.

        int             m_completed;//@ 완료한 미션수

        CHARDBNUM_LIST  m_entrance; //@ 현재 맵에 입장한 인원 리스트
        CHARDBNUM_LIST  m_die;      //@ 현재 맵에 죽은 인원 리스트
};

} // end namespace

#endif