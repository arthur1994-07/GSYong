#pragma once

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

	// 캐릭터 액션 메시지;
	struct SServerActionMessage : public SMessage
	{
		void Command ( CTriggerInstance* _pInstance );

		DWORD dwCharDbNum;
		EMACTIONTYPE emCurrentAction;	// 원래 액션;
		EMACTIONTYPE emNextAction;		// 변경 액션;
	};

}