#include "pch.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"
#include "../../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "../Land/GLLandMan.h"
#include "./GLLandEventServer.h"

#include "../../SigmaCore/DebugInclude.h"

//-------------------------------------------------------------------//

void GLLandEventServer::MsgProcess( NET_MSG_GENERIC* nmg )
{
    // Blank
}

//-------------------------------------------------------------------//

GLLandEventServer::GLLandEventServer(GLGaeaServer* pServer, boost::circular_buffer<unsigned int>* pFreeId, LANDQUADTREE& LandTree)
    : m_pGLGaeaServer(pServer)
    , m_pGUIDList(pFreeId)
    , m_pLandTree(&LandTree)
{
    // Blank
}

GLLandEventServer::GLLandEventServer(GLGaeaServer* pServer)
    : m_pGLGaeaServer(pServer)
{
}

GLLandEventServer::~GLLandEventServer()
{
    CleanUp();
}

void GLLandEventServer::CleanUp()
{	
    EVENTNODESERVERMAP_ITER iter = m_cEventInfoMap.begin();

    for ( ; iter != m_cEventInfoMap.end(); )
    {
        CEVENT_NODE_SERVER* pEvent = iter->second; 

        if ( pEvent )
        {
            if ( m_pGUIDList )
            {
                DWORD dwGUID = iter->first;
                //m_pGUIDList->AddTail(dwGUID);
                m_pGUIDList->push_back(dwGUID);
            }

            if ( pEvent->m_pQuadNode && pEvent->m_pQuadNode->pData ) 
            {
                if ( pEvent->m_pQuadNode->pData->m_LandEventNodeList.DELNODE ( pEvent->m_pCellList ) == false )
				{
					const SNATIVEID& _eventMapID(pEvent->m_sMapID);
					if ( pEvent->m_pLandMan == NULL )
					{
						sc::writeLogError(sc::string::format("GLLandEventServer::CleanUp() : pEvent->m_pCellList is invalid _ EventGaeaMapID(%1%/%2%", 
							_eventMapID.Mid(), _eventMapID.Sid()));
					}
					else
					{
						const MapID& _landMapID(pEvent->m_pLandMan->GetMapID());
						sc::writeLogError(sc::string::format("GLLandEventServer::CleanUp() : pEvent->m_pCellList is invalid _ LandMapID(%1%/%2%, %3%/%4%), EventMapID(%5%/%6%)", 
							_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid(), 
							_eventMapID.Mid(), _eventMapID.Sid()));
					}
				}
            }

            pEvent->Clean();

            delete (pEvent);
        }

        iter = m_cEventInfoMap.erase(iter);
    }
}

void GLLandEventServer::FrameMove(const FLOAT fElapsedTime)
{
    EVENTNODESERVERMAP_ITER iter = m_cEventInfoMap.begin();

    for ( ; iter != m_cEventInfoMap.end(); )
    {
        CEVENT_NODE_SERVER* pEvent = iter->second; 

        if ( !pEvent )
        {
            iter = m_cEventInfoMap.erase(iter);

            continue;
        }

        pEvent->FrameMove( fElapsedTime );

        if ( pEvent->IsEnd() )
        {
            if ( m_pGUIDList )
            {
                DWORD dwGUID = iter->first;
                //m_pGUIDList->AddTail(dwGUID);
                m_pGUIDList->push_back(dwGUID);
            }
            
			if ( pEvent->m_pQuadNode && pEvent->m_pQuadNode->pData ) 
			{
				if ( pEvent->m_pQuadNode->pData->m_LandEventNodeList.DELNODE ( pEvent->m_pCellList ) == false )
				{
					const SNATIVEID& _eventMapID(pEvent->m_sMapID);
					if ( pEvent->m_pLandMan == NULL )
					{
						sc::writeLogError(sc::string::format("GLLandEventServer::FrameMove() : pEvent->m_pCellList is invalid _ EventGaeaMapID(%1%/%2%", 
							_eventMapID.Mid(), _eventMapID.Sid()));
					}
					else
					{
						const MapID& _landMapID(pEvent->m_pLandMan->GetMapID());
						sc::writeLogError(sc::string::format("GLLandEventServer::FrameMove() : pEvent->m_pCellList is invalid _ LandMapID(%1%/%2%, %3%/%4%), EventMapID(%5%/%6%)", 
							_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid(), 
							_eventMapID.Mid(), _eventMapID.Sid()));
					}
				}
			}

            ServerActor* pActor =
				m_pGLGaeaServer->GetActor(
				pEvent->m_pLandMan,
				pEvent->m_sOwnerID );

            if ( pActor )
            {
                pActor->RemoveLandEvent(
					pEvent->m_dwGlobID );
            }

            pEvent->Clean();
            pEvent->Restore();

            iter = m_cEventInfoMap.erase(iter);

            continue;
        }

        ++iter;
    }
}

const BOOL GLLandEventServer::RemoveEvent(const DWORD dwGUID)
{
    EVENTNODESERVERMAP_ITER iter = m_cEventInfoMap.find( dwGUID );

    if ( iter == m_cEventInfoMap.end() )
    {
        return FALSE;
    }

    CEVENT_NODE_SERVER* pEvent = iter->second; 

    if ( !pEvent )
    {
        return FALSE;
    }

    if ( m_pGUIDList )
    {
        DWORD dwGUID = iter->first;
        //m_pGUIDList->AddTail(dwGUID);
        m_pGUIDList->push_back(dwGUID);
    }

	if ( pEvent->m_pQuadNode && pEvent->m_pQuadNode->pData ) 
	{
		if ( pEvent->m_pQuadNode->pData->m_LandEventNodeList.DELNODE ( pEvent->m_pCellList ) == false )
		{
			const SNATIVEID& _eventMapID(pEvent->m_sMapID);
			if ( pEvent->m_pLandMan == NULL )
			{
				sc::writeLogError(sc::string::format("GLLandEventServer::RemoveEvent() : pEvent->m_pCellList is invalid _ EventGaeaMapID(%1%/%2%", 
					_eventMapID.Mid(), _eventMapID.Sid()));
			}
			else
			{
				const MapID& _landMapID(pEvent->m_pLandMan->GetMapID());
				sc::writeLogError(sc::string::format("GLLandEventServer::RemoveEvent() : pEvent->m_pCellList is invalid _ LandMapID(%1%/%2%, %3%/%4%), EventMapID(%5%/%6%)", 
					_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid(), 
					_eventMapID.Mid(), _eventMapID.Sid()));
			}
		}
	}


    ServerActor* pActor =
		m_pGLGaeaServer->GetActor(
		pEvent->m_pLandMan,
		pEvent->m_sOwnerID );

    if ( pActor )
    {
        pActor->RemoveLandEvent(
			pEvent->m_dwGlobID );
    }

    pEvent->Clean();
    pEvent->Restore();

    m_cEventInfoMap.erase(iter);

    return TRUE;
}
