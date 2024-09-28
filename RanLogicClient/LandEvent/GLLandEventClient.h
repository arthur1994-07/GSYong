
#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "../../RanLogic/LandEvent/GLLandEvent.h"
#include "../../RanLogic/LandEvent/GLLandEventGenerator.h"

#include "../Land/GLLandManClient.h"
#include "./GLLandEventNodeClient.h"

typedef std::map <DWORD, CEVENT_NODE_CLIENT*> EVENTNODECLIENTMAP;
typedef std::pair<DWORD, CEVENT_NODE_CLIENT*> EVENTNODECLIENTPAIR;
typedef EVENTNODECLIENTMAP::iterator          EVENTNODECLIENTMAP_ITER;

//-------------------------------------------------------------------//
class GLGaeaClient;


class GLLandEventClient : public GLLandEvent
{

public :
	GLLandEventClient(GLGaeaClient* pGaeaClient);
	virtual ~GLLandEventClient();

private :
	EVENTNODECLIENTMAP m_cEventInfoMap;
    GLGaeaClient* m_pGaeaClient;

public :
	template<typename LANDEVENTNODE>
	LANDEVENTNODE* NewEvent( const DWORD dwGUID, GLLandManClient* pLandManClient, const D3DXVECTOR3& vInPos, const FLOAT fTimeEnd, const FLOAT fTimeEvent = 0.0f, const FLOAT fTimeCurrent = 0.0f )
	{
		if ( !pLandManClient )
		{
			return NULL;
		}

		if ( dwGUID == UINT_MAX )
		{
			return NULL;
		}

		LANDEVENTNODE* pEvent = SLANDEVENT_GENERATOR<LANDEVENTNODE>::Generate( fTimeEnd, fTimeEvent, fTimeCurrent );

		if ( pEvent )
		{
			m_cEventInfoMap[dwGUID] = pEvent;

			pEvent->m_dwGlobID  = dwGUID;
			pEvent->m_sMapID    = pLandManClient->GetBaseMapID();
			pEvent->m_vPosition = vInPos;
		}

		return pEvent;
	}

	CEVENT_NODE_CLIENT* GetEvent ( const DWORD dwGUID )
	{
		EVENTNODECLIENTMAP_ITER MapIter = m_cEventInfoMap.find(dwGUID);

		if ( MapIter != m_cEventInfoMap.end() )
		{
			return (MapIter->second);
		}

		return NULL;
	}

    template<typename _Fn1>
    void ForEach ( _Fn1& cFunc )
    {
        std::for_each( m_cEventInfoMap.begin(), m_cEventInfoMap.end(), cFunc );
    }


public :
	virtual void CleanUp();
	virtual void FrameMove( const FLOAT fElapsedTime );

public :
	virtual void MsgProcess(NET_MSG_GENERIC* nmg);

};
