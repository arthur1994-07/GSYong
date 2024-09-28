
#pragma once

#include "../../SigmaCore/Memory/CMemPool.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

class GLLandEvent;

//-------------------------------------------------------------------//

template<typename LANDEVENTNODE>
struct SLANDEVENT_GENERATOR
{
public:
	struct CS
	{
		CRITICAL_SECTION CSPCLock;

		CS         () { InitializeCriticalSection(&CSPCLock); }
		~CS        () { DeleteCriticalSection(&CSPCLock);     }
		void Enter () { EnterCriticalSection(&CSPCLock);      }
		void Leave () { LeaveCriticalSection(&CSPCLock);      }
	};

public:
    static sc::CMemPool<LANDEVENTNODE> Memory;
	static CS		               Lock;

public :
	static LANDEVENTNODE* Generate ( const FLOAT fTimeEnd, const FLOAT fTimeEvent, const FLOAT fTimeCurrent )
	{
		Lock.Enter();

		LANDEVENTNODE* pData = Memory.New();

		Lock.Leave();

		if ( pData )
		{
			pData->m_fTimeEnd     = fTimeEnd;
			pData->m_fTimeEvent   = fTimeEvent;
			pData->m_fTimeCurrent = fTimeCurrent; 
			pData->m_bInit        = FALSE;
			pData->m_bEvent       = FALSE; 
		}

		return pData;
	}

	static void Restore ( LANDEVENTNODE* pEventInfo )
	{
		if ( pEventInfo )
		{
			Lock.Enter();

			Memory.Release ( pEventInfo );

			Lock.Leave();
		}
	}

public :
	SLANDEVENT_GENERATOR();
};

template<typename LANDEVENTNODE> sc::CMemPool<LANDEVENTNODE>                          SLANDEVENT_GENERATOR<LANDEVENTNODE>::Memory;
template<typename LANDEVENTNODE> typename SLANDEVENT_GENERATOR<LANDEVENTNODE>::CS SLANDEVENT_GENERATOR<LANDEVENTNODE>::Lock;
