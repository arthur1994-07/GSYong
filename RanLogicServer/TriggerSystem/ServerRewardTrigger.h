#ifndef CServerRewardTrigger_H
#define CServerRewardTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

#ifdef V
#pragma push_macro("V")
#undef V
#include "sqrat.h"
#pragma pop_macro("V")
#else
#include "sqrat.h"
#endif

namespace TriggerSystem
{

/**
	서버 보상 트리거
    스테이지가 끝나서 보상 처리르 한다거나 등에 쓰일 수 있다.    
	
 */
class CServerRewardTrigger
    : public CServerTrigger<CTrigger>
{
public:
        CServerRewardTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_REWARD; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			바인딩 인터페이스 함수들

		 */
        void		        SetWaitTime(const float waittime) { m_waittime = waittime; }
        const float	        GetWaitTime() const { return m_waittime; }
		void		        SetMoney(const LONGLONG money) { m_money = money; }
		const LONGLONG	    GetMoney() const { return m_money; }
		void		        SetMVPIncrease(const float mvp) { m_mvp = mvp; }
		const float	        GetMVPIncrease() const { return m_mvp; }
		void		        SetEXPIncrease(const float exp) { m_exp = exp; }
		const float	        GetEXPIncrease() const { return m_exp; }
		void		        SetMinStage(const int stage) { m_stage = stage; }
		const int	        GetMinStage() const { return m_stage; }

		/**
			보상 관련 처리 순수 함수들

		 */
		const LONGLONG		RewardMoney(const WORD wLevel, const int nStage) const;
		const int			RewardExp(const WORD wLevel, const int nStage,  const WORD wMax, const LONGLONG needexp) const;
		const int			RewardMVP(const int nStage, const DWORD dwDamage, const DWORD dwHeal) const;

protected:
        float               m_waittime;     //@ 완료 후 대기 시간
		LONGLONG			m_money;		//@ 게임 머니(식을 위해 필요한 값)
		float				m_mvp;			//@ MVP 보정값(식을 위해 필요한 값)
		float				m_exp;			//@ EXP 보정값(식을 위해 필요한 값)
		int					m_stage;		//@ 적용 스테이지, 이 스테이지 부터 적용 된다.
};

} // end namespace

#endif