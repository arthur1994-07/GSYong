#include "../pch.h"

#include "../../RanLogic/s_NetGlobal.h"
#include "../../RanLogic/MSG/GLContrlMsg.h"
#include "../GLGaeaClient.h"
#include "../Land/GLLandManClient.h"
#include "../../RanLogic/Skill/GLSkill.h"
#include "../Char/GLCharacter.h"

#include "./GLLandEventClient.h"
#include "./GLLandEventNodeClient.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------//

void GLLandEventClient::MsgProcess( NET_MSG_GENERIC* nmg )
{
	switch ( nmg->nType )
	{
    case NET_MSG_GCTRL_LANDEVENT_REMOVE:
        {
            GLMSG::SNETLANDEVENT_REMOVE* pNetMsg = ( GLMSG::SNETLANDEVENT_REMOVE* ) nmg;

            EVENTNODECLIENTMAP_ITER iter = m_cEventInfoMap.find( pNetMsg->dwGaeaID );

            if ( iter == m_cEventInfoMap.end() )
            {
                break;
            }

            CEVENT_NODE_CLIENT* pEvent = iter->second; 

            if ( pEvent )
            {
                pEvent->Clean();
                pEvent->Restore();
            }

            m_cEventInfoMap.erase(iter);
        }
        break;

	case NET_MSG_GCTRL_LANDEVENT_SUMMONGATE_CREATE:
		{
			GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE* pNetMsg = ( GLMSG::SNETLANDEVENT_SUMMONGATE_CREATE* ) nmg;
			
			PLANDMANCLIENT pLandMan   = m_pGaeaClient->GetActiveMap();
			GLCharacter*   pCharacter = m_pGaeaClient->GetCharacter();

			if ( pLandMan && pCharacter )
			{
				DWORD dwGUIDLandEvent = pNetMsg->dwGaeaID; // 랜드 이벤트 GUID

				CEVENT_NODE_CLIENT_SUMMONGATE* pEventNode = pLandMan->DropLandEvent<CEVENT_NODE_CLIENT_SUMMONGATE>(dwGUIDLandEvent, pNetMsg->vPos, pNetMsg->fTimeEnd, pNetMsg->fTimeEvent);

				pEventNode->m_sSkillID      = pNetMsg->skill_id;
				pEventNode->m_wLevel        = pNetMsg->skill_lev;

				pEventNode->m_dwGUIDOwner   = pNetMsg->dwOwnerID;

				pEventNode->m_sEvent        = pNetMsg->sEvent;
				pEventNode->m_sTarget       = pNetMsg->sTarget;

				PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pEventNode->m_sSkillID.wMainID, pEventNode->m_sSkillID.wSubID );

				if ( pSkill )
				{
                    if ( ( pLandMan->IsCullByObjectMap() == true )
                        && ( m_pGaeaClient->IsCollision(pNetMsg->vPos, pCharacter->GetPosition(), EMCC_CULL_NONE_ONLY_WALL) ) )
                    {
                        //캐릭터 시야에 의해 컬링이 되었다면 이펙트는 등록하지 않음
                        pEventNode->m_strEffectName.clear();
                    }
                    else
                    {
                        pEventNode->m_strEffectName = pSkill->m_sSPECIAL_SKILL.strEffectName;
                    }
				}
			}
		}
		break;
	}

}

//-------------------------------------------------------------------//

GLLandEventClient::GLLandEventClient(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
	//Blank
}

GLLandEventClient::~GLLandEventClient()
{
	CleanUp();
}

void GLLandEventClient::CleanUp()
{	
	EVENTNODECLIENTMAP_ITER iter = m_cEventInfoMap.begin();

	for ( ; iter != m_cEventInfoMap.end(); )
	{
		CEVENT_NODE_CLIENT* pEvent = iter->second;

		if ( pEvent )
		{
			pEvent->Clean();

			delete (pEvent);
		}

		iter = m_cEventInfoMap.erase(iter);
	}
}

void GLLandEventClient::FrameMove( const FLOAT fElapsedTime )
{
	EVENTNODECLIENTMAP_ITER iter = m_cEventInfoMap.begin();

	for ( ; iter != m_cEventInfoMap.end(); )
	{
		CEVENT_NODE_CLIENT* pEvent = iter->second; 

		if ( !pEvent )
		{
			iter = m_cEventInfoMap.erase(iter);

			continue;
		}

		pEvent->FrameMove( fElapsedTime );

		if ( pEvent->IsEnd() )
		{
			pEvent->Clean();
			pEvent->Restore();

			iter = m_cEventInfoMap.erase(iter);

			continue;
		}

		++iter;
	}
}
