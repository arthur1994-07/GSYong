#ifndef CRetryTrigger_H
#define CRetryTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	재도전 트리거

	인던에서 모든 파티원들이 죽었을 경우
	다시 재도전을 처리 하기 위한 트리거

 */
class CRetryTrigger
	: public CTrigger
{
public:
		CRetryTrigger();

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void		    SetTime(int time) { m_time = time; }
		const int	    GetTime() const { return m_time; }
		void		    SetMoney(const LONGLONG money) { m_money = money; }
		const LONGLONG	GetMoney() const { return m_money; }
		void		    SetControl(const float contrl) { m_contrl = contrl; }
		const float		GetControl() const { return m_contrl; }

		/**
			재도전 금액 계산 함수

			\param wLevel 계산하려는 캐릭터의 레벨
			\param nStage 이전 스테이지, 첫 스테이지 일 경우 1
			\return 재도전 금액
		 */
		const LONGLONG	RetryMoney(const WORD wLevel, const int nStage) const;

protected:
		int			    m_time;		//@ 재도전 선택 대기 시간
		LONGLONG		m_money;	//@ 게임 머니(식을 위해 필요한 값)
		float			m_contrl;	//@ 조정값(식을 위해 필요한 값)
};

} // end namespace

#endif