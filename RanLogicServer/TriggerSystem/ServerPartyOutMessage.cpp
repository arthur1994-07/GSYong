#include "pch.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../RanLogic/TriggerSystem/TriggerInstance.h"

#include "./ServerPartyOutMessage.h"
#include "./ServerTriggerInstance.h"

namespace TriggerSystem
{

	void SServerPartyOutMessage::Command ( CTriggerInstance* _pInstance )
	{    
		CServerTriggerInstance* pServerInstance = 0;

		if ( _pInstance && _pInstance->CastInstance( pServerInstance ) )
		{
			pServerInstance->PartyOutPC( this );
		}
	}

}