#include "pch.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "./ServerActionMessage.h"
#include "./ServerTriggerInstance.h"

namespace TriggerSystem
{

	void SServerActionMessage::Command ( CTriggerInstance* _pInstance )
	{    
		CServerTriggerInstance* pServerInstance = 0;

		if ( _pInstance && _pInstance->CastInstance( pServerInstance ) )
		{
			pServerInstance->ActionPC( this );
		}
	}

}