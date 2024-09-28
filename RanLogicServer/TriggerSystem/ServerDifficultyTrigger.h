#ifndef CServerDifficultyTrigger_H
#define CServerDifficultyTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� �δ� ���̵� Ʈ����
	���̵��� Ŭ��� �ʿ䰡 ���� ���������� �ʿ�� �Ѵ�.
	���� ����, ���ݷ� ���� ���� �� �� �ִ�.
	
 */
class CServerDifficultyTrigger
	: public CServerTrigger<CTrigger>
{
public:
		CServerDifficultyTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_DIFFICULTY; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			���̵� Ʈ���Ű� ���۵Ǹ� �ʿ� ���̵��� ���� �Ѵ�.
			
		 */
virtual	void		Start(CTriggerInstance* pInstance) const;

		/**
			���ε� �������̽� �Լ���

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
		float		m_fMultipleHP;		//@ HP ���
		float		m_fMultipleAttack;	//@ ���ݷ� ���
		float		m_fMultipleDefense;	//@ ���� ���
        float		m_fMultipleExp;	    //@ ����ġ ���
		float		m_fMultipleDrop;	//@ ������ ������ ����� ���
};

} // end namespace

#endif