#ifndef CServerMapInfoTrigger_H
#define CServerMapInfoTrigger_H

#include "../../RanLogic/TriggerSystem/MapInfoTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	������ �� ������ ������.	
	
 */
class CServerMapInfoTrigger
    : public CServerTrigger<CMapInfoTrigger>
{
public:
		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�ڼ��� ������ CTrigger::GetType() ���� ����

		 */
virtual TriggerType		GetType() const { return TRIGGER_TYPE_MAPINFO; }

		/**
			��ũ��Ʈ���� ���ε��� ó�� �Ѵ�.
			
		 */
virtual	void			Bind(CTriggerScript* pScript);
};

} // end namespace

#endif