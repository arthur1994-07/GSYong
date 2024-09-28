#ifndef CClientPortalCloseTrigger_H
#define CClientPortalCloseTrigger_H

#include "../../RanLogic/TriggerSystem/PortalCloseTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	Ŭ���̾�Ʈ ��Ż �ݱ� Ʈ����
	�������� ���۽� ��Ż�� ���� �� ��� �Ѵ�.
	
 */
class CClientPortalCloseTrigger
	: public CClientTrigger<CPortalCloseTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_PORTALCLOSE; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			Ʈ���Ű� ���۵Ǹ� ��Ż�� �ݾ� �ش�.

		 */
		void        Start(CTriggerInstance* pInstance) const;
};

} // end namespace

#endif