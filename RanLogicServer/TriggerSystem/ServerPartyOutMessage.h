#pragma once

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

	// ��Ƽ Ż�� �޽���;
	struct SServerPartyOutMessage : public SMessage
	{
		void Command ( CTriggerInstance* _pInstance );

	    DWORD dwCharDbNum;
	};

}