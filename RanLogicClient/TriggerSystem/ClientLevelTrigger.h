#ifndef CClientLevelTrigger_H
#define CClientLevelTrigger_H

#include "../../RanLogic/TriggerSystem/LevelTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ ���� Ʈ����
	���̵��� ���� ���� ��ũ��Ʈ�� �ε�, ���� ���� ó���� ���� �Ѵ�.
	
 */
class CClientLevelTrigger
	: public CClientTrigger<CLevelTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_LEVEL; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**	
			���� ������ ����� ��ũ��Ʈ ������ ������
			�ٽ� ��ũ��Ʈ�� �ε� �ϹǷν� ��Ƽ�� ��ũ��Ʈ�� ���� �� �ִ�.

		 */
virtual void		Load();
};

} // end namespace

#endif