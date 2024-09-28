#ifndef CLevelTrigger_H
#define CLevelTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	���� Ʈ����
	
	���̵��� ���� Ʈ���ŵ��� ������ ó���ϱ� ���� ���� �Ǿ���.
	�ϳ��� ���� ��Ȳ�� ���� �������� ����(���̵�, ���)���� ������ ����� �� �ִ�.

	�̸� ���ؼ��� ���� Ʈ���Ŵ� ���� Ʈ���� ���ϸ��� ������ Ʈ���Ÿ� �ε� �Ѵ�.
	���� ������ ����� �����ؼ� �ʿ� ���Խ� ���� ����� ������ �ߵ� ��ų �� �ִ�.

 */
class CLevelTrigger
	: public CTrigger
{
public:
		CLevelTrigger();

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			���� ��ũ��Ʈ�� �ε� �Ѵ�.ear

		 */
virtual void		    Load() {}

		/**
			���ε� �������̽� �Լ���

		 */
		void		    SetLevel(int level) { m_level = level; }
		const int	    GetLevel() const { return m_level; }

		void		    SetUserLevel(int minlevel, int maxlevel) { m_minlevel = minlevel; m_maxlevel = maxlevel; }
		const int		GetUserMinLevel() const { return m_minlevel; }
		const int		GetUserMaxLevel() const { return m_maxlevel; }

		void		    SetFile(TSTRING file) { m_file = file; }
		const TSTRING	GetFile() const { return m_file; }

protected:
		TSTRING		    m_file;			//@ ���� ���� ���ϸ�
		int			    m_level;		//@ ���� ����
		int			    m_minlevel;		//@ ��� ���� �ּ� ���� ����
		int			    m_maxlevel;		//@ ��� ���� �ִ� ���� ����
};

} // end namespace

#endif