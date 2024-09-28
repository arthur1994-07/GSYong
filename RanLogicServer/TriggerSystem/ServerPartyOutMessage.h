#pragma once

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

	// ÆÄÆ¼ Å»Åð ¸Þ½ÃÁö;
	struct SServerPartyOutMessage : public SMessage
	{
		void Command ( CTriggerInstance* _pInstance );

	    DWORD dwCharDbNum;
	};

}