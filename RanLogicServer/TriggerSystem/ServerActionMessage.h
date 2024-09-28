#pragma once

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

	// ĳ���� �׼� �޽���;
	struct SServerActionMessage : public SMessage
	{
		void Command ( CTriggerInstance* _pInstance );

		DWORD dwCharDbNum;
		EMACTIONTYPE emCurrentAction;	// ���� �׼�;
		EMACTIONTYPE emNextAction;		// ���� �׼�;
	};

}