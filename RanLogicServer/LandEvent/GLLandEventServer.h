
#pragma once

#include <map>
#include <string>
#include <algorithm>
#include <boost/circular_buffer.hpp>
#include "../../EngineLib/G-Logic/GLQuadTree.h"

#include "../../RanLogic/LandEvent/GLLandEvent.h"
#include "../../RanLogic/LandEvent/GLLandEventGenerator.h"
#include "../ServerActor.h"
#include "./GLLandEventNodeServer.h"

typedef std::map <DWORD, CEVENT_NODE_SERVER*> EVENTNODESERVERMAP;
typedef std::pair<DWORD, CEVENT_NODE_SERVER*> EVENTNODESERVERPAIR;
typedef EVENTNODESERVERMAP::iterator          EVENTNODESERVERMAP_ITER;

class GLLandNode;
typedef GLQuadTree<GLLandNode> LANDQUADTREE;

class GLLandEventServer : public GLLandEvent
{

public :
    //GLLandEventServer(GLGaeaServer* pServer, sc::CMList<DWORD>& GUIDList, LANDQUADTREE& LandTree);    
    GLLandEventServer(GLGaeaServer* pServer, boost::circular_buffer<unsigned int>* pFreeId, LANDQUADTREE& LandTree);
    GLLandEventServer(GLGaeaServer* pServer);
    virtual ~GLLandEventServer();

private :
    GLGaeaServer* m_pGLGaeaServer;
    EVENTNODESERVERMAP m_cEventInfoMap;
    boost::circular_buffer<unsigned int>* m_pGUIDList;
    LANDQUADTREE*      m_pLandTree;

public :

    template<typename LANDEVENTNODE>
	LANDEVENTNODE* NewEvent(DWORD& dwOutGUID, const Faction::GLActorID& sOwnerID, GLLandMan* pInLandMan, const D3DXVECTOR3& vInPos, const FLOAT fTimeEnd, const FLOAT fTimeEvent = 0.0f, const FLOAT fTimeCurrent = 0.0f )
    {
        dwOutGUID = -1;

        if ( !m_pGUIDList || !m_pLandTree || !pInLandMan )
        {
            return NULL;
        }

        LANDQUADNODE* pLandNode = m_pLandTree->FindNode ( (int)vInPos.x, (int)vInPos.z );
        if ( !pLandNode )
        {
            return NULL;
        }

        //if ( !m_pGUIDList->GetHead ( dwOutGUID ) )
        //    return NULL;
        //m_pGUIDList->DelHead ();
        if (m_pGUIDList->empty())
            return NULL;
        dwOutGUID = m_pGUIDList->front();
        m_pGUIDList->pop_front();

        LANDEVENTNODE* pEvent = SLANDEVENT_GENERATOR<LANDEVENTNODE>::Generate( fTimeEnd, fTimeEvent, fTimeCurrent );

        if ( pEvent )
        {
            m_cEventInfoMap[dwOutGUID] = pEvent;

            pEvent->m_sOwnerID    = sOwnerID;
            pEvent->m_pGaeaServer = m_pGLGaeaServer;
            pEvent->m_pLandMan    = pInLandMan;
            pEvent->m_dwGlobID    = dwOutGUID;
            pEvent->m_sMapID      = pInLandMan->GetBaseMapID();
            pEvent->m_vPosition   = vInPos;
            pEvent->m_pQuadNode   = pLandNode;										          // - 트리 노드.
            pEvent->m_pCellList   = pLandNode->pData->m_LandEventNodeList.ADDHEAD ( pEvent ); // - 트리 노드.

            ServerActor* pActor = m_pGLGaeaServer->GetActor( pInLandMan, sOwnerID );

            if ( pActor )
            {
                pActor->RegisterLandEvent( dwOutGUID );
            }
        }

        return pEvent;
    }

    CEVENT_NODE_SERVER* GetEvent ( const DWORD dwGUID ) const
    {
        EVENTNODESERVERMAP::const_iterator MapIter = m_cEventInfoMap.find(dwGUID);

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
    virtual void FrameMove(const FLOAT fElapsedTime);

public :
    virtual void MsgProcess( NET_MSG_GENERIC* nmg );

public :
    const BOOL RemoveEvent(const DWORD dwGUID);
};
