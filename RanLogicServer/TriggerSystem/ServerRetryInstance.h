#ifndef CServerRetryInstance_H
#define CServerRetryInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	재도전을 제어 한다.
    일반 부활을 막고 스킬 부활은 되게 한다.
    죽음, 부활까지 스테이지가 처리하면 스테이지 복잡도가 높아지므로
    죽음, 부활은 이곳에서 처리 한다.
	
 */
class CServerRetryInstance
    : public CServerTriggerInstance
{
public:
        CServerRetryInstance();

		void			StageClear(SMessage* msg);
		void			StageFail(SMessage* msg);

        /**
        	유저가 진입했을 경우 호출 된다.
            우리는 입장하는 유저들을 모두 부활 불가능 하게 막는다.

         */
        void            EntryPC(SMessage* msg);

        /**
            유저가 맵에서 귀환 될 경우 호출 된다.
			우리는 유저가 귀환 될 때 재도전 시간을 없애 준다.
			이것은 만약에 재도전 중일때 귀환이 올 경우를 위해서 이다.

         */
        void            ReturnPC(SMessage* msg);

        /**
            유저가 맵에서 빠져나갈 경우 호출 된다.
			우리는 유저가 빠져 나갈 때 부활 불가능을 풀어 준다.

         */
        void            ExitPC(SMessage* msg);

		// 플레이어가 파티를 탈퇴했을 경우 호출 된다;
		void			PartyOutPC(SMessage* msg);

		// 플레이어가 죽었을 경우 호출된다;
        void            DiePC(SMessage* msg);

        /**
        	재도전에 대한 처리를 수행 한다.

            RetryAns : 응답
            RetryReq : 요청

         */
        void            RetryAns(SMessage* msg);
        void            RetryReq(SMessage* msg);

		/**
			프레임 처리를 수행 한다.
			우리는 재도전 시간 경과를 처리 한다.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

protected:
        /**
            캐릭터의 부활 유무를 설정 한다.
            중복 코드 방지

            \param dwCharDbNum 캐릭터 DB ID
            \param bEnable 부활 가능하게 할지 유무
         */
        void            SetResurrectPVE(DWORD dwCharDbNum, bool bEnable);

        /**
            재도전 결과를 처리를 한다.
            
         */
        void            Process(bool bOk, int nWhy = -1 );

        /**
            재도전에 OK 했을 경우 재도전에 필요한
            돈을 체크 한다.

         */
        bool            CheckRetry(DWORD dwCharDbNum);
        bool            AllCheckRetry();

		/**
			재도전에 대한 결과를 파티원에 전송한다.
			이것은 재도전이 실패했을 경우에만 처리 된다.

		 */
		void			SendResultMessage(int nWhy, DWORD dwChaDbNum=0);

        /**
            재도전 비용을 처리 한다.

         */
        void            CostRetry();

        /**
            클라이언트에 재도전을 요청 한다.

         */
        void            ReqRetry();
		void            ReqRetry(DWORD dwCharDbNum);

		/**
			재도전에 대한 처리가 완료되면
			부활 시킨 후 다음 처리를 수행 한다.

		 */
		void			Rebirth();

		/**
			부활시 필요한 금전을 계산 한다.

		 */
		const LONGLONG  RetryMoney(GLChar* pChar);

        CHARDBNUM_LIST  m_entrance; //@ 현재 맵에 입장한 인원 리스트
		CHARDBNUM_LIST  m_die;		//@ 현재 맵에 죽은 인원 리스트
		CHARDBNUM_LIST  m_rem;		//@ 재입장 허락 남은 인원 리스트
		CHARDBNUM_LIST  m_retry;	//@ 재입장 인원 리스트

        float           m_time;     //@ 재도전 타이머
		bool			m_bIsRewardReqest; // 보상요청을 하였는가?

		bool			m_bRetring;
};

} // end namespace

#endif