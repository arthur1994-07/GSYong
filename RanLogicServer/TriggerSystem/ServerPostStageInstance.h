#ifndef CServerPostStageInstance_H
#define CServerPostStageInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	스테이지 후 처리
    우리는 스테이지 종료(성공 or 실패) 가 되면 종료 상태 UI 를 임의의 시간 동안 출력 후
    실제 스테이지 종료를 처리 한다. 이 딜레이를 위해 만들었으며
    만약 스테이지에 이 모든 처리가 있으면 복잡도가 높아 짐으로 분리하면 관리도 편해서 만듬

 */
class CServerPostStageInstance
    : public CServerTriggerInstance
{
public:
        CServerPostStageInstance();

		/**
			종료 처리를 수행 한다.
			우리는 실제 종료 처리를 하지 않아야 한다.
			왜냐하면 만약 스테이지 성공 직 후 인던 시간이 끝나거나 파티가 깨진는 등의
			종료 상태가 된다면 이를 처리 하기 위해 후 처리가 필요하기 때문 이다.

		 */
		void		Exit();

        /**
            메시지를 통해 스테이지 성공 유무를 설정 한다.
            스테이지 결과 메시지를 받아야 하므로
            CServerPostStageInstance 는 반드시 스테이지가 동작될 때
            같이 동작되도록 생성 해 놓아야 함에 주의 한다.

         */
        void        StageClear(SMessage* msg);

        /**
            만약 귀환이 귀속 시간 종료에 의한 귀환 이라면
            우리는 스테이지 후 처리를 수행 한다.

         */
        void        ReturnPC(SMessage* msg);

        /**
            프레임 처리를 수행 한다.

         */
        void		FrameMove(float fAccTime, float fElapsedTime);

protected:
        /**
            실패 or 성공 메시지가 전달 되면
            포스트스테이지를 가동 시킨다.

         */
        void        Run();

        
        /**
            우리는 포스트 스테이지가 끝나게 되면
            이제서야 실제 스테이지의 종료를 처리 한다.

         */
        void        PostStage();
        
        float       m_fTime;        //@ 종료 대기 시간
        bool        m_bCompleted;   //@ 스테이지 성공 유무
        bool        m_bLast;        //@ 최종 스테이지 유무
		bool        m_bTimeOver;    //@ 귀속 시간 종료 유무
		bool        m_bFireParty;   //@ 파티 파괴에 의한 종료 유무
};

} // end namespace

#endif