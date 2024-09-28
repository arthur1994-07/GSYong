#ifndef CServerDifficultyTrigger_H
#define CServerDifficultyTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 인던 난이도 트리거
	난이도는 클라는 필요가 없고 서버에서만 필요로 한다.
	몬스터 방어력, 공격력 등을 조절 할 수 있다.
	
 */
class CServerDifficultyTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerDifficultyTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_DIFFICULTY; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			난이도 트리거가 시작되면 맵에 난이도를 설정 한다.
			
		 */
virtual	void		Start(CTriggerInstance* pInstance) const;

		/**
			바인딩 인터페이스 함수들

		 */
		void		SetMultipleHP(float fMultiple) { m_fMultipleHP = fMultiple; }
		const float	GetMultipleHP() const { return m_fMultipleHP; }

		void		SetMultipleAttack(float fMultiple) { m_fMultipleAttack = fMultiple; }
		const float	GetMultipleAttack() const { return m_fMultipleAttack; }

		void		SetMultipleDefense(float fMultiple) { m_fMultipleDefense = fMultiple; }
		const float	GetMultipleDefense() const { return m_fMultipleDefense; }

        void		SetMultipleExp(float fMultiple) { m_fMultipleExp = fMultiple; }
        const float	GetMultipleExp() const { return m_fMultipleExp; }

		void		SetMultipleDrop(float fMultiple) { m_fMultipleDrop = fMultiple; }
		const float	GetMultipleDrop() const { return m_fMultipleDrop; }

private:
		float		m_fMultipleHP;		//@ HP 배수
		float		m_fMultipleAttack;	//@ 공격력 배수
		float		m_fMultipleDefense;	//@ 방어력 배수
        float		m_fMultipleExp;	    //@ 경험치 배수
		float		m_fMultipleDrop;	//@ 몬스터의 아이템 드랍율 배수
};

} // end namespace

#endif