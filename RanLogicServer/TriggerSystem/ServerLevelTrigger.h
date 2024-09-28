#ifndef CServerLevelTrigger_H
#define CServerLevelTrigger_H

#include "../../RanLogic/TriggerSystem/LevelTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	���� ���� Ʈ����
	���̵��� ���� ���� ��ũ��Ʈ�� �ε�, ���� ���� ó���� ���� �Ѵ�.
	
 */
class CServerLevelTrigger
    : public CServerTrigger<CLevelTrigger>	
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