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
	���� ���� Ʈ����
    ���������� ������ ���� ó���� �Ѵٰų� � ���� �� �ִ�.    
	
 */
class CServerRewardTrigger
    : public CServerTrigger<CTrigger>
{
public:
        CServerRewardTrigger();

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_REWARD; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	Ʈ���ſ� ¦�� �Ǵ� �ν��Ͻ��� ���� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			���ε� �������̽� �Լ���

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
			���� ���� ó�� ���� �Լ���

		 */
		const LONGLONG		RewardMoney(const WORD wLevel, const int nStage) const;
		const int			RewardExp(const WORD wLevel, const int nStage,  const WORD wMax, const LONGLONG needexp) const;
		const int			RewardMVP(const int nStage, const DWORD dwDamage, const DWORD dwHeal) const;

protected:
        float               m_waittime;     //@ �Ϸ� �� ��� �ð�
		LONGLONG			m_money;		//@ ���� �Ӵ�(���� ���� �ʿ��� ��)
		float				m_mvp;			//@ MVP ������(���� ���� �ʿ��� ��)
		float				m_exp;			//@ EXP ������(���� ���� �ʿ��� ��)
		int					m_stage;		//@ ���� ��������, �� �������� ���� ���� �ȴ�.
};

} // end namespace

#endif