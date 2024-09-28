#ifndef CPortalCloseTrigger_H
#define CPortalCloseTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	��Ż �ݱ� Ʈ����
	
	������ Ŭ�󿡼� ��Ż�� �ݰ� Ŭ���� �� ��Ż�� �� �� �ִ�. 
	������ ��Ż�� ���� �ϹǷ� �̸� �����ϱ� ���ؼ��� ����, Ŭ�� ��� �ʿ��ϴ�.
	������ ���� ��� ������ �� �� Ŭ��� �˼� �����Ƿ� Ŭ��� ������ ������ ������ ���� �Ѵ�.

 */
class CPortalCloseTrigger
	: public CTrigger
{
public:
		CPortalCloseTrigger();

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			���ε� �������̽� �Լ���

		 */
		void		SetFromGateID(DWORD id) { m_fromGateID = id; }
		const DWORD	GetFromGateID() const { return m_fromGateID; }

protected:
		int			m_fromGateID;	//@ ���� �ʿ��� Ÿ�� ����Ʈ ID
};

} // end namespace

#endif