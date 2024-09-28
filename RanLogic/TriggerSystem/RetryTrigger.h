#ifndef CRetryTrigger_H
#define CRetryTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	�絵�� Ʈ����

	�δ����� ��� ��Ƽ������ �׾��� ���
	�ٽ� �絵���� ó�� �ϱ� ���� Ʈ����

 */
class CRetryTrigger
	: public CTrigger
{
public:
		CRetryTrigger();

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

		 */
		void		    SetTime(int time) { m_time = time; }
		const int	    GetTime() const { return m_time; }
		void		    SetMoney(const LONGLONG money) { m_money = money; }
		const LONGLONG	GetMoney() const { return m_money; }
		void		    SetControl(const float contrl) { m_contrl = contrl; }
		const float		GetControl() const { return m_contrl; }

		/**
			�絵�� �ݾ� ��� �Լ�

			\param wLevel ����Ϸ��� ĳ������ ����
			\param nStage ���� ��������, ù �������� �� ��� 1
			\return �絵�� �ݾ�
		 */
		const LONGLONG	RetryMoney(const WORD wLevel, const int nStage) const;

protected:
		int			    m_time;		//@ �絵�� ���� ��� �ð�
		LONGLONG		m_money;	//@ ���� �Ӵ�(���� ���� �ʿ��� ��)
		float			m_contrl;	//@ ������(���� ���� �ʿ��� ��)
};

} // end namespace

#endif